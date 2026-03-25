/**
  ******************************************************************************
  * @file    dragoneye_leds.h
  * @author  Anders Engineering Team
  * @version 1.6
  * @brief   This file contains definitions for DragonEYE LEDs hardware
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

#include "main.h"
#include "bsp.h"

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DRAGONEYE_LEDS_H
#define __DRAGONEYE_LEDS_H 0106 /* BSP LEDS API REVISION v.1.6 */

#ifdef __cplusplus
	extern "C" {
#endif

/* Following are the DutyCycles of Red, Green and Blue to generate different colours */
/* All colours are estimates and might differ from actual results */
#define RGB_RED          (100)
#define RGB_GREEN        (100 << 8)
#define RGB_BLUE         (100 << 16)

#define RGB_YELLOW       (100 | (100 << 8))
#define RGB_CYAN         ((100 << 8) | (100 << 16))
#define RGB_MAGENTA      (100 | (100 << 16))

#define RGB_ORANGE   	(100 | (70 << 8))
#define RGB_BROWN   	(100 | (60 << 8) | (20 << 16))
#define RGB_SKY_BLUE   	(31 | (59 << 8) | (100 << 16))
#define RGB_DARK_ORANGE   (100 | (40 << 8) | (10 << 16))

#define RGB_DARK_RED     (50)
#define RGB_DARK_GREEN   (50 << 8)
#define RGB_DARK_BLUE    (50 << 16)

#define RGB_OLIVE        (50 | (50 << 8))
#define RGB_TEAL         ((50 << 8) | (50 << 16))
#define RGB_PURPLE       (50 | (50 << 16))

#define RGB_GREY         (25 | (25 << 8) | (25 << 16))
#define RGB_MEDIUM_RED   (75)
#define RGB_MEDIUM_GREEN (75 << 8)
#define RGB_MEDIUM_BLUE  (75 << 16)

#define RGB_LIGHT_YELLOW (75 | (75 << 8))
#define RGB_LIGHT_CYAN   ((75 << 8) | (75 << 16))
#define RGB_LIGHT_MAGENTA (75 | (75 << 16))

#define RGB_WHITE        (100 | (100 << 8) | (100 << 16))

#define RGB_COFFEE			(45 | (27 << 8) | (9 << 16))
#define RGB_OVEN 			(65 | (35 << 8) | (8 << 16))
/* End of RGB Colour Definitions */

/* Includes ------------------------------------------------------------------*/


/** @addtogroup BSP
  * @{
  */

/** @addtogroup DragonEYE
  * @{
  */

/** @addtogroup DragonEYE_LEDs 
  * @{
  */

/** @defgroup DragonEYE_LEDs_Exported_Types DragonEYE LEDs Exported Types
 * @{
 */

/** @brief Led_TypeDef
  *  Anders DragonEYE board leds definitions.
  */
typedef enum
{
	BACK_LIGHT = 0,
	LED_RED = 1,
	LED_GREEN = 2,
	LED_BLUE = 3,
} Led_TypeDef;

typedef enum
{
	LED_OK = 0,
	LED_ERROR,
} LED_RET_TypeDef;

/**
  * @}
  */

/** @defgroup DragonEYE_LEDs_Exported_Constants DragonEYE LEDs Exported Constants
  * @{
  */

/* Three leds included on all Rotary Display variants of the DragonEYE board + 1 Led dedicated for backlight */
#define LEDn                             ((uint8_t)4)


/**
  * @}
  */

/** @defgroup DragonEYE_LEDs_Exported_Macros DragonEYE LEDs Exported Macros
  * @{
  */
/**
  * @}
  */

/** @defgroup DragonEYE_LEDs_Exported_Functions DragonEYE LEDs Exported Functions
 * @{
*/
	void             BSP_LED_Init(Led_TypeDef Led);
	void             BSP_LED_DeInit(Led_TypeDef Led);
	void             BSP_LED_On(Led_TypeDef Led);
	void             BSP_LED_Off(Led_TypeDef Led);
	void             BSP_LED_Toggle(Led_TypeDef Led);

	void             BSP_BL_PWM_Init(TIM_HandleTypeDef *htim, uint32_t BL_TIM_CH);
	void             BSP_BL_SetIntensity(uint8_t dutycycle);

	void             BSP_LED_PWM_Init(TIM_HandleTypeDef *htim, uint32_t R_TIM_CH, uint32_t G_TIM_CH, uint32_t B_TIM_CH);
	void             BSP_LED_SetRGBIntensity(uint8_t red_dutycycle, uint8_t green_dutycycle, uint8_t blue_dutycycle);
	void             BSP_LED_SetRedIntensity(uint8_t dutycycle);
	void             BSP_LED_SetGreenIntensity(uint8_t dutycycle);
	void             BSP_LED_SetBlueIntensity (uint8_t dutycycle);
	void			 BSP_LED_SetColor(uint32_t color);

#ifdef AWINIC_AW21036

	struct LED_State_t {
	    uint8_t R;          // Red value (0-255)
	    uint8_t G;          // Green value (0-255)
	    uint8_t B;          // Blue value (0-255)
	    uint8_t Brightness_R; // Brightness Red (0-255)
	    uint8_t Brightness_G; // Brightness Green (0-255)
	    uint8_t Brightness_B; // Brightness Blue (0-255)
	};

	#define AW21036_NUM_CHANNELS   36
	#define AW21036_I2C_ADDR_GND   0x34<<1
	#define AW21036_I2C_ADDR_VDD   0x35<<1
	#define AW21036_I2C_ADDR_SCL   0x36<<1
	#define AW21036_I2C_ADDR_BCAST 0x1C<<1   // broadcast
	#define AW21036_I2C_ADDR_SDA   0x37<<1

	#define AW21036_REG_GCR     0x00
	#define AW21036_REG_GCCR    0x6E
	#define AW21036_REG_BR0     0x01
	#define AW21036_REG_UPDATE  0x49
	#define AW21036_REG_COL0    0x4A
	#define AW21036_REG_RESET   0x7F

	void BSP_BL_PWM_Init(TIM_HandleTypeDef *htim, uint32_t BL_TIM_CH);
	void BSP_BL_SetIntensity(uint8_t dutycycle);

	LED_RET_TypeDef             BSP_ALL_LED_Init(I2C_HandleTypeDef *i2c_handler);
	LED_RET_TypeDef             BSP_ALL_LED_DeInit(I2C_HandleTypeDef *i2c_handler);
	LED_RET_TypeDef             BSP_ALL_LED_CONTROL(struct LED_State_t* led);
	LED_RET_TypeDef             BSP_ONE_LED_CONTROL(struct LED_State_t* led,uint8_t led_index);
	LED_RET_TypeDef             BSP_INDIVIDUAL_LED_CONTROL(struct LED_State_t* ledArray); //Length of array should be 32 minimum
	LED_RET_TypeDef 			BSP_ALL_LED_Fade(struct LED_State_t *start, struct LED_State_t *end, uint16_t steps, uint16_t delayMs);
	LED_RET_TypeDef				BSP_RANDOM_ALL_LED_PATTERN();
	LED_RET_TypeDef 			BSP_LED_RUNNING_CW();
	LED_RET_TypeDef 			BSP_ALL_LED_ON();
	LED_RET_TypeDef 			BSP_Multicolor_circular_motion();
#endif

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */


#ifdef __cplusplus
}
#endif

#endif /* __DRAGONEYE_LEDS_H */
