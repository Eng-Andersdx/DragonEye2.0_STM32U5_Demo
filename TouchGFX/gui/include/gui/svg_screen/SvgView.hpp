#ifndef SVGVIEW_HPP
#define SVGVIEW_HPP

#include <gui_generated/svg_screen/SvgViewBase.hpp>
#include <gui/svg_screen/SvgPresenter.hpp>

class SvgView : public SvgViewBase
{
public:
    SvgView();
    virtual ~SvgView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void handleTickEvent();
protected:

private :

    const int16_t CANVAS_WIDTH = 480;
    const int16_t CANVAS_HEIGTH = 480;
    const uint8_t FADE_ANIMATION_DURATION = 20;
    const uint8_t FADE_ANIMATION_ALPHA = 255;
    const int16_t AFTER_SCALE_CENTER_POSITION_X = 157;
    const int16_t AFTER_SCALE_CENTER_POSITION_Y = 157;
    const int16_t MOVE_ANIMATION_DURATION = 80;
    const int16_t FULL_ROTATION_ANGLE = 360;
    const int16_t ANIMATION_STEP_DURATION = 30;
    const int16_t ICON_ANIMATION_RADIUS = 187;
    const float SCALE_DOWN_SPEED = 0.963f;
    const float HALF_SCALE = 0.5f;
    const float MEDIUM_SCALE = 1.3f;

    //Enums to keep tracking in which state the program is curently running.
    enum statemachineStep_enum
    {
        introScaleOutAnimationInit_enum,
        introScaleOutAnimation_enum,
        introScaleOutAnimationEnd_enum,

        iconEntranceAnimationInit_enum,
        iconEntranceAnimation_enum,
        iconEntranceAnimationEnd_enum,

        iconCircleMoveAnimationInit_enum,
        iconCircleMoveAnimation_enum,
        iconCircleMoveAnimationEnd_enum,

        dogAnimationInit_enum,
        dogAnimation_enum,
        dogAnimationEnd_enum,

        paletteAnimationInit_enum,
        paletteAnimation_enum,
        paletteAnimationEnd_enum,

        cameraAnimationInit_enum,
        cameraAnimation_enum,
        cameraAnimationEnd_enum,

        waitAnimation_enum

    };

    statemachineStep_enum animationStep;

    //Used for counting the total number of ticks
    int16_t tickCount;
    int16_t dogAnimationtickCount;
    int16_t paletteAnimationtickCount;
    int16_t cameraAnimationtickCount;

    bool inMovement;

    int16_t scaleUpDown;

    int16_t startPositionX;
    int16_t startPositionY;

    float scaleCoeff;

    void stateMachine();

    void reinitDefaultValues();
    bool circleMovement(touchgfx::MoveAnimator<touchgfx::SVGImage>& widget, int16_t& counter, float finalAngle, float originAngle);

    void changeState(statemachineStep_enum nextState);

    void introScaleOutAnimationInit();
    void introScaleOutAnimation();
    void introScaleOutAnimationEnd();

    void iconEntranceAnimationInit();
    void iconEntranceAnimation();
    void iconEntranceAnimationEnd();

    void iconCircleMoveAnimationInit();
    void iconCircleMoveAnimation();
    void iconCircleMoveAnimationEnd();

    void dogAnimationInit();
    void dogAnimation();
    void dogAnimationEnd();

    void paletteAnimationInit();
    void paletteAnimation();
    void paletteAnimationEnd();

    void cameraAnimationInit();
    void cameraAnimation();
    void cameraAnimationEnd();

    void waitAnimation();

};

#endif // SVGVIEW_HPP
