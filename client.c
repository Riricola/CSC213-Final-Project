#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#include "message.h"
#include "socket.h"
#include "blackjack.h" 

int playerTotal = 0;

int computerTotal = 0;

char playerHand[6];

char computerHand[6];

int playerIndex = 0;

int computerIndex = 0;

// global variable for your advantage/disadvantage
int Cond;
// luck = 0
// blind = 1, you start without seeing your cards
// doom = 2, you start with 3 cards

/*
Takes in the message from the server and converts it into two strings to be passed
to the function that will draw the cards with ascii
*/

/*
void global_init () {
playerTotal = 0;
computerTotal = 0;
memset(playerHand, 0, sizeof(char));
memset(computerHand, 0, sizeof(char));
playerIndex = 0;
computerIndex = 0;
}
*/

void receive_card(bool player, char* card)
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

  char cardVal;
  if (cardInt >= 10) {
    cardVal = 'F';
  }
  else {
    cardVal = cardInt + '0';
  }

  //save value into gobal array
  if (player) {
    playerHand[playerIndex] = cardVal;
    printf("Player Array at index %d: %c\n", playerIndex, playerHand[playerIndex]);
    playerIndex += 1;
  }
  else {
    computerHand[computerIndex] = cardVal;
    printf("Dealer Array at index %d: %c\n", computerIndex, computerHand[computerIndex]);
    computerIndex += 1;
  }

  //printf("%d \n", index_to_digit(cardInt));
  //editCard(suitStr, digitStr);
}

void editCardHelper(bool player, char *buf, int index, int j, char p) {
    if ((buf[index] == p) && (player == true)) {
        buf[index] = playerHand[j];
    } 
    else if ((buf[index] == p) && (player == false)) {
        buf[index] = computerHand[j];
    }
}

void editCardHelper2(bool player, char *buf, int index, int j, char p, int playInd) {
    if ((playerIndex == playInd) && (buf[j] == p) && (player = true)) {
        buf[index] = playerHand[j];
    } 
    else if ((playerIndex == playInd) && (buf[index] == p) && (player == false)){
        buf[index] = computerHand[j];
      }
}



//updates and prints the player's hand
void editCard(bool player) {

    FILE* filePtr;
    int fileSize;
    
    // open correct file regarding the suite
    filePtr = fopen("playerHand.txt", "r");
    if (filePtr == NULL) {
        perror("Error opening file");
    }

    // Obtain the file size
    fseek(filePtr, 0L, SEEK_END);
    fileSize = ftell(filePtr);
    rewind(filePtr);

    char buffer[fileSize+1];

    // load file into memory
    for (int i = 0; i < fileSize; i++) {
        char fileChar = fgetc(filePtr);
        buffer[i] = fileChar;
    } // for

    // change the value of the card in accordance to the value parameter
    int j = 0;
    while (j < fileSize) {

      //print out first two cards in the starting round
      editCardHelper(player, buffer, j, 0, '!');

      editCardHelper(player, buffer, j, 1, '@');
      
      //print cards as the user decides to hit or stay
      editCardHelper2(player, buffer, j, 1, '@', 3);
      
      /*if (playerIndex == 4 && (buffer[j] == '#') && (player = true)) {
        buffer[j] = playerHand[3];
      } else {
        buffer[j] = computerHand[3];
      }
      if ((playerIndex == 5) && (buffer[j] == '$') && (player = true)) {
        buffer[j] = playerHand[4];
      } else {
        buffer[j] = computerHand[4];
      }
      if ((playerIndex == 6) && (buffer[j] == '&') && (player = true)) {
        buffer[j] = playerHand[5];
      } else {
        buffer[j] = computerHand[5];
      }
      */
      j++;
    } // while


    // prints out the card 
    for (int k = 0; k < fileSize; k++) {
      printf("%c", buffer[k]); 
    } //for

} //editCard(player, playerArr)


//connects to server
//is dealt two cards from the server
//is sent a message telling top card of dealer
//sends hit or stayy to server (0 or 1?)
//after stay, messages sent from computer about what card it picks up
//highest score under 21 wins

