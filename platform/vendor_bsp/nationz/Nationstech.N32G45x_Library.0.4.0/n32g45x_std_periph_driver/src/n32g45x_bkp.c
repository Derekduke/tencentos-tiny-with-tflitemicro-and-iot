/*****************************************************************************
 * Copyright (c) 2019, Nations Technologies Inc.
 *
 * All rights reserved.
 * ****************************************************************************
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Nations' name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY NATIONS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL NATIONS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ****************************************************************************/

/**
 * @file n32g45x_bkp.c
 * @author Nations Solution Team
 * @version v1.0.0
 *
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 */
#include "n32g45x_bkp.h"

/** @addtogroup N32G45X_StdPeriph_Driver
 * @{
 */

/** @addtogroup BKP
 * @brief BKP driver modules
 * @{
 */

/** @addtogroup BKP_Private_TypesDefinitions
 * @{
 */

/**
 * @}
 */

/** @addtogroup BKP_Private_Defines
 * @{
 */

/* ------------ BKP registers bit address in the alias region --------------- */
#define BKP_OFFSET (BKP_BASE - PERIPH_BASE)

/* --- CTRL Register ----*/

/* Alias word address of TPAL bit */
#define CTRL_OFFSET     (BKP_OFFSET + 0x30)
#define TP_ALEV_BIT     0x01
#define CTRL_TP_ALEV_BB (PERIPH_BB_BASE + (CTRL_OFFSET * 32) + (TP_ALEV_BIT * 4))

/* Alias word address of TPE bit */
#define TP_EN_BIT     0x00
#define CTRL_TP_EN_BB (PERIPH_BB_BASE + (CTRL_OFFSET * 32) + (TP_EN_BIT * 4))

/* --- CTRLSTS Register ---*/

/* Alias word address of TPIE bit */
#define CTRLSTS_OFFSET      (BKP_OFFSET + 0x34)
#define TPINT_EN_BIT        0x02
#define CTRLSTS_TPINT_EN_BB (PERIPH_BB_BASE + (CTRLSTS_OFFSET * 32) + (TPINT_EN_BIT * 4))

/* Alias word address of TIF bit */
#define TINTF_BIT        0x09
#define CTRLSTS_TINTF_BB (PERIPH_BB_BASE + (CTRLSTS_OFFSET * 32) + (TINTF_BIT * 4))

/* Alias word address of TEF bit */
#define TEF_BIT        0x08
#define CTRLSTS_TEF_BB (PERIPH_BB_BASE + (CTRLSTS_OFFSET * 32) + (TEF_BIT * 4))

/* ---------------------- BKP registers bit mask ------------------------ */

/* RTCCTRL register bit mask */
#define RTCCTRL_CALV_MASK ((uint16_t)0xFF80)
#define RTCCTRL_MASK      ((uint16_t)0xFC7F)

/**
 * @}
 */

/** @addtogroup BKP_Private_Macros
 * @{
 */

/**
 * @}
 */

/** @addtogroup BKP_Private_Variables
 * @{
 */

/**
 * @}
 */

/** @addtogroup BKP_Private_FunctionPrototypes
 * @{
 */

/**
 * @}
 */

/** @addtogroup BKP_Private_Functions
 * @{
 */

/**
 * @brief  Deinitializes the BKP peripheral registers to their default reset values.
 */
void BKP_DeInit(void)
{
    RCC_EnableBackupReset(ENABLE);
    RCC_EnableBackupReset(DISABLE);
}

/**
 * @brief  Configures the Tamper Pin active level.
 * @param BKP_TamperPinLevel specifies the Tamper Pin active level.
 *   This parameter can be one of the following values:
 *     @arg BKP_TP_HIGH Tamper pin active on high level
 *     @arg BKP_TP_LOW Tamper pin active on low level
 */
void BKP_ConfigTPLevel(uint16_t BKP_TamperPinLevel)
{
    /* Check the parameters */
    assert_param(IS_BKP_TP_LEVEL(BKP_TamperPinLevel));
    *(__IO uint32_t*)CTRL_TP_ALEV_BB = BKP_TamperPinLevel;
}

/**
 * @brief  Enables or disables the Tamper Pin activation.
 * @param Cmd new state of the Tamper Pin activation.
 *   This parameter can be: ENABLE or DISABLE.
 */
void BKP_TPEnable(FunctionalState Cmd)
{
    /* Check the parameters */
    assert_param(IS_FUNCTIONAL_STATE(Cmd));
    *(__IO uint32_t*)CTRL_TP_EN_BB = (uint32_t)Cmd;
}

/**
 * @brief  Enables or disables the Tamper Pin Interrupt.
 * @param Cmd new state of the Tamper Pin Interrupt.
 *   This parameter can be: ENABLE or DISABLE.
 */
