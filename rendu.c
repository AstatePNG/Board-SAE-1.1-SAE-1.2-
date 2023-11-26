#include <stdio.h>
#include <stdlib.h>
#include "board.h"

#define NICKNAME_LENGHT 100

enum boolean {FALSE,TRUE};

char get_colomn_let(int column){
	char letter;
	switch(column){
		case 0:
			letter='A';break;
		case 1:
			letter='B';break;
		case 2:
			letter='C';break;
		case 3:
			letter='D';break;
		case 4:
			letter='E';break;
		case 5:
			letter='F';break;
		case 6:
			letter='G';break;
		case 7:
			letter='H';break;
		case 8:
			letter='I';break;
		case 9:
			letter='J';break;
		case 10:
			letter='K';break;
		case 11:
			letter='L';break;
		case 12:
			letter='M';break;
		case 13:
			letter='N';break;
		case 14:
			letter='O';break;
		case 15:
			letter='P';break;
		case 16:
			letter='Q';break;
		case 17:
			letter='R';break;
		case 18:
			letter='S';break;
		case 19:
			letter='T';break;
		default:
			letter='X';break;
	}
	return letter;
}

char get_colomn_num(char column){
	int number;
	switch(column){
		case 'A':
		case 'a':
			number=0;break;
		case 'B':
		case 'b':
			number=1;break;
		case 'C':
		case 'c':
			number=2;break;
		case 'D':
		case 'd':
			number=3;break;
		case 'E':
		case 'e':
			number=4;break;
		case 'F':
		case 'f':
			number=5;break;
		case 'G':
		case 'g':
			number=6;break;
		case 'H':
		case 'h':
			number=7;break;
		case 'I':
		case 'i':
			number=8;break;
		case 'J':
		case 'j':
			number=9;break;
		case 'K':
		case 'k':
			number=10;break;
		case 'L':
		case 'l':
			number=11;break;
		case 'M':
		case 'm':
			number=12;break;
		case 'N':
		case 'n':
			number=13;break;
		case 'O':
		case 'o':
			number=14;break;
		case 'P':
		case 'p':
			number=15;break;
		case 'Q':
		case 'q':
			number=16;break;
		case 'R':
		case 'r':
			number=17;break;
		case 'S':
		case 's':
			number=18;break;
		case 'T':
		case 't':
			number=19;break;
		default:
			printf("jsp");
			number=-1;break;
	}
	return number;
}

void ask_player_name(char name[]){
	printf("Quel surnom voulez-vous utiliser pour cette partie ?\n");
	scanf("%s",name);
}

int ask_player_number(){
    int res=-1;
    scanf("%d",&res);
    if(res<0 || res>DIMENSION){
        getchar();
        printf("Merci d'entrer un chiffre valide.\n");
    }
    return res;
}

char ask_player_letter(){
	char res=-1;
	char max=get_colomn_let(DIMENSION-1);
	getchar();
	scanf("%c",&res);
    if((res<'a' || res>max+'a'-'A') && (res<'A' || res>max)){
        printf("Merci d'entrer une lettre valide. (le problème peut venir du buffer mais est résolvable.)\n");
        return ask_player_letter();
    }
    return res;
}

char ask_player_yn(){
	char res=-1;
	getchar();
	scanf("%c",&res);
    if((res<'a' || res>'z') && (res<'A' || res<'Z')){
        printf("Merci d'entrer une lettre valide. (le problème peut venir du buffer mais est résolvable.)\n");
        return ask_player_yn();
    }
    return res;
}

char get_current_pawn(board game, int line, int column){
	char pawn;
	if(get_place_holder(game,line,column)==NORTH){
				if(is_king(game,line,column)){
					pawn='K';
				}
				else{
					pawn='N';
				}
			}
	else{
		if(is_king(game,line,column)){
			pawn='Q';
		}
		else{
			pawn='S';
		}
	}
	return pawn;
}

void display_out_of_board_pieces(board game){
	for(int i=0;i<6;i++){
		printf(" ");
	}
	if(get_nb_pieces_on_board(game,1)<NB_INITIAL_PIECES){
		for(int i=get_nb_pieces_on_board(game,1);i<NB_INITIAL_PIECES;i++){
			if(get_nb_pieces_on_board(game,1)==NB_INITIAL_PIECES-NB_INITIAL_PIECES && i==get_nb_pieces_on_board(game,1)){
				printf(" \033[1;36mK\033[1;0m ");
			}
			else{
				printf(" \033[1;36mN\033[1;0m ");
			}
		}
	}
	if(get_nb_pieces_on_board(game,2)<NB_INITIAL_PIECES){
		for(int i=get_nb_pieces_on_board(game,2);i<NB_INITIAL_PIECES;i++){
			if(get_nb_pieces_on_board(game,2)==NB_INITIAL_PIECES-NB_INITIAL_PIECES && i==get_nb_pieces_on_board(game,2)){
				printf(" \033[1;35mQ\033[1;0m ");
			}
			else{
				printf(" \033[1;35mS\033[1;0m ");
			}
		}
	}
	printf("\n");
}

