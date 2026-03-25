/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32u5xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define MPI_SDPT_SWITCH_Pin GPIO_PIN_0
#define MPI_SDPT_SWITCH_GPIO_Port GPIOE
#define R_LED_GPIO_Pin GPIO_PIN_8
#define R_LED_GPIO_GPIO_Port GPIOC
#define G_LED_GPIO_Pin GPIO_PIN_4
#define G_LED_GPIO_GPIO_Port GPIOE
#define B_LED_GPIO_Pin GPIO_PIN_3
#define B_LED_GPIO_GPIO_Port GPIOE
#define DSI_BL_CTRL_Pin GPIO_PIN_6
#define DSI_BL_CTRL_GPIO_Port GPIOI
#define LCD_RESET_Pin GPIO_PIN_13
#define LCD_RESET_GPIO_Port GPIOC
#define OSPI_CLK_Pin GPIO_PIN_10
#define OSPI_CLK_GPIO_Port GPIOF
#define OSPI_D2_Pin GPIO_PIN_7
#define OSPI_D2_GPIO_Port GPIOF
#define OSPI_D1_Pin GPIO_PIN_9
#define OSPI_D1_GPIO_Port GPIOF
#define OSPI_D4_Pin GPIO_PIN_1
#define OSPI_D4_GPIO_Port GPIOC
#define OSPI_D3_Pin GPIO_PIN_6
#define OSPI_D3_GPIO_Port GPIOF
#define OSPI_D0_Pin GPIO_PIN_8
#define OSPI_D0_GPIO_Port GPIOF
#define OSPI_CS_Pin GPIO_PIN_2
#define OSPI_CS_GPIO_Port GPIOA
#define LCD_TOUCH_INT_Pin GPIO_PIN_8
#define LCD_TOUCH_INT_GPIO_Port GPIOE
#define LCD_TOUCH_INT_EXTI_IRQn EXTI8_IRQn
#define HALL_SENSOR_A_Pin GPIO_PIN_11
#define HALL_SENSOR_A_GPIO_Port GPIOB
#define HALL_SENSOR_A_EXTI_IRQn EXTI11_IRQn
#define HALL_SENSOR_B_Pin GPIO_PIN_12
#define HALL_SENSOR_B_GPIO_Port GPIOB
#define HALL_SENSOR_B_EXTI_IRQn EXTI12_IRQn
#define OSPI_D6_Pin GPIO_PIN_3
#define OSPI_D6_GPIO_Port GPIOC
#define DSI_BL_CTRLA0_Pin GPIO_PIN_0
#define DSI_BL_CTRLA0_GPIO_Port GPIOA
#define Vibration_Motor_Pin GPIO_PIN_10
#define Vibration_Motor_GPIO_Port GPIOB
#define OSPI_D5_Pin GPIO_PIN_2
#define OSPI_D5_GPIO_Port GPIOC
#define OSPI_D7_Pin GPIO_PIN_0
#define OSPI_D7_GPIO_Port GPIOC
#define OSPI_DQS_Pin GPIO_PIN_1
#define OSPI_DQS_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
