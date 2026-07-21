#ifndef WATCHVIEW_HPP
#define WATCHVIEW_HPP

#include <gui_generated/watch_screen/WatchViewBase.hpp>
#include <gui/watch_screen/WatchPresenter.hpp>

#include <touchgfx/widgets/TextureMapper.hpp>

#include <gui/widgets/AnalogClockExtended.hpp>
#include <gui/widgets/TextArc.hpp>

#include <touchgfx/mixins/FadeAnimator.hpp>

class WatchView : public WatchViewBase
{
public:
    WatchView();
    virtual ~WatchView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    virtual void handleTickEvent();

    /*Action created in the designer*/
    virtual void startIntroAnimation();


protected:
    FadeAnimator<AnalogClockExtended> analogClock;

    int hour;
    int minute;
    int second;
    int clockCounter;
    bool clockRunning;

    int introAnimationsCounter;
    int introAnimationBoxCounter;

    bool animateCircleIntro;
    bool animateAnalogClockIntro;
    bool animateBoxIntro;
};

#endif // WATCHVIEW_HPP
