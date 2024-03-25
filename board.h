#ifndef _BOARD_H_
#define _BOARD_H_

#include <stdbool.h>

/**
 * \file board.h
 *
 * \brief This SAE game engine functions.
 *
 * In this file, all the functions for having a game run are defined
 * and documented.
 *
 * \author Dorbec
 *
 */

/**
 * \mainpage SAE 1.1 and 1.2 for IUT Grand Ouest Normandie, computer science department
 * \section description Description of the SAE
 * This SAE is aimed to implement a two player game.
 * The rules of the games are described below.
 *
 * The project is divided into various files. Mostly, the engine of the game,
 * implementing the detailed rules, is provided in the files board.c and board.h
 *
 * \section rules Detailed rules of the game.
 *
 * The game is played by two players that we call here _north_ and _south_.
 * It is played on a six by six _square_ board,
 * each square displaying a _digit_ among 1, 2 and 3, evenly distributed.
 * Thus each digit appears on precisely twelve squares.
 *
 * Each player owns six pieces, one _king_ and five _pawns_.
 *
 * The game is won by the first player to catch the opponent's
 * king with any of his own pieces.
 *
 * \subsection setting Setting up of the game
 *
 * In the first phase of the game, player _north_ places his six pieces
 * on the board, on any six squares among the two top lines.
 * Two pieces may not be placed on the same square.
 * Then player _south_ does the same, on the two bottom lines of the board.
 *
 * \subsection fight Main part of the game
 *
 * Each player then has to move one of his pieces.
 *
 * Player _north_ plays first.
 * He chooses any of his pieces and moves it with a number
 * of step corresponding to the digit displayed on its initial square.
 * Each step may be made in any direction (N, S, E, W), but not in diagonal.
 * The different steps of a move can be made in different directions.
 * However, it is not allowed to enter a non empty square
 * or to go twice through the same square.
 *
 * It is possible to make the last step of a move into a square occupied
 * by a piece belonging to the opponent, in which case the piece is *caught*:
 * the opponent's piece is removed from the game, and the player's piece
 * takes its place on the landing square (like in chess).
 * If the opponent's piece was his king, he looses.
 *
 * \subsection special The prescribed digit
 *
 * The key rule of the game is that after the first move,
 * all moves must start with a piece lying on a square displaying the digit
 * of the landing square of the previous move. If _north_ moved a piece
 * to a square with digit 2, then _south_ has to play a piece that is
 * on a square with digit 2 (and thus must move the corresponding piece
 * by two steps). Then the digit of the square where _south_ piece lands
 * forces _north_ to move a piece on a square with that digit, and so on.
 *
 * There are only two (natural) exceptions to this rule:
 *  * when the player has no piece on a square with the prescribed digit
 *  * when none of the pieces of the player that are on a square displaying
 * 	  the prescribed digit may make a complete move.
 *
 * In both cases, the next player may then
 *  * either play any of his own pieces,
 *  * or bring back one of his pieces that had been caught before,
 *    and place it on any _empty_ square of the board with the prescribed digit.
 *    This is counted as a complete move,
 *    the prescribed number does not change for the next player
 *    (it is the value on which the piece has been placed).
 *    The piece brought back may be moved from the player's next turn
 *    following the normal rules.
 */

/**
 * @brief Pointer to the structure that holds the game.
 *
 * Details of the content are not necessary for its use, so the structure is not
 * included here.
 */
typedef struct board_s * board;

/**
 * @brief Game board size dimension.
 *
 * In the following, all indices are given from 0 to DIMENSION - 1.
 */
#define DIMENSION 6

/**
 * @brief number of pieces of each player at the beginning.
 */
#define NB_INITIAL_PIECES 6

/**
 * @brief number of digits used by the game
 * (digits are from 1 to that constant)
 */
#define NB_DIGITS 3

/**
 * @brief the different types of pieces.
 *
 * This set includes references to a 'none' type, which may be used on empty squares.
 */
