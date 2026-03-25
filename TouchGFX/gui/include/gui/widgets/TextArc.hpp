#ifndef TEXTARC_HPP
#define TEXTARC_HPP

#include <touchgfx/widgets/Widget.hpp>
#include <touchgfx/TextProvider.hpp>
#include <touchgfx/TypedText.hpp>
#include <math.h> //lint !e829

namespace touchgfx
{
/**
 * @class TextArc TextArc.hpp touchgfx/widgets/TextArc.hpp
 *
 * @brief The TextArc class is a widget capable of drawing a transformed image
 *
 *        The TextArc displays a transformed image. The TextArc can be used in
 *        effects where an image should be rotated in two or three dimensions.
 *
 *        The image can be freely scaled and rotated in three dimensions. The scaling and
 *        rotation is done around the adjustable origin. A virtual camera is applied to the
 *        rendered image yielding a perspective impression. The amount of perspective
 *        impression can be adjusted. The transformed image is clipped according to the
 *        dimensions of the TextArc. In order to make the image fully visible the
 *        TextArc should be large enough to accommodate the transformed image.
 *
 *        Note that the drawing of this widget is not trivial and typically has a significant
 *        effect on the mcu load. The number of pixels drawn, the presence of global alpha or
 *        per pixel alpha inflicts the computation and should be considered.
 *
 *        Note that this widget does not support 1 bit per pixel color depth.
 *
 * @see Widget
 */
class TextArc : public Widget
{
public:

    class TextCurveGenerator
    {
    public:
        virtual void getCharacterPosition(const int32_t pixels, float& x, float& y, float& rotation) const = 0;
        virtual ~TextCurveGenerator() {}
    };

    /**
     * @enum RenderingAlgorithm
     *
     * @brief Rendering algorithms of the image.
     *
     *        Rendering algorithms of the image.
     *
     *        NEAREST_NEIGHBOR: Fast algorithm with medium image quality. Good for fast
     *        animations. (Default)
     *
     *        BILINEAR_INTERPOLATION: Slower algorithm but better image quality.
     */
    enum RenderingAlgorithm
    {
        NEAREST_NEIGHBOR,
        BILINEAR_INTERPOLATION
    };

    /**
     * @fn TextArc::TextArc();
     *
     * @brief Default constructor.
     */
    TextArc();

    /**
     * @fn virtual TextArc::~TextArc();
     *
     * @brief Destructor.
     *
     *        Destructor.
     */
    virtual ~TextArc();

    /**
     * @fn virtual void TextArc::draw(const Rect& invalidatedArea) const;
     *
     * @brief Draws the given invalidated area.
     *
     *        Draws the given invalidated area. The part of the transformed image inside the
     *        invalidatedArea will be drawn.
     *
     * @param invalidatedArea The rectangle to draw, with coordinates relative to this drawable.
     *
     * @see Drawable::draw()
     */
    virtual void draw(const Rect& invalidatedArea) const;

    /**
     * @fn virtual Rect TextArc::getSolidRect() const;
     *
     * @brief Gets solid rectangle.
     *
     *        Gets solid rectangle.
     *
     * @return largest possible solid rect.
     *
     * @see Drawable::getSolidRect()
     */
    virtual Rect getSolidRect() const;

    /**
     * @fn virtual void TextArc::setRenderingAlgorithm(RenderingAlgorithm algorithm)
     *
     * @brief Sets the algorithm to be used.
     *
     *        Sets the algorithm to be used. Default setting is NEAREST_NEIGHBOR.
     *
     * @param algorithm The algorithm to use when rendering.
     */
    virtual void setRenderingAlgorithm(RenderingAlgorithm algorithm)
    {
        currentRenderingAlgorithm = algorithm;
    }

    /**
     * @fn virtual RenderingAlgorithm TextArc::getRenderingAlgorithm() const
     *
     * @brief Gets the algorithm used when rendering.
     *
     *        Gets the algorithm used when rendering.
     *
     * @return The algorithm used when rendering.
     */
    virtual RenderingAlgorithm getRenderingAlgorithm() const
    {
        return currentRenderingAlgorithm;
    }

    /**
     * @fn void TextArc::setAlpha(uint8_t a)
     *
     * @brief Sets the global alpha blending value.
     *
     *        Sets the global alpha blending value.
     *
     * @param a new alpha.
     */
    void setAlpha(uint8_t a)
    {
        alpha = a;
    }

    /**
    * @fn uint8_t TextArc::getAlpha();
    *
    * @brief Gets the current alpha value.
    *
    *        Gets the current alpha value.
    *
    * @return The current alpha value.
    *
    * @see setAlpha
    */
    uint8_t getAlpha() const
    {
        return alpha;
    }

