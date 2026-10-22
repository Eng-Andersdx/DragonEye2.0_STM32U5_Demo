#ifndef SETTINGSHAPTICSCREENVIEW_HPP
#define SETTINGSHAPTICSCREENVIEW_HPP

#include <gui_generated/settingshapticscreen_screen/SettingsHapticScreenViewBase.hpp>
#include <gui/settingshapticscreen_screen/SettingsHapticScreenPresenter.hpp>


class SettingsHapticScreenView : public SettingsHapticScreenViewBase
{
public:
    SettingsHapticScreenView();
    virtual ~SettingsHapticScreenView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void goBack();   // navigation (Designer interaction)

    virtual void onHapticSliderChanged(int value);
    virtual void onHapticDefaultClicked();

protected:
    void updateHapticLabel(uint8_t value);
};

#endif // SETTINGSHAPTICSCREENVIEW_HPP
