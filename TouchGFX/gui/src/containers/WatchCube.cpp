#include <gui/containers/WatchCube.hpp>

#include "BitmapDatabase.hpp"

#include <gui/common/Constants.hpp>
#include <gui/common/FloatEasingEquations.hpp>


WatchCube::WatchCube() : hour(12), minute(15), second(30), cubeIsRotating(false), rotationDirectionReverse(false), dragEventDetected(false), topCubeSide(0)
{

}

WatchCube::~WatchCube()
{
    Application::getInstance()->unregisterTimerWidget(this);
    Bitmap::dynamicBitmapDelete(DYNAMIC_BITMAP_CLOCK_01);
}

void WatchCube::initialize()
{
    WatchCubeBase::initialize();

    DYNAMIC_BITMAP_CLOCK_01 = Bitmap::dynamicBitmapCreate(200, 200, Bitmap::RGB888);

    /*---- Setting up the Custome Widget Text Arc -------------------------------------------------------------------------------------*/

    lineTextHour.setStartPosition(55, 177);
    lineTextHour.setAngle(-Constants::PI_QUARTER);
    textArcHour.setPosition(textAreaHour.getX(), textAreaHour.getY(), textAreaHour.getWidth(), textAreaHour.getHeight());
    textArcHour.setPositionOffset(0);
    textArcHour.setTypedText(TypedText(T_WATCHDIGITALCLOCKHOUR));
    textArcHour.setColor(textAreaHour.getColor());
    textArcHour.setRenderingAlgorithm(TextArc::BILINEAR_INTERPOLATION);
    textArcHour.setTextCurveGenerator(lineTextHour);
    textArcHour.setWildcard1(textAreaHourBuffer);

    clock.add(textArcHour);

    lineTextSeconds.setStartPosition(129, 103);
    lineTextSeconds.setAngle(-Constants::PI_QUARTER);
    textArcSeconds.setPosition(textAreaSeconds.getX(), textAreaSeconds.getY(), textAreaSeconds.getWidth(), textAreaSeconds.getHeight());
    textArcSeconds.setPositionOffset(0);
    textArcSeconds.setTypedText(TypedText(T_WATCHDIGITALCLOCKSECOND));
    textArcSeconds.setColor(textAreaSeconds.getColor());
    textArcSeconds.setRenderingAlgorithm(TextArc::BILINEAR_INTERPOLATION);
    textArcSeconds.setTextCurveGenerator(lineTextSeconds);
    textArcSeconds.setWildcard1(textAreaSecondsBuffer);

    clock.add(textArcSeconds);

    lineTextHourReverse.setStartPosition(176, 144);
    lineTextHourReverse.setAngle(-Constants::PI_THREE_QUARTER);
    textArcHourReverse.setPosition(textAreaHour.getX(), textAreaHour.getY(), textAreaHour.getWidth(), textAreaHour.getHeight());
    textArcHourReverse.setPositionOffset(0);
    textArcHourReverse.setTypedText(TypedText(T_WATCHDIGITALCLOCKHOUR));
    textArcHourReverse.setColor(textAreaHour.getColor());
    textArcHourReverse.setRenderingAlgorithm(TextArc::BILINEAR_INTERPOLATION);
    textArcHourReverse.setTextCurveGenerator(lineTextHourReverse);
    textArcHourReverse.setWildcard1(textAreaHourBuffer);

    clockReverse.add(textArcHourReverse);

    lineTextSecondsReverse.setStartPosition(103, 71);
    lineTextSecondsReverse.setAngle(-Constants::PI_THREE_QUARTER);
    textArcSecondsReverse.setPosition(textAreaSeconds.getX(), textAreaSeconds.getY(), textAreaSeconds.getWidth(), textAreaSeconds.getHeight());
    textArcSecondsReverse.setPositionOffset(0);
    textArcSecondsReverse.setTypedText(TypedText(T_WATCHDIGITALCLOCKSECOND));
    textArcSecondsReverse.setColor(textAreaSeconds.getColor());
    textArcSecondsReverse.setRenderingAlgorithm(TextArc::BILINEAR_INTERPOLATION);
    textArcSecondsReverse.setTextCurveGenerator(lineTextSecondsReverse);
    textArcSecondsReverse.setWildcard1(textAreaSecondsBuffer);

    clockReverse.add(textArcSecondsReverse);

    Unicode::snprintf(textAreaHourBuffer, TEXTAREAHOUR_SIZE, "%d", minute);
    Unicode::snprintf(textAreaSecondsBuffer, TEXTAREASECONDS_SIZE, "%d", second);

    /*------------------------------------------------------------------------------------------------------------------------------------------------------*/

    digitalClockContainer.setCacheBitmap(DYNAMIC_BITMAP_CLOCK_01);
    digitalClockContainer.enableCachedMode(true);
    digitalClockContainer.updateCache();

    /* Setting up 4 texture mappers, which are the sides of the watch cube */

    /* We are only using 4 sides for the cube since there are only 4 "active" sides, depending on which way the cube is rotateting  */
    for (int i = 0; i < NUMBER_OF_TM; i++)
    {
        watchCubeTextureMapper[i].setXY(0, 0);

        if (i % 2 == 0)
        {
            watchCubeTextureMapper[i].setBitmap(touchgfx::Bitmap(DYNAMIC_BITMAP_CLOCK_01));
        }
        else
        {
            watchCubeTextureMapper[i].setBitmap(touchgfx::Bitmap(BITMAP_FULL_CALENDAR_TEXTURE_ID));
        }

        watchCubeTextureMapper[i].setWidth(this->getHeight());
        watchCubeTextureMapper[i].setHeight(this->getWidth());
        watchCubeTextureMapper[i].setBitmapPosition((this->getWidth() / 2) - (watchCubeTextureMapper[i].getBitmap().getWidth() / 2), (this->getHeight() / 2) - (watchCubeTextureMapper[i].getBitmap().getHeight() / 2));
        watchCubeTextureMapper[i].setScale(0.000f);
        watchCubeTextureMapper[i].setCameraDistance(1000.000f);
        watchCubeTextureMapper[i].setOrigo(240.000f, 240.000f, 1100.000f);
        watchCubeTextureMapper[i].setCamera(240, 240);
        watchCubeTextureMapper[i].setRenderingAlgorithm(touchgfx::TextureMapper::BILINEAR_INTERPOLATION);
        watchCubeTextureMapper[i].updateAngles(0.000f, Constants::PI_HALF * i, Constants::PI_QUARTER);

        insert(&digitalClockContainer, watchCubeTextureMapper[i]);
    }

    reorderCube();
}

