#ifndef MODEL_HPP
#define MODEL_HPP

#include <stdint.h>

#include <touchgfx/Utils.hpp>
#include <touchgfx/hal/HAL.hpp>

class ModelListener;

using namespace touchgfx;

class Model
{
public:
    Model();

    void bind(ModelListener* listener)
    {
        modelListener = listener;
    }

    void tick();

protected:
    ModelListener* modelListener;
};

#endif // MODEL_HPP
