#include <gui/settingshapticscreen_screen/SettingsHapticScreenView.hpp>
#include <touchgfx/Unicode.hpp>

extern "C" volatile uint8_t haptic_feedback_intensity;

SettingsHapticScreenView::SettingsHapticScreenView()
{
}

void SettingsHapticScreenView::setupScreen()
{
    SettingsHapticScreenViewBase::setupScreen();

    // Snap slider to nearest 5
    haptic_slider.setValue(haptic_feedback_intensity);
    updateHapticLabel(haptic_feedback_intensity);
}

void SettingsHapticScreenView::tearDownScreen()
{
    SettingsHapticScreenViewBase::tearDownScreen();
}

void SettingsHapticScreenView::onHapticSliderChanged(int value)
{
    // Round to nearest 5
    int raw = haptic_slider.getValue();
    int snapped = (raw / 5) * 5;

    haptic_feedback_intensity = (uint8_t)snapped;
    updateHapticLabel((uint8_t)snapped);
}

void SettingsHapticScreenView::onHapticDefaultClicked()
{
    haptic_feedback_intensity = 35;
    haptic_slider.setValue(35);
    updateHapticLabel(35);
}

void SettingsHapticScreenView::updateHapticLabel(uint8_t value)
{
    // Build "XXms" string into the TextArea buffer
    Unicode::UnicodeChar buf[8];
    Unicode::snprintf(buf, 8, "%dms", value);
    haptic_value_label.setWildcard(buf);
    haptic_value_label.invalidate();
}

// ---------- Navigation (animated, see FrontendApplication) ----------

void SettingsHapticScreenView::goBack()
{
    application().gotoSettings(true);
}