void WatchCube::handleClickEvent(const ClickEvent& evt)
{
    // Click event resets the states when pressed is detected
    if (evt.getType() == ClickEvent::PRESSED)
    {
        cubeIsRotating = false;
        horisontalGestureDetected = false;
        dragEventDetected = false;
    }
    else if (cubeIsRotating == false)
    {
        if (dragEventDetected || cubeRotationStepsCounter < cubeRotationSteps)
        {
            /*When the cube is released out off it's main position, the side closes to the main postion is detected*/
            float lowAngle = 10.f;
            for (int i = 0; i < 4; i++)
            {
                float tmpAngle = fmodf(abs(watchCubeTextureMapper[i].getYAngle()), 2 * Constants::PI);

                if (fmodf(abs(watchCubeTextureMapper[i].getYAngle()), 2 * Constants::PI) > Constants::PI)
                {
                    if (tmpAngle > Constants::PI)
                    {
                        tmpAngle = 2 * Constants::PI - tmpAngle;
                    }
                }
                if (tmpAngle < lowAngle)
                {
                    lowAngle = tmpAngle;
                    topCubeSide = i;
                }
            }

            /*The distance to the main postion i calculated and the animation to it is initiated*/
            float offset = fmodf(watchCubeTextureMapper[topCubeSide].getYAngle(), Constants::PI_HALF);

            cubeIsRotating = true;
            cubeRotationSteps = MAX(abs(floorf(offset * 80)), 2);
            cubeRotationStepsCounter = 0;
            prevoiusYAngle = 0;
            cubeRotationStartValue = 0;
            cubeRotationFinalValue = lowAngle * (fmodf(abs(watchCubeTextureMapper[topCubeSide].getYAngle()), 2 * Constants::PI) > Constants::PI ? 1 : -1);
        }
    }
}

