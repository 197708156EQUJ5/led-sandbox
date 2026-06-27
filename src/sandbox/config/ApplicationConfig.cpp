#include "sandbox/config/ApplicationConfig.hpp"

#include <limits>
#include <sstream>
#include <stdexcept>
#include <utility>

#include <toml++/toml.hpp>

namespace
{
const toml::table& requireTable(const toml::table& table, std::string_view key)
{
    const toml::table* child = table[key].as_table();

    if (child == nullptr)
    {
        throw std::runtime_error("Missing required TOML table [" + std::string(key) + "].");
    }

    return *child;
}

std::string requireString(const toml::table& table, std::string_view key)
{
    const auto value = table[key].value<std::string>();

    if (!value)
    {
        throw std::runtime_error("Missing or invalid string value for '" + std::string(key) + "'.");
    }

    if (value->empty())
    {
        throw std::runtime_error("Configuration value '" + std::string(key) + "' cannot be empty.");
    }

    return *value;
}

bool optionalBool(const toml::table& table, std::string_view key, bool defaultValue)
{
    const auto value = table[key].value<bool>();

    if (!value)
    {
        return defaultValue;
    }

    return *value;
}

int optionalInt(const toml::table& table, std::string_view key, int defaultValue)
{
    const auto value = table[key].value<std::int64_t>();

    if (!value)
    {
        return defaultValue;
    }

    if (*value < std::numeric_limits<int>::min() || *value > std::numeric_limits<int>::max())
    {
        throw std::runtime_error("Configuration value '" + std::string(key) + "' is outside the supported integer range.");
    }

    return static_cast<int>(*value);
}

int requirePositiveInt(const toml::table& table, std::string_view key)
{
    const int value = optionalInt(table, key, 0);

    if (value <= 0)
    {
        throw std::runtime_error("Configuration value '" + std::string(key) + "' must be a positive integer.");
    }

    return value;
}

std::filesystem::path resolvePath(const std::filesystem::path& configDirectory, const std::string& configuredPath)
{
    std::filesystem::path path{configuredPath};

    if (path.is_relative())
    {
        path = configDirectory / path;
    }

    return path.lexically_normal();
}

sandbox::config::DataIngestionMethod parseIngestionMethod(const std::string& ingestionMethod)
{
    if (ingestionMethod == "json_folder_watcher")
    {
        return sandbox::config::DataIngestionMethod::FOLDER_WATCHER;
    }
    else if (ingestionMethod == "zmq_ipc")
    {
        return sandbox::config::DataIngestionMethod::ZMQ_IPC;
    }

    throw std::runtime_error("Unsupported data.ingestion value: '" + ingestionMethod + "'.");
}
}

namespace sandbox::config
{
void RgbMatrixConfig::applyTo(rgb_matrix::RGBMatrix::Options& options) const
{
    options.rows = rows;
    options.cols = cols;
    options.chain_length = chainLength;
    options.parallel = parallel;
    options.hardware_mapping = hardwareMapping.c_str();
    options.disable_hardware_pulsing = disableHardwarePulsing;
    options.brightness = brightness;
}

std::filesystem::path FontConfig::fontPath(std::string_view alias) const
{
    const auto iterator = aliases.find(std::string(alias));

    if (iterator == aliases.end())
    {
        throw std::runtime_error("No font alias configured for '" + std::string(alias) + "'.");
    }

    return folder / iterator->second;
}

ApplicationConfig ApplicationConfig::load(const std::filesystem::path& configPath)
{
    if (!std::filesystem::exists(configPath))
    {
        throw std::runtime_error("Configuration file does not exist: " + configPath.string());
    }

    toml::table root;

    try
    {
        root = toml::parse_file(configPath.string());
    }
    catch (const toml::parse_error& error)
    {
        std::ostringstream message;
        message << "Could not parse TOML config '" << configPath.string() << "': " << error;

        throw std::runtime_error(message.str());
    }

    const std::filesystem::path configDirectory = configPath.parent_path();

    ApplicationConfig config;

    const toml::table& matrixTable = requireTable(root, "rgb_matrix");

    config.rgbMatrix.rows = requirePositiveInt(matrixTable, "rows");
    config.rgbMatrix.cols = requirePositiveInt(matrixTable, "cols");
    config.rgbMatrix.chainLength = requirePositiveInt(matrixTable, "chain_length");
    config.rgbMatrix.parallel = requirePositiveInt(matrixTable, "parallel");
    config.rgbMatrix.hardwareMapping = requireString(matrixTable, "hardware_mapping");
    config.rgbMatrix.disableHardwarePulsing = optionalBool(matrixTable, "disable_hardware_pulsing", true);
    config.rgbMatrix.brightness = optionalInt(matrixTable, "brightness", 100);

    if (config.rgbMatrix.brightness < 1 || config.rgbMatrix.brightness > 100)
    {
        throw std::runtime_error("rgb_matrix.brightness must be between 1 and 100.");
    }

    const toml::table& fontsTable = requireTable(root, "fonts");

    config.fonts.folder = resolvePath(configDirectory, requireString(fontsTable, "folder"));

    const toml::table& aliasesTable = requireTable(fontsTable, "aliases");

    for (const auto& [key, value] : aliasesTable)
    {
        const auto fileName = value.value<std::string>();

        if (!fileName || fileName->empty())
        {
            throw std::runtime_error("Font alias '" + std::string(key.str()) + "' must map to a non-empty filename.");
        }

        config.fonts.aliases.emplace(std::string(key.str()), *fileName);
    }

    if (config.fonts.aliases.empty())
    {
        throw std::runtime_error("[fonts.aliases] must contain at least one font alias.");
    }

    const toml::table& dataTable = requireTable(root, "data");

    const std::string ingestionName = requireString(dataTable, "ingestion");

    config.data.method = parseIngestionMethod(ingestionName);

    switch (config.data.method)
    {
        case DataIngestionMethod::FOLDER_WATCHER:
        {
            const toml::table& watcherTable = requireTable(dataTable, "json_folder_watcher");

            config.data.jsonFolderWatcher.folder = resolvePath(configDirectory, requireString(watcherTable, "folder"));

            break;
        }
        case DataIngestionMethod::ZMQ_IPC:
        {
            const toml::table& zmqIpcTable = requireTable(dataTable, "zmq_ipc");

            config.data.zmqIpc.endpoint = requireString(zmqIpcTable, "endpoint");

            break;
        }
    }

    return config;
}
}