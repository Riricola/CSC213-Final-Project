#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#include "message.h"
#include "socket.h"
#include "blackjack.h" 

void receive_card(char* card)
{
  int cardInt = atoi(card);
  card_t drawnCard;
  drawnCard.digit = cardInt/4 + 1;
  drawnCard.suit = cardInt % 4 + 1;
  char suitStr[20];
  char digitStr[20];
  // convert int value to string later for connors print cards cuntion
  //sprintf(suitStr, %d, drawnCard.suit);
  //sprintf(digitStr, %d, drawnCard.digit);
  //editCard(suitStr, digitStr);
  
}


int play(int socket_fd) {
  
  // the 2 receives below will become a function^

  // Receive the first two cards
  char * card = receive_message(socket_fd);
      if (card == NULL) {
        perror("Failed to read card from server");
        exit(EXIT_FAILURE);
      } 
  
  // and receive the computers cards

  while (1) {
    // Read what the user types

    printf("Please enter 'Hit' or 'Stay\n");
    char * userinput;
    size_t n;
    getline(&userinput,&n,stdin);

    if (strcmp(userinput, "Hit\n") == 0) {
  
      //Send over whether the user wants to hit or stay to the server
      int rc = send_message(socket_fd, userinput);
      if (rc == -1) {
        perror("Failed to send message to server");
        exit(EXIT_FAILURE);
      }
      // and then we want to receive a card from server
      char * card = receive_message(socket_fd);
      if (card == NULL) {
        perror("Failed to read card from server");
        exit(EXIT_FAILURE);
      } 
      //change to ascii?
      printf("%s\n", card);
      // free the card
      free(card);
      continue; 
    }

    else if (strcmp(userinput, "Stay\n") == 0) {
      int rc = send_message(socket_fd, userinput);
      if (rc == -1) {
        perror("Failed to send message to server");
        exit(EXIT_FAILURE);
      }
      break;
    }

    else {
      printf("Not a valid input. Remember to use uppercases");
    }

    // Send a message to the server

    /** Messages to SEND:
     * 
     * Draw card -> strcmp(userinput, "draw\n") == 0
     * Stay (relinquish control) -> strcmp(userinput, "stay\n") == 0
     * total card count ( this might just be kept track of by server)
     * 
     */

    // Read a message from the server
    char* message = receive_message(socket_fd);
    if (message == NULL) {
      perror("Failed to read message from server");
      exit(EXIT_FAILURE);
    }

    if(strcmp(message, "You Won! :)") == 0){
        break; // change this to updating count of wins +1
    } else if (strcmp(message, "You Lost... :(") == 0){
        break; // change this to updating loss count +1
    }

    
    // The message received from the server will be:
    /**
     * Card drawn (update your hand)
     * Card drawn by others
     *      Update the displ
y to include a new card image     * Whether you've won or lost
     * 
     */

   
    // Free the message
    free(message);
  }

  // Close socket
  close(socket_fd);

  return 0;
}



/**
 * Takes in a suite and the value of the card and edits the respective text file, the output prints the card with the correct values. 
 */
void editCard(char suite, char cardValue) {

    FILE* filePtr;
    int fileSize;

    // open correct file regarding the suite
    if (suite == 'h') {
        filePtr = fopen("ascii_art/ascii-Heart.txt", "r");
    }
    if (suite == 's') {
        filePtr = fopen("ascii_art/ascii-Spade.txt", "r");
    }
    if (suite == 'c') {
        filePtr = fopen("ascii_art/ascii-Club.txt", "r");
    }
    if (suite == 'd') {
        filePtr = fopen("acsii_art/ascii-Diamond.txt", "r");
    }
    else {
        if (filePtr == NULL) {
            perror("Error opening file");
        }
    }

    // Obtain the file size
    fseek(filePtr, 0L, SEEK_END);
    fileSize = ftell(filePtr);
    rewind(filePtr);

    char buffer[fileSize+1];

    // load file into memory
    for (int i = 0; i < fileSize; i++) {
      buffer[i] = fgetc(filePtr);  
    }

    // change the value of the card in accordance to the vlaue parameter
    int j = 0;
    while (j < fileSize) {
        if (buffer[j] == 'K') {
            buffer[j] = cardValue;
        }
        if (buffer[j] == 'A') {
            buffer[j] = cardValue;
        }
        if (buffer[j] == 'T') {
            buffer[j] = cardValue;
        }
        if (buffer[j] == 'G') {
            buffer[j] = cardValue;
        }
        j++;
    }
    // prints out the card 
    for (int k = 0; k < fileSize; k++) {
      printf("%c", buffer[k]); 
    }

    fclose(filePtr);
}
/*

connects to server
is dealt two cards from the server
is sent a message telling top card of dealer
sends hit or stayy to server (0 or 1?)
after stay, messages sent from computer about what card it picks up
highest score under 21 wins

*/


int main(int argc, char** argv) {
  if (argc != 3) {
    fprintf(stderr, "Usage: %s <server name> <port>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  // Read command line arguments
  char* server_name = argv[1];
  unsigned short port = atoi(argv[2]);

  // Connect to the server
  int socket_fd = socket_connect(server_name, port);
  if (socket_fd == -1) {
    perror("Failed to connect");
    exit(EXIT_FAILURE);
  }
  
  play(socket_fd);
  return 0;
}