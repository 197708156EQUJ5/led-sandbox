#include "sandbox/LedDisplay.hpp"

#include <vector>
#include <utility>
#include <iostream>

#include "sandbox/Scene.hpp"
#include "sandbox/config/ApplicationConfig.hpp"

using namespace rgb_matrix;
using sandbox::config::FontConfig;

namespace sandbox
{

LedDisplay::LedDisplay(rgb_matrix::RGBMatrix::Options options, const FontConfig& fontConfig) : 
    mOptions(options),
    mFontConfig(fontConfig),
    mFontLibrary(fontConfig.folder)
{
    for (const auto& [alias, fontName] : fontConfig.aliases)
    {
        mFontMap.emplace(alias, &mFontLibrary.get(fontName));
    }
}

bool LedDisplay::init()
{
    rgb_matrix::RuntimeOptions runtime_options;

    mMatrix = rgb_matrix::RGBMatrix::CreateFromOptions(mOptions, runtime_options);
    
    if (mMatrix == nullptr)
    {
        return false;
    }

    mCanvas = mMatrix->CreateFrameCanvas();

    if (mCanvas == nullptr)
    {
        delete mMatrix;
        mMatrix = nullptr;
        return false;
    }

    return true;
}

LedDisplay::~LedDisplay()
{
    if (mCanvas != nullptr)
    {
        clear();
    }

    if (mMatrix != nullptr && mCanvas != nullptr)
    {
        present();
    }

    delete mMatrix;
}

void LedDisplay::shutdown()
{
    clear();
    present();
    std::cout << "LedDisplay Shutdown" << std::endl;
}

void LedDisplay::draw(std::vector<sandbox::Scene> scenes)
{
    clear();
    for (Scene scene : scenes) 
    {
        for (SceneObject object : scene.sceneObjects)
        {
            int x = object.position.x;
            int y = object.position.y;
            bool is_filled = object.fill.value_or(false);
            Color color = parseColor(object.color);

            switch (object.sceneObjectType)
            {
            case SceneObjectType::CIRCLE:
            {
                int radius = object.radius.value_or(0);
                if (is_filled)
                {
                    filledCircle(x, y, radius, color);
                }
                else
                {
                    DrawCircle(mCanvas, x, y, radius, color);
                }
                break;
            }
            case SceneObjectType::RECTANGLE:
            {
                if (is_filled)
                {
                    fillBox(x, y, x + object.width.value_or(0), y + object.height.value_or(0), color);
                }
                else
                {
                    drawBox(x, y, x + object.width.value_or(0), y + object.height.value_or(0), color);
                }
                break;
            }
            case SceneObjectType::TEXT:            
            {
                const Font& font = *mFontMap.at(object.fontSize.value_or("small"));
                DrawText(mCanvas, font, x, y, color, nullptr, object.text.value_or("").c_str());
                break;
            }            
            default:
                break;
            }
        }
    }
    present();
}

Color LedDisplay::parseColor(const std::string& colorText) const
{
    if (colorText.empty())
    {
        return Color(255, 255, 255);
    }
    if (colorText.front() == '#')
    {
        int r = std::stoi(colorText.substr(1, 2), nullptr, 16);
        int g = std::stoi(colorText.substr(3, 2), nullptr, 16);
        int b = std::stoi(colorText.substr(5, 2), nullptr, 16);
        return Color(r, g, b);
    }

    return Colors::fromString(colorText);
}

void LedDisplay::filledCircle(int center_x, int center_y, int radius, const Color &color)
{
    for (int y = -radius; y <= radius; ++y)
    {
        for (int x = -radius; x <= radius; ++x)
        {
            if ((x * x) + (y * y) <= (radius * radius))
            {
                mCanvas->SetPixel(center_x + x, center_y + y, color.r, color.g, color.b);
            }
        }
    }
}

void LedDisplay::fillBox(int left, int top, int right, int bottom, const Color& color)
{
    for (int y = top; y <= bottom; ++y)
    {
        DrawLine(mCanvas, left, y, right, y, color);
    }
}

void LedDisplay::drawBox(int left, int top, int right, int bottom, const Color& color)
{
    DrawLine(mCanvas, left, top, right, top, color);
    DrawLine(mCanvas, left, bottom, right, bottom, color);

    DrawLine(mCanvas, left, top, left, bottom, color);
    DrawLine(mCanvas, right, top, right, bottom, color);
}

void LedDisplay::clear()
{
    mCanvas->Clear();
}

void LedDisplay::present()
{
    mCanvas = mMatrix->SwapOnVSync(mCanvas);
}
    
} // namespace sandbox

