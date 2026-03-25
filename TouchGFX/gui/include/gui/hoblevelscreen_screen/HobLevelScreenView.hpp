#ifndef HOBLEVELSCREENVIEW_HPP
#define HOBLEVELSCREENVIEW_HPP

#include <gui_generated/hoblevelscreen_screen/HobLevelScreenViewBase.hpp>
#include <gui/hoblevelscreen_screen/HobLevelScreenPresenter.hpp>
#include <gui/model/Model.hpp>
#include <touchgfx/Unicode.hpp>
#include <cstdint>

class HobLevelScreenView : public HobLevelScreenViewBase
{
public:
    HobLevelScreenView();
    virtual ~HobLevelScreenView() {}

    virtual void setupScreen() override;
    virtual void tearDownScreen() override;

    // wired from Designer
    void onPlusClicked();
    void onMinusClicked();
    void onDoneClicked();

    // <<< hardware interactions >>>
    virtual void goLeft();   // hardware button 0
    virtual void goRight();  // hardware button 1

private:
    void refreshDigits(); // writes current level into TextArea 'lvl_value'

    Model::HobIndex currentHob = Model::BL;        // which hob we're editing
    uint8_t level = 0;                              // 0..9
    touchgfx::Unicode::UnicodeChar buf[4] = {0};    // "9\0"
};

#endif // HOBLEVELSCREENVIEW_HPP
