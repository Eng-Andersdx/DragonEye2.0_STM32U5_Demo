#ifndef SVGPRESENTER_HPP
#define SVGPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class SvgView;

class SvgPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    SvgPresenter(SvgView& v);

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

    virtual ~SvgPresenter() {};

private:
    SvgPresenter();

    SvgView& view;
};

#endif // SVGPRESENTER_HPP
