#ifndef SETTINGSCOLOURSCREENVIEW_HPP
#define SETTINGSCOLOURSCREENVIEW_HPP

#include <gui_generated/settingscolourscreen_screen/SettingsColourScreenViewBase.hpp>
#include <gui/settingscolourscreen_screen/SettingsColourScreenPresenter.hpp>
#include <gui/common/HwInterface.hpp>

extern int LED_mode;

class SettingsColourScreenView : public SettingsColourScreenViewBase
{
public:
    SettingsColourScreenView();
    virtual ~SettingsColourScreenView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void goBack();   // navigation (Designer interaction)

    virtual void onWhiteButtonClicked();
    virtual void onRedButtonClicked();
    virtual void onGreenButtonClicked();
    virtual void onBlueButtonClicked();
    virtual void onCyanButtonClicked();
    virtual void onYellowButtonClicked();
    virtual void onOrangeButtonClicked();
    virtual void onMagentaButtonClicked();

protected:
    void applyColour(uint8_t r, uint8_t g, uint8_t b);
    void markSelected(int idx);   // -1 = none
};

#endif // SETTINGSCOLOURSCREENVIEW_HPP