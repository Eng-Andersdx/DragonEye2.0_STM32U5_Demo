/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          	: STM32TouchController.hpp
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
#include <stdint.h>
#include "stm32u5xx_hal.h"
#include "bsp.h"
/* USER CODE END Header */

/* USER CODE BEGIN STM32TouchController */

#ifndef STM32TOUCHCONTROLLER_HPP
#define STM32TOUCHCONTROLLER_HPP

#ifdef st1x33i

/* Maximum Touch */
#define TS_MAX_NB_TOUCH 				((uint8_t) 4)

/* Maximum border values of the touchscreen pad */
#define  ST_1633I_MAX_WIDTH              ((uint16_t)480)     /* Touchscreen pad max width   */
#define  ST_1633I_MAX_HEIGHT             ((uint16_t)480)    /* Touchscreen pad max height  */

/* Touchscreen pad max width and height values for FT6xx6 Touch*/
#define  ST_1633I_MAX_WIDTH_HEIGHT       ((uint16_t)480)

/* Possible values of driver functions return status */
#define ST1633I_STATUS_OK                0
#define ST1633I_STATUS_NOT_OK            1

/* Possible values of global variable 'TS_I2C_Initialized' */
#define ST1633I_I2C_NOT_INITIALIZED      0
#define ST1633I_I2C_INITIALIZED          1

/* Max detectable simultaneous touches */
//#define ST1633I_MAX_DETECTABLE_TOUCH     2

#define ST1633I_TD_FWVERSION_RO            0x00
#define ST1633I_GEST_ID_NO_GESTURE       0x00
#define ST1633I_GEST_ID_MOVE_UP          0x10
#define ST1633I_GEST_ID_MOVE_RIGHT       0x14
#define ST1633I_GEST_ID_MOVE_DOWN        0x18
#define ST1633I_GEST_ID_MOVE_LEFT        0x1C
#define ST1633I_GEST_ID_ZOOM_IN          0x48
#define ST1633I_GEST_ID_ZOOM_OUT         0x49

/* Touch Data Status register : gives number of active touch points (0..2) */
#define ST1633I_TD_STAT_REG_RO              0x01

/* Device Status and Error */
#define ST1633I_TD_STAT_NORMAL				0x00
#define ST1633I_TD_STAT_INIT				0x01
#define ST1633I_TD_STAT_AUTOTUNING			0x03
#define ST1633I_TD_STAT_POWERDOWN			0x05
#define ST1633I_TD_STAT_BOOTROM				0x06
#define ST1633I_TD_STAT_WESUBAP				0x07
#define ST1633I_TD_ERROR_NOERR				0x00
#define ST1633I_TD_ERROR_INVALIDADD			0x01
#define ST1633I_TD_ERROR_INVALIDVAL			0x02
#define ST1633I_TD_ERROR_INVAIDPLAT			0x03
#define ST1633I_TD_ERROR_DEVNOFOUND			0x04
#define ST1633I_TD_ERROR_STACKOV			0x05

/*device contrrol reg*/
#define ST1633I_TD_CONTROL_REG_RW 			0x02
#define ST1633I_TD_MULTOUCH_DIS				0x40
#define ST1633I_TD_PROXIMITY_EN				0x20
#define ST1633I_TD_POWRDW					0x02
#define ST1633I_TD_RESET					0x01

#define ST1633I_TD_TIMEOUTIDEL_REG_RW		0x03

#define ST1633I_TD_RESOLUTIONXYH_REG_RO		0x04
#define ST1633I_TD_RESOLUTIONXL_REG_RO		0x05
#define ST1633I_TD_RESOLUTIONYL_REG_RO		0x06
#define ST1633I_TD_SENSINGCOUNTERH_REG_RO   0x07
#define ST1633I_TD_SENSINGCOUNTERL_REG_RO	0x08
#define ST1633I_TD_FWREV3_REG_RO			0x0C
#define ST1633I_TD_FWREV2_REG_RO			0x0D
#define ST1633I_TD_FWREV1_REG_RO			0x0E
#define ST1633I_TD_FWREV0_REG_RO			0x0F

#define ST1633I_TD_ADVTOUCHINFO_REG_RO		0x10
#define ST1633I_TD_KEY_REG_RO				0x11

