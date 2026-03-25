/**
  ******************************************************************************
  * @file    dragoneye_leds.c
  * @author  Anders Engineering Team
  * @version 1.6
  * @brief   This file provides a set of functions needed to manage the Rotary
  *          LEDs on DragonEYE board.
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

/******************************************************************************
 * Change History Table
 * -----------------------------------------------------------------------------
 * Version | Date       | Author       | Description
 * -----------------------------------------------------------------------------
 * 1.5     | 2025-10-07 | H.Hunain  | Added driver for AWINIC_AW21036
 * -----------------------------------------------------------------------------
 * 1.6     | 2026-01-08 | R.Gopan   | Added driver for DragonEye_G0 & Improved the LED Brightness for Ring Light
 * -----------------------------------------------------------------------------
 * Notes:
 * - Always increment version when making manual changes.
 * - Describe clearly what was changed in each version.
 ******************************************************************************/

/* File Info : -----------------------------------------------------------------
                                   User NOTES
1. Driver description:
----------------------
   - This driver is used to drive the LEDs on the Rotary modules of the DragonEYE
     board. The Leds supported are controllable in pwm mode as much as simple
	 toggling mode. Display Backlight is treated the same way as all other LEDs

2. How To use this driver:
--------------------------
  + Initialization steps:
     o Initialize the LEDs module using the BSP_LED_Init(Led) function.
	   This function performs hardware resources initialization and set the
       internal leds off by default.
	   Backlight can be initialised by passing Led = BACK_LIGHT.

  +  Rotary LEDs on the use
     o Any of the available LED defined in the Led_TypeDef state can be changed
	   on or off by calling either one of the BSP_LED_On(Led) or BSP_LED_Off(Led).
	   Backlight status can be changed by passing Led = BACK_LIGHT.
     o Any LED can be simply toggled regardless of the previous state by calling
	   BSP_LED_Toggle(Led).
	   Backlight can be toggled by passing Led = BACK_LIGHT.

------------------------------------------------------------------------------*/

/* Includes ------------------------------------------------------------------*/
#include "assert.h"
#include "main.h"
#include "bsp.h"

#ifdef AWINIC_AW21036
#include <ventura_leds.h>
#endif

#ifdef PWM_LED
#include "dragoneye_leds.h"
#endif

#ifdef DragonEyeG0
#include <DE_Lite/dragoneye_leds.h>
#endif


#if __DRAGONEYE_LEDS_H != 0106	/* BSP LEDS REVISION v.1.6 */
#error Wrong API version file (dragoneye_leds.h).
#endif


/** @addtogroup BSP
  * @{
  */

/** @addtogroup DragonEYE
  * @{
  */

/** @defgroup DragonEYE_LEDS DragonEYE LEDs
  * @{
  */


/** @defgroup DragonEYE_LEDS_Private_Variables DragonEYE LEDs Private Variables
  * @{
  */
uint32_t GPIO_PIN[LEDn] = {DSI_BL_CTRL_Pin,  //Backlight
						   R_LED_GPIO_Pin,  //Red
						   G_LED_GPIO_Pin,  //Green
						   B_LED_GPIO_Pin}; //Blue

GPIO_TypeDef* GPIO_PORT[LEDn] = {DSI_BL_CTRL_GPIO_Port,
								 R_LED_GPIO_GPIO_Port,
								 G_LED_GPIO_GPIO_Port,
								 B_LED_GPIO_GPIO_Port};






static TIM_HandleTypeDef *tim_handle=NULL, *bl_handle=NULL;  // Static global variable for internal use only
static uint32_t r_channel, g_channel, b_channel, bl_channel;

/**
  * @brief  Configures LED GPIO.
  * @param  Led: LED to be configured.
  *          This parameter can be one of the following values:
  *            @arg  BACK_LIGHT
  *            @arg  LED_RED
  *            @arg  LED_GREEN
  *            @arg  LED_BLUE
  */
