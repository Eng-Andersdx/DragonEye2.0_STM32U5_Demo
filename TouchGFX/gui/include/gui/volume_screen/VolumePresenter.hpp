#ifndef VOLUMEPRESENTER_HPP
#define VOLUMEPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class VolumeView;

class VolumePresenter : public touchgfx::Presenter, public ModelListener
{
public:
    VolumePresenter(VolumeView& v);

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

    virtual ~VolumePresenter() {}

private:
    VolumePresenter();

    VolumeView& view;
};

#endif // VOLUMEPRESENTER_HPP
