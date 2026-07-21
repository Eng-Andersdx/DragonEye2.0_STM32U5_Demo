#ifndef THERMOSTATPRESENTER_HPP
#define THERMOSTATPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class ThermostatView;

class ThermostatPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    ThermostatPresenter(ThermostatView& v);

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

    virtual ~ThermostatPresenter() {};


private:
    ThermostatPresenter();

    ThermostatView& view;
};

#endif // THERMOSTATPRESENTER_HPP
