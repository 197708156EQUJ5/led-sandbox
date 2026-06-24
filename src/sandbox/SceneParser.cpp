#include "sandbox/SceneParser.hpp"

#include <fstream>
#include <stdexcept>
#include <string>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

using sandbox::FontSize;
using sandbox::Position;
using sandbox::Scene;
using sandbox::SceneObject;
using sandbox::SceneObjectType;

namespace sandbox
{
Scene SceneParser::parse(const std::filesystem::path& file) const
{
    std::ifstream input(file);

    if (!input)
    {
        throw std::runtime_error("Could not open scene file: " + file.string());
    }

    json root;

    try
    {
        input >> root;
    }
    catch (const json::parse_error& error)
    {
        throw std::runtime_error("Invalid JSON in scene file '" + file.string() + "': " + error.what());
    }

    if (!root.contains("objects") || !root["objects"].is_array())
    {
        throw std::runtime_error("Scene JSON must contain an 'objects' array.");
    }

    Scene scene;

    for (const auto& objectJson : root["objects"])
    {
        scene.sceneObjects.push_back(parseObject(objectJson));
    }

    return scene;
}

SceneObjectType SceneParser::parseObjectType(const std::string& type) const
{
    if (type == "circle")
    {
        return SceneObjectType::CIRCLE;
    }

    if (type == "rectangle")
    {
        return SceneObjectType::RECTANGLE;
    }

    if (type == "text")
    {
        return SceneObjectType::TEXT;
    }

    throw std::runtime_error("Unknown scene object type: " + type);
}

Position SceneParser::parsePosition(const json& positionJson) const
{
    Position position;
    position.x = positionJson.at("x").get<int>();
    position.y = positionJson.at("y").get<int>();

    return position;
}

FontSize SceneParser::parseFontSize(const json& objectJson) const
{
    if (!objectJson.contains("fontSize") || objectJson["fontSize"].is_null())
    {
        return std::monostate{};
    }

    const auto& fontSizeJson = objectJson["fontSize"];

    if (fontSizeJson.is_number_integer())
    {
        return fontSizeJson.get<int>();
    }

    if (fontSizeJson.is_string())
    {
        return fontSizeJson.get<std::string>();
    }

    throw std::runtime_error("fontSize must be an integer, string, null, or omitted.");
}

SceneObject SceneParser::parseObject(const json& objectJson) const
{
    SceneObject object;

    object.sceneObjectType = parseObjectType(objectJson.at("type").get<std::string>());
    object.position = parsePosition(objectJson.at("position"));
    object.color = objectJson.value("color", "");
    object.fontSizeValue = parseFontSize(objectJson);

    switch (object.sceneObjectType)
    {
        case SceneObjectType::CIRCLE:
            object.radius = objectJson.at("radius").get<int>();
            break;

        case SceneObjectType::RECTANGLE:
            object.width = objectJson.at("width").get<int>();
            object.height = objectJson.at("height").get<int>();
            break;

        case SceneObjectType::TEXT:
            object.text = objectJson.at("text").get<std::string>();
            break;
    }

    return object;
}
}