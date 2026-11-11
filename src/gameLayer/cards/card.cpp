#include "card.h"

// Effect name
static const char *GetEffectName(EffectType type)
{
    switch (type)
    {
    case EffectType::Damage:
        return "Damage";
    case EffectType::Block:
        return "Block";
    }

    return "Unknown";
}

// Description
std::string DescribeEffects(const Card &card)
{
    std::string text;
    for (const Effect &effect : card.effects)
    {
        if (!text.empty())
        {
            text += ", ";
        }
        text += GetEffectName(effect.type);
        text += ' ';
        text += std::to_string(effect.value);
    }
    return text;
}
