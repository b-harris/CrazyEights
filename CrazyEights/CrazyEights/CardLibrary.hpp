/* Project: Crazy Eights
 * Date: April 21, 2014
 * Student: Rebecca Harris
 * Description: Handles the game, card, and player logic to play Crazy Eights.
 *              Sets up the deck of cards to be used for the game and handles
 *              all interactions with the deck/the discard pile/the player's hand.
 */

#ifndef __CARD_H__
#define __CARD_H__

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <ctime>
#include <string>
#include <sstream>
#include <iterator>
#include <cstdlib>
using namespace std;

// Contains the suits and ranks
string suitName[] = {"D","C","H","S"}; //Diamonds, Clubs, Hearts, Spades
string rankName[] = {"2","3","4","5","6","7","8","9","10","J","Q","K","A"};
locale loc;	

// Limitation variables
int playersMade = 0;
unsigned const MAX_CARDS_PER_HAND = 5;
unsigned const MAX_PLAYERS = 2;

// Card class
// Sets rank and suit of the card
class Card {
public:
	unsigned int rankID_, suitID_;
	static const unsigned int NUM_SUITS = 4, NUM_RANKS = 13;

	// Constructor
	Card(unsigned int s, unsigned int r): suitID_(s), rankID_(r) { }

	// Return the card name in the proper format
	string getCard()
	{
		std::string sName;

		sName = rankName[rankID_];
		sName += suitName[suitID_];
		return sName;
	}

	// Return the card rank and suit
	string getRank() { return rankName[rankID_]; }
	string getSuit() { return suitName[suitID_]; }
};

// Creates the deck and the discard deck
typedef vector<Card> Deck;
Deck deck, discard;

// Player class
class Player
{
public:
	// Player's number and their hand
	int playerNum;
	Deck Hand;

	// Constructor - adds new player, fills their hand
	Player(){
		playersMade += 1;
		playerNum = playersMade;

		if(playersMade > MAX_PLAYERS) {
			cout << "\nCannot add a new player. Maximum number of players reached." << endl;
		}
		fillHand();
	}

	~Player() {
	}

	// Fill the player's hand with cards from the back of the deck
	void fillHand() 
	{
		for(size_t j=0; j< MAX_CARDS_PER_HAND; ++j)
		{
			// Take the back card of the deck and put in the players hand
			Hand.push_back(deck.back());

			// Remove the last card from the deck
			deck.pop_back();
		}
	}

	// Updates the player's hand with the new hand
	void setHand(Deck newHand) { Hand = newHand; }

	// Return player hand deck/hand size
	Deck getHand() { return Hand; }
	int getHandSize() { return Hand.size(); }

	// Removes a card from the player's hand
	void removeFromHand(Card card) {
		for(size_t i = 0; i < Hand.size(); i++) {
			if(Hand[i].getRank() == card.getRank() &&
				Hand[i].getSuit() == card.getSuit()) {
					// Erase the card from the vector, break from loop when card's found
					Hand.erase(Hand.begin() + i);
					break;
			}
		}
	}

	// Returns the player's number
	int getPlayerNum() { return playerNum; }

	// Print out the cards in the player's hand
	void printHand() {
		cout << "Player " << playerNum << "'s hand is: ";
		for_each(this->Hand.begin(),this->Hand.end(), [](Card c) {cout << c.getCard() << " ";});
	}
};

// Vector of Player class
typedef vector<Player> Players;

// CardHandler class - handles the game logic
class CardHandler
{
public:
	Players players;

	static const unsigned int NUM_SUITS = 4, NUM_RANKS = 13;

	string currentSuit;
	int numTurnsMissed, numCardsExtraDraw;
	bool isReversing;

	// Empty constructor
	CardHandler() {}

	// Getters for the wild cards
	int getTurnsMissed() { return numTurnsMissed; }
	int getExtraCards() { return numCardsExtraDraw; }
	bool isReverse() { return isReversing; }

	// Create the deck of cards
	void generateDeck()
	{
		// Initial variable setting
		isReversing = false;
		numTurnsMissed = 0;
		numCardsExtraDraw = 0;

		// Creates a card of each rank for each suit (52 cards total)
		for(unsigned i = 0; i < CardHandler::NUM_SUITS; ++i)
		{
			for(unsigned j = 0; j < CardHandler::NUM_RANKS; ++j)
			{
				Card card(i,j);   
				deck.push_back(card); 
			}
		}

		// Random number generator to randomly shuffle the cards
		srand((unsigned)time(NULL));

		// Shuffle the deck
		random_shuffle(deck.begin(),deck.end());
	}

