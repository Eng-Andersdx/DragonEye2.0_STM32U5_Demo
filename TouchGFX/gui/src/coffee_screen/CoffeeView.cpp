#include <gui/coffee_screen/CoffeeView.hpp>
#include <touchgfx/Color.hpp>
#include <touchgfx/Bitmap.hpp>
#include <touchgfx/mixins/FadeAnimator.hpp>
#include <touchgfx/widgets/AnimatedImage.hpp>
#include <BitmapDatabase.hpp>
#include "dragoneye_leds.h"

using namespace touchgfx;

CoffeeView::CoffeeView() {}

void CoffeeView::setupScreen()
{
    CoffeeViewBase::setupScreen();
    BSP_LED_SetColor(RGB_COFFEE);

    // Fixed assumption for simulator
    fps = 60;

    // Timings
    hintDelayTicks = ticksFromMs(700); // show hint after 1.5s
    pulseGapTicks  = ticksFromMs(2000); // still unused, but member kept
    tickHoldTarget = ticksFromMs(2000); // 2s hold on last tick frame

    // Don’t intercept touches on tick animation
    tick_animation.setTouchable(false);

    // Initial alphas
    title_coffee.setAlpha(255);
    tap_hint.setAlpha(0);

    // Start with icon hidden – it will fade in with the hint
    coffee_icon.setAlpha(0);
    coffee_icon.invalidate();

    tick_animation.setAlpha(0);

    // Hook animation end callback for tick
    tickAnimDoneCb = Callback<CoffeeView, const AnimatedImage&>(
        this, &CoffeeView::onTickAnimDone);
    tick_animation.setDoneAction(tickAnimDoneCb);

    // Ensure stopped at entry
    tick_animation.stopAnimation();

    // Set initial background / title / icon (no fade on first entry)
    updateCoffeeDisplay(false);
    // Hint + icon will fade in after delay
    scheduleHint();
}

void CoffeeView::tearDownScreen()
{
    CoffeeViewBase::tearDownScreen();
}

void CoffeeView::handleTickEvent()
{
    CoffeeViewBase::handleTickEvent();

    // After delay, fade in hint AND icon once
    if (hintWaiting)
    {
        if (++hintWaitTicks >= hintDelayTicks)
        {
            hintWaiting = false;

            tap_hint.startFadeAnimation(255, 20);     // fade in hint
            coffee_icon.startFadeAnimation(255, 20);  // fade in icon same way
        }
    }

    // Tick holding last frame then fading out tick + hint together
    if (tickHoldActive)
    {
        if (++tickHoldTicks >= tickHoldTarget)
        {
            tickHoldActive = false;

            // Fade out tick + hint
            tick_animation.startFadeAnimation(0, 20);
            tap_hint.startFadeAnimation(0, 20);

            // Bring icon back in (selection acknowledged, back to idle)
            coffee_icon.startFadeAnimation(255, 20);
        }
    }
}

// ---------------- Designer button callbacks ----------------

void CoffeeView::onNextClicked()
{
    current = (current + 1 >= COFFEE_COUNT) ? ESPRESSO
                                            : static_cast<CoffeeType>(current + 1);
    updateCoffeeDisplay(true);   // background + title
    scheduleHint();              // handles icon + hint timing
}

void CoffeeView::onPrevClicked()
{
    current = (current == ESPRESSO) ? static_cast<CoffeeType>(COFFEE_COUNT - 1)
                                    : static_cast<CoffeeType>(current - 1);
    updateCoffeeDisplay(true);   // background + title
    scheduleHint();              // handles icon + hint timing
}

void CoffeeView::tap_areaClicked()   // EXACT Designer name
{
    // Stop hint immediately and hide it
    hintWaiting   = false;
    hintWaitTicks = 0;
    tap_hint.startFadeAnimation(0, 10);

    // Fade the icon out quickly on tap
    coffee_icon.startFadeAnimation(0, 10);

    // Start the tick once, then we hold on last frame via tickHoldActive
    tick_animation.setAlpha(255);
    tick_animation.invalidate();
    tick_animation.stopAnimation(); // ensure reset
    tick_animation.startAnimation(false /*rev*/, true /*reset*/, false /*loop*/);
}

// ---------------- Internals ----------------

