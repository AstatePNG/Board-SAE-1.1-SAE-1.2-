#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include "board.h"

/**
 * \file board.c
 *
 * \brief Source code associated with \ref board.h
 *
 * \author you
 */

/**
 * @brief The board of the game.
 */

/* The type path_memory is used to store information about taken case during the game.*/

typedef struct {
	int line;
	int column;
} path_memory;

/* The type path store information about the case taken by a path and search if this path is a legal one.*/

typedef struct {
	direction * movement;
	path_memory * used_position;
	bool is_legal_path;
} path;

/* The type pos_path store information about every existing path and their legality.*/

typedef struct {
	int max_posible_move;
	int move_prescribed_digit;
	int start_line;
	int start_column;
	path * path_state;
} pos_path;

/* The type case_b is used to create a table in the board which contains all the case's information in one place.*/

typedef struct {
	int digit;
	player piece_owner;
	type piece;
} case_b;

struct board_s {
	player current_player;
	int prescribed_digit;
	case_b table[DIMENSION][DIMENSION];
	int phase;
	path_memory selected_piece;
	path_memory current_position;
	path_memory last_move;
	int nb_move;
};

/* This function is used for the random game, to test if the random value is a legal one.
 * If not, the function get another random digit.*/

int legal_random_number(int line, int tab_value[]){
	int value=rand()%3+1;
	if((line<2 && tab_value[value-1]==(2*DIMENSION)/NB_DIGITS) || (line>=DIMENSION-2 && tab_value[value-1]==2*((2*DIMENSION)/NB_DIGITS))){
		return legal_random_number(line,tab_value);
	}
	else{
		return value;
	}
}

board new_random_game(){
    board new_board = malloc(sizeof(struct board_s));
    for(int l = 0; l < DIMENSION; l++){
        int digit = (2 * NB_DIGITS - l) % NB_DIGITS;
        for(int c = 0; c < DIMENSION; c++){
            new_board->table[l][c].digit = digit + 1;
            digit = (digit + 1 + (l % 2)) % NB_DIGITS;
        }
    }
	new_board->current_player=NORTH;
	new_board->phase=0;
	new_board->prescribed_digit=-1;
	new_board->selected_piece.line=-1;
	new_board->selected_piece.column=-1;
    return new_board;
}

board new_game(){
	board new_board = malloc(sizeof(struct board_s));
	int num_digit[NB_DIGITS];
	for(int i=0;i<NB_DIGITS;i++){
		num_digit[i]=0;
	}
	int value;
	for(int l=0;l<DIMENSION;l++){
		for(int c=0;c<DIMENSION;c++){
			value=legal_random_number(l,num_digit);
			if(l<2 || l>=DIMENSION-2){
				num_digit[value-1]++;
			}
		}
	}
	new_board->current_player=NORTH;
	new_board->phase=0;
	new_board->prescribed_digit=-1;
	new_board->selected_piece.line=-1;
	new_board->selected_piece.column=-1;
	return new_board;
}

board copy_game(board original_game){
	board new_board = malloc(sizeof(struct board_s));
	for(int l=0;l<DIMENSION;l++){
		for(int c=0;c<DIMENSION;c++){
			new_board->table[l][c]=original_game->table[l][c];
		}
	}
	new_board->current_player=original_game->current_player;
	new_board->prescribed_digit=original_game->prescribed_digit;
	new_board->phase=original_game->phase;
	new_board->selected_piece.line=original_game->selected_piece.line;
	new_board->selected_piece.column=original_game->selected_piece.column;
	new_board->current_position.line=original_game->current_position.line;
	new_board->current_position.column=original_game->current_position.column;
	return new_board;
}

int get_digit(board game, int line, int column){
    if(line<0 || line>=DIMENSION || column<0 || column>=DIMENSION){
        return 0;
    }
    return game->table[line][column].digit;
}

