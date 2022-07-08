/**
 * @file trim.c
 * @brief Hardware abstraction layer for loading trim values from NVR.
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
 * @endparb
 * */

#include <hw.h>
#include <trim.h>

/* Private function definitions */

static uint32_t Sys_Trim_LoadSingleTrim_private(TRIM_Type *trim_region,
                                                uint32_t target_name,
                                                uint32_t target_value1,
                                                uint32_t target_value2);

static uint32_t Sys_Trim_LoadBandgap_private(TRIM_Type *trim_values,
                                             uint32_t target_v,
                                             uint32_t target_i);

static uint32_t Sys_Trim_LoadDCDC_private(TRIM_Type *trim_values, uint32_t target);

static uint32_t Sys_Trim_LoadVDDC_private(TRIM_Type *trim_values, uint32_t target,
                                          uint32_t target_standby);

static uint32_t Sys_Trim_LoadVDDM_private(TRIM_Type *trim_values, uint32_t target,
                                          uint32_t target_standby);

static uint32_t Sys_Trim_LoadVDDPA_private(TRIM_Type *trim_values, uint32_t target);

static uint32_t Sys_Trim_LoadVDDRF_private(TRIM_Type *trim_values, uint32_t target);

static uint32_t Sys_Trim_LoadVDDFLASH_private(TRIM_Type *trim_values, uint32_t target);

static uint32_t Sys_Trim_LoadRCOSC_private(TRIM_Type *trim_values, uint32_t target);

static uint32_t Sys_Trim_LoadRCOSC32_private(TRIM_Type *trim_values, uint32_t target);

static uint32_t Sys_Trim_LoadThermistor_private(TRIM_Type *trim_values, uint16_t target);

/* List of trim value loading functions */
static uint32_t(*load_trim_functions_2_args[TRIM_NUM_FUNCTIONS_2_ARGS])(TRIM_Type *, uint32_t, uint32_t) =
{
    Sys_Trim_LoadBandgap_private,
    Sys_Trim_LoadVDDC_private,
    Sys_Trim_LoadVDDM_private,
};

/* List of trim value loading functions */
static uint32_t(*load_trim_functions_1_arg[TRIM_NUM_FUNCTIONS_1_ARG])(TRIM_Type *, uint32_t) =
{
    Sys_Trim_LoadDCDC_private,
    Sys_Trim_LoadVDDRF_private,
    Sys_Trim_LoadVDDPA_private,
    Sys_Trim_LoadVDDIF,
    Sys_Trim_LoadVDDFLASH_private,
    Sys_Trim_LoadRCOSC_private,
    Sys_Trim_LoadRCOSC32_private,
};

uint32_t trim_args2[TRIM_NUM_FUNCTIONS_2_ARGS][2] =
{
    { TARGET_BANDGAP_V, TARGET_BANDGAP_I },
    { TARGET_VDDC_1150, TARGET_VDDC_STANDBY },
    { TARGET_VDDM_1150, TARGET_VDDM_STANDBY },
};

uint32_t trim_args1[TRIM_NUM_FUNCTIONS_1_ARG] =
{
    TARGET_DCDC_1200,
    TARGET_VDDRF_1100,
    TARGET_VDDPA_1600,
    TARGET_VDDIF_1800,
    TARGET_FLASH_1600,
    TARGET_RC3,
    TARGET_RC32K
};

uint32_t Sys_Trim_LoadTrims(TRIM_Type *trim_region,
		uint32_t targets_1[TRIM_NUM_FUNCTIONS_1_ARG],
		uint32_t targets_2[TRIM_NUM_FUNCTIONS_1_ARG][2])
{
    uint32_t i;
    uint32_t ret_val = ERROR_NO_ERROR;

    /* Check for null pointers from input */
    if (trim_region == NULL_POINTER)
    {
        return ERROR_NULL;
    }

    ret_val = Sys_Trim_VerifyTrims(trim_region);

    /* Load trim values */
    for (i = 0; i < TRIM_NUM_FUNCTIONS_2_ARGS; i++)
    {
        /* Call trim loading function */
        ret_val |= load_trim_functions_2_args[i] (trim_region, targets_2[i][0], targets_2[i][1]);
    }
    for (i = 0; i < TRIM_NUM_FUNCTIONS_1_ARG; i++)
    {
        /* Call trim loading function */
        ret_val |= load_trim_functions_1_arg[i] (trim_region, targets_1[i]);
    }
    return ret_val;
}

uint32_t Sys_Trim_LoadSingleTrim(uint32_t target_name, uint32_t target_value1, uint32_t target_value2)
{
    uint32_t ret_val = ERROR_NO_ERROR;

    ret_val = Sys_Trim_LoadSingleTrim_private(TRIM_SUPPLEMENTAL, target_name, target_value1, target_value2);

    if (ret_val != ERROR_NO_ERROR)
    {
        ret_val = Sys_Trim_LoadSingleTrim_private(TRIM, target_name, target_value1, target_value2);
    }

    return ret_val;
}

