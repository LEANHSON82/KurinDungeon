#pragma once
#include "assets/assets.h"
#include "events/event.h"
#include "screen/screenAction.h"

ScreenAction UpdateEventScreen(EventId id, int &chosenIndex);

void DrawEventScreen(EventId id, const Assets &assets);
