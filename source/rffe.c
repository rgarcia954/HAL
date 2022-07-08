/**
 * @file @file rffe.c
 * @brief Radio Frequency Front End support functions
 *
 * @copyright @parblock
 * Copyright (c) 2021 Semiconductor Components Industries, LLC (d/b/a
 * onsemi), All Rights Reserved
 *
 * This code is the property of onsemi and may not be redistributed
 * in any form without prior written permission from onsemi.
 * The terms of use and warranty for this code are covered by contractual
 * agreements between onsemi and the licensee.
 *
 * This is Reusable Code.
 * @endparblock
 */

#include <rffe.h>

/* Static function prototypes */

#ifndef NON_SECURE
static uint32_t Sys_RFFE_SetTXPower_NoVDDPA(int8_t target, uint32_t lsad_channel);

static uint32_t Sys_RFFE_SetTXPower_VDDPA(int8_t target, uint32_t lsad_channel);

#endif    /* ifndef NON_SECURE */

static uint32_t Sys_RFFE_MeasureSupply(const volatile uint32_t *adc_ptr);

static uint32_t Sys_RFFE_GetMedian(uint32_t a, uint32_t b, uint32_t c);

void Device_RF_SetMaxPwrIdx(uint8_t pa_pwr_temp) __attribute__((weak));

void Device_RF_SetMaxPwrIdx(uint8_t pa_pwr_temp)
{
	/* empty weak function declaration */
}

uint32_t Sys_RFFE_SetTXPower(int8_t target, uint8_t lsad_channel, bool pa_en)
{
#ifndef NON_SECURE
    uint32_t rffe_error = ERROR_NO_ERROR;

    /* Check lsad_channel selected is correct */
    if ((lsad_channel > MAX_LSAD_CHANNEL) || (rffe_error == ERRNO_RFFE_INVALIDSETTING_ERROR))
    {
        rffe_error = ERRNO_RFFE_INVALIDSETTING_ERROR;
    }
    else
    {
        /* Check if we have a valid target */
        if ((target > RF_MAX_POWER) || (target < RF_MIN_POWER))
        {
            /* Invalid target input */
            rffe_error = ERRNO_RFFE_INVALIDSETTING_ERROR;
        }
        else if ((target <= RF_MAX_POWER) && (target >= RF_MIN_POWER))
        {
            /* We have a valid target power setting */
            if ((!pa_en && (target <= RF_MAX_POWER_NO_VDDPA) && (target > 0)) || (target <= 0))
            {
                /* We have a target that will not require VDDPA. */
                rffe_error = Sys_RFFE_SetTXPower_NoVDDPA(target, lsad_channel);

                /* VCC was too low, enable VDDPA to achieve 2dBm. */
                if (rffe_error == ERRNO_RFFE_VCC_INSUFFICIENT)
                {
                    rffe_error |= Sys_RFFE_SetTXPower_VDDPA(target, lsad_channel);
                }
            }
            else
            {
                /* We have a target that will require VDDPA. */
                rffe_error = Sys_RFFE_SetTXPower_VDDPA(target, lsad_channel);
            }
        }
        else
        {
            rffe_error = ERRNO_RFFE_INVALIDSETTING_ERROR;
        }
    }
    return rffe_error;
#else    /* ifndef NON_SECURE */
    return ERRNO_RFFE_INVALIDSETTING_ERROR;
#endif    /* ifndef NON_SECURE */
}

