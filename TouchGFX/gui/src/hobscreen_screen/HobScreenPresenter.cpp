#include <gui/hobscreen_screen/HobScreenPresenter.hpp>
#include <gui/common/FrontendApplication.hpp>
#include <touchgfx/Application.hpp>

void HobScreenPresenter::goToHobLevelScreen()
{
    static_cast<FrontendApplication*>(touchgfx::Application::getInstance())
        ->gotoHobLevelScreenScreenNoTransition();   // exact function name from FrontendApplicationBase
}
