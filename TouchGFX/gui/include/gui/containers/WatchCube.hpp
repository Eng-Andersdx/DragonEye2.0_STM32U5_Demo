#ifndef WATCHCUBE_HPP
#define WATCHCUBE_HPP

#include <gui_generated/containers/WatchCubeBase.hpp>

#include <touchgfx/widgets/TextureMapper.hpp>

#include <gui/widgets/AnalogClockExtended.hpp>
#include <gui/widgets/TextArc.hpp>

#include <touchgfx/mixins/FadeAnimator.hpp>
#include <texts/TextKeysAndLanguages.hpp>

class WatchCube : public WatchCubeBase
{
public:
    WatchCube();
    virtual ~WatchCube();

    virtual void initialize();

    virtual void handleClickEvent(const ClickEvent& evt);
    virtual void handleDragEvent(const DragEvent& evt);
    virtual void handleGestureEvent(const GestureEvent& evt);

    virtual void handleTickEvent();

    void moveCubeLeftToRight();
    void moveCubeRightToLeft();

    void reorderCube();

    void updateDigitalClock(int hour, int minute, int seconds);

    void startCubeAnimation();

protected:
    enum SwipeDirection
    {
        UP,
        UP_RIGHT,
        RIGHT,
        DOWN_RIGHT,
        DOWN,
        DOWN_LEFT,
        LEFT,
        UP_LEFT
    } direction;

    static const int NUMBER_OF_TM = 4;
    TextureMapper watchCubeTextureMapper[NUMBER_OF_TM];

    int hour;
    int minute;
    int second;

    TextArc textArcHour;
    StraightLineTextGenerator lineTextHour;
    TextArc textArcSeconds;
    StraightLineTextGenerator lineTextSeconds;

    TextArc textArcHourReverse;
    StraightLineTextGenerator lineTextHourReverse;
    TextArc textArcSecondsReverse;
    StraightLineTextGenerator lineTextSecondsReverse;

    BitmapId DYNAMIC_BITMAP_CLOCK_01;

    bool cubeIsRotating;
    bool rotationDirectionReverse;
    bool horisontalGestureDetected;
    bool dragEventDetected;

    float cubeRotationStepsCounter;
    float cubeRotationSteps;
    float cubeRotationStartValue;
    float cubeRotationFinalValue;

    float cubeRotationDistance;

    int topCubeSide;

    float prevoiusYAngle;

    int introAnimationsCounter;
    bool animateCubeIntro;
};

#endif // WATCHCUBE_HPP
