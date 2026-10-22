#ifndef SETTINGSCREENVIEW_HPP
#define SETTINGSCREENVIEW_HPP

#include <gui_generated/settingscreen_screen/SettingScreenViewBase.hpp>
#include <gui/settingscreen_screen/SettingScreenPresenter.hpp>

class SettingScreenView : public SettingScreenViewBase
{
public:
    SettingScreenView();
    virtual ~SettingScreenView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void goBack();   // navigation (Designer interaction)
    virtual void openHaptics();   // navigation (Designer interaction)
    virtual void openLighting();   // navigation (Designer interaction)
    virtual void openColour();   // navigation (Designer interaction)
    virtual void onResetClicked();
};

#endif // SETTINGSCREENVIEW_HPP