//print ascii art for each round
void updateBoard() {
    
    /**
    Advantage and disadvantage
    
    if cond == Blind 
        Both players and comps cards are hidden UNTIL player does their first hit OR stay (then reveal both the cards)

    if cond == luck
      this shit might change but for now:
        player starts with being able to see both the comps card (so they can make a better decision of if they wanna hit or stay)
        OR (and thsi is easier...)
        Simply display the total of the comp


    */
    // print the dealers hand
    printf("\n                                              DEALER TOTAL: %d\n\n", computerTotal);
    editCard(false);
    printf("\n\n");
    // print the players hand
    printf("\n                                              PLAYER TOTAL : %d\n\n", playerTotal);
    editCard(true); 
    printf("\n\n");
}


/*
Receives a message from the server
*/
char* client_receive(bool player, int socket_fd)
{ 
    if (player) {
      char * card = receive_message(socket_fd);
      if (card == NULL) {
        perror("Failed to read card from server");
        exit(EXIT_FAILURE);
      } 
      int temp = atoi(card);
      playerTotal += temp;
      return card;
    } else {
      char * card = receive_message(socket_fd);
      if (card == NULL) {
        perror("Failed to read card from server");
        exit(EXIT_FAILURE);
      } 
      int temp = atoi(card);
      computerTotal += temp;
      return card;
    }
      
}

char* client_receive_both(bool player, int socket_fd)
{ 
   if (player) {
    char * card = receive_message(socket_fd);
      if (card == NULL) {
        perror("Failed to read card from server");
        exit(EXIT_FAILURE);
      } 
      int temp = atoi(card);
      playerTotal += temp;
      char * m = receive_message(socket_fd);
      printf("Total = %s \n", m);
    // print the received total
      return card;
   } 
   else {
    char * card = receive_message(socket_fd);
      if (card == NULL) {
        perror("Failed to read card from server");
        exit(EXIT_FAILURE);
      } 
      int temp = atoi(card);
      computerTotal += temp;
      char * m = receive_message(socket_fd);
      printf("Total = %s \n", m);
    // print the received total
      return card;
   }
}

