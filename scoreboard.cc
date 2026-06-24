#include <chrono>
#include <fstream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "led-matrix.h"
#include "graphics.h"

#include "team_colors.h"

using namespace rgb_matrix;

namespace Colors
{
    inline const Color lightBackground{50, 50, 50};

    inline const Color black{0, 0, 0};
    inline const Color white{255, 255, 255};

    inline const Color red{255, 0, 0};
    inline const Color green{0, 255, 0};
    inline const Color blue{0, 0, 255};

    inline const Color yellow{255, 255, 0};
    inline const Color cyan{0, 255, 255};
    inline const Color magenta{255, 0, 255};

    inline const Color gray{128, 128, 128};
    inline const Color lightGray{192, 192, 192};
    inline const Color darkGray{64, 64, 64};

    inline const Color orange{255, 165, 0};
    inline const Color purple{128, 0, 128};
    inline const Color pink{255, 192, 203};

    inline const Color brown{165, 42, 42};
    inline const Color lime{191, 255, 0};
    inline const Color teal{0, 128, 128};

    inline const Color navy{0, 0, 128};
    inline const Color olive{128, 128, 0};
    inline const Color maroon{128, 0, 0};

    inline const Color silver{192, 192, 192};
    inline const Color gold{255, 215, 0};
    inline const Color violet{238, 130, 238};
}

struct ScoreData
{
    std::string away_name;
    std::string away_score;
    std::string home_name;
    std::string home_score;
    std::string inning;
    std::string balls;
    std::string strikes;
    std::string outs;
};

std::vector<std::string> split(const std::string &text, char delimiter)
{
    std::vector<std::string> fields;
    std::stringstream stream(text);
    std::string field;

    while (std::getline(stream, field, delimiter))
    {
        fields.push_back(field);
    }

    return fields;
}

bool readScoreFile(const std::string &file_path, ScoreData &score_data)
{
    std::ifstream score_file(file_path);

    if (!score_file)
    {
        return false;
    }

    std::string score_line;
    std::getline(score_file, score_line);

    const std::vector<std::string> fields = split(score_line, ':');

    if (fields.size() != 8)
    {
        return false;
    }

    score_data.away_name = fields[0];
    score_data.away_score = fields[1];
    score_data.home_name = fields[2];
    score_data.home_score = fields[3];
    score_data.inning = fields[4];
    score_data.balls = fields[5];
    score_data.strikes = fields[6];
    score_data.outs = fields[7];

    return true;
}

int parseCount(const std::string &text, int maximum)
{
    try
    {
        const int value = std::stoi(text);

        if (value < 0)
        {
            return 0;
        }

        if (value > maximum)
        {
            return maximum;
        }

        return value;
    }
    catch (...)
    {
        return 0;
    }
}

void FilledCircle(Canvas *canvas, int center_x, int center_y, int radius, const Color &color)
{
    for (int y = -radius; y <= radius; ++y)
    {
        for (int x = -radius; x <= radius; ++x)
        {
            if ((x * x) + (y * y) <= (radius * radius))
            {
                canvas->SetPixel(center_x + x, center_y + y, color.r, color.g, color.b);
            }
        }
    }
}

void drawCount(Canvas *canvas, int count, int maximum, int first_x, int y, const Color &active_color)
{
    constexpr int dot_radius = 2;
    constexpr int dot_spacing = 7;

    for (int i = 0; i < maximum; ++i)
    {
        const int x = first_x + (i * dot_spacing);

        if (i < count)
        {
            FilledCircle(canvas, x, y, dot_radius, active_color);
        }
        else
        {
            DrawCircle(canvas, x, y, dot_radius, Colors::darkGray);
        }
    }
}

int main()
{
    RGBMatrix::Options options;
    options.rows = 64;
    options.cols = 64;
    options.chain_length = 2;
    options.hardware_mapping = "regular";
    options.disable_hardware_pulsing = true;

    RuntimeOptions runtime_options;

    RGBMatrix *matrix = RGBMatrix::CreateFromOptions(options, runtime_options);

    if (matrix == nullptr)
    {
        return 1;
    }

    Font font;

    if (!font.LoadFont("/home/ggraves/github/rpi-rgb-led-matrix/fonts/7x13.bdf"))
    {
        delete matrix;
        return 1;
    }

    FrameCanvas *canvas = matrix->CreateFrameCanvas();

    const std::string score_file_path = "/home/ggraves/github/led-display/score.txt";

    /*
       score.txt format:

       away_name:away_score:home_name:home_score:inning:balls:strikes:outs

       Example:
       CIN:0:NYY:0:T6:2:1:0
     */

    while (true)
    {
        ScoreData score_data;

        canvas->Clear();

        if (!readScoreFile(score_file_path, score_data))
        {
            DrawText(canvas,
                    font,
                    2,
                    22,
                    Colors::white,
                    nullptr,
                    "NO GAME DATA");
        }
        else
        {
            // Top row: away team / score / home team / score.
            DrawText(canvas, font, 2, 13, TeamColors::forTeam(score_data.away_name), &Colors::lightBackground, score_data.away_name.c_str());
            DrawText(canvas, font, 27, 13, Colors::white, nullptr, score_data.away_score.c_str());

            DrawText(canvas, font, 48, 13, TeamColors::forTeam(score_data.home_name), &Colors::lightBackground, score_data.home_name.c_str());
            DrawText(canvas, font, 73, 13, Colors::white, nullptr, score_data.home_score.c_str());

            // Inning: "T6", "B6", "F", etc.
            DrawText(canvas, font, 2, 28, Colors::white, nullptr, score_data.inning.c_str());

            // Balls: four circles.
            DrawText(canvas, font, 2, 43, Colors::white, nullptr, "BALLS");

            drawCount(canvas, parseCount(score_data.balls, 4), 4, 44, 38, Colors::green);

            // Strikes: three circles beneath balls.
            DrawText(canvas, font, 2, 56, Colors::white, nullptr, "STRIKES");

            drawCount(canvas, parseCount(score_data.strikes, 3), 3, 59, 51, Colors::red);

            // Outs: right side, three circles.
            DrawText(canvas, font, 87, 43, Colors::white, nullptr, "OUT");

            drawCount(canvas, parseCount(score_data.outs, 3), 3, 110, 38, Colors::yellow);
        }

        canvas = matrix->SwapOnVSync(canvas);

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    delete matrix;
    return 0;
}
