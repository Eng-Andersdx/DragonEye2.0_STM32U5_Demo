/**
  ******************************************************************************
  * @file    dragoneye_encoder.h
  * @author  Anders Engineering Team
  * @version 1.2
  * @brief   This file contains definitions for DragonEYE Encoder hardware 
  *          resources.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 Anders Electronics plc.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */



/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DRAGONEYE_ENCODER_H
#define __DRAGONEYE_ENCODER_H 0103 /* BSP ENCODER API REVISION v.1.3 */
#include "main.h"

#ifdef __cplusplus
	extern "C" {
#endif


 /* Includes ------------------------------------------------------------------*/

/** @defgroup DragonEYE_ENCODER_Exported_Types DragonEYE Encoder Exported Types
 * @{
 */

typedef enum
{
	PB_SET = 0,
	PB_RESET = !PB_SET
} ButtonValue_TypeDef;


/** @brief BSP_Status_TypeDef
  *  BSP Status return possible values.
 */
typedef enum
{
	BSP_OK    = 0,
	BSP_ERROR = 1
} BSP_Status_TypeDef;
/** @brief EncoderPushValue_TypeDef
  *  Anders DVK board Encoders Push Button definition.
  */
typedef enum 
{
	ENC_PB_RELEASED = 0,
	ENC_PB_PUSHED = !ENC_PB_RELEASED
	//ENC_PB_DOUBLE_CLICK
	//ENC_PB_LONG_PRESS
} EncoderPushValue_TypeDef;

/** @brief Button_TypeDef
  *  Anders DVK board Encoders GPIO definitions.
  */
typedef enum
{
	KEY_A = 0,
	KEY_B = 1,
	PUSH_BUTTON = 2
} Encoder_GPIO_TypeDef;


/** @brief EncoderMode_TypeDef
  *  Anders DVK board Encoder Modes definitions.
  */
typedef enum
{
	ENCODER_MODE_POLL = 0,
	ENCODER_MODE_EXTI = 1
} EncoderMode_TypeDef;

/**
  * encLatchMode_FOUR0 - Use this mode when both encoder output signals are always LOW in latch position.
  * encLatchMode_FOUR3 - Use this mode when both encoder output signals are always HIGH in latch position.
  * encLatchMode_TWO03 - Use this mode when both encoder output signals are LOW or HIGH in latch position.
  */
typedef enum
{
	encLatchMode_FOUR3 = 1,
	encLatchMode_FOUR0 = 2,
	encLatchMode_TWO03 = 3
} encLatchMode_t;


/** @brief ButtonMode_TypeDef
  *  Anders EVK board Buttons definitions.
  */
typedef enum
{
  BUTTON_MODE_GPIO = 0,
  BUTTON_MODE_EXTI = 1
} ButtonMode_TypeDef;



/** @brief Button_TypeDef
  *  Anders EVK board Buttons definitions.
  */
typedef enum
{
  BUTTON_LEFT = 0,
  BUTTON_RIGHT = 1,
  BUTTON_PUSH = 2
} Button_TypeDef;

/** @defgroup DragonEYE_ENCODER_Exported_Functions DragonEYE Encoder Exported Functions
  * @{
  */
void 						BSP_ENC_Init(EncoderMode_TypeDef Encoder_Mode);
void						BSP_ENC_ResetRotationValue(void);
signed long					BSP_ENC_GetRotationValue(void);
EncoderPushValue_TypeDef	BSP_ENC_GetPushBtnState(void);
void 						BSP_ENC_EncoderPoll(int iPinState_SignalA, int iPinState_SignalB);
uint32_t                    BSP_PB_GetState(Button_TypeDef Button);
void                        BSP_PB_Init(Button_TypeDef Button, ButtonMode_TypeDef Button_Mode);
void                        BSP_PB_DeInit(Button_TypeDef Button);
uint8_t                     BSP_ENC_GetAntiClockWiseRotation(void);
uint8_t 					BSP_ENC_GetClockWiseRotation(void);


#ifdef __cplusplus
}
#endif

#endif /* __DRAGONEYE_ENCODER_H */
