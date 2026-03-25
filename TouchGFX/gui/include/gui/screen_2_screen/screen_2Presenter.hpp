#ifndef SCREEN_2PRESENTER_HPP
#define SCREEN_2PRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class screen_2View;

class screen_2Presenter : public touchgfx::Presenter, public ModelListener
{
public:
    screen_2Presenter(screen_2View& v);

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

    virtual ~screen_2Presenter() {}

private:
    screen_2Presenter();

    screen_2View& view;
};

#endif // SCREEN_2PRESENTER_HPP