void BSP_LED_Init(Led_TypeDef Led)
{
  GPIO_InitTypeDef  gpio_init_structure;

  if (Led <= LEDn)
  {
    /* Configure the GPIO_LED pin */
    gpio_init_structure.Pin   = GPIO_PIN[Led];
    gpio_init_structure.Mode  = GPIO_MODE_OUTPUT_PP;
    gpio_init_structure.Pull  = GPIO_PULLUP;
    gpio_init_structure.Speed = GPIO_SPEED_HIGH;

    __HAL_RCC_GPIOA_CLK_ENABLE();
    #ifdef PWM_LED
        __HAL_RCC_GPIOB_CLK_ENABLE();
        __HAL_RCC_GPIOC_CLK_ENABLE();
        __HAL_RCC_GPIOD_CLK_ENABLE();
        __HAL_RCC_GPIOE_CLK_ENABLE();
        __HAL_RCC_GPIOF_CLK_ENABLE();
        __HAL_RCC_GPIOG_CLK_ENABLE();
        __HAL_RCC_GPIOH_CLK_ENABLE();
    #endif


    HAL_GPIO_Init(GPIO_PORT[Led], &gpio_init_structure);

    /* By default, turn off LED by setting a high level on corresponding GPIO */
    HAL_GPIO_WritePin(GPIO_PORT[Led], GPIO_PIN[Led], GPIO_PIN_RESET);

  } /* of if (Led <= LED_BLUE) */

}

/**
  * @}
  */

/** @defgroup DragonEYE_LEDS_Private_Function_Prototypes DragonEYE LEDs Private Function Prototypes
  * @{
  */

/**
  * @}
  */

/** @defgroup DragonEYE_LEDS_Public_Functions DragonEYE LEDs Public Functions
  * @{
  */
/**
  * @brief  Initialise PWM Channels for LCD Backlight.
  */
void BSP_BL_PWM_Init(TIM_HandleTypeDef *htim, uint32_t BL_TIM_CH){
	bl_handle = htim;
	bl_channel = BL_TIM_CH;
	HAL_TIM_PWM_Start(bl_handle, bl_channel); //backlight pwm
}

/**
  * @brief  Set the Custom Intensity for the LCD Backlight.
  * @param  uin8_t: DutyCycle for the Backlight PWM Channel which should be between 0-100.
  *         	@arg dutycycle
  */
void BSP_BL_SetIntensity(uint8_t dutycycle){
	assert(dutycycle>=0 && dutycycle<=100&&"The duty cycle should be between 0-100");
	__HAL_TIM_SET_COMPARE(bl_handle, bl_channel, dutycycle*bl_handle->Init.Period/100);
}




/**
  * @brief  Initialise PWM Channels for RGB LEDs.
  */
void BSP_LED_PWM_Init(TIM_HandleTypeDef *htim, uint32_t R_TIM_CH, uint32_t G_TIM_CH, uint32_t B_TIM_CH){
	tim_handle = htim;
	r_channel = R_TIM_CH;
	g_channel = G_TIM_CH;
	b_channel = B_TIM_CH;
	HAL_TIM_PWM_Start(tim_handle, r_channel); //red
	HAL_TIM_PWM_Start(tim_handle, g_channel); //Green
	HAL_TIM_PWM_Start(tim_handle, b_channel); //Blue
}

/**
  * @brief  Set the Intensity for the 3 LED's based on colour.
  * @param  uin32_t: Respective DutyCycles for the 3 PWM Channels which should be between 0-100.
  *         There is only 1 Argument as follows:
  *            @arg 1  color
  *            which has duty cycles of R,G,B respectively
  */
void BSP_LED_SetColor(uint32_t color)
{
    uint8_t red_dutycycle = (color & 0xFF);
    uint8_t green_dutycycle = ((color >> 8) & 0xFF);
    uint8_t blue_dutycycle = ((color >> 16) & 0xFF);
    BSP_LED_SetRGBIntensity(red_dutycycle, green_dutycycle, blue_dutycycle);
}