    /**
     * @fn virtual void TextArc::setCamera(float x, float y)
     *
     * @brief Sets the camera coordinate.
     *
     *        Sets the camera coordinate.
     *
     * @param x The x coordinate for the camera.
     * @param y The y coordinate for the camera.
     */
    virtual void setCamera(float x, float y)
    {
        xCamera = x;
        yCamera = y;
    }

    /**
     * @fn virtual float TextArc::getCameraX() const
     *
     * @brief Gets camera x coordinate.
     *
     *        Gets camera x coordinate.
     *
     * @return The camera x coordinate.
     */
    virtual float getCameraX() const
    {
        return xCamera;
    }

    /**
     * @fn virtual float TextArc::getCameraY() const
     *
     * @brief Gets camera y coordinate.
     *
     *        Gets camera y coordinate.
     *
     * @return The camera y coordinate.
     */
    virtual float getCameraY() const
    {
        return yCamera;
    }

    /**
     * @fn virtual void TextArc::setCameraDistance(float d)
     *
     * @brief Sets camera distance.
     *
     *        Sets camera distance. Minimal allowed distance is MINIMAL_CAMERA_DISTANCE. Values
     *        below will be set to MINIMAL_CAMERA_DISTANCE.
     *
     * @param d The new camera distance.
     */
    virtual void setCameraDistance(float d)
    {
        cameraDistance = MAX(d, MINIMAL_CAMERA_DISTANCE);
    }

    /**
     * @fn virtual float TextArc::getCameraDistance() const
     *
     * @brief Gets camera distance.
     *
     *        Gets camera distance.
     *
     * @return The camera distance.
     */
    virtual float getCameraDistance() const
    {
        return cameraDistance;
    }

    /**
     * @fn void TextArc::setTextCurveGenerator(const TextCurveGenerator& generator);
     *
     * @brief Sets the curve generator of the text.
     *
     *        Sets the curve generator of the text arc.
     *
     * @param generator The TextCurveGenerator for this widget to use.
     */
    void setTextCurveGenerator(const TextCurveGenerator& generator)
    {
        curveGenerator = &generator;
    }

    /**
     * @fn void TextArc::setPositionOffset(const int32_t pixels);
     *
     * @brief Sets the pixel position offset of the text.
     *
     *        Sets the pixel position offset of the text.
     *
     * @param pixels The pixel offset.
     */
    void setPositionOffset(const int32_t pixels)
    {
        positionOffset = pixels;
    }

    /**
     * @fn virtual int32_t TextArc::getPositionOffset() const;
     *
     * @brief Gets the pixel position offset
     *
     *        Gets the pixel position offset
     *
     * @return The pixel position offset.
     */
    int32_t getPositionOffset() const
    {
        return positionOffset;
    }

    /**
     * @fn void TextArc::setTypedText(TypedText t);
     *
     * @brief Sets the TypedText of the text arc.
     *
     *        Sets the TypedText of the text arc.
     *
     * @param t The TypedText for this widget to display.
     */
    void setTypedText(TypedText t)
    {
        typedText = t;
    }

    /**
     * @fn uint32_t TextArc::getTextPixelLength() const
     *
     * @brief Gets the length of the text in pixels.
     *
     *        Gets the length of the text in pixels with the wildcards expanded.
     *
     * @return The length of the text in pixels.
     */
    uint32_t getTextPixelLength() const;

    /**
     * @fn void TextArc::setTypedText(TypedText t);
     *
     * @brief Sets the TypedText of the text arc.
     *
     *        Sets the TypedText of the text arc.
     *
     * @param t The TypedText for this widget to display.
     */
    void setCharacterSpacing(float space)
    {
        characterSpacing = space;
    }

    /**
     * @fn float TextArc::getCharacterSpacing() const
     *
     * @brief Gets the character spacing.
     *
     *        Gets the character spacing.
     *
     * @return The character spacing in pixels.
     */
    float getCharacterSpacing() const
    {
        return characterSpacing;
    }

    /**
     * @fn void TextArc::setWildcard1(const Unicode::UnicodeChar* value)
     *
     * @brief Sets the first wildcard in the text.
     *
     *        Sets the first wildcard in the text. Must be a zero-terminated UnicodeChar array.
     *
     * @param value A pointer to the UnicodeChar to set the wildcard to.
     */
    void setWildcard1(const Unicode::UnicodeChar* value)
    {
        wildcard1 = value;
    }

