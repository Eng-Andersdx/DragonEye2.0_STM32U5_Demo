#include <gui/intro_screen/IntroView.hpp>

const uint16_t IntroView::HOLD_TICKS = 40; // 60 ticks ≈ 2 seconds @ 60 FPS

IntroView::IntroView()
    : dragonDoneCallback(this, &IntroView::onDragonAnimDone)
{
    // Hook the "done" callbacks
    dragonAnim.setDoneAction(dragonDoneCallback);

}

void IntroView::setupScreen()
{
    IntroViewBase::setupScreen();

    // First animation visible
    dragonAnim.setVisible(true);
    dragonAnim.setAlpha(255);

    // Start the dragon-eye animation (once, from first frame, forward)
    dragonAnim.startAnimation(false, true, true);

        // Start hidden still frame
    lastFrameImage.setAlpha(0);
    lastFrameImage.setVisible(false);
    lastFrameImage.invalidate();

    holdActive = false;
    holdTicks  = 0;

    // When the animated logo finishes, call onAnimDone()
    animDoneCallback = touchgfx::Callback<IntroView, const touchgfx::AnimatedImage&>(
    this, &IntroView::onAnimDone
    );
    dragonAnim.setDoneAction(animDoneCallback);


}

void IntroView::onAnimDone(const touchgfx::AnimatedImage& /*src*/)
{
    // stop + hide animation
    dragonAnim.setVisible(false);
    dragonAnim.stopAnimation();
    dragonAnim.invalidate();

    // show last frame image
    lastFrameImage.setVisible(true);
    lastFrameImage.setAlpha(255);
    lastFrameImage.invalidate();

    // start 2-second hold
    holdActive = true;
    holdTicks  = 0;
}

void IntroView::handleTickEvent()
{
    IntroViewBase::handleTickEvent();

    if (holdActive)
    {
        if (++holdTicks >= HOLD_TICKS)
        {
            holdActive = false;

            // After 2s on last frame → go to your next screen
            application().gotoMenuScreenScreenNoTransition();
            // ^ replace with whatever transition you currently use
        }
    }
}

void IntroView::tearDownScreen()
{
    IntroViewBase::tearDownScreen();
}

void IntroView::onDragonAnimDone(const touchgfx::AnimatedImage& src)
{
    // Safety check (in case you add more animated images later)
    if (&src != &dragonAnim)
        return;

    // Hide dragon anim
    dragonAnim.setVisible(false);
    dragonAnim.setAlpha(0);
    dragonAnim.invalidate();

    application().gotoMenuScreenScreenNoTransition();
}