/**
  * @brief  Set the Custom Intensity for the 3 LED's together.
  * @param  uin8_t: Respective DutyCycle for the 3 PWM Channels which should be between 0-100.
  *         There are 3 Arguments as follows:
  *            @arg 1  red_dutycycle
  *            @arg 2   green_dutycycle
  *            @arg 3   blue_dutycycle
  */
void BSP_LED_SetRGBIntensity(uint8_t red_dutycycle, uint8_t green_dutycycle, uint8_t blue_dutycycle){
	assert(red_dutycycle>=0 && red_dutycycle<=100 && green_dutycycle>=0 && green_dutycycle<=100 && blue_dutycycle>=0 && blue_dutycycle<=100 && "The duty cycle should be between 0-100");
	__HAL_TIM_SET_COMPARE(tim_handle, r_channel, red_dutycycle*tim_handle->Init.Period/100);
	__HAL_TIM_SET_COMPARE(tim_handle, g_channel, (green_dutycycle*tim_handle->Init.Period/100)*0.40);
	__HAL_TIM_SET_COMPARE(tim_handle, b_channel, (blue_dutycycle*tim_handle->Init.Period/100)*0.5);
}

/**
  * @brief  Set the Custom Intensity for the Red LED.
  * @param  uin8_t: DutyCycle for the Red PWM Channel which should be between 0-100.
  *         	@arg dutycycle
  */
void BSP_LED_SetRedIntensity(uint8_t dutycycle){
	assert(dutycycle>=0 && dutycycle<=100&&"The duty cycle should be between 0-100");
	__HAL_TIM_SET_COMPARE(tim_handle, r_channel, dutycycle*tim_handle->Init.Period/100);
}

/**
  * @brief  Set the Custom Intensity for the Green LED.
  * @param  uin8_t: DutyCycle for the Green PWM Channel which should be between 0-100.
  *         	@arg dutycycle
  */
void BSP_LED_SetGreenIntensity(uint8_t dutycycle){
	assert(dutycycle>=0 && dutycycle<=100&&"The duty cycle should be between 0-100");
	__HAL_TIM_SET_COMPARE(tim_handle, g_channel, (dutycycle*tim_handle->Init.Period/100)*0.40);
}

/**
  * @brief  Set the Custom Intensity for the Blue LED.
  * @param  uin8_t: DutyCycle for the Blue PWM Channel which should be between 0-100.
  *         	@arg dutycycle
  */
void BSP_LED_SetBlueIntensity(uint8_t dutycycle){
	assert(dutycycle>=0 && dutycycle<=100&&"The duty cycle should be between 0-100");
	__HAL_TIM_SET_COMPARE(tim_handle, b_channel, (dutycycle*tim_handle->Init.Period/100)*0.5);
}

//--------------- BASIC LEDs Switching Function if no PWM available -------------------//

/**
  * @brief  DeInit LEDs.
  * @param  Led: LED to be configured.
  *          This parameter can be one of the following values:
  *            @arg  BACK_LIGHT
  *            @arg  LED_RED
  *            @arg  LED_GREEN
  *            @arg  LED_BLUE
  * @note Led DeInit does not disable the GPIO clock nor disable the Mfx
  */
void BSP_LED_DeInit(Led_TypeDef Led)
{
  GPIO_InitTypeDef  gpio_init_structure;

  if (Led <= LEDn)
  {
    /* DeInit the GPIO_LED pin */
    gpio_init_structure.Pin = GPIO_PIN[Led];

    /* Turn off LED */
    HAL_GPIO_WritePin(GPIO_PORT[Led], GPIO_PIN[Led], GPIO_PIN_RESET);
    HAL_GPIO_DeInit(GPIO_PORT[Led], gpio_init_structure.Pin);
  }

}

/**
  * @brief  Turns selected LED On.
  * @param  Led: LED to be set on
  *          This parameter can be one of the following values:
  *            @arg  BACK_LIGHT
  *            @arg  LED_RED
  *            @arg  LED_GREEN
  *            @arg  LED_BLUE
  */