player get_place_holder(board game, int line, int column){
    if(game->table[line][column].piece_owner==SOUTH){
        return SOUTH;
    }
    if(game->table[line][column].piece_owner==NORTH){
        return NORTH;
    }
    return NO_PLAYER;
}

bool is_king (board game, int line, int column){
    if(game->table[line][column].piece==KING){
        return true;
    }
    return false;
}

player current_player(board game){
	return game->current_player;
}

player get_winner(board game){
    player p = NO_PLAYER;
    int cpt=0;
    for(int i=0; i<DIMENSION; ++i){
        for(int j=0; j<DIMENSION; ++j){
            if(is_king(game,i,j)==true){
                cpt+=1;
                p=game->table[i][j].piece_owner;
            }
        }
    }
    if(cpt==NB_PLAYERS){
        return NO_PLAYER;
    }
    else{
        return p;
    }
}

int get_nb_pieces_on_board(board game, player checked_player){
    int cpt=0;
    for(int i=0; i<DIMENSION; ++i){
        for(int j=0; j<DIMENSION; ++j){
            if((game->table[i][j].piece == KING || game->table[i][j].piece == PAWN)
            && game->table[i][j].piece_owner == checked_player){
                cpt+=1;
            }
        }
    }
    return cpt;
}
 
int get_prescribed_move (board game) {
	return game->prescribed_digit;
}

type piece_to_place (board game) {
	int on_board_pieces=get_nb_pieces_on_board(game,game->current_player);
	if (on_board_pieces==0){
		return KING;
	}
	else if (on_board_pieces<NB_INITIAL_PIECES){
		return PAWN;
	}
	else{
		return NONE;
	}
}

enum return_code place_piece(board game, int line, int column){
    player p = current_player(game);
    type t_piece = piece_to_place(game);
    //Gère l'erreur si la settings-up est finit
    if(game->phase == 1){
        return STAGE;
    }
    //Gère les erreurs si l'endroit désignée par line et column n'est pas dans le plateau
    if(line<0 || line>=DIMENSION || column<0 || column>=DIMENSION){
        return OUT;
    }
    //Gère les erreurs de placement par rapport au joueur
    if(game->current_player == NORTH){
        if(line>1){
            return RULES;
        }
    }
    else{
        if(line<DIMENSION-2){
            return RULES;
        }
    }
    //Gère les erreurs si un pion est déja présent
    if(game->table[line][column].piece == KING || game->table[line][column].piece == PAWN){
        return BUSY;
    }
    //Place la pièce si il n'y a pas d'erreur
    game->table[line][column].piece = t_piece;
    game->table[line][column].piece_owner = p;
    //Change de joueur si il atteint le nombre de piece à placer
    if(get_nb_pieces_on_board(game,p)==NB_INITIAL_PIECES){
        if(p==NORTH){
            game->current_player = SOUTH;
            game->phase=0;
        }
        else{
            game->current_player = NORTH;
			game->phase=1;
        }
    }
    return OK;
}

/* start of the code for 'is_legal_move()' */
/* check if a piece is movable without eating another piece for each direction separatly. */

bool playable_north_case (board game, int line, int column){
	if(line-1>=0 && game->table[line-1][column].piece==NONE){
		return true;
	}
	return false;
}
bool playable_west_case (board game, int line, int column){
	if(column-1>=0 && game->table[line][column-1].piece==NONE){
		return true;
	}
	return false;
}
bool playable_south_case (board game, int line, int column){
	if(line+1<DIMENSION && game->table[line+1][column].piece==NONE){
		return true;
	}
	return false;
}
bool playable_east_case (board game, int line, int column){
	if(column+1<DIMENSION && game->table[line][column+1].piece==NONE){
		return true;
	}
	return false;
}

/* check if a piece is movable either without eating another piece or by doing it for each direction separatly.
 * The separation of this function and the above one is to work in the case of a prescribed digit which isn't one */

