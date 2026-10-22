#include <gui/oventempview_screen/OvenTempViewView.hpp>
#include <touchgfx/Utils.hpp>
#include <touchgfx/EasingEquations.hpp>
#include <gui/OvenTempView_Screen/OvenTempViewView.hpp>
#include <gui/common/FrontendApplication.hpp>
#include <touchgfx/Application.hpp>
#include <gui/common/HwInterface.hpp>

extern "C" volatile uint8_t haptic_continuous;
extern int LED_mode;

using namespace touchgfx;

// Limits and behaviour
static const int16_t kMinC = 0;
static const int16_t kMaxC = 270;
static const int16_t kStep = 10;
// Gradient finishes rising at 210°C
static const int16_t kGradientMaxC = 210;

// Heat glow: starts appearing at this temperature, full at kMaxC
static const int16_t kGlowStartC     = 180;
static const int16_t kGlowMaxAlpha   = 230;   // at kMaxC
static const int16_t kGlowBreathLow  = 150;   // breathing range while at kMaxC
static const int16_t kGlowFadeStep   = 6;     // alpha change per frame (smooth fade)
static const uint16_t kBreathPeriod  = 90;    // frames per breath (~1.5 s at 60 fps)

// Y mapping: 0°C -> 115, 210°C -> -590
// deltaY = -590 - 115 = -705
// y = 115 - (celsius * 705) / 210

OvenTempViewView::OvenTempViewView() {}

void OvenTempViewView::setupScreen()
{
    OvenTempViewViewBase::setupScreen();
    // LED colour is set from the temperature in setTemperature() below

    // Initial state
    currentCelsius = 0;

    // Heat glow hidden until we get hot
    glowAlpha = 0;
    glowTarget = 0;
    glowPhase = 0;
    heat_glow.setAlpha(0);
    heat_glow.invalidate();

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
    // Never leave the motor running when leaving this screen
    haptic_continuous = 0;
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

void OvenTempViewView::updateGlowTarget()
{
    if (currentCelsius <= kGlowStartC)
    {
        glowTarget = 0;
    }
    else
    {
        glowTarget = (int16_t)(((int32_t)(currentCelsius - kGlowStartC) * kGlowMaxAlpha) / (kMaxC - kGlowStartC));
    }
    glowPhase = 0;
}

void OvenTempViewView::handleTickEvent()
{
    OvenTempViewViewBase::handleTickEvent();

    int16_t target = glowTarget;

    // At max temperature the glow slowly breathes (in step with the continuous haptic)
    if (currentCelsius >= kMaxC)
    {
        glowPhase = (uint16_t)((glowPhase + 1) % kBreathPeriod);
        const uint16_t half = kBreathPeriod / 2;
        const uint16_t tri  = (glowPhase < half) ? glowPhase : (uint16_t)(kBreathPeriod - glowPhase); // 0..half..0
        target = (int16_t)(kGlowMaxAlpha - ((int32_t)(kGlowMaxAlpha - kGlowBreathLow) * tri) / half);
    }

    // Move smoothly toward the target
    int16_t next = glowAlpha;
    if (next < target)      { next += kGlowFadeStep; if (next > target) next = target; }
    else if (next > target) { next -= kGlowFadeStep; if (next < target) next = target; }

    if (next != glowAlpha)
    {
        glowAlpha = next;
        heat_glow.setAlpha((uint8_t)glowAlpha);
        heat_glow.invalidate();
    }
}

// Blend LED from RGB_YELLOW (kMinC) to RGB_RED (kMaxC), channel by channel
void OvenTempViewView::updateLedColor(int16_t celsius)
{
    if (LED_mode != 1)
        return; // Fixed / Off mode chosen in Settings

    const uint32_t from = RGB_YELLOW;
    const uint32_t to   = RGB_RED;
    const int32_t span  = kMaxC - kMinC;
    const int32_t pos   = celsius - kMinC;

    uint32_t color = 0;
    for (int shift = 0; shift <= 16; shift += 8)
    {
        const int32_t a = (int32_t)((from >> shift) & 0xFF);
        const int32_t b = (int32_t)((to   >> shift) & 0xFF);
        const int32_t c = a + ((b - a) * pos) / span;
        color |= ((uint32_t)c & 0xFF) << shift;
    }
    BSP_LED_SetColor(color);
}

void OvenTempViewView::setTemperature(int16_t celsius, bool animate)
{
    // Clamp and snap to 10°
    celsius = quantize10(celsius);
    if (celsius < kMinC) celsius = kMinC;
    if (celsius > kMaxC) celsius = kMaxC;

    currentCelsius = celsius;
    updateNumber(currentCelsius);
    updateLedColor(currentCelsius);
    updateGlowTarget();

    // Continuous haptic at max temp; only released once back down to 260°C
    if (currentCelsius >= kMaxC)
    {
        haptic_continuous = 1;
    }
    else if (currentCelsius <= kMaxC - kStep)
    {
        haptic_continuous = 0;
    }

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

// ---------- Navigation (animated, see FrontendApplication) ----------

void OvenTempViewView::goBack()
{
    application().gotoHome();
}
