#include "deck.h"
#include <algorithm>

void Deck::Init(const std::vector<CardId> &cards, std::uint32_t seed)
{
    rng.seed(seed);
    drawPile = cards;
    hand.clear();
    discardPile.clear();
    exhaustPile.clear();
    Shuffle();
}

void Deck::DrawCards(int count)
{
    for (int i = 0; i < count; i++)
    {
        if (static_cast<int>(hand.size()) >= maxHandSize)
        {
            return;
        }

        if (drawPile.empty())
        {
            if (discardPile.empty())
            {
                return;
            }

            drawPile.swap(discardPile);
            Shuffle();
        }

        hand.push_back(drawPile.back());
        drawPile.pop_back();
    }
}

void Deck::RemovePlayedCard(int handIndex, bool exhaust)
{
    if (!IsValidHandIndex(handIndex))
    {
        return;
    }

    if (exhaust)
    {
        exhaustPile.push_back(hand[handIndex]);
    }
    else
    {
        discardPile.push_back(hand[handIndex]);
    }
    hand.erase(hand.begin() + handIndex);
}

void Deck::DiscardHand()
{
    discardPile.insert(discardPile.end(), hand.begin(), hand.end());
    hand.clear();
}

bool Deck::IsValidHandIndex(int handIndex) const
{
    return handIndex >= 0 && handIndex < static_cast<int>(hand.size());
}

void Deck::Shuffle()
{
    std::shuffle(drawPile.begin(), drawPile.end(), rng);
}
