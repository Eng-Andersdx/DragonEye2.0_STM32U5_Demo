/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : app_freertos.c
  * Description        : FreeRTOS applicative file
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

/* Includes ------------------------------------------------------------------*/
#include "app_freertos.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
volatile uint8_t haptic_feedback = 0;
/* Pulse length in ms, set from Settings > Haptics. 5 or less = haptics off */
volatile uint8_t haptic_feedback_intensity = 35;
/* Set to 1 to hold the vibration motor on continuously (e.g. oven at max temp) */
volatile uint8_t haptic_continuous = 0;
/* Home-screen status icons: 1 = connected (bright), 0 = not connected (dimmed).
   Demo default is 1; set these from the Wi-Fi/BT driver once it is enabled. */
volatile uint8_t status_wifi_connected = 1;
volatile uint8_t status_bt_connected = 1;
/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 1024 * 4
};
/* Definitions for GUI_Task */
osThreadId_t GUI_TaskHandle;
const osThreadAttr_t GUI_Task_attributes = {
  .name = "GUI_Task",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 8192 * 4
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

/* USER CODE BEGIN 2 */
void vApplicationIdleHook( void )
{
   /* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
   to 1 in FreeRTOSConfig.h. It will be called on each iteration of the idle
   task. It is essential that code added to this hook function never attempts
   to block in any way (for example, call xQueueReceive() with a block time
   specified, or call vTaskDelay()). If the application makes use of the
   vTaskDelete() API function (as this demo application does) then it is also
   important that vApplicationIdleHook() is permitted to return to its calling
   function, because it is the responsibility of the idle task to clean up
   memory allocated by the kernel to any task that has since been deleted. */
}
/* USER CODE END 2 */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of GUI_Task */
  GUI_TaskHandle = osThreadNew(TouchGFX_Task, NULL, &GUI_Task_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}
/* USER CODE BEGIN Header_StartDefaultTask */
/**
* @brief Function implementing the defaultTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN defaultTask */
	uint32_t haptic_timer = 0;
	uint8_t  local_haptic;
	uint8_t  continuous_active = 0;
  /* Infinite loop */
  for(;;)
  {
	  /* Take a snapshot of the current haptic request */
	  local_haptic = haptic_feedback;

	  if (local_haptic == 1 && haptic_feedback_intensity > 5)
	  {
		  haptic_timer = haptic_feedback_intensity;	// ms (buttons)
		  HAL_GPIO_WritePin(Vibration_Motor_GPIO_Port, Vibration_Motor_Pin, GPIO_PIN_SET);
	  }
	  else if (local_haptic == 2 && haptic_feedback_intensity > 5)
	  {
		  haptic_timer = haptic_feedback_intensity + 15;	// ms (touch, slightly longer: 50ms at default)
		  HAL_GPIO_WritePin(Vibration_Motor_GPIO_Port, Vibration_Motor_Pin, GPIO_PIN_SET);
	  }

	  /* Clear the consumed haptic request */
	  if (local_haptic != 0)
	  {
		  haptic_feedback = 0;
	  }

	  if (haptic_timer > 0)
	  {
		  haptic_timer--;
		  if (haptic_timer == 0 && !haptic_continuous)
		  {
			  HAL_GPIO_WritePin(Vibration_Motor_GPIO_Port, Vibration_Motor_Pin, GPIO_PIN_RESET);
		  }
	  }

	  /* Continuous haptic: hold motor on while requested, release when cleared */
	  if (haptic_continuous && haptic_feedback_intensity > 5)
	  {
		  if (!continuous_active)
		  {
			  continuous_active = 1;
			  HAL_GPIO_WritePin(Vibration_Motor_GPIO_Port, Vibration_Motor_Pin, GPIO_PIN_SET);
		  }
	  }
	  else if (continuous_active)
	  {
		  continuous_active = 0;
		  if (haptic_timer == 0)
		  {
			  HAL_GPIO_WritePin(Vibration_Motor_GPIO_Port, Vibration_Motor_Pin, GPIO_PIN_RESET);
		  }
	  }
	  osDelay(1);
  }
  /* USER CODE END defaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

