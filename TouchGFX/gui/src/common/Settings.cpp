#include "gui/common/Settings.hpp"

Settings& Settings::getInstance()
{
    static Settings instance;
    return instance;
}
