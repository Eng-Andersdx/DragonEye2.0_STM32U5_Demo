/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          	: STM32TouchController.cpp
  * Version				: 1.1
  ******************************************************************************
  * This file was created by TouchGFX Generator 4.26.0. This file is only
  * generated once! Delete this file from your project and re-generate code
  * using STM32CubeMX or change this file manually to update it.
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

/******************************************************************************
 * Change History Table
 * -----------------------------------------------------------------------------
 * Version | Date       | Author       | Description
 * -----------------------------------------------------------------------------
 * 1.0     | 2025-10-07 | H.Hunain  | Added driver for st1x33i
 * 1.1     | 2025-10-07 | H.Hunain  | Added driver for DATAIMAGE_MICROCHIP_TOUCH_IC
 * -----------------------------------------------------------------------------
 * Notes:
 * - Always increment version when making manual changes.
 * - Describe clearly what was changed in each version.
 ******************************************************************************/

/* USER CODE END Header */

/* USER CODE BEGIN STM32TouchController */


#include <STM32TouchController.hpp>
#include <cstdlib>
#include "main.h"

static bool TS_initialized = false;
volatile uint8_t Touch_Event_Flag = 0;

#ifdef st1x33i
static TS_StateTypeDef  TS_State = {0};
touch_data_t ST_1633ITouchData;
uint8_t  I2C_Address = ST1633I_ADD_REG<<1;
uint8_t cord[3]={0};
#endif

#ifdef DATAIMAGE_MICROCHIP_TOUCH_IC
static bool Touch_State = false;
uint8_t check_pic16f1885;
static bool touch_active = false;
static uint32_t touch_start_time = 0;
#endif
extern "C" void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
extern I2C_HandleTypeDef hi2c5;
using namespace touchgfx;
extern "C" volatile uint8_t haptic_feedback;
uint8_t touch_happened = 0;

uint32_t old = 0;

void STM32TouchController::init()
{
	if(!TS_initialized)
	{
#ifdef st1x33i
		BSP_TS_Init(480,480);
#endif
		TS_initialized = true;
	}
}

bool STM32TouchController::sampleTouch(int32_t& x, int32_t& y)
{
#ifdef st1x33i

	static int32_t lastX = 0;
	static int32_t lastY = 0;

	if (Touch_Event_Flag == 1){

		uint32_t now = HAL_GetTick();
		if(now - old >= 10){
			TS_IO_ReadMultiple(I2C_Address, 0x12, cord, sizeof(cord));
			ST1633I_TouchDtect(&ST_1633ITouchData, cord);
			old = HAL_GetTick();
		} else {
			Touch_Event_Flag = 0;
		}
	}

	if (BSP_TS_GetState(&TS_State) == TOUCH_OK){
		if(TS_State.touchDetected){
			if(touch_happened==0){
				touch_happened = 1;
				haptic_feedback = 2;
			}

			int32_t newX = TS_State.touchX[0];
			int32_t newY = TS_State.touchY[0];

			// ---- Jitter filter (dead zone of 15 pixels) ----
			if (std::abs(newX - lastX) < 15 && std::abs(newY - lastY) < 15)
			{
				newX = lastX;
				newY = lastY;
			}

			lastX = newX;
			lastY = newY;

			x = lastX;
			y = lastY;
			return true;
		}
	}
	if(touch_happened==1){
		touch_happened = 0;
	}
#endif
#ifdef DATAIMAGE_MICROCHIP_TOUCH_IC

	if (Touch_State == true)
	{
		HAL_I2C_Mem_Read(&hi2c5, 0x28 << 1, 0x01, I2C_MEMADD_SIZE_8BIT, &check_pic16f1885, 1, HAL_MAX_DELAY);
		if(check_pic16f1885 == 0){
			Touch_State = false;
			touch_active = false;
		} else {
			if (!touch_active)
			        {
			            // First detection, record start time
			            touch_active = true;
			            touch_start_time = HAL_GetTick();
			        }
			else {
				 // Already active, check duration
				  if (HAL_GetTick() - touch_start_time >= 200)
				  {
					  x=5;
					  y=5;
					  Touch_State = false;
					  touch_active = false;
					  return true;  // Valid touch detected
				  }
			}
		}
	}
	return false;
#endif
    return false;
}

