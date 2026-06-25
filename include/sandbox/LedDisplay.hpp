#pragma once

#include <vector>

#include "led-matrix.h"
#include "graphics.h"

#include "sandbox/Scene.hpp"
#include "sandbox/utils/Constants.hpp"
#include "sandbox/utils/FontLibrary.hpp"

namespace sandbox
{
class LedDisplay
{
public:
    LedDisplay();
    ~LedDisplay();

    void draw(std::vector<sandbox::Scene> scenes);
    void close();

private:

    void init();
    void filledCircle(int center_x, int center_y, int radius, const Color &color);
    void drawBox(int left, int top, int right, int bottom, const Color& color);
    void clear();
    void present();

    sandbox::utils::FontLibrary mFonts;
    rgb_matrix::RGBMatrix* mMatrix = nullptr;
    rgb_matrix::FrameCanvas* mCanvas = nullptr;
    const rgb_matrix::Font& mTinyFont;        // 5x7 or 5x8
    const rgb_matrix::Font& mSmallFont;       // 6x10 or 6x12
    const rgb_matrix::Font& mScoreFont;       // 7x13B — what you've been using
    const rgb_matrix::Font& mLargeFont;       // 9x18 or 10x20
    const rgb_matrix::Font& mLargeBoldFont;   // 9x18B, if we use it
    const rgb_matrix::Font& mVeryLargeBoldFont;   // 10x20B, if we use it
};
}