#ifndef INTROVIEW_HPP
#define INTROVIEW_HPP

#include <gui_generated/intro_screen/IntroViewBase.hpp>
#include <gui/intro_screen/IntroPresenter.hpp>

class IntroView : public IntroViewBase
{
public:
    IntroView();
    virtual ~IntroView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    virtual void handleTickEvent();

    virtual void screenIsEntered();

    float floatEaseOut(float t, float b, float c, float d);
    float floatBackEaseOut(float t, float b, float c, float d);

protected:
    enum AnimationSequence
    {
        ANIMATE_BACKGROUND,
        ANIMATE_RING,
        ANIMATE_CENTER_CIRCLE,
        ANIMATE_FLOWER_SCALE_UP,
        ANIMATE_FLOWER_ROTATE,
        ANIMATE_FLOWER_SCALE_DOWN
    } introAnimationSequence;

    static const uint8_t ANIMATE_BACKGROUND_DURATION = 75;
    static const uint8_t ANIMATE_RING_DURATION = 50;
    static const uint8_t ANIMATE_CENTER_CIRCLE_DURATION = 50;
    static const uint8_t ANIMATE_FLOWER_SCALE_UP_DURATION = 160;
    static const uint8_t ANIMATE_FLOWER_ROTATE_DURATION = 120;
    static const uint8_t ANIMATE_FLOWER_SCALE_DOWN_DURATION = 60;

    bool animate;

    int stepCounter;
    int steps;
    int stepCounter2;
    int steps2;

    float startValue;
    float startValue2;
    float stopValue;
    float stopValue2;
};

#endif // INTROVIEW_HPP