// Boards Shipped with following code.
//bool STM32TouchController::sampleTouch(int32_t& x, int32_t& y)
//{
//#ifdef st1x33i
//	if (BSP_TS_GetState(&TS_State) == TOUCH_OK){
//		if(TS_State.touchDetected){
//			x = TS_State.touchX[0];
//			y = TS_State.touchY[0];
//			return true;
//		}
//	}
//#endif
//#ifdef DATAIMAGE_MICROCHIP_TOUCH_IC
//
////	if (Touch_State == true)
////	{
//		HAL_I2C_Mem_Read(&hi2c5, 0x28 << 1, 0x01, I2C_MEMADD_SIZE_8BIT, &check_pic16f1885, 1, HAL_MAX_DELAY);
//		if(check_pic16f1885 == 0){
////			Touch_State = false;
//			touch_active = false;
//		} else {
//			if (!touch_active)
//			        {
//			            // First detection, record start time
//			            touch_active = true;
//			            touch_start_time = HAL_GetTick();
//			        }
//			else {
//				 // Already active, check duration
//				  if (HAL_GetTick() - touch_start_time >= 200)
//				  {
//					  x=5;
//					  y=5;
//					  return true;  // Valid touch detected
//				  }
//			}
//		}
////	}
//	return false;
//#endif
//    return false;
//}

#ifdef st1x33i

void TS_IO_Write(uint8_t Addr, uint8_t Reg, uint8_t Value) {
	//BSP_I2C5_WriteReg(Addr, (uint16_t) Reg, &Value, 1);
	HAL_I2C_Mem_Write(&hi2c5, Addr, (uint16_t) Reg, I2C_MEMADD_SIZE_8BIT, &Value, 1, 10000);
}

uint8_t  TS_IO_Read(uint8_t Addr, uint8_t Reg) {
	uint8_t Value = 0;
	//BSP_I2C5_ReadReg(Addr, Reg, &Value, 1);
	HAL_I2C_Mem_Read(&hi2c5, Addr, (uint16_t) Reg, I2C_MEMADD_SIZE_8BIT, &Value, 1, 10000);
	return Value;
}

uint16_t TS_IO_ReadMultiple(uint8_t Addr, uint8_t Reg, uint8_t *Buffer, uint16_t Length) {
	//return BSP_I2C5_ReadReg(Addr, Reg, Buffer, Length);
	return HAL_I2C_Mem_Read(&hi2c5, Addr, (uint16_t) Reg, I2C_MEMADD_SIZE_8BIT, Buffer, Length, 10000);
}

void ST1633I_TouchDtect (touch_data_t * tdata , uint8_t *cordi)
{

	tdata->xyz_data[0].xy_h = cordi[0];
	if(tdata->xyz_data[0].ValidFlage == 0)
	{
		tdata->xyz_data[0].ValidFlage = tdata->xyz_data[0].xyh_bit.valid;

	}
		if(tdata->xyz_data[0].xyh_bit.valid)
		{

		tdata->xyz_data[0].x = cordi[1] | (cordi[0] & 0x70)<<4;
		tdata->xyz_data[0].y = cordi[2] | (cordi[0] & 0x07)<<8;
		}
		else
		{
			if(tdata->xyz_data[0].ValidFlage == 0)
			{
				tdata->xyz_data[0].x = 0;
				tdata->xyz_data[0].y = 0;
			}
		}
}


/**
 * @brief Initializes the TS_1633I touch sensor
 *
 * This function initializes the TS_1633I touch sensor by calling the power initialization function, introducing a delay, and performing further sensor-specific initialization.
 *
 * @param ts_SizeX The size of the touch sensor in the X-direction
 * @param ts_SizeY The size of the touch sensor in the Y-direction
 * @return The status of the touch sensor initialization
 */
uint8_t BSP_TS_Init(uint16_t ts_SizeX, uint16_t ts_SizeY)
{
	uint8_t ts_status = TOUCH_OK;
//	HAL_GPIO_WritePin(Touch_Reset_GPIO_Port, Touch_Reset_Pin, GPIO_PIN_RESET);
//	HAL_Delay(10);
//	HAL_GPIO_WritePin(Touch_Reset_GPIO_Port, Touch_Reset_Pin, GPIO_PIN_SET);
//	HAL_Delay(10);
	TS_IO_Write(I2C_Address,ST1633I_TD_CONTROL_REG_RW,ST1633I_TD_MULTOUCH_DIS);
	return (ts_status);
}

/**
  * @brief  Returns status and positions of the touch screen.
  * @param  TS_State: Pointer to touch screen current state structure
  * @retval TOUCH_OK if all initializations are OK. Other value if error.
  */
