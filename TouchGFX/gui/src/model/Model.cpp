#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>
#include <cstring>

Model::Model()
    : modelListener(0)
    , pending(BL)
{
    // default all levels to 0
    for (uint8_t i = 0; i < HOB_COUNT; ++i)
        levels[i] = 0;
}

uint8_t Model::getHobLevel(HobIndex i) const
{
    return (i < HOB_COUNT) ? levels[i] : 0;
}

void Model::setHobLevel(HobIndex i, uint8_t lvl)
{
    if (i >= HOB_COUNT) return;
    if (lvl > 9) lvl = 9;
    levels[i] = lvl;
}