int play(int socket_fd) {

// Receive the computer cards (sent first) //Read a message from the server
    char* first = client_receive(false, socket_fd);
    printf("first message: %s\n", first);
    char* second = client_receive(false, socket_fd);
    printf("second message: %s\n", second);
    char* third = client_receive(false, socket_fd);
    printf("third message: %s\n", third);
    char* fourth = client_receive(false, socket_fd);
    printf("fourth message: %s\n", fourth);

    receive_card(false, (client_receive(false, socket_fd)));
    
    receive_card(false, (client_receive(false, socket_fd)));
    
    receive_card(true, (client_receive(true, socket_fd)));
    
    receive_card(true, (client_receive_both(true, socket_fd)));
    
    printf("playtotal = %d \n", playerTotal);
    printf("comptotal = %d \n", computerTotal);
    while(playerTotal <= 21){
      printf("playtotal = %d \n", playerTotal);
      printf("comptotal = %d \n", computerTotal);
      // receive an initial "hit or stay" message from the comp
      char* m = client_receive(true, socket_fd);
      printf("%s\n", m);

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

      updateBoard();
      // and then we want to receive a card from server
      //char* m = client_receive(socket_fd);
      //printf("%s", m);
      // and then we want to receive a card from server and display in ascii
      
      char* update = client_receive(false, socket_fd);
      printf("Update_Hand branch print: %s \n", update);
      receive_card(true, (client_receive_both(true, socket_fd)));

      //receives send from update hand
      
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
  
  //computer is hitting
  while ((playerTotal < 21) && (computerTotal < 21) && (computerTotal < playerTotal)) {
    if(playerIndex == 5 && playerTotal <= 21) {
      break;
    }
    char* update = client_receive(false, socket_fd);
      printf("Update_Hand branch print: %s \n", update);
      receive_card(false, (client_receive(false, socket_fd)));
      
    printf("comptotal = %d \n", computerTotal);
  }
  
   //Read a message from the server
    printf("Trying to read final message \n");
    char* final = client_receive(false, socket_fd);
      printf("%s\n", final);


  // Close socket
  //close(socket_fd);

  return 0;
}


void story(int socket_fd){
  printf("\n Greetings traveler! I'm glad you could regaiin consciousness.\nIn case you're unaware, you're a 3rd year CompSci major at Grinnell. \n" 
  "a. *groggy* 'Uuuughhhghhhh my head hurts. Who are you... what happened?'\n\n"
  "b. 'Computer Science? Ew why'd I choose that field...'\n\n"
  "c. 'OMG I'm a student at Cornell?!'\n");

  int ch = getchar();

  if(ch == '\0'){
    perror("Failed to read choice");
    exit(EXIT_FAILURE);
  } // check if empty


  // Clear the newline character from the buffer
  getchar();

  if(ch == 'a'){
    printf("\n\n\nI'm the Big Deal! Think of me as a benevolent guide leading you to safety!\n"
    "You seem to be out of it so I'll make this quick. There's a wicked snow storm on it's\n"
    "way to Grinnell, and you must make it to Noyce 3rd to survive the storm! (don't ask why)\n"
    "Quick, take this hand of playing cards and make haste! Don't lose the cards!\n\n ");
  } else if(ch == 'b'){
    printf("\n\n\nI know right? Everyone knows it's a dying field... anywho it's too late to change your\n"
    "mind now! You seem to be out of it so I'll make this quick. There's a wicked snow storm on it's\n"
    "way to Grinnell, and you must make it to Noyce 3rd to survive the storm! (don't ask why)\n"
    "Quick, take this hand of playing cards and make haste! Don't lose the cards!\n\n ");
  } else if(ch == 'c'){
    printf("\n\nI'm the-\n"
    "No you're not a student at Cornell, you're a student at GRINNELL. Iowa, not New York, hate to\n"
    "break it to ya bud... Anyways I'm the Big Deal! Think of me as a benevolent guide leading you to safety!\n"
    "You seem to be out of it so I'll make this quick. There's a wicked snow storm on it's\n"
    "way to Grinnell, and you must make it to Noyce 3rd to survive the storm! (don't ask why)\n"
    "Quick, take this hand of playing cards and make haste! Don't lose the cards!\n\n\n");
  }

  printf("\n\n\n**What a strange guy... Anyways you make haste to Noyce 3rd! In the distance you see\n"
  "a grand tornado standing still in the distance. Although you just woke up with no recollection\n"
  "of who or where you are, you seem to remember someone telling you 'If you see a tornado standing \n"
  "still in the distance, it's coming towards you.' You probably want to start sprinting to Noyce...**\n\n\n");
  
  printf("\n\n**On your way to Noyce you encounter a squirrel trapped under a broken tree branch\n\n"
  "Will you:**\n\n"
  
  "a. \n\nLift the branch and set the lil guy free. \n\n"
  "b. \n\nPretend you don't see (or hear) the lil guy struggling and robotically shuffle towards the entrance to Noyce.\n\n"
  "c. \n\nWalk towards the lil guy, getting his hopes up, only to walk right past him, maintaining eye contact the entire time.\n\n");
  
  int choice2 = getchar();

  if(choice2 == '\0'){
    perror("Failed to read choice");
    exit(EXIT_FAILURE);
    } // check if empty

// Clear the newline character from the buffer
  getchar();
  
  if(choice2 == 'a'){
    printf("Wow, what a upstanding person you are! Grinnellians get sh*t done (or whatever).\n"
    "You walk into Noyce feeling like a hero. While this game doesn't have stats implemented,\n"
    "here's an imaginary +20 charisma +10 coolness to make you feel better!\n\n");

    // add advantage
    Cond = 0;
  } else if(choice2 == 'b'){
    printf("Oh wow! You suck... This is a game of Blackjack so...\n\n\n"
    "*You start the match unable to view your cards until you 'hit' or 'stay' \n"
    "Ya know, since your eyes don't work and all*\n\n");

    // add disadvantage
    Cond = 1;
  } else if(choice2 == 'c'){
    printf("\n\n\n\n\n*You will regret that\n\n*");

     // add disadvantage
    Cond = 2;
  }

 
} // story


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
    // Add Story
  //story(socket_fd);
  
  
  play(socket_fd);
  printf("game 2 \n");
  close(socket_fd);
  return 0;
}