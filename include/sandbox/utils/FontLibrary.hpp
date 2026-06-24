#pragma once

#include <filesystem>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <iostream>

#include "graphics.h"

namespace sandbox::utils
{
class FontLibrary
{
public:
    explicit FontLibrary(const std::filesystem::path& fontsFolder)
    {
        for (const auto& entry : std::filesystem::directory_iterator(fontsFolder))
        {
            if (!entry.is_regular_file() || entry.path().extension() != ".bdf")
            {
                continue;
            }

            auto font = std::make_unique<rgb_matrix::Font>();

            if (!font->LoadFont(entry.path().c_str()))
            {
                throw std::runtime_error("Could not load font: " + entry.path().string());
            }

            mFonts.emplace(entry.path().stem().string(), std::move(font));
        }
    }

    const rgb_matrix::Font& get(const std::string& name) const
    {
        return *mFonts.at(name);
    }

private:
    std::unordered_map<std::string, std::unique_ptr<rgb_matrix::Font>> mFonts;
};
}