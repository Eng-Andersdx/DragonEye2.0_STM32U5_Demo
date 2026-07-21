#ifndef MAINVIEW_HPP
#define MAINVIEW_HPP

#include <gui_generated/main_screen/MainViewBase.hpp>
#include <gui/main_screen/MainPresenter.hpp>

#include <touchgfx/mixins/FadeAnimator.hpp>
#include <gui/containers/MainWheelIcon.hpp>

class MainView : public MainViewBase
{
public:
    MainView();
    virtual ~MainView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    // Functions created as actions in Designer
    virtual void startFlowerAnimation();
    virtual void changeScreen(uint8_t value);


protected:
};

#endif // MAINVIEW_HPP