	// Shuffles the cards from the discard pile below the card on top of the pile
	// into the deck
	// Needs to be done when the deck is empty
	void shuffleDiscardToDeck() {
		if(discard.size() > 1) {
			// Transfers all but the top card of the discard pile into the deck
			for(size_t i = 0; i < discard.size()-1; i++) {
				deck.push_back(discard[i]);
				discard.erase(discard.begin() + i);
			}

			// Random number generator to random shuffle the cards
			srand((unsigned)time(NULL));

			// Shuffle the deck
			random_shuffle(deck.begin(),deck.end());
		}
		else {
			// No cards in the discard pile or the deck (all in player hands)
			cout << "Deck is empty." << endl;
		}
	}

	// Adds player to the players vector
	void addPlayer(Player &p) { players.push_back(p); }

	// Return players vector
	Players getPlayers() { return players; }

	// Draw a number of cards and put it in the player's hand
	void drawCard(Player &p, int numCards) {
		for(int i = 0; i < numCards; i++) {
			// No cards available, shuffle discard pile into the deck
			if(deck.size() == 0) {
				shuffleDiscardToDeck();

				// If there's still no available cards, exit (all cards are in players hands)
				if(deck.size() == 0) 
					break;
			}

			// Available cards
			if(deck.size() > 0) {
				// Take card from the back of the deck and put it in the players hand
				Deck playerHand = p.getHand();
				playerHand.push_back(deck.back());
				p.setHand(playerHand);

				// Remove the card added to their hand from the deck
				deck.pop_back();
			}
		}
	}

	// Initial set-up for the discard pile
	// Puts single card in the pile
	void setupDiscard() {
		Card c = deck.back();
		discard.push_back(c);
		currentSuit = c.getSuit();
		deck.pop_back();
	}

	// Display the card on the top of the discard pile
	Card displayDiscard() {	return discard.back(); }