void BSP_LED_On(Led_TypeDef Led)
{
  if (Led <= LEDn)
  {
     HAL_GPIO_WritePin(GPIO_PORT[Led], GPIO_PIN[Led], GPIO_PIN_SET);
  }

}

/**
  * @brief  Turns selected LED Off.
  * @param  Led: LED to be set off
  *          This parameter can be one of the following values:
  *            @arg  BACK_LIGHT
  *            @arg  LED_RED
  *            @arg  LED_GREEN
  *            @arg  LED_BLUE
  */
void BSP_LED_Off(Led_TypeDef Led)
{
  if (Led <= LEDn)
  {
    HAL_GPIO_WritePin(GPIO_PORT[Led], GPIO_PIN[Led], GPIO_PIN_RESET);
  }
}

/**
  * @brief  Toggles the selected LED.
  * @param  Led: LED to be toggled
  *          This parameter can be one of the following values:
  *            @arg  BACK_LIGHT
  *            @arg  LED_RED
  *            @arg  LED_GREEN
  *            @arg  LED_BLUE
  */
void BSP_LED_Toggle(Led_TypeDef Led)
{
  if (Led <= LEDn)
  {
     HAL_GPIO_TogglePin(GPIO_PORT[Led], GPIO_PIN[Led]);
  }
}


#ifdef AWINIC_AW21036

I2C_HandleTypeDef *hi2c_handler;
static TIM_HandleTypeDef *bl_handle=NULL;  // Static global variable for internal use only
static uint32_t bl_channel;
int SHOW_LED_PATTERN = 0;
uint32_t counter = 1;


LED_RET_TypeDef BSP_ALL_LED_Init(I2C_HandleTypeDef *i2c_handler){
	hi2c_handler = i2c_handler;
	if(hi2c_handler == NULL){
		return LED_ERROR;
	}

	HAL_GPIO_WritePin(RGB_LED_Power_EN_GPIO_Port, RGB_LED_Power_EN_Pin, GPIO_PIN_SET);

	uint8_t reset_value = 0x00;
	uint8_t set_gcr = 0x01;	// Enable chip (GCR bit0 = 1)
	uint8_t set_gccr = 0xFF;	// Maximum Current

	//For chip 1
	if (HAL_I2C_Mem_Write(hi2c_handler, AW21036_I2C_ADDR_GND, AW21036_REG_RESET, I2C_MEMADD_SIZE_8BIT, &reset_value, 1, HAL_MAX_DELAY) != HAL_OK){
		return LED_ERROR;
	}


	if (HAL_I2C_Mem_Write(hi2c_handler, AW21036_I2C_ADDR_GND, AW21036_REG_GCR, I2C_MEMADD_SIZE_8BIT, &set_gcr, 1, HAL_MAX_DELAY) != HAL_OK){
		return LED_ERROR;
	}

	if (HAL_I2C_Mem_Write(hi2c_handler, AW21036_I2C_ADDR_GND, AW21036_REG_GCCR, I2C_MEMADD_SIZE_8BIT, &set_gccr, 1, HAL_MAX_DELAY) != HAL_OK){
		return LED_ERROR;
	}

	//For chip 2
	if (HAL_I2C_Mem_Write(hi2c_handler, AW21036_I2C_ADDR_VDD, AW21036_REG_RESET, I2C_MEMADD_SIZE_8BIT, &reset_value, 1, HAL_MAX_DELAY) != HAL_OK){
		return LED_ERROR;
	}


	if (HAL_I2C_Mem_Write(hi2c_handler, AW21036_I2C_ADDR_VDD, AW21036_REG_GCR, I2C_MEMADD_SIZE_8BIT, &set_gcr, 1, HAL_MAX_DELAY) != HAL_OK){
		return LED_ERROR;
	}
	if (HAL_I2C_Mem_Write(hi2c_handler, AW21036_I2C_ADDR_VDD, AW21036_REG_GCCR, I2C_MEMADD_SIZE_8BIT, &set_gccr, 1, HAL_MAX_DELAY) != HAL_OK){
		return LED_ERROR;
	}

	//For chip 3
	if (HAL_I2C_Mem_Write(hi2c_handler, AW21036_I2C_ADDR_SDA, AW21036_REG_RESET, I2C_MEMADD_SIZE_8BIT, &reset_value, 1, HAL_MAX_DELAY) != HAL_OK){
		return LED_ERROR;
	}


	if (HAL_I2C_Mem_Write(hi2c_handler, AW21036_I2C_ADDR_SDA, AW21036_REG_GCR, I2C_MEMADD_SIZE_8BIT, &set_gcr, 1, HAL_MAX_DELAY) != HAL_OK){
		return LED_ERROR;
	}
	if (HAL_I2C_Mem_Write(hi2c_handler, AW21036_I2C_ADDR_SDA, AW21036_REG_GCCR, I2C_MEMADD_SIZE_8BIT, &set_gccr, 1, HAL_MAX_DELAY) != HAL_OK){
		return LED_ERROR;
	}
	    return LED_OK;
}

