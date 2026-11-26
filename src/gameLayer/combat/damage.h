#pragma once
#include "status.h"

int ModifyDamage(int base, const Statuses &source, const Statuses &target);
void ApplyDamage(int amount, int &hp, int &block);
