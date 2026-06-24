#pragma once

#include "graphics.h"

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

    inline Color fromString(std::string_view color_name)
    {
        if (color_name == "black") return black;
        if (color_name == "white") return white;

        if (color_name == "red") return red;
        if (color_name == "green") return green;
        if (color_name == "blue") return blue;

        if (color_name == "yellow") return yellow;
        if (color_name == "cyan") return cyan;
        if (color_name == "magenta") return magenta;

        if (color_name == "gray") return gray;
        if (color_name == "lightGray") return lightGray;
        if (color_name == "darkGray") return darkGray;

        if (color_name == "orange") return orange;
        if (color_name == "purple") return purple;
        if (color_name == "pink") return pink;

        if (color_name == "brown") return brown;
        if (color_name == "lime") return lime;
        if (color_name == "teal") return teal;

        if (color_name == "navy") return navy;
        if (color_name == "olive") return olive;
        if (color_name == "maroon") return maroon;

        if (color_name == "silver") return silver;
        if (color_name == "gold") return gold;
        if (color_name == "violet") return violet;

        return white;
    }
}