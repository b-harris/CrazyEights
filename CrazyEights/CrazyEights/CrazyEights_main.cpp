/* Project: Crazy Eights
 * Date: April 21, 2014
 * Student: Rebecca Harris
 * Description: Handles the game, card, and player logic to play Crazy Eights.
 *              Sets up the deck of cards to be used for the game and handles
 *              all interactions with the deck/the discard pile/the player's hand.
 */

#include <locale>
#include "CardLibrary.hpp"

bool emptyDeck = false;
string getCmd = "", cmd = "";
int numCardsDrawn = 1, numSkippedTurns = 0;
CardHandler ch;

// Prints out the card on the top of the discard pile
void printDiscard() {
	Card discardCard = ch.displayDiscard();
	cout << "\n\n\t\t" << discardCard.getRank() + discardCard.getSuit() << "\n" << endl;
}

int main() {
	int playNum = 0;
	bool validTurn = false, isPlayer1Turn = true;

	// Create the deck and the discard pile
	ch.generateDeck();
	ch.setupDiscard();
	
	// Add the players to the game
	Player p, p2;

	ch.addPlayer(p);
	ch.addPlayer(p2);

	// Populate vector with the players (used to reference whose turn it is)
	vector<Player> players = ch.getPlayers();

	// Print out game header
	cout << "      ~~  CRAZY EIGHTS  ~~" << endl;
	cout << "       By Rebecca Harris" << endl;
	cout << "      Enter RULES for help";

	// Loop through this check while the game is going
	while(cmd != "QUIT") {

		// Determines whose turn it is
		if(isPlayer1Turn == true) 
			playNum = 0;
		else
			playNum = 1;

		validTurn = false;

		// Draw 2 was previously played, player has the draw extra cards
		if(numCardsDrawn > 1) {
			cout << "\nDrawing " << numCardsDrawn << " extra cards this turn." << endl;
			ch.drawCard(players[playNum],numCardsDrawn);
			numCardsDrawn = 1;
		}

		// Queen was previously played, player turn is skipped
		if(numSkippedTurns > 0) {
			cout << "Skipping Player " << players[playNum].getPlayerNum() << "'s turn." << endl;
			numSkippedTurns -= 1;
		}
		else { // Turn was not skipped

			// Loop until their turn was a valid one
			while(validTurn == false) {
				// Print the discard pile card
				printDiscard();			

				// Print whose turn it is and their hand
				cout << "Player " << players[playNum].getPlayerNum() << "'s turn." << endl;
				ch.checkSuit();
				players[playNum].printHand();

				// Prompt the player for their comand
				cout << "\nEnter command: ";
				getline(cin,getCmd);
				cmd = "";
	
				// Make the command all uppercase to recognize it
				for(size_t i = 0; i < getCmd.length(); i++)
					cmd += toupper(getCmd[i],loc);

				// Command evaluation
				if(cmd == "PASS") { // Player passed their turn
					cout << "\nPlayer " << players[playNum].getPlayerNum() << " has passed." << endl;
					validTurn = true;

					ch.drawCard(players[playNum],numCardsDrawn);
				}
				else if(cmd == "QUIT") { // Player quit out
					cout << "\nGame discontinued." << endl;
					return 0;
				}
				else if(cmd == "RULES") { // Player wants to view the rules
					cout << "\n\n\t\t\t~~ Crazy Eights rules ~~" << endl;
					cout << "The goal of the game is to empty out your hand! Match the rank or suit of a card or multiple cards in your hand to the card in the center.";
					cout << "When playing multiple cards, make sure the first card played matches the suit of the card in the center.";
					cout << "When playing multiple cards, all of the ranks must match, however the suits can be different (first card's suit must match center card's).\n" << endl;
					cout << "Ranks: 2, 3, 4, 5, 6, 7, 8, 9, 10, J (Jack), Q (Queen), K (King), A (Ace)" << endl;
					cout << "Suits: D (Diamonds), S (Spades), C (Clubs), H (Hearts)" << endl;
					cout << "Format of cards: 2D (2 of Diamonds), JC (Jack of Clubs)" << endl;
					cout << "How to play a card(s): List the card in your hand like \"2D\", if multiple list them as \"2D 2C 2H\"." << endl;
					cout << "Commands: RULES, QUIT, PASS" << endl;
					cout << "Wild cards: - Rank 2: Makes the next player pick up 2 cards on their hands." << endl;
					cout << "              When stacked, card # to be drawn increases with each 2!" << endl;
					cout << "            - Rank 8: Allows the player to change the current suit." << endl;
					cout << "              When stacked, player can continue to change to suit (for fun!)" << endl;
					cout << "            - Rank Q (Queen): Misses the next player(s) turn." << endl;
					cout << "              When stacked, a turn is missed for each Queen played." << endl;
					cout << "            - Rank A (Ace): Reverses the direction of turns." << endl;
					cout << "              When stacked, goes reverse with each Ace." << endl;
					cout << "-----------------------------------------------------------------------------" << endl;
				}
				else { // Regular turn
					Deck d = ch.checkPlayedCards(players[playNum],cmd);

					// Check that the cards they want to play are valid
					if(d.size() > 0) {
						// Gets the result of the cards played (wild/special cards, regular card, invalid card)
						string result = ch.playCards(players[playNum],d);

						if(result == "Success") { // Regular successful play
							cout << "\nPlayer " << players[playNum].getPlayerNum() << " has successfully played: " << cmd << endl;

							if(players[playNum].getHandSize() == 0) {

								cout << "\n~~ Player " << players[playNum].getPlayerNum() << " has won the game! ~~" << endl;
								return 0;
							}
							validTurn = true;
						}
						else { // Wild/special card or invalid card
							if(result.find("Extra") != string::npos) { // Draw 2 was played
								// Get the number of extra cards for the next player to draw
								numCardsDrawn = ch.getExtraCards();

								// Print that the player's turn was successful and check if their hand is empty
								cout << "\nPlayer " << players[playNum].getPlayerNum() << " has successfully played: " << cmd << endl;

								if(players[playNum].getHandSize() == 0) {
									cout << "\n~~ Player " << players[playNum].getPlayerNum() << " has won the game! ~~" << endl;
									return 0;
								}
								validTurn = true;
							}
							else if(result.find("Skipped") != string::npos) { // Queen was played
								// Get the number of turns to be skipped
								numSkippedTurns = ch.getTurnsMissed();

								// Print that the player's turn was successful and check if their hand is empty
								cout << "\nPlayer " << players[playNum].getPlayerNum() << " has successfully played: " << cmd << endl;

								if(players[playNum].getHandSize() == 0) {
									cout << "\n~~ Player " << players[playNum].getPlayerNum() << " has won the game! ~~" << endl;
									return 0;
								}
								validTurn = true;
							}
							else if(result.find("Reversing") != string::npos) { // Ace was played
								// Print that the player's turn was successful and check if their hand is empty
								cout << "\nPlayer " << players[playNum].getPlayerNum() << " has successfully played: " << cmd << endl;

								if(players[playNum].getHandSize() == 0) {
									cout << "\n~~ Player " << players[playNum].getPlayerNum() << " has won the game! ~~" << endl;
									return 0;
								}
								validTurn = true;
							}
							else { // Invalid card, output results
								cout << result << endl;
							}
						}
					}
					else { // Player tried to play an invalid card (not in hand, invalid format, etc.)
						cout << "\nInvalid play. Only play valid cards from your hand." << endl;
					}
				}
			}
		}
	
		// Switch player turn
		if(isPlayer1Turn == true)
			isPlayer1Turn = false;
		else
			isPlayer1Turn = true;
	}
}