void BKP_TPIntEnable(FunctionalState Cmd)
{
    /* Check the parameters */
    assert_param(IS_FUNCTIONAL_STATE(Cmd));
    *(__IO uint32_t*)CTRLSTS_TPINT_EN_BB = (uint32_t)Cmd;
}

/**
 * @brief  Select the RTC output source to output on the Tamper pin.
 * @param BKP_RTCOutputSource specifies the RTC output source.
 *   This parameter can be one of the following values:
 *     @arg BKP_RTCOUTPUT_NONE no RTC output on the Tamper pin.
 *     @arg BKP_RTCOUTPUT_CALCLK output the RTC clock with frequency
 *                                          divided by 64 on the Tamper pin.
 *     @arg BKP_RTCOUTPUT_ALARM output the RTC Alarm pulse signal on
 *                                     the Tamper pin.
 *     @arg BKP_RTCOUTPUT_SECOND output the RTC Second pulse signal on
 *                                      the Tamper pin.
 */
void BKP_ConfigRtcOutput(uint16_t BKP_RTCOutputSource)
{
    uint16_t tmpregister = 0;
    /* Check the parameters */
    assert_param(IS_BKP_RTC_RTCOUTPUT(BKP_RTCOutputSource));
    tmpregister = BKP->RTCCTRL;
    /* Clear CALO, ASO_EN and ASO_SEL bits */
    tmpregister &= RTCCTRL_MASK;

    /* Set CALO, ASO_EN and ASO_SEL bits according to BKP_RTCOutputSource value */
    tmpregister |= BKP_RTCOutputSource;
    /* Store the new value */
    BKP->RTCCTRL = tmpregister;
}

/**
 * @brief  Sets RTC Clock Calibration value.
 * @param CalibrationValue specifies the RTC Clock Calibration value.
 *   This parameter must be a number between 0 and 0x7F.
 */
void BKP_SetRtcCalValue(uint8_t CalibrationValue)
{
    uint16_t tmpregister = 0;
    /* Check the parameters */
    assert_param(IS_BKP_CAL_VALUE(CalibrationValue));
    tmpregister = BKP->RTCCTRL;
    /* Clear CAL[6:0] bits */
    tmpregister &= RTCCTRL_CALV_MASK;
    /* Set CAL[6:0] bits according to CalibrationValue value */
    tmpregister |= CalibrationValue;
    /* Store the new value */
    BKP->RTCCTRL = tmpregister;
}

/**
 * @brief  Writes user data to the specified Data Backup Register.
 * @param BKP_DAT specifies the Data Backup Register.
 *   This parameter can be BKP_DATx where x:[1, 42]
 * @param Data data to write
 */
void BKP_WriteBkpData(uint16_t BKP_DAT, uint16_t Data)
{
    __IO uint32_t tmp = 0;

    /* Check the parameters */
    assert_param(IS_BKP_DAT(BKP_DAT));

    tmp = (uint32_t)BKP_BASE;
    tmp += BKP_DAT;

    *(__IO uint32_t*)tmp = Data;
}

/**
 * @brief  Reads data from the specified Data Backup Register.
 * @param BKP_DAT specifies the Data Backup Register.
 *   This parameter can be BKP_DATx where x:[1, 42]
 * @return The content of the specified Data Backup Register
 */
uint16_t BKP_ReadBkpData(uint16_t BKP_DAT)
{
    __IO uint32_t tmp = 0;

    /* Check the parameters */
    assert_param(IS_BKP_DAT(BKP_DAT));

    tmp = (uint32_t)BKP_BASE;
    tmp += BKP_DAT;

    return (*(__IO uint16_t*)tmp);
}

/**
 * @brief  Checks whether the Tamper Pin Event flag is set or not.
 * @return The new state of the Tamper Pin Event flag (SET or RESET).
 */
FlagStatus BKP_GetTEFlag(void)
{
    return (FlagStatus)(*(__IO uint32_t*)CTRLSTS_TEF_BB);
}

/**
 * @brief  Clears Tamper Pin Event pending flag.
 */
void BKP_ClrTEFlag(void)
{
    /* Set CTE bit to clear Tamper Pin Event flag */
    BKP->CTRLSTS |= BKP_CTRLSTS_CLRTE;
}

/**
 * @brief  Checks whether the Tamper Pin Interrupt has occurred or not.
 * @return The new state of the Tamper Pin Interrupt (SET or RESET).
 */
INTStatus BKP_GetTINTFlag(void)
{
    return (INTStatus)(*(__IO uint32_t*)CTRLSTS_TINTF_BB);
}

/**
 * @brief  Clears Tamper Pin Interrupt pending bit.
 */
void BKP_ClrTINTFlag(void)
{
    /* Set CTI bit to clear Tamper Pin Interrupt pending bit */
    BKP->CTRLSTS |= BKP_CTRLSTS_CLRTINT;
}

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */
