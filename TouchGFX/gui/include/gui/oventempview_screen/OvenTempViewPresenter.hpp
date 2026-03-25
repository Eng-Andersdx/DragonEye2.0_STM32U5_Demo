#ifndef OVENTEMPVIEWPRESENTER_HPP
#define OVENTEMPVIEWPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class OvenTempViewView;

class OvenTempViewPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    OvenTempViewPresenter(OvenTempViewView& v);

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

    virtual ~OvenTempViewPresenter() {}

private:
    OvenTempViewPresenter();

    OvenTempViewView& view;
};

#endif // OVENTEMPVIEWPRESENTER_HPP