uint8_t BSP_TS_GetState(TS_StateTypeDef *TS_State)
{
	uint8_t ts_status = TOUCH_OK;

	if(ST_1633ITouchData.xyz_data[0].ValidFlage)
	{
		TS_State->touchDetected = ST_1633ITouchData.xyz_data[0].ValidFlage+ST_1633ITouchData.xyz_data[1].ValidFlage+ST_1633ITouchData.xyz_data[2].ValidFlage+ST_1633ITouchData.xyz_data[3].ValidFlage;
		TS_State->touchX[0] = ST_1633ITouchData.xyz_data[0].x;
		TS_State->touchY[0] = ST_1633ITouchData.xyz_data[0].y;
		ST_1633ITouchData.xyz_data[0].ValidFlage = 0;
		ST_1633ITouchData.xyz_data[1].ValidFlage = 0;
		ST_1633ITouchData.xyz_data[2].ValidFlage = 0;
		ST_1633ITouchData.xyz_data[3].ValidFlage = 0;
	}
	else
	{
		TS_State->touchDetected = 0;
	}

	return ts_status;
}

uint8_t BSP_TS_ResetTouchData(TS_StateTypeDef *TS_State)
{

	ST_1633ITouchData.xyz_data[0].x = 0;
	ST_1633ITouchData.xyz_data[0].y = 0;
	ST_1633ITouchData.xyz_data[1].x = 0;
	ST_1633ITouchData.xyz_data[1].y = 0;
	ST_1633ITouchData.xyz_data[2].x = 0;
	ST_1633ITouchData.xyz_data[2].y = 0;
	ST_1633ITouchData.xyz_data[3].x = 0;
	ST_1633ITouchData.xyz_data[3].y = 0;
	ST_1633ITouchData.xyz_data[0].ValidFlage = 0 ;
	ST_1633ITouchData.xyz_data[1].ValidFlage = 0 ;
	ST_1633ITouchData.xyz_data[2].ValidFlage = 0 ;
	ST_1633ITouchData.xyz_data[3].ValidFlage = 0 ;
	for(uint8_t coc=0;coc<sizeof(cord);coc++)
	{
		cord[coc]=0;
	}
	return TOUCH_OK;
}
#endif
/* CTP interrupt callback */
//void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin){
//	HAL_GPIO_EXTI_Callback(GPIO_Pin);
//}
//
//void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin){
//	HAL_GPIO_EXTI_Callback(GPIO_Pin);
//}
//
//extern "C" void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
//{
//#ifdef st1x33i
//	TS_IO_ReadMultiple(I2C_Address, 0x12, cord, sizeof(cord));
//	ST1633I_TouchDtect(&ST_1633ITouchData, cord);
//#endif
//
//#ifdef DATAIMAGE_MICROCHIP_TOUCH_IC
//	Touch_State = true;
//#endif
//}


#ifdef ft6x06

static TS_DrvTypeDef *ts_driver;

/**
 * @brief Initializes the touch sensor (other than TS_1633I)
 *
 * This function initializes a touch sensor other than TS_1633I by performing necessary initialization steps specific to the alternative touch sensor.
 *
 * @param ts_SizeX The size of the touch sensor in the X-direction
 * @param ts_SizeY The size of the touch sensor in the Y-direction
 * @return The status of the touch sensor initialization
 */
uint8_t BSP_TS_Init(uint16_t ts_SizeX, uint16_t ts_SizeY)
{
  uint8_t ts_status = TOUCH_OK;
  uint8_t ts_id1, ts_id2 = 0;
  /* Note : I2C_Address is un-initialized here, but is not used at all in init function */
  /* but the prototype of Init() is like that in template and should be respected       */

  /* Initialize the communication channel to sensor (I2C) if necessary */
  /* that is initialization is done only once after a power up         */
  ft6x06_ts_drv.Init(I2C_Address);

  ts_id1 = ft6x06_ts_drv.ReadID(TS_I2C_ADDRESS);
  if(ts_id1 != FT6206_ID_VALUE)
  {
    ts_id2 = ft6x06_ts_drv.ReadID(TS_I2C_ADDRESS_A02);
    I2C_Address    = TS_I2C_ADDRESS_A02;
  }
  else
  {
    I2C_Address    = TS_I2C_ADDRESS;
  }

  /* Scan FT6xx6 TouchScreen IC controller ID register by I2C Read       */
  /* Verify this is a FT6206 or FT6336G, otherwise this is an error case */
  if((ts_id1 == FT6206_ID_VALUE) || (ts_id2 == FT6206_ID_VALUE) || (ts_id2 == FT6X36_ID2_VALUE))
  {
    /* Found FT6206 : Initialize the TS driver structure */
    ts_driver = &ft6x06_ts_drv;

    /* Get LCD chosen orientation */
    if(ts_SizeX < ts_SizeY)
    {
      ts_orientation = TS_SWAP_NONE;
    }
    else
    {
      ts_orientation = TS_SWAP_XY | TS_SWAP_Y;
    }

    if(ts_status == TOUCH_OK)
    {
      /* Software reset the TouchScreen */
      ts_driver->Reset(I2C_Address);

      /* Calibrate, Configure and Start the TouchScreen driver */
      ts_driver->Start(I2C_Address);

    } /* of if(ts_status == TOUCH_OK) */
  }
  else
  {
    ts_status = TS_DEVICE_NOT_FOUND;
  }

  return (ts_status);
}

