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
    Scene parseFile(const std::filesystem::path& file) const;
    Scene parseJsonText(std::string_view jsonText) const;

private:
    Scene parseJson(const nlohmann::json& root) const;
    SceneObject parseObject(const nlohmann::json& objectJson) const;
    SceneObjectType parseObjectType(const std::string& type) const;
    Position parsePosition(const nlohmann::json& positionJson) const;
    std::optional<std::string> parseFontSize(const nlohmann::json& objectJson) const;
};
}