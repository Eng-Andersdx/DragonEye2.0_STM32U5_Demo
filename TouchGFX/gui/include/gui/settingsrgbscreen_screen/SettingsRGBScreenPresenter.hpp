#ifndef SETTINGSRGBSCREENPRESENTER_HPP
#define SETTINGSRGBSCREENPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class SettingsRGBScreenView;

class SettingsRGBScreenPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    SettingsRGBScreenPresenter(SettingsRGBScreenView& v);

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

    virtual ~SettingsRGBScreenPresenter() {}

private:
    SettingsRGBScreenPresenter();

    SettingsRGBScreenView& view;
};

#endif // SETTINGSRGBSCREENPRESENTER_HPP