LED_RET_TypeDef BSP_ALL_LED_DeInit(I2C_HandleTypeDef *i2c_handler){

	uint8_t reset_gcr = 0x00;

	//For Chip 1
	if (HAL_I2C_Mem_Write(hi2c_handler, AW21036_I2C_ADDR_GND, AW21036_REG_GCR, I2C_MEMADD_SIZE_8BIT, &reset_gcr, 1, HAL_MAX_DELAY) != HAL_OK){
		return LED_ERROR;
	}

	//For Chip 2
	if (HAL_I2C_Mem_Write(hi2c_handler, AW21036_I2C_ADDR_VDD, AW21036_REG_GCR, I2C_MEMADD_SIZE_8BIT, &reset_gcr, 1, HAL_MAX_DELAY) != HAL_OK){
		return LED_ERROR;
	}

	//For Chip 3
	if (HAL_I2C_Mem_Write(hi2c_handler, AW21036_I2C_ADDR_SDA, AW21036_REG_GCR, I2C_MEMADD_SIZE_8BIT, &reset_gcr, 1, HAL_MAX_DELAY) != HAL_OK){
		return LED_ERROR;
	}

	HAL_GPIO_WritePin(RGB_LED_Power_EN_GPIO_Port, RGB_LED_Power_EN_Pin, GPIO_PIN_RESET);
	return LED_OK;
}

LED_RET_TypeDef BSP_ALL_LED_CONTROL(struct LED_State_t* led){
    if (led->R > 255 || led->G > 255 || led->B > 255 || led->Brightness_R > 255 || led->Brightness_G > 255 || led->Brightness_B > 255) {
        return LED_ERROR; // Invalid
    }

    uint8_t br[3];
	uint8_t col[3];

	br[0]  = led->Brightness_R;
	br[1]  = led->Brightness_G;
	br[2]  = led->Brightness_B;

	col[0] = led->R;
	col[1] = led->G;
	col[2] = led->B;

	uint8_t DEV_ADDR;
	uint8_t dev_index;
	uint8_t br_reg;
	uint8_t col_reg;

	for (uint8_t i = 0; i < 32; i++) {
		if (i <= 11) {
			DEV_ADDR = AW21036_I2C_ADDR_GND;
			dev_index = i;
		} else if (i <= 23) {
	        DEV_ADDR = AW21036_I2C_ADDR_VDD;
	        dev_index = i - 12;
	    } else {
	        DEV_ADDR = AW21036_I2C_ADDR_SDA;
	        dev_index = i - 24;
	    }


		br_reg  = AW21036_REG_BR0  + (dev_index * 3);
		col_reg = AW21036_REG_COL0 + (dev_index * 3);

		if (HAL_I2C_Mem_Write(hi2c_handler, DEV_ADDR, br_reg, I2C_MEMADD_SIZE_8BIT, br, 3, HAL_MAX_DELAY) != HAL_OK) {
			return LED_ERROR;
		}

		if (HAL_I2C_Mem_Write(hi2c_handler, DEV_ADDR, col_reg, I2C_MEMADD_SIZE_8BIT, col, 3, HAL_MAX_DELAY) != HAL_OK) {
			return LED_ERROR;
		}
	}

	uint8_t update = 0x00;

	uint8_t dev_addrs[] = {AW21036_I2C_ADDR_GND, AW21036_I2C_ADDR_VDD, AW21036_I2C_ADDR_SDA};
	for (uint8_t j = 0; j < 3; j++) {
		if (HAL_I2C_Mem_Write(hi2c_handler, dev_addrs[j], AW21036_REG_UPDATE, I2C_MEMADD_SIZE_8BIT, &update, 1, HAL_MAX_DELAY) != HAL_OK) {
			return LED_ERROR;
	    }
	}

	return LED_OK;
}

