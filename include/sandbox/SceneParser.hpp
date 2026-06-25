#pragma once

#include <filesystem>
#include <string>
#include <optional>

#include <nlohmann/json.hpp>

#include "sandbox/Scene.hpp"

namespace sandbox
{

class SceneParser
{
public:
    Scene parse(const std::filesystem::path& file) const;

private:
    SceneObjectType parseObjectType(const std::string& type) const;
    Position parsePosition(const nlohmann::json& positionJson) const;
    std::optional<std::string> parseFontSize(const nlohmann::json& objectJson) const;
    SceneObject parseObject(const nlohmann::json& objectJson) const;
};
}