static uint32_t Sys_Trim_LoadSingleTrim_private(TRIM_Type *trim_region,
                                                uint32_t target_name,
                                                uint32_t target_value1,
                                                uint32_t target_value2)
{
    uint32_t ret_val = 0;
    switch (target_name)
    {
        case TRIM_BANDGAP:
        {
            ret_val = Sys_Trim_LoadBandgap(trim_region, target_value1, target_value2);
        }
        break;

        case TRIM_DCDC:
        {
            ret_val = Sys_Trim_LoadDCDC(trim_region, target_value1);
        }
        break;

        case TRIM_VDDC:
        {
            ret_val = Sys_Trim_LoadVDDC(trim_region, target_value1, target_value2);
        }
        break;

        case TRIM_VDDM:
        {
            ret_val = Sys_Trim_LoadVDDM(trim_region, target_value1, target_value2);
        }
        break;

        case TRIM_VDDRF:
        {
            ret_val = Sys_Trim_LoadVDDRF(trim_region, target_value1);
        }
        break;

        case TRIM_VDDPA:
        {
            ret_val = Sys_Trim_LoadVDDPA(trim_region, target_value1);
        }
        break;

        case TRIM_FLASH:
        {
            ret_val = Sys_Trim_LoadVDDFLASH(trim_region, target_value1);
        }
        break;

        case TRIM_RCOSC:
        {
            ret_val = Sys_Trim_LoadRCOSC(trim_region, target_value1);
        }
        break;

        case TRIM_RCOSC32:
        {
            ret_val = Sys_Trim_LoadRCOSC32(trim_region, target_value1);
        }
        break;

        default:
        {
        }
        break;
    }
    return ret_val;
}

uint32_t Sys_Trim_VerifyTrims(TRIM_Type *trim_region)
{
    /* Multiple return paths in order to reduce number of
     * if statements used.*/
    uint32_t i;
    uint32_t ret_val = ERROR_NO_ERROR;
    bool valid_found[6] = {0};

    /* Check for null pointers from input */
    if (trim_region == NULL_POINTER)
    {
        return ERROR_NULL;
    }
    else
    {
        /* Before checking trim values, first run CRC. */

        if (Sys_Trim_CheckCRC(trim_region) != ERROR_NO_ERROR)
        {
            ret_val |= ERROR_INVALID_CRC;
        }

        /* Check voltage trims */
        for (i = 0; i < 4; i++)
        {
        	if (!valid_found[TRIM_BANDGAP])
        	{
				if ((trim_region->bandgap[i].target == MIN_16_BIT)
					|| (trim_region->bandgap[i].target == MAX_16_BIT))
				{
					/* No valid trim found */
					ret_val |= ERROR_BG_INVALID;
				}
				else
				{
					valid_found[TRIM_BANDGAP] = true;
					ret_val &= ~ERROR_BG_INVALID;
				}
        	}

            /* Check DCDC (LDO) voltage trim */
        	if (!valid_found[TRIM_DCDC])
        	{
				if ((trim_region->dcdc[i].target == MIN_16_BIT)
					|| (trim_region->dcdc[i].target == MAX_16_BIT))
				{
					ret_val |= ERROR_DCDC_INVALID;
				}
				else
				{
					valid_found[TRIM_DCDC] = true;
					ret_val &= ~ERROR_DCDC_INVALID;
				}
        	}

            /* Check VDDC voltage trim */
        	if (!valid_found[TRIM_VDDC])
        	{
				if ((trim_region->vddc[i].target_voltage == MIN_8_BIT)
					|| (trim_region->vddc[i].target_voltage == MAX_8_BIT))
				{
					ret_val |= ERROR_VDDC_INVALID;
				}
				else
				{
					valid_found[TRIM_VDDC] = true;
					ret_val &= ~ERROR_VDDC_INVALID;
				}
        	}

            /* Check VDDM voltage trim */
        	if (!valid_found[TRIM_VDDM])
        	{
				if ((trim_region->vddm[i].target_voltage == MIN_8_BIT)
					|| (trim_region->vddm[i].target_voltage == MAX_8_BIT))
				{
					ret_val |= ERROR_VDDM_INVALID;
				}
				else
				{
					valid_found[TRIM_VDDM] = true;
					ret_val &= ~ERROR_VDDM_INVALID;
				}
        	}

            /* Check VDDRF voltage trim */
        	if (!valid_found[TRIM_VDDRF])
        	{
				if ((trim_region->vddrf[i].trim == MIN_16_BIT)
					|| (trim_region->vddrf[i].trim == MAX_16_BIT))
				{
					ret_val |= ERROR_VDDRF_INVALID;
				}
				else
				{
					valid_found[TRIM_VDDRF] = true;
					ret_val &= ~ERROR_VDDRF_INVALID;
				}
        	}

            /* Check VDDPA voltage trim */
        	if (!valid_found[TRIM_VDDPA])
        	{
				if ((trim_region->vddpa[i].target_voltage == MIN_8_BIT)
					|| (trim_region->vddpa[i].target_voltage == MAX_8_BIT))
				{
					ret_val |= ERROR_VDDPA_INVALID;
				}
				else
				{
					valid_found[TRIM_VDDPA] = true;
					ret_val &= ~ERROR_VDDPA_INVALID;
				}
        	}
        }

        /** Check VDDIF voltage trim */
        if ((trim_region->vddif[0].target == MIN_16_BIT)
            || (trim_region->vddif[0].target == MAX_16_BIT))
        {
            ret_val |= ERROR_VDDIF_INVALID;
        }

        /** Check VDDFLASH voltage trim */
        if ((trim_region->vddflash[0].target == MIN_16_BIT)
            || (trim_region->vddflash[0].target == MAX_16_BIT))
        {
            ret_val |= ERROR_VDDFLASH_INVALID;
        }

        /** Check RC oscillator trim */
        for (i = 0; i < TRIM_RC_RECORDS; i++)
        {
            if ((trim_region->rcosc[i * 2].target == MIN_16_BIT)
                || (trim_region->rcosc[i * 2].target == MAX_16_BIT))
            {
                ret_val |= ERROR_RCOSC_INVALID;
            }
        }

        /** Check RC 32kHz oscillator trim */
        if ((trim_region->rcosc32[0].target == MIN_16_BIT)
            || (trim_region->rcosc32[0].target == MAX_16_BIT))
        {
            ret_val |= ERROR_RCOSC32_INVALID;
        }

        /** Check LSAD offset trim */
        if ((trim_region->lsad_trim.hf_offset == MIN_16_BIT)
            || (trim_region->lsad_trim.hf_offset == MAX_16_BIT))
        {
            ret_val |= ERROR_LSAD_INVALID;
        }

        /** Check LSAD gain trim */
        if ((trim_region->lsad_trim.hf_gain == MIN_32_BIT)
            || (trim_region->lsad_trim.lf_gain == MAX_32_BIT))
        {
            ret_val |= ERROR_LSAD_INVALID;
        }

        /** Check temperature sensor trim */
        if ((trim_region->temp_sensor.offset == MIN_32_BIT)
            || (trim_region->temp_sensor.offset == MAX_32_BIT))
        {
            ret_val |= ERROR_TEMPERATURE_INVALID;
        }

        /** Check thermistor trim */
        if ((trim_region->thermistor[0].bias == MIN_16_BIT)
            || (trim_region->thermistor[0].bias == MAX_16_BIT))
        {
            ret_val |= ERROR_THERMISTOR_INVALID;
        }

        /** Check measured trims */
        if ((trim_region->measured.temp_sensor_30C == MIN_16_BIT)
            || (trim_region->measured.temp_sensor_30C == MAX_16_BIT)
            || (trim_region->measured.bandgap_vref_0_75V == MIN_16_BIT)
            || (trim_region->measured.bandgap_vref_0_75V == MAX_16_BIT)
            || (trim_region->measured.lsad_vref_1_0V_internal == MIN_16_BIT)
            || (trim_region->measured.lsad_vref_1_0V_internal == MAX_16_BIT)
            || (trim_region->measured.wedac_600mV == MIN_16_BIT)
            || (trim_region->measured.wedac_600mV == MAX_16_BIT))
        {
            ret_val |= ERROR_MEASURED_INVALID;
        }
    }

    /** Return no error if this point is reached and CRC is not calculated. */
    return ret_val;
}

