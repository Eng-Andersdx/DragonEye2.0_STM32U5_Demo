#include <gui/containers/MenuWheel.hpp>

#include <touchgfx/EasingEquations.hpp>
#include <touchgfx/Utils.hpp>
#include <BitmapDatabase.hpp>
#include <touchgfx/Application.hpp>


#include <math.h>

using namespace touchgfx;

// Use TouchGFX' own PI to avoid ambiguity
static const float TWO_PI       = 2.0f * touchgfx::PI;

// Angle where we consider an item "selected".
// -touchgfx::PI / 2  = top of the circle (smaller Y)
static const float SELECT_ANGLE = -touchgfx::PI / 2.0f;

MenuWheel::MenuWheel()
    : centerX(0.f)
    , centerY(0.f)
    , radius(0.f)
    , angleStep(0.f)
    , centerIndex(0)
    , rotating(false)
    , dragged(false)
    , rotationBase(0.f)
    , startRotationBase(0.f)
    , moveDistance(0.f)
    , steps(0)
    , stepCount(0)
    , lastAngle(0.f)
    , oldAngle(0.f)
    , selectionChangedCb(0)
    , activatedCb(0)
{
    setTouchable(true);
}

void MenuWheel::initialize()
{
    MenuWheelBase::initialize();

    // Ensure handleTickEvent() is called every frame so animations
    // and snapping work.
    touchgfx::Application::getInstance()->registerTimerWidget(this);
}

void MenuWheel::configure(const touchgfx::BitmapId smallIconIds[NUM_ITEMS],
                          const touchgfx::BitmapId bigIconIds[NUM_ITEMS])
{
    // Remember the ids
    for (int i = 0; i < NUM_ITEMS; ++i)
    {
        smallIds[i] = smallIconIds[i];
        bigIds[i]   = bigIconIds[i];
    }

    // Starting selection: item 0 at top 
    centerIndex = 0;

    // Compute origin / radius 
    const int w = getWidth()+10;
    const int h = getHeight()+10;

    // Optional tiny nudge if you want to tweak alignment later
    const float offsetX = 0.0f;   // try +2 or -2 if you want to move horizontally
    const float offsetY = 0.0f;   // try +2 or -2 if you want to move vertically

    centerX = w * 0.5f + offsetX;
    centerY = h * 0.5f + offsetY;
    radius  = (w < h ? w : h) * 0.32f;   // your tuned radius
    angleStep = 2.0f * touchgfx::PI / NUM_ITEMS;

    // We want item 0 at SELECT_ANGLE (top) to start
    rotationBase = SELECT_ANGLE;

    for (int i = 0; i < NUM_ITEMS; ++i)
    {
        items[i].angle = touchgfx::PI + angleStep * i; // arrange around circle

        touchgfx::Bitmap bmp(smallIds[i]);             // start with SMALL icon

        items[i].icon.setBitmap(bmp);
        items[i].icon.setWidth(bmp.getWidth());
        items[i].icon.setHeight(bmp.getHeight());
        items[i].icon.setBitmapPosition(0.0f, 0.0f);

        const float cam = 1000.0f;
        items[i].icon.setCameraDistance(cam);
        items[i].icon.setOrigo(bmp.getWidth() * 0.5f,
                               bmp.getHeight() * 0.5f,
                               cam);

        items[i].icon.setScale(1.0f);
        items[i].icon.setRenderingAlgorithm(touchgfx::TextureMapper::BILINEAR_INTERPOLATION);

        items[i].icon.setAlpha(255);
        items[i].icon.setVisible(true);

        add(items[i].icon);
    }

    // Start with selected index exactly at SELECT_ANGLE (top)
    rotationBase = SELECT_ANGLE - angleStep * centerIndex;
    layout();

    // Initial alpha and bitmap: center = big+bright, others = small+dim
    for (int i = 0; i < NUM_ITEMS; ++i)
    {
    const touchgfx::Bitmap& bmp = (i == centerIndex)
        ? touchgfx::Bitmap(bigIds[i])
        : touchgfx::Bitmap(smallIds[i]);

    items[i].icon.setBitmap(bmp);
    items[i].icon.setWidth(bmp.getWidth());
    items[i].icon.setHeight(bmp.getHeight());
    items[i].icon.setAlpha(i == centerIndex ? 255 : 50);
    items[i].icon.invalidate();
    }

    // Sizes have changed (center icon is now big), so recompute positions
    layout();

}

void MenuWheel::setSelectionChangedCallback(GenericCallback<int>& cb)
{
    selectionChangedCb = &cb;
}

void MenuWheel::setActivatedCallback(GenericCallback<int>& cb)
{
    activatedCb = &cb;
}

// ---------- Helpers ----------

float MenuWheel::normalize(float a) const
{
    while (a >  touchgfx::PI)  a -= TWO_PI;
    while (a < -touchgfx::PI)  a += TWO_PI;
    return a;
}