enum type_e {
  NONE, /**< no pieces */
  PAWN, /**< a regular pawn piece*/
  KING  /**< a king piece */
};

/** simplified name */
typedef enum type_e type;

/**
 * @brief return codes for functions when they fail.
 *
 * * OK means the function ran smoothly
 * * ::OUT means coordinates where not on the grid
 * * ::BUSY means some square that needed to be empty is actually occupied
 * * ::RULES means the move does not respect the rules.
 */
enum return_code {
  OK, /**< things went smoothly */
  OUT, /**< coordinates where not on the grid */
  BUSY, /**< some square that needed to be empty is actually occupied */
  RULES, /**< the move does not respect the rules*/
  STAGE, /**< it is not the right time to call the function */
};

/**
 * @brief the different players for further reference.
 * ::NO_PLAYER is used when informing that a square is empty.
 */
enum players_e {
  NO_PLAYER, /**< None of the two players*/
  NORTH, /**< the first player, north*/
  SOUTH /**< the second player, south*/
};

/** simplified type name for the player */
typedef enum players_e player;

/**
 * @brief number of players in the game.
 */
#define NB_PLAYERS 2


/**
 * @brief the directions that can be used for moving pieces.
 **/

enum direction_e {
  N, /**< towards decreasing line numbers **/
  S, /**< towards increasing line numbers **/
  E, /**< towards increasing column numbers **/
  W /**< towards decreasing column numbers **/
};

/** simplified type name for the direction **/
typedef enum direction_e direction;

/**@{
 * \name Game creation/deletion functionalities.
 */

/**
 * @brief Defines a new empty board for starting a game.
 * This board is periodic.
 *
 * The first player is always ::NORTH
 * @return the newly created board
 */
board new_game();

/**
 * @brief Defines a new random empty board for starting a game.
 * The positions of the digits are placed randomly,
 * though the two first lines and the two last lines must
 * contain the same number of each digit
 * (on a six by six standard board, that is four of each).
 *
 * The first player is always ::NORTH
 * @return the newly created board
 */
board new_random_game();

/**
 * @brief Makes a deep copy of the game.
 * @param original_game the game to copy.
 * @return a new copy fully independent of the original game.
 */
board copy_game(board original_game);

/**
 * @brief Delete the game and frees all associated memory.
 * @param game the game to destroy.
 */
void destroy_game(board game);

/**@}*/

/**@{
 * \name Accessing game data functionalities.
 */

/**
 * @brief returns the digit displayed by the corresponding square
 * when attempting to access a square outside the grid,
 * this function returns 0
 *
 * @param game the game from which to collect information.
 * @param line the line number of the square.
 * @param column the column number of the square.
 * @return the digit on the square
 */
int get_digit(board game, int line, int column);

/**
 * @brief return the player whose turn it is to play.
 *
 * Current player is ::NORTH at the beginning of the game,
 * and it should always allow to check whose turn it is to play,
 * including during the setting up of the game.
 *
 * @param game the game to consider
 * @return the current player in the game
 */
player current_player(board game);

/**
 * @brief return the digit prescribed for the next move,
 * except on two special situations:
 * * During the set-up phase of the game, this returns -1.
 * * Once the set-up is over but still no player moved, returns 0.
 *
 * @param game the game queried.
 * @return a digit between -1 and ::NB_DIGITS (included).
 */
int get_prescribed_move(board game);

/**
 * @brief returns the player holding the indicated place.
 *
 * If there is no piece on that place, or if the request is misplaced,
 * this function returns ::NO_PLAYER
 *
 * @param game the game from which to collect information
 * @param line the line number
 * @param column the column number
 * @return the player who has a piece on that place
 */
player get_place_holder(board game, int line, int column);

/**
 * @brief returns true if there is a king at the given position, false otherwise

 * @param game the game from which to collect information.
 * @param line the line number of the square.
 * @param column the column number of the square.
 * @return true if there is a king at the given position
 */
