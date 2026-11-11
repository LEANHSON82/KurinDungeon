#include "deck.h"
#include <algorithm>

// Setup
void Deck::Init(const std::vector<CardId> &cards, std::uint32_t seed)
{
    rng.seed(seed);
    drawPile = cards;
    hand.clear();
    discardPile.clear();
    Shuffle();
}

// Draw cards
void Deck::DrawCards(int count)
{
    for (int i = 0; i < count; i++)
    {
        if (drawPile.empty())
        {
            if (discardPile.empty())
            {
                return;
            }

            // Recycle
            drawPile.swap(discardPile);
            Shuffle();
        }

        // Move to hand
        hand.push_back(drawPile.back());
        drawPile.pop_back();
    }
}

// Discard card
void Deck::DiscardPlayedCard(int handIndex)
{
    if (!IsValidHandIndex(handIndex))
    {
        return;
    }

    discardPile.push_back(hand[handIndex]);
    hand.erase(hand.begin() + handIndex);
}

// Discard hand
void Deck::DiscardHand()
{
    discardPile.insert(discardPile.end(), hand.begin(), hand.end());
    hand.clear();
}

// Validate index
bool Deck::IsValidHandIndex(int handIndex) const
{
    return handIndex >= 0 && handIndex < static_cast<int>(hand.size());
}

// Shuffle
void Deck::Shuffle()
{
    std::shuffle(drawPile.begin(), drawPile.end(), rng);
}
