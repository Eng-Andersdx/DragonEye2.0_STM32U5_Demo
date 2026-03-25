#include <gui/oventempview_screen/OvenTempViewView.hpp>
#include <touchgfx/Utils.hpp>
#include <touchgfx/EasingEquations.hpp>
#include <gui/OvenTempView_Screen/OvenTempViewView.hpp>
#include <gui/common/FrontendApplication.hpp>
#include <touchgfx/Application.hpp>
#include "dragoneye_leds.h"

using namespace touchgfx;

// Limits and behaviour
static const int16_t kMinC = 0;
static const int16_t kMaxC = 270;
static const int16_t kStep = 10;
// Gradient finishes rising at 210°C
static const int16_t kGradientMaxC = 210;

// Y mapping: 0°C -> 115, 210°C -> -590
// deltaY = -590 - 115 = -705
// y = 115 - (celsius * 705) / 210

OvenTempViewView::OvenTempViewView() {}

void OvenTempViewView::setupScreen()
{
    OvenTempViewViewBase::setupScreen();
    BSP_LED_SetColor(RGB_OVEN);

    // Initial state
    currentCelsius = 0;

    // Background circle stays fixed at (0,0)
    Oven_gradient_2.moveTo(0, 0);
    Oven_gradient_2.invalidate();

    // Black gradient starts at Y=115 for 0°C
    blackContainer.moveTo(0, 115);
    blackContainer.invalidate();

    setTemperature(currentCelsius, false);
}

void OvenTempViewView::tearDownScreen()
{
    OvenTempViewViewBase::tearDownScreen();
}

// Round to nearest multiple of 10
int16_t OvenTempViewView::quantize10(int16_t v) const
{
    return (v >= 0) ? (int16_t)((v + 5) / 10 * 10)
                    : (int16_t)((v - 5) / 10 * 10);
}

// Map 0..210°C -> Y: 115..-590; hold -590 for 210..270
int16_t OvenTempViewView::mapTempToY(int16_t celsius) const
{
    if (celsius <= kGradientMaxC)
    {
        // y = 115 - (celsius * 705) / 210
        return (int16_t)(115 - (celsius * 705) / 210);
    }
    return -590;
}

void OvenTempViewView::updateNumber(int16_t celsius)
{
    // Only digits; the "°" is added by the text key (<>°)
    Unicode::snprintf(tempBuffer,
                      sizeof(tempBuffer) / sizeof(tempBuffer[0]),
                      "%d",
                      celsius);
    Degree.setWildcard(tempBuffer);
    Degree.invalidate();

    // ---- Fahrenheit text ----
    // F = C * 9 / 5 + 32
    int16_t fahrenheit = static_cast<int16_t>((celsius * 9) / 5 + 32);

    Unicode::snprintf(fahrenheitBuffer,
                      sizeof(fahrenheitBuffer) / sizeof(fahrenheitBuffer[0]),
                      "%d",
                      fahrenheit);
    Fahrenheit.setWildcard(fahrenheitBuffer);
    Fahrenheit.invalidate();
}

void OvenTempViewView::setTemperature(int16_t celsius, bool animate)
{
    // Clamp and snap to 10°
    celsius = quantize10(celsius);
    if (celsius < kMinC) celsius = kMinC;
    if (celsius > kMaxC) celsius = kMaxC;

    currentCelsius = celsius;
    updateNumber(currentCelsius);

    const int16_t targetY = mapTempToY(currentCelsius);

    if (animate)
    {
        // Smooth eased slide (~14 frames; tweak as you like)
        blackContainer.startMoveAnimation(
            0,                       // X stays 0
            targetY,                 // Y target
            14,                      // frames
            EasingEquations::cubicEaseInOut
        );
    }
    else
    {
        blackContainer.moveTo(0, targetY);
        blackContainer.invalidate();
    }
}

void OvenTempViewView::increaseTemp()
{
    setTemperature(currentCelsius + kStep, true);
}

void OvenTempViewView::decreaseTemp()
{
    setTemperature(currentCelsius - kStep, true);
}

// Button callbacks (wire in Designer → Interactions → Call function)
void OvenTempViewView::onPlusClicked()
{
    increaseTemp();
}

void OvenTempViewView::onMinusClicked()
{
    decreaseTemp();
}

void OvenTempViewView::H_Left()
{
    // behave exactly like the minus button
    onMinusClicked();
}

void OvenTempViewView::H_Right()
{
    // behave exactly like the plus button
    onPlusClicked();
}
