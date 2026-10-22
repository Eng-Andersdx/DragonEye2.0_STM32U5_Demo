#ifndef TOUCHGFX_NEMASTATE_HPP
#define TOUCHGFX_NEMASTATE_HPP

#include <touchgfx/hal/Types.hpp>

namespace touchgfx
{
/**
 * @class NemaState NemaState.hpp touchgfx_nema/NemaState.hpp
 *
 * @brief Caches the state of the GPU2D (NemaGFX) hardware used by LCDGPU2D and LCDGPU2D_AXI.
 *
 *        Changing the clipping rectangle, constant color, texture color or blend mode on the
 *        GPU2D hardware is comparatively expensive. NemaState keeps track of the values that are
 *        currently active and only issues the underlying nema_* calls when a requested value
 *        actually differs from what is already active, thereby avoiding redundant GPU2D register
 *        writes when consecutive draw operations reuse the same clip/color/blend settings.
 *
 * @note NemaState assumes it has exclusive knowledge of the cached GPU2D registers. Call reset()
 *       whenever the GPU2D hardware may have been reprogrammed by something other than the cached
 *       setters on this class, e.g. once per frame in HALGPU2D::endFrame().
 */
class NemaState
{
public:
    /**
     * @fn static NemaState& NemaState::getInstance();
     *
     * @brief Gets the single, shared NemaState instance used by the GPU2D LCD drivers.
     *
     * @return A reference to the shared NemaState instance.
     */
    static NemaState& getInstance();

    /**
     * @fn void NemaState::reset();
     *
     * @brief Marks all cached state as unknown.
     *
     *        Marks all cached state as unknown, forcing the next call to each of the setters to
     *        issue the corresponding nema_* call again.
     */
    void reset();

    /**
     * @fn void NemaState::setDestinationTexture(uint32_t address, int32_t width, int32_t height, uint32_t format, int32_t stride);
     *
     * @brief Binds a destination texture, unless the requested destination is already bound.
     *
     *        Binds a destination texture (framebuffer) to the GPU2D hardware for subsequent draw
     *        operations, unless the requested destination is already bound.
     *
     * @note Automatically invalidates the cached clipping rectangle when the destination texture
     *       actually changes, since binding a destination texture invalidates the clipping
     *       rectangle currently active on the GPU2D hardware.
     *
     * @param  address The address of the destination texture.
     * @param  width   The width of the destination texture, in pixels.
     * @param  height  The height of the destination texture, in pixels.
     * @param  format  The destination texture format, as passed to nema_bind_dst_tex().
     * @param  stride  The destination texture stride, in bytes.
     */
    void setDestinationTexture(uint32_t address, int32_t width, int32_t height, uint32_t format, int32_t stride);

    /**
     * @fn void NemaState::setClippingRect(int16_t x, int16_t y, int16_t width, int16_t height);
     *
     * @brief Sets the GPU2D clipping rectangle, unless the requested rectangle is already active.
     *
     * @param  x      The x coordinate of the clipping rectangle.
     * @param  y      The y coordinate of the clipping rectangle.
     * @param  width  The width of the clipping rectangle.
     * @param  height The height of the clipping rectangle.
     */
    void setClippingRect(int16_t x, int16_t y, int16_t width, int16_t height);

    /**
     * @fn void NemaState::setClippingRect(const Rect& rect);
     *
     * @brief Sets the GPU2D clipping rectangle, unless the requested rectangle is already active.
     *
     * @param  rect The clipping rectangle.
     */
    void setClippingRect(const Rect& rect);

    /**
     * @fn void NemaState::setConstColorAlpha(uint8_t alpha);
     *
     * @brief Sets the GPU2D constant color alpha value, unless already active.
     *
     * @param  alpha The constant color alpha value (0 = fully transparent, 255 = fully opaque).
     */
    void setConstColorAlpha(uint8_t alpha);

    /**
     * @fn void NemaState::setTexColor(uint32_t rgba);
     *
     * @brief Sets the GPU2D texture color, unless already active.
     *
     *        Sets the GPU2D texture color used when rendering alpha-only (e.g. A4/A8) bitmaps and
     *        fonts, unless the requested color is already active.
     *
     * @param  rgba The texture color, encoded as returned by nema_rgba().
     */
    void setTexColor(uint32_t rgba);

    /**
     * @fn void NemaState::setBlendModeFill(uint32_t blendFlags);
     *
     * @brief Sets the GPU2D blend mode used for fill operations, unless already active.
     *
     *        Sets the GPU2D blend mode used for fill operations (nema_set_blend_fill), unless the
     *        requested blend flags are already active for fill operations.
     *
     * @param  blendFlags The GPU2D blend flags, e.g. NEMA_BL_SRC or
     *                     NEMA_BL_SIMPLE | NEMA_BLOP_MODULATE_A.
     */
    void setBlendModeFill(uint32_t blendFlags);

    /**
     * @fn void NemaState::setBlendModeBlit(uint32_t blendFlags);
     *
     * @brief Sets the GPU2D blend mode used for blit operations, unless already active.
     *
     *        Sets the GPU2D blend mode used for blit operations (nema_set_blend_blit), unless the
     *        requested blend flags are already active for blit operations.
     *
     * @param  blendFlags The GPU2D blend flags, e.g. NEMA_BL_SRC or
     *                     NEMA_BL_SIMPLE | NEMA_BLOP_MODULATE_A.
     */
    void setBlendModeBlit(uint32_t blendFlags);

private:
    /** Identifies which GPU2D operation the cached blend flags were last set for. */
    enum BlendKind
    {
        BLEND_UNKNOWN, ///< No blend mode has been cached yet.
        BLEND_FILL,    ///< The cached blend flags were set using nema_set_blend_fill().
        BLEND_BLIT     ///< The cached blend flags were set using nema_set_blend_blit().
    };

    NemaState();

    void setBlendMode(BlendKind kind, uint32_t flags);
    void invalidateClippingRect();

    bool dstValid;           ///< True if the destination texture fields below hold a known, cached value.
    uint32_t dstAddress;     ///< The cached destination texture address.
    int32_t dstWidth;        ///< The cached destination texture width.
    int32_t dstHeight;       ///< The cached destination texture height.
    uint32_t dstFormat;      ///< The cached destination texture format.
    int32_t dstStride;       ///< The cached destination texture stride.
    int16_t clipX;           ///< The cached clipping rectangle x coordinate.
    int16_t clipY;           ///< The cached clipping rectangle y coordinate.
    int16_t clipWidth;       ///< The cached clipping rectangle width. -1 means unknown.
    int16_t clipHeight;      ///< The cached clipping rectangle height.
    int16_t constColorAlpha; ///< The cached constant color alpha. -1 means unknown.
    bool texColorValid;      ///< True if texColorRgba holds a known, cached value.
    uint32_t texColorRgba;   ///< The cached texture color.
    BlendKind blendKind;     ///< Whether the cached blend flags were set for fill or blit.
    uint32_t blendFlags;     ///< The cached blend flags.
};

} // namespace touchgfx

#endif // TOUCHGFX_NEMASTATE_HPP
