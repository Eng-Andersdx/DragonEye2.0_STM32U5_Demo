#ifndef HW_INTERFACE_HPP
#define HW_INTERFACE_HPP

/*
 * Hardware interface for the GUI.
 *
 * GUI code includes this instead of "dragoneye_leds.h" so the same screens build
 * both for the board and for the TouchGFX simulator:
 *  - On target:    forwards to the real BSP (dragoneye_leds.h) and firmware globals.
 *  - In simulator: the BSP calls are stubs that print to the simulator log
 *                  (touchgfx_printf), and the firmware globals are defined in
 *                  HwInterface.cpp, so nothing from Core/ or the BSP is needed.
 * Same idea as knob_interface in the DEG0 project, but switched automatically by
 * the SIMULATOR define instead of by hand.
 */

#include <stdint.h>

#ifndef SIMULATOR

#include "dragoneye_leds.h"

#else // SIMULATOR

/* Colour codes (copy of dragoneye_leds.h - keep in sync) */
#define RGB_RED          (100)
#define RGB_GREEN        (100 << 8)
#define RGB_BLUE         (100 << 16)

#define RGB_YELLOW       (100 | (100 << 8))
#define RGB_CYAN         ((100 << 8) | (100 << 16))
#define RGB_MAGENTA      (100 | (100 << 16))

#define RGB_ORANGE   	(100 | (70 << 8))
#define RGB_BROWN   	(100 | (60 << 8) | (20 << 16))
#define RGB_SKY_BLUE   	(31 | (59 << 8) | (100 << 16))
#define RGB_DARK_ORANGE   (100 | (40 << 8) | (10 << 16))

#define RGB_DARK_RED     (50)
#define RGB_DARK_GREEN   (50 << 8)
#define RGB_DARK_BLUE    (50 << 16)

#define RGB_OLIVE        (50 | (50 << 8))
#define RGB_TEAL         ((50 << 8) | (50 << 16))
#define RGB_PURPLE       (50 | (50 << 16))

#define RGB_GREY         (25 | (25 << 8) | (25 << 16))
#define RGB_MEDIUM_RED   (75)
#define RGB_MEDIUM_GREEN (75 << 8)
#define RGB_MEDIUM_BLUE  (75 << 16)

#define RGB_LIGHT_YELLOW (75 | (75 << 8))
#define RGB_LIGHT_CYAN   ((75 << 8) | (75 << 16))
#define RGB_LIGHT_MAGENTA (75 | (75 << 16))

#define RGB_WHITE        (100 | (100 << 8) | (100 << 16))

#define RGB_COFFEE			(45 | (27 << 8) | (9 << 16))
#define RGB_OVEN 			(65 | (35 << 8) | (8 << 16))
/* End of RGB Colour Definitions */

#ifdef __cplusplus
extern "C" {
#endif
void BSP_LED_SetColor(uint32_t color);
void BSP_LED_SetRGBIntensity(uint8_t red_dutycycle, uint8_t green_dutycycle, uint8_t blue_dutycycle);
void BSP_BL_SetIntensity(uint8_t dutycycle);
#ifdef __cplusplus
}
#endif

#endif // SIMULATOR

#endif // HW_INTERFACE_HPP