uint32_t Sys_Trim_CheckCRC(TRIM_Type *trim_region)
{
    uint32_t *i;
    uint32_t *top_addr;
    uint32_t *bottom_addr;
    uint32_t ret_val;

    top_addr = (uint32_t *)trim_region;
    bottom_addr = (uint32_t *)&(trim_region->SOS_REV);

    /* Check for null pointers from input */
    if (trim_region == 0)
    {
        ret_val = ERROR_NULL;
    }

    /* Configure & Initialize CRC */

    /* BEGIN: Work-around for early chips */

    if (trim_region->CHECKSUM <= MAX_16_BIT)
    {
        SYS_CRC_CONFIG(CRC_CCITT);
        SYS_CRC_CCITTINITVALUE();
    }
    else
    {
        SYS_CRC_CONFIG(CRC_32);
        SYS_CRC_32INITVALUE();
    }

    /* END: Work-around for early chips */

    /* Add values to CRC*/
    for (i = top_addr; i <= bottom_addr; i++)
    {
        SYS_CRC_ADD(*i, 32);
    }

    /* Check if final CRC value equals the provided CRC value. */
    if ((trim_region->CHECKSUM) == SYS_CRC_GETFINALVALUE())
    {
        /* CRC value matches, return no error */
        ret_val =  ERROR_NO_ERROR;
    }
    else
    {
        /* CRC value mismatch, return invalid CRC error. */
        ret_val =  ERROR_INVALID_CRC;
    }
    return ret_val;
}

