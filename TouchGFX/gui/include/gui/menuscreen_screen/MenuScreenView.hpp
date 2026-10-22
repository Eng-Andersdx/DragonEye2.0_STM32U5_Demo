#ifndef MENUSCREENVIEW_HPP
#define MENUSCREENVIEW_HPP

#include <gui_generated/menuscreen_screen/MenuScreenViewBase.hpp>
#include <gui/menuscreen_screen/MenuScreenPresenter.hpp>

#include <gui/containers/MenuWheel.hpp>
#include <touchgfx/Callback.hpp>
#include <texts/TextKeysAndLanguages.hpp>
#include <BitmapDatabase.hpp>
#include <touchgfx/widgets/Image.hpp>

/**
 * MenuScreenView
 *
 * Hosts:
 *  - bgBack (Image)
 *  - bgFront (Image)
 *  - menu_title (TextArea)
 *  - menuWheel (MenuWheel container)
 *
 * When the wheel selection changes, we update bg and title.
 * When the selected icon is activated (tapped), we navigate.
 */
class MenuScreenView : public MenuScreenViewBase
{
public:
    MenuScreenView();
    virtual ~MenuScreenView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void handleTickEvent();

    // Callbacks from MenuWheel
    void onWheelSelectionChanged(int idx);
    void onWheelActivated(int idx);

    // *** Hardware rotary callbacks (override Designer virtuals) ***
    virtual void goLeft()  override;
    virtual void goRight() override;
    
    // HW button 3 press (configured in Designer as "H_Press")
    virtual void H_Press();   // or: virtual void H_Press() override; if the base declares it
    

protected:
    static const int WHEEL_ITEMS = 5;

    // Small wheel icons (around the circle)
    touchgfx::BitmapId iconIdsSmall[WHEEL_ITEMS] =
    {
        BITMAP_SPEAKER_ICON_MAIN_ID,   // Volume small
        BITMAP_SETTINGS_ICON_MAIN_ID,  // Settings small
        BITMAP_OVEN_ICON_MAIN_ID,      // Oven small
        BITMAP_COFFEE_ICON_MAIN_ID,    // Coffee small
        BITMAP_EXERCISE_ICON_MAIN_ID   // Exercise small
    };

    // Big center icons (your 100x100 PNGs)
    touchgfx::BitmapId iconIdsBig[WHEEL_ITEMS] =
    {
        BITMAP_SPEAKER_BIG_MAIN_ID,     // Volume 100x100
        BITMAP_SETTINGS_BIG_MAIN_ID,   // Settings 100x100
        BITMAP_OVEN_BIG_MAIN_ID,       // Oven 100x100
        BITMAP_COFFEE_BIG_MAIN_ID,     // Coffee 100x100
        BITMAP_EXERCISE_BIG_MAIN_ID    // Exercise 100x100
    };

    // Background images
    touchgfx::BitmapId bgIds[WHEEL_ITEMS] =
    {
        // Full-screen 480x480: radial glow + guide ring + halo/arc at the selected slot
        BITMAP_HOME_BG_VOLUME_ID,      // Volume background
        BITMAP_HOME_BG_SETTINGS_ID,    // Settings background
        BITMAP_HOME_BG_OVEN_ID,        // Oven background
        BITMAP_HOME_BG_COFFEE_ID,      // Coffee background
        BITMAP_HOME_BG_EXERCISE_ID     // Exercise background
    };

    // Accent colour per item (rings around the wheel icons); matches the backgrounds
    uint8_t accentRGB[WHEEL_ITEMS][3] =
    {
        {  38, 157, 208 },  // Volume   - blue
        { 220,  30,  30 },  // Settings - red
        { 222, 108,  30 },  // Oven     - orange
        { 170, 125,  80 },  // Coffee   - brown
        {  30, 190,  40 }   // Exercise - green
    };
    void applyAccent(int idx);

    // Canvas buffer for the ring widgets (Circle) in the wheel
    static const uint32_t CANVAS_BUFFER_SIZE = 7200;
    uint8_t canvasBuffer[CANVAS_BUFFER_SIZE];

    // Title text IDs
    uint16_t titleIds[WHEEL_ITEMS] =
    {
        T_T_MENU_VOLUME,
        T_T_MENU_SETTINGS,
        T_T_MENU_OVEN,
        T_T_MENU_COFFEE,
        T_T_MENU_EXERCISE
    };

    // Track current background for smooth cross-fade
    touchgfx::BitmapId currentBgId;
    bool hasBgId = false;

    // Wheel callbacks
    touchgfx::Callback<MenuScreenView, int> wheelSelectionChangedCb;
    touchgfx::Callback<MenuScreenView, int> wheelActivatedCb;
    touchgfx::Callback<MenuScreenView, int> wheelTargetChangedCb;
    int shownIndex = -1;   // item whose colour/title is currently shown

    // Helpers
    void setTitle(uint16_t textId, bool animate = true);
    void setBackground(touchgfx::BitmapId id, bool animate = true);
    void setCenterIcon(int idx, bool animate = true);

    // ---- Status icons (upper-left, following the round edge) ----
    touchgfx::Image wifiIcon;
    touchgfx::Image btIcon;
    void updateStatusBar(bool force = false);

};

#endif // MENUSCREENVIEW_HPP