#define ST1633I_TD_CORD_XYH0_REG_RO			0x12
#define ST1633I_TD_CORD_XL0_REG_RO			0x13
#define ST1633I_TD_CORD_YL0_REG_RO			0x14
#define ST1633I_TD_CORD_XYH1_REG_RO			0x16
#define ST1633I_TD_CORD_XL1_REG_RO			0x17
#define ST1633I_TD_CORD_YL1_REG_RO			0x18
#define ST1633I_TD_CORD_XYH2_REG_RO			0x1A
#define ST1633I_TD_CORD_XL2_REG_RO			0x1B
#define ST1633I_TD_CORD_YL2_REG_RO		0x1C
#define ST1633I_TD_CORD_XYH3_REG_RO			0x1E
#define ST1633I_TD_CORD_XL3_REG_RO			0x1F
#define ST1633I_TD_CORD_YL3_REG_RO			0x20
#define ST1633I_TD_CONTACTCOUNT_REG_RO		0x3F
/* Values Pn_XL and Pn_YL related */

/* Values related to ST1633I_CTRL_REG */

/* Will keep the Active mode when there is no touching */
#define ST1633I_CTRL_KEEP_ACTIVE_MODE    0x00

/* Switching from Active mode to Monitor mode automatically when there is no touching */
#define ST1633I_CTRL_KEEP_AUTO_SWITCH_MONITOR_MODE  0x01

/* The time period of switching from Active mode to Monitor mode when there is no touching */
#define ST1633I_TIMEENTERMONITOR_REG     0x87

/* Report rate in Active mode */
#define ST1633I_PERIODACTIVE_REG         0x88

/* Report rate in Monitor mode */
#define ST1633I_PERIODMONITOR_REG        0x89

/* The value of the minimum allowed angle while Rotating gesture mode */
#define ST1633I_RADIAN_VALUE_REG         0x91

/* Maximum offset while Moving Left and Moving Right gesture */
#define ST1633I_OFFSET_LEFT_RIGHT_REG    0x92

/* Maximum offset while Moving Up and Moving Down gesture */
#define ST1633I_OFFSET_UP_DOWN_REG       0x93

/* Minimum distance while Moving Left and Moving Right gesture */
#define ST1633I_DISTANCE_LEFT_RIGHT_REG  0x94

/* Minimum distance while Moving Up and Moving Down gesture */
#define ST1633I_DISTANCE_UP_DOWN_REG     0x95

/* Maximum distance while Zoom In and Zoom Out gesture */
#define ST1633I_DISTANCE_ZOOM_REG        0x96

/* Chip Selecting */
#define ST1633I_ADD_REG                  0x55

/* Current power mode the FT6xx6 system is in (R) */
#define ST1633I_PWR_MODE_REG             0xA5

/* FT6xx6 firmware version */
#define ST1633I_FIRMID_REG               0xA6

/* FT6xx6 Chip identification register */
#define ST1633I_CHIP_ID_REG              0xA8

/*  Possible values of touchscreen controller ID */
#define ST1633I_ID_VALUE                 0x11

#endif

#include <platform/driver/touch/TouchController.hpp>

/**
 * @class STM32TouchController
 *
 * @brief This class specializes TouchController Interface.
 *
 * @sa touchgfx::TouchController
 */

class STM32TouchController : public touchgfx::TouchController
{
public:

    STM32TouchController() {}

    /**
     * @fn virtual void STM32TouchController::init() = 0;
     *
     * @brief Initializes touch controller.
     *
     *        Initializes touch controller.
     */
    virtual void init();

    /**
     * @fn virtual bool STM32TouchController::sampleTouch(int32_t& x, int32_t& y) = 0;
     *
     * @brief Checks whether the touch screen is being touched, and if so, what coordinates.
     *
     *        Checks whether the touch screen is being touched, and if so, what coordinates.
     *
     * @param [out] x The x position of the touch
     * @param [out] y The y position of the touch
     *
     * @return True if a touch has been detected, otherwise false.
     */
    virtual bool sampleTouch(int32_t& x, int32_t& y);
};

typedef struct {
uint16_t x;
uint16_t y;
uint8_t z;
uint8_t ValidFlage;
union {
uint8_t xy_h;
struct __attribute__((packed))
	{
	unsigned	y_h: 3;
	unsigned reserved: 1;
	unsigned x_h: 3;
	unsigned valid: 1;
}xyh_bit;
};

} xyz_data_t;

typedef struct{
	union{
	uint8_t AdvanceTouchInfo;
	struct __attribute__((packed))
		{
		unsigned	Ainfo_gtype: 4;
		unsigned	Ainfo_reserved4 : 1;
		unsigned	Ainfo_WaterFlag :1;
		unsigned	Ainfo_ProximityFlag:1;
		unsigned	Ainfo_reserved7 : 1	;

		}AdvanceTouchInfobits;
	};

}AdvanceTouchInfo_t;

