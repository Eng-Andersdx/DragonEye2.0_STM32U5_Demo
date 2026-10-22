#ifndef SETTINGSRGBSCREENVIEW_HPP
#define SETTINGSRGBSCREENVIEW_HPP

#include <gui_generated/settingsrgbscreen_screen/SettingsRGBScreenViewBase.hpp>
#include <gui/settingsrgbscreen_screen/SettingsRGBScreenPresenter.hpp>

class SettingsRGBScreenView : public SettingsRGBScreenViewBase
{
public:
    SettingsRGBScreenView();
    virtual ~SettingsRGBScreenView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void goBack();   // navigation (Designer interaction)

    virtual void onDynamicClicked();
    virtual void onFixedClicked();
    virtual void onColourSliderChanged(int value);
    virtual void onBrightnessSliderChanged(int value);
    virtual void onRGBOffClicked();

protected:
    enum RGBMode
    {
        MODE_OFF,
        MODE_DYNAMIC,
        MODE_FIXED
    };

    RGBMode currentMode;

    static const int NUM_COLOURS = 9;

    struct ColourEntry
    {
        const char* name;
        uint8_t r;
        uint8_t g;
        uint8_t b;
    };

    static const ColourEntry colours[NUM_COLOURS];

    void applyCurrentColour();
};

#endif // SETTINGSRGBSCREENVIEW_HPP
