#ifndef MAINWHEEL_HPP
#define MAINWHEEL_HPP

#include <gui_generated/containers/MainWheelBase.hpp>


#include <touchgfx/mixins/FadeAnimator.hpp>
#include <gui/containers/MainWheelIcon.hpp>

class MainWheel : public MainWheelBase
{
public:
    MainWheel();
    virtual ~MainWheel() {}

    virtual void initialize();

    // Functions to overide TouchGFX interaction functions
    void handleClickEvent(const ClickEvent& evt) override;
    void handleDragEvent(const DragEvent& evt) override;
    void handleGestureEvent(const GestureEvent& evt) override;

    // handleTickEvent enables the UI to act everytime there is a tick, wich comes from the Vsync
    void handleTickEvent() override;

    void startFlowerAnimation();

    void updateIconSize(FadeAnimator<MainWheelIcon>* icon, float x);

    float offesetCompensation(float moveDistance, float startAngle);

protected:

    static const int NUMBER_OF_WHEEL_ELEMENTS = 8;
    static const int NUMBER_OF_SUB_DEMOS = 5;

    struct menuElement
    {
        FadeAnimator<MainWheelIcon> icon;
        float angle;
        float size;
        uint32_t textId;
    };

    float lastAngle;

    menuElement wheelElements[NUMBER_OF_WHEEL_ELEMENTS];

    float rotatingSteps;
    float rotatingStepCounter;
    float moveDistance;
    float startValue;

    int centerIcon;

    bool rotating;
    bool dragEventHappend;

    bool horizontalSwipeDetected;

    float oldAngle;
    bool animateCircleBounce;
    bool animateCircleOut;

    int circleStartX;

    bool animateFlower;
    uint16_t flowerStepCounter;
};

#endif // MAINWHEEL_HPP
