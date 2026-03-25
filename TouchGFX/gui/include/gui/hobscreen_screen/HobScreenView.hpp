#ifndef HOBSCREENVIEW_HPP
#define HOBSCREENVIEW_HPP

#include <gui_generated/hobscreen_screen/HobScreenViewBase.hpp>
#include <gui/hobscreen_screen/HobScreenPresenter.hpp>
#include <gui/model/Model.hpp>
#include <touchgfx/Unicode.hpp>
#include <cstdint>

class HobScreenView : public HobScreenViewBase
{
public:
    HobScreenView() {}
    virtual ~HobScreenView() {}

    virtual void setupScreen() override;
    virtual void tearDownScreen() override {}

    // FlexButton callbacks from Designer – now wired for all 5
    virtual void onHobBLClicked() override;
    virtual void onHobTLClicked() override;
    virtual void onHobMClicked()  override;
    virtual void onHobTRClicked() override;
    virtual void onHobBRClicked() override;

    // Debug helpers (unchanged)
    void onPlusClicked();
    void onMinusClicked();

private:
    void refreshAllFromModel();
    void showLevelDigit(Model::HobIndex i, uint8_t lvl);
    void setAlphaFor(Model::HobIndex i, uint8_t a, uint16_t dur = 12);

    // Wildcard buffers – one per hob ("9\0")
    touchgfx::Unicode::UnicodeChar bufBL[3] = {0};
    touchgfx::Unicode::UnicodeChar bufTL[3] = {0};
    touchgfx::Unicode::UnicodeChar bufM [3] = {0};
    touchgfx::Unicode::UnicodeChar bufTR[3] = {0};
    touchgfx::Unicode::UnicodeChar bufBR[3] = {0};
};

#endif // HOBSCREENVIEW_HPP
