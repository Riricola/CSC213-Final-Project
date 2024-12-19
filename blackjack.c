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

//Holds the deck of cards
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
            if (i > 9) {
              deck[current].digit = 10;
            }
            else {
              deck[current].digit = i; //initializes card number
            }
            deck[current].picked = false; 
            current++;
        }
    }
}

 /**
    Initialize hand and return it
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
    Initalize player and return it
 */
 player_t player_init(int socket, int playerNum) {
    player_t player;
    player.score = 0;
    player.Id = playerNum; //to keep track of computer vs. player
    player.socket = socket;
    player.phand = hand_init(); //call the function to initialize players hand
    return player;
 }         

/*
Send a message to the client - includes error checking
*/
void client_message(char* m, int clientId)
{
    int rc = send_message(clientId, m);
    if (rc == -1) {
        perror("Failed to send message to client");
        //continue;
    }

} // client message

/*
Send a card to the client
*/
void client_card(card_t card, int clientId)
{
    char m[2];
    //convert card integer to string
    sprintf(m,"%d",card.digit);
    int rc = send_message(clientId, m);
    if (rc == -1) {
        perror("Failed to send card to client");
        //continue;
    }

}

/*
Send a message with a players total and a card in one function
*/
void card_message(card_t card, int clientId, player_t player)
{
  //send over the card first
    char m[2];
    sprintf(m,"%d",card.digit);
    int rc = send_message(clientId, m);
    if (rc == -1) {
        perror("Failed to send card to client");
        //continue;
    }
    //send a message containing the player's current total
       char total[20];
    sprintf(total, "%d", player.phand.total);
    int rb = send_message(clientId, total);
    if (rb == -1) {
        perror("Failed to send message to client");
        //continue;
    }


}

/**
    Draw cards
 */
card_t draw_cards(){
  // Seed the random number generator with a known starting point
  srand(time(NULL)); 
  int pick = rand() % 52;
    while (deck[pick].picked == true) {
        pick = rand() % 52; //pick a random number between 0 and 51
    } // while
    
    deck[pick].picked = true; //set picked value to true
    card_t drawnCard;
    drawnCard.digit = index_to_digit(pick); //calculate the digit
    drawnCard.suit = index_to_suit(pick); // calculate the suit
  // after drawing a card, print it so we can see if it is random each time
  //printf("%d \n", deck[pick].digit);

  return drawnCard;
}

/**
  Prints the deck of cards - used for testing
 */
void print_deck(){
    for(int i = 0; i < 52; i++){
        printf("%d, ", deck[i].digit);
    
        if (deck[i].picked == true) {
            printf("picked ");
        }
    
    }
}

/**
    Uodates the hand variables in the player's hand struct - calculates their total after card drawn
 */
void update_hand(player_t* player, card_t newcard)
{
      //Increment the number of cards that the client has by one
      player->phand.num_cards++;
      //Check if they drew an ace and increment this value
      if(newcard.digit == 1)
      {
        player->phand.ace++;
      }
      
      player->phand.total += newcard.digit;
      //Print the player's total to see if our game is working properly
      printf("player %d total = %d \n", player->Id, player->phand.total);

      //Inform the player to stay if they have reached exactly 21
       if(player->phand.total == 21 && player->Id != 0){
        char * m = "You scored 21! 'STAY' for the Computer to play their turn. \n";
        client_message(m, player->socket);
      }

        //Need to send message to player for uniformity
        else {
          char * m = "";
        client_message(m, player->socket);
        }
      } //update hand

 
