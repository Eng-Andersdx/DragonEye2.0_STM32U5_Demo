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
 * @file touchgfx/widgets/DeltaCompressedAnimation.hpp
 *
 * Declares the touchgfx::DeltaCompressedAnimation class.
 */
#ifndef TOUCHGFX_DELTACOMPRESSEDANIMATION_HPP
#define TOUCHGFX_DELTACOMPRESSEDANIMATION_HPP

#include <touchgfx/Callback.hpp>
#include <touchgfx/hal/HAL.hpp>
#include <touchgfx/hal/Types.hpp>
#include <touchgfx/widgets/Widget.hpp>

#include <stddef.h>

namespace touchgfx
{
/**
 * @brief Widget that displays frames from a TouchGFX Delta Compressed Animation
 * resource.
 *
 * For animation data that is not directly addressable, a caller-owned header
 * buffer must be configured with setHeaderBuffer() if the animation uses L8 or L8_RLE.
 * The resource and configured header buffer must remain valid for
 * as long as the widget uses them.
 */
class DeltaCompressedAnimation : public touchgfx::Widget
{
public:
    /**
     * @brief Constructs an empty animation widget with no animation assigned.
     */
    DeltaCompressedAnimation();

    /**
     * @brief Assigns an animation resource and loads its current frame.
     *
     * The widget dimensions are updated from the resource header. The resource
     * is not copied and must remain valid while the animation is in use.
     *
     * @param animation Pointer to the start of a valid animation resource.
     */
    void setAnimation(const void* animation);

    /**
     * @brief Starts advancing the animation on timer ticks.
     *
     * @param loop If true, restart at the first frame after the final frame.
     */
    void startAnimation(bool loop = false);

    /**
     * @brief Stops advancing the animation and unregisters its timer.
     */
    void stopAnimation();

    /**
     * @brief Associates an action to be performed when the animation reaches its final frame.
     *
     * If the animation is set to loop, the action is also triggered at the end of each loop.
     *
     * @param callback The callback is executed when the animation is done and receives this widget.
     */
    void setDoneAction(touchgfx::GenericCallback<const DeltaCompressedAnimation&>& callback)
    {
        animationDoneAction = &callback;
    }

    /**
     * @brief Sets whether the animation restarts after its final frame.
     *
     * @param loop If true, restart at the first frame after the final frame.
     */
    void setLoopAnimation(bool loop)
    {
        loopAnimation = loop;
    }

    /**
     * @brief Sets the number of timer ticks between frame changes.
     *
     * @param updateInterval Number of ticks between frame changes.
     */
    void setUpdateTicksInterval(uint8_t updateInterval);

    /**
     * @brief Advances the animation when the configured tick interval elapses.
     *
     * The widget must be started with startAnimation() to receive timer ticks.
     * At the final frame, playback either stops or wraps to the first frame
     * depending on the loop setting.
     */
    virtual void handleTickEvent();

    /**
     * @brief Sets the working buffer used for non-addressable L8 and L8-RLE bitmap data.
     *
     * The buffer is not owned by the widget and must remain valid while
     * non-addressable animations are drawn.
     * Addressable animation data does not require this buffer.
     *
     * @param buffer Pointer to writable, 32-bit-aligned memory.
     * @param size   Buffer capacity in bytes.
     */
    void setHeaderBuffer(uint32_t* buffer, size_t size)
    {
        headerBuffer = buffer;
        headerBufferSizeWords = (size) / sizeof(uint32_t);
    }

    /**
     * @brief Advances to the next frame and invalidates its rectangular parts.
     *
     * The frame index wraps to the first frame after the final frame.
     *
     * @return @c false if no animation is assigned or the newly selected frame is the final frame;
     *         otherwise @c true.
     */
    bool nextFrame();

    /**
     * @brief Returns the zero-based index of the current frame.
     *
     * @return The current frame index.
     */
    int getFrameNumber() const
    {
        return frameNumber;
    }

