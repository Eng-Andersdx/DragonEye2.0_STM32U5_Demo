#include <gui/hobscreen_screen/HobScreenView.hpp>
#include <touchgfx/EasingEquations.hpp>
#include "dragoneye_leds.h"
using namespace touchgfx;

namespace {
    inline void writeDigit(touchgfx::Unicode::UnicodeChar* dst, uint8_t lvl)
    {
        // lvl 0 => "", lvl 1..9 => "1".."9"
        if (lvl == 0) { dst[0] = 0; return; }
        Unicode::snprintf(dst, 3, "%d", lvl);
    }
}

void HobScreenView::setupScreen()
{
    HobScreenViewBase::setupScreen();
    BSP_LED_SetColor(RGB_RED);


    // Hook all wildcards + make visible; sizing will be handled per-update
    lvl_BL.setWildcard(bufBL); lvl_BL.setVisible(true); lvl_BL.setAlpha(255);
    lvl_TL.setWildcard(bufTL); lvl_TL.setVisible(true); lvl_TL.setAlpha(255);
    lvl_M .setWildcard(bufM ); lvl_M .setVisible(true); lvl_M .setAlpha(255);
    lvl_TR.setWildcard(bufTR); lvl_TR.setVisible(true); lvl_TR.setAlpha(255);
    lvl_BR.setWildcard(bufBR); lvl_BR.setVisible(true); lvl_BR.setAlpha(255);

    refreshAllFromModel();
}

// ---------- Button handlers (all identical pattern) ----------
void HobScreenView::onHobBLClicked() { setAlphaFor(Model::BL, 255, 10); presenter->setPendingHob(Model::BL); presenter->goToHobLevelScreen(); }
void HobScreenView::onHobTLClicked() { setAlphaFor(Model::TL, 255, 10); presenter->setPendingHob(Model::TL); presenter->goToHobLevelScreen(); }
void HobScreenView::onHobMClicked()  { setAlphaFor(Model::M , 255, 10); presenter->setPendingHob(Model::M ); presenter->goToHobLevelScreen(); }
void HobScreenView::onHobTRClicked() { setAlphaFor(Model::TR, 255, 10); presenter->setPendingHob(Model::TR); presenter->goToHobLevelScreen(); }
void HobScreenView::onHobBRClicked() { setAlphaFor(Model::BR, 255, 10); presenter->setPendingHob(Model::BR); presenter->goToHobLevelScreen(); }

// Debug helpers – keep as quick jump
void HobScreenView::onPlusClicked()  { presenter->setPendingHob(Model::BL); presenter->goToHobLevelScreen(); }
void HobScreenView::onMinusClicked() {}

// ---------- Paint from model ----------
void HobScreenView::refreshAllFromModel()
{
    const uint8_t lvBL = presenter->getHobLevel(Model::BL);
    const uint8_t lvTL = presenter->getHobLevel(Model::TL);
    const uint8_t lvM  = presenter->getHobLevel(Model::M );
    const uint8_t lvTR = presenter->getHobLevel(Model::TR);
    const uint8_t lvBR = presenter->getHobLevel(Model::BR);

    showLevelDigit(Model::BL, lvBL); setAlphaFor(Model::BL, lvBL ? 255 : 80, 0);
    showLevelDigit(Model::TL, lvTL); setAlphaFor(Model::TL, lvTL ? 255 : 80, 0);
    showLevelDigit(Model::M , lvM ); setAlphaFor(Model::M , lvM  ? 255 : 80, 0);
    showLevelDigit(Model::TR, lvTR); setAlphaFor(Model::TR, lvTR ? 255 : 80, 0);
    showLevelDigit(Model::BR, lvBR); setAlphaFor(Model::BR, lvBR ? 255 : 80, 0);
}

void HobScreenView::showLevelDigit(Model::HobIndex i, uint8_t lvl)
{
    TextArea* ta = nullptr;
    touchgfx::Unicode::UnicodeChar* buf = nullptr;

    switch (i)
    {
    case Model::BL: ta = &lvl_BL; buf = bufBL; break;
    case Model::TL: ta = &lvl_TL; buf = bufTL; break;
    case Model::M : ta = &lvl_M ; buf = bufM ; break;
    case Model::TR: ta = &lvl_TR; buf = bufTR; break;
    case Model::BR: ta = &lvl_BR; buf = bufBR; break;
    default: return;
    }

    writeDigit(buf, lvl);
    ta->setVisible(lvl > 0);
    ta->resizeToCurrentText();
    ta->invalidate();
}

void HobScreenView::setAlphaFor(Model::HobIndex i, uint8_t a, uint16_t dur)
{
    FadeAnimator<Image>* img = nullptr;
    switch (i)
    {
    case Model::BL: img = &hob_bl; break;
    case Model::TL: img = &hob_tl; break;
    case Model::M : img = &hob_m ; break;
    case Model::TR: img = &hob_tr; break;
    case Model::BR: img = &hob_br; break;
    default: return;
    }

    if (dur == 0) { img->setAlpha(a); img->invalidate(); }
    else          { img->startFadeAnimation(a, dur, EasingEquations::linearEaseNone); }
}
