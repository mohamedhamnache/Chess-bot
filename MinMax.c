#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX 1
#define MIN -1
#define INFINI 10000


char H[256];  // une chaine de caractaires qui sauvegarde l'historique des coups
int opening = 1; // un variable qui nous permet de savoir si on est toujours dans la phase d'ouverture
int firstHit = 1; //


FILE *f;
char tabBook[500][256];
int taille =0;





void copier( struct configuration *c1, struct configuration *c2 ) /// Copie la configuration c1 dans c2
{
	int i, j;

	for (i=0; i<8; i++)
		for (j=0; j<8; j++)
			c2->board[i][j] = c1->board[i][j];
	for(i=0;i<4;i++)
        c2->roque[i] = c1->roque[i];
	c2->turn = c1->turn;
	c2->val = c1->val;
}

int egal(char c1[8][8], char c2[8][8] ) /// Teste si les conf c1 et c2 sont égales
{
	int i, j;

	for (i=0; i<8; i++)
		for (j=0; j<8; j++)
			if (c1[i][j] != c2[i][j]) return 0;
	return 1;
}

int feuille( struct configuration conf, int *cout ) /// Teste si conf représente une fin de partie et retourne dans 'cout' la valeur associée
{
	*cout = 0;
    int i=0,j=0,pasroiN=0,pasroiB=0;

    for (i=0;i<8;i++){
        for (j=0;j<8;j++){
            if(conf.board[i][j]== 'R'){
               pasroiB=1;
            }
        }
    }
	// Si victoire pour les Blancs cout = +10000
	for (i=0;i<8;i++){
        for (j=0;j<8;j++){
            if(conf.board[i][j]== -'R'){
               pasroiN=1;
            }
        }
    }
    if(!pasroiB) {
        *cout = -10000;
        return 1;
    }
    if(!pasroiN) {
        *cout = +10000;
        return 1;
    }
	// Sinon ce n'est pas une config feuille
	return 0;
}

int minmax_ab( struct configuration conf, int mode, int niv, int alpha, int beta ) /// MinMax avec elagage alpha-beta
{
 	int n, i, score, score2;
 	struct configuration T[100];

   	if ( feuille(conf, &score) )
	    return score;

   	if ( niv == 0 )
		return evaluation( conf );

   	if ( mode == MAX ) {

	   n=cout_possible(T, conf, MAX);
	   score = alpha;
	   for ( i=0; i<n; i++ ) {
   	    	score2 = minmax_ab( T[i], MIN, niv-1, score, beta );
		if (score2 > score) score = score2;
		if (score > beta) {
   	      		return beta;
	    	}
	   }
	}
	else  { // mode == MIN
	   n=cout_possible(T, conf, MIN);
	   score = beta;
	   for ( i=0; i<n; i++ ) {
   	    	score2 = minmax_ab( T[i], MAX, niv-1, alpha, score );
		if (score2 < score) score = score2;
		if (score < alpha) {
   	      		return alpha;
	    	}
	   }
	}
        if ( score == +INFINI ) score = +10000;
        if ( score == -INFINI ) score = -10000;
	return score;
}

int minmax( struct configuration conf, int mode, int niv) /// MinMax sans elagage alpha-beta
{
 	int n, i, score, score2;
 	struct configuration T[100];

   	if ( feuille(conf, &score) )
		return score;

   	if ( niv == 0 )
		return evaluation( conf );
    if ( mode == MAX ) {
            score = -INFINI;
   	}else{
            score = +INFINI;
    }
    if ( mode == MAX ) {
	   n = cout_possible( T, conf, MAX);
	   for ( i=0; i<n; i++ ) {
            score2 = minmax( T[i], MIN, niv-1);
            if (score2 > score) score = score2;

	   }
	}
	else  {
	   n=cout_possible(T, conf, MIN);
	   for ( i=0; i<n; i++ ) {
   	    	score2 = minmax( T[i], MAX, niv-1);

            if (score2 < score) score = score2;
	   }
	}
    if ( score == +INFINI ) score = +10000;
    if ( score == -INFINI ) score = -10000;
	return score;
}