int piece_bougeable(board game,int i, int j){
    if(is_legal_move(game,i,j)){
		return 1;
	}
    return 0;
}

void display_current_board(board game){
	int col_num=0,lin_num=1,number;
	char col_letter,pawn_letter;
	for(int i=0;i<=DIMENSION*3;i++){
		for(int l=0;l<DIMENSION;l++){
			printf("|");
			for(int space=0;space<7;space++){
				if(i==0 && space==3){
					col_letter=get_colomn_let(col_num);
					col_num++;
					printf("%c",col_letter);
				}
				else if(i>0 && space==0 && i%3==1){
						number=get_digit(game,i/3,l);
						printf("%d",number);
				}
				else if(space==3 && i%3==2 && get_place_holder(game,i/3,l)!=NO_PLAYER){
					pawn_letter=get_current_pawn(game,i/3,l);
					if(get_place_holder(game,i/3,l)==NORTH){
						if(piece_bougeable(game,i/3,l)==1 && current_player(game)==NORTH){
							printf("\033[1;36m\e[6m%c\e[0m\033[1;0m",pawn_letter);
						}
						else{
							printf("\033[1;36m%c\033[1;0m",pawn_letter);
						}
					}
					else{
						if(piece_bougeable(game,i/3,l)==1 && current_player(game)==SOUTH){
							printf("\033[1;35m\e[6m%c\e[0m\033[1;0m",pawn_letter);
						}
						else{
							printf("\033[1;35m%c\033[1;0m",pawn_letter);
						}
					}
				}
				else{
					printf(" ");
				}
			}
			if(l==DIMENSION-1){
				printf("|");
			}
		}
		if(i%3==2){
			printf("%d",lin_num);
			lin_num++;
		}
		printf("\n");
		if(i%3==0){
			for(int j=0;j<DIMENSION+1;j++){
				printf(" ");
				if(j<DIMENSION){
					for(int line=0;line<7;line++){
						printf("-");
					}
				}
				else{
					printf("-");
				}
			}
			printf("\n");
		}
	}
	display_out_of_board_pieces(game);
}

void display_player_name(board game,char p1[],char p2[]){
	if(current_player(game)==NORTH){
		printf("\033[1;36m%s\033[1;0m",p1);
	}
	else{
		printf("\033[1;35m%s\033[1;0m",p2);
	}
}

void display_winner_name(board game,char p1[], char p2[]){
	if(get_winner(game)==NORTH){
		printf("\033[1;36m%s\033[1;0m",p1);
	}
	else{
		printf("\033[1;35m%s\033[1;0m",p2);
	}
}

void set_player(board game,char nameP1[],char nameP2[]){
    int nb_ligne=0, nb_colonne=0, i=0;
    char col_num;
    for(int j=0; j<NB_PLAYERS; ++j){
		i=0;
        while(i<NB_INITIAL_PIECES){
			printf("Voici le plateau de jeu actuel :\n");
			display_current_board(game);
			current_player(game);
			display_player_name(game,nameP1,nameP2);
			printf(" :\n");
			if(piece_to_place(game)==KING){
				if(current_player(game)==NORTH){
					printf("Place ton roi\n");
				}
				else{
					printf("Place ta reine\n");
				}
            }
            else{
                printf("Place ton pion n°%d\n",i);
            }
            printf("Indique le numéro de la case (exemple : A1,F6)\n");
            col_num=ask_player_letter();
            nb_colonne=get_colomn_num(col_num);
            nb_ligne=ask_player_number()-1;
            enum return_code result = place_piece(game,nb_ligne,nb_colonne); 
            if(result==OK){
                printf("Piece placée.\n\n");
                ++i;
            }
            else{
                printf("Placement non valide.\n\n");
            }
        }
        current_player(game);
    }
}

void move_validation(board game,char Dcolo, int Dline, char Acolo, int Aline){
    char rep;
    int ca,cd;
    ca=get_colomn_num(Acolo);
    cd=get_colomn_num(Dcolo);
    printf("%c%d -> %c%d : Vous validez votre coup ?\n",Dcolo,Dline,Acolo,Aline);
    getchar();
    scanf("%c",&rep);
    if(rep=='n' || rep=='N'){
        cancel_move(game);
    }
    else{
		quick_move(game,Dline,cd,Aline,ca);
	}
}

