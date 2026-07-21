#ifndef COMPASSPRESENTER_HPP
#define COMPASSPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class CompassView;

class CompassPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    CompassPresenter(CompassView& v);

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

    virtual ~CompassPresenter() {};

private:
    CompassPresenter();

    CompassView& view;
};

#endif // COMPASSPRESENTER_HPP
