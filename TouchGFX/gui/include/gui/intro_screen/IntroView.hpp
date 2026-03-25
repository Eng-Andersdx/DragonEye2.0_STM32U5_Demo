#ifndef INTROVIEW_HPP
#define INTROVIEW_HPP

#include <gui_generated/intro_screen/IntroViewBase.hpp>
#include <gui/intro_screen/IntroPresenter.hpp>
#include <touchgfx/widgets/AnimatedImage.hpp>
#include <touchgfx/Callback.hpp>


class IntroView : public IntroViewBase
{
public:
    IntroView();
    virtual ~IntroView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void handleTickEvent() override;

protected:
    // Callbacks for animation finished
    touchgfx::Callback<IntroView, const touchgfx::AnimatedImage&> dragonDoneCallback;
    touchgfx::Callback<IntroView, const touchgfx::AnimatedImage&> mainMenuDoneCallback;

    // Handlers
    void onDragonAnimDone(const touchgfx::AnimatedImage& src);
    void onMainMenuAnimDone(const touchgfx::AnimatedImage& src);

    // Called when the animation finishes
    void onAnimDone(const touchgfx::AnimatedImage& src);

    touchgfx::Callback<IntroView, const touchgfx::AnimatedImage&> animDoneCallback;

    bool     holdActive;
    uint16_t holdTicks;
    static const uint16_t HOLD_TICKS;
};

#endif // INTROVIEW_HPP
