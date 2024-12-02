# CSC213-Final-Project


We have a "Game" file that will tie all the components together

  Networking file
  
    - Connects computers to the central computer
    - recieve information from 2 connected computers
      - update deck of cards from that
    - 
  Black Jack file
  
    - keep score of cards
    - 

  Game Board file

    - either write code that draws boards within the client and server.c files (reuse the code, read in messages and update the board if one of the two players drew a card)
    - or make a gameInterface file with functions, and have both client and server programs include the file, and utilize the functions
  
  Computer vs player 1

    - player 1 gets dealt cards
    - computer gets dealt
    - player 1 draws until they hold or score >= 21
    - message is sent to the comp that it is their turn
    - ** One card is face down, the subsequent cards are face up
      - Don't reveal everyones cards until the end of everyones turns
    - 


To DO

  - Implement a card counter