	// Play the cards the user is placing onto the discard pile
	// Special ranks:
	// 8 = change the suit
	// Q = miss a turn
	// A = direction reverses
	// 2 = draw two
	string playCards(Player &p, Deck cards) {
		Card discardCard = discard.back();
		bool isCardValid = false;
		string cardRank = "", cardSuit = "", discRank = "", discSuit = "", prevRank = "", newSuit = "", inSuit = "";
		
		numTurnsMissed = 0;

		// Get the rank and suit of the card on the discard pile
		discRank = discardCard.getRank();
		discSuit = discardCard.getSuit();

		// Multiple cards being played
		if(cards.size() > 1) {
			for(size_t i = 0; i < cards.size(); i++) {
				// First card being played, make sure it's the same suit as the card on the discard pile if it's not the same rank
				if(i == 0) {
					if(cards[i].getRank() != discRank) {
						if(cards[i].getSuit() != currentSuit) {
							return "Invalid - first card played must match the discard pile suit.";
						}
					}

					prevRank = cards[i].getRank();
				}
				else {
					// Second + card being played, make sure it's the same rank as the previous one played
					cardRank = cards[i].getRank();

					if(cardRank != prevRank) {
						return "Invalid - cannot play multiple cards of different ranks.";
					}

					prevRank = cards[i].getRank();
				}
			}
		}
		
		// Go through all cards being played
		for(size_t i = 0; i < cards.size(); i++) {
			cardRank = cards[i].getRank();
			cardSuit = cards[i].getSuit();

			// Make sure the card is a valid rank/suit
			if(cardRank == discRank || cardSuit == currentSuit) {
				// Special/wild card check
				if(cardRank == "Q") { // Miss a turn
					numTurnsMissed += 1;
					numCardsExtraDraw = 0;

					// Add card to the discard pile and remove it from the player's hand
					discard.push_back(cards[i]);
					p.removeFromHand(cards[i]);

					// Reset the discard card to the new card
					discardCard = discard.back();
					discRank = discardCard.getRank();
					currentSuit = discardCard.getSuit();
				}
				else if(cardRank == "A") { // Reverse
					if(isReversing == true)
						isReversing = false;
					else
						isReversing = true;
					
					numCardsExtraDraw = 0;
					
					// Add card to the discard pile and remove it from the player's hand
					discard.push_back(cards[i]);
					p.removeFromHand(cards[i]);

					// Reset the discard card to the new card
					discardCard = discard.back();
					discRank = discardCard.getRank();
					currentSuit = discardCard.getSuit();
				}
				else if(cardRank == "2") { // next player pick up 2 more cards
					numCardsExtraDraw += 2;

					// Add card to the discard pile and remove it from the player's hand
					discard.push_back(cards[i]);
					p.removeFromHand(cards[i]);

					// Reset the discard card to the new card
					discardCard = discard.back();
					discRank = discardCard.getRank();
					currentSuit = discardCard.getSuit();
				}
				else if(cardRank == "8") { // change suit
					bool validSuit = false;
					numCardsExtraDraw = 0;

					// User specifies the new suit, loops to make sure they enter a valid suit (S/D/C/H)
					cout << "Enter the new suit (S/D/C/H): ";
					while(validSuit == false) {
						getline(cin,inSuit);
						newSuit = "";

						// Make the command all uppercase to recognize it
						for(size_t u = 0; u < inSuit.length(); u++)
							newSuit += toupper(inSuit[u],loc);		

						// Suit specified was valid
						if(newSuit == "S" || newSuit == "D" || newSuit == "C" || newSuit == "H") {
								validSuit = true;
								string newSuitStr = "";
								
								if(newSuit == "D")
									newSuitStr = "Diamonds";
								else if(newSuit == "C")
									newSuitStr = "Clubs";
								else if(newSuit == "H")
									newSuitStr = "Hearts";
								else if(newSuit == "S")
									newSuitStr = "Spades";

								currentSuit = newSuit;

								// Display the newly changed suit
								cout << "Player " << p.getPlayerNum() << " has played " << cards[i].getRank() + cards[i].getSuit() << " and changed the suit to " << newSuitStr << "." << endl; 
													
								// Add card to the discard pile and remove it from the player's hand
								discard.push_back(cards[i]);
								p.removeFromHand(cards[i]);

								// Reset the discard card to the new card
								discardCard = discard.back();
								discRank = discardCard.getRank();
								currentSuit = discardCard.getSuit();
						}
						else {
							// Suit specified was invalid, have them re-enter a suit
							cout << "Invalid suit. Please enter either S, D, C, or H: ";
						}
						
					}
				}
				else { // Regular valid card
					numCardsExtraDraw = 0;

					// Add card to the discard pile and remove it from the player's hand
					discard.push_back(cards[i]);
					p.removeFromHand(cards[i]);

					// Reset the discard card to the new card
					discardCard = discard.back();
					discRank = discardCard.getRank();
					currentSuit = discardCard.getSuit();
				}

				// The card was valid
				isCardValid = true;
			}
			else {
				isCardValid = false;
			}
		}

		// Card is invalid
		if(isCardValid == false)
			return cardRank + cardSuit + " is not a valid play.";

		// 2 was played, return extra
		if(numCardsExtraDraw > 0) {
			return "Extra";
		}

		// Queen was played, return skipped
		if(numTurnsMissed > 0) {
			return "Skipped";
		}

		// Ace was played, return reverse
		if(isReversing == true)
			return "Reversing";

		// No special card played, successful play
		return "Success";
	}
	
	// Check that the cards being played are valid (cards are in the player's hand)
	Deck checkPlayedCards(Player p, string cards) {
		Deck playerD = p.getHand();
		Deck passedCards;
		stringstream ss(cards);
		string passedSuit = "", passedRank = "", passedCard = "";

		vector<string> tempV;
		string curr = "";
		int num = 0;
		
		// Grab all of the cards in the cards string (delimited with spaces)
		while(ss.good()) {
			ss >>curr;
			tempV.push_back(curr);
			num++;
		}

		// Card played exists in their hand, push into vector
		for(size_t i = 0; i < tempV.size(); i++) {
			for(size_t j = 0; j < playerD.size(); j++) {
				if((playerD[j].getRank() + playerD[j].getSuit()) == tempV[i]) {
					passedCards.push_back(playerD[j]);
				}	
			}
		}

		// Cards played do not match the cards in their hand
		if(passedCards.size() != tempV.size()) {
			passedCards.clear();
		}

		return passedCards;
	}

	// Displays the current suit that needs to be played if it's different from the discard pile's suit
	// This will happen after a player has changed the current suit with an 8 card
	void checkSuit() {
		if(currentSuit != discard[discard.size()-1].getSuit()) {
			string newSuitStr = "";

			if(currentSuit == "S")
				newSuitStr = "Spades";
			else if(currentSuit == "D")
				newSuitStr = "Diamonds";
			else if(currentSuit == "C")
				newSuitStr = "Clubs";
			else if(currentSuit == "H")
				newSuitStr = "Hearts";

			// Displays the current suit
			cout << "Current Suit: " << newSuitStr << endl;
		}
	}
};


#endif