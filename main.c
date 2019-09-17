#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "Board.h"
#include "initialisation.c"
#include "affichage.c"
#include "deplacement.c"
#include "estimation.c"
#include "MinMax.c"
#include<omp.h>
/* Added Lines*/
#include <signal.h>
#define LIGHT			0
#define DARK			1
#define EMPTY			6


////
int main(){

    //------------------------------------------- dans le cas d'utilisation du ARENA
    printf("tapez quelque chose pour commencer ! \n");
    char s[256];

            scanf("%s",&s);
            if (!strcmp(s, "xboard")) {
                    xboard();
                }

    //-------------------------------------------------------------------------------

    int choix;
    configuration board;

    MODE:
        printf("Veuillez-vous choisir le mode du jeu:\n\t1- Homme - Homme\n\t2- Homme - Machine\nVotre chiox: ");
        scanf("%d", &choix);
        switch(choix)
        {
            case 1:
                init(&board);
                affich(board);
                while(true)
                {
                    board = deplacementManuele(board);
                    affich(board);
                }
            break;
            case 2:
                MachineIA();
            break;
            default :
                goto MODE;
            break;
        }

    return 0;
}

//-------------------------------------------------UTILISER L'INTERFACE ARENA
int parse_move(char *s)
{
	int from, to, i;

	if (s[0] < 'a' || s[0] > 'h' ||
			s[1] < '0' || s[1] > '9' ||
			s[2] < 'a' || s[2] > 'h' ||
			s[3] < '0' || s[3] > '9')
		return -1;
    else return 1;
}


//------------------------ la partie suivante est analogue à celle avec la console, voir la fonction 'machineIA()'