void MenuWheel::layout()
{
    for (int i = 0; i < NUM_ITEMS; ++i)
    {
        float angle = rotationBase + angleStep * i;
        items[i].angle = angle;

        // Invalidate old position before moving
        items[i].icon.invalidate();

        // Decide scale based on whether this is the selected index
        float scale = (i == centerIndex) ? SCALE_SELECTED : SCALE_NORMAL;
        items[i].icon.setScale(scale);

        // Center of this icon along the circle
        float cx = centerX + cosf(angle) * radius;
        float cy = centerY + sinf(angle) * radius;

        int iw = items[i].icon.getWidth();
        int ih = items[i].icon.getHeight();

        // Position top-left so the scaled icon stays centered on (cx, cy)
        int ix = int(cx - iw * 0.5f);
        int iy = int(cy - ih * 0.5f);

        items[i].icon.setXY(ix, iy);
        items[i].icon.invalidate();
    }
}

float MenuWheel::angleFromPoint(int16_t x, int16_t y) const
{
    float dx = float(x) - centerX;
    float dy = float(y) - centerY;
    return atan2f(dy, dx);   // [-pi..pi], 0 at right
}

float MenuWheel::compensateToIconSteps(float rawMove) const
{
    // Convert raw angle move into a whole number of icon-steps
    float stepsFloat = rawMove / angleStep;
    int   stepsInt;

    if (stepsFloat >= 0.f)
        stepsInt = int(stepsFloat + 0.5f);
    else
        stepsInt = int(stepsFloat - 0.5f);

    if (stepsInt == 0)
        stepsInt = (rawMove >= 0.f) ? 1 : -1;

    return angleStep * float(stepsInt);
}

void MenuWheel::snapToNearest()
{
    // Find item whose angle is closest to SELECT_ANGLE
    int   bestIdx  = 0;
    float bestDist = 1000.f;

    for (int i = 0; i < NUM_ITEMS; ++i)
    {
        float angle = rotationBase + angleStep * i;
        float d     = normalize(angle - SELECT_ANGLE); // [-pi..pi]
        if (fabsf(d) < fabsf(bestDist))
        {
            bestDist = d;
            bestIdx  = i;
        }
    }

    // We want angle(bestIdx) == SELECT_ANGLE
    float currentAngleBest = rotationBase + angleStep * bestIdx;
    float desiredBase      = rotationBase + normalize(SELECT_ANGLE - currentAngleBest);
    moveDistance           = desiredBase - rotationBase;

    startRotationBase = rotationBase;
    stepCount         = 0;
    steps             = MAX(5, int(fabsf(moveDistance) / angleStep * 10.f)); // use MAX macro
    rotating          = true;
}

void MenuWheel::animateStep(int delta)
{
    if (delta == 0 || rotating)
        return;

    // Target index (wrap 0..NUM_ITEMS-1)
    int newIndex = centerIndex + delta;
    if (newIndex < 0)
        newIndex += NUM_ITEMS;
    if (newIndex >= NUM_ITEMS)
        newIndex -= NUM_ITEMS;

    // Use same math as when tapping a side icon
    float angleCurrent = rotationBase + angleStep * newIndex;
    float desiredBase  = rotationBase + normalize(SELECT_ANGLE - angleCurrent);
    moveDistance       = desiredBase - rotationBase;

    startRotationBase = rotationBase;
    stepCount         = 0;
    steps             = MAX(5, int(fabsf(moveDistance) / angleStep * 10.f));
    rotating          = true;
}

void MenuWheel::setSelectionAndNotify(int newIndex)
{
    if (newIndex < 0 || newIndex >= NUM_ITEMS)
        return;

    int oldIndex = centerIndex;
    centerIndex  = newIndex;

    // Swap bitmaps: old center gets small icon, new center gets big icon
    {
        touchgfx::Bitmap smallBmp(smallIds[oldIndex]);
        items[oldIndex].icon.setBitmap(smallBmp);
        items[oldIndex].icon.setWidth(smallBmp.getWidth());
        items[oldIndex].icon.setHeight(smallBmp.getHeight());
    }
    {
        touchgfx::Bitmap bigBmp(bigIds[newIndex]);
        items[newIndex].icon.setBitmap(bigBmp);
        items[newIndex].icon.setWidth(bigBmp.getWidth());
        items[newIndex].icon.setHeight(bigBmp.getHeight());
    }

    // Alpha: center bright, others dim
    for (int i = 0; i < NUM_ITEMS; ++i)
    {
        uint8_t a = (i == centerIndex) ? 255 : 50;
        items[i].icon.setAlpha(a);
        items[i].icon.invalidate();
    }

    // Notify view
    if (selectionChangedCb && selectionChangedCb->isValid() && oldIndex != newIndex)
    {
        selectionChangedCb->execute(centerIndex);
    }
    
    // Recompute positions now that some icons changed size
    layout();

}

// ---------- Events ----------

