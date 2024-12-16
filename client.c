#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#include "message.h"
#include "socket.h"
#include "blackjack.h" 


/**
 * Takes in a suite and the value of the card and edits the respective text file, the output prints the card with the correct values. 
 */
// void editCard(int suite, char cardValue) {

//     FILE* filePtr;
//     int fileSize;

//     // open correct file regarding the suite
//     if (suite == 1) {
//         filePtr = fopen("ascii_art/ascii-Heart.txt", "r");
//     }
//     if (suite == 2) {
//         filePtr = fopen("ascii_art/ascii-Spade.txt", "r");
//     }
//     if (suite == 4) {
//         filePtr = fopen("ascii_art/ascii-Club.txt", "r");
//     }
//     if (suite == 3) {
//         filePtr = fopen("acsii_art/ascii-Diamond.txt", "r");
//     }
//     else {
//         if (filePtr == NULL) {
//             perror("Error opening file");
//         }
//     }

//     // Obtain the file size
//     fseek(filePtr, 0L, SEEK_END);
//     fileSize = ftell(filePtr);
//     rewind(filePtr);

//     char buffer[fileSize+1];

//     // load file into memory
//     for (int i = 0; i < fileSize; i++) {
//       buffer[i] = fgetc(filePtr);  
//     }

//     // change the value of the card in accordance to the vlaue parameter
//     int j = 0;
//     while (j < fileSize) {
//         if (buffer[j] == 'K') {
//             buffer[j] = cardValue;
//         }
//         if (buffer[j] == 'A') {
//             buffer[j] = cardValue;
//         }
//         if (buffer[j] == 'T') {
//             buffer[j] = cardValue;
//         }
//         if (buffer[j] == 'G') {
//             buffer[j] = cardValue;
//         }
//         j++;
//     }
//     // prints out the card 
//     for (int k = 0; k < fileSize; k++) {
//       printf("%c", buffer[k]); 
//     }

//     fclose(filePtr);
// }
/*

connects to server
is dealt two cards from the server
is sent a message telling top card of dealer
sends hit or stayy to server (0 or 1?)
after stay, messages sent from computer about what card it picks up
highest score under 21 wins

*/

/*
Takes in the message from the server and converts it into two strings to be passed
to the function that will draw the cards with ascii
*/
void receive_card(char* card)
{
  int cardInt = atoi(card);
  card_t drawnCard;
  drawnCard.digit = index_to_digit(cardInt);
  drawnCard.suit = index_to_suit(cardInt);
  char suitStr[20];
  char digitStr[20];
  //convert the int to a string for the ascii art
  sprintf(suitStr, "%d", drawnCard.suit);
  sprintf(digitStr, "%d", drawnCard.digit);

  //printf("%d \n", index_to_digit(cardInt));
  //editCard(suitStr, digitStr);
}

/*
Receives a message from the server
*/
char* client_receive(int socket_fd)
{ 
   char * card = receive_message(socket_fd);
      if (card == NULL) {
        perror("Failed to read card from server");
        exit(EXIT_FAILURE);
      } 
      return card;
}

char* client_receive_both(int socket_fd)
{ 
   char * card = receive_message(socket_fd);
      if (card == NULL) {
        perror("Failed to read card from server");
        exit(EXIT_FAILURE);
      } 
      char * m = receive_message(socket_fd);
    printf("Total = %s\n", m);
    // priin the received total

      return card;
}

int play(int socket_fd) {

// Receive the computer cards (sent first) //Read a message from the server
    receive_card(client_receive(socket_fd));
    client_receive(socket_fd);
    receive_card(client_receive(socket_fd));
    client_receive(socket_fd);
    receive_card(client_receive(socket_fd));
    client_receive(socket_fd);
    receive_card(client_receive_both(socket_fd));
    client_receive(socket_fd);
    
     while(1){
    // receive an initial "hit or stay" message from the comp
    char* m = client_receive(socket_fd);
    printf("check%s", m);

    // read in client's choice
    char * userinput;
    size_t n;
    getline(&userinput,&n,stdin);
  //If the user wants another card
    if (strcmp(userinput, "Hit\n") == 0) {
  //Send over whether the user wants to hit or stay to the server
      int rc = send_message(socket_fd, userinput);
      if (rc == -1) {
        perror("Failed to send 'Hit' to server");
        exit(EXIT_FAILURE);
      }
      // and then we want to receive a card from server
      //char* m = client_receive(socket_fd);
      //printf("%s", m);
       // and then we want to receive a card from server and display in ascii

      receive_card(client_receive_both(socket_fd));

      char* update = receive_message(socket_fd);
      printf("%s", update);



      //free(card);

      continue; 
    }
    //If the user is done
    else if (strcmp(userinput, "Stay\n") == 0) {
      //printf("Got to stay part of client");
      //Tell the server we are staying
      int rc = send_message(socket_fd, userinput);
      if (rc == -1) {
        perror("Failed to send 'STAY' to server");
        exit(EXIT_FAILURE);
      }

      // char* final = client_receive(socket_fd);
      // printf("%s", final);
      break;
    }
    else {
      //printf("invalid input \n");
      int rc = send_message(socket_fd, userinput);
      if (rc == -1) {
        perror("Failed to send invalid input to server");
        exit(EXIT_FAILURE);
      }
      continue;
    }
    //  * Draw card -> strcmp(userinput, "draw\n") == 0
    //  * Stay (relinquish control) -> strcmp(userinput, "stay\n") == 0
    //  * total card count ( this might just be kept track of by server)
    //  * 
    //  */

   
  /*
    if(strcmp(final, "You Won! :)") == 0){
        break; // change this to updating count of wins +1
    } else if (strcmp(final, "You have lost this game\n") == 0){
        break; // change this to updating loss count +1
    }
    */

    
    // The message received from the server will be:
    /**
     * Card drawn (update your hand)
     * Card drawn by others
     *      Update the displ
y to include a new card image     * Whether you've won or lost
     * 
     */

    // Free the message
    //free(message);

    
  } // while  
  
  // typing "Stay" brings you out the while loop, now you're waiting for the computer/server to play their turn
   //Read a message from the server
    char* final = client_receive(socket_fd);
      printf("%s\n", final);


  // Close socket
  //close(socket_fd);

  return 0;
}



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
  close(socket_fd);
  return 0;
}