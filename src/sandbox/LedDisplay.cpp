#include "sandbox/LedDisplay.hpp"

#include <vector>
#include <utility>

#include "sandbox/Scene.hpp"

using namespace rgb_matrix;

namespace sandbox
{

LedDisplay::LedDisplay(RGBMatrix::Options options, std::map<std::string, rgb_matrix::Font*> fontMap) :
    mFontMap(fontMap)
{
    RuntimeOptions runtime_options;

    mMatrix = RGBMatrix::CreateFromOptions(options, runtime_options);
    mCanvas = mMatrix->CreateFrameCanvas();
}

LedDisplay::~LedDisplay()
{
    clear();
    present();
    delete mMatrix;
}

void LedDisplay::close()
{
    clear();
    present();
}

void LedDisplay::draw(std::vector<sandbox::Scene> scenes)
{
    clear();
    for (Scene scene : scenes) 
    {
        for (SceneObject object : scene.sceneObjects)
        {
            switch (object.sceneObjectType)
            {
            case SceneObjectType::CIRCLE:
                filledCircle(object.position.x, object.position.y, object.radius.value_or(0), 
                    Colors::fromString(object.color));
                break;
            case SceneObjectType::RECTANGLE:
                drawBox(object.position.x, object.position.y, object.position.x + object.width.value_or(0), 
                    object.position.y + object.height.value_or(0), Colors::fromString(object.color));
                break;
            case SceneObjectType::TEXT:
            {
                const Font& font = *mFontMap.at(object.fontSize.value_or("small"));
                DrawText(mCanvas, font, object.position.x, object.position.y, Colors::fromString(object.color), 
                    nullptr, object.text.value_or("").c_str());
                break;
            }
            
            default:
                break;
            }
        }
    }
    present();
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

void LedDisplay::drawBox(int left, int top, int right, int bottom, const Color& color)
{
    for (int x = left; x <= right; ++x)
    {
        mCanvas->SetPixel(x, top, color.r, color.g, color.b);
        mCanvas->SetPixel(x, bottom, color.r, color.g, color.b);
    }

    for (int y = top; y <= bottom; ++y)
    {
        mCanvas->SetPixel(left, y, color.r, color.g, color.b);
        mCanvas->SetPixel(right, y, color.r, color.g, color.b);
    }
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

