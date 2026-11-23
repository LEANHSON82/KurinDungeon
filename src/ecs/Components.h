#pragma once
#include <memory>
#include <string>
#include <vector>
#include <raylib.h>
#include "data/Cards.h"
#include "data/Enemies.h"
#include "engine/Animation.h"

class Entity;

// Every component remembers whether its entity has it.
struct Component
{
    bool has = false;
};

struct CTransform : public Component
{
    Vector2 pos{};
    CTransform() = default;
    explicit CTransform(Vector2 p) : pos(p) {}
};

struct CHealth : public Component
{
    int hp = 0;
    int maxHp = 0;
    CHealth() = default;
    CHealth(int h, int m) : hp(h), maxHp(m) {}
};

struct CBlock : public Component
{
    int value = 0;
};

struct CStrength : public Component
{
    int value = 0;
};

enum class IntentType
{
    Attack,
    Defend,
    Buff,
};

struct CIntent : public Component
{
    IntentType type = IntentType::Attack;
    int value = 0;
    bool bossAction = false;
    CIntent() = default;
    CIntent(IntentType t, int v, bool boss) : type(t), value(v), bossAction(boss) {}
};

struct CEnemy : public Component
{
    EnemyId id = EnemyId::Slime;
    int slot = 0;
    CEnemy() = default;
    CEnemy(EnemyId i, int s) : id(i), slot(s) {}
};

struct CBossAbility : public Component
{
    bool used = false;
};

enum class Zone
{
    Draw,
    Hand,
    Discard,
    Exhaust,
};

struct CCard : public Component
{
    CardId id = CardId::Strike;
    Zone zone = Zone::Draw;
    int order = 0; // position inside its pile
    CCard() = default;
    CCard(CardId i, Zone z, int o) : id(i), zone(z), order(o) {}
};

struct CAnimation : public Component
{
    Animation animation;
    bool repeat = true;
    CAnimation() = default;
    CAnimation(const Animation &a, bool r) : animation(a), repeat(r) {}
};

// A character drawn on the battlefield. "sprites" is the prefix of its animations,
// e.g. "Knight" for KnightIdle, KnightAttack...
struct CActor : public Component
{
    std::string sprites;
    bool flip = false;
    bool fallen = false; // the death animation has started
    CActor() = default;
    CActor(const std::string &s, bool f) : sprites(s), flip(f) {}
};

struct CFloatingText : public Component
{
    std::string text;
    Color color = WHITE;
    int framesLeft = 0;
    CFloatingText() = default;
    CFloatingText(const std::string &t, Color c, int frames) : text(t), color(c), framesLeft(frames) {}
};

enum class RoomType
{
    Start,
    Combat,
    Rest,
    Boss,
};

struct CRoom : public Component
{
    RoomType type = RoomType::Start;
    int floor = 0;
    int column = 0;
    std::vector<std::shared_ptr<Entity>> next;
    std::vector<EnemyId> enemies;
    bool cleared = false;
    CRoom() = default;
    CRoom(RoomType t, int f, int c) : type(t), floor(f), column(c) {}
};