/**
  * @brief  Update gesture Id following a touch detected.
  * @param  TS_State: Pointer to touch screen current state structure
  * @retval TOUCH_OK if all initializations are OK. Other value if error.
  */
uint8_t BSP_TS_Get_GestureId(TS_StateTypeDef *TS_State)
{
  uint32_t gestureId = 0;
  uint8_t  ts_status = TOUCH_OK;

  /* Get gesture Id */
  ft6x06_TS_GetGestureID(I2C_Address, &gestureId);

  /* Remap gesture Id to a TS_GestureIdTypeDef value */
  switch(gestureId)
  {
    case FT6206_GEST_ID_NO_GESTURE :
      TS_State->gestureId = GEST_ID_NO_GESTURE;
      break;
    case FT6206_GEST_ID_MOVE_UP :
      TS_State->gestureId = GEST_ID_MOVE_UP;
      break;
    case FT6206_GEST_ID_MOVE_RIGHT :
      TS_State->gestureId = GEST_ID_MOVE_RIGHT;
      break;
    case FT6206_GEST_ID_MOVE_DOWN :
      TS_State->gestureId = GEST_ID_MOVE_DOWN;
      break;
    case FT6206_GEST_ID_MOVE_LEFT :
      TS_State->gestureId = GEST_ID_MOVE_LEFT;
      break;
    case FT6206_GEST_ID_ZOOM_IN :
      TS_State->gestureId = GEST_ID_ZOOM_IN;
      break;
    case FT6206_GEST_ID_ZOOM_OUT :
      TS_State->gestureId = GEST_ID_ZOOM_OUT;
      break;
    default :
      ts_status = TOUCH_ERROR;
      break;
  } /* of switch(gestureId) */

  return(ts_status);
}


/** @defgroup STM32469I-Discovery_TS_Private_Functions STM32469I Discovery TS Private Functions
  * @{
  */


/**
  * @brief  Function used to reset all touch data before a new acquisition
  *         of touch information.
  * @param  TS_State: Pointer to touch screen current state structure
  * @retval TOUCH_OK if OK, TE_ERROR if problem found.
  */
uint8_t BSP_TS_ResetTouchData(TS_StateTypeDef *TS_State)
{
  uint8_t ts_status = TOUCH_ERROR;
  uint32_t index;

  if (TS_State != (TS_StateTypeDef *)NULL)
  {
    TS_State->gestureId = GEST_ID_NO_GESTURE;
    TS_State->touchDetected = 0;

    for(index = 0; index < TS_MAX_NB_TOUCH; index++)
    {
      TS_State->touchX[index]       = 0;
      TS_State->touchY[index]       = 0;
      TS_State->touchArea[index]    = 0;
      TS_State->touchEventId[index] = TOUCH_EVENT_NO_EVT;
      TS_State->touchWeight[index]  = 0;
    }

    ts_status = TOUCH_OK;

  } /* of if (TS_State != (TS_StateTypeDef *)NULL) */

  return (ts_status);

}

