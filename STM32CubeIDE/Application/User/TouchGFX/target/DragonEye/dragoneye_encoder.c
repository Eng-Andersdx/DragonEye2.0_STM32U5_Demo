/**
  ******************************************************************************
  * @file    dragoneye_encoder.c
  * @author  Anders Engineering Team
  * @version 1.3
  * @brief   This file provides a set of functions needed to manage the Rotary
  *          Encoder on DragonEYE board.
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

/* File Info : -----------------------------------------------------------------
                                   User NOTES
1. Driver description:
----------------------
   - This driver is used to drive the Rotary Encoders module of the DragonEYE
     board. The enocder driver supported are the one of the ECxx01 series.

2. How To use this driver:
--------------------------
  + Initialization steps:
     o Initialize the Encoder module using the BSP_ENC_Init() function. This
       function performs hardware resources initialization and reset the
       internal encoder states and values. Initialization mode is passed as
	   parameter.
     o If interrupt mode is desired, you must pass the ENCODER_MODE_EXTI value
       while calling the BSP_ENC_Init() function. The internal interrupt
	   subroutine will be called whenever a rotation or a press of the push
	   button is detected.
       The interrupt mode internally update the rotation position value that
	   can be read at any time.

  + Rotary Encoder use
     o An absolute rotation position value is captured whenever the callback
       used. This function returns information about the last LCD touch occurred
       in the TS_StateTypeDef structure.
     o If TS interrupt mode is used, the function BSP_TS_ITGetStatus() is needed to get
       the interrupt status. To clear the IT pending bits, you should call the
       function BSP_TS_ITClear().
     o The IT is handled using the corresponding external interrupt IRQ handler,
       the user IT callback treatment is implemented on the same external interrupt
       callback.

------------------------------------------------------------------------------*/

/* Includes ------------------------------------------------------------------*/
#include "dragoneye_encoder.h"
#include "stm32u5xx.h"
#include "stdbool.h"

#if __DRAGONEYE_ENCODER_H != 0103 /* BSP ENCODER REVISION v.1.2 */
#error Wrong API version file (dragoneye_encoder.h).
#endif

/** @defgroup DragonEYE_ENCODER_Config_Defines DragonEYE Encoder Configuration Defines
  * @{
  */
	/* Specify the status of KeyA and Key B terminal on the dent position for the specific encoder used
	* If no #define is selected, then it is assumed both KeyA and KeyB are in High states on each dent position
	*/
	//#define LOW_STATE_DENTS
	//#define BOTH_STATE_DENTS
/**
  * @}
  */

/** @defgroup DragonEYE_ENCODER_Private_Defines DragonEYE Encoder Private Types Defines
  * @{
  */

#define ENCODER_KEYA_GPIO_CLK_ENABLE()			__HAL_RCC_GPIOB_CLK_ENABLE()
#define ENCODER_KEYA_GPIO_CLK_DISABLE()			__HAL_RCC_GPIOB_CLK_DISABLE()
#define ENCODER_KEYB_GPIO_CLK_ENABLE()			__HAL_RCC_GPIOB_CLK_ENABLE()
#define ENCODER_KEYB_GPIO_CLK_DISABLE()			__HAL_RCC_GPIOB_CLK_DISABLE()
#define ENCODER_PUSH_BUTTON_GPIO_CLK_ENABLE()	__HAL_RCC_GPIOE_CLK_ENABLE()
#define ENCODER_PUSH_BUTTON_GPIO_CLK_DISABLE()	__HAL_RCC_GPIOE_CLK_DISABLE()

#define LATCH0 0
#define LATCH3 3

GPIO_TypeDef* ENCODER_PORT[3] = {GPIOB,
								 GPIOB,
								 GPIOE};

const uint16_t ENCODER_PIN[3] = {GPIO_PIN_11,
								 GPIO_PIN_12,
								 GPIO_PIN_8};

const uint16_t ENCODER_IRQn[3] = {EXTI11_IRQn,
							  	  EXTI12_IRQn,
								  EXTI8_IRQn};

#ifdef LOW_STATE_DENTS

#define RISING_EDGE 0
#define	FALLING_EDGE 1

#else

#define FALLING_EDGE 0
#define	RISING_EDGE 1

#endif

/**
  * This array stores the encoder position entries.
  * 1 and -1 indicates the increment and decrement respectively in encoder position.
  * 0 indicates no change in encoder position.
  */
