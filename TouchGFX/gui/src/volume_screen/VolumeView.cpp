#include <gui/volume_screen/VolumeView.hpp>
#include "dragoneye_leds.h"

using namespace touchgfx;

void VolumeView::setupScreen()
{
    VolumeViewBase::setupScreen();
    BSP_LED_SetColor(RGB_WHITE);

    // Ensure a known start: show “0” and hide others (Designer visibility doesn’t matter)
    vol_0.setVisible(true);
    vol_1.setVisible(false);
    vol_33.setVisible(false);
    vol_66.setVisible(false);

    // Ensure icons match current ring value from Designer (Initial property)
    updateIconsFor(volRing.getValue());
}

void VolumeView::tearDownScreen()
{
    VolumeViewBase::tearDownScreen();
}

void VolumeView::onPlusClicked()
{
    setRingValue(volRing.getValue() + 5); // step by 5 (tweak as you like)
}

void VolumeView::onMinusClicked()
{
    setRingValue(volRing.getValue() - 5);
}

// OPTIONAL: only implement if you wired the ring’s “Value updated” trigger to this.
void VolumeView::onRingValueUpdated()
{
    updateIconsFor(volRing.getValue());
}

void VolumeView::setRingValue(int newValue)
{
    if (newValue < 0)   newValue = 0;
    if (newValue > 100) newValue = 100;

    int old = volRing.getValue();
    if (newValue == old) return;

    volRing.setValue(newValue);   // instant; if you added duration in Designer it will animate
    updateIconsFor(newValue);
}

void VolumeView::updateIconsFor(int value)
{
    // Decide which icon should be visible
    bool show0 = (value == 0);
    bool show1 = (value >= 1  && value <= 33);
    bool show33= (value >= 34 && value <= 66);
    bool show66= (value >= 67); // 67..100

    // Only invalidate the ones that actually change (optional micro-optimisation)
    if (vol_0.isVisible() != show0) { vol_0.setVisible(show0); vol_0.invalidate(); }
    if (vol_1.isVisible() != show1) { vol_1.setVisible(show1); vol_1.invalidate(); }
    if (vol_33.isVisible() != show33){ vol_33.setVisible(show33); vol_33.invalidate(); }
    if (vol_66.isVisible() != show66){ vol_66.setVisible(show66); vol_66.invalidate(); }
}

void VolumeView::goRight()
{
    // Hardware button 1 (clockwise): volume up
    onPlusClicked();
}

void VolumeView::goLeft()
{
    // Hardware button 0 (anti-clockwise): volume down
    onMinusClicked();
}
