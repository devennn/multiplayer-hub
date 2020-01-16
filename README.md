# Multi Processes Game
Game that involves multiple processes communicating via Pipe

This is an assignment for Programming Course.

Features:
1. Spawn multiple process determined from command arguments
2. Communication using Pipe
3. Signal handling for SIGHUP
4. Reaping Zombies from system when child dies

How this Program works
1. Hub
- Spawns processes (players)
- Keep track of overall game dynamic
- Send information to players via stdout
- Receive information from players via stdin

2. Players
- Receive information from hub via stdin
- Send info to hub via stdout
- Does not communicate with other palyers
- Every information must be through hub

Game Rules
1. Players (P of them): who are positioned in a circle. One player is labelled 0 and the remaining players
are numbered in clockwise order.
2. deck of cards (d of them) containing no duplicates. Each card in the deck has:
– A suit from {S,C,D,H}
– A rank (in hexadecimal) from 1 . . . 9, a . . . f.
- A threshold (T)
4. At the start of the game, each player is given a “hand” of H. The first H cards in the deck are given to Player 0, the next H cards are given to Player 1, etc. Each player tracks:
5. Their points (S)
- The number of cards with the ’D’ suit, played in rounds which they won (V ). D cards are worth −1 points at the end of the game unless the player won at least T of them. In which case, they are worth 1 point each.
6. The game is played in H rounds (one card per round).
- The lead player for each round is:
- Player 0 in the first round.
- The player who won the previous round for all other rounds.
- The lead player plays one of the cards from their hand. The suit of this card is the “lead suit”.
- The other players, in clockwise order from the lead player, play one card from their hands:
- If they have any cards belonging to the lead suit, thån they must play one of those.
 -Otherwise play any card
7. Find the highest ranked card of the lead suit. The player who played that card:
- gets 1 point/ (ie increases S)
- is the lead player for the next round.
- records how many D cards were played that round. (ie increases that player’s V value)
8. When all rounds are complete, each player’s score is
- S − V if V < T
- S + V if V > T
