#ifndef NAVTRANSITIONS_HPP
#define NAVTRANSITIONS_HPP

#include <touchgfx/transitions/CoverTransition.hpp>

// Screen-change animation used across the app.
// CoverTransition draws the new screen sliding over the old one and works without
// animation storage (this project has none configured, so SlideTransition would not animate).
//   Forward (going deeper, e.g. Home -> Oven): new screen comes in from the right.
//   Back    (back button):                     new screen comes in from the left.
static const uint8_t NAV_TRANSITION_STEPS = 14;   // frames; higher = slower

class NavForwardTransition : public touchgfx::CoverTransition<touchgfx::EAST>
{
public:
    NavForwardTransition() : touchgfx::CoverTransition<touchgfx::EAST>(NAV_TRANSITION_STEPS) {}
};

class NavBackTransition : public touchgfx::CoverTransition<touchgfx::WEST>
{
public:
    NavBackTransition() : touchgfx::CoverTransition<touchgfx::WEST>(NAV_TRANSITION_STEPS) {}
};

#endif // NAVTRANSITIONS_HPP
