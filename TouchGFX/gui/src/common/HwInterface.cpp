#include <gui/common/HwInterface.hpp>

#ifdef SIMULATOR
/*
 * Simulator-only stand-ins for the board's BSP functions and firmware globals.
 * On target this file compiles to nothing; the real ones live in
 * dragoneye_leds.c and Core/Src/app_freertos.c.
 */
#include <touchgfx/Utils.hpp>

/* Globals normally defined in dragoneye_leds.c / app_freertos.c */
int LED_mode = 1;

extern "C"
{
    volatile uint8_t haptic_feedback           = 0;
    volatile uint8_t haptic_feedback_intensity = 35;
    volatile uint8_t haptic_continuous         = 0;
    volatile uint8_t status_wifi_connected     = 1;
    volatile uint8_t status_bt_connected       = 1;

    void BSP_LED_SetRGBIntensity(uint8_t red, uint8_t green, uint8_t blue)
    {
        touchgfx_printf("LED ring -> R=%d G=%d B=%d (%%)\r\n", red, green, blue);
    }

    void BSP_LED_SetColor(uint32_t color)
    {
        BSP_LED_SetRGBIntensity((uint8_t)(color & 0xFF),
                                (uint8_t)((color >> 8) & 0xFF),
                                (uint8_t)((color >> 16) & 0xFF));
    }

    void BSP_BL_SetIntensity(uint8_t dutycycle)
    {
        touchgfx_printf("Backlight -> %d%%\r\n", dutycycle);
    }
}

#endif // SIMULATOR
