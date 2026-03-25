#ifndef SCREEN_1PRESENTER_HPP
#define SCREEN_1PRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class screen_1View;

class screen_1Presenter : public touchgfx::Presenter, public ModelListener
{
public:
    screen_1Presenter(screen_1View& v);

    /**
     * The activate function is called automatically when this screen is "switched in"
     * (ie. made active). Initialization logic can be placed here.
     */
    virtual void activate();

    /**
     * The deactivate function is called automatically when this screen is "switched out"
     * (ie. made inactive). Teardown functionality can be placed here.
     */
    virtual void deactivate();

    virtual ~screen_1Presenter() {}

private:
    screen_1Presenter();

    screen_1View& view;
};

#endif // SCREEN_1PRESENTER_HPP
