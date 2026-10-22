#include <gui/menuscreen_screen/MenuScreenView.hpp>
#include <touchgfx/Color.hpp>
#include <touchgfx/canvas_widget_renderer/CanvasWidgetRenderer.hpp>
#include <gui/common/HwInterface.hpp>
extern int LED_mode;
extern "C" volatile uint8_t status_wifi_connected;
extern "C" volatile uint8_t status_bt_connected;

// Status icons sit in the upper-left gap between the top (selected) wheel icon
// and the left wheel icon, stepped along the round edge (~185px from centre).
static const int16_t kWifiX    = 74;
static const int16_t kWifiY    = 100;
static const int16_t kBtX      = 106;
static const int16_t kBtY      = 74;
static const uint8_t kAlphaOn  = 255;
static const uint8_t kAlphaOff = 70;

static void setMenuLed(int idx)
{
    if (LED_mode != 1)
        return; // Fixed / Off mode chosen in Settings

    switch (idx)
    {
    case 0:  BSP_LED_SetColor(RGB_WHITE);  break; // Volume
    case 1:  BSP_LED_SetColor(RGB_RED);    break; // Settings
    case 2:  BSP_LED_SetColor(RGB_OVEN);   break; // Oven
    case 3:  BSP_LED_SetColor(RGB_COFFEE); break; // Coffee
    case 4:  BSP_LED_SetColor(RGB_GREEN);  break; // Exercise
    default: BSP_LED_SetColor(RGB_WHITE);  break;
    }
}

using namespace touchgfx;

// Home-screen transition speeds (frames). Lower = faster.
static const int kBgFadeTicks    = 16;   // background colour cross-fade (was 40)
static const int kTitleFadeTicks = 12;   // title fade-in (was effectively 120)

MenuScreenView::MenuScreenView()
    : wheelSelectionChangedCb(this, &MenuScreenView::onWheelSelectionChanged)
    , wheelActivatedCb(this, &MenuScreenView::onWheelActivated)
    , wheelTargetChangedCb(this, &MenuScreenView::onWheelSelectionChanged)
{
    // Status icons, added last so they draw on top
    wifiIcon.setBitmap(Bitmap(BITMAP_STATUS_WIFI_ID));
    wifiIcon.setXY(kWifiX, kWifiY);

    btIcon.setBitmap(Bitmap(BITMAP_STATUS_BT_ID));
    btIcon.setXY(kBtX, kBtY);

    add(wifiIcon);
    add(btIcon);
}

void MenuScreenView::handleTickEvent()
{
    MenuScreenViewBase::handleTickEvent();
    updateStatusBar(false);
}

void MenuScreenView::updateStatusBar(bool force)
{
    // Connection icons: bright when connected, dimmed when not
    const uint8_t wa = status_wifi_connected ? kAlphaOn : kAlphaOff;
    const uint8_t ba = status_bt_connected   ? kAlphaOn : kAlphaOff;
    if (force || wifiIcon.getAlpha() != wa) { wifiIcon.setAlpha(wa); wifiIcon.invalidate(); }
    if (force || btIcon.getAlpha()   != ba) { btIcon.setAlpha(ba);   btIcon.invalidate(); }
}

void MenuScreenView::setupScreen()
{
    MenuScreenViewBase::setupScreen();

    // The wheel's accent rings are canvas widgets and need a render buffer
    touchgfx::CanvasWidgetRenderer::setupBuffer(canvasBuffer, CANVAS_BUFFER_SIZE);

    // Backgrounds are now full-screen 480x480
    bgBack.setXY(0, 0);
    bgFront.setXY(0, 0);

    // Configure wheel
    menuWheel1.configure(iconIdsSmall, iconIdsBig);
    menuWheel1.setSelectionChangedCallback(wheelSelectionChangedCb);
    menuWheel1.setActivatedCallback(wheelActivatedCb);
    menuWheel1.setTargetChangedCallback(wheelTargetChangedCb);
    menuWheel1.setEnabled(true);

    // Initial selection
    const int idx = menuWheel1.getSelectedIndex();
    shownIndex = idx;
    setTitle(titleIds[idx], false);
    setBackground(bgIds[idx], false);
    applyAccent(idx);
    setMenuLed(idx);

    currentBgId = bgIds[idx];
    hasBgId = true;

    updateStatusBar(true);
}


void MenuScreenView::tearDownScreen()
{
    MenuScreenViewBase::tearDownScreen();
    touchgfx::CanvasWidgetRenderer::resetBuffer();
}