int8_t Sys_RFFE_GetTXPower(uint32_t lsad_channel)
{
#ifndef NON_SECURE
    uint32_t aout_bk = 0;
    uint32_t lsad_bk = 0;
    uint32_t lsad_cfg_bk = 0;
    uint32_t vddrf = 0;
    int32_t vddpa = 0;
    float currentTXPower = -100.0f;    /* Set default reading to an invalid value */
    TRIM_Type *trims = TRIM;
    float temp = 0.0f;
    uint16_t trim_temp;									   

    if ((SYSCTRL->VDDPA_CFG0 & DYNAMIC_CTRL_ENABLE_BYTE) ||
        ((ACS->VDDPA_CTRL & VDDPA_ENABLE) && (!(ACS->VDDPA_CTRL & VDDPA_SW_VDDRF))))
    {
        /* VDDPA is enabled, calculate output power accordingly. */
        /* Cannot measure VDDPA accurately, since dynamic VDDPA is enabled*/
        /* Use trims to determine approximate VDDPA voltage */
        /* 10mV per trim setting */
        trim_temp = trims->vddpa[2].trim_voltage;
        vddpa = (TARGET_VDDPA_1600 * 10) +
                ((ACS_VDDPA_CTRL->VTRIM_BYTE - trim_temp) * 10);

        /* Calculate current set power, using VDDPA measurement and current
         * register setting. */
        currentTXPower = (RF_MAX_POWER -
                          (((PA_PWR_BYTE_0DBM - RF0_REG1A->PA_PWR_PA_PWR_BYTE) * 3.0f) / 2.0f));
        temp = ((float)(vddpa) - (TARGET_VDDPA_1600 * 10.0f));
        currentTXPower += temp / (float)(MV_PER_DBM_VDDPA);
    }
    else
    {
        /* Save LSAD and AOUT states */
        lsad_bk = LSAD->INPUT_SEL[lsad_channel];
        aout_bk = ACS->AOUT_CTRL;
        lsad_cfg_bk = LSAD->CFG;

        /* Configure LSAD to measure AOUT */
#if (RSL15_CID == 202)
        LSAD->CFG = LSAD_NORMAL | LSAD_PRESCALE_200;
#else  /* if (RSL15_CID == 202) */
        LSAD->CFG = LSAD_NORMAL | LSAD_PRESCALE_200 | VBAT_DIV2_ENABLE;
#endif /* if (RSL15_CID == 202) */
        LSAD->INPUT_SEL[lsad_channel] = LSAD_POS_INPUT_AOUT | LSAD_NEG_INPUT_GND;

        /* Configure AOUT for internal measurement */
        ACS->AOUT_CTRL &= ~(ACS_AOUT_CTRL_TEST_AOUT_Mask | ACS_AOUT_CTRL_AOUT_TO_GPIO_Mask);
        ACS->AOUT_CTRL |= AOUT_NOT_CONNECTED_TO_GPIO | SEL_AOUT_TO_GPIO;

        /* VDDPA is disabled, calculate output power accordingly. */
        /* Measure VDDRF */
        ACS->AOUT_CTRL |= AOUT_VDDRF;
        vddrf = Sys_RFFE_MeasureSupply(&(LSAD->DATA_TRIM_CH[lsad_channel]));

        /* Calculate current set power, using VDDRF measurement and current
         * register setting. */
        currentTXPower = (RF_NO_VDDPA_TYPICAL_POWER -
                          (((PA_PWR_BYTE_0DBM - RF0_REG1A->PA_PWR_PA_PWR_BYTE) * 3.0f) / 2.0f));
        temp = ((float)(vddrf) - (TARGET_VDDRF_1070 * 10.0f));
        currentTXPower += temp / (float)(MV_PER_DBM_VDDRF);

        /* Restore ACS_AOUT_CTRL, LSAD_CFG and LSAD_INPUT_SEL* registers. */
        LSAD->CFG = lsad_cfg_bk;
        ACS->AOUT_CTRL = aout_bk;
        LSAD->INPUT_SEL[lsad_channel] = lsad_bk;
    }

    /* Round value */
    if (currentTXPower > 0)
    {
        currentTXPower += 0.5f;
    }
    else if (currentTXPower < 0)
    {
        currentTXPower -= 0.5f;
    }

    return (int8_t)(currentTXPower);
#else    /* ifndef NON_SECURE */
    return ERRNO_RFFE_INVALIDSETTING_ERROR;
#endif    /* ifndef NON_SECURE */
}

/**
 * @brief       Set the TX Power to the level indicated, while disabling
 *              the power amplifier. VDDRF will be increased beyond default
 *              voltage for output power settings higher than 0dBm.
 * @param[in]   target        Target transmission power in the range
 *                            from (-17dBM) - (2dBm).
 * @param[in]   lsad_channel  LSAD channel to use to measure rails,
 *                            if necessary.
 * @return      return value  error value, if any
 * */

