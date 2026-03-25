#include "KeySampler.hpp"
#include "touchgfx/hal/Buttons.hpp"
#include "string.h"
#include "main.h"
#include "dragoneye_encoder.h"

using namespace touchgfx;

extern "C" void             BSP_PB_Init(Button_TypeDef Button, ButtonMode_TypeDef Button_Mode);
extern "C" uint32_t         BSP_PB_GetState(Button_TypeDef Button);
extern "C" void             BSP_ENC_ResetRotationValue();
extern "C" volatile uint8_t haptic_feedback;

using namespace touchgfx;
uint32_t PrevState_BLeft=1;
uint32_t PrevState_BRight=1;

void KeySampler::init()
{
	BSP_PB_Init(BUTTON_LEFT, BUTTON_MODE_EXTI);
	BSP_PB_Init(BUTTON_PUSH, BUTTON_MODE_EXTI);
}

bool KeySampler::sample(uint8_t& key)
{
	 uint32_t result = 0;

	 result=BSP_PB_GetState(BUTTON_LEFT);
	 if(result==0 && PrevState_BLeft != result){
		 haptic_feedback = 1;
		 PrevState_BLeft = result;
		 BSP_ENC_ResetRotationValue();
		 key=0;
	     return true;
	 }
	 PrevState_BLeft = result;

	 result=BSP_PB_GetState(BUTTON_RIGHT);
	 if(result==0 && PrevState_BRight != result){
		haptic_feedback = 1;
	 	key=1;
	   	PrevState_BRight = result;
	   	BSP_ENC_ResetRotationValue();
	    return true;
	 }
	 PrevState_BRight = result;

	 return false;
}