bool playable_north_by_eating (board game, int line, int column){
	if(line-1>=0 && game->table[line-1][column].piece_owner!=game->current_player){
		return true;
	}
	return false;
}
bool playable_west_by_eating (board game, int line, int column){
	if(column-1>=0 && game->table[line][column-1].piece_owner!=game->current_player){
		return true;
	}
	return false;
}
bool playable_south_by_eating (board game, int line, int column){
	if(line+1<DIMENSION && game->table[line+1][column].piece_owner!=game->current_player){
		return true;
	}
	return false;
}
bool playable_east_by_eating (board game, int line, int column){
	if(column+1<DIMENSION && game->table[line][column+1].piece_owner!=game->current_player){
		return true;
	}
	return false;
}

/* Is only used to know the amont of possible path*/

int power_of_3(int pow_num){
	int x=1;
	for(int i=0;i<pow_num;i++){
		x=x*3;
	}
	return x;
}

/* Create a theorical path for a position.*/

void path_with_case(path theory, pos_path information){
	theory.used_position[0].line=information.start_line;
	theory.used_position[0].column=information.start_column;
	for(int i=1;i<information.move_prescribed_digit;i++){
		switch(theory.movement[i-1]){
			case N:
				theory.used_position[i].line=(theory.used_position[i-1].line)-1;
				theory.used_position[i].column=theory.used_position[i-1].column;
				break;
			case W:
				theory.used_position[i].line=theory.used_position[i-1].line;
				theory.used_position[i].column=(theory.used_position[i-1].column)-1;
				break;
			case S:
				theory.used_position[i].line=(theory.used_position[i-1].line)+1;
				theory.used_position[i].column=theory.used_position[i-1].column;
				break;
			default:
				theory.used_position[i].line=theory.used_position[i-1].line;
				theory.used_position[i].column=(theory.used_position[i-1].column)+1;
				break;
		}
	}
}

bool is_legal_path_on_board(path theory, board game, pos_path information){
	int line_movement,column_movement;
	theory.is_legal_path=false;
	int test_path=0;
	for(int i=0;i<information.move_prescribed_digit-1;i++){
		line_movement=theory.used_position[i].line;
		column_movement=theory.used_position[i].column;
		if(test_path==0){
			switch(theory.movement[i]){
				case N:
					if(playable_north_case(game,line_movement,column_movement)==false){
						test_path++;
					}
					break;
				case W:
					if(playable_west_case(game,line_movement,column_movement)==false){
						test_path++;
					}
					break;
				case S:
					if(playable_south_case(game,line_movement,column_movement)==false){
						test_path++;
					}
					break;
				default:
					if(playable_east_case(game,line_movement,column_movement)==false){
						test_path++;
					}
					break;
			}
		}
	}
	if(test_path==0){
		switch(theory.movement[information.move_prescribed_digit-1]){
			case N:
				if(!playable_north_by_eating(game,theory.used_position[information.move_prescribed_digit-1].line,
				theory.used_position[information.move_prescribed_digit-1].column)){
					test_path++;
				}
				break;
			case W:
				if(!playable_west_by_eating(game,theory.used_position[information.move_prescribed_digit-1].line,
				theory.used_position[information.move_prescribed_digit-1].column)){
					test_path++;
				}
				break;
			case S:
				if(!playable_south_by_eating(game,theory.used_position[information.move_prescribed_digit-1].line,
				theory.used_position[information.move_prescribed_digit-1].column)){
					test_path++;
				}
				break;
			default:
				if(!playable_east_by_eating(game,theory.used_position[information.move_prescribed_digit-1].line,
				theory.used_position[information.move_prescribed_digit-1].column)){
					test_path++;
				}
				break;
		}
	}
	if(test_path==0){
		return true;
	}
	return false;
}

