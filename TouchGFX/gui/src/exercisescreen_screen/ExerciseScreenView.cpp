#include <gui/exercisescreen_screen/ExerciseScreenView.hpp>
#include <touchgfx/EasingEquations.hpp>
#include <BitmapDatabase.hpp>
#include <texts/TextKeysAndLanguages.hpp>
#include <touchgfx/Font.hpp>
#include <gui/exercisescreen_screen/ExerciseScreenView.hpp>
#include <gui/common/FrontendApplication.hpp>
#include <touchgfx/Application.hpp>
#include "dragoneye_leds.h"

using namespace touchgfx;

ExerciseScreenView::ExerciseScreenView() {}

void ExerciseScreenView::setupScreen()
{
    ExerciseScreenViewBase::setupScreen();
    BSP_LED_SetColor(RGB_GREEN);

    initItems();

    // ---------- Icons ----------
    iconA.setBitmap(Bitmap(items[0].bmp));
    iconA.setXY(CENTER_X - iconA.getWidth()/2, ICON_Y - iconA.getHeight()/2);
    iconA.setAlpha(255);
    add(iconA);

    iconB.setBitmap(Bitmap(items[1].bmp));
    iconB.setXY(CENTER_X - iconB.getWidth()/2 + OFF, ICON_Y - iconB.getHeight()/2);
    iconB.setAlpha(0);
    add(iconB);

    // ---------- Label (animated) ----------
    label.setVisible(false);
    label.invalidate();

    animLabel.setTypedText(items[0].txt);
    animLabel.setColor(label.getColor());

    // Measure string width, make the TextArea exactly that width, then center
    const int16_t w0 = textPixelWidth(items[0].txt);
    const int16_t x0 = centeredXForWidth(w0);

    animLabel.setWidth(w0);
    animLabel.setHeight(label.getHeight()); // reuse height from Designer
    labelBaseY = label.getY();

    // Start lower + transparent, then rise + fade in
    animLabel.setXY(x0, labelBaseY + LABEL_RISE);
    animLabel.setAlpha(0);
    add(animLabel);

    animLabel.startMoveAnimation(x0, labelBaseY, LABEL_DUR, EasingEquations::quadEaseOut);
    animLabel.startFadeAnimation(255, LABEL_DUR, EasingEquations::quadEaseOut);

    currentIndex = 0;
    aIsFront = true;
}

void ExerciseScreenView::tearDownScreen()
{
    ExerciseScreenViewBase::tearDownScreen();
}

void ExerciseScreenView::handleTickEvent()
{
    ExerciseScreenViewBase::handleTickEvent();
}

// ------------ Navigation ------------

void ExerciseScreenView::goRight()
{
    const int nextIndex = wrap(currentIndex + 1);
    const Item& nextItem = items[nextIndex];

    if (aIsFront)
        animateInFromRight(nextItem, iconB, iconA);
    else
        animateInFromRight(nextItem, iconA, iconB);

    updateLabelTo(nextItem);
    currentIndex = nextIndex;
    aIsFront = !aIsFront;
}

void ExerciseScreenView::goLeft()
{
    const int prevIndex = wrap(currentIndex - 1);
    const Item& prevItem = items[prevIndex];

    if (aIsFront)
        animateInFromLeft(prevItem, iconB, iconA);
    else
        animateInFromLeft(prevItem, iconA, iconB);

    updateLabelTo(prevItem);
    currentIndex = prevIndex;
    aIsFront = !aIsFront;
}

// ------------ Helpers ------------

void ExerciseScreenView::initItems()
{
    // Use your generated bitmap and text IDs
    items[0].bmp = BITMAP_STRETCH_ICON_ID;   // Stretch_icon.png
    items[0].txt = TypedText(T_T_EXERCISE);

    items[1].bmp = BITMAP_RUN_ICON_ID;       // Run_icon.png
    items[1].txt = TypedText(T_T_RUNNING);

    items[2].bmp = BITMAP_CYCLE_ICON_ID;     // Cycle_icon.png
    items[2].txt = TypedText(T_T_CYCLING);
}