void xboard() /// L'interface de comunication avec le logiciel ARENA
{
	int sx, dx, n, i, j, score, cout, cout2, hauteur;
    int computer_side;
    int choix=0;
    int xside,side;
    struct configuration T[100];
    char line[256], command[256];
    char cmd[5], str[5];
    int x1, y1, x2, y2,z,x11,x22,y11,y22;
    bool deplacer = false;

    configuration board;
    signal(SIGINT, SIG_IGN);
    printf("\n");
    init(&board);
    int cpt,r;
    configuration tab_conf[100];
    hauteur=3;

LABEL:    computer_side= DARK;
    int debut = 0;
    printf("enter !;\n");
	for (;;) {
		fflush(stdout);
		printf("computer_side : %d \n", computer_side);
		if (computer_side==DARK && debut == 1) {
            if (opening==1){
                char chaine [4];
                char *rep;
                rep = nextHit(H);

                //printf("la réponse est : %s\n",rep);
                  if (rep != NULL) {

                        strcpy(chaine,rep);
                        x11 = chaine[1] - '1' + 1;
                        y11 = chaine[0] - 'a' + 1;
                        x22 = chaine[3] - '1' + 1;
                        y22 = chaine[2] - 'a' + 1;

                        if(strcmp(chaine, "e8c8") == 0)
                            {
                                board.board[x22-1][y22-1] = board.board[x11-1][y11-1];
                                board.board[x11-1][y11-1] = 0;
                                board.board[7][3] = board.board[7][0];
                                board.board[7][0] = 0;
                                board.turn ++;
                                deplacer = false;
                                printf("move e8c8\n");
                                strcat(H," ");
                                strcat(H,"e8c8");
                                strcat(H," ");
                            }
                            else
                            {
                                if(strcmp(chaine, "e8g8") == 0)
                                {
                                board.board[x22-1][y22-1] = board.board[x11-1][y11-1];
                                board.board[x11-1][y11-1] = 0;

                                board.board[7][5] = board.board[7][7];
                                board.board[7][7] = 0;

                                board.roque[2] = 1;
                                board.roque[3] = -1;
                                board.turn ++;
                                deplacer = false;
                                printf("move e8g8\n");
                                strcat(H," ");
                                strcat(H,"e8g8");
                                strcat(H," ");
                                }
                                else
                                {
                                    board.board[x22-1][y22-1] = board.board[x11-1][y11-1];
                                    board.board[x11-1][y11-1] = 0;
                                    board.turn ++;
                                    deplacer = false;
                                    printf("move %s\n",chaine);
                                    strcat(H," ");
                                    strcat(H,chaine);
                                    strcat(H," ");
                                }
                            }
                       }

                else {
                    opening = 0;
                    n=cout_possible( T, board, MIN );
                    score = +INFINI;
                    j = -1;
                    #pragma omp parallel for schedule (dynamic)
                    for (i=0; i<n; i++) {
                        if(choix==0){
                            cout = minmax_ab( T[i], MAX, hauteur,-INFINI, +INFINI);
                        }else{
                            cout = minmax( T[i], MAX, hauteur);
                            printf("le ccout MinMax est %d\n",cout);
                        }
                          #pragma omp critical
                           {
                              if ( cout < score ) {
                                 score = cout;
                                 j = i;
                               }
                           }
                    }


                   if ( j != -1 ) {
                             getCMD(board, T[j], &x1, &y1, &x2, &y2, &z);
                             copier( &T[j], &board );
                               board.val = score;
                               board.turn ++;
                               printf("move %c%c%c%c%c\n", x1,y1,x2,y2,z);
                             }
                }
        }

        else
        {
                    n=cout_possible( T, board, MIN );
                    score = +INFINI;
                    j = -1;
                    #pragma omp parallel for schedule (dynamic)
                    for (i=0; i<n; i++) {
                        if(choix==0){
                            cout = minmax_ab( T[i], MAX, hauteur,-INFINI, +INFINI);
                        }else{
                            cout = minmax( T[i], MAX, hauteur);
                            printf("le ccout MinMax est %d\n",cout);
                        }
                           #pragma omp critical
                           {
                              if ( cout < score ) {
                                 score = cout;
                                 j = i;
                               }
                           }
                    }
                   if ( j != -1 ) {
                             getCMD(board, T[j], &x1, &y1, &x2, &y2, &z);
                             copier( &T[j], &board );
                             board.val = score;
                             board.turn ++;
                             printf("move %c%c%c%c\n", x1,y1,x2,y2);
                             }

        }
           computer_side=LIGHT;
           continue;
		}

		if (!fgets(line, 256, stdin))
			return;

		if (line[0] == '\n')
			continue;
		sscanf(line, "%s", command);


		if (!strcmp(command, "xboard"))
			continue;
		if (!strcmp(command, "new")) {
			init(&board);
            affich(board);
			computer_side = DARK;
			goto LABEL;
			continue;
		}
		if (!strcmp(command, "quit"))
			return;
		if (!strcmp(command, "force")) {
			computer_side = EMPTY;
			continue;
		}
		if (!strcmp(command, "white")) {
			side = LIGHT;
			xside = DARK;
			computer_side = DARK;
			continue;
		}
		if (!strcmp(command, "black")) {
			side = DARK;
			xside = LIGHT;
			computer_side = LIGHT;
			continue;
		}
		if (!strcmp(command, "st")) {

			continue;
		}
		if (!strcmp(command, "sd")) {

			continue;
		}
		if (!strcmp(command, "time")) {

			continue;
		}
		if (!strcmp(command, "otim")) {
			continue;
		}
		if (!strcmp(command, "go")) {
			computer_side = DARK;
			continue;
		}
		if (!strcmp(command, "hint")) {

			continue;
		}
		if (!strcmp(command, "undo")) {

			continue;
		}
		if (!strcmp(command, "remove")) {

			continue;
		}
		if (!strcmp(command, "post")) {

			continue;
		}
		if (!strcmp(command, "nopost")) {

			continue;
		}

		if(parse_move(line)==1){
              sprintf(cmd, "%c%c%c%c",line[0],line[1],line[2],line[3]);
              if (opening == 1)
              {
                 char coup [4];
                 sprintf(coup, "%c%c%c%c",cmd[0], cmd[1], cmd[2], cmd[3]);
                 strcat(H,coup);
		         if (firstHit==1)
                {
                  readF(coup);
                  firstHit--;
                }
              }

              if(strcmp(cmd, "e1c1") == 0)
                {
                    deplacer = grandRock(&board);
                }
                else
                {
                    if(strcmp(cmd, "e1g1") == 0)
                    {
                        deplacer = petitRock(&board);

                    }
                    else
                    {
                        x1 = cmd[1] - '1' + 1;
                        y1 = cmd[0] - 'a' + 1;
                        x2 = cmd[3] - '1' + 1;
                        y2 = cmd[2] - 'a' + 1;
                        deplacer = deplacerBlanc(&board, x1, y1, x2, y2);
                    }
                }
                if (deplacer)
                {
                    board.turn --;
                }
            debut = 1;
            affich(board);
            computer_side=DARK;
        }
	}
}
//////////////////////////////////////////////////////////////////////////////////