path create_path(pos_path theory_tab, int path_num, board game){
	path created;
	created.movement = malloc(sizeof(direction)*theory_tab.move_prescribed_digit);
	created.used_position = malloc(sizeof(path_memory)*theory_tab.move_prescribed_digit);
	switch(path_num%4){
		case 1:
			created.movement[0]=W;
			break;
		case 2:
			created.movement[0]=S;
			break;
		case 3:
			created.movement[0]=E;
			break;
		default:
			created.movement[0]=N;
			break;
	}
	for(int j=1;j<theory_tab.move_prescribed_digit;j++){
		int val_test=((((path_num/4)/3)/power_of_3(theory_tab.move_prescribed_digit-j))+(path_num/4))%3;
		switch(val_test){
			case 1:
				created.movement[j]=created.movement[j-1];
				break;
			case 2:
				if(created.movement[j-1]==E){
					created.movement[j]=N;
				}
				else if(created.movement[j-1]==W){
					created.movement[j]=S;
				}
				else if(created.movement[j-1]==N){
					created.movement[j]=E;
				}
				else{
					created.movement[j]=W;
				}
				break;
			default:
				if(created.movement[j-1]==E){
					created.movement[j]=S;
				}
				else if(created.movement[j-1]==W){
					created.movement[j]=N;
				}
				else if(created.movement[j-1]==N){
					created.movement[j]=W;
				}
				else{
					created.movement[j]=E;
				}
				break;
		}
	}
	path_with_case(created,theory_tab);
	created.is_legal_path=is_legal_path_on_board(created,game,theory_tab);
	return created;
}

/* Create a table of theorical path starting from a board case.
 * The function is called for the more than 1-lenght moves.*/
 
pos_path path_for_pos(board game, int line, int column){
	pos_path all;
	all.start_line=line;
	all.start_column=column;
	all.move_prescribed_digit=game->table[line][column].digit;
	all.max_posible_move=4*power_of_3(all.move_prescribed_digit-1);
	all.path_state=malloc(sizeof(path)*all.max_posible_move);
	for(int i=0;i<all.max_posible_move;i++){
		all.path_state[i]=create_path(all,i,game);
	}
	return all;
}

/* The 2 case of test in separate function : the case in which the prescribed digit is one 
 * and the case in which it is above one.*/

bool is_legal_for_1 (board game, int line, int column){
	if(game->table[line][column].piece_owner==game->current_player){
		if(playable_north_by_eating(game,line,column) || playable_west_by_eating(game,line,column) 
		|| playable_south_by_eating(game,line,column) || playable_east_by_eating(game,line,column)){
			return true;
		}
	}
	return false;
}

bool is_legal_for_2_or_more (board game, int line, int column){
	pos_path all_legal=path_for_pos(game,line,column);
	for(int i=0;i<all_legal.max_posible_move;i++){
		if(all_legal.path_state[i].is_legal_path==true){
			free(all_legal.path_state);
			return true;
		}
	}
	free(all_legal.path_state);
	return false;
}

/* Test if another piece can be moved if the current one isn't on the prescribed digit.*/

bool have_another_move_available (board game){
	for(int l=0;l<DIMENSION;l++){
		for(int c=0;c<DIMENSION;c++){
			if(game->table[l][c].piece_owner==game->current_player && 
			game->table[l][c].digit==game->prescribed_digit && game->phase==1){
				if((game->prescribed_digit==1 && is_legal_for_1(game,l,c)) 
				|| (game->prescribed_digit>1 && is_legal_for_2_or_more(game,l,c))){
					return true;
				}
			}
		}
	}
	return false;
}


bool is_legal_for_piece(board game, int line, int column) {
    if (game->table[line][column].piece_owner == game->current_player) {
        if (game->table[line][column].digit == 1) {
            return is_legal_for_1(game, line, column);
        } else if (game->table[line][column].digit > 1) {
            return is_legal_for_2_or_more(game, line, column);
        }
    }
    return false;
}

/* The main function for the legality of a move test. */

