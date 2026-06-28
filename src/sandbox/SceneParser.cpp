#include "sandbox/SceneParser.hpp"

#include <fstream>
#include <stdexcept>
#include <string>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

using sandbox::Position;
using sandbox::Scene;
using sandbox::SceneObject;
using sandbox::SceneObjectType;

namespace sandbox
{

Scene SceneParser::parseFile(const std::filesystem::path& file) const
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
        throw std::runtime_error(
            "Invalid JSON in scene file '" + file.string() + "': " + error.what()
        );
    }

    return parseJson(root);
}

Scene SceneParser::parseJsonText(std::string_view jsonText) const
{
    json root;

    try
    {
        root = json::parse(jsonText);
    }
    catch (const json::parse_error& error)
    {
        throw std::runtime_error(
            "Invalid scene JSON received over IPC: " + std::string(error.what())
        );
    }

    return parseJson(root);
}

Scene SceneParser::parseJson(const json& root) const
{
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

std::optional<std::string> SceneParser::parseFontSize(const json& objectJson) const
{
    if (!objectJson.contains("fontSize"))
    {
        return std::nullopt;
    }

    return objectJson.at("fontSize").get<std::string>();
}

SceneObject SceneParser::parseObject(const json& objectJson) const
{
    SceneObject object;

    object.sceneObjectType = parseObjectType(objectJson.at("type").get<std::string>());
    object.position = parsePosition(objectJson.at("position"));
    object.color = objectJson.value("color", "");
    object.fontSize = parseFontSize(objectJson);

    switch (object.sceneObjectType)
    {
        case SceneObjectType::CIRCLE:
            object.radius = objectJson.at("radius").get<int>();
            object.fill = objectJson.at("fill").get<bool>();
            break;

        case SceneObjectType::RECTANGLE:
            object.width = objectJson.at("width").get<int>();
            object.height = objectJson.at("height").get<int>();
            object.fill = objectJson.at("fill").get<bool>();
            break;

        case SceneObjectType::TEXT:
            object.text = objectJson.at("text").get<std::string>();
            break;
    }

    return object;
}
}