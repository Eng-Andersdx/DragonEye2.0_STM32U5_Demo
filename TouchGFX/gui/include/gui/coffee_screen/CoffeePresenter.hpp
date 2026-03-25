#ifndef COFFEEPRESENTER_HPP
#define COFFEEPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class CoffeeView;

class CoffeePresenter : public touchgfx::Presenter, public ModelListener
{
public:
    CoffeePresenter(CoffeeView& v);

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

    virtual ~CoffeePresenter() {}

private:
    CoffeePresenter();

    CoffeeView& view;
};

#endif // COFFEEPRESENTER_HPP
