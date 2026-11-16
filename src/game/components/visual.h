#pragma once
#include <string>
#include <raylib.h>
#include "game/assets/assets.h"

struct Position
{
    Vector2 value{};
};

struct Sprite
{
    const CharacterSprites *sprites = nullptr;
    bool flip = false;
};

struct Animator
{
    AnimationType animation = AnimationType::Idle;
    float time = 0.0f;
    bool dead = false;
};

struct FloatingText
{
    std::string text;
    Color color = WHITE;
    float age = 0.0f;
};

struct MapPosition
{
    Vector2 value{};
};