void getCMD(configuration board1, configuration board2,char *x1,char *y1,char *x2,char *y2,char *z) ///
{
    int i, j, x, y;
    bool get = false;

    if((board1.board[0][0] == 'T') && (board1.board[0][4] == 'R') && (board2.board[0][0] == 0) && (board2.board[0][4] == 0))
    {
        *x1 = 'e';
        *y1 = '1';
        *x2 = 'c';
        *y2 = '1';
        get = true;
    }
    if((board1.board[7][0] == -'T') && (board1.board[7][4] == -'R') && (board2.board[7][0] == 0) && (board2.board[7][4] == 0))
    {
        *x1 = 'e';
        *y1 = '8';
        *x2 = 'c';
        *y2 = '8';
        get = true;
    }
    if((board1.board[0][7] == 'T') && (board1.board[0][4] == 'R') && (board2.board[0][7] == 0) && (board2.board[0][4] == 0))
    {
        *x1 = 'e';
        *y1 = '1';
        *x2 = 'g';
        *y2 = '1';
        get = true;
    }
    if((board1.board[7][7] == -'T') && (board1.board[7][4] == -'R') && (board2.board[7][7] == 0) && (board2.board[7][4] == 0))
    {
        *x1 = 'e';
        *y1 = '8';
        *x2 = 'g';
        *y2 = '8';
        get = true;
    }
    if(!get)
    {
        for(i = 0 ; i <= 7 ; i++)
        {
            for(j = 0 ; j <= 7 ; j++)
            {
                if((board1.board[i][j] != 0) && (board2.board[i][j] == 0))
                {
                    x = i;
                    y = j;
                    *y1 = i + '1';
                    *x1 = j + 'a';
                }
            }
        }
        for(i = 0 ; i <= 7 ; i++)
        {
            for(j = 0 ; j <= 7 ; j++)
            {
                if((board1.board[i][j] != board2.board[i][j]) && ((x != i) || (y != j)))
                {
                    *y2 = i + '1';
                    *x2 = j + 'a';
                }
            }
        }
        //printf("1 %c , 2 %c ,3 %c,4 %c \n");
    }
    *z=NULL;
            for(j = 0 ; j <= 7 ; j++)
            {
                if(board1.board[1][j]==-'P' ){
                    switch(board2.board[0][j]){
                        case -'T' : *z = 'T';
                            break;
                        case -'C' : *z = 'C';
                            break;
                        case -'F' : *z = 'F';
                            break;
                        case -'D' : *z = 'D';
                            break;
                    }

                }

                if(board1.board[6][j]=='P'){
                    switch(board2.board[7][j]){
                        case 'T' : *z = 'T';
                            break;
                        case 'C' : *z = 'C';
                            break;
                        case 'F' : *z = 'F';
                            break;
                        case 'D' : *z = 'D';
                            break;
                    }

                }
            }
}


void openF()   //ouvrir le fichier d'ouvertures..
{
   f= fopen("result.txt","r");
   if (!f)printf("the file can't be opened\n");
}

void closeF() //
{
    if (f){fclose(f);}
    else{
        printf("The file is not opened");
    }

}

void readF(char *line)  // lire du fichier tous les lignes qui commencent par la chaine #line et les mettre dans le tableau #tabBook
                        // on n'aura plus besoin du fichier mtnt on manipule seulement #tabBook qui est en MC
{
    char subString [256];
    char lineF [256];
    int stop =0,i;

    openF();
    while(fgets(lineF, 256, f))
    {
        strncpy(subString,lineF,strlen(line));
        char c =strstr(subString,line);
        if (c)
        {
            strcpy(tabBook[taille],lineF);
            taille++;
        }

    }
    int j;
    closeF();
}

char * nextHit(char *line) //elle a comme parametre l'historique des coups, elle retourne ainsi le prochain coup s'il y en a une correspondance
                           //dans le fichier sinon elle retourne NULL
{
    int t = 0;
    char* coup=NULL;
    char ** couptab = malloc(100 * sizeof(char*)); // #couptab contient tous les coups possible (dans le fichier d'ouverture)
    int k;
    //------------------------------------
    for(k=0 ; k<100 ; k++){
        couptab[k]=malloc(4*sizeof(char));
        sprintf(couptab[k],"%c",'#');
    }//-----------------------------------il sert à connaitre la taille du tableau :p

    char subString [256];
    int j=0;
    while (j<taille)
    {
        int lenght= strlen(line)+5;
        int i = lenght-4;
        if (lenght< strlen(tabBook[j]))
        {
            //strcpy(subString,"");
            strncpy(subString,tabBook[j],lenght);
            char c =strstr(subString,line);
            if (c)
            {
               coup =malloc(sizeof(char)*4);
               sprintf(coup,"%c",subString[i]);
               i++;
               while (i<lenght)
               {
                 sprintf(coup,"%s%c",coup,subString[i]);
                 i++;
               }

               if (!existe(couptab,coup,t)) sprintf(couptab[t++],"%s",coup);
            }
        }
       j++;
    }

   if(t>0){  // choisir un coup aléatoirement (c pour ainsi dire :  choisir une ligne dans le fichier aléatoirement) c plus judicieux ^^
         srand(time(NULL));
         int ind = rand()%t;

         return couptab[ind];
   }
   else return NULL;
}

//voir si la chaine #mot existe déjà dans le tableau #tab de taille #taille
int  existe(char ** tab, char *mot, int taille){
    int test = 0, i =0;
    while (!test && i<taille){
        if(!strcmp(tab[i],mot)) test =1;
        i++;
    }
    return test;
}




