#ifndef VOLUMEVIEW_HPP
#define VOLUMEVIEW_HPP

#include <gui_generated/volume_screen/VolumeViewBase.hpp>
#include <gui/volume_screen/VolumePresenter.hpp>
#include <cstdint>

class VolumeView : public VolumeViewBase
{
public:
    VolumeView() {}
    virtual ~VolumeView() {}

    virtual void setupScreen() override;
    virtual void tearDownScreen() override;

    // Wired from Designer (Interactions on the two buttons)
    virtual void onPlusClicked() override;
    virtual void onMinusClicked() override;

    // OPTIONAL: if you also wire volRing -> Value updated -> Call new virtual function
    virtual void onRingValueUpdated(); // only implement if you wire it

    // *** Hardware bezel callbacks ***
    virtual void goRight() override; // HW button 1 → volume up
    virtual void goLeft()  override; // HW button 0 → volume down

private:
    void setRingValue(int newValue); // clamps 0..100, updates ring + icon
    void updateIconsFor(int value);  // shows exactly one icon
};

#endif // VOLUMEVIEW_HPP
