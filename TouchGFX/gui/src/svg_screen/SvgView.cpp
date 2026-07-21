#include <gui/svg_screen/SvgView.hpp>
#include <touchgfx/Utils.hpp>
#include <math.h>

SvgView::SvgView() :
    animationStep(introScaleOutAnimationInit_enum),
    tickCount(0),
    dogAnimationtickCount(0),
    paletteAnimationtickCount(0),
    cameraAnimationtickCount(0),
    inMovement(false),
    scaleUpDown(1),
    startPositionX(0),
    startPositionY(0),
    scaleCoeff(0)
{

}

void SvgView::setupScreen()
{
    SvgViewBase::setupScreen();
    HAL::getInstance()->setFrameRateCompensation(true);
    stLogoSvg.setVisible(false);
    stLogoSvg.invalidate();
}

void SvgView::tearDownScreen()
{
    SvgViewBase::tearDownScreen();
}

void SvgView::introScaleOutAnimationInit()
{
    stLogoSvg.setScaleX(4.f);
    stLogoSvg.setScaleY(4.f);
    stLogoSvg.resizeToCurrentSVG();
    stLogoSvg.setX((CANVAS_WIDTH - stLogoSvg.getWidth()) / 2);
    stLogoSvg.setY((CANVAS_HEIGTH - stLogoSvg.getHeight()) / 2);
    stLogoSvg.setVisible(true);
    stLogoSvg.invalidate();
    backroundImage.setFadeAnimationDelay(50);
    changeState(introScaleOutAnimation_enum);
}

void SvgView::introScaleOutAnimation()
{
    stLogoSvg.invalidate();
    if (stLogoSvg.getHeight() > stLogoSvg.getImageHeight())
    {
        stLogoSvg.setScale(0.963f * stLogoSvg.getScaleX());
        stLogoSvg.resizeToCurrentSVG();
        stLogoSvg.setX((CANVAS_WIDTH - stLogoSvg.getWidth()) / 2);
        stLogoSvg.setY((CANVAS_HEIGTH - stLogoSvg.getHeight()) / 2);
        stLogoSvg.invalidate();
    }
    else
    {
        backroundImage.startFadeAnimation(FADE_ANIMATION_ALPHA, FADE_ANIMATION_DURATION, EasingEquations::cubicEaseOut);
        changeState(introScaleOutAnimationEnd_enum);
    }
}

void SvgView::introScaleOutAnimationEnd()
{
    if (!backroundImage.isFadeAnimationRunning())
    {
        stLogoSvg.setVisible(false);
        stLogoSvg.invalidate();
        whiteBackgroundBox.setVisible(false);
        whiteBackgroundBox.invalidate();
        changeState(iconEntranceAnimationInit_enum);
    }
}
void SvgView::iconEntranceAnimationInit()
{

    dogSvg.setScale(HALF_SCALE);
    paletteSvg.setScale(HALF_SCALE);
    cameraSvg.setScale(HALF_SCALE);

    dogSvg.resizeToCurrentSVG();
    paletteSvg.resizeToCurrentSVG();
    cameraSvg.resizeToCurrentSVG();

    dogSvg.invalidate();
    paletteSvg.invalidate();
    cameraSvg.invalidate();

    dogSvg.setMoveAnimationDelay(0);
    paletteSvg.setMoveAnimationDelay(70);
    cameraSvg.setMoveAnimationDelay(140);

    changeState(iconEntranceAnimation_enum);
}

void SvgView::iconEntranceAnimation()
{
    dogSvg.startMoveAnimation(dogSvg.getX() + 430, dogSvg.getY(), MOVE_ANIMATION_DURATION, EasingEquations::cubicEaseOut);
    paletteSvg.startMoveAnimation(dogSvg.getX() + 430, paletteSvg.getY(), MOVE_ANIMATION_DURATION, EasingEquations::cubicEaseOut);
    cameraSvg.startMoveAnimation(dogSvg.getX() + 430, cameraSvg.getY(), MOVE_ANIMATION_DURATION, EasingEquations::cubicEaseOut);

    changeState(iconEntranceAnimationEnd_enum);
}