uint8_t BSP_TS_GetState(TS_StateTypeDef *TS_State)
{
	  static uint32_t _x[TS_MAX_NB_TOUCH] = {0, 0};
	  static uint32_t _y[TS_MAX_NB_TOUCH] = {0, 0};

	  uint16_t tmp;
	  uint16_t Raw_x[TS_MAX_NB_TOUCH];
	  uint16_t Raw_y[TS_MAX_NB_TOUCH];
	  uint16_t xDiff;
	  uint16_t yDiff;
	  uint32_t index;
	#if (TS_MULTI_TOUCH_SUPPORTED == 1)
	  uint32_t weight = 0;
	  uint32_t area = 0;
	  uint32_t event = 0;
	#endif /* TS_MULTI_TOUCH_SUPPORTED == 1 */

	  /* Check and update the number of touches active detected */
	  TS_State->touchDetected = ts_driver->DetectTouch(I2C_Address);
	  if(TS_State->touchDetected)
	  {
	    for(index=0; index < TS_State->touchDetected; index++)
	    {
	      /* Get each touch coordinates */
	      ts_driver->GetXY(I2C_Address, &(Raw_x[index]), &(Raw_y[index]));

	      if(ts_orientation & TS_SWAP_XY)
	      {
	        tmp = Raw_x[index];
	        Raw_x[index] = Raw_y[index];
	        Raw_y[index] = tmp;
	      }

	      if(ts_orientation & TS_SWAP_X)
	      {
	        Raw_x[index] = FT_6206_MAX_WIDTH - 1 - Raw_x[index];
	      }

	      if(ts_orientation & TS_SWAP_Y)
	      {
	        Raw_y[index] = FT_6206_MAX_HEIGHT - 1 - Raw_y[index];
	      }

	      xDiff = Raw_x[index] > _x[index]? (Raw_x[index] - _x[index]): (_x[index] - Raw_x[index]);
	      yDiff = Raw_y[index] > _y[index]? (Raw_y[index] - _y[index]): (_y[index] - Raw_y[index]);

	      if ((xDiff + yDiff) > 5)
	      {
	        _x[index] = Raw_x[index];
	        _y[index] = Raw_y[index];
	      }


	      TS_State->touchX[index] = _x[index];
	      TS_State->touchY[index] = _y[index];

	#if (TS_MULTI_TOUCH_SUPPORTED == 1)

	      /* Get touch info related to the current touch */
	      ft6x06_TS_GetTouchInfo(I2C_Address, index, &weight, &area, &event);

	      /* Update TS_State structure */
	      TS_State->touchWeight[index] = weight;
	      TS_State->touchArea[index]   = area;

	      /* Remap touch event */
	      switch(event)
	      {
	        case FT6206_TOUCH_EVT_FLAG_PRESS_DOWN  :
	          TS_State->touchEventId[index] = TOUCH_EVENT_PRESS_DOWN;
	          break;
	        case FT6206_TOUCH_EVT_FLAG_LIFT_UP :
	          TS_State->touchEventId[index] = TOUCH_EVENT_LIFT_UP;
	          break;
	        case FT6206_TOUCH_EVT_FLAG_CONTACT :
	          TS_State->touchEventId[index] = TOUCH_EVENT_CONTACT;
	          break;
	        case FT6206_TOUCH_EVT_FLAG_NO_EVENT :
	          TS_State->touchEventId[index] = TOUCH_EVENT_NO_EVT;
	          break;
	        default :
	          ts_status = TOUCH_ERROR;
	          break;
	      } /* of switch(event) */

	#endif /* TS_MULTI_TOUCH_SUPPORTED == 1 */

	    } /* of for(index=0; index < TS_State->touchDetected; index++) */

	#if (TS_MULTI_TOUCH_SUPPORTED == 1)
	    /* Get gesture Id */
	    ts_status = BSP_TS_Get_GestureId(TS_State);
	#endif /* TS_MULTI_TOUCH_SUPPORTED == 1 */

	  } /* end of if(TS_State->touchDetected != 0) */

	  return (ts_status);
}
#endif

#if FT5426 || ILI2130 || ILI2511

void STM32TouchController::getDriver ( void )
{
	uint8_t Buf[ 10 ];

	TP.driver = unknown;

	if( HAL_I2C_Mem_Read( &hi2c1, 0x70, 0xA3, I2C_MEMADD_SIZE_8BIT, &Buf[0], 1, 10 ) == HAL_OK )
	{
		if( Buf[0] == 0x0A ) // FT5x16
		{
		}
		else if( Buf[0] == 0x54 )
			TP.driver = FT5426;
		else if( Buf[0] == 0x55 ) // FT5x06
		{
		}
		else {}
	}
	else if( HAL_I2C_Mem_Read( &hi2c1, 0x82, 0x40, I2C_MEMADD_SIZE_8BIT, &Buf[0], 8, 10 ) == HAL_OK )
	{
		if( Buf[0] == 0x06 )
			TP.driver = ILI2511;
		if( Buf[0] == 0x07 )
			TP.driver = ILI2130;
	}
	else {
	}
}