bool is_legal_move (board game, int line, int column){
 	if(game->phase==1){
		if(game->prescribed_digit==0){
			return true;
		}
		else if(game->table[line][column].digit==game->prescribed_digit){
			if((game->prescribed_digit==1 && is_legal_for_1(game,line,column)) 
			|| (game->prescribed_digit>1 && is_legal_for_2_or_more(game,line,column))){
				return true;
			}
		}
		else if(!have_another_move_available(game)){
			if((game->table[line][column].digit==1 && is_legal_for_1(game,line,column)) 
			|| (game->table[line][column].digit>1 && is_legal_for_2_or_more(game,line,column))){
				return true;
			}
		}
	}
	return false;
}

/* end of test legal_move*/

int drop_condition(board game, player current_player){
    int res=-1;
    for(int i=0; i<DIMENSION; ++i){
        for(int j=0; j<DIMENSION; ++j){
            if(is_legal_move(game,i,j)==true && game->table[i][j].piece_owner == current_player){
                return 1;
            }
        }
    }
    return res;
}

enum return_code select_piece(board game, int line, int column){
    player p = current_player(game);
    //Gère l'erreur si la settings-up est en cours
    if(game->phase == 0 || game->selected_piece.line!=-1){
        return STAGE;
    }
    //Gère les erreurs si l'endroit désignée par line et column n'est pas dans le plateau
    if(line<0 || line>=DIMENSION || column<0 || column>=DIMENSION){
        return OUT;
    }
    //Gère les erreurs de selection de mauvaise piece par rapport au joueur
    if(game->table[line][column].piece_owner != p){
        return BUSY;
    }
    //Gère l'erreur de prescribed digit
    if(game->prescribed_digit!=game->table[line][column].digit){
        for(int i=0; i<DIMENSION; ++i){
            for(int j=0; j<DIMENSION; ++j){
                if(game->table[i][j].piece_owner==p){
                    if(game->table[i][j].digit==game->prescribed_digit){
                        return RULES;
                    }
                }
            }
        }
    }
    //Gère le is_legal_move
    if(is_legal_move(game,line,column)==false){
        //Permet de regarder si d'autre is_legal_move sont dispos
        if(drop_condition(game, p)==1){
            return RULES;
        }
    }
    game->selected_piece.line=line;
    game->selected_piece.column=column;
	game->current_position.line = line;
	game->current_position.column = column;
	game->nb_move = 0;
    return OK;
}

enum return_code cancel_move(board game){
    if(game->selected_piece.line == -1 || game->selected_piece.column == -1 || game->phase == 0 ){
        return STAGE;
    }
    game->table[game->selected_piece.line][game->selected_piece.column].piece 
    = game->table[game->current_position.line][game->current_position.column].piece;
    game->table[game->selected_piece.line][game->selected_piece.column].piece_owner 
    = game->table[game->current_position.line][game->current_position.column].piece_owner;
    game->table[game->current_position.line][game->current_position.column].piece = NONE;
    game->table[game->current_position.line][game->current_position.column].piece_owner = NO_PLAYER;
    game->selected_piece.line = -1;
    game->selected_piece.column = -1;
    return OK;
}

enum return_code insert_pawn (board game, int line, int column) {
    player p = current_player(game);
    int nb_piece = NB_INITIAL_PIECES - get_nb_pieces_on_board(game, p);
    //Gère les erreurs si l'endroit désignée par line et column n'est pas dans le plateau
    if(line<0 || line>=DIMENSION || column<0 || column>=DIMENSION){
        return OUT;
    }
        
    //Gère l'erreur si la settings-up est en cours
    if(game->phase == 0 || game->selected_piece.line != -1 || game->selected_piece.column != -1 ){
        return STAGE;
    }
    //Gère les erreurs si un pion est déja présent
    if(game->table[line][column].piece == KING || game->table[line][column].piece == PAWN){
        return BUSY;
    }
    //Gère les erreurs de RULES
    if(nb_piece == 0 || have_another_move_available(game)==true || game->prescribed_digit != game->table[line][column].digit){
        return RULES;
    }
    game->table[line][column].piece_owner = current_player(game);
    game->table[line][column].piece = PAWN;
    if(p==NORTH){
        game->current_player = SOUTH;
    }
    else{
        game->current_player = NORTH;
    }
    return OK;
}

