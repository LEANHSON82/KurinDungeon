#include "scenes/battle/Scene_Battle.h"
#include "engine/GameEngine.h"
#include "scenes/Scene_End.h"
#include "scenes/Scene_Reward.h"
#include "scenes/battle/BattleLayout.h"

// ---------------------------------------------------------------------------
// Setup

Scene_Battle::Scene_Battle(GameEngine *game, const std::vector<EnemyId> &enemies, bool isBoss)
    : Scene(game), m_isBoss(isBoss), m_rng(game->currentRun().rng())
{
    registerAction(KEY_ENTER, "CONFIRM");
    registerAction(KEY_ESCAPE, "CANCEL");
    registerAction(KEY_E, "END_TURN");
    registerAction(KEY_LEFT, "TARGET_LEFT");
    registerAction(KEY_RIGHT, "TARGET_RIGHT");
    for (int i = 0; i < 9; i++)
    {
        registerAction(KEY_ONE + i, "SELECT_" + std::to_string(i + 1));
    }

    spawnPlayer();
    spawnEnemies(enemies);
    spawnDeck();
    m_entityManager.update();

    shuffleDrawPile();
    chooseEnemyIntents();
    startPlayerTurn();
    updateResult();
    m_target = nextLivingEnemy(nullptr, 1);
}

void Scene_Battle::spawnPlayer()
{
    const Run &run = m_game->currentRun();
    m_player = m_entityManager.addEntity("player");
    m_player->add<CHealth>(run.hp, run.maxHp);
    m_player->add<CBlock>();
    m_player->add<CTransform>(Vector2{playerX, feetY});
    m_player->add<CActor>("Knight", false);
    setAnimation(m_player, "Idle", true);
}

void Scene_Battle::spawnEnemies(const std::vector<EnemyId> &enemies)
{
    int count = static_cast<int>(enemies.size());
    for (int slot = 0; slot < count; slot++)
    {
        const EnemyDefinition &definition = GetEnemyDefinition(enemies[slot]);
        std::shared_ptr<Entity> enemy = m_entityManager.addEntity("enemy");
        enemy->add<CEnemy>(enemies[slot], slot);
        enemy->add<CHealth>(definition.maxHp, definition.maxHp);
        enemy->add<CBlock>();
        enemy->add<CStrength>();
        enemy->add<CIntent>();
        if (definition.bossAbility)
        {
            enemy->add<CBossAbility>();
        }
        enemy->add<CTransform>(EnemyFeet(slot, count));
        enemy->add<CActor>(definition.sprites, true);
        setAnimation(enemy, "Idle", true);
    }
}

void Scene_Battle::spawnDeck()
{
    for (CardId id : m_game->currentRun().deck)
    {
        m_entityManager.addEntity("card")->add<CCard>(id, Zone::Draw, m_nextOrder++);
    }
}

// ---------------------------------------------------------------------------
// Input

std::shared_ptr<Entity> Scene_Battle::nextLivingEnemy(const std::shared_ptr<Entity> &from, int step)
{
    const EntityVec &enemies = getEnemies();
    int count = static_cast<int>(enemies.size());
    int index = -1;
    for (int i = 0; i < count; i++)
    {
        if (enemies[i] == from)
        {
            index = i;
        }
    }

    for (int tries = 0; tries < count; tries++)
    {
        index = (index + step + count) % count;
        if (isAlive(enemies[index]))
        {
            return enemies[index];
        }
    }
    return from;
}

std::shared_ptr<Entity> Scene_Battle::cardAt(Vector2 point)
{
    EntityVec hand = getPile(Zone::Hand);
    int count = static_cast<int>(hand.size());
    // Later cards are drawn on top when a big hand overlaps.
    for (int i = count - 1; i >= 0; i--)
    {
        if (CheckCollisionPointRec(point, CardRect(i, count, hand[i] == m_selectedCard)))
        {
            return hand[i];
        }
    }
    return nullptr;
}

