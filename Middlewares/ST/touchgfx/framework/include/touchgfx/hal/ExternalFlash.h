/******************************************************************************
* Copyright (c) 2018(-2026) STMicroelectronics.
* All rights reserved.
*
* This file is part of the TouchGFX 4.27.0 distribution.
*
* This software is licensed under terms that can be found in the LICENSE file in
* the root directory of this software component.
* If no LICENSE file comes with this software, it is provided AS-IS.
*
*******************************************************************************/

/**
 * @file touchgfx/hal/ExternalFlash.h
 *
 * Declares functions for setting an offset used when reading flash
 * data for images, text, and fonts.
 * This is a "C" interface because it is expected to be called from
 * main.c or app_touchgfx.c.
 */
#ifndef TOUCHGFX_EXTERNALFLASH_H
#define TOUCHGFX_EXTERNALFLASH_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Sets the offset added to image data addresses that lie at or above the images flash
 * start address when reading image data from external flash.
 *
 * @param  address The start address of external flash.
 * @param  offset The offset, in bytes, to add to image data addresses in the external flash.
 *
 * @see touchgfx_setImagesFlashStartAddress
 */
void touchgfx_setImagesFlashOffset(uint32_t address, uint32_t offset);

/**
 * Sets the offset added to font data addresses when reading font data from external flash.
 *
 * @param  offset The offset, in bytes, to add to font data addresses.
 */
void touchgfx_setFontFlashOffset(uint32_t offset);

/**
 * Sets the offset added to text data addresses when reading text data from external flash.
 *
 * @param  offset The offset, in bytes, to add to text data addresses.
 */
void touchgfx_setTextFlashOffset(uint32_t offset);

#ifdef __cplusplus
}
#endif

#endif // TOUCHGFX_EXTERNALFLASH_H
