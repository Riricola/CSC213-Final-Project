/*
Authors:
    Grace Charron, Connor Heagy, Noah Mendola, Maria Rodriguez
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <time.h>

// #include "message.h"
// #include "socket.h"

#define NUM_SUITS 4
#define NUM_CARDS 13

#define HEART 1
#define SPADE 2
#define DIAMOND 3
#define CLUB 4



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


card_t deck[52];


/**
    Initialize the deck. Puts the cards in order from ace to king, with
    hearts, spades, diamonds, and clubs in that order
 */
void deck_init () {
    int current = 0;
    for (int i = 1; i <= NUM_CARDS; i++) {
        for (int j = 1; j <= NUM_SUITS; j++) {
            deck[current].suit = j; //initializes suit
            deck[current].digit = i; //initializes card number
            deck[current].picked = false; 
            current++;
        }
    }
}

 /**
    Initialize hand
  */
  void hand_init (player_t player)
  {
    player.phand.total = 0;
    player.phand.num_cards = 0;
    player.phand.ace = 0;
    
  }

/**
    Initalize player. 
 */
 void player_init(player_t player) {
    player.score = 0;
    //player.Id = pthread_self();
    player.luck = 0;
    hand_init(player);
    
 }         
 

/**
    Draw cards
 */
void draw_cards(player_t player){
  // Seed the random number generator with a known starting point
  srand(time(NULL)); // 
  int pick = rand() % 52;
    while (deck[pick].picked == true) {
        pick = (pick + 1) % 52;
    } // while
    
    deck[pick].picked = true;
  // after drawing a card, print it so we can see if it is random each time
  printf("%d \n", deck[pick].digit);

  // add the card to the player's hand total
  player.phand.total += deck[pick].digit;
  //card[] card_hand = player.phand.cards
  // 
}

void print_deck(){
    for(int i = 0; i < 52; i++){
        printf("%d, ", deck[i].digit);
    
        if (deck[i].picked == true) {
            printf("picked ");
        }
    
    }
}

int main(){
    deck_init();
    draw_cards();
    draw_cards();
    draw_cards();
        draw_cards();
    draw_cards();
    draw_cards();
    print_deck();

} 


/* MIGHT NOT USE:

*   Calculates the total of the players hand

    Parameters:
    player_t* player's hand

    Returns:
    total; coounts the val of each card
 */
 /*
int count_point(tlayerhread_t* pla
    int total = 0;yer){
    for(int i = 0; i <= player.phand.num_cards; i++){
        total += 
        
    }
    
}
*/

// optional:
//  This distributes advantages and disadvantages
//void distribute(){
    
//}

/** NOTES:

Networking
    Players play on their own computers. When they encounter a “card game” scenario, 
    they are put in a room where, using networking, they wait for another player to 
    connect. Both players don’t need to be at the same point in the story to play together; 
    they simply have to be in the room within 30-50 seconds of the other entering. If the 
    player waits for more than 50 seconds, the computer will take over and play against them. 

    
 */
