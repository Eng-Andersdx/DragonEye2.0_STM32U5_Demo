#ifndef MENUWHEEL_HPP
#define MENUWHEEL_HPP

#include <gui_generated/containers/MenuWheelBase.hpp>
#include <touchgfx/widgets/Image.hpp>
#include <touchgfx/mixins/FadeAnimator.hpp>
#include <touchgfx/events/ClickEvent.hpp>
#include <touchgfx/events/DragEvent.hpp>
#include <touchgfx/events/GestureEvent.hpp>
#include <touchgfx/Callback.hpp>
#include <touchgfx/widgets/TextureMapper.hpp>
#include <touchgfx/widgets/canvas/Circle.hpp>
#include <touchgfx/widgets/canvas/PainterRGB888.hpp>
#include <touchgfx/hal/Types.hpp>


class MenuWheel : public MenuWheelBase
{
public:
    static const int NUM_ITEMS = 5;   // Volume, Hob, Oven, Coffee, Exercise

    MenuWheel();
    virtual ~MenuWheel() {}

    virtual void initialize();

    // Called from MenuScreenView::setupScreen()
   void configure(const touchgfx::BitmapId smallIconIds[NUM_ITEMS],
               const touchgfx::BitmapId bigIconIds[NUM_ITEMS]);

    // Query current selected index (0..NUM_ITEMS-1)
    int getSelectedIndex() const { return centerIndex; }

    // Animate the wheel by +1 or -1 items (used by hardware buttons)
    void animateStep(int delta);  // delta = +1 (right/next), -1 (left/prev)

    // Colour of the rings around the icons (follows the selected item)
    void setAccentColor(touchgfx::colortype color);

    // Enable/disable touch
    void setEnabled(bool en) { setTouchable(en); }

    // Callbacks into the view
    void setSelectionChangedCallback(touchgfx::GenericCallback<int>& cb);
    void setActivatedCallback(touchgfx::GenericCallback<int>& cb);
    // Called as soon as the wheel knows where it is heading (start of a step / during a drag),
    // so the view can start the colour change without waiting for the rotation to finish
    void setTargetChangedCallback(touchgfx::GenericCallback<int>& cb);

    // Events
    virtual void handleClickEvent(const touchgfx::ClickEvent& evt);
    virtual void handleDragEvent(const touchgfx::DragEvent& evt);
    virtual void handleGestureEvent(const touchgfx::GestureEvent& evt);
    virtual void handleTickEvent();

    // Called from MenuScreenView for hardware bezel
    void rotateRight();   // one step in one direction
    void rotateLeft();    // one step in the opposite direction


private:
    struct Item
    {
        float angle;   // current angle on circle (radians)
        touchgfx::FadeAnimator<touchgfx::TextureMapper> icon;
    };

    Item items[NUM_ITEMS];

    // Accent rings drawn behind each icon (thick ring on the selected one)
    touchgfx::Circle rings[NUM_ITEMS];
    touchgfx::PainterRGB888 ringPainter;

    // Arrays to remember which bitmap to use for each state
    touchgfx::BitmapId smallIds[NUM_ITEMS];
    touchgfx::BitmapId bigIds[NUM_ITEMS];

    // You can keep these, but set both to 1.0 (we're not using scaling anymore)
    static constexpr float SCALE_SELECTED = 1.0f;
    static constexpr float SCALE_NORMAL   = 0.7f;	//1.0f

    // Geometry
    float centerX;
    float centerY;
    float radius;
    float angleStep;           // 2*pi / NUM_ITEMS

    // Wheel state
    int   centerIndex;         // which index is currently at bottom (selected)

    // Animation / drag state
    bool  rotating;
    bool  dragged;

    float rotationBase;        // base angle for item 0
    float startRotationBase;   // base angle at start of animation
    float moveDistance;        // how much base rotates during animation

    int   steps;
    int   stepCount;

    float lastAngle;           // last finger angle
    float oldAngle;

    // Callbacks into the view
    touchgfx::GenericCallback<int>* selectionChangedCb;
    touchgfx::GenericCallback<int>* activatedCb;
    touchgfx::GenericCallback<int>* targetChangedCb;
    int   lastTargetIndex;

    // Helpers
    void  layout();                         // position all icons from rotationBase
    float angleFromPoint(int16_t x, int16_t y) const;
    float normalize(float a) const;         // [-pi..pi]
    void  snapToNearest();                  // start animation to nearest icon
    void  setSelectionAndNotify(int idx);
    float compensateToIconSteps(float rawMove) const; // round move to full steps
    int   indexForBase(float base) const;      // item that sits at the top for this base angle
    void  notifyTarget(float targetBase);

};

#endif // MENUWHEEL_HPP
