#pragma once

#include <map>
#include <vector>

#include "led-matrix.h"
#include "graphics.h"

#include "sandbox/Scene.hpp"
#include "sandbox/utils/Constants.hpp"

namespace sandbox
{
class LedDisplay
{
public:
    LedDisplay(RGBMatrix::Options options, std::map<std::string, rgb_matrix::Font*> fontMap);
    ~LedDisplay();

    void draw(std::vector<sandbox::Scene> scenes);
    void close();

private:

    void init();
    void filledCircle(int center_x, int center_y, int radius, const Color &color);
    void drawBox(int left, int top, int right, int bottom, const Color& color);
    void clear();
    void present();

    rgb_matrix::RGBMatrix* mMatrix = nullptr;
    rgb_matrix::FrameCanvas* mCanvas = nullptr;
    std::map<std::string, rgb_matrix::Font*> mFontMap;
};
}