typedef struct {
AdvanceTouchInfo_t tsinfo;
xyz_data_t xyz_data[10];
} touch_data_t;


#ifdef st1x33i
/**
*  @brief TS_StateTypeDef
*  Define TS State structure
*/
typedef struct
{
  uint8_t  touchDetected;                /*!< Total number of active touches detected at last scan */
  uint16_t touchX[TS_MAX_NB_TOUCH];      /*!< Touch X[0], X[1] coordinates on 12 bits */
  uint16_t touchY[TS_MAX_NB_TOUCH];      /*!< Touch Y[0], Y[1] coordinates on 12 bits */

#if (TS_MULTI_TOUCH_SUPPORTED == 1)
  uint8_t  touchWeight[TS_MAX_NB_TOUCH]; /*!< Touch_Weight[0], Touch_Weight[1] : weight property of touches */
  uint8_t  touchEventId[TS_MAX_NB_TOUCH];     /*!< Touch_EventId[0], Touch_EventId[1] : take value of type @ref TS_TouchEventTypeDef */
  uint8_t  touchArea[TS_MAX_NB_TOUCH];   /*!< Touch_Area[0], Touch_Area[1] : touch area of each touch */
  uint32_t gestureId; /*!< type of gesture detected : take value of type @ref TS_GestureIdTypeDef */
#endif  /* TS_MULTI_TOUCH_SUPPORTED == 1 */

} TS_StateTypeDef;

#endif

/**
 *  @brief TS_StatusTypeDef
 *  Define BSP_TS_xxx() functions possible return value,
 *  when status is returned by those functions.
 */
typedef enum
{
  TOUCH_OK                = 0x00, /*!< Touch Ok */
  TOUCH_ERROR             = 0x01, /*!< Touch Error */
} Touch_StatusTypeDef;

/**
 *  @brief TS_GestureIdTypeDef
 *  Define Possible managed gesture identification values returned by touch screen
 *  driver.
 */
typedef enum
{
  GEST_ID_NO_GESTURE = 0x00, /*!< Gesture not defined / recognized */
  GEST_ID_MOVE_UP    = 0x01, /*!< Gesture Move Up */
  GEST_ID_MOVE_RIGHT = 0x02, /*!< Gesture Move Right */
  GEST_ID_MOVE_DOWN  = 0x03, /*!< Gesture Move Down */
  GEST_ID_MOVE_LEFT  = 0x04, /*!< Gesture Move Left */
  GEST_ID_ZOOM_IN    = 0x05, /*!< Gesture Zoom In */
  GEST_ID_ZOOM_OUT   = 0x06, /*!< Gesture Zoom Out */
  GEST_ID_NB_MAX     = 0x07 /*!< max number of gesture id */
} TS_GestureIdTypeDef;

/**
 *  @brief TS_TouchEventTypeDef
 *  Define Possible touch events kind as returned values
 *  by touch screen IC Driver.
 */
typedef enum
{
  TOUCH_EVENT_NO_EVT        = 0x00, /*!< Touch Event : undetermined */
  TOUCH_EVENT_PRESS_DOWN    = 0x01, /*!< Touch Event Press Down */
  TOUCH_EVENT_LIFT_UP       = 0x02, /*!< Touch Event Lift Up */
  TOUCH_EVENT_CONTACT       = 0x03, /*!< Touch Event Contact */
  TOUCH_EVENT_NB_MAX        = 0x04  /*!< max number of touch events kind */
} TS_TouchEventTypeDef;


#ifdef st1x33i
uint8_t 		BSP_TS_Init(uint16_t ts_SizeX, uint16_t ts_SizeY);
void 			ST1633I_TouchDtect (touch_data_t * , uint8_t *);
extern void     TS_IO_Write(uint8_t Addr, uint8_t Reg, uint8_t Value);
extern uint8_t  TS_IO_Read(uint8_t Addr, uint8_t Reg);
extern uint16_t TS_IO_ReadMultiple(uint8_t Addr, uint8_t Reg, uint8_t *Buffer, uint16_t Length);
uint8_t 		BSP_TS_GetState(TS_StateTypeDef *TS_State);
#endif

#if FT5426 || ILI2130 || ILI2511

void getDriver ( void );
void getRange ( void );
void getVersion ( void );

bool FT5426_Read ( int32_t& x, int32_t& y );
bool ILI2130_Read ( int32_t& x, int32_t& y );
bool ILI2511_Read ( int32_t& x, int32_t& y );

#endif

#endif // STM32TOUCHCONTROLLER_HPP

/* USER CODE END STM32TouchController */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
