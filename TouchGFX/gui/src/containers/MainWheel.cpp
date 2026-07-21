#include <gui/containers/MainWheel.hpp>

#include <gui/common/Constants.hpp>
#include <gui/common/FloatEasingEquations.hpp>
#include <gui/common/Enums.hpp>

#include <math.h>
#include <touchgfx/Utils.hpp>

#include <texts/TextKeysAndLanguages.hpp>
#include <BitmapDatabase.hpp>

static float originX;
static float originY;
static const float radius = 166; // Radius for the circle the menu items move in
static const float angle_between_elements = 2 * PI / 8;

MainWheel::MainWheel() : rotating(false), dragEventHappend(false), horizontalSwipeDetected(false), animateCircleBounce(false), animateCircleOut(false)
{

}

void MainWheel::initialize()
{
    MainWheelBase::initialize();


    // Center coordinates of the menu wheel
    originX = static_cast<float>((this->getWidth() - wheelElements[0].icon.getWidth()) / 2);
    originY = static_cast<float>((this->getHeight() - wheelElements[0].icon.getWidth()) / 2);

    setTouchable(false);

    float newX, newY;

    centerIcon = 4; // The index value of the icon that will be in the center/selcted position


    //Setup of the menuw heel placing the icons with a radian of a quarter pi apart
    for (int i = 0; i < NUMBER_OF_WHEEL_ELEMENTS; i++)
    {
        wheelElements[i].angle = angle_between_elements * i;

        newX = originX + cosf(wheelElements[i].angle) * radius;
        newY = originY + sinf(wheelElements[i].angle) * radius;

        wheelElements[i].icon.setXY((int)newX, (int)newY);

        switch (i % NUMBER_OF_SUB_DEMOS)
        {
        case Enums::E_Bike:
            wheelElements[i].textId = T_MAINSELECTEDSUBDEMOEBIKE;
            wheelElements[i].icon.setIcon(BITMAP_E_BIKE_ICON_48_ID);
            break;
        case Enums::Thermostat:
            wheelElements[i].textId = T_MAINSELECTEDSUBDEMOTHERMOSTAT;
            wheelElements[i].icon.setIcon(BITMAP_THERMOSTAT_ICON_48_ID);
            break;
        case Enums::Watch:
            wheelElements[i].textId = T_MAINSELECTEDSUBDEMOWATCH;
            wheelElements[i].icon.setIcon(BITMAP_WATCH_ICON_48_ID);
            break;
        case Enums::Compass:
            wheelElements[i].textId = T_MAINSELECTEDSUBDEMOCOMPASS;
            wheelElements[i].icon.setIcon(BITMAP_COMPASS_ICON_48_ID);
            break;
        case Enums::Svg:
            wheelElements[i].textId = T_MAINSELECTEDSUBDEMOSVG;
            wheelElements[i].icon.setIcon(BITMAP_IMAGE_ICON_48_ID);
            break;
        }

        wheelElements[i].icon.updateZAngle((wheelElements[i].angle + Constants::PI) * 3);
        updateIconSize(&wheelElements[i].icon, newX);
        wheelElements[i].icon.setVisible(false);

        add(wheelElements[i].icon);
    }

    remove(flower);
    remove(selectedDemoName);
    add(selectedDemoName);
    add(flower);
}

