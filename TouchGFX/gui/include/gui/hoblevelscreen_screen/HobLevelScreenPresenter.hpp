#ifndef HOBLEVELSCREENPRESENTER_HPP
#define HOBLEVELSCREENPRESENTER_HPP

#include <mvp/Presenter.hpp>
#include <cstdint>
#include <gui/model/ModelListener.hpp>
#include <gui/model/Model.hpp>

class HobLevelScreenView;

class HobLevelScreenPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    HobLevelScreenPresenter(HobLevelScreenView& v) : view(v) {}
    virtual ~HobLevelScreenPresenter() {}

    virtual void activate() override {}
    virtual void deactivate() override {}

    // Model accessors for the current pending hob
    Model::HobIndex getPendingHob() const            { return model->getPendingHob(); }
    uint8_t         getHobLevel(Model::HobIndex i)   const { return model->getHobLevel(i); }
    void            setHobLevel(Model::HobIndex i, uint8_t lvl) { model->setHobLevel(i, lvl); }

    // Navigation back to HobScreen
    void goBackToHobScreen();

private:
    HobLevelScreenView& view;
};

#endif // HOBLEVELSCREENPRESENTER_HPP
