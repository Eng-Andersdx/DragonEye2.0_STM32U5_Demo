#include <gui/thermostat_screen/ThermostatView.hpp>

#include "BitmapDatabase.hpp"
#include "gui/common/Settings.hpp"
#include <touchgfx/Utils.hpp>
#include <touchgfx/canvas_widget_renderer/CanvasWidgetRenderer.hpp>

#include <algorithm>
#include <cmath>

void clamp(float& angle, float upper_limit, float lower_limit);
float angleToTemperature(float angle);
float celsiusToFahrenheit(float temp_c);


namespace
{
static constexpr auto PI_2 = 1.57079632679f;
static constexpr auto screen_center_x = 240;
static constexpr auto screen_center_y = 240;
static constexpr auto wheel_center_x = 240.0f;
static constexpr auto wheel_center_y = 240.0f;
static constexpr auto outer_handle_distance_squared = 170 * 170;
static constexpr auto temperature_setpoint_min_c = 17.0f;
static constexpr auto temperature_range_c = 10.0f;
static constexpr auto fan_velocity_eps = 0.001f;
static constexpr auto temperature_eps_c = 0.02f;     // the epsilon must be a multiple of the damping, otherwise the
static constexpr auto temperature_damping = 0.005f;  // nominal temperature can never be reached.
static constexpr auto fan_acceleration = 0.003f;
static constexpr auto fan_deceleration = 0.003f;
static constexpr auto fan_rotation_speed = 0.1f;
}

//
//  -PI/2            0.0             PI/2
//    |               |               |
//    +---------------+---------------+
//    |               |               |
//   17.0            20.5            27.0
//
//


void clamp(float& angle, float upper_limit, float lower_limit)
{
    if (angle > upper_limit)
    {
        angle = upper_limit;
    }
    else if (angle < lower_limit)
    {
        angle = lower_limit;
    }
}


float angleToTemperature(float angle)
{
    return temperature_setpoint_min_c + (temperature_range_c * ((-angle + PI_2) / PI));
}


float celsiusToFahrenheit(float temp_c)
{
    return (temp_c * 1.8f) + 32.0f;
}


colortype ThermostatView::backgroundColor(float temp_c)
{
    const auto temp_normalized = (temp_c - temperature_setpoint_min_c) / temperature_range_c;
    const auto color_index = static_cast<uint8_t>(temp_normalized * 255);
    return _gradient[color_index];
}


void ThermostatView::handleDragEvent(const touchgfx::DragEvent& event)
{
    const auto x1 = static_cast<float>(event.getOldX());
    const auto y1 = static_cast<float>(event.getOldY());

    // disregard drag events not coming from the outer handle
    const auto vx_px = event.getOldX() - screen_center_x;
    const auto vy_px = event.getOldY() - screen_center_y;
    const auto v_length_squared = vx_px * vx_px + vy_px * vy_px;
    if (v_length_squared < outer_handle_distance_squared)
    {
        return;
    }

    const auto x2 = static_cast<float>(event.getNewX());
    const auto y2 = static_cast<float>(event.getNewY());

    const auto dx1 = x1 - wheel_center_x;
    const auto dy1 = y1 - wheel_center_y;

    const auto dx2 = x2 - wheel_center_x;
    const auto dy2 = y2 - wheel_center_y;

    const auto angle_1 = atan2(dy1, dx1);
    const auto angle_2 = atan2(dy2, dx2);

    const auto delta_angle = static_cast<float>(angle_2 - angle_1);

    // disregard jumps when user spins the handle too far
    if (static_cast<float>(fabs(delta_angle)) > PI_2)
    {
        return;
    }

    _handle_angle_rad += delta_angle;
    clamp(_handle_angle_rad, PI_2, -PI_2);

    handle.setZAngle(_handle_angle_rad);
    handle.invalidate();

    // at this point we have good angles that we can pass to the temperature control
    const auto temperature = angleToTemperature(_handle_angle_rad);
    _thermostat._temperature_nominal_c = temperature;

    updateLines();
}


void ThermostatView::handleTickEvent()
{
    update();
    _tick_current++;
}


void ThermostatView::updateFan()
{
    if (_fan._velocity > 0.95f)
    {
        fan.setVisible(false);
        fanMotionBlur.setVisible(true);
        fanMotionBlur.setZAngle(_fan._angle);
        fanMotionBlur.invalidateContent();
    }
    else
    {
        fanMotionBlur.setVisible(false);
        fan.setVisible(true);
        fan.setZAngle(_fan._angle);
        fan.invalidateContent();
    }
}


