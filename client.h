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
    int Id; //player ID (aka thread id)
    int luck; //keeps track of advantages/disadvantages
    hand_t phand; //the players hand
    bool stay;  // did the player stay or did they hit? Scheduler checks this, if TRUE, skip players turn
} player_t;