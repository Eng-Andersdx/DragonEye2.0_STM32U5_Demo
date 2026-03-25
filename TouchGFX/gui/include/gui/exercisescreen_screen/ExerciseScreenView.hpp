#ifndef EXERCISESCREENVIEW_HPP
#define EXERCISESCREENVIEW_HPP

#include <gui_generated/exercisescreen_screen/ExerciseScreenViewBase.hpp>
#include <gui/exercisescreen_screen/ExerciseScreenPresenter.hpp>

#include <touchgfx/mixins/MoveAnimator.hpp>
#include <touchgfx/mixins/FadeAnimator.hpp>
#include <touchgfx/widgets/Image.hpp>
#include <touchgfx/widgets/TextArea.hpp>

class ExerciseScreenView : public ExerciseScreenViewBase
{
public:
    ExerciseScreenView();
    virtual ~ExerciseScreenView() {}

    virtual void setupScreen() override;
    virtual void tearDownScreen() override;
    virtual void handleTickEvent() override;

    // Hook rotary to these
    void goRight();
    void goLeft();

    virtual void Left();   // HW button 0
    virtual void Right();  // HW button 1
    virtual void H_Select() override;  //Select running or cycling or exercise

private:
    struct Item {
        touchgfx::BitmapId bmp;
        touchgfx::TypedText txt;
    };

    // icon lanes
    touchgfx::FadeAnimator< touchgfx::MoveAnimator< touchgfx::Image > > iconA;
    touchgfx::FadeAnimator< touchgfx::MoveAnimator< touchgfx::Image > > iconB;
    bool aIsFront = true;

    // animated label
    touchgfx::FadeAnimator< touchgfx::MoveAnimator< touchgfx::TextArea > > animLabel;

    // label target baseline Y
    int16_t labelBaseY = 0;

    static const int NUM_ITEMS = 3;
    Item items[NUM_ITEMS];
    int  currentIndex = 0;

    // layout / timing
    static const int SCREEN_W   = 480;
    static const int CENTER_X   = 240;
    static const int ICON_Y     = 200;
    static const int DURATION   = 14;
    static const int OFF        = 320;
    static const int LABEL_RISE = 16;
    static const int LABEL_DUR  = 12;

    // helpers
    void initItems();
    void animateInFromRight(const Item& item,
                            touchgfx::FadeAnimator< touchgfx::MoveAnimator< touchgfx::Image > >& inSlot,
                            touchgfx::FadeAnimator< touchgfx::MoveAnimator< touchgfx::Image > >& outSlot);
    void animateInFromLeft (const Item& item,
                            touchgfx::FadeAnimator< touchgfx::MoveAnimator< touchgfx::Image > >& inSlot,
                            touchgfx::FadeAnimator< touchgfx::MoveAnimator< touchgfx::Image > >& outSlot);
    void updateLabelTo(const Item& item);

    // width calculation + centered X
    int16_t textPixelWidth(const touchgfx::TypedText& tt) const;
    int16_t centeredXForWidth(int16_t w) const { return static_cast<int16_t>((SCREEN_W - w) / 2); }

    int wrap(int i) const { return (i + NUM_ITEMS) % NUM_ITEMS; }
};

#endif // EXERCISESCREENVIEW_HPP
