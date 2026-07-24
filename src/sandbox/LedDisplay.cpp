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

void LedDisplay::draw(const std::vector<sandbox::Scene>& scenes)
{
    clear();
    for (const Scene& scene : scenes) 
    {
        for (const SceneObject& object : scene.sceneObjects)
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
            case SceneObjectType::TRIANGLE:
            {
                const std::string directionJson = object.direction.value_or("");
                //std::cout << "JSON Direction: " << directionJson << std::endl;
                TriangleDirection direction = TriangleDirection::NORTH;
                if (directionJson == "north")
                {
                    direction = TriangleDirection::NORTH;
                }
                else if (directionJson == "south")
                {
                    direction = TriangleDirection::SOUTH;
                }
                else if (directionJson == "east")
                {
                    direction = TriangleDirection::EAST;
                }
                else if (directionJson == "west")
                {
                    direction = TriangleDirection::WEST;
                }
                //std::cout << "enum Direction: " << static_cast<int>(direction) << std::endl;
                drawTriangle(x, y, object.width.value_or(0), object.height.value_or(0), direction, color);
                break;
            }
            case SceneObjectType::TEXT:            
            {
                const Font& font = *mFontMap.at(object.fontSize.value_or("small"));
                const std::string text = object.text.value_or("");
                DrawText(mCanvas, font, x, y, color, nullptr, text.c_str());
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

void LedDisplay::drawTriangle(int center_x, int center_y, int width, int height, TriangleDirection direction, const Color& color)
{
    if (width < 3 || height < 3)
    {
        return;
    }

    // Odd dimensions give the triangle a real center pixel.
    if ((width % 2) == 0)
    {
        ++width;
    }

    if ((height % 2) == 0)
    {
        ++height;
    }

    const int half_width = width / 2;
    const int half_height = height / 2;

    const int left = center_x - half_width;
    const int right = center_x + half_width;
    const int top = center_y - half_height;
    const int bottom = center_y + half_height;

    const auto draw_pixel = [&](int pixel_x, int pixel_y)
    {
        DrawLine(mCanvas, pixel_x, pixel_y, pixel_x, pixel_y, color);
    };

    switch (direction)
    {
        case TriangleDirection::NORTH:
        {
            for (int row = 0; row < height; ++row)
            {
                const int pixel_y = top + row;

                const int extent = std::min(half_width,
                    (row * half_width + half_height - 1) / half_height);

                const int edge_left = center_x - extent;
                const int edge_right = center_x + extent;

                if (pixel_y == bottom)
                {
                    DrawLine(mCanvas, edge_left, pixel_y, edge_right, pixel_y, color);
                }
                else
                {
                    draw_pixel(edge_left, pixel_y);
                    draw_pixel(edge_right, pixel_y);
                }
            }

            break;
        }

        case TriangleDirection::SOUTH:
        {
            for (int row = 0; row < height; ++row)
            {
                const int pixel_y = top + row;
                const int distance_from_tip = bottom - pixel_y;
                const int extent = std::min(half_width, 
                    (distance_from_tip * half_width + half_height - 1) / half_height);
                const int edge_left = center_x - extent;
                const int edge_right = center_x + extent;

                if (pixel_y == top)
                {
                    DrawLine(mCanvas, edge_left, pixel_y, edge_right, pixel_y, color);
                }
                else
                {
                    draw_pixel(edge_left, pixel_y);
                    draw_pixel(edge_right, pixel_y);
                }
            }

            break;
        }

        case TriangleDirection::EAST:
        {
            DrawLine(mCanvas, left, top, left, bottom, color);
            for (int column = 0; column < width; ++column)
            {
                const int pixel_x = left + column;
                const int extent = std::min(half_height,
                    (column * half_height + half_width - 1) / half_width);
                const int edge_top = center_y - extent;
                const int edge_bottom = center_y + extent;

                draw_pixel(pixel_x, edge_top);
                draw_pixel(pixel_x, edge_bottom);
            }

            break;
        }

        case TriangleDirection::WEST:
        {
            DrawLine(mCanvas, right, top, right, bottom, color);
            for (int column = 0; column < width; ++column)
            {
                const int pixel_x = left + column;
                const int distance_from_tip = right - pixel_x;
                const int extent = std::min(half_height,
                    (distance_from_tip * half_height + half_width - 1) / half_width);
                const int edge_top = center_y - extent;
                const int edge_bottom = center_y + extent;

                draw_pixel(pixel_x, edge_top);
                draw_pixel(pixel_x, edge_bottom);
            }

            break;
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

