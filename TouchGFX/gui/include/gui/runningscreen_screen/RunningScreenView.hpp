#ifndef RUNNINGSCREENVIEW_HPP
#define RUNNINGSCREENVIEW_HPP

#include <gui_generated/runningscreen_screen/RunningScreenViewBase.hpp>
#include <gui/runningscreen_screen/RunningScreenPresenter.hpp>
#include <touchgfx/Unicode.hpp>

// This screen shows a running speed 0..10.
// The green progress arc is an AnimatedImage we drive by swapping bitmaps
// (we pre-rendered 79 PNGs). We animate by stepping frame-by-frame.

class RunningScreenView : public RunningScreenViewBase
{
public:
    RunningScreenView();
    virtual ~RunningScreenView() {}

    virtual void setupScreen() override;
    virtual void tearDownScreen() override;
    virtual void handleTickEvent() override;

    // Wire these in Designer
    void onPlusClicked();
    void onMinusClicked();

    // Set 0..10
    void setSpeed(uint8_t value);

    // Hardware buttons (0 = left, 1 = right)
    virtual void H_Left() override;   // HW button 0
    virtual void H_Right() override;  // HW button 1

private:
    // --- UI updates ---
    void updateDigits(uint8_t value);                  // updates "Speed_value"
    void requestBarFrameForSpeed(uint8_t speed);       // sets target frame for given speed

    // --- Frame animation state machine (runs in handleTickEvent) ---
    void startFrameAnim(uint8_t targetFrame);
    void stepFrameAnim();                               // advance one step if animating

    // --- Helpers ---
    uint8_t speedToFrame(uint8_t speed) const;          // maps 0..10 -> frame index (1..79)

    // Buffers / state
    touchgfx::Unicode::UnicodeChar numBuf[4]; // "10\0"
    uint8_t currentSpeed = 0;                  // 0..10

    // --- Arc animation state ---
    int16_t currentArcEnd = -100;   // what is currently shown
    int16_t arcStartEnd   = -100;   // start of the current animation
    int16_t targetArcEnd  = -100;   // where we want to go
    bool    arcAnimating  = false;
    uint8_t arcStepCount  = 0;
    uint8_t arcSteps      = 0;      // how many ticks the animation lasts

        // --- Bar step animation (discrete ticks) ---
    uint8_t displaySpeed   = 0;   // what the bar currently shows
    uint8_t targetSpeed    = 0;   // where we want the bar to end up
    bool    speedAnimating = false;
    uint8_t speedDelay     = 0;   // ticks until next bar step
    static const uint8_t SPEED_STEP_DELAY = 1;   // 0 = fastest, 1–2 = quick & visible

};

#endif // RUNNINGSCREENVIEW_HPP
