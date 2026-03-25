#include <gui/hoblevelscreen_screen/HobLevelScreenPresenter.hpp>
#include <gui/common/FrontendApplication.hpp>

void HobLevelScreenPresenter::goBackToHobScreen()
{
    static_cast<FrontendApplication*>(Application::getInstance())
        ->gotoHobScreenScreenNoTransition();   // exact generated name
}