void MainWheel::handleClickEvent(const ClickEvent& evt)
{
    //Ignores click while intro is animating
    if (wheelElements[1].icon.isVisible() == false || animateCircleBounce || animateCircleOut)
    {
        return;
    }
    // Calculates the angle of the press (touch) acording to the wheel, used later to dertimine the wheels movement
    if (evt.getType() == ClickEvent::PRESSED)
    {
        dragEventHappend = false;
        rotating = false;

        selectedDemoName.startFadeAnimation(0, 10);

        float a = evt.getY() - static_cast<float>(this->getHeight() / 2);
        float b = evt.getX() - static_cast<float>(this->getWidth() / 2);
        float c = sqrtf(a * a + b * b);

        if (evt.getX() >= originX)
        {
            lastAngle = asinf(a / c);
        }
        else if (evt.getX() < originX)
        {
            lastAngle = (Constants::PI_HALF + (Constants::PI_HALF - asinf(a / c)));
        }
    }
    //IF the wheel is not rotating and a drag have happen the wheel will move to have the icon that is closest to teh "center" as center icon
    else if (rotating == false)
    {
        if (dragEventHappend || rotatingStepCounter < rotatingSteps)
        {
            int closetsIcon = 0;
            float distance = 10.f; //Number large enough to never be the closest

            for (int i = 0; i < NUMBER_OF_WHEEL_ELEMENTS; i++)
            {
                while (wheelElements[i].angle > 2 * PI || wheelElements[i].angle < 0)
                {
                    if (wheelElements[i].angle > 2 * PI)
                    {
                        wheelElements[i].angle = wheelElements[i].angle - 2 * PI;
                    }
                    else
                    {
                        wheelElements[i].angle = wheelElements[i].angle + 2 * PI;
                    }
                }
                if (abs(abs(wheelElements[i].angle) - PI) < abs(abs(distance) - PI))
                {
                    closetsIcon = i;
                    distance = wheelElements[i].angle;
                }
            }

            rotatingStepCounter = 0;
            startValue = wheelElements[0].angle;
            moveDistance = PI - wheelElements[closetsIcon].angle;
            rotatingSteps = MAX(abs(floorf(moveDistance * 40)), 2);
            rotating = true;
        }
        //If a drag is not performed between press and release touch, either move to the selected icon, or start animation to the selected screen
        else
        {
            if (selectedDemoName.getRect().intersect(evt.getX(), evt.getY()))
            {
                for (int j = 0; j < NUMBER_OF_WHEEL_ELEMENTS; j++)
                {
                    if (j != centerIcon)
                    {
                        wheelElements[j].icon.startFadeAnimation(0, 55);
                    }
                }
                flower.startFadeAnimation(0, 55);

                animateCircleBounce = true;
                rotatingSteps = Constants::MAIN_EXIT_BOUNCE_ANIMATION_DURATION;
                rotatingStepCounter = 0;

                return;
            }
            else
            {
                bool wheellIconReleased = false;

                for (int i = 0; i < NUMBER_OF_WHEEL_ELEMENTS; i++)
                {
                    if (wheelElements[i].icon.getRect().intersect(evt.getX(), evt.getY()))
                    {
                        wheellIconReleased = true;
                        if (centerIcon == i)
                        {
                            for (int j = 0; j < NUMBER_OF_WHEEL_ELEMENTS; j++)
                            {
                                if (j != i)
                                {
                                    wheelElements[j].icon.startFadeAnimation(0, 55);
                                }
                            }
                            flower.startFadeAnimation(0, 55);

                            animateCircleBounce = true;
                            rotatingSteps = Constants::MAIN_EXIT_BOUNCE_ANIMATION_DURATION;
                            rotatingStepCounter = 0;

                            return;
                        }
                        else
                        {
                            if (fmodf(wheelElements[i].angle, 2 * PI) > PI)
                            {
                                moveDistance = -1 * (fmodf(wheelElements[i].angle, 2 * PI) - PI);
                            }
                            else
                            {
                                moveDistance = PI - fmodf(wheelElements[i].angle, 2 * PI);
                            }

                            rotatingStepCounter = 0;
                            startValue = wheelElements[0].angle;
                            rotatingSteps = MAX(abs(floorf(moveDistance * 40)), 2);
                            rotating = true;
                            return;
                        }
                    }
                }

                if (wheellIconReleased == false)
                {
                    selectedDemoName.setX(Constants::SUBDEMO_NAME_TEXT_START_X);
                    selectedDemoName.setTypedText(TypedText(wheelElements[centerIcon].textId));
                    selectedDemoName.startMoveAnimation(Constants::SUBDEMO_NAME_TEXT_END_X, selectedDemoName.getY(), Constants::SUBDEMO_NAME_TEXT_MOVE_ANIMATION_DURATION, EasingEquations::bounceEaseOut);
                    selectedDemoName.startFadeAnimation(255, Constants::SUBDEMO_NAME_TEXT_MOVE_ANIMATION_DURATION);
                }
            }
        }
    }
}

//In handle drag the angle detected when we have a press event is used to calculate wheere to movw the wheel to
void MainWheel::handleDragEvent(const DragEvent& evt)
{
    if (!wheelElements[1].icon.isVisible() || animateCircleBounce || animateCircleOut)
    {
        return;
    }

    dragEventHappend = true;

    float changeInAngle;

    float a = evt.getNewY() - (float)HAL::DISPLAY_HEIGHT / 2;
    float b = evt.getNewX() - (float)HAL::DISPLAY_WIDTH / 2;
    float c = sqrtf(a * a + b * b);

    if (evt.getNewX() >= HAL::DISPLAY_WIDTH / 2)
    {
        changeInAngle = asinf(a / c);
    }
    else
    {
        changeInAngle = (Constants::PI_HALF + (Constants::PI_HALF - asinf(a / c)));
    }

    for (int i = 0; i < NUMBER_OF_WHEEL_ELEMENTS; i++)
    {
        wheelElements[i].icon.invalidateWheelContent();
        wheelElements[i].angle = fmodf(wheelElements[i].angle - (lastAngle - changeInAngle), 2 * PI);

        wheelElements[i].icon.setXY((int)(originX + cosf(wheelElements[i].angle) * radius), (int)(originY + sinf(wheelElements[i].angle) * radius));
        wheelElements[i].icon.updateZAngle((wheelElements[i].angle + PI) * 3);

        float newX = originX + cosf(wheelElements[i].angle) * radius;

        //wheelElements[i].icon.setText(i);
        updateIconSize(&wheelElements[i].icon, newX);
        wheelElements[i].icon.invalidateWheelContent();
    }
    oldAngle = lastAngle;
    lastAngle = changeInAngle;
}