uint32_t Sys_Trim_GetTrim(uint32_t *addr,
                          uint16_t trim_target,
                          uint32_t record_length,
                          uint16_t *trim_val)
{
    uint32_t i;
    uint16_t stored_target;
    uint16_t stored_trim;
    uint32_t ret_val;

    /* Check for null pointers from input */
    if ((addr == NULL_POINTER) || (trim_val == NULL_POINTER))
    {
        return ERROR_NULL;
    }

    for (i = 0; i < record_length; i++)
    {
        /* Reset back to no error if no trim found yet. Last iteration of loop will
         * determine if a true error has resulted. */
        ret_val = ERROR_NO_ERROR;

        /* Check if valid trim values might exist in specified location. */
        if (addr[i] == MIN_32_BIT || addr[i] == MAX_32_BIT)
        {
            ret_val = ERROR_INVALID_TRIM;
        }

        /* Grab stored data. */
        stored_target = (addr[i] >> 16) & TRIM_16_BIT_TRIM_MASK;
        stored_trim = (addr[i]) & TRIM_16_BIT_TRIM_MASK;

        /* Check to see what type of trim we are looking for. */
        if ((stored_target > MAX_8_BIT) && (ret_val == ERROR_NO_ERROR))
        {
            /* We have a 16-bit trim target, therefore it must be:
             * VDDC, VDDM, VDDPA, RCOSC, RCOSC32 */
            if (trim_target == stored_target)
            {
                /* Requested 16-bit trim setting.
                 * RCOSC setting. Found the desired setting. */
                *trim_val = stored_trim;
                ret_val = ERROR_NO_ERROR;
                break;
            }

            /* If trim is found here, we have a VDDC/VDDM/VDDPA trim value. */
            /* Check if we are  looking for a standby/min or regular voltage. */
            else if (trim_target == (stored_target & 0x00FFU))
            {
                /* Found target voltage */
                *trim_val = (stored_trim & 0x00FFU);
                ret_val = ERROR_NO_ERROR;
                break;
            }
            else if (trim_target == ((stored_target & 0xFF00U) >> 8))
            {
                /* Found target standby/minimum voltage */
                *trim_val = ((stored_trim & 0xFF00U) >> 8);
                ret_val = ERROR_NO_ERROR;
                break;
            }
            else
            {
                /* Desired setting not found. */
                *trim_val = 0xFFFF;
                ret_val = ERROR_NO_TRIM_FOUND;
            }
        }
        /* We have an 8-bit trim setting, therefore it must be:
         * Bandgap, DCDC, VDDRF, VDDIF or VDDFLASH */
        else if (((stored_trim & 0xFF00) == 0) && (ret_val == ERROR_NO_ERROR))
        {
            /* 8 bit target, 8 bit trim, must be VDDRF, VDDIF or
            * VDDFLASH. stored_trim assumed to not be 16-bits.*/
            if (trim_target == stored_target)
            {
                *trim_val = stored_trim & 0xFF;
                ret_val = ERROR_NO_ERROR;
                break;
            }
            else
            {
                /* Desired setting not found. */
                *trim_val = 0xFFFF;
                ret_val = ERROR_NO_TRIM_FOUND;
            }
        }
        else
        {
            /* 8 bit target, 16 bit trim, must be bandgap or DCDC.*/
            if (trim_target == (stored_target & 0xFFU))
            {
                /* Trim setting for DCDC or Bandgap found. */
                *trim_val = stored_trim;
                ret_val = ERROR_NO_ERROR;
                break;
            }
            else
            {
                /* Desired setting not found. */
                *trim_val = 0xFFFF;
                ret_val = ERROR_NO_TRIM_FOUND;
            }
        }
    }

    return ret_val;
}

uint32_t Sys_Trim_GetLSADTrim(uint32_t *addr, uint32_t *gain, uint32_t *offset)
{
    uint32_t ret_val = ERROR_NO_ERROR;

    /* Check for null pointers */
    if ((addr == NULL_POINTER) || (gain == NULL_POINTER) || (offset == NULL_POINTER))
    {
        ret_val = ERROR_NULL;
    }

    /* check if a valid LSAD offset and gain value is present. */
    else if (((*addr == MIN_32_BIT) || (*addr == MAX_32_BIT) ||
              (*(addr + LSAD_GAIN) == MIN_32_BIT) || (*(addr + LSAD_GAIN) == MAX_32_BIT))
             && (ret_val == ERROR_NO_ERROR))
    {
        ret_val = ERROR_NO_TRIM_FOUND;
    }

    if (ret_val == ERROR_NO_ERROR)
    {
        /* Valid gain and offset values found. Return them. */
        *gain = ((*addr) & LSAD_OFFSET_MASK);
        *offset = (*(addr + LSAD_GAIN)) & LSAD_GAIN_MASK;
    }

    return ret_val;
}

uint32_t Sys_Trim_LoadBandgap(TRIM_Type *trim_values, uint32_t target_v, uint32_t target_i)
{
    uint32_t ret_val = ERROR_NO_ERROR;

    /* Check for null pointers */
    if (trim_values == NULL_POINTER)
    {
        return ERROR_NULL;
    }

    /* Load trim values */
    ret_val = Sys_Trim_LoadBandgap_private(trim_values, target_v, target_i);

    return ret_val;
}