void WatchCube::handleDragEvent(const DragEvent& evt)
{
    /*The direction of the drag is detected and the cube is setup to move from left to right or right to left*/
    if (!dragEventDetected && (cubeRotationStepsCounter >= cubeRotationSteps))
    {
        if ((evt.getDeltaX() >= 0 && evt.getDeltaY() >= 0) || (evt.getDeltaX() < 0 && evt.getDeltaY() < 0))
        {
            if (rotationDirectionReverse)
            {
                rotationDirectionReverse = false;
                moveCubeLeftToRight();
            }
        }
        else
        {
            if (rotationDirectionReverse == false)
            {
                rotationDirectionReverse = true;
                moveCubeRightToLeft();
            }
        }
    }

    dragEventDetected = true;

    float orientation = (float)(evt.getDeltaY() > 0 ? -1 : 1);
    float angleChange = sqrtf((float)(evt.getDeltaY() * evt.getDeltaY() + evt.getDeltaX() * evt.getDeltaX())) * 0.01f;

    for (int i = 0; i < 4; i++)
    {
        watchCubeTextureMapper[i].updateAngles(watchCubeTextureMapper[i].getXAngle(), watchCubeTextureMapper[i].getYAngle() + (angleChange * orientation), watchCubeTextureMapper[i].getZAngle());
    }

    reorderCube();
}

void WatchCube::handleGestureEvent(const GestureEvent& evt)
{
    float orientation = 0;

    if (evt.getType() == GestureEvent::SWIPE_HORIZONTAL)
    {
        horisontalGestureDetected = true;

        if (evt.getVelocity() > 0)
        {
            direction = RIGHT;
        }
        else
        {
            direction = LEFT;
        }
        orientation = -1;
    }
    else
    {
        if (horisontalGestureDetected)
        {
            if (evt.getVelocity() > 0)
            {
                direction = (direction == RIGHT) ? DOWN_RIGHT : DOWN_LEFT;
                orientation = -1;
            }
            else
            {
                direction = (direction == RIGHT) ? UP_RIGHT : UP_LEFT;
                orientation = 1;
            }
        }
        else
        {
            if (evt.getVelocity() > 0)
            {
                direction = DOWN;
                orientation = -1;
            }
            else
            {
                direction = UP;
                orientation = 1;
            }
        }
    }

    /*Based on the direction of the swipe detected, the cube is setup to move from left to right or right to left*/
    if (!dragEventDetected)
    {
        switch (direction)
        {
        case WatchCube::RIGHT:
        case WatchCube::DOWN_RIGHT:
        case WatchCube::DOWN:
        case WatchCube::UP_LEFT:
            if (rotationDirectionReverse)
            {
                rotationDirectionReverse = false;
                moveCubeLeftToRight();
            }
            break;
        case WatchCube::UP:
        case WatchCube::UP_RIGHT:
        case WatchCube::DOWN_LEFT:
        case WatchCube::LEFT:
            if (!rotationDirectionReverse)
            {
                rotationDirectionReverse = true;
                moveCubeRightToLeft();
            }
            break;
        }
    }
    cubeIsRotating = true;


    /* The distance of the cube rotation and speed is calculated */
    float velocityAbsolute = (float)abs((double)evt.getVelocity());

    if (horisontalGestureDetected && evt.getType() != GestureEvent::SWIPE_HORIZONTAL)
    {
        cubeRotationDistance = sqrtf(cubeRotationDistance * cubeRotationDistance + (velocityAbsolute * 0.1f) * (velocityAbsolute * 0.1f)) /*orientation*/;
        cubeRotationSteps = sqrtf(cubeRotationSteps * cubeRotationSteps + floorf(velocityAbsolute * 2) * floorf(velocityAbsolute * 2));
    }
    else
    {
        cubeRotationDistance = (velocityAbsolute * 0.1f) /*orientation*/;
        cubeRotationSteps = floorf(velocityAbsolute * 2);
    }

    cubeRotationStepsCounter = 0;
    cubeRotationDistance = cubeRotationDistance * orientation;

    float offset = fmodf(watchCubeTextureMapper[0].getYAngle() + cubeRotationDistance, Constants::PI_HALF);

    if ((watchCubeTextureMapper[0].getYAngle() + cubeRotationDistance) > 0)
    {
        if (offset > Constants::PI_QUARTER)
        {
            cubeRotationDistance = cubeRotationDistance + (Constants::PI_HALF - offset);
        }
        else
        {
            cubeRotationDistance = cubeRotationDistance - offset;
        }
    }
    else
    {
        if (offset < -Constants::PI_QUARTER)
        {
            cubeRotationDistance = cubeRotationDistance - (Constants::PI_HALF + offset);
        }
        else
        {
            cubeRotationDistance = cubeRotationDistance - offset;
        }
    }

    prevoiusYAngle = 0;
    cubeRotationStartValue = 0;
    cubeRotationFinalValue = cubeRotationDistance;
}

