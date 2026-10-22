#include "gui/common/FrontendApplication.hpp"
#include <gui/common/FrontendHeap.hpp>
#include <gui/common/NavTransitions.hpp>
#include <touchgfx/transitions/NoTransition.hpp>

#include <touchgfx/Utils.hpp>
#include <touchgfx/hal/HAL.hpp>

const uint32_t BITMAP_CACHE_SIZE = 128 * 1024;
LOCATION_PRAGMA_NOLOAD("BmpCacheSection")
uint8_t bitmapCache[BITMAP_CACHE_SIZE] LOCATION_ATTRIBUTE_NOLOAD("BmpCacheSection");

FrontendApplication::FrontendApplication(Model& m, FrontendHeap& heap)
    : FrontendApplicationBase(m, heap)
{
    Bitmap::setCache((uint16_t*)bitmapCache, BITMAP_CACHE_SIZE, 128);

    // If a frame takes longer than one display refresh (e.g. full-screen fades on the
    // home screen), deliver the missed ticks so animations keep their real-time length
    // instead of stretching out.
    touchgfx::HAL::getInstance()->setFrameRateCompensation(true);
}

// ---------- Navigation helpers ----------

template <class ViewType, class PresenterType, class TransitionType>
void FrontendApplication::navigateImpl()
{
    touchgfx::makeTransition<ViewType, PresenterType, TransitionType, Model>(
        &currentScreen, &currentPresenter, frontendHeap, &currentTransition, &model);
}

template <class ViewType, class PresenterType, class TransitionType>
void FrontendApplication::navigate()
{
    navCallback = touchgfx::Callback<FrontendApplication>(
        this, &FrontendApplication::navigateImpl<ViewType, PresenterType, TransitionType>);
    pendingScreenTransitionCallback = &navCallback;
}

void FrontendApplication::gotoHome()             { navigate<MenuScreenView, MenuScreenPresenter, NavBackTransition>(); }
void FrontendApplication::gotoHomeNoTransition() { navigate<MenuScreenView, MenuScreenPresenter, touchgfx::NoTransition>(); }
void FrontendApplication::gotoVolume()           { navigate<VolumeView, VolumePresenter, NavForwardTransition>(); }
void FrontendApplication::gotoOven()             { navigate<OvenTempViewView, OvenTempViewPresenter, NavForwardTransition>(); }
void FrontendApplication::gotoCoffee()           { navigate<CoffeeView, CoffeePresenter, NavForwardTransition>(); }
void FrontendApplication::gotoRunning()          { navigate<RunningScreenView, RunningScreenPresenter, NavForwardTransition>(); }
void FrontendApplication::gotoSettingsHaptics()  { navigate<SettingsHapticScreenView, SettingsHapticScreenPresenter, NavForwardTransition>(); }
void FrontendApplication::gotoSettingsLighting() { navigate<SettingsRGBScreenView, SettingsRGBScreenPresenter, NavForwardTransition>(); }
void FrontendApplication::gotoSettingsColour()   { navigate<SettingsColourScreenView, SettingsColourScreenPresenter, NavForwardTransition>(); }

void FrontendApplication::gotoSettings(bool back)
{
    if (back) navigate<SettingScreenView, SettingScreenPresenter, NavBackTransition>();
    else      navigate<SettingScreenView, SettingScreenPresenter, NavForwardTransition>();
}

void FrontendApplication::gotoExercise(bool back)
{
    if (back) navigate<ExerciseScreenView, ExerciseScreenPresenter, NavBackTransition>();
    else      navigate<ExerciseScreenView, ExerciseScreenPresenter, NavForwardTransition>();
}

void FrontendApplication::handleTickEvent()
{
    Application::handleTickEvent();
    // If you later add Model::tick(), call it here.
}
