#include <gui/settingsrgbscreen_screen/SettingsRGBScreenView.hpp>
#include <touchgfx/Unicode.hpp>
#include <gui/common/HwInterface.hpp>
extern int LED_mode;
extern int colour_picker_selected;
const SettingsRGBScreenView::ColourEntry SettingsRGBScreenView::colours[NUM_COLOURS] =
{
    { "White",   100, 100, 100 },
    { "Red",     100,   0,   0 },
    { "Green",     0, 100,   0 },
    { "Blue",      0,   0, 100 },
    { "Yellow",  100, 100,   0 },
    { "Cyan",      0, 100, 100 },
    { "Magenta", 100,   0, 100 },
    { "Orange",  100,  50,   0 },
    { "Coffee",   45,  27,   9 }
};

SettingsRGBScreenView::SettingsRGBScreenView()
{
}

void SettingsRGBScreenView::setupScreen()
{
    SettingsRGBScreenViewBase::setupScreen();

    currentMode = MODE_DYNAMIC;

    colour_slider.setValue(0);
    brightness_slider.setValue(100);

    // Colour slider hidden until Fixed mode selected
    colour_slider.setVisible(false);
    colour_slider.invalidate();

}

void SettingsRGBScreenView::tearDownScreen()
{
    SettingsRGBScreenViewBase::tearDownScreen();
}

void SettingsRGBScreenView::onDynamicClicked()
{
    currentMode = MODE_DYNAMIC;
    LED_mode = 1;
    colour_picker_selected = -1;
    BSP_LED_SetColor(RGB_RED);
    colour_slider.setVisible(false);
    colour_slider.invalidate();

}

void SettingsRGBScreenView::onFixedClicked()
{
    currentMode = MODE_FIXED;
    LED_mode = 0;

    colour_slider.setVisible(true);
    colour_slider.invalidate();

    applyCurrentColour();
}

void SettingsRGBScreenView::onColourSliderChanged(int value)
{
    if (currentMode != MODE_FIXED)
        return;

    applyCurrentColour();
}

void SettingsRGBScreenView::onBrightnessSliderChanged(int value)
{
    int brightness = brightness_slider.getValue();
    if (brightness < 20) brightness = 20;
    BSP_BL_SetIntensity(brightness);
}

void SettingsRGBScreenView::onRGBOffClicked()
{
    if (btn_rgb_off.getState())
    {
        // Toggled ON = LED off
        currentMode = MODE_OFF;
        LED_mode = 0; // stop other screens turning the LED back on
        colour_picker_selected = -1;
        BSP_LED_SetRGBIntensity(0, 0, 0);
    }
    else
    {
        // Toggled OFF = restore dynamic
        currentMode = MODE_DYNAMIC;
        LED_mode = 1;
        BSP_LED_SetColor(RGB_RED);
    }
}

void SettingsRGBScreenView::applyCurrentColour()
{
    colour_picker_selected = -1;   // colour now comes from the slider, not the picker

    int idx = colour_slider.getValue();
    if (idx < 0 || idx >= NUM_COLOURS)
        return;

    uint8_t r = colours[idx].r;
    uint8_t g = colours[idx].g;
    uint8_t b = colours[idx].b;

    BSP_LED_SetRGBIntensity(r, g, b);
}

// ---------- Navigation (animated, see FrontendApplication) ----------

void SettingsRGBScreenView::goBack()
{
    application().gotoSettings(true);
}