void STM32TouchController::getRange ( void )
{
	uint8_t Buf[ 15 ];

	switch( TP.driver )
	{
		case FT5426:
			TP.range.x = 1792;
			TP.range.y = 1024;
			break;
		case ILI2130:
			if( HAL_I2C_Mem_Read( &hi2c1, 0x82, 0x20, I2C_MEMADD_SIZE_8BIT, &Buf[0], 10, 10 ) != HAL_OK )
				return;

			if( (Buf[8] == 0) || (Buf[8] > 32) )
				return;

			TP.range.x = *(uint16_t*)&Buf[0];
			TP.range.y = *(uint16_t*)&Buf[2];
			break;
		case ILI2511:
			if( HAL_I2C_Mem_Read( &hi2c1, 0x82, 0x20, I2C_MEMADD_SIZE_8BIT, &Buf[0], 10, 10 ) != HAL_OK )
				return;

			if( (Buf[6] == 0) || (Buf[6] == 255) )
				return;

			TP.range.x = *(uint16_t*)&Buf[0];
			TP.range.y = *(uint16_t*)&Buf[2];
			break;
		default:
			return;
	}
}

void STM32TouchController::getVersion ( void )
{
	uint8_t Buf[ 10 ];
	switch( TP.driver )
	{
		case FT5426:
			if( HAL_I2C_Mem_Read( &hi2c1, 0x70, 0xA1, I2C_MEMADD_SIZE_8BIT, &Buf[0], 2, 10 ) == HAL_OK )
			{
				TP.version = ( Buf[0] << 8 ) + Buf[1];
			}
			break;
		case ILI2130:
			if( HAL_I2C_Mem_Read( &hi2c1, 0x82, 0x42, I2C_MEMADD_SIZE_8BIT, &Buf[0], 3, 10 ) == HAL_OK )
			{
				if( (0 < Buf[ 0 ]) && (Buf[ 0 ] < 255) )
					TP.version = Buf[0];
			}
			break;
		case ILI2511:
			if( HAL_I2C_Mem_Read( &hi2c1, 0x82, 0x42, I2C_MEMADD_SIZE_8BIT, &Buf[0], 3, 10 ) == HAL_OK )
			{
				if( (0 < Buf[ 0 ]) && (Buf[ 0 ] < 255) )
					TP.version = Buf[0];
			}
			break;
		default:
			TP.type = NTP;
			return;
	}
}

#endif

#ifdef FT5426

bool STM32TouchController::FT5426_Read ( int32_t& x, int32_t& y )
{
		// +------+-----------+--------+--------+--------+--------+--------+--------+--------+--------+
		// | Addr |    Name   |   b7   |   b6   |   b5   |   b4   |   b3   |   b2   |   b1   |   b0   |
		// +------+-----------+--------+--------+--------+--------+--------+--------+--------+--------+
		// | 0x02 | Cur Point |      			  Number of touch points[7:0]  						  |
		// +------+-----------+-----------------+--------+--------+-----------------------------------+
		// | 0x03 | TOUCH1_XH | 1st Event Flag  |        |		  | 	1st Touch X Position[11:8]    |
		// +------+-----------+-----------------+--------+--------+-----------------------------------+
		// | 0x04 | TOUCH1_XL | 				  1st Touch X Position[7:0]							  |
		// +------+-----------+-----------------------------------------------------------------------+
		// | 0x05 | TOUCH1_YH | 	  1st Touch ID[3:0]			  |     1st Touch Y Position[11:8]    |
		// +------+-----------+-----------------------------------+-----------------------------------+
		// | 0x06 | TOUCH1_YL | 				  1st Touch Y Position[7:0]							  |
		// +------+-----------+-----------------------------------------------------------------------+

		#pragma pack( 1 )
		union{
			uint8_t Buf[ 5 ];
			struct{
				/*-------- Byte0 --------*/
				uint8_t point_number :8;
				/*-------- Byte1 --------*/
				uint8_t X_H :4;
				uint8_t :2;
				uint8_t Event :2;
				/*-------- Byte2 --------*/
				uint8_t X_L :8;
				/*-------- Byte3 --------*/
				uint8_t Y_H :4;
				uint8_t ID :4;
				/*-------- Byte4 --------*/
				uint8_t Y_L :8;
			}reg;
		}Data;
		#pragma pack( )

		if(HAL_I2C_Mem_Read( &hi2c1, 0x70, 0x02, I2C_MEMADD_SIZE_8BIT, &Data.Buf[0], 5, 10 ) != HAL_OK)
			return false;

		if( Data.reg.ID != 0 )
			return false;

		if( ( Data.reg.Event != 0x00 ) && ( Data.reg.Event != 0x02 ) )
			return false;

		uint32_t sx,sy;

		sx = ((Data.reg.X_H & 0x0F) << 8) | Data.reg.X_L;
		sy = ((Data.reg.Y_H & 0x0F) << 8) | Data.reg.Y_L;
		x = (sx * 1024) / TP.range.x;
		y = (sy * 600) / TP.range.y;

		return true;
}