LED_RET_TypeDef BSP_ONE_LED_CONTROL(struct LED_State_t* led,uint8_t led_index){
	if(led_index > 32 || led->R > 255 || led->G > 255 || led->B > 255 || led->Brightness_R > 255 || led->Brightness_G > 255 || led->Brightness_B > 255){
		return LED_ERROR;
	}
	uint8_t DEV_ADDR;
	uint8_t Dev_Index;
	if( 0 <= led_index && led_index <= 11 ){
		DEV_ADDR = AW21036_I2C_ADDR_GND;
		Dev_Index = led_index;
	} else if (12 <= led_index && led_index <= 23 ){
		DEV_ADDR = AW21036_I2C_ADDR_VDD;
		Dev_Index = led_index - 12;
	} else if (24 <= led_index && led_index <= 31 ){
		DEV_ADDR = AW21036_I2C_ADDR_SDA;
		Dev_Index = led_index - 24;
	} else {
		return LED_ERROR;
	}
	 uint8_t br[3];
	 uint8_t col[3];

	 br[0]  = led->Brightness_R;
	 br[1]  = led->Brightness_G;
	 br[2]  = led->Brightness_B;

	 col[0] = led->R;
	 col[1] = led->G;
	 col[2] = led->B;

	 uint8_t br_reg  = AW21036_REG_BR0  + (Dev_Index * 3);
	 uint8_t col_reg = AW21036_REG_COL0 + (Dev_Index * 3);


	 if (HAL_I2C_Mem_Write(hi2c_handler, DEV_ADDR, br_reg, I2C_MEMADD_SIZE_8BIT, br, 3, HAL_MAX_DELAY) != HAL_OK){
	     return LED_ERROR;
	 }

	 if (HAL_I2C_Mem_Write(hi2c_handler, DEV_ADDR, col_reg, I2C_MEMADD_SIZE_8BIT, col, 3, HAL_MAX_DELAY) != HAL_OK){
	     return LED_ERROR;
	 }

	 uint8_t update = 0x00;
	 if (HAL_I2C_Mem_Write(hi2c_handler, DEV_ADDR, AW21036_REG_UPDATE, I2C_MEMADD_SIZE_8BIT, &update, 1, HAL_MAX_DELAY) != HAL_OK){
		 return LED_ERROR;
	 }
	 return LED_OK;
}