void ExerciseScreenView::animateInFromRight(
    const Item& item,
    FadeAnimator< MoveAnimator<Image> >& inSlot,
    FadeAnimator< MoveAnimator<Image> >& outSlot)
{
    inSlot.setBitmap(Bitmap(item.bmp));
    inSlot.setXY(CENTER_X - inSlot.getWidth()/2 + OFF, ICON_Y - inSlot.getHeight()/2);
    inSlot.setAlpha(0);

    // outgoing -> left & fade out
    outSlot.startMoveAnimation(CENTER_X - outSlot.getWidth()/2 - OFF,
                               ICON_Y - outSlot.getHeight()/2,
                               DURATION, EasingEquations::cubicEaseInOut);
    outSlot.startFadeAnimation(0, DURATION, EasingEquations::cubicEaseInOut);

    // incoming -> center & fade in
    inSlot.startMoveAnimation(CENTER_X - inSlot.getWidth()/2,
                              ICON_Y - inSlot.getHeight()/2,
                              DURATION, EasingEquations::cubicEaseInOut);
    inSlot.startFadeAnimation(255, DURATION, EasingEquations::cubicEaseInOut);

    inSlot.invalidate();
    outSlot.invalidate();
}

void ExerciseScreenView::animateInFromLeft(
    const Item& item,
    FadeAnimator< MoveAnimator<Image> >& inSlot,
    FadeAnimator< MoveAnimator<Image> >& outSlot)
{
    inSlot.setBitmap(Bitmap(item.bmp));
    inSlot.setXY(CENTER_X - inSlot.getWidth()/2 - OFF, ICON_Y - inSlot.getHeight()/2);
    inSlot.setAlpha(0);

    // outgoing -> right & fade out
    outSlot.startMoveAnimation(CENTER_X - outSlot.getWidth()/2 + OFF,
                               ICON_Y - outSlot.getHeight()/2,
                               DURATION, EasingEquations::cubicEaseInOut);
    outSlot.startFadeAnimation(0, DURATION, EasingEquations::cubicEaseInOut);

    // incoming -> center & fade in
    inSlot.startMoveAnimation(CENTER_X - inSlot.getWidth()/2,
                              ICON_Y - inSlot.getHeight()/2,
                              DURATION, EasingEquations::cubicEaseInOut);
    inSlot.startFadeAnimation(255, DURATION, EasingEquations::cubicEaseInOut);

    inSlot.invalidate();
    outSlot.invalidate();
}

int16_t ExerciseScreenView::textPixelWidth(const TypedText& tt) const
{
    const Font* font = tt.getFont();
    const Unicode::UnicodeChar* str = tt.getText(); // resolved translation
    int w = (font && str) ? font->getStringWidth(str) : 0;
    return static_cast<int16_t>(w < 0 ? 0 : w);
}

void ExerciseScreenView::updateLabelTo(const Item& item)
{
    // 1) Invalidate the *old* rect so the previous text gets cleared
    animLabel.invalidate();

    // 2) Re-measure string width and center
    const int16_t w  = textPixelWidth(item.txt);
    const int16_t cx = centeredXForWidth(w);

    // 3) Update content and geometry
    animLabel.setTypedText(item.txt);
    animLabel.setWidth(w);                // new width
    // keep same height as before

    // Reset below baseline & invisible with new string
    animLabel.setAlpha(0);
    animLabel.setXY(cx, labelBaseY + LABEL_RISE);

    // 4) Invalidate the *new* rect so first frame is drawn correctly
    animLabel.invalidate();

    // 5) Animate rise + fade in
    animLabel.startMoveAnimation(cx, labelBaseY,
                                 LABEL_DUR, EasingEquations::quadEaseOut);
    animLabel.startFadeAnimation(255, LABEL_DUR,
                                 EasingEquations::quadEaseOut);
}

void ExerciseScreenView::Left()
{
    goLeft();   // behave exactly like the touchscreen left arrow
}

void ExerciseScreenView::Right()
{
    goRight();  // behave exactly like the touchscreen right arrow
}

void ExerciseScreenView::H_Select()
{
    // Do exactly what the flexButton1 "Change screen → RunningScreen" does
    static_cast<FrontendApplication*>(Application::getInstance())
        ->gotoRunningScreenScreenNoTransition();    // name matches RunningScreen
}
