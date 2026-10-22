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

#include <touchgfx/Application.hpp>
#include <touchgfx/Bitmap.hpp>
#include <touchgfx/Color.hpp>
#include <touchgfx/hal/FlashDataReader.hpp>
#include <touchgfx/widgets/DeltaCompressedAnimation.hpp>

#include <string.h>

namespace touchgfx
{
DeltaCompressedAnimation::DeltaCompressedAnimation()
    : frameNumber(0), animationDoneAction(0), numberOfFrames(0), isOpaque(false), frameWidth(0), frameHeight(0), lastKeyFrame(0), updateTicksInterval(1), ticksSinceUpdate(0),
      loopAnimation(false), running(false), frameOffsetsAddr(0), dataIsAddressable(false), currentFrame(), headerBuffer(NULL), headerBufferSizeWords(0)
{
}

void DeltaCompressedAnimation::copyData(void* dst, const void* src, uint32_t bytes) const
{
    if (dataIsAddressable)
    {
        memcpy(dst, src, bytes);
    }
    else
    {
        HAL::getInstance()->blockCopy(dst, src, bytes);
    }
}

void DeltaCompressedAnimation::setAnimation(const void* animation)
{
    if (animation == 0)
    {
        numberOfFrames = 0;
        isOpaque = false;
        dataIsAddressable = false;
        currentFrame.numberOfChangedRects = 0;
        currentFrame.numberOfDataRects = 0;
        return;
    }

    animation = Bitmap::addOffset(static_cast<const uint8_t*>(animation));

    FlashDataReader* flashReader = HAL::getInstance()->getFlashDataReader();
    dataIsAddressable = flashReader == 0 || flashReader->addressIsAddressable(animation);

    uint32_t header[2];
    copyData(header, animation, 8);

    const uint16_t* h16 = reinterpret_cast<const uint16_t*>(header);
    frameWidth = h16[0];
    frameHeight = h16[1];
    setWidth(frameWidth);
    setHeight(frameHeight);

    numberOfFrames = h16[2];
    const uint8_t* h8 = reinterpret_cast<const uint8_t*>(header);
    isOpaque = h8[6] != 0;
    frameOffsetsAddr = static_cast<const uint8_t*>(animation) + 8;

    readFrameData();

    // Invalidate the whole widget to show whole first frame
    invalidate();
}

void DeltaCompressedAnimation::startAnimation(bool loop)
{
    if (numberOfFrames == 0)
    {
        return;
    }

    loopAnimation = loop;
    ticksSinceUpdate = 0;
    Application::getInstance()->registerTimerWidget(this);
    running = true;
}

void DeltaCompressedAnimation::stopAnimation()
{
    if (running)
    {
        Application::getInstance()->unregisterTimerWidget(this);
        running = false;
    }
}

void DeltaCompressedAnimation::handleTickEvent()
{
    if (!running)
    {
        return;
    }

    ++ticksSinceUpdate;
    if (ticksSinceUpdate != updateTicksInterval)
    {
        return;
    }

    ticksSinceUpdate = 0;
    const bool frameIsNotFinal = nextFrame();
    if (!frameIsNotFinal && !loopAnimation)
    {
        stopAnimation();
    }

    if (!frameIsNotFinal && animationDoneAction && animationDoneAction->isValid())
    {
        animationDoneAction->execute(*this);
    }
}

void DeltaCompressedAnimation::setUpdateTicksInterval(uint8_t updateInterval)
{
    updateTicksInterval = updateInterval;
    ticksSinceUpdate = 0;
}

bool DeltaCompressedAnimation::nextFrame()
{
    if (numberOfFrames == 0)
    {
        return false; // No animation
    }

    frameNumber++;
    if (frameNumber == numberOfFrames)
    {
        frameNumber = 0;
    }

    readFrameData();
    invalidateCurrentFrame();

    if ((frameNumber + 1) == numberOfFrames)
    {
        return false;
    }
    return true;
}

void DeltaCompressedAnimation::readFrameData()
{
    assert(frameOffsetsAddr != 0);

    currentFrame = readFrameInfo(frameNumber);

    if (currentFrame.frameFormat == FRAME_TYPE_INITIAL || currentFrame.frameFormat == FRAME_TYPE_KEYFRAME)
    {
        lastKeyFrame = frameNumber;
    }
}

void DeltaCompressedAnimation::invalidateCurrentFrame()
{
    for (int i = 0; i < currentFrame.numberOfChangedRects; i++)
    {
        invalidateRect(currentFrame.changedRectTable[i]);
    }
}

void DeltaCompressedAnimation::draw(const Rect& invalidatedArea) const
{
    if (!isOpaque && currentFrame.frameFormat == FRAME_TYPE_DELTA)
    {
        const FrameInfo keyFrame = readFrameInfo(lastKeyFrame);
        const RectTableEntry& keyFrameRect = keyFrame.dataRectTable[0]; // INITIAL/KEYFRAME always have exactly 1 data rect

        const Rect dataRect = currentFrame.numberOfDataRects > 0
                                  ? currentFrame.dataRectTable[0].rect
                                  : Rect();
        const Rect overlap = invalidatedArea & dataRect;

        // No overlap with this frame, just draw the key frame and return.
        if (overlap.isEmpty())
        {
            drawPart(keyFrame.frameAddr, keyFrameRect, invalidatedArea);
            return;
        }

        if (overlap.y > invalidatedArea.y)
        {
            // Draw the strip above the overlap.
            drawPart(keyFrame.frameAddr, keyFrameRect, Rect(invalidatedArea.x, invalidatedArea.y, invalidatedArea.width, overlap.y - invalidatedArea.y));
        }
        if (overlap.y + overlap.height < invalidatedArea.y + invalidatedArea.height)
        {
            // Draw the strip below the overlap.
            drawPart(keyFrame.frameAddr, keyFrameRect, Rect(invalidatedArea.x, overlap.y + overlap.height, invalidatedArea.width, invalidatedArea.y + invalidatedArea.height - (overlap.y + overlap.height)));
        }
        if (overlap.x > invalidatedArea.x)
        {
            // Draw the strip to the left of the overlap.
            drawPart(keyFrame.frameAddr, keyFrameRect, Rect(invalidatedArea.x, overlap.y, overlap.x - invalidatedArea.x, overlap.height));
        }
        if (overlap.x + overlap.width < invalidatedArea.x + invalidatedArea.width)
        {
            // Draw the strip to the right of the overlap.
            drawPart(keyFrame.frameAddr, keyFrameRect, Rect(overlap.x + overlap.width, overlap.y, invalidatedArea.x + invalidatedArea.width - (overlap.x + overlap.width), overlap.height));
        }

        // Draw the overlap with current frame.
        drawPart(currentFrame.frameAddr, currentFrame.dataRectTable[0], overlap);
        return;
    }

    // Check this frame's parts.
    // Search for parts that fully contains the invalidated area or where the invalidated area has a simple overlap.
    Rect drawArea = invalidatedArea;
    bool replayAreaReduced = true;
    while (replayAreaReduced && !drawArea.isEmpty())
    {
        replayAreaReduced = false;
        for (int i = 0; i < currentFrame.numberOfDataRects; i++)
        {
            const RectTableEntry& rte = currentFrame.dataRectTable[i];
            if (rte.rect.intersect(drawArea)) // is this part relevant
            {
                // If we can draw the whole area, we are done.
                if (rte.rect.includes(drawArea))
                {
                    drawPart(currentFrame.frameAddr, rte, drawArea);
                    return;
                }

                // If we can draw a part of the area because of simple overlap,
                // draw that and restart the search with the remaining area.
                Rect remainder;
                if (removeSimpleOverlap(drawArea, rte.rect, remainder))
                {
                    drawPart(currentFrame.frameAddr, rte, drawArea);
                    drawArea = remainder;
                    replayAreaReduced = true;
                    break; // parts are unordered; restart from the first part against the smaller area
                }
            }
        }
    }

    // If we reach this point, the invalidated area was not fully covered by the current frame's parts.
    assert(currentFrame.frameFormat != FRAME_TYPE_INITIAL && currentFrame.frameFormat != FRAME_TYPE_KEYFRAME);

    // The current frame did not cover the complete area. Replay from the last
    // key frame through the current frame using each frame's data rectangles.
    for (int frame = lastKeyFrame; frame <= frameNumber && !drawArea.isEmpty(); frame++)
    {
        const FrameInfo info = readFrameInfo(frame);
        for (int i = 0; i < info.numberOfDataRects; i++)
        {
            const RectTableEntry& rte = info.dataRectTable[i];
            const Rect& pixelRect = rte.rect;
            Rect invalidatedPixels = pixelRect & drawArea;
            if (!invalidatedPixels.isEmpty())
            {
                drawPart(info.frameAddr, rte, drawArea);
            }
        }
    }
}

bool DeltaCompressedAnimation::removeSimpleOverlap(const Rect& drawArea, const Rect& partRect, Rect& remainder)
{
    // A simple overlap crosses only one side of drawArea.
    // E.g. partRect (x'es) covering the
    // top strip and leaving the bottom strip as remainder:
    //
    // xxxxxxxxxxxxxxxxxxxxx <- partRect
    // xxxxxxxxxxxxxxxxxxxxx
    // x+-----------------+x
    // x|xxxxxxxxxxxxxxxxx|x
    // x|xxxxxxxxxxxxxxxxx|x
    //  +-----------------+
    //  |                 | <- remainder
    //  |                 |
    //  +-----------------+ <- drawArea
    // The intersection is the portion that can be drawn from this part.
    const Rect overlap = drawArea & partRect;
    if (overlap.isEmpty())
    {
        return false;
    }

    // A simple overlap spans one complete dimension and touches exactly one
    // edge in the other dimension. This leaves one rectangular remainder.
    const bool fullWidth = (overlap.x == drawArea.x) && (overlap.width == drawArea.width);
    const bool fullHeight = (overlap.y == drawArea.y) && (overlap.height == drawArea.height);

    if (fullWidth && !fullHeight)
    {
        const bool topAligned = (overlap.y == drawArea.y);
        const bool bottomAligned = (overlap.y + overlap.height == drawArea.y + drawArea.height);
        if (topAligned != bottomAligned)
        {
            // Remove a top or bottom strip from the draw area.
            remainder = topAligned
                            ? Rect(drawArea.x, overlap.y + overlap.height, drawArea.width, drawArea.y + drawArea.height - (overlap.y + overlap.height))
                            : Rect(drawArea.x, drawArea.y, drawArea.width, overlap.y - drawArea.y);
            return true;
        }
    }
    else if (fullHeight && !fullWidth)
    {
        const bool leftAligned = (overlap.x == drawArea.x);
        const bool rightAligned = (overlap.x + overlap.width == drawArea.x + drawArea.width);
        if (leftAligned != rightAligned)
        {
            // Remove a left or right strip from the draw area.
            remainder = leftAligned
                            ? Rect(overlap.x + overlap.width, drawArea.y, drawArea.x + drawArea.width - (overlap.x + overlap.width), drawArea.height)
                            : Rect(drawArea.x, drawArea.y, overlap.x - drawArea.x, drawArea.height);
            return true;
        }
    }

    return false;
}

DeltaCompressedAnimation::FrameInfo DeltaCompressedAnimation::readFrameInfo(int frame) const
{
    FrameInfo info;
    // Frame offsets are relative to the first byte of the frames area.
    const uint8_t* const frameOffsets = static_cast<const uint8_t*>(frameOffsetsAddr);
    const uint8_t* const framesBaseAddr = frameOffsets + numberOfFrames * 4;
    const uint32_t frameOffset = readU32(frameOffsets + frame * 4);
    info.frameAddr = framesBaseAddr + frameOffset;

    // The frame header contains FrameType followed by the two 8-bit counts.
    uint8_t frameHeader[4];
    copyData(frameHeader, info.frameAddr, sizeof(frameHeader));
    info.frameFormat = static_cast<FrameType>(frameHeader[0] | (static_cast<uint16_t>(frameHeader[1]) << 8));
    info.numberOfChangedRects = frameHeader[2];
    info.numberOfDataRects = frameHeader[3];

    // The INITIAL and KEYFRAME formats always have exactly one data rectangle.
    assert((info.frameFormat != FRAME_TYPE_INITIAL && info.frameFormat != FRAME_TYPE_KEYFRAME) || info.numberOfDataRects == 1);
    // The DELTA format has at most one data rectangle if the animation is opaque.
    assert(isOpaque || info.numberOfDataRects <= 1);

    // The changed and data tables are contiguous in the resource. Read them
    // together to avoid an additional flash address transaction.
    const uint32_t changedRectsSize = sizeof(Rect) * info.numberOfChangedRects;
    const uint32_t dataRectsSize = DATA_RECT_ENTRY_SIZE * info.numberOfDataRects;
    if (changedRectsSize + dataRectsSize > 0)
    {
        uint8_t rectTables[sizeof(Rect) * 4 + DATA_RECT_ENTRY_SIZE * 4];
        copyData(rectTables, static_cast<const uint8_t*>(info.frameAddr) + 4, changedRectsSize + dataRectsSize);
        memcpy(info.changedRectTable, rectTables, changedRectsSize);
        for (int i = 0; i < info.numberOfDataRects; i++)
        {
            const uint8_t* const entry = rectTables + changedRectsSize + i * DATA_RECT_ENTRY_SIZE;
            info.dataRectTable[i].dataOffset = static_cast<uint32_t>(entry[0]) |
                                               (static_cast<uint32_t>(entry[1]) << 8) |
                                               (static_cast<uint32_t>(entry[2]) << 16);
            info.dataRectTable[i].format = entry[3];
            uint8_t* rectData = reinterpret_cast<uint8_t*>(&info.dataRectTable[i].rect);
            memcpy(rectData, entry + 4, sizeof(Rect));
        }
    }

    return info;
}

void DeltaCompressedAnimation::drawPart(const void* frameBaseAddr, const RectTableEntry& rte, const Rect& invalidatedArea) const
{
    const uint8_t* const frameBase = static_cast<const uint8_t*>(frameBaseAddr);
    const uint8_t* const bitmap = frameBase + rte.dataOffset;
    Rect pixelRect = rte.rect;
    Rect invalidatedPixels = pixelRect & invalidatedArea;
    if (!invalidatedPixels.isEmpty())
    {
        switch (rte.format)
        {
        case 0: // RGB565
            drawRGB(invalidatedPixels, pixelRect, bitmap, Bitmap::RGB565);
            break;
        case 1: // RGB888
            drawRGB(invalidatedPixels, pixelRect, bitmap, Bitmap::RGB888);
            break;
        case 2: // ARGB8888
            drawRGB(invalidatedPixels, pixelRect, bitmap, Bitmap::ARGB8888);
            break;
        case 3: // L8_RGB565
            drawL8(invalidatedPixels, pixelRect, bitmap, touchgfx::Bitmap::CLUT_FORMAT_L8_RGB565, 2);
            break;
        case 4: // L8_RGB888
            drawL8(invalidatedPixels, pixelRect, bitmap, touchgfx::Bitmap::CLUT_FORMAT_L8_RGB888, 3);
            break;
        case 5: // L8_ARGB8888
            drawL8(invalidatedPixels, pixelRect, bitmap, touchgfx::Bitmap::CLUT_FORMAT_L8_ARGB8888, 4);
            break;
        case 6: // L8_RLE_RGB565
            drawL8RLE(invalidatedPixels, pixelRect, bitmap, touchgfx::Bitmap::CLUT_FORMAT_L8_RGB565, 2);
            break;
        case 7: // L8_RLE_RGB888
            drawL8RLE(invalidatedPixels, pixelRect, bitmap, touchgfx::Bitmap::CLUT_FORMAT_L8_RGB888, 3);
            break;
        case 8: // L8_RLE_ARGB8888
            drawL8RLE(invalidatedPixels, pixelRect, bitmap, touchgfx::Bitmap::CLUT_FORMAT_L8_ARGB8888, 4);
            break;
        case 9: // COMP_RGB565
            drawCompressedRGB(invalidatedPixels, pixelRect, bitmap, touchgfx::Bitmap::COMPRESSED_RGB565);
            break;
        case 10: // COMP_RGB888
            drawCompressedRGB(invalidatedPixels, pixelRect, bitmap, touchgfx::Bitmap::COMPRESSED_RGB888);
            break;
        case 11: // COMP_ARGB8888
            drawCompressedRGB(invalidatedPixels, pixelRect, bitmap, touchgfx::Bitmap::COMPRESSED_ARGB8888);
            break;
        case 12: // COLOR_ARGB
            drawColorARGB(invalidatedPixels, bitmap);
            break;
        default:
            break;
        }
    }
}

void DeltaCompressedAnimation::drawRGB(const Rect& blitRect, const Rect& pixelRect, const uint8_t* pixels, touchgfx::Bitmap::BitmapFormat bitmapFormat) const
{
    // Convert Pixel rect to LCD coordinates.
    Rect absRect = pixelRect;
    translateRectToAbsolute(absRect);
    // Move invalidatedArea from widget coordiantes to pixelRect coordinates
    Rect drawRect = blitRect;
    drawRect.x -= pixelRect.x;
    drawRect.y -= pixelRect.y;

    // ARGB8888 carries per-pixel alpha, so it is not fully solid; RGB565/RGB888 have no alpha channel.
    const Rect solidRect = (bitmapFormat == Bitmap::ARGB8888) ? Rect(0, 0, 0, 0) : Rect(0, 0, pixelRect.width, pixelRect.height);
    const LCD::RawBitmapData rawBitmap = { pixels, 0, solidRect, static_cast<uint16_t>(pixelRect.width), static_cast<uint16_t>(pixelRect.height), bitmapFormat };
    HAL::lcd().drawPartialBitmap(rawBitmap, absRect.x, absRect.y, drawRect, 255);
}

void DeltaCompressedAnimation::drawL8(const Rect& blitRect, const Rect& pixelRect, const uint8_t* data, touchgfx::Bitmap::ClutFormat clutFormat, int bytesPerColor) const
{
    const int numColors = static_cast<int>((readU32(data) >> 16) & 0xFFFF);
    const uint8_t* pixels = data + 4;
    const uint8_t* extraData = data;
    if (!dataIsAddressable)
    {
        if (headerBuffer == 0)
        {
            return;
        }
        // Copy the CLUT to RAM to make it addressable as required in LCD
        assert(headerBufferSizeWords >= 1 + (numColors * bytesPerColor + 3) / 4 && "Header buffer too small for L8 image");
        copyData(headerBuffer, data, 4 + numColors * bytesPerColor);
        extraData = reinterpret_cast<const uint8_t*>(headerBuffer);
    }

    pixels += numColors * bytesPerColor; // Move to after CLUT

    // Convert Pixel rect to LCD coordinates.
    Rect absRect = pixelRect;
    translateRectToAbsolute(absRect);
    // Move invalidatedArea from widget coordiantes to pixelRect coordinates
    Rect drawRect = blitRect;
    drawRect.x -= pixelRect.x;
    drawRect.y -= pixelRect.y;

    // ARGB8888 CLUT entries carry per-pixel alpha, so the image is not fully solid.
    const Rect solidRect = (clutFormat == touchgfx::Bitmap::CLUT_FORMAT_L8_ARGB8888) ? Rect(0, 0, 0, 0) : Rect(0, 0, pixelRect.width, pixelRect.height);

    const LCD::RawBitmapData rawBitmap = { pixels, extraData, solidRect, static_cast<uint16_t>(pixelRect.width), static_cast<uint16_t>(pixelRect.height), Bitmap::L8 };
    HAL::lcd().drawPartialBitmap(rawBitmap, absRect.x, absRect.y, drawRect, 255);
}

void DeltaCompressedAnimation::drawL8RLE(const Rect& blitRect, const Rect& pixelRect, const uint8_t* data, touchgfx::Bitmap::ClutFormat clutFormat, int bytesPerColor) const
{
    const int numBlock = static_cast<int>(readU32(data));
    data += 4;
    const int numColors = static_cast<int>((readU32(data) >> 16) & 0xFFFF);
    const uint8_t* const pixels = data + 4 + numBlock * 4 + numColors * bytesPerColor;
    const uint8_t* extraData = data;
    if (!dataIsAddressable)
    {
        if (headerBuffer == 0)
        {
            return;
        }
        // Copy the CLUT and blockOffsets to RAM to make it addressable as required in LCD
        assert(headerBufferSizeWords >= 1 + (numColors * bytesPerColor + 3) / 4 + numBlock && "Header buffer too small for L8RLE image");
        copyData(headerBuffer, data, 4 + numBlock * 4 + numColors * bytesPerColor);
        extraData = reinterpret_cast<const uint8_t*>(headerBuffer);
    }

    // Convert Pixel rect to LCD coordinates.
    Rect absRect = pixelRect;
    translateRectToAbsolute(absRect);
    // Move invalidatedArea from widget coordiantes to pixelRect coordinates
    Rect drawRect = blitRect;
    drawRect.x -= pixelRect.x;
    drawRect.y -= pixelRect.y;

    // ARGB8888 CLUT entries carry per-pixel alpha, so the image is not fully solid.
    const Rect solidRect = (clutFormat == touchgfx::Bitmap::CLUT_FORMAT_L8_ARGB8888) ? Rect(0, 0, 0, 0) : Rect(0, 0, pixelRect.width, pixelRect.height);
    const LCD::RawBitmapData rawBitmap = { pixels, extraData, solidRect, static_cast<uint16_t>(pixelRect.width), static_cast<uint16_t>(pixelRect.height), Bitmap::L8 };
    HAL::lcd().drawPartialBitmap(rawBitmap, absRect.x, absRect.y, drawRect, 255);
}

void DeltaCompressedAnimation::drawCompressedRGB(const Rect& blitRect, const Rect& pixelRect, const uint8_t* data, touchgfx::Bitmap::BitmapFormat bitmapFormat) const
{
    // Convert Pixel rect to LCD coordinates.
    Rect absRect = pixelRect;
    translateRectToAbsolute(absRect);
    // Move invalidatedArea from widget coordiantes to pixelRect coordinates
    Rect drawRect = blitRect;
    drawRect.x -= pixelRect.x;
    drawRect.y -= pixelRect.y;

    // ARGB8888 carries per-pixel alpha, so the image is not fully solid.
    const Rect solidRect = (bitmapFormat == touchgfx::Bitmap::COMPRESSED_ARGB8888) ? Rect(0, 0, 0, 0) : Rect(0, 0, pixelRect.width, pixelRect.height);
    const LCD::RawBitmapData rawBitmap = { data, 0, solidRect, static_cast<uint16_t>(pixelRect.width), static_cast<uint16_t>(pixelRect.height), bitmapFormat };
    HAL::lcd().drawPartialBitmap(rawBitmap, absRect.x, absRect.y, drawRect, 255);
}

void DeltaCompressedAnimation::drawColorARGB(const Rect& blitRect, const uint8_t* data) const
{
    // Convert Pixel rect to LCD coordinates.
    Rect absRect = blitRect;
    translateRectToAbsolute(absRect);
    colortype color;
    copyData(&color, data, sizeof(colortype));
    const uint8_t alpha = color >> 24;
    HAL::lcd().fillRect(absRect, color, alpha); // Fill absrect with solid color
}

} // namespace touchgfx