void SvgView::iconEntranceAnimationEnd()
{
    changeState(iconCircleMoveAnimationInit_enum);
}

void SvgView::iconCircleMoveAnimationInit()
{
    changeState(iconCircleMoveAnimation_enum);
}

void SvgView::iconCircleMoveAnimation()
{

    if (!dogSvg.isMoveAnimationRunning())
    {
        circleMovement(dogSvg, dogAnimationtickCount, (PI), (PI / 3));
    }

    if (!paletteSvg.isMoveAnimationRunning())
    {
        circleMovement(paletteSvg, paletteAnimationtickCount, (5 * PI / 6), (PI / 3));
    }

    if (!cameraSvg.isMoveAnimationRunning())
    {
        inMovement = circleMovement(cameraSvg, cameraAnimationtickCount, (2 * PI / 3), (PI / 3));
        if (!inMovement)
        {
            changeState(iconCircleMoveAnimationEnd_enum);
            dogAnimationtickCount = 0;
            cameraAnimationtickCount = 0;
            paletteAnimationtickCount = 0;
        }
    }
}

void SvgView::iconCircleMoveAnimationEnd()
{
    changeState(dogAnimationInit_enum);

}
void SvgView::dogAnimationInit()
{
    changeState(dogAnimation_enum);
}
void SvgView::dogAnimation()
{

    dogSvg.invalidate();
    if (dogAnimationtickCount < ANIMATION_STEP_DURATION)
    {
        dogSvg.setScale(dogSvg.getScaleX() + 0.25f * sinf(dogAnimationtickCount * PI / ANIMATION_STEP_DURATION) / (2 * dogSvg.getScaleX()));
        dogSvg.resizeToCurrentSVG();
        dogSvg.setX(((-dogSvg.getWidth() + CANVAS_WIDTH) / 2) - (180 - 6 * dogAnimationtickCount));
        dogSvg.setY((-dogSvg.getHeight() + CANVAS_HEIGTH) / 2 - 6);
        dogSvg.invalidate();
    }
    else if (dogAnimationtickCount < 40)
    {
        dogSvg.setX(dogSvg.getX() + 7);
        dogSvg.invalidate();
    }
    else
    {
        dogSvg.setScale(dogSvg.getScaleX() * 0.93f);
        dogSvg.resizeToCurrentSVG();
        dogSvg.setX(dogSvg.getX() + 13);
        dogSvg.setY((-dogSvg.getHeight() + CANVAS_HEIGTH) / 2);

        dogSvg.invalidate();

        if (dogSvg.getX() > CANVAS_WIDTH)
        {
            changeState(dogAnimationEnd_enum);
        }
    }

    dogAnimationtickCount++;
}

void SvgView::dogAnimationEnd()
{
    dogSvg.setVisible(false);
    dogSvg.invalidate();
    changeState(paletteAnimationInit_enum);
}

void SvgView::paletteAnimationInit()
{
    changeState(paletteAnimation_enum);
    startPositionX = paletteSvg.getX();
    startPositionY = paletteSvg.getY();
    scaleCoeff = (MEDIUM_SCALE - paletteSvg.getScaleX()) / (ANIMATION_STEP_DURATION - 1);
}
void SvgView::paletteAnimation()
{
    paletteSvg.invalidate();

    if (paletteAnimationtickCount < ANIMATION_STEP_DURATION)
    {
        paletteSvg.setX(startPositionX + (AFTER_SCALE_CENTER_POSITION_X - startPositionX) * (1 + paletteAnimationtickCount) / ANIMATION_STEP_DURATION);
        paletteSvg.setY(startPositionY + (AFTER_SCALE_CENTER_POSITION_Y - startPositionY) * (1 + paletteAnimationtickCount) / ANIMATION_STEP_DURATION);
        paletteSvg.setScale(paletteSvg.getScaleX() + scaleCoeff);
        paletteSvg.resizeToCurrentSVG();
        paletteSvg.setRotationCenter((float)paletteSvg.getWidth() / 2, (float)paletteSvg.getHeight() / 2);
        paletteSvg.invalidate();
        paletteAnimationtickCount++;
    }
    else
    {
        if (paletteSvg.getRotation() < 3 * FULL_ROTATION_ANGLE)
        {
            paletteSvg.setRotation(paletteSvg.getRotation() + 12);
        }
        else
        {
            if (paletteSvg.getX() <= CANVAS_WIDTH)
            {
                paletteSvg.setX(paletteSvg.getX() + 15);
            }
            else
            {
                changeState(paletteAnimationEnd_enum);
            }
        }
        paletteSvg.invalidate();
    }
}

