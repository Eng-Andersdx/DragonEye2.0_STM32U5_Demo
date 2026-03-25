#include <gui/widgets/TextArc.hpp>
#include <touchgfx/transforms/DisplayTransformation.hpp>
#include <touchgfx/Math3D.hpp>
#include <touchgfx/TextureMapTypes.hpp>
#include <touchgfx/hal/HAL.hpp>
#include <touchgfx/FontManager.hpp>
#include <fonts/ApplicationFontProvider.hpp>

namespace touchgfx
{
TextArc::TextArc() :
    Widget(),
    currentRenderingAlgorithm(NEAREST_NEIGHBOR),
    alpha(255),
    zOrigo(1000.0f),
    xCamera(0.0f),
    yCamera(0.0f),
    cameraDistance(1000.0f),
    curveGenerator(0),
    positionOffset(0),
    characterSpacing(0.f),
    typedText(TYPED_TEXT_INVALID),
    wildcard1(0),
    wildcard2(0),
    color(0)
{
}

TextArc::~TextArc()
{
}

void TextArc::applyCharacterTransformation(const BitmapTransformation& bt, BitmapCorners& bc) const
{
    const uint8_t n = 4;

    const int imgWidth = bt.character_width;
    const int imgHeight = bt.character_height;

    touchgfx::Point4 vertices[n] =
    {
        touchgfx::Point4(bt.xBitmapPosition,            bt.yBitmapPosition,             cameraDistance),
        touchgfx::Point4(bt.xBitmapPosition + imgWidth, bt.yBitmapPosition,             cameraDistance),
        touchgfx::Point4(bt.xBitmapPosition + imgWidth, bt.yBitmapPosition + imgHeight, cameraDistance),
        touchgfx::Point4(bt.xBitmapPosition,            bt.yBitmapPosition + imgHeight, cameraDistance),
    };
    touchgfx::Point4 transformed[n];

    Vector4 center(bt.xOrigo, bt.yOrigo, zOrigo);

    Matrix4x4 translateToCenter;
    translateToCenter.concatenateXTranslation(-center.getX()).concatenateYTranslation(-center.getY()).concatenateZTranslation(-center.getZ());

    Matrix4x4 rotateAroundCenter;
    rotateAroundCenter.concatenateZRotation(bt.zAngle);

    Matrix4x4 translateFromCenter;
    translateFromCenter.concatenateXTranslation(center.getX()).concatenateYTranslation(center.getY()).concatenateZTranslation(center.getZ());

    Matrix4x4 transform = translateFromCenter * rotateAroundCenter * translateToCenter;

    Matrix4x4 translateToCamera;
    translateToCamera.concatenateXTranslation(-xCamera);
    translateToCamera.concatenateYTranslation(-yCamera);

    Matrix4x4 perspectiveProject;
    perspectiveProject.setViewDistance(cameraDistance);

    Matrix4x4 translateFromCamera;
    translateFromCamera.concatenateXTranslation(xCamera).concatenateYTranslation(yCamera);

    transform = translateFromCamera * perspectiveProject * translateToCamera * transform;

    for (int i = 0; i < n; i++)
    {
        transformed[i] = transform * vertices[i];
    }

    bc.imageX0 = ((float)transformed[0].getX() * cameraDistance / (float)transformed[0].getZ());
    bc.imageY0 = ((float)transformed[0].getY() * cameraDistance / (float)transformed[0].getZ());
    bc.imageZ0 = ((float)transformed[0].getZ());

    bc.imageX1 = ((float)transformed[1].getX() * cameraDistance / (float)transformed[1].getZ());
    bc.imageY1 = ((float)transformed[1].getY() * cameraDistance / (float)transformed[1].getZ());
    bc.imageZ1 = ((float)transformed[1].getZ());

    bc.imageX2 = ((float)transformed[2].getX() * cameraDistance / (float)transformed[2].getZ());
    bc.imageY2 = ((float)transformed[2].getY() * cameraDistance / (float)transformed[2].getZ());
    bc.imageZ2 = ((float)transformed[2].getZ());

    bc.imageX3 = ((float)transformed[3].getX() * cameraDistance / (float)transformed[3].getZ());
    bc.imageY3 = ((float)transformed[3].getY() * cameraDistance / (float)transformed[3].getZ());
    bc.imageZ3 = ((float)transformed[3].getZ());
}

void TextArc::drawOneCharacter(const Rect& invalidatedArea, uint16_t* const fb, const BitmapCorners& bc, const uint8_t* pixels, const uint32_t width, const uint32_t height) const
{
    // Setup texture coordinates
    float right = (float)(width);
    float bottom = (float)(height);
    float textureU0 = 0.0f;
    float textureV0 = 0.0f;
    float textureU1 = right;
    float textureV1 = 0.0f;
    float textureU2 = right;
    float textureV2 = bottom;
    float textureU3 = 0.0f;
    float textureV3 = bottom;
    if (HAL::DISPLAY_ROTATION == rotate90)
    {
        textureU0 = 0.0f;
        textureV0 = right;
        textureU1 = 0.0f;
        textureV1 = 0.0f;
        textureU2 = bottom;
        textureV2 = 0.0f;
        textureU3 = bottom;
        textureV3 = right;
    }

    float triangleXs[3];
    float triangleYs[3];
    float triangleZs[3];
    float triangleUs[3];
    float triangleVs[3];

    // Determine winding order
    Vector4 zeroToOne(bc.imageX1 - bc.imageX0, bc.imageY1 - bc.imageY0, bc.imageZ1 - bc.imageZ0);
    Vector4 zeroToTwo(bc.imageX2 - bc.imageX0, bc.imageY2 - bc.imageY0, bc.imageZ2 - bc.imageZ0);
    Vector4 normal = zeroToOne.crossProduct(zeroToTwo);

    if (normal.getZ() > 0)
    {
        triangleXs[0] = bc.imageX0;
        triangleXs[1] = bc.imageX1;
        triangleXs[2] = bc.imageX2;
        triangleYs[0] = bc.imageY0;
        triangleYs[1] = bc.imageY1;
        triangleYs[2] = bc.imageY2;
        triangleZs[0] = bc.imageZ0;
        triangleZs[1] = bc.imageZ1;
        triangleZs[2] = bc.imageZ2;

        triangleUs[0] = textureU0;
        triangleUs[1] = textureU1;
        triangleUs[2] = textureU2;
        triangleVs[0] = textureV0;
        triangleVs[1] = textureV1;
        triangleVs[2] = textureV2;
    }
    else
    {
        // invert due to the triangles winding order (showing backface of the triangle)
        triangleXs[1] = bc.imageX0;
        triangleXs[0] = bc.imageX1;
        triangleXs[2] = bc.imageX2;
        triangleYs[1] = bc.imageY0;
        triangleYs[0] = bc.imageY1;
        triangleYs[2] = bc.imageY2;
        triangleZs[1] = bc.imageZ0;
        triangleZs[0] = bc.imageZ1;
        triangleZs[2] = bc.imageZ2;

        triangleUs[1] = textureU0;
        triangleUs[0] = textureU1;
        triangleUs[2] = textureU2;
        triangleVs[1] = textureV0;
        triangleVs[0] = textureV1;
        triangleVs[2] = textureV2;
    }

    drawTriangle(invalidatedArea, fb, pixels, width, height, triangleXs, triangleYs, triangleZs, triangleUs, triangleVs);

    if (normal.getZ() > 0)
    {
        triangleXs[0] = bc.imageX0;
        triangleXs[1] = bc.imageX2;
        triangleXs[2] = bc.imageX3;
        triangleYs[0] = bc.imageY0;
        triangleYs[1] = bc.imageY2;
        triangleYs[2] = bc.imageY3;
        triangleZs[0] = bc.imageZ0;
        triangleZs[1] = bc.imageZ2;
        triangleZs[2] = bc.imageZ3;

        triangleUs[0] = textureU0;
        triangleUs[1] = textureU2;
        triangleUs[2] = textureU3;
        triangleVs[0] = textureV0;
        triangleVs[1] = textureV2;
        triangleVs[2] = textureV3;
    }
    else
    {
        triangleXs[1] = bc.imageX0;
        triangleXs[0] = bc.imageX2;
        triangleXs[2] = bc.imageX3;
        triangleYs[1] = bc.imageY0;
        triangleYs[0] = bc.imageY2;
        triangleYs[2] = bc.imageY3;
        triangleZs[1] = bc.imageZ0;
        triangleZs[0] = bc.imageZ2;
        triangleZs[2] = bc.imageZ3;

        triangleUs[1] = textureU0;
        triangleUs[0] = textureU2;
        triangleUs[2] = textureU3;
        triangleVs[1] = textureV0;
        triangleVs[0] = textureV2;
        triangleVs[2] = textureV3;
    }

    drawTriangle(invalidatedArea, fb, pixels, width, height, triangleXs, triangleYs, triangleZs, triangleUs, triangleVs);
}

void TextArc::draw(const Rect& invalidatedArea) const
{
    if (!alpha || !typedText.hasValidId() || !curveGenerator)
    {
        return;
    }
    drawInternal(invalidatedArea, 0, wildcard1, wildcard2);
}

void TextArc::drawInternal(const Rect& invalidatedArea, int dummy, ...) const
{
    //setup start transformation
    BitmapTransformation bt;
    bt.xBitmapPosition = 0;
    bt.yBitmapPosition = 0;
    bt.zAngle = 0;

    const Font* font = typedText.getFont();
    const TextDirection direction = typedText.getTextDirection();

    TextProvider textProvider;
    va_list pArg;
    va_start(pArg, dummy);
    textProvider.initialize(typedText.getText(), pArg, font->getGSUBTable());

    //text color
    HAL::lcd().setDefaultColor(color);

    //bitmap coordinates for each character
    BitmapCorners bc;

    uint16_t* fb = HAL::getInstance()->lockFrameBuffer();

    int16_t pixelPos = positionOffset;
    uint16_t prevChar = 0;

    //scan text
    for (;;)
    {
        uint16_t ligature = textProvider.getNextLigature(direction);
        //terminate string
        if (ligature == 0)
        {
            break;
        }

        //find glyph node
        uint8_t bpp;
        const uint8_t* pixels;
        const GlyphNode* glyph = font->getGlyph(ligature, pixels, bpp);

        //skip unknown characters
        if (glyph == 0)
        {
            continue;
        }

        const uint32_t character_width = glyph->width();
        const uint32_t character_height = glyph->height();
        const int8_t kerning = font->getKerning(prevChar, glyph);

        //calculate position for glyph
        float x = 0;
        float y = 0;
        float angleZ = 0.f;
        curveGenerator->getCharacterPosition(pixelPos + kerning + glyph->advance() / 2, x, y, angleZ);


        //transform and draw
        bt.character_width = character_width;
        bt.character_height = character_height;
        bt.xBitmapPosition = x + glyph->left - character_width / 2;
        bt.yBitmapPosition = y - glyph->top();
        bt.zAngle = angleZ;
        bt.xOrigo = x;
        bt.yOrigo = y;
        applyCharacterTransformation(bt, bc);
        drawOneCharacter(invalidatedArea, fb, bc, pixels, character_width, character_height);

        prevChar = ligature;

        pixelPos += (int16_t)characterSpacing;
        pixelPos += kerning;
        pixelPos += glyph->advance();
    }

    HAL::getInstance()->unlockFrameBuffer();
}

void TextArc::drawTriangle(const Rect& invalidatedArea, uint16_t* fb, const uint8_t* pixels, const int32_t width, const int32_t height, const float* triangleXs, const float* triangleYs, const float* triangleZs, const float* triangleUs, const float* triangleVs) const
{
    // Area to redraw. Relative to the TextArc.
    Rect dirtyArea = Rect(0, 0, getWidth(), getHeight()) & invalidatedArea;

    // Absolute position of the TextArc.
    Rect dirtyAreaAbsolute = dirtyArea;
    translateRectToAbsolute(dirtyAreaAbsolute);

    Rect absoluteRect = getAbsoluteRect();
    DisplayTransformation::transformDisplayToFrameBuffer(absoluteRect);

    // Transform rects to match framebuffer coordinates
    // This is needed if the display is rotated compared to the framebuffer
    DisplayTransformation::transformDisplayToFrameBuffer(dirtyArea, this->getRect());
    DisplayTransformation::transformDisplayToFrameBuffer(dirtyAreaAbsolute);

    float x0 = triangleXs[0];
    float x1 = triangleXs[1];
    float x2 = triangleXs[2];
    float y0 = triangleYs[0]; //lint !e578
    float y1 = triangleYs[1]; //lint !e578
    float y2 = triangleYs[2];

    DisplayTransformation::transformDisplayToFrameBuffer(x0, y0, this->getRect());
    DisplayTransformation::transformDisplayToFrameBuffer(x1, y1, this->getRect());
    DisplayTransformation::transformDisplayToFrameBuffer(x2, y2, this->getRect());

    Point3D vertices[3];
    Point3D point0 = { floatToFixed28_4(x0), floatToFixed28_4(y0), (float)(triangleZs[0]), (float)(triangleUs[0]), (float)(triangleVs[0]) };
    Point3D point1 = { floatToFixed28_4(x1), floatToFixed28_4(y1), (float)(triangleZs[1]), (float)(triangleUs[1]), (float)(triangleVs[1]) };
    Point3D point2 = { floatToFixed28_4(x2), floatToFixed28_4(y2), (float)(triangleZs[2]), (float)(triangleUs[2]), (float)(triangleVs[2]) };

    vertices[0] = point0;
    vertices[1] = point1;
    vertices[2] = point2;

    DrawingSurface dest = { fb, HAL::FRAME_BUFFER_WIDTH };
    TextureSurface src = { (const uint16_t*)pixels, 0, width, height, (width + 1) & ~0x1 };

    HAL::lcd().drawTextureMapTriangle(dest, vertices, src, absoluteRect, dirtyAreaAbsolute, lookupRenderVariant(), alpha, 0xFFFF);
}

RenderingVariant TextArc::lookupRenderVariant() const
{
    //TODO introduce A4 in bitmap format someway
    if (currentRenderingAlgorithm == NEAREST_NEIGHBOR)
    {
        return ((touchgfx::Bitmap::A4) << RenderingVariant_FormatShift) | RenderingVariant_Alpha | RenderingVariant_NearestNeighbor;
    }
    else
    {
        return ((touchgfx::Bitmap::A4) << RenderingVariant_FormatShift) | RenderingVariant_Alpha | RenderingVariant_Bilinear;
    }
}

uint32_t TextArc::getTextPixelLength() const
{
    return getTextPixelLengthInternal(0, wildcard1, wildcard2);
}

uint32_t TextArc::getTextPixelLengthInternal(int dummy, ...) const
{
    const Font* font = typedText.getFont();
    const TextDirection direction = typedText.getTextDirection();

    uint32_t totalLength = 0;

    TextProvider textProvider;
    va_list pArg;
    va_start(pArg, dummy);
    textProvider.initialize(typedText.getText(), pArg, font->getGSUBTable());

    Unicode::UnicodeChar prevChar = 0;
    Unicode::UnicodeChar ligature;
    while ((ligature = textProvider.getNextLigature(direction)) != 0)
    {
        //find glyph node
        uint8_t bpp;
        const uint8_t* pixels;
        const GlyphNode* glyph = font->getGlyph(ligature, pixels, bpp);

        const int8_t kerning = font->getKerning(prevChar, glyph);

        totalLength += (uint32_t)characterSpacing;
        totalLength += kerning;
        totalLength += glyph->advance();

        prevChar = ligature;
    }

    return totalLength;
}

Rect TextArc::getSolidRect() const
{
    if (alpha < 255)
    {
        return Rect(0, 0, 0, 0);
    }
    return Rect(0, 0, 0, 0);
}
} // namespace touchgfx