void MainWheel::handleGestureEvent(const GestureEvent& evt)
{
    /*Guard to ensure that the wheel can't be interacted with when the intro animation is ongoing*/
    if (!wheelElements[1].icon.isVisible() || animateCircleBounce || animateCircleOut)
    {
        return;
    }

    /*Detects the new angle the wheel will move to*/
    if (oldAngle > Constants::PI + Constants::PI_HALF)
    {
        oldAngle -= Constants::PI * 2;
    }
    else if (oldAngle < -Constants::PI_HALF)
    {
        oldAngle += PI * 2;
    }

    if (lastAngle > Constants::PI + Constants::PI_HALF)
    {
        lastAngle -= Constants::PI * 2;
    }
    else if (lastAngle < -Constants::PI_HALF)
    {
        lastAngle += Constants::PI * 2;
    }
    float direction = lastAngle > oldAngle ? 1.f : -1.f;

    /*Based on the velocity the speed and distance is calculated*/
    if (evt.getType() == GestureEvent::SWIPE_VERTICAL)
    {
        float velocityAbsolute = (float)abs((double)evt.getVelocity());
        float VmoveDistance = (velocityAbsolute * 0.1f) * direction;

        if (horizontalSwipeDetected)
        {
            moveDistance = sqrtf(moveDistance * moveDistance + VmoveDistance * VmoveDistance) * direction;
            rotatingSteps = sqrtf(rotatingSteps * rotatingSteps + floorf(velocityAbsolute * 2) * floorf(velocityAbsolute * 2));
        }
        else
        {
            moveDistance = VmoveDistance;
            rotatingSteps = floorf(velocityAbsolute * 2);
        }

        startValue = wheelElements[0].angle;

        moveDistance = offesetCompensation(moveDistance, startValue);
        rotatingStepCounter = 0;
        rotating = true;
        horizontalSwipeDetected = false;
    }
    else
    {
        horizontalSwipeDetected = true;

        float velocityAbsolute = (float)abs((double)evt.getVelocity());

        rotatingSteps = floorf(velocityAbsolute * 2);
        rotatingStepCounter = 0;
        startValue = wheelElements[0].angle;

        moveDistance = (velocityAbsolute * 0.1f) * direction;
        moveDistance = offesetCompensation(moveDistance, startValue);

        rotating = true;
    }
}