/*
  Function that the server thread runs - send cards to player, receives input if they want to hit
  or stay, and tells them if they have won or lost
*/
void * handle_connection(void* arg) {
  int client_socket_fd = * (int*)arg;
  free(arg);

  //Initialize the computer player first
  int compNum = 0;
  int playerNum = 1;
  player_t computer = player_init(client_socket_fd, compNum);
  //printf("starting hand total 1: %d\n", computer.phand.total);

  //Initialize the client player
  player_t player1 = player_init(client_socket_fd, playerNum);
  //printf("starting hand total 2: %d\n", player1.phand.total);

  //Draw two cards for the computer
  card_t compcard1 = draw_cards();
  card_t compcard2 = draw_cards();

    //Draw two cards for the player
  card_t card1 = draw_cards();
  card_t card2 = draw_cards();

  //Call the function to update the computer's hand
  update_hand(&computer, compcard1);
  update_hand(&computer, compcard2);
  
    //Call the function to update the players hand
  update_hand(&player1, card1);
  update_hand(&player1, card2);

  //Send the computer's cards over to the client - for ascii art
  client_card(compcard1, client_socket_fd);
  client_card(compcard2, client_socket_fd);

  //Send the player's cards over to the client - for ascii art
  client_card(card1, client_socket_fd);
  card_message(card2, client_socket_fd, player1);

//Receives user input and gives them a card or stays while they have less than 21 points
while (player1.phand.total <= 21) {

  char * m = "Please choose to Hit or Stay and type your response below\n";
  client_message(m, client_socket_fd);

  // Read a message from the client
  char* cmessage = receive_message(client_socket_fd);
  //printf("Client chose %s \n", cmessage);
  if (cmessage == NULL) 
  {
    perror("Failed to read message from client");
    break;
  }
  
  if(strcmp(cmessage, "Stay\n") == 0)
  {
    //The player is done, exit the while loop

    break;
  }
  else if (strcmp(cmessage, "Hit\n") == 0)
  {
      // update hand
      card_t newcard = draw_cards();
      //Print the card to make sure things are working as expected
      printf("card drawn: %d\n", newcard.digit);
      update_hand(&player1, newcard);
      //send the card to the client
      card_message(newcard, client_socket_fd, player1);
      continue;
    
  }
  else 
  {
    //Will go to top of the loop and prompt them to type hit or stay again
    continue;
  }
   // else
} // while


//At this point the user has chosen to stay and it is the computers turn
//Print for error checking
   printf("computer total: %d \n", computer.phand.total);
  //If the player has won by having 5 or more cards
  if(player1.phand.num_cards >= 5){
    char * m = "You have won this game by the charlie rule. \n";
        client_message(m, client_socket_fd);
        return NULL;
  }
  //If the client is over 21
  if(player1.phand.total > 21) {
    char * m = "You have lost this game by busting\n";
    client_message(m, client_socket_fd);
    return NULL;
  }
    //If the computer already has a better hand than the player
  else if((computer.phand.total > player1.phand.total) && (computer.phand.total <= 21))
  {
    char * m = "You have lost this game by being outscored by the dealer\n";
    client_message(m, client_socket_fd);
    return NULL;
  }
  //Keep having the computer hitting until it wins or goes over 21
  while ((computer.phand.total < 21) && (player1.phand.total < 21) && (computer.phand.total < player1.phand.total))
  {
    card_t c = draw_cards();
    //Printing to ensure things run correctly
    printf("computer drew: %d \n", c.digit);
    update_hand(&computer, c);
    //send the client the total so it can be printed for them
    client_card(c, client_socket_fd);
  }
  //Check if the computer won
  if(computer.phand.total <= 21)
  {
    char * m = "You have lost this game in the clutch by being outscored by the dealer\n";
    client_message(m, client_socket_fd);
    return NULL;
  }
  //In the case of a tie, dealer wins
  else if(computer.phand.total == player1.phand.total)
  {
    char * m = "You have lost this game in the clutch by being outscored by the dealer\n";
    client_message(m, client_socket_fd);
    return NULL;
  }
  //If computer goes over 21, the player wins
  else {
    char * m = "You have won in the clutch by the dealer busting\n";
    client_message(m, client_socket_fd);
    return NULL;
  }
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
    pthread_detach(thread);
  }

//Close sockets
  close(server_socket_fd);

  return 0;
} 
