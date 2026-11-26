#pragma once
#include <cstdint>
#include <random>
#include <vector>
#include "card.h"

const int maxHandSize = 10;

class Deck
{
public:
    void Init(const std::vector<CardId> &cards, std::uint32_t seed);
    void DrawCards(int count);
    void RemovePlayedCard(int handIndex, bool exhaust);
    void DiscardHand();
    bool IsValidHandIndex(int handIndex) const;

    const std::vector<CardId> &GetDrawPile() const { return drawPile; }
    const std::vector<CardId> &GetHand() const { return hand; }
    const std::vector<CardId> &GetDiscardPile() const { return discardPile; }
    const std::vector<CardId> &GetExhaustPile() const { return exhaustPile; }

private:
    void Shuffle();

    std::vector<CardId> drawPile;
    std::vector<CardId> hand;
    std::vector<CardId> discardPile;
    std::vector<CardId> exhaustPile;

    std::mt19937 rng;
};