void WatchCube::handleTickEvent()
{
    /* If cube animation is ongoing, the angle of the cube is update*/
    if (cubeIsRotating)
    {
        float newYAngle = FloatEasingEquations::backEaseOut(cubeRotationStepsCounter, cubeRotationStartValue, cubeRotationFinalValue, cubeRotationSteps);

        switch (direction)
        {
        case WatchCube::UP:
        case WatchCube::UP_RIGHT:
            for (int i = 0; i < 4; i++)
            {
                watchCubeTextureMapper[i].updateAngles(watchCubeTextureMapper[i].getXAngle(), watchCubeTextureMapper[i].getYAngle() + (newYAngle - prevoiusYAngle), watchCubeTextureMapper[i].getZAngle());
            }
            break;
        case WatchCube::RIGHT:
        case WatchCube::DOWN_RIGHT:
            for (int i = 0; i < 4; i++)
            {
                watchCubeTextureMapper[i].updateAngles(watchCubeTextureMapper[i].getXAngle(), watchCubeTextureMapper[i].getYAngle() + (newYAngle - prevoiusYAngle), watchCubeTextureMapper[i].getZAngle());
            }
            break;
        case WatchCube::DOWN:
        case WatchCube::DOWN_LEFT:
            for (int i = 0; i < 4; i++)
            {
                watchCubeTextureMapper[i].updateAngles(watchCubeTextureMapper[i].getXAngle(), watchCubeTextureMapper[i].getYAngle() + (newYAngle - prevoiusYAngle), watchCubeTextureMapper[i].getZAngle());
            }
            break;
        case WatchCube::LEFT:
        case WatchCube::UP_LEFT:
            for (int i = 0; i < 4; i++)
            {
                watchCubeTextureMapper[i].updateAngles(watchCubeTextureMapper[i].getXAngle(), watchCubeTextureMapper[i].getYAngle() + (newYAngle - prevoiusYAngle), watchCubeTextureMapper[i].getZAngle());
            }
            break;
        default:
            break;
        }

        prevoiusYAngle = newYAngle;

        cubeRotationStepsCounter++;

        if (cubeRotationStepsCounter > cubeRotationSteps)
        {
            cubeIsRotating = false;
            float lowAngle = 10.f;
            for (int i = 0; i < 4; i++)
            {
                if (fmodf(abs(watchCubeTextureMapper[i].getYAngle()) + 0.5f, 2 * PI) < lowAngle)
                {
                    lowAngle = fmodf(abs(watchCubeTextureMapper[i].getYAngle()) + 0.5f, 2 * PI);
                    topCubeSide = i;
                }
            }
        }
        reorderCube();
    }

    /*Handling the animation done in the intro*/
    if (animateCubeIntro)
    {
        float scale = FloatEasingEquations::bounceEaseOut((float)introAnimationsCounter, 0.f, 1.f, 50.f);
        int alpha = EasingEquations::linearEaseNone(introAnimationsCounter, 0, 255, 45);

        for (int i = 0; i < NUMBER_OF_TM; i++)
        {
            watchCubeTextureMapper[i].setScale(scale);
            watchCubeTextureMapper[i].invalidate();
        }

        cubeShadow.setAlpha(alpha);
        cubeShadow.invalidate();

        reorderCube();

        introAnimationsCounter++;

        if (introAnimationsCounter > 50)
        {
            animateCubeIntro = false;
            setTouchable(true);
        }
    }
}

