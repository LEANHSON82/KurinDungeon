#pragma once
#include <cstdint>
#include <random>
#include <vector>
#include "card.h"

class Deck
{
public:
    // Actions
    void Init(const std::vector<CardId> &cards, std::uint32_t seed);
    void DrawCards(int count);
    void DiscardPlayedCard(int handIndex);
    void DiscardHand();
    bool IsValidHandIndex(int handIndex) const;

    // Read only
    const std::vector<CardId> &GetDrawPile() const { return drawPile; }
    const std::vector<CardId> &GetHand() const { return hand; }
    const std::vector<CardId> &GetDiscardPile() const { return discardPile; }

private:
    void Shuffle();

    // Piles
    std::vector<CardId> drawPile;
    std::vector<CardId> hand;
    std::vector<CardId> discardPile;

    // RNG
    std::mt19937 rng;
};
