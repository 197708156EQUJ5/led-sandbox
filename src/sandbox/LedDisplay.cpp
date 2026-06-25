#include "sandbox/LedDisplay.hpp"

#include <vector>

#include "sandbox/Scene.hpp"

using namespace rgb_matrix;

namespace sandbox
{

LedDisplay::LedDisplay() :
    mFonts(std::filesystem::current_path() / "assets" / "fonts"),
    mTinyFont(mFonts.get("5x8")),
    mSmallFont(mFonts.get("6x10")),
    mScoreFont(mFonts.get("7x13B")),
    mLargeFont(mFonts.get("9x18")),
    mLargeBoldFont(mFonts.get("9x18B")),
    mVeryLargeBoldFont(mFonts.get("9x18B"))
{
    init();
}

LedDisplay::~LedDisplay()
{
    clear();
    present();
    delete mMatrix;
}

void LedDisplay::init()
{
    RGBMatrix::Options options;
    options.rows = 64;
    options.cols = 64;
    options.chain_length = 2;
    options.hardware_mapping = "regular";
    options.disable_hardware_pulsing = true;

    RuntimeOptions runtime_options;

    mMatrix = RGBMatrix::CreateFromOptions(options, runtime_options);
    mCanvas = mMatrix->CreateFrameCanvas();
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
                const Font* font = &mScoreFont;
                if (object.fontSize == "tiny")
                {
                    font = &mTinyFont;
                }
                else if (object.fontSize == "small")
                {
                    font = &mSmallFont;
                }
                else if (object.fontSize == "large")
                {
                    font = &mLargeFont;
                }
                DrawText(mCanvas, *font, object.position.x, object.position.y, Colors::fromString(object.color), 
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

