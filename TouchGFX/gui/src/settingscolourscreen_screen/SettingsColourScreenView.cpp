#include <gui/settingscolourscreen_screen/SettingsColourScreenView.hpp>

#include <BitmapDatabase.hpp>

extern int LED_mode;

// Index of the colour last picked here (-1 = none). Cleared by the Lighting page
// and Reset when the LED is no longer showing a picked colour.
int colour_picker_selected = -1;

enum { C_WHITE, C_RED, C_GREEN, C_BLUE, C_CYAN, C_YELLOW, C_ORANGE, C_MAGENTA, C_COUNT };

static const touchgfx::BitmapId kUnselected[C_COUNT] =
{
    BITMAP_WHITE_UNSELECTED_ID, BITMAP_RED_UNSELECTED_ID, BITMAP_GREEN_UNSELECTED_ID, BITMAP_BLUE_UNSELECTED_ID,
    BITMAP_CYAN_UNSELECTED_ID,  BITMAP_YELLOW_UNSELECTED_ID, BITMAP_ORANGE_UNSELECTED_ID, BITMAP_MAGENTA_UNSELECTED_ID
};
static const touchgfx::BitmapId kSelected[C_COUNT] =
{
    BITMAP_WHITE_SELECTED_ID, BITMAP_RED_SELECTED_ID, BITMAP_GREEN_SELECTED_ID, BITMAP_BLUE_SELECTED_ID,
    BITMAP_CYAN_SELECTED_ID,  BITMAP_YELLOW_SELECTED_ID, BITMAP_ORANGE_SELECTED_ID, BITMAP_MAGENTA_SELECTED_ID
};

SettingsColourScreenView::SettingsColourScreenView()
{
}

void SettingsColourScreenView::setupScreen()
{
    SettingsColourScreenViewBase::setupScreen();

    // Show which colour is active (only while the LED is in fixed mode)
    markSelected(LED_mode == 0 ? colour_picker_selected : -1);
}

void SettingsColourScreenView::markSelected(int idx)
{
    touchgfx::Button* buttons[C_COUNT] =
    {
        &white_button, &red_button, &green_button, &blue_button,
        &cyan_button,  &yellow_button, &orange_button, &magenta_button
    };

    colour_picker_selected = idx;
    for (int i = 0; i < C_COUNT; ++i)
    {
        // Selected colour keeps its white ring; the others show the ring only while pressed
        const touchgfx::BitmapId released = (i == idx) ? kSelected[i] : kUnselected[i];
        buttons[i]->setBitmaps(touchgfx::Bitmap(released), touchgfx::Bitmap(kSelected[i]));
        buttons[i]->invalidate();
    }
}

void SettingsColourScreenView::tearDownScreen()
{
    SettingsColourScreenViewBase::tearDownScreen();
}

void SettingsColourScreenView::applyColour(uint8_t r, uint8_t g, uint8_t b)
{
    LED_mode = 0; // Fixed mode
    BSP_LED_SetRGBIntensity(r, g, b);
}

void SettingsColourScreenView::onWhiteButtonClicked()
{
    applyColour(100, 100, 100);
    markSelected(C_WHITE);
}

void SettingsColourScreenView::onRedButtonClicked()
{
    applyColour(100, 0, 0);
    markSelected(C_RED);
}

void SettingsColourScreenView::onGreenButtonClicked()
{
    applyColour(0, 100, 0);
    markSelected(C_GREEN);
}

void SettingsColourScreenView::onBlueButtonClicked()
{
    applyColour(0, 0, 100);
    markSelected(C_BLUE);
}

void SettingsColourScreenView::onCyanButtonClicked()
{
    applyColour(0, 100, 100);
    markSelected(C_CYAN);
}

void SettingsColourScreenView::onYellowButtonClicked()
{
    applyColour(100, 100, 0);
    markSelected(C_YELLOW);
}

void SettingsColourScreenView::onOrangeButtonClicked()
{
    applyColour(100, 50, 0);
    markSelected(C_ORANGE);
}

void SettingsColourScreenView::onMagentaButtonClicked()
{
    applyColour(100, 0, 100);
    markSelected(C_MAGENTA);
}

// ---------- Navigation (animated, see FrontendApplication) ----------

void SettingsColourScreenView::goBack()
{
    application().gotoSettings(true);
}