#ifndef NON_SECURE
static uint32_t Sys_RFFE_SetTXPower_NoVDDPA(int8_t target, uint32_t lsad_channel)
{
    uint32_t aout_bk = 0;
    uint32_t lsad_bk = 0;
    uint32_t lsad_cfg_bk = 0;
    uint32_t vcc = 0;
    uint32_t vddrf_max = 0;
    TRIM_Type *trims = TRIM;
    uint32_t error = ERROR_NO_ERROR;
    float power_error = 0.0f;
    float upper_trim = 0.0f;
    float lower_trim = 0.0f;

    uint8_t pa_pwr_temp = 0xff;

    /* Save LSAD and AOUT states */
    lsad_bk = LSAD->INPUT_SEL[lsad_channel];
    aout_bk = ACS->AOUT_CTRL;

    lsad_cfg_bk = LSAD->CFG;
#if (RSL15_CID == 202)
    LSAD->CFG = LSAD_NORMAL | LSAD_PRESCALE_200;
#else  /* if (RSL15_CID == 202) */
    LSAD->CFG = LSAD_NORMAL | LSAD_PRESCALE_200 | VBAT_DIV2_ENABLE;
#endif /* if (RSL15_CID == 202) */

    /* Configure LSAD to measure AOUT */
    LSAD->INPUT_SEL[lsad_channel] = LSAD_POS_INPUT_AOUT | LSAD_NEG_INPUT_GND;

    /* Configure AOUT for internal measurement */
    ACS->AOUT_CTRL &= ~(ACS_AOUT_CTRL_TEST_AOUT_Mask | ACS_AOUT_CTRL_AOUT_TO_GPIO_Mask);
    ACS->AOUT_CTRL |= AOUT_NOT_CONNECTED_TO_GPIO | SEL_AOUT_TO_GPIO;

    /* Set power level */
    if (target > RF_NO_VDDPA_TYPICAL_POWER)
    {
        /* We are not using the power amplifier,
         * but need more than 0dBm output power.
         * Measure VCC */
        ACS->AOUT_CTRL |= AOUT_VCC;

        /* Measure VCC */
        vcc = Sys_RFFE_MeasureSupply(&(LSAD->DATA_TRIM_CH[lsad_channel]));

        /* Calculate the maximum vddrf voltage available */
        vddrf_max = vcc - VCC_VDDRF_MARGIN;

        if (vddrf_max < ((TARGET_VDDRF_1070 * 10) + (target * MV_PER_DBM_VDDRF)))
        {
            /* If VCC is too low to support the required VDDRF, return an error.
             * Do not change any settings. */
            error = ERRNO_RFFE_VCC_INSUFFICIENT;
        }
        else
        {
            /* Disable dynamic VDDPA, do not disable if in error state. */
            SYSCTRL_VDDPA_CFG0->DYNAMIC_CTRL_BYTE = DYNAMIC_CTRL_DISABLE_BYTE;
            ACS->VDDPA_CTRL = VDDPA_INITIAL_TRIM_1P10V |
                              VDDPA_SW_VDDRF |
                              VDDPA_ISENSE_DISABLE |
                              VDDPA_DISABLE |
                              (trims->vddpa[2].trim_voltage << ACS_VDDPA_CTRL_VTRIM_Pos);

            /* We have sufficient VCC, raise VDDRF according to the target power. */
            ACS_VDDRF_CTRL->VTRIM_BYTE = trims->vddrf[1].trim + (target * STEPS_PER_DBM_VDDRF);

            /* Set TX power to typical value for 0dBm output at typical VDDRF. */
            pa_pwr_temp = PA_PWR_BYTE_0DBM;

            /* Disable the power amplifier */
            RF0_BIAS_0_2->BIAS_0_IQ_RXTX_BYTE = PA_DISABLE_BIAS_SETTING;
        }
    }
    else
    {
        /* Disable dynamic VDDPA */
        SYSCTRL_VDDPA_CFG0->DYNAMIC_CTRL_BYTE = DYNAMIC_CTRL_DISABLE_BYTE;
        ACS->VDDPA_CTRL = VDDPA_INITIAL_TRIM_1P10V |
                          VDDPA_SW_VDDRF |
                          VDDPA_ISENSE_DISABLE |
                          VDDPA_DISABLE |
                          (trims->vddpa[2].trim_voltage << ACS_VDDPA_CTRL_VTRIM_Pos);

        /* Load nominal VDDRF value */
        ACS_VDDRF_CTRL->VTRIM_BYTE = trims->vddrf[1].trim;

        /* We have a target below 0dBm, VDDRF may need compensation.*/
        /* Decrease PA_PWR by 1 for every 1.5dBm requested */
        /* Nearest 1.5dBm setting should be found */
        upper_trim = (target + 0.5f) * 2;
        lower_trim = (target - 0.5f) * 2;

        if (!((int32_t)(upper_trim) % 3))
        {
            /* A multiplication of 2/3 has been applied. The remainder is zero when
             * the target value was increased by 0.5. This means this pa_pwr setting
             * is within 0.5dBm of the target value. We will use this pa_pwr setting. */
            pa_pwr_temp = PA_PWR_BYTE_0DBM + (uint8_t)(upper_trim / 3);

            /* Calculate error */
            power_error = (float)(target) - ((float)(pa_pwr_temp - PA_PWR_BYTE_0DBM) * (1.5f));

            /* Adjust VDDRF to compensate for error */
            ACS_VDDRF_CTRL->VTRIM_BYTE += (power_error * STEPS_PER_DBM_VDDRF);
        }
        else if (!((int32_t)(lower_trim) % 3))
        {
            /* A multiplication of 2/3 has been applied. The remainder is zero when
             * the target value was decreased by 0.5. This means this pa_pwr setting
             * is within 0.5dBm of the target value. We will use this pa_pwr setting. */
            pa_pwr_temp = PA_PWR_BYTE_0DBM + (int)(lower_trim / 3);

            /* Calculate error */
            power_error = (float)(target) - ((float)(pa_pwr_temp - PA_PWR_BYTE_0DBM) * (1.5f));

            /* Adjust VDDRF to compensate for error */
            ACS_VDDRF_CTRL->VTRIM_BYTE += (power_error * STEPS_PER_DBM_VDDRF);
        }
        else
        {
            /* Target divides by 1.5 evenly. */
            pa_pwr_temp = PA_PWR_BYTE_0DBM + (((target) * 2) / 3);
        }

        /* Disable the power amplifier */
        RF0_BIAS_0_2->BIAS_0_IQ_RXTX_BYTE = PA_DISABLE_BIAS_SETTING;
    }

    /* Restore ACS_AOUT_CTRL, LSAD_CFG and LSAD_INPUT_SEL* registers. */
    LSAD->CFG = lsad_cfg_bk;
    ACS->AOUT_CTRL = aout_bk;
    LSAD->INPUT_SEL[lsad_channel] = lsad_bk;

    if(pa_pwr_temp != 0xff)
    {
    	RF0_REG1A->PA_PWR_PA_PWR_BYTE = pa_pwr_temp;
    	Device_RF_SetMaxPwrIdx(pa_pwr_temp);
    }

    return error;
}

