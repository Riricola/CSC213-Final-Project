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
#include "blackjack.h"

#define NUM_SUITS 4
#define NUM_CARDS 13

#define HEART 1
#define SPADE 2
#define DIAMOND 3
#define CLUB 4

//Computer resides at index 0
//int playNumber = 1;

card_t deck[52];

//Array that holds the players
//player_t players[5];


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
  hand_t hand_init ()
  {
    hand_t phand;
    phand.total = 0;
    phand.num_cards = 0;
    phand.ace = 0;
    return phand;
  }

/**
    Initalize player. 
 */
 player_t player_init(int socket) {
    player_t player;
    player.score = 0;
    player.Id = socket;
    player.luck = 0;
    player.phand = hand_init();
    printf("player %d hand total on initialize : %d\n", player.Id, player.phand.total);

    //Add the player to the array of players
    //players[playerNum] = player;
    //increment playnumber so that next player gets sent to next spot in array
    //playNumber++;
    //printf("success initializing player");
    return player;
    
 }         

/*
Send a card to the client
*/
void client_card(card_t card, int clientId)
{
   char m[20];
    sprintf(m, "%d", card_to_index(card));
    int rc = send_message(clientId, m);
    if (rc == -1) {
        perror("Failed to send card to client");
        //continue;
    }

}

/*
Send a message to the client
*/
void client_message(char* m, int clientId)
{
    int rc = send_message(clientId, m);
    if (rc == -1) {
        perror("Failed to send message to client");
        //continue;
    }

}
  
/**
    Draw cards
 */
card_t draw_cards(){
  // Seed the random number generator with a known starting point
  srand(time(NULL)); // 
  int pick = rand() % 52;
    while (deck[pick].picked == true) {
        pick = rand() % 52;
    } // while
    
    deck[pick].picked = true;
    card_t drawnCard;
    drawnCard.digit = index_to_digit(pick);
    drawnCard.suit = index_to_suit(pick);
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

void update_hand(player_t player, card_t newcard)
{
  /**/
  //for(int i = 0; i < 4; i++)
  //{
    //if(players[i].Id == clientId)
     //{
      //Increment the number of cards that the client has by one
      player.phand.num_cards++;
      //Check if they drew an ace and increment this value
      if(newcard.digit == 1)
      {
        player.phand.ace++;
      }
      //Calculate their new hand total
      printf("hand: %d, digit %d \n", player.phand.total, newcard.digit);

      player.phand.total += newcard.digit;
      printf("player %d total = %d \n", player.Id, player.phand.total);

      //Add the new card to the players hand
      //player.phand.total = total;

      //If they bust, tell them
      if (player.phand.total > 21)
      {
        char * m = "You have lost this game";
        int rc = send_message(player.Id, m);
        if (rc == -1)
        {
          perror("Failed to send message to client");
          //continue;
        }
        close(player.Id);
      }

      //Check if they are under 21 and have 5 or more cards: automatic win
      else {
        if(player.phand.num_cards > 4)
        {
        char * m = "You have won this game by the charlie rule";
        int rc = send_message(player.Id, m);
        if (rc == -1)
        {
          perror("Failed to send message to client");
          //continue;
        }
        close(player.Id);
        }
      }
              
      }
    //}
//}

void * handle_connection(void* arg) {
  int client_socket_fd = * (int*)arg;
  free(arg);

  //Initialize the computer player first
  int Id = 0;
  player_t computer = player_init(Id);
  //hand_init(computer);
  printf("starting hand total 1: %d\n", computer.phand.total);

  //Initialize the client player
  player_t player1 = player_init(client_socket_fd);
  //hand_init(player1);
  printf("starting hand total 2: %d\n", player1.phand.total);

  //Draw two cards for the computer
  card_t compcard1 = draw_cards();
  card_t compcard2 = draw_cards();

    //Draw two cards for the player
  card_t card1 = draw_cards();
  card_t card2 = draw_cards();

  printf("check");


  //Call the function to update the computer's hand
  update_hand(computer, compcard1);
  update_hand(computer, compcard2);
  
    //Call the function to update the players hand
  update_hand(player1, card1);
  update_hand(player1, card2);

  //Send the computer's cards over to the client - for ascii art
  client_card(compcard1, client_socket_fd);
  client_card(compcard2, client_socket_fd);

  //Send the player's cards over to the client - for ascii art
  client_card(card1, client_socket_fd);

   printf("check2");
  client_card(card2, client_socket_fd);


while (1) {
  char * m = "Please choose to Hit or Stay and type your response below\n";
  client_message(m, client_socket_fd);

  // Read a message from the client
  char* cmessage = receive_message(client_socket_fd);
  /*
  if (cmessage == NULL) 
  {
    perror("Failed to read message from client");
    break;
  }
  */
  
  if(strcmp(cmessage, "Stay\n") == 0)
  {
    //The player is done, exit the while loop
    printf("start of stay");    /// <---- WE GET HERE BUT IT DOESNT PRINT

    // hit or stay, then send the total cards to client
    //At this point the user has chosen to stay and it is the computers turn
    

    //m = "suprise shawtyyyy \n";
    //client_message(m, client_socket_fd);

    

    break;
  }
  else if (strcmp(cmessage, "Hit\n") == 0)
  {
    //printf("Got to hit area of while loop");
      // update hand
      card_t newcard = draw_cards();
      printf("card drawn: %d\n", newcard.digit);
      update_hand(player1, newcard);
      client_card(newcard, client_socket_fd);
      continue;
    
  } // else if 
  else 
  {
    //char * m = "Invalid response entered\n";
    //client_message(m, client_socket_fd);
    continue;
  } // else
} // while



printf("end of while");
//At this point the user has chosen to stay and it is the computers turn
/*
for(int i = 1; i <= 4; i++)
{
 if(players[i].Id == client_socket_fd)
  { 
  int playerTotal = players[i].phand.total;
  //If the computer already has a better hand than the player
  if(players[0].phand.total > playerTotal && players[0].phand.total <= 21)
  {
    char * m = "You have lost this game\n";
    client_message(m, client_socket_fd);
    close(client_socket_fd);
    return NULL;
  }
  //Keep having the computer hitting until it wins or goes over 21
  while (players[0].phand.total < 21 && players[0].phand.total < players[i].phand.total)
  {
    card_t c = draw_cards();
    update_hand(0, c);
  }
  //Check if the computer won
  if(players[0].phand.total <= 21)
  {
    char * m = "You have lost this game\n";
    client_message(m, client_socket_fd);
    close(client_socket_fd);
    return NULL;
  }
  else {
    char * m = "You have won this game\n";
    client_message(m, client_socket_fd);
    close(client_socket_fd);
    return NULL;
  }
  }
   else if(i == 4)
   {
      close(client_socket_fd);
         return NULL;
   } 
  }
  */
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

  //int playNumber = 1;
  while(1) {
    printf("Server listening on port %u\n", port);
  
    // Wait for a client to connect
    int client_socket_fd = server_socket_accept(server_socket_fd);
    if (client_socket_fd == -1) {
      perror("accept failed");
      //break;
    }

    printf("Client connected!\n");


    pthread_t thread;

    int * threadarg = malloc(sizeof(int));
    *threadarg = client_socket_fd;

    pthread_create(&thread, NULL, handle_connection, threadarg);    
  }

//Close sockets
  close(server_socket_fd);

  return 0;
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

