#ifndef EXERCISESCREENPRESENTER_HPP
#define EXERCISESCREENPRESENTER_HPP

#include <mvp/Presenter.hpp>
#include <gui/model/ModelListener.hpp>

class ExerciseScreenView;

class ExerciseScreenPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    ExerciseScreenPresenter(ExerciseScreenView& v);
    virtual void activate() override;
    virtual void deactivate() override;

private:
    ExerciseScreenPresenter();              // not implemented
    ExerciseScreenView& view;
};

#endif // EXERCISESCREENPRESENTER_HPP