#endif

#ifdef ILI2130

bool STM32TouchController::ILI2130_Read ( int32_t& x, int32_t& y )
{
	// I2C Packet Format
	// +-----------------+---------------- +---------------------------------+
	// | Number of Bytes |	 Description   |              Notes              |
	// +-----------------+-----------------+---------------------------------+
	// | 	   0		 |    Report ID    | I2C = 0x48                      |
	// +-----------------+-----------------+---------------------------------+
	// |       1~5       |     Point 1     | Please refer Report ID Format   |
	// +-----------------+-----------------+---------------------------------+
	// |       6~10      |     Point 2     | Please refer Report ID Format   |
	// +-----------------+-----------------+---------------------------------+
	// |       11~15     |     Point 3     | Please refer Report ID Format   |
	// +-----------------+-----------------+---------------------------------+
	// |       16~20     |     Point 4     | Please refer Report ID Format   |
	// +-----------------+-----------------+---------------------------------+
	// |       21~25     |     Point 5     | Please refer Report ID Format   |
	// +-----------------+-----------------+---------------------------------+
	// |       26~30     |     Point 6     | Please refer Report ID Format   |
	// +-----------------+-----------------+---------------------------------+
	// |       31~35     |     Point 7     | Please refer Report ID Format   |
	// +-----------------+-----------------+---------------------------------+
	// |       36~40     |     Point 8     | Please refer Report ID Format   |
	// +-----------------+-----------------+---------------------------------+
	// |       41~45     |     Point 9     | Please refer Report ID Format   |
	// +-----------------+-----------------+---------------------------------+
	// |       46~50     |     Point 10    | Please refer Report ID Format   |
	// +-----------------+-----------------+---------------------------------+
	// |       51~60     |     Reserve     |                                 |
	// +-----------------+-----------------+---------------------------------+
	// |       61        |  Contact Count  |                                 |
	// +-----------------+-----------------+---------------------------------+
	// |       62        |     Reserve     |                                 |
	// +-----------------+-----------------+---------------------------------+
	// |       63        |    Check Sum    |                                 |
	// +-----------------+-----------------+---------------------------------+

	// Report ID Format
	// +-----------------+-------------------------+------+------+------+------+------+------+------+------+
	// | Number of Bytes |	     Description       |  b7  |  b6  |  b5  |  b4  |  b3  |  b2  |  b1  |  b0  |
	// +-----------------+-------------------------+------+------+------+------+------+------+------+------+
	// |        1        | Tip Switch and Point ID |  0   | Tip  |                 Point ID                |
	// +-----------------+-------------------------+------+------+------+------+------+------+------+------+
	// |        2        |                         | X Position (LSB)                                      |
	// +-----------------+ X direction coordinate  +------+------+------+------+------+------+------+------+
	// |        3        |                         | X Position (MSB)                                      |
	// +-----------------+-------------------------+------+------+------+------+------+------+------+------+
	// |        4        |                         | Y Position (LSB)                                      |
	// +-----------------+ Y direction coordinate  +------+------+------+------+------+------+------+------+
	// |        5        |                         | Y Position (MSB)                                      |
	// +-----------------+-------------------------+------+------+------+------+------+------+------+------+

	#pragma pack( 1 )
	union{
		uint8_t Buf[ 64 ];
		struct{
			/*-------- Byte0 --------*/
			uint8_t Report_ID :8;
			/*-------- Byte1 to Byte50 --------*/
			struct{
				uint8_t Point_ID :6;
				uint8_t Tip :1;
				uint8_t :1;

				uint16_t x;

				uint16_t y;
			}point[ 10 ];
			/*-------- Byte51 to Byte60 --------*/
			uint8_t Reserve[ 10 ];
			/*-------- Byte61 --------*/
			uint8_t Contact_Count :8;
			/*-------- Byte62 --------*/
			uint8_t :8;
			/*-------- Byte63 --------*/
			uint8_t Check_Sum :8;
		}reg;
	}Data;
	#pragma pack( )

	if( HAL_I2C_Mem_Read( &hi2c1, 0x82, 0x10, I2C_MEMADD_SIZE_8BIT, &Data.Buf[0], 64, 10 ) != HAL_OK )
	{
		HAL_I2C_DeInit(&hi2c1);
		HAL_I2C_Init(&hi2c1);
		HAL_GPIO_WritePin(CTP_RST_GPIO_Port, CTP_RST_Pin, GPIO_PIN_RESET);
		HAL_Delay( 20 );
		HAL_GPIO_WritePin(CTP_RST_GPIO_Port, CTP_RST_Pin, GPIO_PIN_SET);
		HAL_Delay( 1000 );
		return false;
	}

	if( Data.reg.Report_ID != 0x48 )
		return false;

	if( Data.reg.point[0].Tip == 0 ) // Tip switch
		return false;

	if( Data.reg.point[0].Point_ID != 0x00 ) // Point ID = 0
		return false;

	x = ( Data.reg.point[0].x * 1024 ) / TP.range.x;
	y = ( Data.reg.point[0].y * 600 ) / TP.range.y;

	return true;
}