int MachineIA() /// La fonction qui permet à la machine de prendre la bonne decision de deplacement
{
    int n, m, i, j, score, cout, hauteur;


    struct configuration T[100];

    char cmd[5];

    int x1, y1, x2, y2;
    bool deplacer = false;
    bool fin = false;
    int choix;

    configuration board;
    configuration echequier;


    printf("Veuillez-vous entrer la profondeur de l'arbre:");
    scanf("%d", &hauteur);
    printf("\n\n");

    while((choix!=1)&&(choix!=2)){
    printf("MinMax Avec Elagage : 1\n");
    printf("MinMax Sans Elagage : 2\n");
    scanf("%d", &choix);
    }
    printf("\n\n");

    //hauteur=2;

    init(&board);
    affich(board);



    while(!fin)
    {
        dupliquerConfiguration(board, &echequier);
        if(roiEchec(board))
        {
            printf("Echec !");
        }
        printf("\n- Tour des blancs\n");
        while(!deplacer)
        {
            printf("Entrer votre déplacement: ");
            scanf("%s", cmd);
            if(strcmp(cmd, "0-0-0") == 0)
            {

                deplacer = grandRock(&board);
            }
            else
            {
                if(strcmp(cmd, "0-0") == 0)
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
                    if(deplacer && (x2 == 8))
                    {
                        transformerPiant(&board, x2 - 1, y2 - 1);
                    }
                }
            }
            if(deplacer)
            {

                if (firstHit == 1){
                    firstHit--;

                    if (strcmp(cmd,"0-0")==0)
                    {
                        strcat(H,"e1g1");
                    }
                    else{
                        if (strcmp(cmd,"0-0-0")==0){
                            strcat(H,"e1c1");
                        }
                        else{
                            strcat(H,cmd);
                        }
                    }

                    readF(cmd);
                }
                else{
                     strcat(H," ");

                     if (strcmp(cmd,"0-0")==0)
                    {
                        strcat(H,"e1g1");
                    }
                    else{
                        if (strcmp(cmd,"0-0-0")==0){
                            strcat(H,"e1c1");
                        }
                        else{
                            strcat(H,cmd);
                        }
                    }
                }

                board.turn--;
                if(roiEchec(board))
                {
                    deplacer = false;
                    dupliquerConfiguration(echequier, &board);
                }
                else
                {
                    board.turn++;
                }
            }
            if(!deplacer)
            {
                printf("Déplacement impossible!!!\n");
            }
        }
        affich(board);
        printf("\n- Tour des noirs\n");

        /** tour de la machine : ***/
        if (opening==1){
                char chaine [4];
                char *rep;
                rep = nextHit(H);
                if ( rep != NULL) {

                        strcpy(chaine,rep);
                        x1 = chaine[1] - '1' + 1;
                        y1 = chaine[0] - 'a' + 1;
                        x2 = chaine[3] - '1' + 1;
                        y2 = chaine[2] - 'a' + 1;

                        if(strcmp(chaine, "e8c8") == 0)
                            {
                                board.board[x2-1][y2-1] = board.board[x1-1][y1-1];
                                board.board[x1-1][y1-1] = 0;

                                board.board[7][3] = board.board[7][0];
                                board.board[7][0] = 0;
                                board.turn ++;
                                deplacer = false;
                            }
                            else
                            {
                                if(strcmp(chaine, "e8g8") == 0)
                                {
                                board.board[x2-1][y2-1] = board.board[x1-1][y1-1];
                                board.board[x1-1][y1-1] = 0;

                                board.board[7][5] = board.board[7][7];
                                board.board[7][7] = 0;

                                board.roque[2] = 1;
                                board.roque[3] = -1;
                                board.turn ++;
                                deplacer = false;
                                }
                                else
                                {
                                    board.board[x2-1][y2-1] = board.board[x1-1][y1-1];
                                    board.board[x1-1][y1-1] = 0;

                                    board.turn ++;
                                    deplacer = false;
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
                       // printf("le nombre de threads est :%d",omp_get_num_threads());

                        if(choix==1){
                            cout = minmax_ab( T[i], MAX, hauteur,-INFINI, +INFINI);
                        }else{
                            cout = minmax( T[i], MAX, hauteur);
                        }
                           #pragma omp critical
                           {
                               if ( cout < score ){
                               score = cout;
                               j = i;
                               }
                           }

                    }
                    if ( j != -1 ) {
                       copier( &T[j], &board );
                       board.val = score;
                    }

                }



                printf(" %s --> %s\n",H,nextHit(H));

                strcat(H," ");

                if (strcmp(chaine,"0-0")==0)
                    {
                        strcat(H,"e8g8");
                    }
                    else{
                        if (strcmp(chaine,"0-0-0")==0){
                            strcat(H,"e8c8");
                        }
                        else{
                            strcat(H,chaine);
                        }
                    }

        }
        else{
            n=cout_possible( T, board, MIN );
            score = +INFINI;
            j = -1;
            int beta =+INFINI;
            #pragma omp parallel for schedule (dynamic)
            for (i=0; i<n; i++) {
                if(choix==1){
                    cout = minmax_ab( T[i], MAX, hauteur,-INFINI, +INFINI);
                }else{
                    cout = minmax( T[i], MAX, hauteur);
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
               copier( &T[j], &board );
               board.val = score;
            }
        }

        deplacer = false;
        affich(board);
        m=cout_possible( T, board, MAX );
        if((n == 0) || (m == 0))
        {
            fin = true;
        }
        printf("\nn:%d         m:%d\n",n,m);
    }
    printf("Echec est mat !");
    return cout;
}
