#pragma once

#include <gui_generated/thermostat_screen/ThermostatViewBase.hpp>
#include <gui/thermostat_screen/ThermostatPresenter.hpp>

#include <touchgfx/widgets/canvas/Line.hpp>
#include <touchgfx/widgets/canvas/PainterRGB888.hpp>

#include <array>


struct Fan
{
    bool update(float dt);

    bool _on = false;
    float _angle = 0.0f;
    float _velocity = 0.0f;
    float _direction = 1.0f;
};


struct Thermostat
{
    bool update(float dt);

    float _temperature_nominal_c = 20.5f; //!< the temperature that has been set by the user
    float _temperature_current_c = 20.5f; //!< the current temperature measured by our thermostat
    float _temperature_delta = 0.0f;      //!< indicates how much we are heating or cooling
    bool _fan_is_blowing = false;
};


class ThermostatView : public ThermostatViewBase
{
public:

    enum class TemperatureUnit
    {
        Celsius,
        Fahrenheit
    };

    ThermostatView() = default;

    void handleDragEvent(const touchgfx::DragEvent& event);
    void handleTickEvent() override;
    void update();

    void setupScreen() override;
    void selectCelsius() override;
    void selectFahrenheit() override;



private:

    colortype backgroundColor(float temp_c);
    void updateFan();
    void updateLines();

    static constexpr int32_t _line_count = 360 / 3;
    static constexpr float _line_angle_increment_rad = (6.28318530718f / static_cast<float>(_line_count));
    touchgfx::PainterRGB888 _line_painter;
    std::array<touchgfx::Line, _line_count> _lines;
    static constexpr uint32_t CANVAS_BUFFER_SIZE = 7200u;
    uint8_t canvasBuffer[CANVAS_BUFFER_SIZE];

    uint32_t _tick_current = 0;
    uint32_t _tick_previous = 0;
    Thermostat _thermostat;
    Fan _fan;
    TemperatureUnit _temperature_unit = TemperatureUnit::Fahrenheit;
    static constexpr uint32_t GRADIENT_SIZE = 256u;
    colortype _gradient[GRADIENT_SIZE];
    float _handle_angle_rad = 0.0f;
    bool _update_ui_elements_requested = true;
};
