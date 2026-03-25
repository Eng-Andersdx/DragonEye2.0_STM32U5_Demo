#pragma once
class FloatEasingEquations
{
public:
    FloatEasingEquations();
    ~FloatEasingEquations() {}

    static float cubicEaseOut(float t, float b, float c, float d);
    static float backEaseInOut(float t, float b, float c, float d);
    static float easeNone(float t, float b, float c, float d);
    static float backEaseOut(float t, float b, float c, float d);
    static float bounceEaseOut(float t, float b, float c, float d);

private:

};
