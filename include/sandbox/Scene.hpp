#pragma once

#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace sandbox
{
enum class SceneObjectType
{
    CIRCLE,
    RECTANGLE,
    TEXT
};

struct Position
{
    int x{0};
    int y{0};
};

struct SceneObject
{
    SceneObjectType sceneObjectType{SceneObjectType::CIRCLE};

    Position position;
    std::string color;

    std::optional<int> radius;
    std::optional<int> width;
    std::optional<int> height;

    std::optional<std::string> text;
    std::optional<std::string> fontSize;
};

struct Scene
{
    std::vector<SceneObject> sceneObjects;
};
}