const int8_t ci8TWO03Dir[] = {
	0, -1, 1, 0,
	1, 0, 0, -1,
	-1, 0, 0, 1,
	0, 1, -1, 0
};

const int8_t ci8FOUR0Dir[] = {
	0, 1, -1, 0,
	-1, 0, 0, 1,
	1, 0, 0, -1,
	0, -1, 1, 0
};

encLatchMode_t xEncoderLatchMode = encLatchMode_TWO03;
volatile uint8_t	vui8EncoderState_Prev;        // old internal position
volatile long		vlEncoderPosition_Int;        // internal position, 4 times position_ext
//Actual Encoder States
signed long			lEncoderPosition = 0;
static uint8_t             clockwise_counter=0;
static uint8_t             anti_clockwise_counter=0;
static uint8_t             push_counter=0;
uint8_t				ui8EncoderPushed = 0;

volatile int Activate_Buzzer_Duration = 0;

static uint8_t lastABState = 0;

extern volatile uint8_t Touch_Event_Flag;
/**
  * @brief  Configures Encoder GPIO and EXTI Line.
  */
void BSP_ENC_Init(EncoderMode_TypeDef Encoder_Mode)
{
	//Reset all internal counters
	vui8EncoderState_Prev = 0;
	vlEncoderPosition_Int = 0;
	lEncoderPosition = 0;
	ui8EncoderPushed = 0;
}

/**
  * @brief  Configures button GPIO and EXTI Line.
  * @param  Button: Button to be configured
  *          This parameter can be one of the following values:
  *            @arg  BUTTON_WAKEUP: Wakeup Push Button
  *            @arg  BUTTON_USER: User Push Button
  * @param  Button_Mode: Button mode
  *          This parameter can be one of the following values:
  *            @arg  BUTTON_MODE_GPIO: Button will be used as simple IO
  *            @arg  BUTTON_MODE_EXTI: Button will be connected to EXTI line
  *                                    with interrupt generation capability
  */
void BSP_PB_Init(Button_TypeDef Button, ButtonMode_TypeDef Button_Mode)
{
	if(Button_Mode==BUTTON_MODE_EXTI){
		BSP_ENC_Init(ENCODER_MODE_EXTI);
	} else {
		// Hardcoded option for now...
		BSP_ENC_Init(ENCODER_MODE_EXTI);
	}
}

/**
  * @brief  Push Button DeInit.
  * @param  Button: Button to be configured
  *          This parameter can be one of the following values:
  *            @arg  BUTTON_WAKEUP: Wakeup Push Button
  *            @arg  BUTTON_USER: User Push Button
  * @note PB DeInit does not disable the GPIO clock
  */
void BSP_PB_DeInit(Button_TypeDef Button)
{
	//TBD
}




/**
  * @brief  This function resets the encoder position counts.
  * @param  None
  * @retval None
  */
void BSP_ENC_ResetRotationValue(void)
{
	vui8EncoderState_Prev = 0;
	vlEncoderPosition_Int = 0;
	lEncoderPosition = 0;
	ui8EncoderPushed = 0;
}

/**
 * @brief  This function report encoder rotation position
 * @param  None
 * @retval The Encoder absolute rotation value (integer limited)
 */
signed long BSP_ENC_GetRotationValue(void)
{
	return lEncoderPosition;
}

/**
 * @brief  This function report the number of clockwise rotations of the encoder
 * @param  None
 * @retval The clockwise_counter
 */
uint8_t BSP_ENC_GetClockWiseRotation(void){
	return clockwise_counter;
}

/**
 * @brief  This function report the number of pushes in the encoder
 * @param  None
 * @retval The push_counter
 */
uint8_t BSP_ENC_GetPushCounter(void){
	return push_counter;
}

/**
 * @brief  This function report the number of anti clockwise rotations of the encoder
 * @param  None
 * @retval The anti_clockwise_counter
 */
uint8_t BSP_ENC_GetAntiClockWiseRotation(void){
	return anti_clockwise_counter;
}

/**
  * @brief  Returns the Push button state at any time.
  * @param  None
  * @retval The Button GPIO pin value
  */
EncoderPushValue_TypeDef BSP_ENC_GetPushBtnState(void)
{
	if(ui8EncoderPushed){
		ui8EncoderPushed = 0;
		return ENC_PB_PUSHED;
	}
	return ENC_PB_RELEASED;
}





