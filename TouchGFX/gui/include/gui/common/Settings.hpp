#pragma once

class Settings
{
public:

    enum class TemperatureUnit
    {
        Celsius,
        Fahrenheit
    };

    static Settings& getInstance();

private:

    Settings() = default;

    TemperatureUnit temperatureUnit = TemperatureUnit::Fahrenheit;
};