#endif

#ifdef ILI2511

bool STM32TouchController::ILI2511_Read ( int32_t& x, int32_t& y )
{
	// +----------+--------------+------+---------------+------+------+------+------+------+------+------+
	// | CMD Code |     Name     | Note |      b7       |  b6  |  b5  |  b4  |  b3  |  b2  |  b1  |  b0  |
	// +----------+--------------+------+---------------+------+------+------+------+------+------+------+
	// |          | Touch        |      | 0: No touch	                                                 |
	// |          | Information  |      | 1: Last Report as ID0 to ID5(include release ststus)           |
	// |          |              |      | 1: Last Report as ID0 to ID5(include release ststus)           |
	// |          |              +------+---------------+------+-----------------------------------------+
	// |          |              | ID0  | 0: Touch Off  |  0   | X_High direction coordinate             |
	// |          |              |      | 1: Touch Down |      |                                         |
	// |          |              |      +---------------+------+-----------------------------------------+
	// |          |              |      | X_Low direction coordinate                                     |
	// |          |              |      +---------------+------+-----------------------------------------+
	// |          |              |      |       0       |  0   | Y_High direction coordinate             |
	// |          |              |      +---------------+------+-----------------------------------------+
	// |          |              |      | Y_Low direction coordinate                                     |
	// |          |              +------+----------------------------------------------------------------+
	// |          |              |   .  |                                .                               |
	// |          |              |   .  |                                .                               |
	// |          |              |   .  |                                .                               |
	// |          |              |   .  |                                .                               |
	// |          |              +------+---------------+------+-----------------------------------------+
	// |          |              | ID5  | 0: Touch Off  |  0   | X_High direction coordinate             |
	// |          |              |      | 1: Touch Down |      |                                         |
	// |          |              |      +---------------+------+-----------------------------------------+
	// |          |              |      | X_Low direction coordinate                                     |
	// |          |              |      +---------------+------+-----------------------------------------+
	// |          |              |      |       0       |  0   | Y_High direction coordinate             |
	// |          |              |      +---------------+------+-----------------------------------------+
	// |          |              |      | Y_Low direction coordinate                                     |
	// +----------+--------------+------+----------------------------------------------------------------+

	#pragma pack( 1 )
	union{
		uint8_t Buf[ 31 ];
		struct{
			/*-------- Byte0 --------*/
			uint8_t Packet_Number :8;
			/*-------- Byte1 to Byte50 --------*/
			struct{
				uint8_t X_H:6;
				uint8_t :1;
				uint8_t Touch_Status :1;

				uint8_t X_L :8;

				uint8_t Y_H:6;
				uint8_t :2;

				uint8_t Y_L :8;

				uint8_t Touch_Pressure :8;
			}point[ 6 ];
		}reg;
	}Data;
	#pragma pack( )

	uint32_t sx,sy;

	switch( TP.version )
	{
		case 2:
			break;
		case 3:
			if( HAL_I2C_Mem_Read( &hi2c1, 0x82, 0x10, I2C_MEMADD_SIZE_8BIT, &Data.Buf[0], 31, 10 ) != HAL_OK )
				return false;

			if( Data.reg.point[0].Touch_Status != 1 )
				return false;

			sx = ( (Data.reg.point[0].X_H & 0x3F) << 8 ) | Data.reg.point[0].X_L;
			sy = ( (Data.reg.point[0].Y_H & 0x3F) << 8 ) | Data.reg.point[0].Y_L;

			x = ( sx * 1024 ) / TP.range.x;
			y = ( sy * 600 ) / TP.range.y;

			return true;
		default:
			break;
	}
	return false;
}

#endif
/* USER CODE END STM32TouchController */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