    /**
     * @fn void TextArc::setWildcard2(const Unicode::UnicodeChar* value)
     *
     * @brief Sets the first wildcard in the text.
     *
     *        Sets the first wildcard in the text. Must be a zero-terminated UnicodeChar array.
     *
     * @param value A pointer to the UnicodeChar to set the wildcard to.
     */
    void setWildcard2(const Unicode::UnicodeChar* value)
    {
        wildcard2 = value;
    }

    /**
     * @fn inline void TextArc::setColor(colortype color)
     *
     * @brief Sets the color of the text.
     *
     *        Sets the color of the text.
     *
     * @param color The color to use.
     */
    void setColor(colortype color)
    {
        this->color = color;
    }

protected:

    /**
     * @fn void TextArc::applyTransformation();
     *
     * @brief Applies the transformation.
     *
     *        Transform the bitmap using the supplied origo, scale, rotation and camera. This
     *        method is called by all the methods that manipulate origo, scale, rotation and
     *        camera.
     */
    void applyTransformation();

    virtual void drawInternal(const Rect& invalidatedArea, int dummy, ...) const;
    uint32_t getTextPixelLengthInternal(int dummy, ...) const;

    struct BitmapCorners;
    struct BitmapTransformation;

    void drawOneCharacter(const Rect& invalidatedArea, uint16_t* const fb, const BitmapCorners& bc, const uint8_t* pixels, const uint32_t width, const uint32_t height) const;
    void applyCharacterTransformation(const BitmapTransformation& bt, BitmapCorners& bc) const;

    /**
     * @fn void TextArc::drawTriangle(const Rect& invalidatedArea, uint16_t* fb, const float* triangleXs, const float* triangleYs, const float* triangleZs, const float* triangleUs, const float* triangleVs) const;
     *
     * @brief Draw triangle.
     *
     *        The TextArc will draw the transformed bitmap by drawing two triangles. One
     *        triangle is created from the points 0,1,2 and the other triangle from the points
     *        1,2,3. The triangle is drawn using the x,y,z values from each point along with
     *        the u,v coordinates in the bitmap associated with each point.
     *
     * @param invalidatedArea The invalidated area.
     * @param [in,out] fb     If non-null, the fb.
     * @param triangleXs      The triangle xs.
     * @param triangleYs      The triangle ys.
     * @param triangleZs      The triangle zs.
     * @param triangleUs      The triangle us.
     * @param triangleVs      The triangle vs.
     */
    void drawTriangle(const Rect& invalidatedArea, uint16_t* fb, const uint8_t* pixels, const int32_t width, const int32_t height, const float* triangleXs, const float* triangleYs, const float* triangleZs, const float* triangleUs, const float* triangleVs) const;

    /**
     * @fn RenderingVariant TextArc::lookupRenderVariant() const;
     *
     * @brief Returns the rendering variant based on the bitmap format, alpha value and rendering
     *        algorithm.
     *
     *        Returns the rendering variant based on the bitmap format, alpha value and
     *        rendering algorithm.
     *
     * @return The RenderingVariant.
     */
    RenderingVariant lookupRenderVariant() const;

    void initializeTextProvider(TextProvider& tp, TypedText t, ...) const;

    RenderingAlgorithm currentRenderingAlgorithm;   ///< The current rendering algorithm.
    uint8_t            alpha;                       ///< An alpha value that is applied to the entire text.

    static const int MINIMAL_CAMERA_DISTANCE = 1;   ///< The minimal camera distance

    float zOrigo;           ///< The origo z coordinate
    float xCamera;          ///< The camera x coordinate
    float yCamera;          ///< The camera y coordinate
    float cameraDistance;   ///< The camera distance

    const TextCurveGenerator* curveGenerator; ///< The character position generator
    int32_t   positionOffset;                 ///< The character position to start from
    float     characterSpacing;               ///< Additional spacing between characters in pixels
    TypedText typedText;                      ///< The TypedText to display
    const Unicode::UnicodeChar* wildcard1;    ///< Pointer to the first wildcard string. Must be zero-terminated.
    const Unicode::UnicodeChar* wildcard2;    ///< Pointer to the second wildcard string. Must be zero-terminated.
    colortype color;                          ///< The color to use.
    TextProvider textProvider;                ///< The TextProvider to scan the text and wildcards.