std::shared_ptr<Entity> Scene_Battle::enemyAt(Vector2 point)
{
    for (const std::shared_ptr<Entity> &enemy : getEnemies())
    {
        if (isAlive(enemy) && CheckCollisionPointRec(point, ActorRect(enemy->get<CTransform>().pos)))
        {
            return enemy;
        }
    }
    return nullptr;
}

void Scene_Battle::toggleCard(const std::shared_ptr<Entity> &card)
{
    m_selectedCard = card == m_selectedCard ? nullptr : card;
}

void Scene_Battle::playSelectedCard(const std::shared_ptr<Entity> &target)
{
    if (!canPlayCard(m_selectedCard, target))
    {
        TraceLog(LOG_DEBUG, "PlayCard rejected by input");
        return;
    }
    sPlayCard(m_selectedCard, target);
    m_game->assets().playSound("PlayCard");
    m_selectedCard = nullptr;
}

void Scene_Battle::clickBattlefield(Vector2 point)
{
    std::shared_ptr<Entity> enemy = enemyAt(point);
    switch (selectedCard()->target)
    {
    case Target::SingleEnemy:
        if (enemy)
        {
            playSelectedCard(enemy);
        }
        break;
    case Target::AllEnemies:
        if (enemy)
        {
            playSelectedCard(nullptr);
        }
        break;
    case Target::Self:
        if (CheckCollisionPointRec(point, ActorRect(m_player->get<CTransform>().pos)))
        {
            playSelectedCard(nullptr);
        }
        break;
    }
}

void Scene_Battle::finishBattle()
{
    Run &run = m_game->currentRun();
    run.hp = m_player->get<CHealth>().hp;

    if (m_result == BattleResult::Lost)
    {
        m_game->changeScene("END", std::make_shared<Scene_End>(m_game, false), true);
    }
    else if (m_isBoss)
    {
        m_game->changeScene("END", std::make_shared<Scene_End>(m_game, true), true);
    }
    else
    {
        m_game->changeScene("REWARD", std::make_shared<Scene_Reward>(m_game), true);
    }
}

void Scene_Battle::sDoAction(const Action &action)
{
    const std::string &name = action.name();
    bool start = action.type() == "START";

    if (m_result != BattleResult::InProgress)
    {
        if (!isPlayingBack() && ((name == "CONFIRM" && start) || isClicked(action, ContinueButton())))
        {
            finishBattle();
        }
        return;
    }
    if (isPlayingBack())
    {
        return;
    }
    if (!isAlive(m_target))
    {
        m_target = nextLivingEnemy(m_target, 1);
    }

    if (name == "MOUSE_MOVE")
    {
        if (std::shared_ptr<Entity> enemy = enemyAt(action.pos()))
        {
            m_target = enemy;
        }
    }
    else if ((name == "CANCEL" || name == "RIGHT_CLICK") && start)
    {
        m_selectedCard = nullptr;
    }
    else if ((name == "END_TURN" && start) || isClicked(action, EndTurnButton()))
    {
        sEndTurn();
        m_selectedCard = nullptr;
    }
    else if (name == "TARGET_LEFT" && start)
    {
        m_target = nextLivingEnemy(m_target, -1);
    }
    else if (name == "TARGET_RIGHT" && start)
    {
        m_target = nextLivingEnemy(m_target, 1);
    }
    else if (name.rfind("SELECT_", 0) == 0 && start)
    {
        int index = std::stoi(name.substr(7)) - 1;
        EntityVec hand = getPile(Zone::Hand);
        if (index < static_cast<int>(hand.size()))
        {
            toggleCard(hand[index]);
        }
    }
    else if (name == "CONFIRM" && start && m_selectedCard)
    {
        playSelectedCard(selectedCard()->target == Target::SingleEnemy ? m_target : nullptr);
    }
    else if (name == "LEFT_CLICK" && start)
    {
        if (std::shared_ptr<Entity> card = cardAt(action.pos()))
        {
            toggleCard(card);
        }
        else if (m_selectedCard)
        {
            clickBattlefield(action.pos());
        }
    }
}

void Scene_Battle::update()
{
    m_entityManager.update();
    sPlayback();
    sAnimation();
    sFloatingText();
    sRender();
    m_currentFrame++;
}