enum return_code move_one_step (board game, direction direction){
	//Vérifie que la pièce est selectionné
	 if(game->selected_piece.line != -1 || game->selected_piece.column != -1){
		 return STAGE;
	 }
	 //Vérifie que la case demandée n'est pas en dehors du tableau
	 if(direction == N){
		 if(game->selected_piece.line - 1 < 0){
			 return OUT;
		 }
	 }
	 if(direction == S){
		 if(game->selected_piece.line + 1 == DIMENSION){
			 return OUT;
		 }
	 }
	 if(direction == E){
		 if(game->selected_piece.column + 1 == DIMENSION){
			 return OUT;
		 }
	 }
	 if(direction == W){
		 if(game->selected_piece.column - 1 < 0){
			 return OUT;
		 }
	 }
	 //Vérifie que la case n'est pas occupé et que ce n'est pas le dernier mouvement
	 if(direction == N){
		 if(game->selected_piece.line - 1 != NONE && game->nb_move + 1 != game->prescribed_digit ){
			 return BUSY;
		 }
	 }
	 if(direction == S){
		 if(game->selected_piece.line + 1 != NONE && game->nb_move + 1 != game->prescribed_digit){
			 return BUSY;
		 }
	 }
	 if(direction == E){
		 if(game->selected_piece.column + 1 != NONE && game->nb_move + 1 != game->prescribed_digit){
			 return BUSY;
		 }
	 }
	 if(direction == W){
		 if(game->selected_piece.column - 1 != NONE && game->nb_move + 1 != game->prescribed_digit){
			 return BUSY;
		 }
	 }
	 //vérifie qu'il ne passe pas 2 fois au même endroit
	 if(direction == N && game->current_position.line - 1 == game->last_move.line){
		 return RULES;
	 }
	 if(direction == S && game->current_position.line + 1 == game->last_move.line){
		 return RULES;
	 }
	 if(direction == E && game->current_position.column + 1 == game->last_move.column){
		 return RULES;
	 }
	 if(direction == W && game->current_position.column - 1 == game->last_move.column){
		 return RULES;
	 }
	 //ACTION!
	 if(direction == N){
		 game->current_position.line -= 1;
		 game->table[game->current_position.line][game->current_position.column].piece 
		 = game->table[game->current_position.line+1][game->current_position.column].piece;
		 game->table[game->current_position.line][game->current_position.column].piece_owner
		 = game->current_player;
		 game->table[game->current_position.line+1][game->current_position.column].piece 
		 = NONE;
		 game->table[game->current_position.line+1][game->current_position.column].piece_owner
		 = NO_PLAYER;
	 }
	 if(direction == S){
		 game->current_position.line += 1;
		 game->table[game->current_position.line][game->current_position.column].piece 
		 = game->table[game->current_position.line-1][game->current_position.column].piece;
		 game->table[game->current_position.line][game->current_position.column].piece_owner
		 = game->current_player;
		 game->table[game->current_position.line-1][game->current_position.column].piece 
		 = NONE;
		 game->table[game->current_position.line-1][game->current_position.column].piece_owner
		 = NO_PLAYER;
	 }
	 if(direction == E){
		 game->current_position.column += 1;
		 game->table[game->current_position.line][game->current_position.column].piece 
		 = game->table[game->current_position.line][game->current_position.column-1].piece;
		 game->table[game->current_position.line][game->current_position.column].piece_owner
		 = game->current_player;
		 game->table[game->current_position.line][game->current_position.column-1].piece 
		 = NONE;
		 game->table[game->current_position.line][game->current_position.column-1].piece_owner
		 = NO_PLAYER;
	 }
	 if(direction == W){
		 game->current_position.column -= 1;
		 game->table[game->current_position.line][game->current_position.column].piece 
		 = game->table[game->current_position.line][game->current_position.column+1].piece;
		 game->table[game->current_position.line][game->current_position.column].piece_owner
		 = game->current_player;
		 game->table[game->current_position.line][game->current_position.column+1].piece 
		 = NONE;
		 game->table[game->current_position.line][game->current_position.column+1].piece_owner
		 = NO_PLAYER;
	 }
	 game->last_move.column = game->current_position.column;
	 game->last_move.line = game->current_position.line;
	 if(game->nb_move + 1 == game->prescribed_digit){
		if(game->current_player==NORTH){
			game->current_player = SOUTH;
			game->nb_move=0;
			game->selected_piece.line=-1;
			game->selected_piece.column=-1;
			game->prescribed_digit = game->table[game->current_position.line][game->current_position.column].digit;
		}
		else{
			game->current_player = NORTH;
			game->nb_move=0;
			game->selected_piece.line=-1;
			game->selected_piece.column=-1;
			game->prescribed_digit = game->table[game->current_position.line][game->current_position.column].digit;
		}
	 }
	 else{
		 game->nb_move+=1;
	 }
	 return OK;
}

