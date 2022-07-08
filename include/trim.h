/**
 * @file trim.h
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
 * @endparblock
 */
/* ----------------------------------------------------------------------------
 * If building with a C++ compiler, make all of the definitions in this header
 * have a C binding.
 * ------------------------------------------------------------------------- */

#ifndef TRIM_H
#define TRIM_H

#include <hw.h>

#ifndef RSL15_CID
#include <trim_vddif.h>
#endif    /* ifndef RSL15_CID */

#ifdef __cplusplus
extern "C"
{
#endif    /* ifdef __cplusplus */

/* Required for calibration error workaround */
/** @cond INTERNAL */
#define TEST_DATESTAMP_OFFSET   0x24
#define TEST_BADCAL_DATE_NOV    0x07e30B00UL
#define TEST_BADCAL_DATE_DEC    0x07e30C00UL
#define TEST_DATE_MASK          0xFFFFFF00UL

/** @endcond */

/** @addtogroup HAL
 *  @{
 */
/** @defgroup HALTRIM Trimming Support
 *  Power, clock, and sensor component trimming hardware abstraction layer
 *  @{
 */

/** NULL pointer */
#define NULL_POINTER                        0

/* Max and minimum values to determine if a valid trim is present. */
/** Minimum 32-bit value */
#define MIN_32_BIT                          0x00000000UL

/** Maximum 32-bit value */
#define MAX_32_BIT                          0xFFFFFFFFUL

/** Minimum 18-bit value */
#define MIN_18_BIT                          0x00000U

/** Maximum 18-bit value */
#define MAX_18_BIT                          0x3FFFFU

/** Minimum 16-bit value */
#define MIN_16_BIT                          0x0000U

/** Maximum 16-bit value */
#define MAX_16_BIT                          0xFFFFU

/** Minimum 8-bit value */
#define MIN_8_BIT                           0x00U

/** Maximum 8-bit value */
#define MAX_8_BIT                           0xFFU

/** Maximum 4-bit value */
#define MAX_4_BIT                           0xFU

/* Trim function errors */
/** @section errors */
#define ERROR_NO_ERROR                      0

/** Null pointer error */
#define ERROR_NULL                          (1 << 1)

/** Target trim value not found */
#define ERROR_NO_TRIM_FOUND                 (1 << 3)

/** Trims in region specified are not valid */
#define ERROR_INVALID_TRIM                  (1 << 4)

/** Trim region CRC has failed */
#define ERROR_INVALID_CRC                   (1 << 5)

/** Bandgap target value is invalid */
#define ERROR_BG_INVALID                    (1 << 6)

/** Bandgap voltage trim is invalid */
#define ERROR_BG_V_INVALID                  (1 << 7)

/** Bandgap current trim is invalid */
#define ERROR_BG_I_INVALID                  (1 << 8)

/** DCDC trim is invalid */
#define ERROR_DCDC_INVALID                  (1 << 9)

/** VDDC trim is invalid */
#define ERROR_VDDC_INVALID                  (1 << 10)

/** VDCC standby trim is invalid */
#define ERROR_VDDC_STBY_INVALID             (1 << 11)

/** VDDM trim is invalid */
#define ERROR_VDDM_INVALID                  (1 << 12)

/** VDCM standby trim is invalid */
#define ERROR_VDDM_STBY_INVALID             (1 << 13)

/** VDDRF trim is invalid */
#define ERROR_VDDRF_INVALID                 (1 << 14)

/** VDDPA trim is invalid */
#define ERROR_VDDPA_INVALID                 (1 << 15)

/** VDDPA minimum trim is invalid */
#define ERROR_VDDPA_MIN_INVALID             (1 << 16)

/** VDDIF trim is invalid */
#define ERROR_VDDIF_INVALID                 (1 << 17)

/** VDDFLASH trim is invalid */
#define ERROR_VDDFLASH_INVALID              (1 << 18)

/** RC start oscillator trim is invalid */
#define ERROR_RCOSC_INVALID                 (1 << 19)

/** RC standby oscillator trim is invalid */
#define ERROR_RCOSC32_INVALID               (1 << 20)

/** LSAD gain or offset is invalid */
#define ERROR_LSAD_INVALID                  (1 << 21)

/** Temperature sensor gain or offset is invalid */
#define ERROR_TEMPERATURE_INVALID           (1 << 22)

/** Thermistor gain or offset is invalid */
#define ERROR_THERMISTOR_INVALID            (1 << 23)

/** Measured reference temperature is invalid */
#define ERROR_MEASURED_INVALID              (1 << 25)

/** Custom signature check is invalid */
#define ERROR_TRIM_CUSTOM_SIGNATURE_INVALID (1 << 26)

/** Custom ICH trim value is invalid */
#define ERROR_TRIM_CUSTOM_ICH_INVALID       (1 << 27)

/** Custom Xtal trim value is invalid */
#define ERROR_TRIM_CUSTOM_XTAL_INVALID      (1 << 28)

/**
 * @brief Default trim targets present in NVR7
 */
typedef enum
{
    TARGET_BANDGAP_V            =    75,     /**< 750mV  */
    TARGET_BANDGAP_I            =    100,    /**< 1000nA */
    TARGET_DCDC_1200            =    120,    /**< 1.2V   */
    TARGET_DCDC_1120            =    112,    /**< 1.12V  */
    TARGET_DCDC_1350            =    135,    /**< 1.35V  */
    TARGET_DCDC_1100            =    110,    /**< 1.10V  */
    TARGET_VDDC_1150            =    115,    /**< 1.15V  */
    TARGET_VDDC_1000            =    100,    /**< 1.00V  */
    TARGET_VDDC_1080            =    108,    /**< 1.08V  */
    TARGET_VDDC_920             =    92,     /**< 0.92V  */
    TARGET_VDDC_1050            =    105,    /**< 1.05V  */
    TARGET_VDDC_STANDBY         =    80,     /**< 0.80V  */
    TARGET_VDDM_1150            =    115,    /**< 1.15V  */
    TARGET_VDDM_1080            =    108,    /**< 1.08V  */
    TARGET_VDDM_1100            =    110,    /**< 1.05V  */
    TARGET_VDDM_STANDBY         =    80,     /**< 0.80V  */
    TARGET_VDDRF_1100           =    110,    /**< 1.10V  */
    TARGET_VDDRF_1070           =    107,    /**< 1.07V  */
    TARGET_VDDRF_1200           =    120,    /**< 1.20V  */
    TARGET_VDDPA_1300           =    130,    /**< 1.30V  */
    TARGET_VDDPA_1260           =    126,    /**< 1.26V  */
    TARGET_VDDPA_1600           =    160,    /**< 1.60V  */
    TARGET_VDDPA_MIN_1100       =    110,    /**< 1.10V  */
    TARGET_VDDIF_1800           =    180,    /**< 1.80V  */
    TARGET_FLASH_1600           =    160,    /**< 1.60V  */
    TARGET_RC3                  =    3000,    /**< 3MHz   */
    TARGET_RC12                 =    12000,    /**< 12MHz  */
    TARGET_RC24                 =    24000,    /**< 24MHz  */
    TARGET_RC48                 =    48000,    /**< 48MHz  */
    TARGET_RC32K                =    32768,    /**< 32kHz  */
    TARGET_THERMISTOR_10        =    10,     /**< 10uA  */
    TARGET_THERMISTOR_5         =    5     /**< 5.0uA  */
} TrimTarget_t;

/**
 * @brief Voltage rail and oscillator names
 */
typedef enum
{
    TRIM_BANDGAP,     /** Select loading bandgap trim values */
    TRIM_DCDC,        /** Select loading DCDC trim values */
    TRIM_VDDC,        /** Select loading VDDC trim values */
    TRIM_VDDM,        /** Select loading VDDM trim values */
    TRIM_VDDRF,       /** Select loading VDDRF trim values */
    TRIM_VDDPA,       /** Select loading VDDPA trim values */
    TRIM_VDDIF,       /** Select loading VDDIF trim values */
    TRIM_FLASH,       /** Select loading VDDFLASH trim values */
    TRIM_RCOSC,       /** Select loading RC oscillator trim values */
    TRIM_RCOSC32,     /** Select loading RC32K oscillator trim values */
} TrimName_t;

/* Trim Masks*/
/** Temperature record 18-bit trim value mask */
#define TR_REG_TRIM_MASK                0x3FU

/** 8-bit trim value mask */
#define TRIM_8_BIT_TRIM_MASK            0xFFU

/** 16-bit trim value mask */
#define TRIM_16_BIT_TRIM_MASK           0xFFFFU

/* @LSAD related defines */
/** LSAD high frequency compensation values */
#define LSAD_HF                         0

/** LSAD low frequency compensation values */
#define LSAD_LF                         1

/** LSAD offset compensation address offset */
#define LSAD_OFFSET                     0x00U

/** LSAD offset compensation mask */
#define LSAD_OFFSET_MASK                0xFFU

/** LSAD gain compensation address offset */
#define LSAD_GAIN                       0x04U

/** LSAD gain compensation mask */
#define LSAD_GAIN_MASK                  0x3FFU

/* Used for function pointer arrays. */
/** @cond Function pointer count */
#define TRIM_NUM_FUNCTIONS_2_ARGS       3
#define TRIM_NUM_FUNCTIONS_1_ARG        7

/** @endcond */

/** Default trim instance, pointing to NVR7 */
#define TRIM (TRIM_Type *)TRIM_BASE_DEFAULT

/** Supplemental trim instance, pointing to NVR4 */
#define TRIM_SUPPLEMENTAL (TRIM_Type *)FLASH0_NVR4_BASE

/** SiP Signature for NVR6 custom trim calibration */
#define TRIM_CUSTOM_SIP1_SIGNATURE       0x53495031

/** Custom Signature for NVR6 custom trim calibration */
#define TRIM_CUSTOM_CUST_SIGNATURE     	 0x43555354

/* Global variables */
extern uint32_t trim_args1[TRIM_NUM_FUNCTIONS_1_ARG];       /**< Trim targets for items needing one trim */
extern uint32_t trim_args2[TRIM_NUM_FUNCTIONS_2_ARGS][2];   /**< Trim targets for items needing two trims */

/**
 * @brief       Load trim values from the specified memory location.
 * @param[in]   trim_region Pointer to section of memory containing
 *                          trim values, typically base of NVR7.
 * @param[in]   targets_1 uint32_t[6] containing targets in this order:
 *              DCDC, VDDRF, VDDIF, VDDFLASH, RC 3MHz, RC 32kHz
 * @param[in]   targets_2 uint32_t[4][2] containing targets in this order:
 *              Bandgap voltage | Bandgap current
 *              VDDC voltage    | VDDC standby voltage
 *              VDDM voltage    | VDDM standby voltage
 *              VDDPA voltage   | VDDPA minimum voltage
 * @return      A code indicating whether an error has occurred.
 *              Error codes: @ref errors
 * @note        Does not work with LSAD gain/offset. Use @ref lsadload
 *              Sys_Trim_GetLSADTrim() instead.
 * @note        For detail on input parameters, see trim_args1 & 2 in trim.c
 * @examplecode HAL_examples.c Sys_Trim_LoadTrims_Example
 */
uint32_t Sys_Trim_LoadTrims(TRIM_Type *trim_region,
        uint32_t targets_1[TRIM_NUM_FUNCTIONS_1_ARG],
        uint32_t targets_2[TRIM_NUM_FUNCTIONS_1_ARG][2]);

/**
 * @brief       Load a trim value for a specific voltage regulator or oscillator.
 *              This function attempts to load calibration values from customer
 *              trim settings in NVR4, then load manufacturing calibration values
 *              from NVR7 if customer calibration values are not found.
 * @param[in]   target_name Voltage regulator or oscillator to load trim values for.
 * @param[in]   target_value1 Main trim target value
 * @param[in]   target_value2 Secondary trim target value used on some regulators.
 * @return      A code indicating whether an error has occurred.
 *              Error codes: @ref errors
 * @note        Does not work with LSAD gain/offset. Use @ref lsadload
 *              Sys_Trim_GetLSADTrim() instead.
 * @examplecode HAL_examples.c Sys_Trim_LoadTrims_Example
 */
uint32_t Sys_Trim_LoadSingleTrim(uint32_t target_name,
                                 uint32_t target_value1,
                                 uint32_t target_value2);

/**
 * @brief       Verify if the trims memory is populated correctly.
 * @param[in]   trim_region Pointer to section of memory containing
 *                          trim values, typically base of NVR7.
 * @return      A code indicating whether an error has occurred.
 *              Error codes: @ref errors
 * @examplecode HAL_examples.c Sys_Trim_VerifyTrims_Example
 */
uint32_t Sys_Trim_VerifyTrims(TRIM_Type *trim_region);

/**
 * @brief       Check if the CRC for the indicated region is valid.
 * @param[in]   trim_region Pointer to section of memory containing
 *                          trim values, typically base of NVR7.
 * @return      A code indicating whether an error has occurred.
 *              Error codes: @ref errors
 * @examplecode HAL_examples.c Sys_Trim_CheckCRC_Example
 */
uint32_t Sys_Trim_CheckCRC(TRIM_Type *trim_region);

/**
 * @brief       Get the trim value requested, check if it is valid.
 * @param[in]   addr   Pointer to address of base of trim record.
 * @param[in]   trim_target   Target voltage/current/clock
 * @param[in]   record_length   Number of records for that trim value.
 * @param[out]  trim_val   Pointer to return retrieved trim value.
 * @return      A code indicating whether an error has occurred.
 *              Error codes: @ref errors
 * @examplecode HAL_examples.c Sys_Trim_GetTrim_Example
 */
uint32_t Sys_Trim_GetTrim(uint32_t *addr,
                          uint16_t trim_target,
                          uint32_t record_length,
                          uint16_t *trim_val);

/**
 * @brief       Load target trim value, if present.
 * @param[in]   trim_values  Pointer to section of memory containing
 *                          trim values, typically base of NVR7.
 * @param[in]   target_v  The target voltage trim setting.
 * @param[in]   target_i  The target current trim setting.
 * @return      A code indicating whether an error has occurred.
 *              Error codes: @ref errors
 * @examplecode HAL_examples.c Sys_Trim_LoadBandgap_Example
 */
uint32_t Sys_Trim_LoadBandgap(TRIM_Type *trim_values,
                              uint32_t target_v,
                              uint32_t target_i);

/**
 * @brief       Load target trim value for current mode (LDO or BUCK).
 * @param[in]   trim_values  Pointer to section of memory containing
 *                          trim values, typically base of NVR7.
 * @param[in]   target  The target voltage trim setting.
 * @return      A code indicating whether an error has occurred.
 *              Error codes: @ref errors
 * @examplecode HAL_examples.c Sys_Trim_LoadDCDC_Example
 */
uint32_t Sys_Trim_LoadDCDC(TRIM_Type *trim_values, uint32_t target);

/**
 * @brief       Load target trim value, if present.
 * @param[in]   trim_values  Pointer to section of memory containing
 *                          trim values, typically base of NVR7.
 * @param[in]   target  The target voltage trim setting.
 * @param[in]   target_standby  The target standby voltage trim setting.
 * @return      A code indicating whether an error has occurred.
 *              Error codes: @ref errors
 * @examplecode HAL_examples.c Sys_Trim_LoadVDDC_Example
 */
uint32_t Sys_Trim_LoadVDDC(TRIM_Type *trim_values, uint32_t target,
                           uint32_t target_standby);

/**
 * @brief       Load target trim value, if present.
 * @param[in]   trim_values  Pointer to section of memory containing
 *                          trim values, typically base of NVR7.
 * @param[in]   target  The target voltage trim setting.
 * @param[in]   target_standby  The target standby voltage trim setting.
 * @return      A code indicating whether an error has occurred.
 *              Error codes: @ref errors
 * @examplecode HAL_examples.c Sys_Trim_LoadVDDM_Example
 */
uint32_t Sys_Trim_LoadVDDM(TRIM_Type *trim_values, uint32_t target,
                           uint32_t target_standby);

/**
 * @brief       Load target trim value, if present.
 * @param[in]   trim_values  Pointer to section of memory containing
 *                          trim values, typically base of NVR7.
 * @param[in]   target  The target voltage trim setting.
 * @return      A code indicating whether an error has occurred.
 *              Error codes: @ref errors
 * @examplecode HAL_examples.c Sys_Trim_LoadVDDPA_Example
 */
uint32_t Sys_Trim_LoadVDDPA(TRIM_Type *trim_values, uint32_t target);

/**
 * @brief       Load target trim value, if present.
 * @param[in]   trim_values  Pointer to section of memory containing
 *                          trim values, typically base of NVR7.
 * @param[in]   target  The target voltage trim setting.
 * @return      A code indicating whether an error has occurred.
 *              Error codes: @ref errors
 * @examplecode HAL_examples.c Sys_Trim_LoadVDDRF_Example
 */
uint32_t Sys_Trim_LoadVDDRF(TRIM_Type *trim_values, uint32_t target);

/**
 * @brief       Load custom trim values from NVR6.
 * @return      A code indicating whether an error has occurred.
 *              Error codes: @ref errors
 * @examplecode HAL_examples.c SYS_TRIM_LOAD_CUSTOM_Example
 */
uint32_t Sys_Trim_LoadCustom(void);

#ifdef RSL15_CID
/**
 * @brief       Load target trim value, if present.
 * @param[in]   trim_values  Pointer to section of memory containing
 *                          trim values, typically base of NVR7.
 * @param[in]   target  The target voltage trim setting.
 * @return      VDDIF does not exist, return error.
 * @examplecode HAL_examples.c Sys_Trim_LoadVDDIF_Example
 */
static inline uint32_t Sys_Trim_LoadVDDIF(__attribute__ ((unused)) TRIM_Type *trim_values,
                                          __attribute__ ((unused)) uint32_t target)
{
    return ERROR_NO_TRIM_FOUND;
}

#endif    /* ifdef RSL15_CID */

/**
 * @brief       Load target trim value, if present.
 * @param[in]   trim_values Pointer to section of memory containing
 *                          trim values, typically base of NVR7.
 * @param[in]   target The voltage trim setting desired.
 * @return      A code indicating whether an error has occurred.
 *              Error codes: @ref errors
 * @examplecode HAL_examples.c Sys_Trim_LoadVDDFLASH_Example
 */
uint32_t Sys_Trim_LoadVDDFLASH(TRIM_Type *trim_values, uint32_t target);

/**
 * @brief       Load target trim value, if present.
 * @param[in]   trim_values Pointer to section of memory containing
 *                          trim values, typically base of NVR7.
 * @param[in]   target  The target clock trim setting.
 * @return      A code indicating whether an error has  occurred.
 *              Error codes: @ref errors
 * @examplecode HAL_examples.c Sys_Trim_LoadRCOSC_Example
 */
uint32_t Sys_Trim_LoadRCOSC(TRIM_Type *trim_values, uint32_t target);

/**
 * @brief       Load target trim value, if present.
 * @param[in]   trim_values Pointer to section of memory containing
 *                          trim values, typically base of NVR7.
 * @param[in]   target  The target clock trim setting.
 * @return      A code indicating whether an error has occurred.
 *              Error codes: @ref errors
 * @examplecode HAL_examples.c Sys_Trim_LoadRCOSC32_Example
 */
uint32_t Sys_Trim_LoadRCOSC32(TRIM_Type *trim_values, uint32_t target);

/**
 * @brief       Load target trim value, if present.
 * @param[in]   trim_values Pointer to section of memory containing
 *                          trim values, typically base of NVR7.
 * @param[in]   target  The target thermistor trim setting.
 * @return      A code indicating whether an error has occurred.
 *              Error codes: @ref errors
 * @examplecode HAL_examples.c Sys_Trim_LoadThermistor_Example
 */
uint32_t Sys_Trim_LoadThermistor(TRIM_Type *trim_values, uint16_t target);

/**@section lsadload
 * @brief       Load LSAD gain and offset value from specified address. Verifies valid
 *              values first.
 * @param[in]   addr Pointer to memory containing offset in the first
 *                          word, and gain in the second word.
 * @param[out]  gain  Pointer to return gain value.
 * @param[out]  offset  Pointer to return offset value.
 * @return      A code indicating whether an error has occurred.
 *              Error codes: @ref errors
 * @note        Assumes format of LSAD gain and offset storage.
 * @examplecode HAL_examples.c Sys_Trim_GetLSADTrim_Example
 */
uint32_t Sys_Trim_GetLSADTrim(uint32_t *addr, uint32_t *gain, uint32_t *offset);

/**
 * @brief       Load default trim values from NVR7.
 * @return      A code indicating whether an error has occurred.
 *              Error codes: @ref errors
 * @examplecode HAL_examples.c SYS_TRIM_LOAD_DEFAULT_Example
 */
#define SYS_TRIM_LOAD_DEFAULT() Sys_Trim_LoadTrims(TRIM, trim_args1, trim_args2)

/**
 * @brief       Load supplemental trim values from NVR4.
 * @param[in]   x uint32_t[6] containing targets in this order:
 *              DCDC, VDDRF, VDDIF, VDDFLASH, RC 3MHz, RC 32kHz
 * @param[in]   y uint32_t[4][2] containing targets in this order:
 *              Bandgap voltage | Bandgap current
 *              VDDC voltage    | VDDC standby voltage
 *              VDDM voltage    | VDDM standby voltage
 *              VDDPA voltage   | VDDPA minimum voltage
 * @return      A code indicating whether an error has occurred.
 *              Error codes: @ref errors
 * @note        For detail on input parameters, see trim_args1 & 2 in trim.c
 * @examplecode HAL_examples.c SYS_TRIM_LOAD_SUPPLEMENTAL_Example
 */
#define SYS_TRIM_LOAD_SUPPLEMENTAL(x,y) Sys_Trim_LoadTrims(TRIM_SUPPLEMENTAL,x,y)

/**
 * @brief       Load custom trim values from NVR6.
 * @return      A code indicating whether an error has occurred.
 *              Error codes: @ref errors
 * @examplecode HAL_examples.c SYS_TRIM_LOAD_CUSTOM_Example
 */
#define SYS_TRIM_LOAD_CUSTOM() Sys_Trim_LoadCustom()

/** @} */ /* End of the HALTRIM group */
/** @} */ /* End of the HAL group */

/* ----------------------------------------------------------------------------
 * Close the 'extern "C"' block
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */

#endif    /* TRIM_H */