bool is_king(board game, int line, int column);

/**
 * @brief Checks whether the game has a winner already
 *
 * To win, a player has to catch the opponent king.
 *
 * @param game the game to test.
 * @return the winning player or ::NO_PLAYER if no player is winning.
 */
player get_winner(board game);

/**
 * @brief Returns the number of pieces of the given player on the board.
 * This is necessary to decide whether 
 * a player may bring back a caught piece
 * when he is not able to play a piece 
 * from one of the prescribed positions.
 *
 * @param game the game to check
 * @param checked_player the player whose number of pieces on board is requested
 * @return the number of pieces of the given player on the board.
 */
int get_nb_pieces_on_board(board game, player checked_player);

/**@}*/


/**@{
 * \name Setting up fonctionnalities
 */

/**
 * @brief indicates which piece the current player should place.
 *
 * The player places first his king then his other pieces.
 * This should indicate which piece the player must play,
 * ::KING or ::PAWN.
 * If this function is called after the end of the setting up,
 * it returns ::NONE.
 *
 * @param game the board of the game considered
 * @return the piece to place, ::NONE if the setting up is over.
 */
type piece_to_place(board game);

/**
 * @brief adds a piece on the board on the given position.
 *
 * This adds a piece of the current player on the board at the given
 * coordinates.
 * The player and the type of the pieces can be checked before use
 * with functions current_player() and piece_to_place()
 *
 * If the function returns ::OK, everything worked fine,
 * the piece is placed and the values of current_player() and piece_to_place()
 * are updated if necessary.
 *
 * Any other ::return_code translates a **problem**,
 * in which case no piece is placed.
 * They are returned with this priority order:
 * * ::STAGE the setting up is over.
 * * ::OUT the given coordinates are not correct.
 * * ::RULES the given square is not on the current player's side,
 * * ::BUSY the given square already contains a piece.
 *
 * @param game the board on which to place the piece.
 * @param line line and column where to place the piece, 
 *  (0 or 1 for ::NORTH, 
 *  ::DIMENSION - 2 or ::DIMENSION - 1 for ::SOUTH).
 * @param column the column where to place the piece, 
 *   between 0 and ::DIMENSION - 1
 * @return ::OK if everything went smoothly
 */
enum return_code place_piece(board game, int line, int column);

/**@}*/

/**@{
 * \name Playing functionalities
 *
 */

/**
 * @brief states whether the piece at the indicated place may be moved.
 *
 * This function may be useful to emphasize which pieces may be moved.
 * Whether the piece has a legal move is also checked before selecting the piece.
 * The function may return false for any of the following reason:
 * * it is not the right time to select a piece 
 *   (because the game is not set up yet or another piece is selected already).
 * * the position does not correspond to a square of the grid.
 * * the position does not match a current player piece
 * * the piece is not on the prescribed digit and
 *   another piece on a square with the prescribed digit may be moved.
 *
 * @param game the board to consider
 * @param line the line number of the square where the piece stands
 * @param column the column number of the square where the piece stands
 * @return true if it is legal to select that piece for the next move.
 **/
bool is_legal_move(board game, int line, int column);

/**
 * @brief Allow the current player to select which piece to move
 *
 * With this function the current player chooses the piece to move.
 * This piece must belong to the current player 
 * (that can be checked with the function current_player() )
 * have a possible legal move 
 * and occupy a square that bears the prescribed digit.
 * In the case when no piece satisfy these three condition, 
 * the prescribed digit condition is dropped, 
 * and it is possible to select any piece of the current player with
 * a legal move.
 *
 * The following ::return_code are used, in this priority order:
 * * ::OUT when the coordinates are not on the grid.
 * * ::STAGE is returned when we are still during setting up, 
 *   when a piece is already selected or when a move is already started.
 * * ::BUSY when the coordinates do not match a piece from 
 *   the current player.
 * * ::RULES is returned in two situations:
 *   * when the place does not display the prescribed digit and 
 *   there is a movable piece lying on a place displaying the prescribed digit. 
 *   * when the piece does not have a legal move (e.g. because it is surrounded by other pieces).
 * @param game the game where to select the piece
 * @param line the line number (from 0 to ::DIMENSION - 1)
 * @param column the column number
 * @return a return code, ::OK if everything went smoothly
 */