void CoffeeView::updateCoffeeDisplay(bool animate)
{
    BitmapId bgBmp    = BITMAP_ESPRESSO_GRAD_ID;
    BitmapId iconBmp  = BITMAP_ESPRESSO_ICON_ID;
    uint16_t textId   = T_T_ESPRESSO;

    switch (current)
    {
    case ESPRESSO:
        bgBmp   = BITMAP_ESPRESSO_GRAD_ID;
        iconBmp = BITMAP_ESPRESSO_ICON_ID;
        textId  = T_T_ESPRESSO;
        break;

    case AMERICANO:
        bgBmp   = BITMAP_AMERICANO_GRAD_ID;
        iconBmp = BITMAP_AMERICANO_ICON_ID;
        textId  = T_T_AMERICANO;
        break;

    case CAPPUCCINO:
        bgBmp   = BITMAP_CAPPUCCINO_GRAD_ID;
        iconBmp = BITMAP_CAPPUCCINO_ICON_ID;
        textId  = T_T_CAPPUCCINO;
        break;

    case LATTE:
        bgBmp   = BITMAP_LATTE_GRAD_ID;
        iconBmp = BITMAP_LATTE_ICON_ID;
        textId  = T_T_LATTE;
        break;

    case MOCHA:
        bgBmp   = BITMAP_MOCHA_GRAD_ID;
        iconBmp = BITMAP_MOCHA_ICON_ID;
        textId  = T_T_MOCHA;
        break;

    // WHITE_COFFEE background has been removed.
    // If WHITE_COFFEE still exists in the enum, we just map it to
    // an existing gradient to avoid missing bitmap errors.
    case WHITE_COFFEE:
        bgBmp   = BITMAP_LATTE_GRAD_ID;      // or another gradient you prefer
        iconBmp = BITMAP_LATTE_ICON_ID;      // pick a reasonable fallback
        textId  = T_T_WHITE_COFFEE;         // keep text if still in use
        break;

    case HOT_WATER:
        bgBmp   = BITMAP_HOT_WATER_GRAD_ID;
        iconBmp = BITMAP_HOT_WATER_ICON_ID;
        textId  = T_T_HOT_WATER;
        break;

    case COFFEE_COUNT:
    default:
        break; // silence -Wswitch
    }

    // Update the coffee icon to match the background
    coffee_icon.setBitmap(Bitmap(iconBmp));
    coffee_icon.invalidate();

    // Title cross-fade
    title_coffee.startFadeAnimation(0, 20);
    updateTitle(textId);
    title_coffee.startFadeAnimation(255, 20);

    // Background cross-fade (you liked ~60 ticks)
    if (animate)
    {
        fadeTo(bgBmp, 60);
    }
    else
    {
        if (frontShowing)
        {
            imgFront.setBitmap(Bitmap(bgBmp));
            imgFront.setAlpha(255);
            imgBack.setAlpha(0);
            imgFront.invalidate();
            imgBack.invalidate();
        }
        else
        {
            imgBack.setBitmap(Bitmap(bgBmp));
            imgBack.setAlpha(255);
            imgFront.setAlpha(0);
            imgBack.invalidate();
            imgFront.invalidate();
        }
    }
}

void CoffeeView::updateTitle(uint16_t textKey)
{
    title_coffee.setTypedText(TypedText(textKey));
    title_coffee.invalidate();
}

void CoffeeView::fadeTo(BitmapId bmp, int durationTicks)
{
    if (frontShowing)
    {
        imgBack.setBitmap(Bitmap(bmp));
        imgBack.setAlpha(0);
        imgBack.invalidate();

        imgFront.startFadeAnimation(0, durationTicks);
        imgBack.startFadeAnimation(255, durationTicks);

        frontShowing = false;
    }
    else
    {
        imgFront.setBitmap(Bitmap(bmp));
        imgFront.setAlpha(0);
        imgFront.invalidate();

        imgBack.startFadeAnimation(0, durationTicks);
        imgFront.startFadeAnimation(255, durationTicks);

        frontShowing = true;
    }
}

void CoffeeView::scheduleHint()
{
    // Hide hint & icon NOW and re-arm delayed show
    tap_hint.setAlpha(0);
    tap_hint.invalidate();

    coffee_icon.setAlpha(0);
    coffee_icon.invalidate();

    hintWaiting   = true;
    hintWaitTicks = 0;

    // Also cancel any pending tick fade/hold if user navigates
    tickHoldActive = false;
    tick_animation.startFadeAnimation(0, 10);
}

void CoffeeView::onTapAnimDone(const AnimatedImage& /*sender*/)
{
    // tap_animation has been removed; keep empty stub so header/linking stays happy
}

void CoffeeView::onTickAnimDone(const AnimatedImage& /*sender*/)
{
    // Tick reached final frame → begin hold period
    tickHoldActive = true;
    tickHoldTicks  = 0;
}

void CoffeeView::goLeft()
{
    // behave exactly like the left arrow (previous coffee)
    onPrevClicked();
}

void CoffeeView::goRight()
{
    // behave exactly like the right arrow (next coffee)
    onNextClicked();
}