/**
 * @brief  Returns the selected button state.
 * @param  Button: Button to be checked
 *          This parameter can be one of the following values:
 *            @arg  BUTTON_WAKEUP: Wakeup Push Button
 *            @arg  BUTTON_USER: User Push Button
 * @retval The Button GPIO pin value
 */
uint32_t BSP_PB_GetState(Button_TypeDef Button){

	switch(Button){
	case BUTTON_LEFT:
		if(anti_clockwise_counter){
			anti_clockwise_counter--;
			return PB_SET;
		}
		else{
			return PB_RESET;
		}
		break;
	case BUTTON_RIGHT:
		if(clockwise_counter){
			clockwise_counter--;
			return PB_SET;
		}
		else{
			return PB_RESET;
		}
		break;
	case BUTTON_PUSH:
		if(ui8EncoderPushed==ENC_PB_PUSHED){
				return PB_SET;
		}
		else if(push_counter){
			push_counter--;
			return PB_SET;
		}
		else{
			return PB_RESET;
		}
		break;
	default:
		return PB_RESET;
	}


}



/**
  * @brief  This function reads the encoder pins, either it is high or low.
  *         Based on both pin state, it calculates the encoder position.
  * @param  iPinState_SignalA: encoder output pin a state
  *         iPinState_SignalB: encoder output pin b state
  * @retval None
  */
void BSP_ENC_EncoderPoll(int iPinState_SignalA, int iPinState_SignalB)
{
	/* calculate the current position */
	int8_t i8EncoderState_Current = iPinState_SignalA | (iPinState_SignalB << 1);
	ui8EncoderPushed = HAL_GPIO_ReadPin(ENCODER_PORT[PUSH_BUTTON], ENCODER_PIN[PUSH_BUTTON]);

	/* check if encoder position is changed or not */
	if (vui8EncoderState_Prev != i8EncoderState_Current)
	{

		/* check encoder mode */
		switch (xEncoderLatchMode)
		{
			case encLatchMode_FOUR3:
			case encLatchMode_FOUR0:
				vlEncoderPosition_Int += ci8FOUR0Dir[i8EncoderState_Current | (vui8EncoderState_Prev << 2)];
				vui8EncoderState_Prev = i8EncoderState_Current;
				if ((i8EncoderState_Current == LATCH3) || (i8EncoderState_Current == LATCH0))
				{
					lEncoderPosition = vlEncoderPosition_Int >> 2;
				}
				break;
			case encLatchMode_TWO03:
				vlEncoderPosition_Int += ci8TWO03Dir[i8EncoderState_Current | (vui8EncoderState_Prev << 2)];
				vui8EncoderState_Prev = i8EncoderState_Current;
				if ((i8EncoderState_Current == LATCH3) || (i8EncoderState_Current == LATCH0))
				{
					lEncoderPosition = vlEncoderPosition_Int >> 1;
				}
				break;
		}
	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin==ENCODER_PIN[PUSH_BUTTON]) {
		Touch_Event_Flag = 1;
	}

	if ((GPIO_Pin==ENCODER_PIN[KEY_A]) || (GPIO_Pin==ENCODER_PIN[KEY_B]))
	{
		static bool valid =true;
		uint8_t A = HAL_GPIO_ReadPin(ENCODER_PORT[KEY_A], ENCODER_PIN[KEY_A]) ? 1 : 0;
		uint8_t B = HAL_GPIO_ReadPin(ENCODER_PORT[KEY_B], ENCODER_PIN[KEY_B]) ? 1 : 0;

		uint8_t currentState = (A << 1) | B;
		uint8_t transition = (lastABState << 2) | currentState;

		// Decode rotation direction
		if(valid){
			switch (transition)
			{
				case 0b0100:	lEncoderPosition--;		anti_clockwise_counter++; valid=!valid;	break;
				case 0b0111:    lEncoderPosition++;		clockwise_counter++; 	valid=!valid;	break;
				case 0b1000:	lEncoderPosition++;		clockwise_counter++; 	valid=!valid;	break;
				case 0b1011:	lEncoderPosition--;		anti_clockwise_counter++; valid=!valid;	break;
				default:		break;
			}
		}else {
			valid=!valid;
		}
		lastABState = currentState;
	}
}


/* encoder interrupt callback */
void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin){
	HAL_GPIO_EXTI_Callback(GPIO_Pin);
}

void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin){
	HAL_GPIO_EXTI_Callback(GPIO_Pin);
}

