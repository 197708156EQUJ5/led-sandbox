#include "sandbox/LedDisplay.hpp"

#include <vector>

#include "sandbox/Scene.hpp"

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

LedDisplay::~LedDisplay()
{
    delete mMatrix;
}

void LedDisplay::draw(std::vector<sandbox::Scene> scenes)
{
    clear();
    for (Scene scene : scenes) 
    {
        for (SceneObject object : scene.sceneObjects)
        {
            if (object.sceneObjectType == SceneObjectType::CIRCLE)
            {
                filledCircle(object.position.x, object.position.y, object.radius.value_or(0), 
                    Colors::fromString(object.color));
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

void LedDisplay::clear()
{
    mCanvas->Clear();
}

void LedDisplay::present()
{
    mCanvas = mMatrix->SwapOnVSync(mCanvas);
}

    
} // namespace sandbox