/* Test for the quick_move if a piece can move toward another one with a legal move.*/

bool can_move_toward (board game, int start_line, int start_column, int target_line, int target_column){
	pos_path available_pos=path_for_pos(game,start_line,start_column);
	int line_before_last_move,column_before_last_move;
	if(is_legal_for_2_or_more(game,start_line,start_column)){
		for(int i=0;i<available_pos.max_posible_move;i++){
			line_before_last_move=available_pos.path_state[i].used_position[(available_pos.move_prescribed_digit)-1].line;
			column_before_last_move=available_pos.path_state[i].used_position[(available_pos.move_prescribed_digit)-1].column;
			switch(available_pos.path_state[i].movement[(available_pos.move_prescribed_digit)-1]){
				case N:
					if(!playable_north_by_eating(game,line_before_last_move,column_before_last_move)){
						break;
					}
					if(target_line==line_before_last_move-1 && target_column==column_before_last_move){
						return true;
						break;
					}
				case W:
					if(!playable_west_by_eating(game,line_before_last_move,column_before_last_move)){
						break;
					}
					if(target_line==line_before_last_move && target_column==column_before_last_move-1){
						return true;					
						break;
					}
				case S:
					if(!playable_south_by_eating(game,line_before_last_move,column_before_last_move)){
						break;
					}
					if(target_line==line_before_last_move+1 && target_column==column_before_last_move){
						return true;
						break;
					}
				default:
					if(!playable_east_by_eating(game,line_before_last_move,column_before_last_move)){
						break;
					}
					if(target_line==line_before_last_move && target_column==column_before_last_move+1){
						return true;
						break;
					}
			}
		}
	}
	return false;
}

enum return_code quick_move (board game, int start_line, int start_column, int target_line, int target_column){
	if(start_line<0 || target_line<0 || start_line>=DIMENSION || target_line>=DIMENSION
	|| start_column<0 || start_column>=DIMENSION || target_column<0 || target_column>=DIMENSION){
		return OUT;
	}
	if(game->phase!=1 || game->selected_piece.line!=-1){
		return STAGE;
	}
	if(game->table[start_line][start_column].piece_owner!=game->current_player){
		return BUSY;
	}
	if(!can_move_toward(game,start_line,start_column,target_line,target_column)){
		return RULES;
	}
	game->table[target_line][target_column].piece = game->table[start_line][start_column].piece;
	game->table[target_line][target_column].piece_owner = game->current_player;
	game->table[start_line][start_column].piece_owner = NO_PLAYER;
	game->table[start_line][start_column].piece = NONE;
	if(game->current_player==NORTH){
		game->current_player = SOUTH;
		game->prescribed_digit = game->table[target_line][target_column].digit;
	}
	else{
		game->current_player = NORTH;
		game->prescribed_digit = game->table[target_line][target_column].digit;
	}
	return OK;
}


void destroy_game(board game){
	free(game);
};

/** © Elsa HAMON et Paul LETELLIER **/