static uint32_t Sys_Trim_LoadBandgap_private(TRIM_Type *trim_values, uint32_t target_v, uint32_t target_i)
{
    uint16_t trim_voltage = 0;
    uint16_t trim_current = 0;
    uint32_t ret_val = ERROR_NO_ERROR;

    /* Check for null pointers */
    if (trim_values == NULL_POINTER)
    {
        return ERROR_NULL;
    }

    /* Grab voltage trim values */
    if (Sys_Trim_GetTrim((uint32_t *)(trim_values->bandgap),
                         target_v,
                         TRIM_RECORDS,
                         &trim_voltage))
    {
        /* If there is an error grabbing a value, flag an error. */
        ret_val |= ERROR_BG_V_INVALID;
    }

    /* Grab slope trim values */
    if (Sys_Trim_GetTrim(((uint32_t *)(trim_values->bandgap)) + TRIM_BANDGAP_CURRENT_OFFSET,
                         target_i,
                         TRIM_RECORDS,
                         &trim_current))
    {
        /* If there is an error grabbing a value, flag an error. */
        ret_val |= ERROR_BG_I_INVALID;
    }

    if (ret_val == ERROR_NO_ERROR)
    {
        /* Assumption if this point is reached is that trim values
        * are valid. Load control register with retrieved values*/
        /* According to TCL script, BG values are left shifted by 2,
         * need to right shift back. Check if checksum is 32 bit or 16 bit.*/
        if (trim_values->CHECKSUM <= MAX_16_BIT)
        {
            ACS->BG_CTRL =  ((trim_current << ACS_BG_CTRL_ITRIM_Pos) & 0xFF000000UL) |
                           ((trim_current << (ACS_BG_CTRL_ITRIM_Pos - 2)) & 0x00FF0000UL) |
                           (trim_voltage & 0x0000FF00UL)        |
                           ((trim_voltage >> 2) & 0x000000FFUL);
        }
        else
        {
            ACS->BG_CTRL =  ((trim_current << ACS_BG_CTRL_ITRIM_Pos) & 0xFFFF0000UL) |
                           (trim_voltage & 0x0000FFFFUL);
        }
    }
    else if (!(ret_val & ERROR_BG_I_INVALID))
    {
        /* Assumption if this point is reached is that current trim values
         * are valid. Load control register with retrieved values
         * According to TCL script, BG values are left shifted by 2,
         *  need to right shift back. */

        if (trim_values->CHECKSUM <= MAX_16_BIT)
        {
            ACS->BG_CTRL = (ACS->BG_CTRL & 0x0000FFFFUL)  |
                           ((trim_current << ACS_BG_CTRL_ITRIM_Pos) & 0xFF000000UL) |
                           ((trim_current << (ACS_BG_CTRL_ITRIM_Pos - 2)) & 0x00FF0000UL);
        }
        else
        {
            ACS->BG_CTRL = (ACS->BG_CTRL & 0x0000FFFFUL)  |
                           ((trim_current << ACS_BG_CTRL_ITRIM_Pos) & 0xFFFF0000UL);
        }
    }
    else if (!(ret_val & ERROR_BG_V_INVALID))
    {
        /* Assumption if this point is reached is that voltage trim values
         * are valid. Load control register with retrieved values
         * According to TCL script, BG values are left shifted by 2,
         *  need to right shift back. */
        if (trim_values->CHECKSUM <= MAX_16_BIT)
        {
            ACS->BG_CTRL = (ACS->BG_CTRL & 0xFFFF0000UL)  |
                           (trim_voltage & 0x0000FF00UL) |
                           ((trim_voltage >> 2) & 0x000000FFUL);
        }
        else
        {
            ACS->BG_CTRL = (ACS->BG_CTRL & 0xFFFF0000UL) | (trim_voltage & 0x0000FFFFUL);
        }
    }
    else
    {
        /* other error conditions captured by returning ret_val */
    }

    return ret_val;
}

uint32_t Sys_Trim_LoadDCDC(TRIM_Type *trim_values, uint32_t target)
{
    uint32_t ret_val = ERROR_NO_ERROR;

    /* Check for null pointers */
    if (trim_values == NULL_POINTER)
    {
        return ERROR_NULL;
    }

    /* Load trim values */
    ret_val = Sys_Trim_LoadDCDC_private(trim_values, target);

    return ret_val;
}

static uint32_t Sys_Trim_LoadDCDC_private(TRIM_Type *trim_values, uint32_t target)
{
    uint16_t trim = 0;
    uint32_t reg_contents = (ACS->VCC_CTRL & ~ACS_VCC_CTRL_VTRIM_Mask);
    uint32_t ret_val = ERROR_NO_ERROR;

    /* Check for null pointers */
    if (trim_values == NULL_POINTER)
    {
        return ERROR_NULL;
    }

    /* Grab voltage trim values */
    ret_val = Sys_Trim_GetTrim((uint32_t *)trim_values->dcdc,
                               target,
                               TRIM_RECORDS,
                               &trim);

    if (ret_val == ERROR_NO_ERROR)
    {
        /* Assumption if this point is reached is that trim values
         * are valid. Load control register with retrieved values
         * Check if we are in LDO or BUCK mode.*/
        ACS->VCC_CTRL = ((ACS->VCC_CTRL & VCC_BUCK) ? ((trim & 0xFF00) >> 8 ) : (trim & 0x00FF))
                        | reg_contents;
    }
    else
    {
        ret_val |= ERROR_DCDC_INVALID;
    }

    return ret_val;
}

