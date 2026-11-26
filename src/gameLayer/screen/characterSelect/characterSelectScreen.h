#pragma once
#include "assets/assets.h"
#include "characters/character.h"
#include "screen/screenAction.h"

struct CharacterSelectState
{
    int selected = 0;
};

ScreenAction UpdateCharacterSelectScreen(CharacterSelectState &state);

void DrawCharacterSelectScreen(const CharacterSelectState &state, const Assets &assets);
