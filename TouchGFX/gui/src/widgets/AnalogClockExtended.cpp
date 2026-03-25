/**
  ******************************************************************************
  * This file is part of the TouchGFX 4.13.0 distribution.
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

#include <gui/widgets/AnalogClockExtended.hpp>

namespace touchgfx
{
AnalogClockExtended::AnalogClockExtended()
{
}

AnalogClockExtended::~AnalogClockExtended()
{
}

void AnalogClockExtended::setAlpha(uint8_t alpha)
{
    hourHand.setAlpha(alpha);
    minuteHand.setAlpha(alpha);
    secondHand.setAlpha(alpha);

    background.setAlpha(alpha);
}
uint8_t AnalogClockExtended::getAlpha() const
{
    return hourHand.getAlpha();
}
void AnalogClockExtended::setMinuteZAngle(float angle)
{
    minuteHand.updateZAngle(angle);
}
void AnalogClockExtended::setSecondsZAngle(float angle)
{
    secondHand.updateZAngle(angle);
}
}