void select_a_move(board game,char nameP1[],char nameP2[]){
	int start_turn_nb_line,start_turn_nb_column,asked_column,asked_nb_line,i=0,j=0; 
	char start_turn_column,asked_nb_column;
	current_player(game);
	while(i==0){
		printf("Quelle pièce voulez-vous jouer (indiquez le numéro de la case) ?\n");
		if(get_prescribed_move(game)>0){
			printf("Si vous en avez la possibilité, vous devez jouer une pièce sur une case %d.\n(Elle(s) clignotte(nt))\n",get_prescribed_move(game));
		}
		start_turn_column=ask_player_letter();
		start_turn_nb_column=get_colomn_num(start_turn_column);
		start_turn_nb_line=ask_player_number()-1;
		if(is_legal_move(game,start_turn_nb_line,start_turn_nb_column) && 
		get_place_holder(game,start_turn_nb_line,start_turn_nb_column)==current_player(game)){
			printf("Vous avez choisi la pièce %c%d ?\n",start_turn_column,start_turn_nb_line+1);
			char letter;
			letter=ask_player_yn();
			if(letter=='y' || letter=='Y' || letter=='o' || letter=='O'){
				i++;
			}
			else{
				printf("Je n'ai pas compris.\n");
			}
		}
		else{
			printf("Vous ne pouvez pas choisir cette pièce.\n");
		}
		if(j%10==0){
			display_current_board(game);
		}
		j++;
	}
	i=0,j=0;
	display_current_board(game);
	while(i==0){
		printf("Où voulez-vous bouger votre pièce %c%d ?\n",start_turn_column,start_turn_nb_line+1);
		asked_column=ask_player_letter();
		asked_nb_column=get_colomn_num(asked_column);
		asked_nb_line=ask_player_number()-1;
		if(quick_move(game,start_turn_nb_line,start_turn_nb_column,asked_nb_line,asked_nb_column)==OK){
			move_validation(game,start_turn_column,start_turn_nb_line+1,asked_column,asked_nb_line+1);
			i++;
		}
		else{
			printf("Ce coup n'est pas valide.\n");
		}
		if(j%10==0){
			display_current_board(game);
		}
		j++;
	}
	display_current_board(game);
}

int can_insert_a_piece(board game){
	int playable_prescribed_pawn=0;
	for(int line=0;line<DIMENSION;line++){
		for(int column=0;column<DIMENSION;column++){
			if(get_place_holder(game,line,column)==current_player(game) && get_digit(game,line,column)==get_prescribed_move(game)){
				playable_prescribed_pawn++;
				return FALSE;
			}
		}
	}
	return TRUE;
}

void allow_back_piece(board game){
	int asked_nb_column,asked_nb_line,i=0;
	char asked_column;
	while(i==0){
		printf("Où voulez-vous introduire votre pièce ? (indiquez le numéro de la case)\n");
		asked_column=ask_player_letter();
		asked_nb_column=get_colomn_num(asked_column);
		asked_nb_line=ask_player_number()-1;
		if(get_place_holder(game,asked_nb_line,asked_nb_column)==NO_PLAYER){
			if(insert_pawn(game,asked_nb_line,asked_nb_column)==OK){
				printf("Piece placée en %c%d.\n",asked_nb_column,asked_nb_line);
				i++;
			}
			else{
				printf("Erreur lors du placement de la pièce, veuillez réessayer.\n");
			}
		}
		else{
			printf("Vous ne pouvez pas placer votre pièce ici.\n");
		}
	}
}

void game_turn(board game,char nameP1[],char nameP2[]){
	display_player_name(game,nameP1,nameP2);
	printf(" :\n");
	if(get_nb_pieces_on_board(game,current_player(game))<NB_INITIAL_PIECES){
		if(can_insert_a_piece(game)){
			printf("Vous pouvez réintroduire dans le jeu l'une de vos pièces.\nVoulez-vous le faire ?\n");
			char letter;
			letter=ask_player_yn();
			if(letter=='y' || letter=='Y' || letter=='o' || letter=='O'){
				allow_back_piece(game);
			}	
			else{
				select_a_move(game,nameP1,nameP2);
			}
		}
		else{
			select_a_move(game,nameP1,nameP2);
		}
	}
	else{
		select_a_move(game,nameP1,nameP2);
	}
    current_player(game);
}

void play_game(board game,char nameP1[],char nameP2[]){
	printf("J1:\n");
	ask_player_name(nameP1);
	printf("\n");
	printf("J2:\n");
	ask_player_name(nameP2);
	printf("\n");
	set_player(game,nameP1,nameP2);
	display_current_board(game);
	while(get_winner(game)==NO_PLAYER){
		game_turn(game,nameP1,nameP2);
	}
	printf("Bravo ");
	display_winner_name(game,nameP1,nameP2);
	printf(" !\nTu as gagné !\n");
}

int main(int args, char **argv){
	board game = new_random_game();
	char nameP1 [NICKNAME_LENGHT], nameP2 [NICKNAME_LENGHT];
	play_game(game,nameP1,nameP2);
	destroy_game(game);
	return 0;
}