    virtual void draw(const Rect& invalidatedArea) const;
    virtual Rect getSolidRect() const
    {
        return isOpaque ? Rect(0, 0, rect.width, rect.height) : Rect();
    }

private:
    enum
    {
        DATA_RECT_ENTRY_SIZE = 12
    };

    /**
     * @brief Describes one rectangular part within a frame.
     */
    struct RectTableEntry
    {
        uint32_t dataOffset; ///< 24-bit byte offset from the frame start to the part data.
        uint8_t format;      ///< AnimationPixelFormat of the part data.
        Rect rect;           ///< Position and dimensions in widget coordinates.
    };

    /**
     * @brief Invalidates every rectangular part of the current frame.
     */
    void invalidateCurrentFrame();

    /**
     * @brief Draws the portion of a frame part that intersects an invalidated area.
     *
     * @param frameBaseAddr   Address of the frame data that @p rte belongs to.
     * @param rte             Rectangular-part descriptor to draw.
     * @param invalidatedArea Invalidated area in widget coordinates.
     */
    void drawPart(const void* frameBaseAddr, const RectTableEntry& rte, const Rect& invalidatedArea) const;

    /**
     * @brief Removes a part's overlap from an area, if the overlap crosses only one side.
     *
     * A "simple" overlap touches three sides of @p drawArea, leaving a single
     * rectangle uncovered on the remaining side. Overlaps touching fewer than
     * three sides, or all four (full containment), are not simple.
     *
     * @param drawArea  Area to shrink.
     * @param partRect  Part rectangle to test against @p drawArea.
     * @param remainder Set to @p drawArea with the overlap removed, if the overlap is simple.
     * @return @c true if the overlap is simple and @p remainder was set.
     */
    static bool removeSimpleOverlap(const Rect& drawArea, const Rect& partRect, Rect& remainder);

    /**
     * @brief Draws a raw RGB565, RGB888, or ARGB8888 frame part.
     *
     * @param blitRect     Area of the frame part to draw, in widget coordinates.
     * @param pixelRect    Bounds of the complete frame part, in widget coordinates.
     * @param pixels       Pointer to the raw pixel data.
     * @param bitmapFormat Pixel format of @p pixels; ARGB8888 carries per-pixel alpha.
     */
    void drawRGB(const Rect& blitRect, const Rect& pixelRect, const uint8_t* pixels, touchgfx::Bitmap::BitmapFormat bitmapFormat) const;

    /**
     * @brief Draws a palette-based L8 frame part.
     *
     * @param blitRect      Area of the frame part to draw, in widget coordinates.
     * @param pixelRect     Bounds of the complete frame part, in widget coordinates.
     * @param data          Pointer to the color table header and L8 pixel data.
     * @param clutFormat    Format of the color table entries.
     * @param bytesPerColor Number of bytes per color table entry.
     */
    void drawL8(const Rect& blitRect, const Rect& pixelRect, const uint8_t* data, touchgfx::Bitmap::ClutFormat clutFormat, int bytesPerColor) const;

    /**
     * @brief Draws a run-length encoded, palette-based L8 frame part.
     *
     * @param blitRect      Area of the frame part to draw, in widget coordinates.
     * @param pixelRect     Bounds of the complete frame part, in widget coordinates.
     * @param data          Pointer to the block count, block offsets, color table, and encoded pixel data.
     * @param clutFormat    Format of the color table entries.
     * @param bytesPerColor Number of bytes per color table entry.
     */
    void drawL8RLE(const Rect& blitRect, const Rect& pixelRect, const uint8_t* data, touchgfx::Bitmap::ClutFormat clutFormat, int bytesPerColor) const;

    /**
     * @brief Draws a compressed RGB frame part.
     *
     * @param blitRect     Area of the frame part to draw, in widget coordinates.
     * @param pixelRect    Bounds of the complete frame part, in widget coordinates.
     * @param data         Pointer to the compressed pixel data.
     * @param bitmapFormat Pixel format of @p data: COMPRESSED_RGB565, COMPRESSED_RGB888, or COMPRESSED_ARGB8888.
     */
    void drawCompressedRGB(const Rect& blitRect, const Rect& pixelRect, const uint8_t* data, touchgfx::Bitmap::BitmapFormat bitmapFormat) const;

