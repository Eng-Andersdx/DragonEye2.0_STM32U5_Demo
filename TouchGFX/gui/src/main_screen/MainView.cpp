#include <gui/main_screen/MainView.hpp>

#include <gui/common/Constants.hpp>
#include <gui/common/FloatEasingEquations.hpp>
#include <gui/common/Enums.hpp>

#include <math.h>
#include <touchgfx/Utils.hpp>

#include <texts/TextKeysAndLanguages.hpp>
#include <BitmapDatabase.hpp>

#include <initializer_list>

MainView::MainView()
{

}

void MainView::setupScreen()
{
    MainViewBase::setupScreen();

    HAL::getInstance()->setFrameRateCompensation(true);
}

void MainView::tearDownScreen()
{
    MainViewBase::tearDownScreen();
}

void MainView::startFlowerAnimation()
{
    //Action form designer based on when the screen is entered
    wheel.startFlowerAnimation();
}

void MainView::changeScreen(uint8_t value)
{
    /*based on callback from Main Wheel through the designer, the designer actions to change ti the selected screens is malled*/
    switch (value)
    {
    case Enums::E_Bike:
        goToDashboard();
        break;
    case Enums::Thermostat:
        goToThermostat();
        break;
    case Enums::Watch:
        goToWatch();
        break;
    case Enums::Compass:
        goToCompass();
        break;
    case Enums::Svg:
        goToSvg();
        break;
    }
}