//Length of array should be 32
LED_RET_TypeDef BSP_INDIVIDUAL_LED_CONTROL(struct LED_State_t* ledArray){

	uint8_t DEV_ADDR;
	uint8_t br[3];
	uint8_t col[3];
	uint8_t br_reg;
	uint8_t col_reg;
	uint8_t update = 0x00;
	uint8_t dev_index;

	for (size_t i = 0; i <= 31; i++) {
		if (ledArray[i].Brightness_R > 255 || ledArray[i].Brightness_G > 255 || ledArray[i].Brightness_B > 255 || ledArray[i].R > 255 || ledArray[i].G > 255 || ledArray[i].B > 255 ) {
			return LED_ERROR; // Invalid
		}


		if (i <= 11) {
			DEV_ADDR = AW21036_I2C_ADDR_GND;
			dev_index = i;
		} else if (i <= 23) {
			DEV_ADDR = AW21036_I2C_ADDR_VDD;
			dev_index = i-12;
		} else {
			DEV_ADDR = AW21036_I2C_ADDR_SDA;
			dev_index = i-24;
		}

		br[0]  = ledArray[i].Brightness_R;
		br[1]  = ledArray[i].Brightness_G;
		br[2]  = ledArray[i].Brightness_B;

		col[0] = ledArray[i].R;
		col[1] = ledArray[i].G;
		col[2] = ledArray[i].B;

		br_reg  = AW21036_REG_BR0  + (dev_index * 3);
		col_reg = AW21036_REG_COL0 + (dev_index * 3);


		if (HAL_I2C_Mem_Write(hi2c_handler, DEV_ADDR, br_reg, I2C_MEMADD_SIZE_8BIT, br, 3, HAL_MAX_DELAY) != HAL_OK) {
			return LED_ERROR;
		}

		if (HAL_I2C_Mem_Write(hi2c_handler, DEV_ADDR, col_reg, I2C_MEMADD_SIZE_8BIT, col, 3, HAL_MAX_DELAY) != HAL_OK) {
			return LED_ERROR;
		}
	}

	// Update all devices
	uint8_t dev_addrs[] = {AW21036_I2C_ADDR_GND, AW21036_I2C_ADDR_VDD, AW21036_I2C_ADDR_SDA};

	for (uint8_t j = 0; j < 3; j++) {
		if (HAL_I2C_Mem_Write(hi2c_handler, dev_addrs[j], AW21036_REG_UPDATE, I2C_MEMADD_SIZE_8BIT, &update, 1, HAL_MAX_DELAY) != HAL_OK) {
	    	return LED_ERROR;
		}
	}

	return LED_OK;
}

LED_RET_TypeDef BSP_ALL_LED_Fade(struct LED_State_t *start, struct LED_State_t *end, uint16_t steps, uint16_t delayMs)
{
    struct LED_State_t current;

    for (uint16_t i = 0; i <= steps; i++)
    {
        // Linear interpolation for each field
        current.R  = start->R  + ((end->R  - start->R)  * i) / steps;
        current.G  = start->G  + ((end->G  - start->G)  * i) / steps;
        current.B  = start->B  + ((end->B  - start->B)  * i) / steps;

        current.Brightness_R = start->Brightness_R + ((end->Brightness_R - start->Brightness_R) * i) / steps;
        current.Brightness_G = start->Brightness_G + ((end->Brightness_G - start->Brightness_G) * i) / steps;
        current.Brightness_B = start->Brightness_B + ((end->Brightness_B - start->Brightness_B) * i) / steps;

        BSP_ALL_LED_CONTROL(&current);   // Push to hardware

        HAL_Delay(delayMs); // Adjust delay based on your RTOS / HAL
    }
    return LED_OK;
}

LED_RET_TypeDef BSP_RANDOM_ALL_LED_PATTERN()
{
	SHOW_LED_PATTERN = 0;
	counter++;
	uint32_t rnd = counter * 2654435761; // Knuth’s multiplicative hash
	int random_number = (rnd % 3) + 1;             // Map to 1-3

	switch(random_number){
	case 1: BSP_ALL_LED_ON(); break;
	case 2: BSP_LED_RUNNING_CW(); break;
	case 3: BSP_Multicolor_circular_motion(); break;
	default:BSP_ALL_LED_ON(); break;
	}
	return LED_OK;
}

