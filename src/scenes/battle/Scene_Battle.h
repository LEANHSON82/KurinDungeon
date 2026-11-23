#pragma once
#include <deque>
#include <memory>
#include <random>
#include <string>
#include <vector>
#include "engine/Scene.h"

inline constexpr int startingEnergy = 3;
inline constexpr int handSize = 5;
inline constexpr int maxHandSize = 10;

enum class BattleResult
{
    InProgress,
    Won,
    Lost,
};

// The rules change numbers instantly and record what happened as events. The scene then
// plays the events back one by one (animation, floating text, sound) and ignores input
// until they are done.
enum class BattleEventType
{
    Damage,
    Block,
    Status, // a status was put on the target
    HpLoss, // HP lost without Block, e.g. from the player's own cards
    Death,
};

struct BattleEvent
{
    BattleEventType type = BattleEventType::Damage;
    std::shared_ptr<Entity> source;
    std::shared_ptr<Entity> target;
    int value = 0;
    StatusType status = StatusType::Strength; // only for Status events
};

class Scene_Battle : public Scene
{
    bool m_isBoss = false;
    std::shared_ptr<Entity> m_player;
    std::shared_ptr<Entity> m_selectedCard;
    std::shared_ptr<Entity> m_target;

    int m_energy = 0;
    int m_turn = 1;
    int m_nextOrder = 0;
    BattleResult m_result = BattleResult::InProgress;
    std::mt19937 m_rng;

    std::deque<BattleEvent> m_events;
    int m_playbackTimer = 0;
    bool m_endSoundPlayed = false;

    // Setup
    void spawnPlayer();
    void spawnEnemies(const std::vector<EnemyId> &enemies);
    void spawnDeck();

    // Rules
    void shuffleDrawPile();
    void moveToZone(const std::shared_ptr<Entity> &card, Zone zone);
    void discardHand();
    void startPlayerTurn();
    void sEnemyTurn();
    void dealDamage(const std::shared_ptr<Entity> &source, const std::shared_ptr<Entity> &target, int amount);
    void gainBlock(const std::shared_ptr<Entity> &source, const std::shared_ptr<Entity> &target, int amount);
    void loseHp(const std::shared_ptr<Entity> &target, int amount);
    void applyStatus(const std::shared_ptr<Entity> &source, const std::shared_ptr<Entity> &target, StatusType type, int amount);
    void tickStatuses(const std::shared_ptr<Entity> &entity, const CStatus &hadAtStart);
    void markDeadAtZeroHp(const std::shared_ptr<Entity> &source, const std::shared_ptr<Entity> &target);
    void updateResult();
    void pushEvent(BattleEventType type, const std::shared_ptr<Entity> &source, const std::shared_ptr<Entity> &target, int value,
                   StatusType status = StatusType::Strength);
    EntityVec getTargets(const Card &card, const std::shared_ptr<Entity> &target);
    void logBattle(const std::string &label);

    // Input
    void toggleCard(const std::shared_ptr<Entity> &card);
    void playSelectedCard(const std::shared_ptr<Entity> &target);
    void clickBattlefield(Vector2 point);
    std::shared_ptr<Entity> nextLivingEnemy(const std::shared_ptr<Entity> &from, int step);
    void retarget();
    std::shared_ptr<Entity> cardAt(Vector2 point);
    std::shared_ptr<Entity> enemyAt(Vector2 point);
    void finishBattle();

    // Playback and animation
    void sPlayback();
    void playEvent(const BattleEvent &event);
    void sAnimation();
    void sFloatingText();
    void setAnimation(const std::shared_ptr<Entity> &entity, const std::string &name, bool repeat);
    void say(const std::shared_ptr<Entity> &entity, const std::string &text, Color color);

    // Drawing
    const Card *selectedCard() const;
    Color outlineFor(const std::shared_ptr<Entity> &actor) const;
    void drawActors();
    void drawActorInfo();
    void drawStatuses(const std::shared_ptr<Entity> &actor, Vector2 center);
    void drawFloatingTexts();
    void drawHint();
    void drawHud();

public:
    Scene_Battle(GameEngine *game, const std::vector<EnemyId> &enemies, bool isBoss);

    void update() override;
    void sDoAction(const Action &action) override;
    void sRender() override;

    // Rules. Public so the tests can drive a battle without input.
    bool canPlayCard(const std::shared_ptr<Entity> &card, const std::shared_ptr<Entity> &target) const;
    bool sPlayCard(const std::shared_ptr<Entity> &card, const std::shared_ptr<Entity> &target);
    void sEndTurn();
    void sDrawCards(int count);
    void chooseEnemyIntents();
    bool isAlive(const std::shared_ptr<Entity> &enemy) const;
    // The damage a hit of base damage really deals: + source Strength, -25% if the source is
    // Weak, +50% if the target is Vulnerable (rounded down). target may be null.
    int modifyDamage(const std::shared_ptr<Entity> &source, const std::shared_ptr<Entity> &target, int base) const;
    bool isPlayingBack() const;

    EntityVec getPile(Zone zone);
    const EntityVec &getEnemies() { return m_entityManager.getEntities("enemy"); }
    const EntityVec &getCards() { return m_entityManager.getEntities("card"); }
    std::shared_ptr<Entity> player() const { return m_player; }
    const std::deque<BattleEvent> &events() const { return m_events; }
    void clearEvents() { m_events.clear(); }
    int energy() const { return m_energy; }
    void setEnergy(int energy) { m_energy = energy; }
    int turn() const { return m_turn; }
    void setTurn(int turn) { m_turn = turn; }
    void setNextOrder(int order) { m_nextOrder = order; }
    BattleResult result() const { return m_result; }
};
