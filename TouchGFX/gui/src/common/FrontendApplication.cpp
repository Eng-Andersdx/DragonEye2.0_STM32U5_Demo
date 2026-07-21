#include "gui/common/FrontendApplication.hpp"

#include <touchgfx/Utils.hpp>

const uint32_t BITMAP_CACHE_SIZE = 128 * 1024;
LOCATION_PRAGMA_NOLOAD("BmpCacheSection")
uint8_t bitmapCache[BITMAP_CACHE_SIZE] LOCATION_ATTRIBUTE_NOLOAD("BmpCacheSection");

FrontendApplication::FrontendApplication(Model& m, FrontendHeap& heap)
    : FrontendApplicationBase(m, heap)
{
    Bitmap::setCache((uint16_t*)bitmapCache, BITMAP_CACHE_SIZE, 128);
}

void FrontendApplication::handleTickEvent()
{
    model.tick();
    FrontendApplicationBase::handleTickEvent();
}
