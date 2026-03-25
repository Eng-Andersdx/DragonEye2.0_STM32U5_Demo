#ifndef OVENTEMPVIEWVIEW_HPP
#define OVENTEMPVIEWVIEW_HPP

#include <gui_generated/oventempview_screen/OvenTempViewViewBase.hpp>
#include <gui/oventempview_screen/OvenTempViewPresenter.hpp>

class OvenTempViewView : public OvenTempViewViewBase
{
public:
    OvenTempViewView();
    virtual ~OvenTempViewView() {}

    // keep these overrides so the signatures match what the base expects
    virtual void setupScreen() override;
    virtual void tearDownScreen() override;

    // temperature control API
    void setTemperature(int16_t celsius, bool animate = false);
    void increaseTemp();   // +10°
    void decreaseTemp();   // -10°
    void onPlusClicked();
    void onMinusClicked();

    // Hardware buttons (0 = left, 1 = right)
    virtual void H_Left() override;   // HW button 0
    virtual void H_Right() override;  // HW button 1

private:
    void updateNumber(int16_t celsius);
    int16_t quantize10(int16_t v) const;
	  int16_t mapTempToY(int16_t celsius) const;

    // buffer for wildcard text, enough for "270°\0"
    touchgfx::Unicode::UnicodeChar tempBuffer[6];
    touchgfx::Unicode::UnicodeChar fahrenheitBuffer[6];  // °F digits

    // current temperature state
    int16_t currentCelsius = 0;
};

#endif // OVENTEMPVIEWVIEW_HPP