void MenuWheel::handleClickEvent(const ClickEvent& evt)
{
    if (evt.getType() == ClickEvent::PRESSED)
    {
        dragged   = false;
        rotating  = false;
        lastAngle = angleFromPoint(evt.getX(), evt.getY());
        oldAngle  = lastAngle;
    }
    else // RELEASED
    {
        if (dragged || rotating)
        {
            // If we dragged, just snap when we let go
            snapToNearest();
            return;
        }

        // Tap without dragging: either select or activate
        const int16_t x = evt.getX();
        const int16_t y = evt.getY();

        // Check if the tap is inside this rectangle
        if (x >= 130 && x <= 340 &&
            y >= 0  && y <= 310)
        {
            if (activatedCb && activatedCb->isValid())
            {
                activatedCb->execute(centerIndex);
            }
            return;  // done, no need to check other icons
        }

        for (int i = 0; i < NUM_ITEMS; ++i)
        {

        	Rect r = items[i].icon.getRect();
            // Increase hit area equally for all icons
            const int padding = 25;   // good for 480x480 screen
            r.x -= padding;
            r.y -= padding;
            r.width  += padding * 2;
            r.height += padding * 2;

            if (r.intersect(x, y))
            {
                if (i == centerIndex)
                {
                    // Tapped the selected icon -> activate
                    if (activatedCb && activatedCb->isValid())
                    {
                        activatedCb->execute(centerIndex);
                    }
                }
                else
                {
                    // Tapped a side icon -> rotate it to center
                    float angleCurrent = rotationBase + angleStep * i;
                    float desiredBase  = rotationBase + normalize(SELECT_ANGLE - angleCurrent);
                    moveDistance       = desiredBase - rotationBase;

                    startRotationBase = rotationBase;
                    stepCount         = 0;
                    steps             = MAX(5, int(fabsf(moveDistance) / angleStep * 10.f));
                    rotating          = true;
                }
                return;
            }
        }
    }
}

void MenuWheel::handleDragEvent(const DragEvent& evt)
{
    dragged = true;

    float newAngle = angleFromPoint(evt.getNewX(), evt.getNewY());
    float diff     = normalize(newAngle - lastAngle);

    rotationBase += diff;
    layout();

    oldAngle  = lastAngle;
    lastAngle = newAngle;
}

void MenuWheel::handleGestureEvent(const GestureEvent& evt)
{
    if (evt.getType() == GestureEvent::SWIPE_HORIZONTAL ||
        evt.getType() == GestureEvent::SWIPE_VERTICAL)
    {
        float vel = (float)touchgfx::abs((double)evt.getVelocity());
        if (vel < 1.f)
            return;

        // Direction: sign from last/old angle
        float dir = (lastAngle >= oldAngle) ? 1.f : -1.f;

        float rawMove   = (vel * 0.01f) * dir;   // tweak multiplier for feel
        moveDistance    = compensateToIconSteps(rawMove);

        startRotationBase = rotationBase;
        stepCount         = 0;

        // More velocity -> more steps (longer animation)
        steps = MAX(8, int(vel * 0.1f));
        rotating = true;
    }
}

void MenuWheel::rotateRight()
{
    // If an animation is already running, ignore this click
    if (rotating)
        return;

    // Move the wheel by one item step.
    // Sign of moveDistance decides direction. If this feels backwards
    // on the real hardware, just swap the sign here or swap which
    // function you call from goRight/goLeft.
    moveDistance = -angleStep;   // one step
    startRotationBase = rotationBase;
    stepCount = 0;
    steps = 10;                  // tweak for speed/feel if you like
    rotating = true;
}

void MenuWheel::rotateLeft()
{
    if (rotating)
        return;

    moveDistance = +angleStep;   // opposite direction of rotateRight
    startRotationBase = rotationBase;
    stepCount = 0;
    steps = 10;
    rotating = true;
}

void MenuWheel::handleTickEvent()
{
    if (!rotating)
        return;

    if (steps <= 0)
        steps = 1;

    float t = (float)stepCount / (float)steps;
    if (t > 1.f) t = 1.f;

    // cubicEaseOut over [0..1]
    float eased = EasingEquations::cubicEaseOut(t, 0.f, 1.f, 1.f);

    rotationBase = startRotationBase + moveDistance * eased;
    layout();

    stepCount++;
    if (stepCount > steps)
    {
        rotating      = false;
        rotationBase  = startRotationBase + moveDistance;

        // Determine new center
        int   bestIdx  = 0;
        float bestDist = 1000.f;
        for (int i = 0; i < NUM_ITEMS; ++i)
        {
            float angle = rotationBase + angleStep * i;
            float d     = fabsf(normalize(angle - SELECT_ANGLE));
            if (d < bestDist)
            {
                bestDist = d;
                bestIdx  = i;
            }
        }
        setSelectionAndNotify(bestIdx);

        // *** Hard-align selected item exactly at SELECT_ANGLE ***
        rotationBase = SELECT_ANGLE - angleStep * bestIdx;
        layout();
    }
}
