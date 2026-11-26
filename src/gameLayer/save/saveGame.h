#pragma once
#include "run/runState.h"

bool HasSave();
void SaveRun(const RunState &run);
bool LoadRun(RunState &run);
void DeleteSave();
