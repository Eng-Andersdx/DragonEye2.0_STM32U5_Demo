#include <gui/hoblevelscreen_screen/HobLevelScreenView.hpp>
#include <touchgfx/Unicode.hpp>
#include <touchgfx/EasingEquations.hpp>
#include "dragoneye_leds.h"
using namespace touchgfx;


HobLevelScreenView::HobLevelScreenView() {}

void HobLevelScreenView::setupScreen()
{
    HobLevelScreenViewBase::setupScreen();
    BSP_LED_SetColor(RGB_RED);

    // Which hob are we editing?
    currentHob = presenter->getPendingHob();
    level      = presenter->getHobLevel(currentHob);

    // Dim all, highlight selected (same 55/255 convention)
    hob_bl.setAlpha(55);
    hob_tl.setAlpha(55);
    hob_m .setAlpha(55);
    hob_tr.setAlpha(55);
    hob_br.setAlpha(55);

    switch (currentHob) {
        case Model::BL: hob_bl.setAlpha(255); break;
        case Model::TL: hob_tl.setAlpha(255); break;
        case Model::M:  hob_m .setAlpha(255); break;
        case Model::TR: hob_tr.setAlpha(255); break;
        case Model::BR: hob_br.setAlpha(255); break;
        default: break;
    }

    // Show initial level digit (TextArea id: lvl_value)
    Unicode::snprintf(buf, sizeof(buf) / sizeof(buf[0]), "%d", level);
    lvl_value.setWildcard(buf);
    lvl_value.invalidate();
}

void HobLevelScreenView::tearDownScreen()
{
    HobLevelScreenViewBase::tearDownScreen();
}

void HobLevelScreenView::refreshDigits()
{
    Unicode::snprintf(buf, sizeof(buf) / sizeof(buf[0]), "%d", level);
    lvl_value.invalidate();
}

// Designer: plus_button → onPlusClicked
void HobLevelScreenView::onPlusClicked()
{
    if (level < 9) { level++; refreshDigits(); }
}

// Designer: minus_button → onMinusClicked
void HobLevelScreenView::onMinusClicked()
{
    if (level > 0) { level--; refreshDigits(); }
}

// Designer: back_button → onDoneClicked
void HobLevelScreenView::onDoneClicked()
{
    // Persist to model and go back
    presenter->setHobLevel(currentHob, level);
    presenter->goBackToHobScreen();
}

// Hardware button 0 → same as minus_button
void HobLevelScreenView::goLeft()
{
    onMinusClicked();
}

// Hardware button 1 → same as plus_button
void HobLevelScreenView::goRight()
{
    onPlusClicked();
}
