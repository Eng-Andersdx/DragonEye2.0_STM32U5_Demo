#ifndef RUNNINGSCREENPRESENTER_HPP
#define RUNNINGSCREENPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class RunningScreenView;

class RunningScreenPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    RunningScreenPresenter(RunningScreenView& v);

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

    virtual ~RunningScreenPresenter() {}

private:
    RunningScreenPresenter();

    RunningScreenView& view;
};

#endif // RUNNINGSCREENPRESENTER_HPP
