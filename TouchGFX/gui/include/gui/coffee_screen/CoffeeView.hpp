#ifndef COFFEEVIEW_HPP
#define COFFEEVIEW_HPP

#include <gui_generated/coffee_screen/CoffeeViewBase.hpp>
#include <touchgfx/widgets/AnimatedImage.hpp>              // AnimatedImage
#include <touchgfx/Bitmap.hpp>                              // BitmapId
#include <texts/TextKeysAndLanguages.hpp>                   // T_T_*

class CoffeeView : public CoffeeViewBase
{
public:
    // Keep the enum here to avoid missing symbols
    enum CoffeeType : uint8_t
    {
        ESPRESSO = 0,
        AMERICANO,
        CAPPUCCINO,
        LATTE,
        MOCHA,
        WHITE_COFFEE,
        HOT_WATER,
        COFFEE_COUNT
    };

    CoffeeView();
    virtual ~CoffeeView() {}

    virtual void setupScreen() override;
    virtual void tearDownScreen() override;
    virtual void handleTickEvent() override;

    // Wired by Designer (check your Interaction names)
    virtual void onNextClicked();
    virtual void onPrevClicked();
    virtual void tap_areaClicked();  // EXACT name from Designer

    // Hardware buttons
    void goLeft();    // HW button 0
    void goRight();   // HW button 1

protected:
    // Helpers
    void updateCoffeeDisplay(bool animate);
    void updateTitle(uint16_t textKey);
    void fadeTo(touchgfx::BitmapId bmp, int durationTicks);

    // Hint / pulse control
    void scheduleHint();
    void onTapAnimDone(const touchgfx::AnimatedImage& sender);
    void onTickAnimDone(const touchgfx::AnimatedImage& sender);

    inline int ticksFromMs(int ms) const { return (ms * fps) / 1000; }

private:
    // Selection state
    CoffeeType current = ESPRESSO;

    // Which background image is on top right now
    bool frontShowing = true;

    // Timing (assume 60Hz in sim)
    int fps = 60;

    // Hint delay, pulse gap and tick hold (in ticks)
    int hintDelayTicks = 0;
    bool hintWaiting   = false;
    int hintWaitTicks  = 0;

    int pulseGapTicks  = 0;
    bool pulseWaiting  = false;
    int pulseWaitTicks = 0;

    int  tickHoldTarget = 0;
    bool tickHoldActive = false;
    int  tickHoldTicks  = 0;

    // Callbacks from AnimatedImage widgets
    touchgfx::Callback<CoffeeView, const touchgfx::AnimatedImage&> tapAnimDoneCb;
    touchgfx::Callback<CoffeeView, const touchgfx::AnimatedImage&> tickAnimDoneCb;
};

#endif // COFFEEVIEW_HPP