void ThermostatView::update()
{
    const auto dt = static_cast<float>(_tick_current - _tick_previous);
    _tick_previous = _tick_current;

    bool update_thermostat = _thermostat.update(dt);

    _fan._on = update_thermostat;

    // at the moment the fan should just go into a single direction
    // _fan._direction = (_thermostat._temperature_delta > 0.0f) ? 1.0f : -1.0f;

    bool update_fan = _fan.update(dt);

    // only start actually changing the temperature when the fan is properly blowing
    _thermostat._fan_is_blowing = (_fan._velocity > 0.95f);

    // ui updates
    if (update_thermostat || _update_ui_elements_requested)
    {
        const auto celsius = (_temperature_unit == TemperatureUnit::Celsius);
        auto nominal = celsius ? _thermostat._temperature_nominal_c : celsiusToFahrenheit(_thermostat._temperature_nominal_c);
        auto current = celsius ? _thermostat._temperature_current_c : celsiusToFahrenheit(_thermostat._temperature_current_c);

        Unicode::snprintfFloat(temperatureNominalBuffer, 5, "%.1f", nominal);
        temperatureNominal.invalidate();

        // avoid that the decimal dot jumps around by updating the text areas
        // Unicode::snprintfFloat(temperatureCurrentBuffer, 3, "%.0f", current);
        const auto current_floor = std::floor(current);
        Unicode::snprintf(temperatureCurrentBuffer, 3, "%d", static_cast<int32_t>(current_floor));
        temperatureCurrent.invalidate();
        Unicode::snprintf(temperatureCurrentFractionalBuffer, 2, "%d", static_cast<int32_t>((current - current_floor) * 10.0f));
        temperatureCurrentFractional.invalidate();

        // we have to discuss whether we want to display the nominal or current temperature here
        background.setColor(backgroundColor(_thermostat._temperature_current_c));
    }

    if (update_fan || _update_ui_elements_requested)
    {
        updateFan();
    }

    if (_update_ui_elements_requested)
    {
        _update_ui_elements_requested = false;
    }
}


void ThermostatView::setupScreen()
{
    ThermostatViewBase::setupScreen();

    useSMOCDrawing(false);

    HAL::getInstance()->setFrameRateCompensation(true);

    // initialize color gradient
    touchgfx::Bitmap gradient_bitmap(BITMAP_GRADIENT_ID);
    const uint8_t* pixels = reinterpret_cast<const uint8_t*>(gradient_bitmap.getData());
    for (uint16_t i = 0; i < GRADIENT_SIZE; i++)
    {
        const uint8_t red =   pixels[i * 3 + 2];
        const uint8_t green = pixels[i * 3 + 1];
        const uint8_t blue =  pixels[i * 3 + 0];

        _gradient[i] = touchgfx::Color::getColorFromRGB(red, green, blue);
    }

    // setup lines
    touchgfx::CanvasWidgetRenderer::setupBuffer(canvasBuffer, CANVAS_BUFFER_SIZE);

    _line_painter.setColor(touchgfx::Color::getColorFromRGB(255, 255, 255));

    for (auto& line : _lines)
    {
        line.setPosition(0, 0, 480, 480);
        line.setPainter(_line_painter);
        line.setLineEndingStyle(touchgfx::Line::ROUND_CAP_ENDING);
        add(line);
    }

    updateLines();

}


void ThermostatView::selectCelsius()
{
    _temperature_unit = TemperatureUnit::Celsius;
    _update_ui_elements_requested = true;
}


void ThermostatView::selectFahrenheit()
{
    _temperature_unit = TemperatureUnit::Fahrenheit;
    _update_ui_elements_requested = true;
}


bool Fan::update(float dt)
{
    if (_on)
    {
        _velocity = std::min<float>(1.0f, _velocity + fan_acceleration);
    }
    else
    {
        _velocity = std::max<float>(0.0f, _velocity - fan_deceleration);
    }

    _angle += dt * _velocity * _direction * fan_rotation_speed;

    return static_cast<float>(fabs(_velocity)) > fan_velocity_eps;
}


bool Thermostat::update(float dt)
{
    _temperature_delta = _temperature_nominal_c - _temperature_current_c;

    if (_fan_is_blowing)
    {
        _temperature_current_c += dt * ((_temperature_delta > 0.0f) ? temperature_damping : -temperature_damping);
    }

    return static_cast<float>(fabs(_temperature_delta)) > temperature_eps_c;
}


void ThermostatView::updateLines()
{
    static constexpr auto center_x_px = 240;
    static constexpr auto center_y_px = 240;
    static constexpr auto radius_r_1 = 216.0f;
    static constexpr auto radius_r_2 = 231.0f;

    float angle_rad = 0.0f;
    auto index = 0;
    for (auto& line : _lines)
    {
        auto indicator_offset = (index % 5 == 0) ? -5 : 0;
        float x_normalized = static_cast<float>(sin(-_handle_angle_rad + angle_rad));
        float y_normalized = static_cast<float>(cos(-_handle_angle_rad + angle_rad));

        // pull out indicators near directional vector (0, 1)
        auto stretch_factor = 0.0f;
        if (y_normalized < -0.97f)
        {
            stretch_factor = y_normalized * (1.0f - fabs(x_normalized));
            stretch_factor *= stretch_factor; // x^2
            stretch_factor *= stretch_factor; // x^4
            stretch_factor *= stretch_factor; // x^8
            stretch_factor = -stretch_factor;
            indicator_offset = 0;
        }

        const auto offset = (y_normalized < 0.0f) ? stretch_factor : 0.0f;
        const auto alpha = (1.0f - ((y_normalized + 1.0f) * 0.5f));
        const auto p1_x = center_x_px + x_normalized * (radius_r_1 + offset * 15.0f + indicator_offset);
        const auto p1_y = center_y_px + y_normalized * (radius_r_1 + offset * 15.0f + indicator_offset);
        const auto p2_x = center_x_px + x_normalized * radius_r_2;
        const auto p2_y = center_y_px + y_normalized * radius_r_2;

        line.setAlpha(static_cast<uint8_t>(alpha * 255.0f));
        line.setVisible((alpha > 0.17f));
        line.setStart(p1_x, p1_y);
        line.setEnd(p2_x, p2_y);
        line.setLineWidth(std::max<float>(1.0f, 2.5f * fabs(offset)));
        line.invalidate();

        angle_rad += _line_angle_increment_rad;
        index++;
    }
}
