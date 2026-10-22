#ifndef SETTINGSHAPTICSCREENPRESENTER_HPP
#define SETTINGSHAPTICSCREENPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class SettingsHapticScreenView;

class SettingsHapticScreenPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    SettingsHapticScreenPresenter(SettingsHapticScreenView& v);

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

    virtual ~SettingsHapticScreenPresenter() {}

private:
    SettingsHapticScreenPresenter();

    SettingsHapticScreenView& view;
};

#endif // SETTINGSHAPTICSCREENPRESENTER_HPP
