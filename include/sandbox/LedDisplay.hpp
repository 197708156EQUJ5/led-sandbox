#pragma once

#include <map>
#include <vector>

#include "led-matrix.h"
#include "graphics.h"

#include "sandbox/Scene.hpp"
#include "sandbox/utils/Constants.hpp"
#include "sandbox/utils/FontLibrary.hpp"
#include "sandbox/ApplicationConfig.hpp"

namespace sandbox
{
class LedDisplay
{
public:
    LedDisplay(rgb_matrix::RGBMatrix::Options options, const sandbox::FontConfig& fontConfig);
    ~LedDisplay();

    void draw(std::vector<sandbox::Scene> scenes);
    void close();
    bool init();

private:

    void filledCircle(int center_x, int center_y, int radius, const Color &color);
    void fillBox(int left, int top, int right, int bottom, const Color& color);
    void drawBox(int left, int top, int right, int bottom, const Color& color);
    void clear();
    void present();

    rgb_matrix::RGBMatrix::Options mOptions;
    const sandbox::FontConfig mFontConfig;
    rgb_matrix::RGBMatrix* mMatrix = nullptr;
    rgb_matrix::FrameCanvas* mCanvas = nullptr;
    utils::FontLibrary mFontLibrary;
    std::map<std::string, const rgb_matrix::Font*> mFontMap;
};
}