uint32_t Sys_Trim_LoadVDDC(TRIM_Type *trim_values, uint32_t target, uint32_t target_standby)
{
    uint32_t ret_val = ERROR_NO_ERROR;

    /* Check for null pointers */
    if (trim_values == NULL_POINTER)
    {
        return ERROR_NULL;
    }

    /* Load trim values */
    ret_val = Sys_Trim_LoadVDDC_private(trim_values, target, target_standby);

    return ret_val;
}

static uint32_t Sys_Trim_LoadVDDC_private(TRIM_Type *trim_values, uint32_t target, uint32_t target_standby)
{
    uint16_t trim = 0;
    uint16_t standby_trim = 0;
    uint32_t ret_val = ERROR_NO_ERROR;

    /* Check for null pointers */
    if (trim_values == NULL_POINTER)
    {
        return ERROR_NULL;
    }

    /* Grab voltage trim values */
    if (Sys_Trim_GetTrim((uint32_t *)trim_values->vddc,
                         target,
                         TRIM_RECORDS,
                         &trim))
    {
        /* If we get an error, flag it */
        ret_val |= ERROR_VDDC_INVALID;
    }

    /* Grab standby voltage trim values. */
    if (Sys_Trim_GetTrim((uint32_t *)trim_values->vddc,
                         target_standby,
                         TRIM_RECORDS,
                         &standby_trim))
    {
        ret_val |= ERROR_VDDC_STBY_INVALID;
    }

    /* Load trim values */
    if (!(ret_val & ERROR_VDDC_INVALID))
    {
        /* We did not find a standby voltage, but we did find a regular
         * trim setting. */
        ACS_VDDC_CTRL->VTRIM_BYTE = trim & TRIM_8_BIT_TRIM_MASK;
    }
    if (!(ret_val & ERROR_VDDC_STBY_INVALID))
    {
        /* We did not find a regular voltage, but we did find a standby
         * trim setting. */
        ACS_VDDC_CTRL->STANDBY_VTRIM_BYTE = standby_trim & TRIM_8_BIT_TRIM_MASK;
    }

    return ret_val;
}

uint32_t Sys_Trim_LoadVDDM(TRIM_Type *trim_values, uint32_t target, uint32_t target_standby)
{
    uint32_t ret_val = ERROR_NO_ERROR;

    /* Check for null pointers */
    if (trim_values == NULL_POINTER)
    {
        return ERROR_NULL;
    }

    /* Load trim values */
    ret_val = Sys_Trim_LoadVDDM_private(trim_values, target, target_standby);

    return ret_val;
}

static uint32_t Sys_Trim_LoadVDDM_private(TRIM_Type *trim_values, uint32_t target, uint32_t target_standby)
{
    uint16_t trim = 0;
    uint16_t standby_trim = 0;
    uint32_t ret_val = ERROR_NO_ERROR;

    /* Check for null pointers */
    if (trim_values == NULL_POINTER)
    {
        return ERROR_NULL;
    }

    /* Grab voltage trim values */
    if (Sys_Trim_GetTrim((uint32_t *)trim_values->vddm,
                         target,
                         TRIM_RECORDS,
                         &trim))
    {
        /* If we get an error, flag it */
        ret_val |= ERROR_VDDM_INVALID;
    }

    /* Grab standby voltage trim values. */
    if (Sys_Trim_GetTrim((uint32_t *)trim_values->vddm,
                         target_standby,
                         TRIM_RECORDS,
                         &standby_trim))
    {
        ret_val |= ERROR_VDDM_STBY_INVALID;
    }

    /* Load trim values */
    if (!(ret_val & ERROR_VDDM_INVALID))
    {
        /* We did not find a standby voltage, but we did find a regular
         * trim setting. */
        ACS_VDDM_CTRL->VTRIM_BYTE = trim & TRIM_8_BIT_TRIM_MASK;
    }
    if (!(ret_val & ERROR_VDDM_STBY_INVALID))
    {
        /* We did not find a regular voltage, but we did find a standby
         * trim setting. */
        ACS_VDDM_CTRL->STANDBY_VTRIM_BYTE = standby_trim & TRIM_8_BIT_TRIM_MASK;
    }

    return ret_val;
}

uint32_t Sys_Trim_LoadVDDPA(TRIM_Type *trim_values, uint32_t target)
{
    uint32_t ret_val = ERROR_NO_ERROR;

    /* Check for null pointers */
    if (trim_values == NULL_POINTER)
    {
        return ERROR_NULL;
    }

    /* Load trim values */
    ret_val = Sys_Trim_LoadVDDPA_private(trim_values, target);

    return ret_val;
}

