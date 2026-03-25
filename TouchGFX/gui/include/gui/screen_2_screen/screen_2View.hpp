#ifndef SCREEN_2VIEW_HPP
#define SCREEN_2VIEW_HPP

#include <gui_generated/screen_2_screen/screen_2ViewBase.hpp>
#include <gui/screen_2_screen/screen_2Presenter.hpp>

class screen_2View : public screen_2ViewBase
{
public:
    screen_2View();
    virtual ~screen_2View() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
protected:
};

#endif // SCREEN_2VIEW_HPP
