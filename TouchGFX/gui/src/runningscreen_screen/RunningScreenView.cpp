#include <gui/runningscreen_screen/RunningScreenView.hpp>
#include <touchgfx/EasingEquations.hpp>
#include <BitmapDatabase.hpp>
#include <cmath>
#include "dragoneye_leds.h"

using namespace touchgfx;

RunningScreenView::RunningScreenView() {}

void RunningScreenView::setupScreen()
{
    RunningScreenViewBase::setupScreen();
    BSP_LED_SetColor(RGB_GREEN);

    // Start at speed 0
    currentSpeed = 0;
    updateDigits(currentSpeed);

    // Bar visuals:
    Speed_bar_empty.setAlpha(255);
    Speed_bar_empty.invalidate();

    Speed_bar_full.setAlpha(255);   // make sure it’s visible
    currentArcEnd = -100;
    targetArcEnd  = -100;
    arcAnimating  = false;

    // Draw initial arc for speed 0
    Speed_bar_full.updateArcEnd(-100);   // 0 bars
    Speed_bar_full.invalidate();
}

void RunningScreenView::tearDownScreen()
{
    RunningScreenViewBase::tearDownScreen();
}

void RunningScreenView::handleTickEvent()
{
    RunningScreenViewBase::handleTickEvent();

    // We no longer animate through 79 frames, but keep the stub
    // so the header declaration is still satisfied.
    stepFrameAnim();
}

// ---------------- UI events ----------------

void RunningScreenView::onPlusClicked()
{
    if (currentSpeed < 10)
    {
        setSpeed(static_cast<uint8_t>(currentSpeed + 1));
    }
}

void RunningScreenView::onMinusClicked()
{
    if (currentSpeed > 0)
    {
        setSpeed(static_cast<uint8_t>(currentSpeed - 1));
    }
}

void RunningScreenView::setSpeed(uint8_t value)
{
    if (value > 10)
        value = 10;
    if (value == currentSpeed)
        return;

    currentSpeed = value;
    updateDigits(currentSpeed);

    // Directly request bar update + animation for this speed
    requestBarFrameForSpeed(currentSpeed);
}

// --------------- helpers -------------------

void RunningScreenView::updateDigits(uint8_t value)
{
    Unicode::snprintf(numBuf,
                      sizeof(numBuf) / sizeof(numBuf[0]),
                      "%d",
                      value);
    Speed_value.setWildcard(numBuf);
    Speed_value.invalidate();
}

// Keep this function to satisfy the header; its return value
// is no longer important, but we implement it safely.
uint8_t RunningScreenView::speedToFrame(uint8_t speed) const
{
    if (speed > 10)
        speed = 10;
    return speed;
}

void RunningScreenView::requestBarFrameForSpeed(uint8_t speed)
{
    // Always show the empty ring
    Speed_bar_empty.setAlpha(255);
    Speed_bar_empty.invalidate();

    if (speed > 10)
        speed = 10;

    // Map 0..10 → -100..100  (same as before)
    int16_t newEnd = static_cast<int16_t>(-100 + speed * 20);

    targetArcEnd = newEnd;

    // Start / continue animation
    if (targetArcEnd != currentArcEnd)
    {
        arcAnimating = true;
    }
}

// Kept as stubs to match existing declarations in the header

void RunningScreenView::startFrameAnim(uint8_t /*dest*/)
{
    // No frame-based animation anymore.
}

void RunningScreenView::stepFrameAnim()
{
    if (!arcAnimating)
        return;

    // How fast the bar grows/shrinks per tick (angle units)
    const int16_t step = 3;   // tweak: 2 = slower/smoother, 8 = snappier

    if (currentArcEnd < targetArcEnd)
    {
        currentArcEnd += step;
        if (currentArcEnd > targetArcEnd)
            currentArcEnd = targetArcEnd;
    }
    else if (currentArcEnd > targetArcEnd)
    {
        currentArcEnd -= step;
        if (currentArcEnd < targetArcEnd)
            currentArcEnd = targetArcEnd;
    }

    Speed_bar_full.updateArcEnd(currentArcEnd);
    Speed_bar_full.invalidate();

    if (currentArcEnd == targetArcEnd)
    {
        arcAnimating = false;
    }
}

void RunningScreenView::H_Left()
{
    // behave exactly like the minus button
    onMinusClicked();
}

void RunningScreenView::H_Right()
{
    // behave exactly like the plus button
    onPlusClicked();
}