    struct BitmapCorners
    {
        float imageX0;    ///< The coordinate for the image points
        float imageY0;    ///< The coordinate for the image points
        float imageZ0;    ///< The coordinate for the image points
        float imageX1;    ///< The coordinate for the image points
        float imageY1;    ///< The coordinate for the image points
        float imageZ1;    ///< The coordinate for the image points
        float imageX2;    ///< The coordinate for the image points
        float imageY2;    ///< The coordinate for the image points
        float imageZ2;    ///< The coordinate for the image points
        float imageX3;    ///< The coordinate for the image points
        float imageY3;    ///< The coordinate for the image points
        float imageZ3;    ///< The coordinate for the image points
    };

    struct BitmapTransformation
    {
        float xBitmapPosition;     ///< The bitmap position x
        float yBitmapPosition;     ///< The bitmap position y
        float xOrigo;              ///< The origo x coordinate
        float yOrigo;              ///< The origo y coordinate
        float zAngle;              ///< The angle z
        uint16_t character_width;  ///< The width of the character (bitmap)
        uint16_t character_height; ///< The height of the character (bitmap)
    };
};

class StraightLineTextGenerator : public TextArc::TextCurveGenerator
{
public:
    StraightLineTextGenerator() : x0(0), y0(0), angle(0.f), cosA(1.f), sinA(0.f)
    { }

    void setStartPosition(const int16_t x, const int16_t y)
    {
        x0 = x;
        y0 = y;
    }

    void setAngle(const float _angle)
    {
        angle = _angle;
        cosA = cosf(_angle);
        sinA = sinf(_angle);
    }

    float getAngle()
    {
        return angle;
    }

    virtual void getCharacterPosition(const int32_t pixels, float& x, float& y, float& rotation) const
    {
        x = x0 + cosA * pixels;
        y = y0 + sinA * pixels;
        rotation = angle;
    }

protected:
    int16_t x0, y0;
    float angle, cosA, sinA;
};

class CircularTextGenerator : public TextArc::TextCurveGenerator
{
public:

    enum TextFlowDirection
    {
        FLOW_CLOCKWISE,
        FLOW_COUNTER_CLOCKWISE
    };

    CircularTextGenerator() : x0(0), y0(0), radius(0.f), angle0(0.f), angleS(1.0f), textFLowDirection(FLOW_CLOCKWISE)
    { }

    void setCenter(const int16_t x, const int16_t y)
    {
        x0 = x;
        y0 = y;
    }

    void setRadius(const float _radius)
    {
        radius = _radius;
    }

    float getRadius() const
    {
        return radius;
    }

    void setStartAngle(const float angle)
    {
        angle0 = angle;
    }

    void setAngleScale(const float angleScale)
    {
        angleS = angleScale;
    }

    void setTextFlowDirection(TextFlowDirection direction)
    {
        textFLowDirection = direction;
    }

    TextFlowDirection getTextFlowDirection()
    {
        return textFLowDirection;
    }

    virtual void getCharacterPosition(const int32_t pixels, float& x, float& y, float& rotation) const
    {
        float angle = angle0 + pixels * angleS / radius; //a0+p*2*pi/(2*pi*r)

        x = x0 + radius * cosf(angle + 3.14159f / 2);
        y = y0 - radius * sinf(angle + 3.14159f / 2);

        if (textFLowDirection == FLOW_CLOCKWISE)
        {
            x = x0 + radius * cosf(-angle + 3.14159f / 2);
            y = y0 - radius * sinf(-angle + 3.14159f / 2);
            rotation = angle;
        }
        else
        {
            x = x0 + radius * cosf(angle + 3.14159f / 2);
            y = y0 - radius * sinf(angle + 3.14159f / 2);
            rotation = 3.14159f - angle;

        }
    }

protected:
    int16_t x0, y0;
    float radius;
    float angle0;
    float angleS;

    TextFlowDirection textFLowDirection;
};

class SegmentedCurveGenerator : public TextArc::TextCurveGenerator
{
public:
    SegmentedCurveGenerator() : g1(0), g2(0), offset(0)
    { }

    void setGenerators(const TextCurveGenerator& generator1, const TextCurveGenerator& generator2, int32_t _offset)
    {
        g1 = &generator1;
        g2 = &generator2;
        offset = _offset;
    }

    virtual void getCharacterPosition(const int32_t pixels, float& x, float& y, float& rotation) const
    {
        assert(g1);
        assert(g2);
        if (pixels > offset)
        {
            g2->getCharacterPosition(pixels - offset, x, y, rotation);
        }
        else
        {
            g1->getCharacterPosition(pixels, x, y, rotation);
        }
    }
protected:
    const TextCurveGenerator* g1;
    const TextCurveGenerator* g2;
    int32_t offset;
};

} // namespace touchgfx

#endif // TEXTARC_HPP
