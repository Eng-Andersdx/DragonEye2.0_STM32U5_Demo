#ifndef FRONTENDAPPLICATION_HPP
#define FRONTENDAPPLICATION_HPP

#include <gui_generated/common/FrontendApplicationBase.hpp>

class FrontendHeap;

using namespace touchgfx;

class FrontendApplication : public FrontendApplicationBase
{
public:
    FrontendApplication(Model& m, FrontendHeap& heap);
    virtual ~FrontendApplication() { }

    virtual void handleTickEvent();

    // ---- Animated navigation (see NavTransitions.hpp) ----
    void gotoHome();                    // back to the menu (from the left)
    void gotoHomeNoTransition();        // used by the intro
    void gotoVolume();
    void gotoOven();
    void gotoCoffee();
    void gotoSettings(bool back = false);
    void gotoExercise(bool back = false);
    void gotoRunning();
    void gotoSettingsHaptics();
    void gotoSettingsLighting();
    void gotoSettingsColour();

private:
    touchgfx::Callback<FrontendApplication> navCallback;

    template <class ViewType, class PresenterType, class TransitionType>
    void navigate();

    template <class ViewType, class PresenterType, class TransitionType>
    void navigateImpl();
};

#endif // FRONTENDAPPLICATION_HPP
