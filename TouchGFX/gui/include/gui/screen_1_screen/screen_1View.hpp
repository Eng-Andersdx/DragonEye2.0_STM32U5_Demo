#ifndef SCREEN_1VIEW_HPP
#define SCREEN_1VIEW_HPP

#include <gui_generated/screen_1_screen/screen_1ViewBase.hpp>
#include <gui/screen_1_screen/screen_1Presenter.hpp>

class screen_1View : public screen_1ViewBase
{
public:
    screen_1View();
    virtual ~screen_1View() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
protected:
};

#endif // SCREEN_1VIEW_HPP