void WatchCube::moveCubeLeftToRight()
{
    /*Based on the current sides of the cube and the new dicrection, the cube is update accordingly */
    uint16_t frontImage = watchCubeTextureMapper[topCubeSide].getBitmap().getId();
    uint16_t sideImage = watchCubeTextureMapper[(topCubeSide % 2) + 1].getBitmap().getId();

    for (int i = 0; i < NUMBER_OF_TM; i++)
    {
        if (topCubeSide % 2 != i % 2)
        {
            switch (sideImage)
            {
            case BITMAP_FULL_CALENDAR_TEXTURE_REVERSE_ID:
                if (frontImage == BITMAP_FULL_WEATHER_TEXTURE_REVERSE_ID)
                {
                    clockReverse.setVisible(false);
                    clock.setVisible(true);
                    digitalClockContainer.invalidate();
                    digitalClockContainer.updateCache();

                    watchCubeTextureMapper[i].setBitmap(Bitmap(DYNAMIC_BITMAP_CLOCK_01));
                    watchCubeTextureMapper[i].setWidth(this->getHeight());
                    watchCubeTextureMapper[i].setHeight(this->getWidth());
                }
                else
                {
                    watchCubeTextureMapper[i].setBitmap(Bitmap(BITMAP_FULL_WEATHER_TEXTURE_ID));
                    watchCubeTextureMapper[i].setWidth(this->getHeight());
                    watchCubeTextureMapper[i].setHeight(this->getWidth());
                }
                break;

            case BITMAP_FULL_WEATHER_TEXTURE_REVERSE_ID:
                if (frontImage == BITMAP_FULL_CALENDAR_TEXTURE_REVERSE_ID)
                {
                    clockReverse.setVisible(false);
                    clock.setVisible(true);
                    digitalClockContainer.invalidate();
                    digitalClockContainer.updateCache();

                    watchCubeTextureMapper[i].setBitmap(Bitmap(DYNAMIC_BITMAP_CLOCK_01));
                    watchCubeTextureMapper[i].setWidth(this->getHeight());
                    watchCubeTextureMapper[i].setHeight(this->getWidth());
                }
                else
                {
                    watchCubeTextureMapper[i].setBitmap(Bitmap(BITMAP_FULL_CALENDAR_TEXTURE_ID));
                    watchCubeTextureMapper[i].setWidth(this->getHeight());
                    watchCubeTextureMapper[i].setHeight(this->getWidth());
                }
                break;

            default:
                if (frontImage == BITMAP_FULL_CALENDAR_TEXTURE_REVERSE_ID)
                {
                    watchCubeTextureMapper[i].setBitmap(Bitmap(BITMAP_FULL_WEATHER_TEXTURE_ID));
                    watchCubeTextureMapper[i].setWidth(this->getHeight());
                    watchCubeTextureMapper[i].setHeight(this->getWidth());
                }
                else
                {
                    watchCubeTextureMapper[i].setBitmap(Bitmap(BITMAP_FULL_CALENDAR_TEXTURE_ID));
                    watchCubeTextureMapper[i].setWidth(this->getHeight());
                    watchCubeTextureMapper[i].setHeight(this->getWidth());
                }
                break;
            }
        }

        else if (topCubeSide % 2 == i % 2)
        {
            switch (frontImage)
            {
            case BITMAP_FULL_CALENDAR_TEXTURE_REVERSE_ID:
                watchCubeTextureMapper[i].setBitmap(Bitmap(BITMAP_FULL_CALENDAR_TEXTURE_ID));
                watchCubeTextureMapper[i].setWidth(this->getHeight());
                watchCubeTextureMapper[i].setHeight(this->getWidth());
                break;

            case BITMAP_FULL_WEATHER_TEXTURE_REVERSE_ID:
                watchCubeTextureMapper[i].setBitmap(Bitmap(BITMAP_FULL_WEATHER_TEXTURE_ID));
                watchCubeTextureMapper[i].setWidth(this->getHeight());
                watchCubeTextureMapper[i].setHeight(this->getWidth());
                break;

            default:
                clockReverse.setVisible(false);
                clock.setVisible(true);
                digitalClockContainer.invalidate();
                digitalClockContainer.updateCache();

                watchCubeTextureMapper[i].setBitmap(Bitmap(DYNAMIC_BITMAP_CLOCK_01));
                watchCubeTextureMapper[i].setWidth(this->getHeight());
                watchCubeTextureMapper[i].setHeight(this->getWidth());
                break;
            }
        }
        watchCubeTextureMapper[i].updateAngles(0.000f, watchCubeTextureMapper[i].getYAngle(), Constants::PI_QUARTER);
    }
}