LED_RET_TypeDef BSP_LED_RUNNING_CW(){
	  struct LED_State_t ledArray[32];	//Compulsory Length should be 32 //To control Individual LEDs and set different color and brightness to each

	  for (int i = 0; i <= 31; i++) {
	      // Calculate increment: evenly distributed from 0 to 255
		  ledArray[i].G = (uint8_t)((255 * i) / (32 - 1));
		  ledArray[i].Brightness_G = ledArray[i].G;

	      // Set other values to 0
		  ledArray[i].R = 0;
		  ledArray[i].B = 0;
		  ledArray[i].Brightness_R = 0;
		  ledArray[i].Brightness_B = 0;
	  }

	  struct LED_State_t temp;
	  while(1){
		  temp = ledArray[31];
		  // Shift all elements one step to the right
		  for (int i = 32 - 1; i > 0; i--) {
			  ledArray[i] = ledArray[i - 1];
		  }
		  // Place the last element at the first position
		  ledArray[0] = temp;
		  HAL_Delay(5);
		  BSP_INDIVIDUAL_LED_CONTROL(ledArray); //Length of array should be exactly 32 and To control Individual LEDs and set different color and brightness to each

	  	  if (SHOW_LED_PATTERN == 1){
	  		  return LED_OK;
	  	  }
	  }
}

LED_RET_TypeDef BSP_ALL_LED_HEARTBEAT(){
	  struct LED_State_t led;			//To control All LEDs and set same color and brightness to ALL

	  led.R = 255;
	  led.G = 255;
	  led.B = 255;
	  led.Brightness_R = 255;
	  led.Brightness_G = 255;
	  led.Brightness_B = 255;

	  int invert = 0;
	  while(1){

		  if(invert == 0){
			  led.R--; led.G--; led.B--; led.Brightness_R--; led.Brightness_G--; led.Brightness_B--;
		  } else if (invert == 1){
			  led.R++; led.G++; led.B++; led.Brightness_R++; led.Brightness_G++; led.Brightness_B++;
		  }
		  if(led.R == 1)
			  invert = 1;
		  else if (led.R == 255)
			  invert = 0;

		  BSP_ALL_LED_CONTROL(&led);
	  	  if (SHOW_LED_PATTERN == 1){
	  		  return LED_OK;
	  	  }
	  }
	  return LED_OK;
}

LED_RET_TypeDef BSP_ALL_LED_ON(){
	  struct LED_State_t led;			//To control All LEDs and set same color and brightness to ALL

	  led.R = 255;
	  led.G = 255;
	  led.B = 255;
	  led.Brightness_R = 255;
	  led.Brightness_G = 255;
	  led.Brightness_B = 255;

	  BSP_ALL_LED_CONTROL(&led);
	  return LED_OK;
}

LED_RET_TypeDef BSP_Multicolor_circular_motion(){
	struct LED_State_t ledArray[32];

    for (int i = 0; i <= 31; i++) {

    	if(i<=10)	{ ledArray[i].R = 255; ledArray[i].Brightness_R = 255; ledArray[i].G = 0;	ledArray[i].B = 0;	ledArray[i].Brightness_G = 0;	ledArray[i].Brightness_B = 0; }
		else if(i>10 && i<=20)	{ ledArray[i].G = 255; ledArray[i].Brightness_G = 255; ledArray[i].R = 0;	ledArray[i].B = 0;	ledArray[i].Brightness_R = 0;	ledArray[i].Brightness_B = 0;}
		else if( i>20)	{ ledArray[i].B = 255; ledArray[i].Brightness_B = 255;	ledArray[i].R = 0;	ledArray[i].G = 0;	ledArray[i].Brightness_R = 0;	ledArray[i].Brightness_G = 0; }

	    }

	    struct LED_State_t temp;
	    for(int loop_counter=0;;loop_counter++){
	  	  temp = ledArray[31];
	  	  // Shift all elements one step to the right
	  	  for (int i = 32 - 1; i > 0; i--) {
	  		  ledArray[i] = ledArray[i - 1];
	  	  }
	  	  // Place the last element at the first position
	  	  ledArray[0] = temp;
	  	  HAL_Delay(1);
	  	  BSP_INDIVIDUAL_LED_CONTROL(ledArray); //Length of array should be exactly 32 and To control Individual LEDs and set different color and brightness to each
	  	  if (SHOW_LED_PATTERN == 1){
	  		  return LED_OK;
	  	  }
	    }
}
#endif