static uint32_t Sys_Trim_LoadVDDPA_private(TRIM_Type *trim_values, uint32_t target)
{
    uint16_t trim = 0;
    uint32_t ret_val = ERROR_NO_ERROR;

    /* Check for null pointers */
    if (trim_values == NULL_POINTER)
    {
        return ERROR_NULL;
    }

    /* Grab voltage trim values */
    if (Sys_Trim_GetTrim((uint32_t *)trim_values->vddpa,
                         target,
                         TRIM_RECORDS,
                         &trim))
    {
        ret_val |= ERROR_VDDPA_INVALID;
    }

    if (ret_val == ERROR_NO_ERROR)
    {
        /* Assumption if this point is reached is trim values
         * are valid. Load control register with retrieved value*/
        ACS_VDDPA_CTRL->VTRIM_BYTE = trim;
    }
    else
    {
        ret_val |= ERROR_NO_TRIM_FOUND;
    }

    return ret_val;
}

uint32_t Sys_Trim_LoadVDDRF(TRIM_Type *trim_values, uint32_t target)
{
    uint32_t ret_val = ERROR_NO_ERROR;

    /* Check for null pointers */
    if (trim_values == NULL_POINTER)
    {
        return ERROR_NULL;
    }

    /* Load trim values */
    ret_val = Sys_Trim_LoadVDDRF_private(trim_values, target);

    return ret_val;
}

static uint32_t Sys_Trim_LoadVDDRF_private(TRIM_Type *trim_values, uint32_t target)
{
    uint16_t trim = 0;
    uint32_t ret_val = ERROR_NO_ERROR;

    /* Check for null pointers */
    if (trim_values == NULL_POINTER)
    {
        return ERROR_NULL;
    }

    /* Grab voltage trim values */
    ret_val = Sys_Trim_GetTrim((uint32_t *)trim_values->vddrf,
                               target,
                               TRIM_RECORDS,
                               &trim);
    if (ret_val == ERROR_NO_ERROR)
    {
        /* We have valid trim value, load the trim value. */
        ACS_VDDRF_CTRL->VTRIM_BYTE = trim & TRIM_8_BIT_TRIM_MASK;
    }

    return ret_val;
}

uint32_t Sys_Trim_LoadVDDFLASH(TRIM_Type *trim_values, uint32_t target)
{
    uint32_t ret_val = ERROR_NO_ERROR;

    /* Check for null pointers */
    if (trim_values == NULL_POINTER)
    {
        return ERROR_NULL;
    }

    /* Load trim values */
    ret_val = Sys_Trim_LoadVDDFLASH_private(trim_values, target);

    return ret_val;
}

static uint32_t Sys_Trim_LoadVDDFLASH_private(TRIM_Type *trim_values, uint32_t target)
{
    uint16_t trim = 0;
    uint32_t ret_val = ERROR_NO_ERROR;

    /* Check for null pointers */
    if (trim_values == NULL_POINTER)
    {
        return ERROR_NULL;
    }

    /* Grab voltage trim values */
    ret_val = Sys_Trim_GetTrim((uint32_t *)trim_values->vddflash,
                               target,
                               TRIM_RECORDS,
                               &trim);
    if (ret_val == ERROR_NO_ERROR)
    {
        /* We have valid trim value, load the trim value. */
        ACS_VDDFLASH_CTRL->VTRIM_BYTE = trim & TRIM_8_BIT_TRIM_MASK;
    }

    return ret_val;
}

uint32_t Sys_Trim_LoadRCOSC(TRIM_Type *trim_values, uint32_t target)
{
    uint32_t ret_val = ERROR_NO_ERROR;

    /* Check for null pointers */
    if (trim_values == NULL_POINTER)
    {
        return ERROR_NULL;
    }

    /* Load trim values */
    ret_val = Sys_Trim_LoadRCOSC_private(trim_values, target);

    return ret_val;
}

static uint32_t Sys_Trim_LoadRCOSC_private(TRIM_Type *trim_values, uint32_t target)
{
    uint16_t trim = 0;
    uint32_t ret_val = ERROR_NO_ERROR;

    /* Check for null pointers */
    if (trim_values == NULL_POINTER)
    {
        return ERROR_NULL;
    }

    /* Grab voltage trim values */
    ret_val = Sys_Trim_GetTrim((uint32_t *)trim_values->rcosc,
                               target,
                               TRIM_RC_RECORDS * 4,
                               &trim);
    if (ret_val == ERROR_NO_ERROR)
    {
        /* We have valid trim value, load the trim value. */
        ACS_RCOSC_CTRL->RC_FTRIM_BYTE = trim & TRIM_8_BIT_TRIM_MASK;
    }
    return ret_val;
}

uint32_t Sys_Trim_LoadRCOSC32(TRIM_Type *trim_values, uint32_t target)
{
    uint32_t ret_val = ERROR_NO_ERROR;

    /* Check for null pointers */
    if (trim_values == NULL_POINTER)
    {
        return ERROR_NULL;
    }

    /* Load trim values */
    ret_val = Sys_Trim_LoadRCOSC32_private(trim_values, target);

    return ret_val;
}