#endif    /* ifndef NON_SECURE */

/**
 * @brief       Set the TX Power to the level indicated, turning on the
 *              power amplifier.
 * @param[in]   target        Target transmission power in the range
 *                            from (-11dBm) - (6dBm)
 * @param[in]   lsad_channel  LSAD channel to use to measure rails,
 *                            if necessary.
 * @assumptions VDDRF is set to 1.1V.
 * @return      return value  error value, if any
 * */

#ifndef NON_SECURE
static uint32_t Sys_RFFE_SetTXPower_VDDPA(int8_t target, uint32_t lsad_channel)
{
    uint32_t error = ERROR_NO_ERROR;
    TRIM_Type *trims = TRIM;
    float power_error = 0.0f;

    uint8_t pa_pwr_temp = 0xff;

    /* Power Amplifier is disabled (will use dynamic mode) */
    /* Assume 1600mV = 6dBM output power */
    ACS->VDDPA_CTRL = VDDPA_INITIAL_TRIM_1P10V |
                      VDDPA_SW_HIZ |
                      VDDPA_ISENSE_DISABLE |
                      VDDPA_DISABLE |
                      (trims->vddpa[2].trim_voltage << ACS_VDDPA_CTRL_VTRIM_Pos);

    /* Enable dynamic VDDPA */
    SYSCTRL_VDDPA_CFG0->DYNAMIC_CTRL_BYTE = DYNAMIC_CTRL_ENABLE_BYTE;
    SYSCTRL_VDDPA_CFG0->SW_CTRL_DELAY_BYTE = SW_CTRL_DELAY_3_BYTE;
    SYSCTRL_VDDPA_CFG0->RAMPUP_DELAY_BYTE = RAMPUP_DELAY_3_BYTE;
    SYSCTRL_VDDPA_CFG0->DISABLE_DELAY_BYTE = DISABLE_DELAY_3_BYTE;

    /* Change VDDPA bias */
    RF0_BIAS_0_2->BIAS_0_IQ_RXTX_BYTE = PA_ENABLE_BIAS_SETTING;

    /* We have a target below 6dBm, VDDPA should be fine as is.*/
    /* Decrease PA_PWR by 1 for every 1.5dBm requested */
    pa_pwr_temp = PA_PWR_BYTE_0DBM + (((target - RF_MAX_POWER) * 2) / 3);

    if (target % 3)
    {
        /* There will be a power error when the target is not a multiple of 3.*/
        power_error = (float)(target - RF_MAX_POWER) -
                      ((float)(pa_pwr_temp - PA_PWR_BYTE_0DBM) * (1.5f));

        /* Increase VDDPA to compensate. Each trim step is 10mV. */
        ACS_VDDPA_CTRL->VTRIM_BYTE += (power_error * STEPS_PER_DBM_VDDPA);
    }

    if(pa_pwr_temp != 0xff)
    {
    	RF0_REG1A->PA_PWR_PA_PWR_BYTE = pa_pwr_temp;
    	Device_RF_SetMaxPwrIdx(pa_pwr_temp);
    }

    return error;
}

