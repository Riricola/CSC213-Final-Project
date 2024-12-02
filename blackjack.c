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

#include "message.h"
#include "socket.h"

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
    int Id; //player ID (aka client id)
    int luck; //keeps track of advantages/disadvantages
    hand_t phand; //the players hand
    bool stay;  // did the player stay or did they hit? Scheduler checks this, if TRUE, skip players turn
} player_t;


card_t deck[52];

//Array that holds the players
player_t players[4];

//Global that makes array spot different each time
//Might have to put a lock around this?
int playNumber;


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
 void player_init(player_t player, void* playerId) {
    int client_socket_fd = * (int*)playerId;
    player.score = 0;
    player.Id = client_socket_fd;
    player.luck = 0;
    hand_init(player);
    //Add the player to the array of players
    players[playNumber] = player;
    //increment playnumber so that next player gets sent to next spot in array
    playNumber++;
 }         
 

/**
    Draw cards
 */
card_t draw_cards(int playerId){
  // Seed the random number generator with a known starting point
  srand(time(NULL)); // 
  int pick = rand() % 52;
    while (deck[pick].picked == true) {
        pick = (pick + 1) % 52;
    } // while
    
    deck[pick].picked = true;
    card_t drawnCard;
    drawnCard.digit = pick/4 + 1;
    drawnCard.suit = pick % 4 + 1;
  // after drawing a card, print it so we can see if it is random each time
  printf("%d \n", deck[pick].digit);

  // add the card to the player's hand total
  //player.phand.total += deck[pick].digit;
  //card[] card_hand = player.phand.cards
  // 
  return drawnCard;
}

void print_deck(){
    for(int i = 0; i < 52; i++){
        printf("%d, ", deck[i].digit);
    
        if (deck[i].picked == true) {
            printf("picked ");
        }
    
    }
}

void * handle_connection(void* arg) {
  int client_socket_fd = * (int*)arg;
  free(arg);
  
  typedef struct startingHand{
    card_t card1;
    card_t card2;
  } startingHand_t;

  startingHand_t message;

  message.card1 = draw_cards(client_socket_fd);
  message.card2 = draw_cards(client_socket_fd);

int nbytes;

// Send a struct to the client containing their starting hand
if ((nbytes = write(client_socket_fd, &message, sizeof(startingHand_t)) != sizeof(message)))
{
  printf("Error writing my message");
}

  while (1) {
    // Read a message from the client
    char* cmessage = receive_message(client_socket_fd);
    if (cmessage == NULL) {
      perror("Failed to read message from client");
      break;
    }
    if(strcmp(cmessage, "stay") == 0)
    {
        break;
    }
    else if (strcmp(cmessage, "hit") == 0)
    {
        for(int i = 0; i < 4; i++)
        {
            if(players[i].Id == client_socket_fd)
            {
              players[i].phand
            }
        }
        draw_cards(client_socket_fd);
        continue;
    }
    else {
        char * m = "Please enter a valid result: Either 'hit' or 'stay'";
        int rc = send_message(client_socket_fd, m);
        if (rc == -1) {
            perror("Failed to send message to client");
            continue;
        }
    }
  }


  close(client_socket_fd);
  return NULL;
}



int main(){
  // Open a server socket
  unsigned short port = 0;
  int server_socket_fd = server_socket_open(&port);
  if (server_socket_fd == -1) {
    perror("Server socket was not opened");
    exit(EXIT_FAILURE);
  }

  // Start listening for connections, with a maximum of one queued connection
  if (listen(server_socket_fd, 1)) {
    perror("listen failed");
    exit(EXIT_FAILURE);
  }

  while(1) {
    printf("Server listening on port %u\n", port);
  
    // Wait for a client to connect
    int client_socket_fd = server_socket_accept(server_socket_fd);
    if (client_socket_fd == -1) {
      perror("accept failed");
      break;
    }

    printf("Client connected!\n");

    pthread_t thread;

    int * threadarg = malloc(sizeof(int));
    *threadarg = client_socket_fd;

    pthread_create(&thread, NULL, handle_connection, threadarg);    
  }

  

  // Close sockets
  close(server_socket_fd);

  return 0;




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

