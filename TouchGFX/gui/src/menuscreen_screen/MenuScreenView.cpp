#include <gui/menuscreen_screen/MenuScreenView.hpp>
#include "dragoneye_leds.h"

using namespace touchgfx;

MenuScreenView::MenuScreenView()
    : wheelSelectionChangedCb(this, &MenuScreenView::onWheelSelectionChanged)
    , wheelActivatedCb(this, &MenuScreenView::onWheelActivated)
{
}

void MenuScreenView::setupScreen()
{
    MenuScreenViewBase::setupScreen();

    // Configure wheel
    menuWheel1.configure(iconIdsSmall, iconIdsBig);
    menuWheel1.setSelectionChangedCallback(wheelSelectionChangedCb);
    menuWheel1.setActivatedCallback(wheelActivatedCb);
    menuWheel1.setEnabled(true);

    // Initial selection
    const int idx = menuWheel1.getSelectedIndex();
    setTitle(titleIds[idx], false);
    setBackground(bgIds[idx], false);

    currentBgId = bgIds[idx];
    hasBgId = true;
}


void MenuScreenView::tearDownScreen()
{
    MenuScreenViewBase::tearDownScreen();
}

void MenuScreenView::onWheelSelectionChanged(int idx)
{
    if (idx < 0 || idx >= WHEEL_ITEMS)
        return;

    setTitle(titleIds[idx], true);
    setBackground(bgIds[idx], true);

    switch (idx)
       {
       case 0:	BSP_LED_SetColor(RGB_WHITE);	break;	//Volume
       case 1:	BSP_LED_SetColor(RGB_RED);		break;	//Hob
       case 2:	BSP_LED_SetColor(RGB_OVEN);		break;	//Oven
       case 3:	BSP_LED_SetColor(RGB_COFFEE);	break;	//Coffee
       case 4:	BSP_LED_SetColor(RGB_GREEN);	break;	//Exercise
       default:	BSP_LED_SetColor(RGB_WHITE);	break;
       }
}

void MenuScreenView::onWheelActivated(int idx)
{
    // Center icon tapped -> go to the corresponding screen
    switch (idx)
    {
    case 0: // Volume
        application().gotoVolumeScreenNoTransition();
        break;

    case 1: // Hob
        application().gotoHobScreenScreenNoTransition();
        break;

    case 2: // Oven
        application().gotoOvenTempViewScreenNoTransition();
        break;

    case 3: // Coffee
        application().gotoCoffeeScreenNoTransition();
        break;

    case 4: // Exercise
        application().gotoExerciseScreenScreenNoTransition();
        break;

    default:
        break;
    }
}

void MenuScreenView::setTitle(uint16_t textId, bool animate)
{
    if (animate)
    {
        menu_title.startFadeAnimation(0, 60);
        menu_title.setTypedText(TypedText(textId));
        menu_title.invalidate();
        menu_title.startFadeAnimation(255, 120);
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
        bgFront.startFadeAnimation(255, 40);
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
