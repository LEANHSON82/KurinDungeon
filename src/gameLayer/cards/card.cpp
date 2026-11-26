#include "card.h"
#include "combat/damage.h"

std::string DescribeEffect(const Effect &effect, Target target, const Statuses &source, const Statuses &victim)
{
    std::string value = std::to_string(effect.value);
    const char *toAll = target == Target::AllEnemies ? " to all" : "";
    switch (effect.type)
    {
    case EffectType::Damage:
        return "Damage " + std::to_string(ModifyDamage(effect.value, source, victim)) + toAll;
    case EffectType::Block:
        return "Block " + value;
    case EffectType::DrawCards:
        return "Draw " + value;
    case EffectType::GainEnergy:
        return "+" + value + " Energy";
    case EffectType::LoseHp:
        return "Lose " + value + " HP";
    case EffectType::ApplyStatus:
    {
        std::string name = GetStatusDefinition(effect.status).name;
        if (target == Target::Self)
        {
            return "Gain " + value + " " + name;
        }
        return name + " " + value + toAll;
    }
    case EffectType::Heal:
        return "Heal " + value + " HP";
    case EffectType::GainGold:
        return "+" + value + " Gold";
    }
    return "";
}

std::string DescribeEffects(const Card &card, const Statuses &source, const Statuses &victim)
{
    std::string text;
    for (const Effect &effect : card.effects)
    {
        if (!text.empty())
        {
            text += ", ";
        }
        text += DescribeEffect(effect, card.target, source, victim);
    }
    if (card.exhaust)
    {
        text += text.empty() ? "Exhaust" : ", Exhaust";
    }
    return text;
}

std::string DescribeEffects(const Card &card)
{
    return DescribeEffects(card, Statuses{}, Statuses{});
}

const char *GetKindName(CardKind kind)
{
    switch (kind)
    {
    case CardKind::Attack:
        return "Attack";
    case CardKind::Skill:
        return "Skill";
    case CardKind::Power:
        return "Power";
    }
    return "";
}

const char *GetRarityName(CardRarity rarity)
{
    switch (rarity)
    {
    case CardRarity::Starter:
        return "Starter";
    case CardRarity::Common:
        return "Common";
    case CardRarity::Uncommon:
        return "Uncommon";
    case CardRarity::Rare:
        return "Rare";
    }
    return "";
}