void SvgView::paletteAnimationEnd()
{
    paletteSvg.setVisible(false);
    paletteSvg.invalidate();

    changeState(cameraAnimationInit_enum);
}

void SvgView::cameraAnimationInit()
{
    changeState(cameraAnimation_enum);
    startPositionX = cameraSvg.getX();
    startPositionY = cameraSvg.getY();
    scaleCoeff = (MEDIUM_SCALE - cameraSvg.getScaleX()) / (ANIMATION_STEP_DURATION - 1);
}

void SvgView::cameraAnimation()
{

    cameraSvg.invalidate();

    if (cameraAnimationtickCount < ANIMATION_STEP_DURATION)
    {
        cameraSvg.setX(startPositionX + (AFTER_SCALE_CENTER_POSITION_X - startPositionX) * (1 + cameraAnimationtickCount) / ANIMATION_STEP_DURATION);
        cameraSvg.setY(startPositionY + (AFTER_SCALE_CENTER_POSITION_Y - startPositionY) * (1 + cameraAnimationtickCount) / ANIMATION_STEP_DURATION);
        cameraSvg.setScale(cameraSvg.getScaleX() + scaleCoeff);
        cameraSvg.resizeToCurrentSVG();
        cameraSvg.setRotationCenter((float)cameraSvg.getWidth() / 2, (float)cameraSvg.getHeight() / 2);
        cameraSvg.invalidate();
        cameraAnimationtickCount++;
    }
    else
    {
        if (cameraSvg.getRotation() > - FULL_ROTATION_ANGLE)
        {
            cameraSvg.setRotation(cameraSvg.getRotation() - 15);
        }
        else
        {
            if (cameraAnimationtickCount < 2 * ANIMATION_STEP_DURATION)
            {
                cameraAnimationtickCount++;
                cameraSvg.setScale(cameraSvg.getScaleX() - 0.2f * sinf(cameraAnimationtickCount * 2 * PI / (2 * ANIMATION_STEP_DURATION)) / (3.f * cameraSvg.getScaleX()));
                cameraSvg.resizeToCurrentSVG();
                cameraSvg.setX((CANVAS_WIDTH - cameraSvg.getHeight()) / 2);
                cameraSvg.setY((CANVAS_HEIGTH - cameraSvg.getWidth()) / 2);
                cameraSvg.invalidate();

            }
            else if (cameraAnimationtickCount < 4 * ANIMATION_STEP_DURATION)
            {
                cameraAnimationtickCount++;
                cameraSvg.setScale(cameraSvg.getScaleX() - 0.1f * sinf(cameraAnimationtickCount * 2 * PI / (2 * ANIMATION_STEP_DURATION)) / (cameraSvg.getScaleX()));
                cameraSvg.resizeToCurrentSVG();
                cameraSvg.setX((CANVAS_WIDTH - cameraSvg.getHeight()) / 2);
                cameraSvg.setY((CANVAS_HEIGTH - cameraSvg.getWidth()) / 2);
                cameraSvg.invalidate();
            }
            else if (cameraSvg.getX() <= CANVAS_WIDTH)
            {
                cameraSvg.setX(cameraSvg.getX() + 15);
            }
            else
            {
                cameraSvg.setVisible(false);
                changeState(cameraAnimationEnd_enum);
            }
        }
        cameraSvg.invalidate();
    }
}

void SvgView::cameraAnimationEnd()
{
    reinitDefaultValues();
    changeState(iconEntranceAnimationInit_enum);
}

