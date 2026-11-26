#include "saveGame.h"
#include <filesystem>
#include <fstream>
#include <string>
#include <raylib.h>

const int saveVersion = 1;

static std::string GetSaveFolder()
{
    return std::string(GetApplicationDirectory()) + "saves/";
}

static std::string GetSavePath()
{
    return GetSaveFolder() + "run.txt";
}

bool HasSave()
{
    return std::filesystem::exists(GetSavePath());
}

void SaveRun(const RunState &run)
{
    std::error_code error;
    std::filesystem::create_directories(GetSaveFolder(), error);
    std::ofstream file(GetSavePath());
    if (!file)
    {
        TraceLog(LOG_WARNING, "Cannot write save file %s", GetSavePath().c_str());
        return;
    }

    file << "version " << saveVersion << "\n";
    file << "character " << static_cast<int>(run.player.character) << "\n";
    file << "seed " << run.seed << "\n";
    file << "hp " << run.player.hp << "\n";
    file << "maxHp " << run.player.maxHp << "\n";
    file << "gold " << run.gold << "\n";
    file << "room " << run.currentRoom << "\n";

    file << "deck " << run.player.masterDeck.size();
    for (CardId card : run.player.masterDeck)
    {
        file << " " << static_cast<int>(card);
    }
    file << "\n";

    file << "relics " << run.relics.size();
    for (RelicId relic : run.relics)
    {
        file << " " << static_cast<int>(relic);
    }
    file << "\n";

    std::vector<int> cleared;
    for (int id = 0; id < run.map.GetRoomCount(); id++)
    {
        if (run.map.GetRoom(id).cleared)
        {
            cleared.push_back(id);
        }
    }
    file << "cleared " << cleared.size();
    for (int id : cleared)
    {
        file << " " << id;
    }
    file << "\n";

    file << "rng " << run.rng << "\n";
}

static bool ReadValue(std::ifstream &file, const char *key, int &value)
{
    std::string word;
    file >> word >> value;
    return file && word == key;
}

static bool ReadList(std::ifstream &file, const char *key, int limit, std::vector<int> &values)
{
    int count = 0;
    if (!ReadValue(file, key, count) || count < 0 || count > 1000)
    {
        return false;
    }
    values.clear();
    for (int i = 0; i < count; i++)
    {
        int value = -1;
        file >> value;
        if (!file || value < 0 || value >= limit)
        {
            return false;
        }
        values.push_back(value);
    }
    return true;
}

static bool ReadRun(std::ifstream &file, RunState &run)
{
    int version = 0;
    int character = 0;
    if (!ReadValue(file, "version", version) || version != saveVersion)
    {
        return false;
    }
    if (!ReadValue(file, "character", character) || character < 0 || character >= characterCount)
    {
        return false;
    }
    std::string word;
    unsigned long seedValue = 0;
    file >> word >> seedValue;
    if (!file || word != "seed")
    {
        return false;
    }

    run.player.character = static_cast<CharacterId>(character);
    run.seed = static_cast<std::uint32_t>(seedValue);
    if (!ReadValue(file, "hp", run.player.hp) || !ReadValue(file, "maxHp", run.player.maxHp) ||
        !ReadValue(file, "gold", run.gold) || !ReadValue(file, "room", run.currentRoom))
    {
        return false;
    }
    if (run.player.maxHp <= 0 || run.player.hp <= 0 || run.player.hp > run.player.maxHp || run.gold < 0)
    {
        return false;
    }

    std::vector<int> deck;
    std::vector<int> relics;
    std::vector<int> cleared;
    run.map = DungeonMap::Generate(run.seed);
    if (!ReadList(file, "deck", cardCount, deck) || !ReadList(file, "relics", relicCount, relics) ||
        !ReadList(file, "cleared", run.map.GetRoomCount(), cleared))
    {
        return false;
    }

    file >> word >> run.rng;
    if (!file || word != "rng")
    {
        return false;
    }

    for (int card : deck)
    {
        run.player.masterDeck.push_back(static_cast<CardId>(card));
    }
    for (int relic : relics)
    {
        run.relics.push_back(static_cast<RelicId>(relic));
    }
    for (int id : cleared)
    {
        run.map.MarkCleared(id);
    }
    return run.map.IsValidRoom(run.currentRoom) && run.map.GetRoom(run.currentRoom).cleared;
}

bool LoadRun(RunState &run)
{
    std::ifstream file(GetSavePath());
    if (!file)
    {
        return false;
    }

    RunState loaded;
    if (!ReadRun(file, loaded))
    {
        TraceLog(LOG_WARNING, "Save file %s is damaged", GetSavePath().c_str());
        return false;
    }
    run = loaded;
    TraceLog(LOG_INFO, "Run loaded, seed %u", static_cast<unsigned>(run.seed));
    return true;
}

void DeleteSave()
{
    std::error_code error;
    std::filesystem::remove(GetSavePath(), error);
}
