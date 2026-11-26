#include "characterDatabase.h"

static const CharacterDefinition characters[characterCount]{
    {"Knight", 80,
     {CardId::Strike, CardId::Strike, CardId::Strike, CardId::Strike, CardId::Strike,
      CardId::Defend, CardId::Defend, CardId::Defend, CardId::Defend,
      CardId::HeavyStrike},
     RelicId::FieldBandage},
    {"Swordsman", 70,
     {CardId::Strike, CardId::Strike, CardId::Strike, CardId::Strike,
      CardId::Defend, CardId::Defend, CardId::Defend, CardId::Defend,
      CardId::QuickSlash, CardId::DoubleCut},
     RelicId::Whetstone},
};

const CharacterDefinition &GetCharacter(CharacterId id)
{
    return characters[static_cast<int>(id)];
}