void SvgView::waitAnimation()
{
}

void SvgView::changeState(statemachineStep_enum nextState)
{
    animationStep = nextState;
}

bool SvgView::circleMovement(touchgfx::MoveAnimator< touchgfx::SVGImage >& widget, int16_t& counter, float finalAngle, float originAngle)
{
    bool inMovement = false;
    uint32_t ICON_ANIMATION_CIRCLE_X = (CANVAS_WIDTH - widget.getWidth()) / 2;
    uint32_t ICON_ANIMATION_CIRCLE_Y = (CANVAS_HEIGTH - widget.getHeight()) / 2;

    counter = counter + 2; //easeOut((float)counter, 0, 1, (float)counter);
    float angle = counter * PI / 100 - originAngle;
    if (angle <= finalAngle)
    {
        inMovement = true;
        widget.invalidate();
        widget.setX(ICON_ANIMATION_CIRCLE_X + (int16_t)(cos(angle) * ICON_ANIMATION_RADIUS));
        widget.setY(ICON_ANIMATION_CIRCLE_Y - (int16_t)(sin(angle) * ICON_ANIMATION_RADIUS));
        widget.invalidate();
    }
    else
    {
        inMovement = false;
    }

    return inMovement;
}


void SvgView::reinitDefaultValues()
{
    dogSvg.setScale(1);
    paletteSvg.setScale(1);
    cameraSvg.setScale(1);
    dogSvg.setXY(-128, 370);
    paletteSvg.setXY(-229, 370);
    cameraSvg.setXY(-331, 370);
    dogSvg.setVisible(true);
    paletteSvg.setVisible(true);
    cameraSvg.setVisible(true);
    dogSvg.invalidate();
    paletteSvg.invalidate();
    cameraSvg.invalidate();
    inMovement = false;

    paletteSvg.setRotation(0);
    cameraSvg.setRotation(0);

    dogAnimationtickCount = 0;
    paletteAnimationtickCount = 0;
    cameraAnimationtickCount = 0;

    scaleUpDown = 1;

}

/****************   Handle Tick    *************************/
void SvgView::handleTickEvent()
{
    // Call the state machine every tick
    tickCount++;
    stateMachine();
}

void SvgView::stateMachine()
{
    //touchgfx_printf("stateMachine %i  \n",animationStep);

    switch (animationStep)
    {
    case introScaleOutAnimationInit_enum:
        introScaleOutAnimationInit();
        break;

    case introScaleOutAnimation_enum:
        introScaleOutAnimation();
        break;

    case introScaleOutAnimationEnd_enum:
        introScaleOutAnimationEnd();
        break;

    case iconEntranceAnimationInit_enum:
        iconEntranceAnimationInit();
        break;

    case iconEntranceAnimation_enum:
        iconEntranceAnimation();
        break;

    case iconEntranceAnimationEnd_enum:
        iconEntranceAnimationEnd();
        break;

    case iconCircleMoveAnimationInit_enum:
        iconCircleMoveAnimationInit();
        break;

    case iconCircleMoveAnimation_enum:
        iconCircleMoveAnimation();
        break;

    case iconCircleMoveAnimationEnd_enum:
        iconCircleMoveAnimationEnd();
        break;

    case dogAnimationInit_enum:
        dogAnimationInit();
        break;

    case dogAnimation_enum:
        dogAnimation();
        break;

    case dogAnimationEnd_enum:
        dogAnimationEnd();
        break;


    case paletteAnimationInit_enum:
        paletteAnimationInit();
        break;

    case paletteAnimation_enum:
        paletteAnimation();
        break;

    case paletteAnimationEnd_enum:
        paletteAnimationEnd();
        break;


    case cameraAnimationInit_enum:
        cameraAnimationInit();
        break;

    case cameraAnimation_enum:
        cameraAnimation();
        break;

    case cameraAnimationEnd_enum:
        cameraAnimationEnd();
        break;

    case waitAnimation_enum:
        waitAnimation();
        break;

    default:
        break;

    }
}