void WatchCube::moveCubeRightToLeft()
{
    uint16_t frontImage = watchCubeTextureMapper[topCubeSide].getBitmap().getId();
    uint16_t sideImage = watchCubeTextureMapper[(topCubeSide % 2) + 1].getBitmap().getId();

    for (int i = 0; i < NUMBER_OF_TM; i++)
    {
        if (topCubeSide % 2 != i % 2)
        {
            switch (sideImage)
            {
            case BITMAP_FULL_CALENDAR_TEXTURE_ID:
                if (frontImage == BITMAP_FULL_WEATHER_TEXTURE_ID)
                {
                    clockReverse.setVisible(true);
                    clock.setVisible(false);
                    digitalClockContainer.invalidate();
                    digitalClockContainer.updateCache();

                    watchCubeTextureMapper[i].setBitmap(Bitmap(DYNAMIC_BITMAP_CLOCK_01));
                    watchCubeTextureMapper[i].setWidth(this->getHeight());
                    watchCubeTextureMapper[i].setHeight(this->getWidth());
                }
                else
                {
                    watchCubeTextureMapper[i].setBitmap(Bitmap(BITMAP_FULL_WEATHER_TEXTURE_REVERSE_ID));
                    watchCubeTextureMapper[i].setWidth(this->getHeight());
                    watchCubeTextureMapper[i].setHeight(this->getWidth());
                }
                break;

            case BITMAP_FULL_WEATHER_TEXTURE_ID:
                if (frontImage == BITMAP_FULL_CALENDAR_TEXTURE_ID)
                {
                    clockReverse.setVisible(true);
                    clock.setVisible(false);
                    digitalClockContainer.invalidate();
                    digitalClockContainer.updateCache();

                    watchCubeTextureMapper[i].setBitmap(Bitmap(DYNAMIC_BITMAP_CLOCK_01));
                    watchCubeTextureMapper[i].setWidth(this->getHeight());
                    watchCubeTextureMapper[i].setHeight(this->getWidth());
                }
                else
                {
                    watchCubeTextureMapper[i].setBitmap(Bitmap(BITMAP_FULL_CALENDAR_TEXTURE_REVERSE_ID));
                    watchCubeTextureMapper[i].setWidth(this->getHeight());
                    watchCubeTextureMapper[i].setHeight(this->getWidth());
                }
                break;

            default:
                if (frontImage == BITMAP_FULL_CALENDAR_TEXTURE_ID)
                {
                    watchCubeTextureMapper[i].setBitmap(Bitmap(BITMAP_FULL_WEATHER_TEXTURE_REVERSE_ID));
                    watchCubeTextureMapper[i].setWidth(this->getHeight());
                    watchCubeTextureMapper[i].setHeight(this->getWidth());
                }
                else
                {
                    watchCubeTextureMapper[i].setBitmap(Bitmap(BITMAP_FULL_CALENDAR_TEXTURE_REVERSE_ID));
                    watchCubeTextureMapper[i].setWidth(this->getHeight());
                    watchCubeTextureMapper[i].setHeight(this->getWidth());
                }
                break;
            }
        }
        else if (topCubeSide % 2 == i % 2)
        {
            switch (frontImage)
            {
            case BITMAP_FULL_CALENDAR_TEXTURE_ID:
                watchCubeTextureMapper[i].setBitmap(Bitmap(BITMAP_FULL_CALENDAR_TEXTURE_REVERSE_ID));
                watchCubeTextureMapper[i].setWidth(this->getHeight());
                watchCubeTextureMapper[i].setHeight(this->getWidth());
                break;

            case BITMAP_FULL_WEATHER_TEXTURE_ID:
                watchCubeTextureMapper[i].setBitmap(Bitmap(BITMAP_FULL_WEATHER_TEXTURE_REVERSE_ID));
                watchCubeTextureMapper[i].setWidth(this->getHeight());
                watchCubeTextureMapper[i].setHeight(this->getWidth());
                break;

            default:
                clockReverse.setVisible(true);
                clock.setVisible(false);
                digitalClockContainer.invalidate();
                digitalClockContainer.updateCache();

                watchCubeTextureMapper[i].setBitmap(Bitmap(DYNAMIC_BITMAP_CLOCK_01));
                watchCubeTextureMapper[i].setWidth(this->getHeight());
                watchCubeTextureMapper[i].setHeight(this->getWidth());
                break;
            }
        }
        watchCubeTextureMapper[i].updateAngles(0.000f, watchCubeTextureMapper[i].getYAngle(), Constants::PI_THREE_QUARTER);
    }
}