void MainWheel::handleTickEvent()
{
    //The animation that handles the flower, picture is changed every third tick, while scaling is updated every tick
    if (animateFlower)
    {
        if (fmodf(flowerStepCounter, 3) == 0 && flowerStepCounter != 0)
        {
            flower.setBitmap(Bitmap(BITMAP_FLOWER_BLUE0000_ID + ((int)flowerStepCounter / 3)));
        }

        float scale = FloatEasingEquations::cubicEaseOut(flowerStepCounter, 0.f, 1.f, 74);
        flower.setScale(scale);
        flower.invalidate();

        flowerStepCounter++;

        if (flowerStepCounter > 74)
        {
            animateFlower = false;

            rotatingSteps = 140;//180;
            moveDistance = 2 * PI;
            rotatingStepCounter = 0;
            startValue = wheelElements[0].angle;
            rotating = true;
        }
    }
    else if (rotating)
    {
        /*When wheel is set to rotate every tick the next angle is calculated until the desired angel is achived*/
        float newAngle = FloatEasingEquations::cubicEaseOut(rotatingStepCounter, startValue, moveDistance, rotatingSteps);

        for (int i = 0; i < NUMBER_OF_WHEEL_ELEMENTS; i++)
        {
            wheelElements[i].icon.invalidateWheelContent();
            wheelElements[i].angle = newAngle + angle_between_elements * i;

            wheelElements[i].icon.setXY((int)(originX + cosf(wheelElements[i].angle) * radius), (int)(originY + sinf(wheelElements[i].angle) * radius));

            float newX = originX + cosf(wheelElements[i].angle) * radius;

            updateIconSize(&wheelElements[i].icon, newX);

            wheelElements[i].icon.updateZAngle((wheelElements[i].angle + PI) * 3);

            if (!wheelElements[i].icon.isVisible())
            {
                if (fmodf(wheelElements[i].angle, 2 * PI) < 0.4f && fmodf(wheelElements[i].angle, 2 * PI) > 0)
                {
                    wheelElements[i].icon.setVisible(true);
                }
            }
            wheelElements[i].icon.invalidateWheelContent();
        }

        // The rotating step counter, counts up until it has hit the desired steps (rotatingSteps + 1) then it informs handleTick to stop rotating (rotating = false) before it looks for the icon that is placed in the selected icon posistion
        rotatingStepCounter++;
        if (rotatingStepCounter > rotatingSteps)
        {
            rotating = false;
            for (int i = 0; i < NUMBER_OF_WHEEL_ELEMENTS; i++)
            {
                if (abs(fmodf(wheelElements[i].angle, 2 * PI)) > PI - 0.1 && abs(fmodf(wheelElements[i].angle, 2 * PI)) < PI + 0.1)
                {
                    centerIcon = i;
                }
            }
            setTouchable(true);
            selectedDemoName.setX(Constants::SUBDEMO_NAME_TEXT_START_X);
            selectedDemoName.setTypedText(TypedText(wheelElements[centerIcon].textId));
            selectedDemoName.startMoveAnimation(Constants::SUBDEMO_NAME_TEXT_END_X, selectedDemoName.getY(), Constants::SUBDEMO_NAME_TEXT_MOVE_ANIMATION_DURATION, EasingEquations::bounceEaseOut);
            selectedDemoName.startFadeAnimation(255, Constants::SUBDEMO_NAME_TEXT_MOVE_ANIMATION_DURATION);
        }
    }
    else if (animateCircleBounce)
    {
        wheelElements[centerIcon].icon.invalidateWheelContent();
        float scaleSbustration = 0.25f * rotatingStepCounter / rotatingSteps + 1;
        float newScale = FloatEasingEquations::backEaseInOut(rotatingStepCounter, 1, 0.25f, rotatingSteps) - scaleSbustration;
        wheelElements[centerIcon].icon.setIconScale(1 + newScale);
        wheelElements[centerIcon].icon.invalidateWheelContent();

        rotatingStepCounter++;
        if (rotatingStepCounter > rotatingSteps)
        {
            Application::getInstance()->unregisterTimerWidget(this);
            animateCircleBounce = false;
            subDemoSelected(centerIcon % NUMBER_OF_SUB_DEMOS);
        }
    }
}

/*Called from MainView to start screen transition*/
void MainWheel::startFlowerAnimation()
{
    Application::getInstance()->registerTimerWidget(this);
    animateFlower = true;
    flowerStepCounter = 0;
}

/*Based on the X value (distance from selected icon), the scale and alpha is determind*/
void MainWheel::updateIconSize(FadeAnimator<MainWheelIcon>* icon, float x)
{
    if (x <= 76)
    {
        icon->setIconScale(0.8f + (0.004166666f * (48.f - (x - 28.f))));
        icon->setAlpha(255);
    }
    else if (x <= 194)
    {
        icon->setIconScale(0.6f + (0.001694915f * (118.f - (x - 76.f))));
        icon->setAlpha(255);
    }
    else if (x <= 311)
    {
        icon->setIconScale(0.4f + (0.001709402f * (117.f - (x - 194.f))));
        icon->setAlpha(255);
    }
    else
    {
        icon->setIconScale(0.4f);
        icon->setAlpha(255 - ((int)x - 311) * 4);
    }
}

/*Calculates the actual move distance for the wheel to ensure that it's rotation stops in the center, with a selected Icon*/
float MainWheel::offesetCompensation(float moveDistance, float startAngle)
{
    float tmpStartAngle = startAngle;
    if (tmpStartAngle > 0 && moveDistance <= 0)
    {
        tmpStartAngle = tmpStartAngle - (2 * PI);
    }
    else if (tmpStartAngle <= 0 && moveDistance > 0)
    {
        tmpStartAngle = tmpStartAngle + (2 * PI);
    }

    float finalDestination = moveDistance + tmpStartAngle;
    float modFD = fmodf(finalDestination, angle_between_elements);

    float absMoveDistance = abs(moveDistance);
    float absModFD = abs(modFD);

    if (absModFD > angle_between_elements / 2)
    {
        absMoveDistance = absMoveDistance + (angle_between_elements - absModFD);
    }
    else
    {
        absMoveDistance = absMoveDistance - absModFD;
    }

    float finalMoveDistance = moveDistance > 0.f ? absMoveDistance : -absMoveDistance;
    return finalMoveDistance;
}
