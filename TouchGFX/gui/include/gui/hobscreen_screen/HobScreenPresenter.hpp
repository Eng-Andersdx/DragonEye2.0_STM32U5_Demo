#ifndef HOBSCREENPRESENTER_HPP
#define HOBSCREENPRESENTER_HPP

#include <mvp/Presenter.hpp>
#include <gui/model/ModelListener.hpp>
#include <gui/model/Model.hpp>        // for Model::HobIndex
#include <cstdint>

// Forward declare the view so we can have a ctor that takes it
class HobScreenView;

class HobScreenPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    // TouchGFX constructs the presenter with a reference to the concrete view
    explicit HobScreenPresenter(HobScreenView& v) : view(v) {}

    virtual void activate() override {}
    virtual void deactivate() override {}

    // Pass-throughs to Model (guard against null)
    void            setPendingHob(Model::HobIndex i)            { if (model) model->setPendingHob(i); }
    Model::HobIndex getPendingHob() const                       { return model ? model->getPendingHob() : Model::M; }

    uint8_t getHobLevel(Model::HobIndex i) const                { return model ? model->getHobLevel(i) : 0; }
    void    setHobLevel(Model::HobIndex i, uint8_t l)           { if (model) model->setHobLevel(i, l); }

    // Navigation to the level screen
    void goToHobLevelScreen();

private:
    HobScreenView& view;
};

#endif // HOBSCREENPRESENTER_HPP
