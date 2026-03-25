#ifndef MODEL_HPP
#define MODEL_HPP

#include <cstdint>

class ModelListener;

class Model
{
public:
    // Identify the 5 hobs
    enum HobIndex : uint8_t { BL = 0, TL, M, TR, BR, HOB_COUNT };

    Model();

    void bind(ModelListener* listener) { modelListener = listener; }

    // Selected hob when user clicks on HobScreen (pending selection)
    void     setPendingHob(HobIndex i) { pending = i; }
    HobIndex getPendingHob() const     { return pending; }

    // Per-hob level (0..9)
    uint8_t  getHobLevel(HobIndex i) const;
    void     setHobLevel(HobIndex i, uint8_t lvl);

private:
    ModelListener* modelListener;

    HobIndex pending;
    uint8_t  levels[HOB_COUNT];
};

#endif // MODEL_HPP