static uint32_t Sys_Trim_LoadRCOSC32_private(TRIM_Type *trim_values, uint32_t target)
{
    uint16_t trim = 0;
    uint32_t reg_contents = (ACS->RCOSC_CTRL & 0xFFFFFF00UL);
    uint32_t ret_val = ERROR_NO_ERROR;

    /* Check for null pointers */
    if (trim_values == NULL_POINTER)
    {
        return ERROR_NULL;
    }

    /* Grab voltage trim values */
    ret_val = Sys_Trim_GetTrim((uint32_t *)trim_values->rcosc32,
                               target,
                               TRIM_RECORDS,
                               &trim);
    if (ret_val == ERROR_NO_ERROR)
    {
        /* We have valid trim value, load the trim value. */
        ACS->RCOSC_CTRL = trim | reg_contents;
    }

    return ret_val;
}

uint32_t Sys_Trim_LoadThermistor(TRIM_Type *trim_values, uint16_t target)
{
    uint32_t ret_val = ERROR_NO_ERROR;

    /* Check for null pointers */
    if (trim_values == NULL_POINTER)
    {
        return ERROR_NULL;
    }

    /* Load trim values */
    ret_val = Sys_Trim_LoadThermistor_private(trim_values, target);

    return ret_val;
}

static uint32_t Sys_Trim_LoadThermistor_private(TRIM_Type *trim_values, uint16_t target)
{
    uint16_t trim = 0;
    uint32_t reg_contents = (ACS->TEMP_CURR_CFG & 0xFFU);
    uint32_t ret_val = ERROR_NO_ERROR;

    /* Check for null pointers */
    if (trim_values == NULL_POINTER)
    {
        return ERROR_NULL;
    }

    /* Grab voltage trim values */
    ret_val = Sys_Trim_GetTrim((uint32_t *)(&(trim_values->thermistor)),
                               target,
                               TRIM_THERMISTOR_RECORDS,
                               &trim);
    if (ret_val == ERROR_NO_ERROR)
    {
        /* We have valid trim value, load the trim value. */
        ACS->TEMP_CURR_CFG = (trim << 8) | reg_contents;
    }

    return ret_val;
}

uint32_t Sys_Trim_LoadCustom(void)
{
    uint32_t ret_val = ERROR_NO_ERROR;
    uint32_t *addr;
	uint32_t value;

	/* Read the signature and check for SiP signature. Exit if fails. */
	value = *((uint32_t* )(TRIM_CUSTOM_BASE+TRIM_CUSTOM_SIGNATURE_OFFSET));
    if ((value != TRIM_CUSTOM_SIP1_SIGNATURE) && (value != TRIM_CUSTOM_CUST_SIGNATURE))
    {
    	return ERROR_TRIM_CUSTOM_SIGNATURE_INVALID;
    }

    /* Calculate CRC. Check the value and exit if fails. */
    SYS_CRC_CONFIG(CRC_32);
    SYS_CRC_32INITVALUE();
    for (addr = (uint32_t *)TRIM_CUSTOM_BASE;
	     addr <= ((uint32_t *)(TRIM_CUSTOM_BASE + TRIM_CUSTOM_CRC_OFFSET - 1));
		 addr++)
    {
        SYS_CRC_ADD(*addr, 32);
    }
	value = *((uint32_t *)(TRIM_CUSTOM_BASE + TRIM_CUSTOM_CRC_OFFSET));
	if ((value) != SYS_CRC_GETFINALVALUE())
    {
        return ERROR_INVALID_CRC;
    }

    /* Read ICH_TRIM value. If value is out of range, update the error code.
     * Otherwise, write to the register. */
	value = *((uint32_t *)(TRIM_CUSTOM_BASE + TRIM_CUSTOM_ICH_OFFSET));
    if (value > MAX_4_BIT)
    {
    	ret_val |= ERROR_TRIM_CUSTOM_ICH_INVALID;
    }
    else
    {
        ACS->VCC_CTRL = ((ACS->VCC_CTRL) & (~(ACS_VCC_CTRL_ICH_TRIM_Mask))) | 
		                 (value << ACS_VCC_CTRL_ICH_TRIM_Pos);
    }

    /* Read XTAL_TRIM value. If value is out of range, update the error code.
 	 * Otherwise, write to the register. */
    value = *((uint32_t *)(TRIM_CUSTOM_BASE + TRIM_CUSTOM_XTAL_OFFSET));
    if (value > MAX_8_BIT)
    {
        ret_val |= ERROR_TRIM_CUSTOM_XTAL_INVALID;
    }
    else
    {
		RF->REG2E = ((RF->REG2E) & (~(RF_REG2E_XTAL_TRIM_XTAL_TRIM_INIT_Mask))) | 
		             (value << RF_REG2E_XTAL_TRIM_XTAL_TRIM_INIT_Pos);
		RF->REG2E = ((RF->REG2E) & (~(RF_REG2E_XTAL_TRIM_XTAL_TRIM_Mask))) |
                	 (value << RF_REG2E_XTAL_TRIM_XTAL_TRIM_Pos);
    }

    return ret_val;
}

