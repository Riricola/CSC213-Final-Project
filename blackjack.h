#pragma once


#include <stdio.h>
#include <stdlib.h>


typedef struct card {
    int suit;   // suit of the card
    int digit;  // card value (also used to caluclates points)
    bool picked;    // has the card been picked?
} card_t;

typedef struct hand {
    int total;   // total points of the hand, must be <21 to not bust
    int num_cards;  // Using the Charlie rule, if num == 5, player automatically wins 
    int ace;    // how many aces does the hand contain

    // might remove later
    card_t cards[5];
} hand_t;

typedef struct player {
    int score; // how many games won
    int socket; //aka client id
    int Id;  //player number
    hand_t phand; //the players hand
    bool stay;  // did the player stay or did they hit? Scheduler checks this, if TRUE, skip players turn
} player_t;

typedef struct startingHand{
    card_t card1;
    card_t card2;
  } startingHand_t;

/**
 Function that takes the index of a card and converts it to its number value
  */
  int index_to_digit(int index)
  {
    return index/4 + 1;
  }

  /**
 Function that takes the index of a card and converts it to its suit
  */
   int index_to_suit(int index)
  {
     return index % 4 + 1;
  }

/**
Function that takes a card and finds its index in array
 */
  int card_to_index(card_t card)
  {
    return ((card.digit -1) * 4) + (card.suit - 1);
  }
