#include <gui/settingscreen_screen/SettingScreenView.hpp>
#include <gui/common/HwInterface.hpp>
extern int LED_mode;
extern int colour_picker_selected;

extern "C" volatile uint8_t haptic_feedback_intensity;

SettingScreenView::SettingScreenView()
{
}

void SettingScreenView::setupScreen()
{
    SettingScreenViewBase::setupScreen();
    if(LED_mode==1){
    	BSP_LED_SetColor(RGB_RED);
    }
}

void SettingScreenView::tearDownScreen()
{
    SettingScreenViewBase::tearDownScreen();
}

void SettingScreenView::onResetClicked()
{
    haptic_feedback_intensity = 35;
    LED_mode = 1; // back to dynamic LEDs
    colour_picker_selected = -1;
    BSP_LED_SetColor(RGB_RED);
}

// ---------- Navigation (animated, see FrontendApplication) ----------

void SettingScreenView::goBack()
{
    application().gotoHome();
}

void SettingScreenView::openHaptics()
{
    application().gotoSettingsHaptics();
}

void SettingScreenView::openLighting()
{
    application().gotoSettingsLighting();
}

void SettingScreenView::openColour()
{
    application().gotoSettingsColour();
}
