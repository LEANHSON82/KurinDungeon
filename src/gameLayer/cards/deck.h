#pragma once
#include <random>
#include <vector>
#include "card.h"

class Deck
{
public:
    void Init(const std::vector<CardId> &cards);
    void DrawCards(int count);
    void DiscardPlayedCard(int handIndex);
    void DiscardHand();
    bool IsValidHandIndex(int handIndex) const;

    const std::vector<CardId> &GetDrawPile() const { return drawPile; }
    const std::vector<CardId> &GetHand() const { return hand; }
    const std::vector<CardId> &GetDiscardPile() const { return discardPile; }

private:
    void Shuffle();

    std::vector<CardId> drawPile;
    std::vector<CardId> hand;
    std::vector<CardId> discardPile;
    std::mt19937 rng{std::random_device{}()};
};