void MenuScreenView::applyAccent(int idx)
{
    if (idx < 0 || idx >= WHEEL_ITEMS)
        return;
    menuWheel1.setAccentColor(touchgfx::Color::getColorFromRGB(accentRGB[idx][0],
                                                                accentRGB[idx][1],
                                                                accentRGB[idx][2]));
}

void MenuScreenView::onWheelSelectionChanged(int idx)
{
    // Called early (as soon as the wheel knows its target) and again when it lands;
    // only act when the item actually changes.
    if (idx < 0 || idx >= WHEEL_ITEMS || idx == shownIndex)
        return;
    shownIndex = idx;

    setTitle(titleIds[idx], true);
    setBackground(bgIds[idx], true);
    applyAccent(idx);

    setMenuLed(idx);
}

void MenuScreenView::onWheelActivated(int idx)
{
    // Center icon tapped -> go to the corresponding screen
    switch (idx)
    {
    case 0: // Volume
        application().gotoVolume();
        break;

    case 1: // Settings
        application().gotoSettings();
        break;

    case 2: // Oven
        application().gotoOven();
        break;

    case 3: // Coffee
        application().gotoCoffee();
        break;

    case 4: // Exercise
        application().gotoExercise();
        break;

    default:
        break;
    }
}

void MenuScreenView::setTitle(uint16_t textId, bool animate)
{
    if (animate)
    {
        menu_title.setTypedText(TypedText(textId));
        menu_title.setAlpha(0);
        menu_title.invalidate();
        menu_title.startFadeAnimation(255, kTitleFadeTicks);
    }
    else
    {
        menu_title.setTypedText(TypedText(textId));
        menu_title.invalidate();
    }
}

void MenuScreenView::setBackground(BitmapId bmpId, bool animate)
{
    if (animate && hasBgId)
    {
        touchgfx::BitmapId old = currentBgId;
        currentBgId = bmpId;

        bgBack.setBitmap(Bitmap(old));
        bgBack.setAlpha(255);
        bgBack.invalidate();

        bgFront.setBitmap(Bitmap(bmpId));
        bgFront.setAlpha(0);
        bgFront.invalidate();
        bgFront.startFadeAnimation(255, kBgFadeTicks);
    }
    else
    {
        currentBgId = bmpId;
        hasBgId = true;

        bgFront.setBitmap(Bitmap(bmpId));
        bgBack.setBitmap(Bitmap(bmpId));
        bgFront.setAlpha(255);
        bgBack.setAlpha(255);
        bgFront.invalidate();
        bgBack.invalidate();
    }
}

void MenuScreenView::setCenterIcon(int idx, bool animate)
{
    if (idx < 0 || idx >= WHEEL_ITEMS)
        return;

    touchgfx::BitmapId bmpId = iconIdsBig[idx];
    touchgfx::Bitmap bmp(bmpId);

    // Preserve current visual center
    int oldCenterX = iconCenter.getX() + iconCenter.getWidth()  / 2;
    int oldCenterY = iconCenter.getY() + iconCenter.getHeight() / 2;

    // Apply new bitmap
    iconCenter.setBitmap(bmp);

    // Make the TextureMapper the same size as the bitmap
    iconCenter.setWidth(bmp.getWidth());
    iconCenter.setHeight(bmp.getHeight());

    // Draw bitmap starting at (0,0) inside the TextureMapper
    iconCenter.setBitmapPosition(0.0f, 0.0f);

    // Lock origo to center of the bitmap (Z = current camera distance)
    float cam = iconCenter.getCameraDistance();
    iconCenter.setOrigo(bmp.getWidth() * 0.5f,
                        bmp.getHeight() * 0.5f,
                        cam);

    // Re-center the widget at the same screen position as before
    iconCenter.setXY(oldCenterX - iconCenter.getWidth()  / 2,
                     oldCenterY - iconCenter.getHeight() / 2);

    // Now handle fade / alpha
    if (animate)
    {
        iconCenter.startFadeAnimation(0, 40);
        iconCenter.invalidate();
        iconCenter.startFadeAnimation(255, 40);
    }
    else
    {
        iconCenter.setAlpha(255);
        iconCenter.invalidate();
    }
}

void MenuScreenView::goLeft()
{
    // Animate one step “counter-clockwise”
    menuWheel1.animateStep(-1);
}

void MenuScreenView::goRight()
{
    // Animate one step “clockwise”
    menuWheel1.animateStep(+1);
}

void MenuScreenView::H_Press()
{
    // Ask the wheel which item is currently selected
    int idx = menuWheel1.getSelectedIndex();

    // Reuse the normal activation logic
    onWheelActivated(idx);
}