#endif    /* ifndef NON_SECURE */
#ifndef NON_SECURE
static uint32_t Sys_RFFE_MeasureSupply(const volatile uint32_t *adc_ptr)
#else    /* ifndef NON_SECURE */
__attribute__ ((unused)) static uint32_t Sys_RFFE_MeasureSupply(const volatile uint32_t * adc_ptr)
#endif    /* ifndef NON_SECURE */
{
    uint32_t supply1 = 0;
    uint32_t supply2 = 0;
    uint32_t supply3 = 0;

    TRIM_Type *trim = TRIM;

    float median_supply = 0;
    float offset_error = (float)((int16_t)trim->lsad_trim.lf_offset) / LSAD_OFFSET_ERROR_CONV_QUOTIENT;
    float gain_error = (float)(trim->lsad_trim.lf_gain) / LSAD_GAIN_ERROR_CONV_QUOTIENT;

    /* Define a short stabilization delay to ensure that we
     * allow the ADC measured parameter to stabilize. */
    Sys_Delay(STABILIZATION_DELAY);
    supply1 = *adc_ptr;

    Sys_Delay(MEASUREMENT_DELAY);
    supply2 = *adc_ptr;

    Sys_Delay(MEASUREMENT_DELAY);
    supply3 = *adc_ptr;

    /* Return median of the 3 measurements */
    median_supply = ((float)CONVERT(Sys_RFFE_GetMedian(supply1, supply2, supply3))) / V_TO_MV_F;

    /* Multiply by 1000 to convert back to mV */
    return (uint32_t)((median_supply - offset_error) / gain_error * V_TO_MV);
}

static uint32_t Sys_RFFE_GetMedian(uint32_t a, uint32_t b, uint32_t c)
{
    uint32_t temp = 0;

    /* Put smallest in a, median in b, and max in c */
    if (a > b)
    {
        /* Swap a and b */
        SWAP(a, b, temp);
    }

    if (b > c)
    {
        /* Swap b and c */
        SWAP(b, c, temp);

        if (a > b)
        {
            /* Swap a and b */
            SWAP(a, b, temp);
        }
    }

    /* b always contains the median value */
    return (b);
}
