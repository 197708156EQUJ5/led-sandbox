#pragma once

#include <cstdint>
#include <filesystem>
#include <map>
#include <string>
#include <string_view>

#include "led-matrix.h"

namespace sandbox
{
enum class DataIngestionMethod
{
    JsonFolderWatcher
};

struct RgbMatrixConfig
{
    int rows{64};
    int cols{64};
    int chainLength{1};
    int parallel{1};

    std::string hardwareMapping{"regular"};
    bool disableHardwarePulsing{true};
    int brightness{100};

    void applyTo(rgb_matrix::RGBMatrix::Options& options) const;
};

struct FontConfig
{
    std::filesystem::path folder;
    std::map<std::string, std::string> aliases;

    std::filesystem::path fontPath(std::string_view alias) const;
};

struct JsonFolderWatcherConfig
{
    std::filesystem::path folder;
};

struct DataIngestionConfig
{
    DataIngestionMethod method{DataIngestionMethod::JsonFolderWatcher};
    JsonFolderWatcherConfig jsonFolderWatcher;
};

struct ApplicationConfig
{
    RgbMatrixConfig rgbMatrix;
    FontConfig fonts;
    DataIngestionConfig data;

    static ApplicationConfig load(const std::filesystem::path& configPath);
};
}