void WatchCube::reorderCube()
{
    /*The Z order for texture mappers in the cube is analyzed and reorder if needed to ensure that the right sides are on top*/
    for (int i = 0; i < NUMBER_OF_TM; i++)
    {
        remove(watchCubeTextureMapper[i]);
    }

    bool inserted[NUMBER_OF_TM];
    for (int i = 0; i < NUMBER_OF_TM; i++)
    {
        inserted[i] = false;
    }

    int index = -1;

    for (int i = 0; i < NUMBER_OF_TM; i++)
    {
        float biggest = -10000.0f;
        for (int j = 0; j < NUMBER_OF_TM; j++)
        {
            float zvalue = MAX(MAX(watchCubeTextureMapper[j].getZ0(), watchCubeTextureMapper[j].getZ1()), MAX(watchCubeTextureMapper[j].getZ2(), watchCubeTextureMapper[j].getZ3()));
            if (!inserted[j] && zvalue > biggest)
            {
                biggest = zvalue;
                index = j;
            }
        }
        add(watchCubeTextureMapper[index]);
        inserted[index] = true;
    }
}

void WatchCube::updateDigitalClock(int hour, int minute, int seconds)
{

    Unicode::snprintf(textAreaHourBuffer, TEXTAREAHOUR_SIZE, "%02d", minute);
    Unicode::snprintf(textAreaSecondsBuffer, TEXTAREASECONDS_SIZE, "%02d", seconds);
    digitalClockContainer.invalidate();
    digitalClockContainer.updateCache();


    watchCubeTextureMapper[topCubeSide].invalidate();
    for (int i = 0; i < NUMBER_OF_TM; i++)
    {
        if (watchCubeTextureMapper[i].getBitmapId() == DYNAMIC_BITMAP_CLOCK_01)
        {
            watchCubeTextureMapper[i].invalidateContent();
        }
    }
}

// Called from the view to start the intro animation of the cube
void WatchCube::startCubeAnimation()
{
    Application::getInstance()->registerTimerWidget(this);

    animateCubeIntro = true;
    introAnimationsCounter = 0;

    cubeRotationSteps = floorf(100);
    cubeRotationStepsCounter = 0;
    prevoiusYAngle = 0;
    cubeRotationStartValue = 0;
    cubeRotationFinalValue = 3 * PI;

    cubeIsRotating = true;
}