    /**
     * @brief Fills a frame part with an ARGB color.
     *
     * @param blitRect Area of the frame part to draw, in widget coordinates.
     * @param data     Pointer to the ARGB fill color.
     */
    void drawColorARGB(const Rect& blitRect, const uint8_t* data) const;

    /**
     * @brief Loads the current frame header and rectangular-part table.
     */
    void readFrameData();

    /**
     * @brief Copies data from the animation resource.
     *
     * @param dst   Destination buffer.
     * @param src   Source address in the animation resource.
     * @param bytes Number of bytes to copy.
     */
    void copyData(void* dst, const void* src, uint32_t bytes) const;

    /// FrameTypes.
    enum FrameType
    {
        FRAME_TYPE_INITIAL = 0,
        FRAME_TYPE_DELTA = 1,
        FRAME_TYPE_KEYFRAME = 2
    };

    /**
     * @brief Base address and rectangle tables of an arbitrary frame.
     */
    struct FrameInfo
    {
        const void* frameAddr;           ///< Address of the frame's FrameType field.
        uint8_t numberOfChangedRects;    ///< U8 count of rectangles used for invalidation.
        uint8_t numberOfDataRects;       ///< U8 count of rectangles containing frame data.
        FrameType frameFormat;           ///< FrameType value decoded from the frame header.
        Rect changedRectTable[4];        ///< Changed areas; used only to invalidate the widget.
        RectTableEntry dataRectTable[4]; ///< Data areas and offsets; used to draw the frame.
    };

    /**
     * @brief Reads the header and rectangular-part table of an arbitrary frame.
     *
     * Unlike readFrameData(), this does not alter widget state, so it is safe
     * to call from const methods to look up frames other than the current one.
     *
     * @param frame Zero-based index of the frame to read.
     * @return The frame's base address, rectangle counts, and rectangle tables.
     */
    FrameInfo readFrameInfo(int frame) const;

    /**
     * @brief Reads a 32-bit value from an animation resource.
     *
     * @param address Address of the value to read.
     * @return The value copied from the resource.
     */
    uint32_t readU32(const void* address) const
    {
        uint32_t data;
        copyData(&data, address, 4);
        return data;
    }

    int frameNumber;                                                                 ///< Zero-based index of the current frame.
    touchgfx::GenericCallback<const DeltaCompressedAnimation&>* animationDoneAction; ///< Pointer to the callback to be executed when animation is done.
    int numberOfFrames;                                                              ///< Number of frames in the animation resource. Zero means no animation.
    bool isOpaque;                                                                   ///< True if all pixels in the animation are fully opaque.
    int frameWidth;                                                                  ///< Width of the animation frames.
    int frameHeight;                                                                 ///< Height of the animation frames.
    int lastKeyFrame;                                                                ///< Index of the most recent INITIAL or KEYFRAME frame.
    uint8_t updateTicksInterval;                                                     ///< Number of timer ticks between frame changes.
    uint8_t ticksSinceUpdate;                                                        ///< Number of timer ticks since the last frame change.
    bool loopAnimation;                                                              ///< If true, restart at the first frame after the final frame.
    bool running;                                                                    ///< If true, the widget is registered for timer ticks.
    const void* frameOffsetsAddr;                                                    ///< Address of the frame-offset table.
    bool dataIsAddressable;                                                          ///< True if animation data can be read directly by the MCU.
    FrameInfo currentFrame;                                                          ///< Header and changed/data rectangle tables of the current frame.

    uint32_t* headerBuffer;    ///< Working storage for non-addressable L8 and L8-RLE bitmap data.
    int headerBufferSizeWords; ///< Header-buffer capacity in 32-bit words.
};

} // namespace touchgfx

#endif // TOUCHGFX_DELTACOMPRESSEDANIMATION_HPP