enum return_code select_piece(board game, int line, int column);

/**
 * @brief cancel a started move, and unselect any selected piece, 
 * returning to the previous state of the game.
 *
 * This function is helpful when a player wants to cancel a started move.
 * This can avoid being stuck when a player started a move that cannot be finished.
 *
 * This function return the following ::return_code :
 * * ::STAGE if no piece was selected or it is not the proper phase of the game.
 * * ::OK if unselecting was possible.
 */
enum return_code cancel_move(board game);

/**
 * @brief Inserts a pawn took earlier at the given position.
 *
 * The following ::return_code are used, in this priority order:
 * * ::OUT when the coordinates are not on the grid.
 * * ::STAGE is returned when we are still during setting up, 
 *   when a piece is already selected or when a move is already started.
 * * ::BUSY when the coordinates correspond to an occupied square.
 * * ::RULES is returned in three situations:
 *   * when the player does not have a piece to reintroduce in the game
 *   * when the player has a piece that can move on the prescribed digit.
 *   * when the square where the player tries to reintroduce his piece does not display
 *     the prescribed digit.
 * 
 * If the insertion is successful, this function returns OK and switches the moving player,
 * without changing the prescribed digit.
 *
 * @param game the game to consider
 * @param line the line numbre of the insertion position (from 0 to ::DIMENSION - 1)
 * @param column the column number of the insertion position
 * @return OK if the introduction was successful.
 **/
enum return_code insert_pawn(board game, int line, int column);

/**
 * @brief moves the currently selected piece one step in the required direction
 *
 * The following ::return_code are used, in this priority order:
 * * ::STAGE if no piece is selected to move 
 * * ::RULES if the move targets a square which 
 *   was already occupied during this move.
 * * ::OUT if the move would bring the piece out of the board
 * * ::BUSY if the target square is occupied by a piece of the same player 
 *   or of the opponent but it is not the last step of the move.
 * 
 * If the move is the last move of the piece, this concludes the move and 
 * starts next player's move.
 * 
 * @param game the game where to move the piece
 * @param direction the direction where to move the piece
 * @return a return code, ::OK if everything went smoothly
 */
enum return_code move_one_step(board game, direction direction); 

/**
 * @brief Allow the current player to directly move a piece from a starting
 * position to an ending position.
 *
 * With this function the current player indicates the coordinates of the piece to move
 * and the ending coordinate of the piece.
 * The piece at the start coordinates must belong to the current player 
 * and occupy a square that bears the prescribed digit 
 * (unless no piece with a legal move stand on a square with the prescribed digit,
 * when any piece may be chosen).
 *
 * The following ::return_code are used:
 * * ::OUT when the coordinates are not on the grid.
 * * ::STAGE is returned when we are still during setting up, 
 *   when a piece is already selected or when a move is already started.
 * * ::BUSY when the coordinates do not match a piece from 
 *   the current player.
 * * ::RULES is returned in two situations:
 *   * when the place does not display the prescribed digit and 
 *   there is a movable piece lying on a place displaying the prescribed digit. 
 *   * when there is no legal move for the piece toward the given target.
 *
 * @param game the game where to select the piece
 * @param line the line number (from 0 to ::DIMENSION - 1)
 * @param column the column number
 * @return a return code, ::OK if everything went smoothly
 */
enum return_code quick_move(board game, int start_line, int start_column, int target_line, int target_column);

/**@}*/
#endif /*_BOARD_H_*/
