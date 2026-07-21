#include <gui/watch_screen/WatchView.hpp>

#include <gui/common/Constants.hpp>
#include <gui/common/FloatEasingEquations.hpp>

#include "BitmapDatabase.hpp"
#include <texts/TextKeysAndLanguages.hpp>
#include <touchgfx/Color.hpp>

#include "touchgfx/Utils.hpp"

#include <initializer_list>


WatchView::WatchView() : hour(12), minute(15), second(30), clockRunning(false), animateCircleIntro(false), animateAnalogClockIntro(false), animateBoxIntro(false)
{

}

void WatchView::setupScreen()
{
    WatchViewBase::setupScreen();

    analogClock.setXY(0, 0);
    analogClock.setRotationCenter(240, 240);
    analogClock.setWidthHeight(480, 480);
    analogClock.setupSecondHand(BITMAP_SECONDS_INDICATOR_ID, 6, 219);
    analogClock.setupMinuteHand(BITMAP_MINUTES_CIRCLE_ID, 37, 215);
    analogClock.setupHourHand(BITMAP_HOURS_DOT_ID, 37, 215);
    analogClock.setMinuteHandSecondCorrection(true);
    analogClock.setHourHandMinuteCorrection(true);
    analogClock.initializeTime24Hour(hour, minute, second);
    analogClock.setAnimation();
    analogClock.setAlpha(0);

    insert(&cube, analogClock);

}

void WatchView::tearDownScreen()
{
    WatchViewBase::tearDownScreen();
}

void WatchView::handleTickEvent()
{
    if (clockCounter >= 60)
    {
        if (++second >= 60)
        {
            second = 0;
            if (++minute >= 60)
            {
                minute = 0;
                if (++hour >= 24)
                {
                    hour = 0;
                }
            }
        }

        analogClock.setTime24Hour(hour, minute, second);
        cube.updateDigitalClock(hour, minute, second);
        clockCounter = 0;
    }
    else
    {
        clockCounter++;
    }

    if (animateCircleIntro)
    {
        float scale = FloatEasingEquations::easeNone((float)introAnimationsCounter, 1.2f, -0.2f, 50.f);

        backgroundRingTextureMapper.invalidate();
        backgroundRingTextureMapper.setScale(scale);

        introAnimationsCounter++;

        if (introAnimationsCounter > 50)
        {
            backgroundRingTextureMapper.setVisible(false);
            backgroundRingImage.setVisible(true);
            backgroundRingImage.invalidate();

            animateCircleIntro = false;
            animateAnalogClockIntro = true;
            introAnimationsCounter = 0;
            analogClock.startFadeAnimation(255, 30);
        }
    }

    else if (animateAnalogClockIntro)
    {
        float angle = FloatEasingEquations::easeNone((float)introAnimationsCounter, 0, Constants::PI_HALF, 50.f);

        analogClock.setMinuteZAngle(angle);
        analogClock.setSecondsZAngle(Constants::PI_HALF + angle);

        introAnimationsCounter++;

        if (introAnimationsCounter == 35)
        {
            animateBoxIntro = true;
            boxTextureMapper.setVisible(true);
            introAnimationBoxCounter = 0;
        }
        else if (introAnimationsCounter > 50)
        {
            animateAnalogClockIntro = false;
            clockRunning = true;
        }
    }
    if (animateBoxIntro)
    {
        float scale = FloatEasingEquations::easeNone((float)introAnimationBoxCounter, 0.f, 1.f, 50.f);

        boxTextureMapper.invalidate();
        boxTextureMapper.setScale(scale);

        introAnimationBoxCounter++;

        if (introAnimationBoxCounter == 25)
        {
            cube.startCubeAnimation();
        }
        else if (introAnimationBoxCounter > 50)
        {
            animateBoxIntro = false;
            introAnimationBoxCounter = 0;

            boxTextureMapper.setVisible(false);
            boxImage.setVisible(true);
            boxImage.invalidate();
        }
    }
}

void WatchView::startIntroAnimation()
{
    introAnimationsCounter = 0;
    animateCircleIntro = true;
}
