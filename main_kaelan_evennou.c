#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


//déclaration des constantes

#define MUR '#'
#define CAISSE '$'
#define CIBLE '.'
#define SOKO '@'
#define CAISSE_CIBLE '*'
#define SOKO_CIBLE '+'
#define ESPACE ' '
#define NB_LIGNES 12
#define NB_COLONNES 12
#define TAILLE 12
#define TAILLE_NOM_NIV 20
#define SOK 4
#define TAILLE_TAB_DEP 500
#define ZOOM_INIT 1
#define ZOOM_MAX 3
#define MOVE_Z 'z'
#define MOVE_S 's'
#define MOVE_D 'd'
#define MOVE_Q 'q'
#define HAUT_MAJ 'H'
#define BAS_MAJ 'B'
#define GAUCHE_MAJ 'G'
#define DROITE_MAJ 'D'
#define HAUT 'h'
#define BAS 'b'
#define GAUCHE 'g'
#define DROITE 'd'
#define OUI 'y'
#define NON 'n'
#define SOK_EXT ".sok"
#define VAR_TAB_Z_Q -1
#define VAR_TAB_S_D +1
#define VAR_TAB_2_Z_Q -2
#define VAR_TAB_2_S_D +2
#define TAILLE_SOKO 2
#define POS_TAB_MOVE -1
#define INCR -1
#define NEUTRE 'm'
#define DEP ".dep"
#define TOUCHE_MOINS '-'
#define TOUCHE_PLUS '+'
#define TOUCHE_R 'r'
#define TOUCHE_X 'x'
#define TOUCHE_U 'u'


//déclaration des types

typedef char t_Plateau[NB_LIGNES][NB_COLONNES];
typedef char t_Deplacement[TAILLE_TAB_DEP];

//déclaration des procédures

void affiche_entete(int nbCoups, char fichier[]);
void charger_partie(t_Plateau plateau, char fichier[]);
void enregistrer_partie(t_Plateau plateau, char fichier[]);
void afficher_plateau(t_Plateau plateau, int zoom);
void press_x(char touche, t_Plateau tab, t_Deplacement listeMove, int indiceMove);
void press_r(char touche, t_Plateau tab,char nomNiveau[], int *nbCoups, char soko[TAILLE_SOKO]);
void pos_soko(char soko[TAILLE_SOKO], t_Plateau tab);
void test_cible(t_Plateau tab, int i, int j);
void undo(t_Deplacement listeMove, t_Plateau plateau,char soko[TAILLE_SOKO], int * indiceMove, int * nbCoups);
void undo_h(t_Plateau plateau, char soko[TAILLE_SOKO], t_Deplacement listeMove, int indiceMove);
void undo_b(t_Plateau plateau, char soko[TAILLE_SOKO], t_Deplacement listeMove, int indiceMove);
void undo_g(t_Plateau plateau, char soko[TAILLE_SOKO], t_Deplacement listeMove, int indiceMove);
void undo_d(t_Plateau plateau, char soko[TAILLE_SOKO], t_Deplacement listeMove, int indiceMove);
void enregistrerDeplacements(t_Deplacement t, int nb, char fic[]);

//déclaration des fonctions

bool deplacer(char caractere, t_Plateau plateau, char soko[TAILLE_SOKO], t_Deplacement listeMove, int * indiceMove);
bool gagner(t_Plateau plateau,int nb);
int kbhit();
bool deplacement_z(t_Plateau plateau, char soko[TAILLE_SOKO], t_Deplacement listeMove, int indiceMove);
bool deplacement_s(t_Plateau plateau, char soko[TAILLE_SOKO], t_Deplacement listeMove, int indiceMove);
bool deplacement_d(t_Plateau plateau, char soko[TAILLE_SOKO], t_Deplacement listeMove, int indiceMove);
bool deplacement_q(t_Plateau plateau, char soko[TAILLE_SOKO], t_Deplacement listeMove, int indiceMove);
int zoom(char touche,int zoomNb);
int deplacement_soko(t_Deplacement listeMove, char touche, int indiceMove);
int deplacement_caisse(t_Deplacement listeMove, char touche, int indiceMove);

//début du main

int main(void){
    t_Plateau tab;
    t_Deplacement listeMove;
    int indiceMove=0, lenNomNiv = 0, nbCoups = 0, zoomNb = ZOOM_INIT;
    char niveau[TAILLE_NOM_NIV];
    char touche;
    char soko[TAILLE_SOKO];
    printf("Quel partie/niveau voulez-vous charger ?");
    scanf("%s", niveau);
    lenNomNiv = strlen(niveau) + SOK;    //calcul de la taille du nom du niveau
    char nomNiveau[lenNomNiv];
    strcpy(nomNiveau,niveau);
    strcat(nomNiveau,SOK_EXT);     //concaténation du nom du niveau et de l'etention .sok
    charger_partie(tab,nomNiveau);
    affiche_entete(nbCoups,nomNiveau);
    afficher_plateau(tab,zoomNb);
    pos_soko(soko, tab);
    touche = '\0';
    bool win=gagner(tab, nbCoups);
    while (!win){
        while (kbhit() == 0){   //recherche de l'appuie d'une touche
        }
        touche = getchar();
        if (touche == TOUCHE_X){    //test de l'appuie sur la touche x
            press_x(touche, tab, listeMove, indiceMove);
            break;
        }
        if (touche == TOUCHE_R){    //test de l'appuie sur la touche r
            press_r(touche, tab, nomNiveau, &nbCoups, soko);
        }
        if (touche == TOUCHE_U){    //test de l'appuie sur la touche u
            undo(listeMove, tab, soko, &indiceMove, &nbCoups);
        }
        if (deplacer(touche, tab, soko, listeMove, &indiceMove) == true){
            nbCoups++;     //incrémentation du nombre de coups si il y a un déplacement
        }
        if ((touche == TOUCHE_PLUS) || (touche == TOUCHE_MOINS)){
            zoomNb=zoom(touche,zoomNb);     //test de l'appuie des touches - et + pour le zoom
        }
        affiche_entete(nbCoups,nomNiveau);
        afficher_plateau(tab,zoomNb);     //affichage de la map et de l'entête
        win=gagner(tab, nbCoups);
    }
    if (win){   //test si le joueur a gagné
        printf("Souhaitez-vous sauvegarder vos déplacements ? (y/n)\n");
        while(kbhit() == 0){
        }
        touche = getchar();     //partie qui demande au joueur si il veut sauvegarder
        while(touche != OUI && touche != NON){
            printf("Touche incorrecte, veulliez taper une réponse valide.\n");
            while(kbhit() == 0){    //demande une touche tant que celle-ci n'est pas y ou n
            }
            touche = getchar();
        }
        if (touche == OUI){
            printf("Quelle nom voulez-vous donner à votre sauvegarde ? (20 caractères maximum) : ");
            char saveDep[TAILLE_NOM_NIV + 1];
            scanf("%s", saveDep);
            while (strlen(saveDep) > TAILLE_NOM_NIV){   //test le taille du nom du fichier
                printf("Trop de caractères ! Tapez un nouveau nom : ");
                scanf("%s", saveDep);
            }
            strcat(saveDep,DEP);   //concaténation et appelle de la fonction de sauvegarde des éléments de listeMove
            enregistrerDeplacements(listeMove, indiceMove, saveDep);
        }
    }
    return EXIT_SUCCESS;
}



//écriture des procédures


//partie du undo
void undo(t_Deplacement listeMove, t_Plateau plateau,char soko[TAILLE_SOKO], int * indiceMove, int * nbCoups){
    if ((listeMove[*indiceMove POS_TAB_MOVE]==HAUT) || (listeMove[*indiceMove POS_TAB_MOVE]==HAUT_MAJ)){
        undo_h(plateau, soko, listeMove, *indiceMove);
        listeMove[*indiceMove] = NEUTRE;
        *indiceMove=*indiceMove INCR;   //diminution de m'indice de la dernière position et du nombre de coups
        *nbCoups=*nbCoups INCR;

    }
    else if ((listeMove[*indiceMove POS_TAB_MOVE]==BAS) || (listeMove[*indiceMove POS_TAB_MOVE]==BAS_MAJ)){
        undo_b(plateau, soko, listeMove, *indiceMove);
        listeMove[*indiceMove] = NEUTRE;
        *indiceMove=*indiceMove INCR;  //diminution de m'indice de la dernière position et du nombre de coups
        *nbCoups=*nbCoups INCR;
    }
    else if ((listeMove[*indiceMove POS_TAB_MOVE]==GAUCHE) || (listeMove[*indiceMove POS_TAB_MOVE]==GAUCHE_MAJ)){
        undo_g(plateau, soko, listeMove, *indiceMove);
        listeMove[*indiceMove] = NEUTRE;
        *indiceMove=*indiceMove INCR;  //diminution de m'indice de la dernière position et du nombre de coups
        *nbCoups=*nbCoups INCR;
    }
    else if ((listeMove[*indiceMove POS_TAB_MOVE]==DROITE) || (listeMove[*indiceMove POS_TAB_MOVE]==DROITE_MAJ)){
        undo_d(plateau, soko, listeMove, *indiceMove);
        listeMove[*indiceMove] = NEUTRE;
        *indiceMove=*indiceMove INCR;  //diminution de m'indice de la dernière position et du nombre de coups
        *nbCoups=*nbCoups INCR;
    }
}

void undo_h(t_Plateau plateau, char soko[TAILLE_SOKO], t_Deplacement listeMove, int indiceMove){
    int i=soko[0];   //initialisation de la postion de sokoban
    int j=soko[1];
    if (listeMove[indiceMove POS_TAB_MOVE]==HAUT){   //différent test sur la prochaine postion de sokoban
        if (plateau[i VAR_TAB_S_D][j] == CIBLE){
            plateau[i VAR_TAB_S_D][j] = SOKO_CIBLE;
        }
        else{
            plateau[i VAR_TAB_S_D][j] = SOKO;
        }
        test_cible(plateau, i, j);
        soko[0]=i VAR_TAB_S_D;
    }
    if (listeMove[indiceMove POS_TAB_MOVE]==HAUT_MAJ){
        if (plateau[i VAR_TAB_S_D][j] == CIBLE){
            plateau[i VAR_TAB_S_D][j] = SOKO_CIBLE;
        }
        else{
            plateau[i VAR_TAB_S_D][j] = SOKO;
        }
        if (plateau[i][j] == SOKO_CIBLE){
            plateau[i][j] = CAISSE_CIBLE;
        }
        else{
            plateau[i][j] = CAISSE;
        }
        if (plateau[i VAR_TAB_Z_Q][j] == CAISSE_CIBLE){
            plateau[i VAR_TAB_Z_Q][j] = CIBLE;
        }
        else{
            plateau[i VAR_TAB_Z_Q][j] = ESPACE;
        }
        soko[0]=i VAR_TAB_S_D;
    }
}

void undo_b(t_Plateau plateau, char soko[TAILLE_SOKO], t_Deplacement listeMove, int indiceMove){
    int i=soko[0];   //initialisation de la postion de sokoban
    int j=soko[1];
    if (listeMove[indiceMove POS_TAB_MOVE]==BAS){     //différent test sur la prochaine postion de sokoban
        if (plateau[i VAR_TAB_Z_Q][j] == CIBLE){
            plateau[i VAR_TAB_Z_Q][j] = SOKO_CIBLE;
        }
        else{
            plateau[i VAR_TAB_Z_Q][j] = SOKO;
        }
        test_cible(plateau, i, j);
        soko[0]=i VAR_TAB_Z_Q;
    }
    if (listeMove[indiceMove POS_TAB_MOVE]==BAS_MAJ){
        if (plateau[i VAR_TAB_Z_Q][j] == CIBLE){
            plateau[i VAR_TAB_Z_Q][j] = SOKO_CIBLE;
        }
        else{
            plateau[i VAR_TAB_Z_Q][j] = SOKO;
        }
        if (plateau[i][j] == SOKO_CIBLE){
            plateau[i][j] = CAISSE_CIBLE;
        }
        else{
            plateau[i][j] = CAISSE;
        }
        if (plateau[i VAR_TAB_S_D][j] == CAISSE_CIBLE){
            plateau[i VAR_TAB_S_D][j] = CIBLE;
        }
        else{
            plateau[i VAR_TAB_S_D][j] = ESPACE;
        }
        soko[0]=i VAR_TAB_Z_Q;
    }
}

void undo_g(t_Plateau plateau, char soko[TAILLE_SOKO], t_Deplacement listeMove, int indiceMove){
    int i=soko[0];   //initialisation de la postion de sokoban
    int j=soko[1];
    if (listeMove[indiceMove POS_TAB_MOVE]==GAUCHE){   //différent test sur la prochaine postion de sokoban
        if (plateau[i][j VAR_TAB_S_D] == CIBLE){
            plateau[i][j VAR_TAB_S_D] = SOKO_CIBLE;
        }
        else{
            plateau[i][j VAR_TAB_S_D] = SOKO;
        }
        test_cible(plateau, i, j);
        soko[1]=j VAR_TAB_S_D;
    }
    if (listeMove[indiceMove POS_TAB_MOVE]==GAUCHE_MAJ){
        if (plateau[i][j VAR_TAB_S_D] == CIBLE){
            plateau[i][j VAR_TAB_S_D] = SOKO_CIBLE;
        }
        else{
            plateau[i][j VAR_TAB_S_D] = SOKO;
        }
        if (plateau[i][j] == SOKO_CIBLE){
            plateau[i][j] = CAISSE_CIBLE;
        }
        else{
            plateau[i][j] = CAISSE;
        }
        if (plateau[i][j VAR_TAB_Z_Q] == CAISSE_CIBLE){
            plateau[i][j VAR_TAB_Z_Q] = CIBLE;
        }
        else{
            plateau[i][j VAR_TAB_Z_Q] = ESPACE;
        }
        soko[1]=j VAR_TAB_S_D;
    }
}

void undo_d(t_Plateau plateau, char soko[TAILLE_SOKO], t_Deplacement listeMove, int indiceMove){
    int i=soko[0];   //initialisation de la postion de sokoban
    int j=soko[1];
    if (listeMove[indiceMove POS_TAB_MOVE]==DROITE){     //différent test sur la prochaine postion de sokoban
        if (plateau[i][j VAR_TAB_Z_Q] == CIBLE){
            plateau[i][j VAR_TAB_Z_Q] = SOKO_CIBLE;
        }
        else{
            plateau[i][j VAR_TAB_Z_Q] = SOKO;
        }
        test_cible(plateau, i, j);
        soko[1]=j VAR_TAB_Z_Q;
    }
    if (listeMove[indiceMove POS_TAB_MOVE]==DROITE_MAJ){
        if (plateau[i][j VAR_TAB_Z_Q] == CIBLE){
            plateau[i][j VAR_TAB_Z_Q] = SOKO_CIBLE;
        }
        else{
            plateau[i][j VAR_TAB_Z_Q] = SOKO;
        }
        if (plateau[i][j] == SOKO_CIBLE){
            plateau[i][j] = CAISSE_CIBLE;
        }
        else{
            plateau[i][j] = CAISSE;
        }
        if (plateau[i][j VAR_TAB_S_D] == CAISSE_CIBLE){
            plateau[i][j VAR_TAB_S_D] = CIBLE;
        }
        else{
            plateau[i][j VAR_TAB_S_D] = ESPACE;
        }
        soko[1]=j VAR_TAB_Z_Q;
    }
}

//fin partie du undo

void enregistrerDeplacements(t_Deplacement t, int nb, char fic[]){
    FILE * f;

    f = fopen(fic, "w");
    fwrite(t,sizeof(char), nb, f);
    fclose(f);
}


void press_r(char touche, t_Plateau tab,char nomNiveau[], int *nbCoups, char soko[TAILLE_SOKO]){
    printf("Voulez-vous vraiment recommencer ?(y/n)\n");
    while(kbhit() == 0){
        }
        touche = getchar();
    while(touche != OUI && touche != NON){   //demande une touche tant que celle-ci n'est pas y ou n
        printf("Touche incorrecte, veulliez taper une réponse valide.\n");
        while(kbhit() == 0){
        }
        touche = getchar();
    }
    if (touche == OUI){
        charger_partie(tab,nomNiveau);   //on appelle la procédure charger_partie pour recharger le niveau
        pos_soko(soko, tab);
        *nbCoups = 0;
    }
}

void press_x(char touche, t_Plateau tab, t_Deplacement listeMove, int indiceMove){
    printf("Souhaitez-vous sauvegarder votre partie ? (y/n)\n");
    while(kbhit() == 0){
    }
    touche = getchar();
    while(touche != OUI && touche != NON){   //demande une touche tant que celle-ci n'est pas y ou n
        printf("Touche incorrecte, veulliez taper une réponse valide.\n");
        while(kbhit() == 0){
        }
        touche = getchar();
    }
    if (touche == OUI){
        printf("Quelle nom voulez-vous donner à votre sauvegarde ? (20 caractères maximum) : ");
        char save[TAILLE_NOM_NIV + 1];   //demande de la taille du nom du niveau
        scanf("%s", save);
        while (strlen(save) > TAILLE_NOM_NIV){   //demande une touche tant que celle-ci n'est pas y ou n
            printf("Trop de caractères ! Tapez un nouveau nom : ");
            scanf("%s", save);
        }
        strcat(save,SOK_EXT);
        enregistrer_partie(tab,save);   //création d'un fichier sauvegarde
    }
    else{
        printf("Dommage, une prochaine fois peut-être...\n");
    }

    printf("Souhaitez-vous sauvegarder vos déplacements ? (y/n)\n");
    while(kbhit() == 0){
    }
    touche = getchar();
    while(touche != OUI && touche != NON){   //demande une touche tant que celle-ci n'est pas y ou n
        printf("Touche incorrecte, veulliez taper une réponse valide.\n");
        while(kbhit() == 0){
        }
        touche = getchar();
    }
    if (touche == OUI){
        printf("Quelle nom voulez-vous donner à votre sauvegarde ? (20 caractères maximum) : ");
        char saveDep[TAILLE_NOM_NIV + 1];   //demande de la taille du nom du fichier de déplacement
        scanf("%s", saveDep);
        while (strlen(saveDep) > TAILLE_NOM_NIV){   //demande une touche tant que celle-ci n'est pas y ou n
            printf("Trop de caractères ! Tapez un nouveau nom : ");
            scanf("%s", saveDep);
        }
        strcat(saveDep,DEP);
        enregistrerDeplacements(listeMove, indiceMove, saveDep);  //création d'un fichier de sauvegarde des déplacements
    }
}

void charger_partie(t_Plateau plateau, char fichier[]){
    FILE * f;
    char finDeLigne;

    f = fopen(fichier, "r");
    if (f==NULL){
        printf("ERREUR SUR FICHIER");
        exit(EXIT_FAILURE);
    } else {
        for (int ligne=0 ; ligne<TAILLE ; ligne++){
            for (int colonne=0 ; colonne<TAILLE ; colonne++){
                fread(&plateau[ligne][colonne], sizeof(char), 1, f);
            }
            fread(&finDeLigne, sizeof(char), 1, f);
        }
        fclose(f);
    }
}
    
void affiche_entete(int nbCoups, char fichier[]){
    system("clear");
    printf("---------------------------------------------------------------\n");
    printf("         Map : %s             Coups : %d\n", fichier, nbCoups);
    printf("Directions : z--->haut   s--->bas   q--->gauche   d--->droite\n");
    printf("  Recommencer la partie : r                   Abandonner : x \n");
    printf("                 Annuler le déplacement : u                  \n");
    printf("---------------------------------------------------------------\n");
}

void enregistrer_partie(t_Plateau plateau, char fichier[]){
    FILE * f;
    char finDeLigne='\n';

    f = fopen(fichier, "w");
    for (int ligne=0 ; ligne<TAILLE ; ligne++){
        for (int colonne=0 ; colonne<TAILLE ; colonne++){
            fwrite(&plateau[ligne][colonne], sizeof(char), 1, f);
        }
        fwrite(&finDeLigne, sizeof(char), 1, f);
    }
    fclose(f);
}

void afficher_plateau(t_Plateau plateau, int zoom){
    for (int i = 0; i < NB_LIGNES;i++){
        for (int k=0; k<zoom; k++){   //pour doubler les lignes
            for (int j = 0; j < NB_COLONNES;j++){
                for (int l=0; l<zoom; l++){   //pour doubler les colonnes
                    if(plateau[i][j] == SOKO_CIBLE){
                        printf("%c", SOKO);
                    }
                    else if (plateau[i][j] == CAISSE_CIBLE){
                        printf("%c", CAISSE);
                    }
                    else{
                        printf("%c", plateau[i][j]);
                    }
                }
            } 
            printf("\n"); 
        }   
    }
}

void pos_soko(char soko[TAILLE_SOKO], t_Plateau plateau){
    for (int i = 0; i < NB_LIGNES;i++){
        for (int j = 0; j < NB_COLONNES;j++){
            if ((SOKO == plateau[i][j]) || (SOKO_CIBLE == plateau[i][j])){
                soko[0] = i;
                soko[1] = j;   //si la position de sokoban est trouvée, ajoute dans soko les coordonnées de sokoban
            }
        }
    }
}



//écriture des fonctions

int zoom(char touche,int zoomNb){
    if (touche == TOUCHE_PLUS){
        if (zoomNb != ZOOM_MAX){
            zoomNb++;   //incrémentation du zoom par 1
        }
    }
    else{
        if (zoomNb != ZOOM_INIT){
            zoomNb--;   //incrémentation du zoom par -1
        }
    }
    return zoomNb;
}

int kbhit(){
	// la fonction retourne :
	// 1 si un caractere est present
	// 0 si pas de caractere présent
	int unCaractere=0;
	struct termios oldt, newt;
	int ch;
	int oldf;

	// mettre le terminal en mode non bloquant
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
 
	ch = getchar();

	// restaurer le mode du terminal
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	fcntl(STDIN_FILENO, F_SETFL, oldf);
 
	if(ch != EOF){
		ungetc(ch, stdin);
		unCaractere=1;
	}
	return unCaractere;
}

bool deplacer(char caractere, t_Plateau plateau, char soko[TAILLE_SOKO], t_Deplacement listeMove, int * indiceMove){
    if (caractere == MOVE_Z){
        if (deplacement_z(plateau, soko, listeMove, *indiceMove) == true){
            *indiceMove=*indiceMove+1;    //si il y a un déplacment, incrémentation du nombre de déplacments pas 1
            return true;
        }
        return false;
    }
    else if (caractere == MOVE_S){
        if (deplacement_s(plateau, soko, listeMove, *indiceMove) == true){
            *indiceMove=*indiceMove+1;   //si il y a un déplacment, incrémentation du nombre de déplacments pas 1
            return true;
        }
        return false;
    }
    else if (caractere == MOVE_D){
        if (deplacement_d(plateau, soko, listeMove, *indiceMove) == true){
            *indiceMove=*indiceMove+1;  //si il y a un déplacment, incrémentation du nombre de déplacments pas 1
            return true;
        }
        return false;
    }
    else if (caractere == MOVE_Q){
        if (deplacement_q(plateau, soko, listeMove, *indiceMove) == true){
            *indiceMove=*indiceMove+1;    //si il y a un déplacment, incrémentation du nombre de déplacments pas 1
            return true;
        }
        return false;
    }
    return false;
}

bool gagner(t_Plateau plateau, int nb){
    for (int l = 0; l < NB_LIGNES;l++){
        for (int m = 0; m < NB_COLONNES;m++){
            if ((plateau[l][m] == CIBLE) || (plateau[l][m] == SOKO_CIBLE)){
                return false;   //si une ciblre est trouvé sans une caisse dessus, le joueur gagne
            }
        }
    }
    printf("Bravo, vous avez gagné en %d coups!\n", nb);
    return true;
}

void test_cible(t_Plateau plateau, int i, int j){
    if (SOKO_CIBLE == plateau[i][j]){
        plateau[i][j] = CIBLE;   //test si la case sur laquelle on se trouve est une cible
    }
    else{
        plateau[i][j] = ESPACE;
    }
}


//section en rapport avec l'appuie de z

bool deplacement_z(t_Plateau plateau, char soko[TAILLE_SOKO], t_Deplacement listeMove, int indiceMove){
    int i = soko[0];   //initialisation de la postion de sokoban
    int j = soko[1];
    if (plateau[i VAR_TAB_Z_Q][j] == ESPACE){   //différent test sur la prochaine postion de sokoban et de la caisse qu'il peut pousser
        plateau[i VAR_TAB_Z_Q][j] = SOKO;
        soko[0]=i VAR_TAB_Z_Q;
        soko[1]=j;
        test_cible(plateau, i, j);
        listeMove[indiceMove]=HAUT;
        return true;
    }
    else if (plateau[i VAR_TAB_Z_Q][j] == CIBLE){
        plateau[i VAR_TAB_Z_Q][j] = SOKO_CIBLE;
        soko[0]=i VAR_TAB_Z_Q;
        soko[1]=j;
        test_cible(plateau, i, j);
        listeMove[indiceMove]=HAUT;
        return true;
    }
    else if (((plateau[i VAR_TAB_Z_Q][j] == CAISSE) || (plateau[i VAR_TAB_Z_Q][j] == CAISSE_CIBLE)) && ((plateau[i VAR_TAB_2_Z_Q][j] == ESPACE) || (plateau[i VAR_TAB_2_Z_Q][j] == CIBLE))){
        if (plateau[i VAR_TAB_Z_Q][j] == CAISSE_CIBLE){
            plateau[i VAR_TAB_Z_Q][j] = SOKO_CIBLE;
            soko[0]=i VAR_TAB_Z_Q;
            soko[1]=j;
            if (plateau[i VAR_TAB_2_Z_Q][j] == CIBLE){
                plateau[i VAR_TAB_2_Z_Q][j] = CAISSE_CIBLE;
            }
            else{
                plateau[i VAR_TAB_2_Z_Q][j] = CAISSE;
            }
            test_cible(plateau, i, j);
            listeMove[indiceMove]=HAUT_MAJ;
            return true;
        }
        else if (plateau[i VAR_TAB_2_Z_Q][j] == ESPACE){
            plateau[i VAR_TAB_Z_Q][j] = SOKO;
            soko[0]=i VAR_TAB_Z_Q;
            soko[1]=j;
            plateau[i VAR_TAB_2_Z_Q][j] = CAISSE;
            test_cible(plateau, i, j);
            listeMove[indiceMove]=HAUT_MAJ;
            return true;
        }
        else if (plateau[i VAR_TAB_2_Z_Q][j] == CIBLE){
            plateau[i VAR_TAB_Z_Q][j] = SOKO;
            soko[0]=i VAR_TAB_Z_Q;
            soko[1]=j;
            plateau[i VAR_TAB_2_Z_Q][j] = CAISSE_CIBLE;
            test_cible(plateau, i, j);
            listeMove[indiceMove]=HAUT_MAJ;
            return true;
        }
    }
    return false;
}






//section en rapport avec l'appuie de s

bool deplacement_s(t_Plateau plateau, char soko[TAILLE_SOKO], t_Deplacement listeMove, int indiceMove){
    int i = soko[0];   //initialisation de la postion de sokoban
    int j = soko[1];
    if (plateau[i VAR_TAB_S_D][j]==ESPACE){   //différent test sur la prochaine postion de sokoban et de la caisse qu'il peut pousser
        plateau[i VAR_TAB_S_D][j]=SOKO;
        soko[0]=i VAR_TAB_S_D;
        soko[1]=j;
        test_cible(plateau, i, j);
        listeMove[indiceMove]=BAS;
        return true;   
    }
    else if (plateau[i VAR_TAB_S_D][j]==CIBLE){
        plateau[i VAR_TAB_S_D][j]=SOKO_CIBLE;
        soko[0]=i VAR_TAB_S_D;
        soko[1]=j;
        test_cible(plateau, i, j);
        listeMove[indiceMove]=BAS;
        return true;
    }
    else if (((plateau[i VAR_TAB_S_D][j]==CAISSE) || (plateau[i VAR_TAB_S_D][j]==CAISSE_CIBLE)) && ((plateau[i VAR_TAB_2_S_D][j]==ESPACE) || (plateau[i VAR_TAB_2_S_D][j]==CIBLE))){
        if (plateau[i VAR_TAB_S_D][j]==CAISSE_CIBLE){
            plateau[i VAR_TAB_S_D][j]=SOKO_CIBLE;
            soko[0]=i VAR_TAB_S_D;
            soko[1]=j;
            if (plateau[i VAR_TAB_2_S_D][j]==CIBLE){
                plateau[i VAR_TAB_2_S_D][j]=CAISSE_CIBLE;
            }
            else{
                plateau[i VAR_TAB_2_S_D][j]=CAISSE;
            }
            test_cible(plateau, i, j);
            listeMove[indiceMove]=BAS_MAJ;
            return true;
        }
        else if (plateau[i VAR_TAB_2_S_D][j]==ESPACE){
            plateau[i VAR_TAB_S_D][j]=SOKO;
            soko[0]=i VAR_TAB_S_D;
            soko[1]=j;
            plateau[i VAR_TAB_2_S_D][j]=CAISSE;
            test_cible(plateau, i, j);
            listeMove[indiceMove]=BAS_MAJ;
            return true;
        }
        else if (plateau[i VAR_TAB_2_S_D][j]==CIBLE){
            plateau[i VAR_TAB_S_D][j]=SOKO;
            soko[0]=i VAR_TAB_S_D;
            soko[1]=j;
            plateau[i VAR_TAB_2_S_D][j]=CAISSE_CIBLE;
            test_cible(plateau, i, j);
            listeMove[indiceMove]=BAS_MAJ;
            return true;
        }
    }
    return false;
}
        

    






//section en rapport avec d

bool deplacement_d(t_Plateau plateau, char soko[TAILLE_SOKO], t_Deplacement listeMove, int indiceMove){
    int i = soko[0];   //initialisation de la postion de sokoban
    int j = soko[1];
    if (plateau[i][j VAR_TAB_S_D]==ESPACE){   //différent test sur la prochaine postion de sokoban et de la caisse qu'il peut pousser
        plateau[i][j VAR_TAB_S_D]=SOKO;
        soko[0]=i;
        soko[1]=j VAR_TAB_S_D;
        test_cible(plateau, i, j);
        listeMove[indiceMove]=DROITE;
        return true;
    }
    else if (plateau[i][j VAR_TAB_S_D]==CIBLE){
        plateau[i][j VAR_TAB_S_D]=SOKO_CIBLE;
        soko[0]=i;
        soko[1]=j VAR_TAB_S_D;
        test_cible(plateau, i, j);
        listeMove[indiceMove]=DROITE;
        return true;
    }
    else if (((plateau[i][j VAR_TAB_S_D]==CAISSE) || (plateau[i][j VAR_TAB_S_D]==CAISSE_CIBLE)) && ((plateau[i][j VAR_TAB_2_S_D]==ESPACE) || (plateau[i][j VAR_TAB_2_S_D]==CIBLE))){
        if (plateau[i][j VAR_TAB_S_D]==CAISSE_CIBLE){
            plateau[i][j VAR_TAB_S_D]=SOKO_CIBLE;
            soko[0]=i;
            soko[1]=j VAR_TAB_S_D;
            if (plateau[i][j VAR_TAB_2_S_D]==CIBLE){
                plateau[i][j VAR_TAB_2_S_D]=CAISSE_CIBLE;
            }
            else{
                plateau[i][j VAR_TAB_2_S_D]=CAISSE;
            }
            test_cible(plateau, i, j);
            listeMove[indiceMove]=DROITE_MAJ;
            return true;
        }
        else if (plateau[i][j VAR_TAB_2_S_D]==ESPACE){
            plateau[i][j VAR_TAB_S_D]=SOKO;
            soko[0]=i;
            soko[1]=j VAR_TAB_S_D;
            plateau[i][j VAR_TAB_2_S_D]=CAISSE;
            test_cible(plateau, i, j);
            listeMove[indiceMove]=DROITE_MAJ;
            return true;
        }
        else if (plateau[i][j VAR_TAB_2_S_D]==CIBLE){
            plateau[i][j VAR_TAB_S_D]=SOKO;
            soko[0]=i;
            soko[1]=j VAR_TAB_S_D;
            plateau[i][j VAR_TAB_2_S_D]=CAISSE_CIBLE;
            test_cible(plateau, i, j);
            listeMove[indiceMove]=DROITE_MAJ;
            return true;
        }
    }
    return false;
}
    







//section en rapport avec q

bool deplacement_q(t_Plateau plateau, char soko[TAILLE_SOKO], t_Deplacement listeMove, int indiceMove){
    int i = soko[0];   //initialisation de la postion de sokoban
    int j = soko[1];
    if (plateau[i][j VAR_TAB_Z_Q]==ESPACE){   //différent test sur la prochaine postion de sokoban et de la caisse qu'il peut pousser
        plateau[i][j VAR_TAB_Z_Q]=SOKO;
        test_cible(plateau, i, j);
        soko[0]=i;
        soko[1]=j VAR_TAB_Z_Q;
        listeMove[indiceMove]=GAUCHE;
        return true;
    }
    else if (plateau[i][j VAR_TAB_Z_Q]==CIBLE){
        soko[0]=i;
        soko[1]=j VAR_TAB_Z_Q;
        plateau[i][j VAR_TAB_Z_Q]=SOKO_CIBLE;
        test_cible(plateau, i, j);
        listeMove[indiceMove]=GAUCHE;
        return true;
    }
    else if (((plateau[i][j VAR_TAB_Z_Q]==CAISSE) || (plateau[i][j VAR_TAB_Z_Q]==CAISSE_CIBLE)) && ((plateau[i][j VAR_TAB_2_Z_Q]==ESPACE) || (plateau[i][j VAR_TAB_2_Z_Q]==CIBLE))){
        if (plateau[i][j VAR_TAB_Z_Q]==CAISSE_CIBLE){
            plateau[i][j VAR_TAB_Z_Q]=SOKO_CIBLE;
            soko[0]=i;
            soko[1]=j VAR_TAB_Z_Q;
            if (plateau[i][j VAR_TAB_2_Z_Q]==CIBLE){
                plateau[i][j VAR_TAB_2_Z_Q]=CAISSE_CIBLE;
            }
            else{
                plateau[i][j VAR_TAB_2_Z_Q]=CAISSE;
            }
            test_cible(plateau, i, j);
            listeMove[indiceMove]=GAUCHE_MAJ;
            return true;
        }
        else if (plateau[i][j VAR_TAB_2_Z_Q]==ESPACE){
            plateau[i][j VAR_TAB_Z_Q]=SOKO;
            plateau[i][j VAR_TAB_2_Z_Q]=CAISSE;
            soko[0]=i;
            soko[1]=j VAR_TAB_Z_Q;
            test_cible(plateau, i, j);
            listeMove[indiceMove]=GAUCHE_MAJ;
            return true;
        }
        else if (plateau[i][j VAR_TAB_2_Z_Q]==CIBLE){
            plateau[i][j VAR_TAB_Z_Q]=SOKO;
            plateau[i][j VAR_TAB_2_Z_Q]=CAISSE_CIBLE;
            soko[0]=i;
            soko[1]=j VAR_TAB_Z_Q;
            test_cible(plateau, i, j);
            listeMove[indiceMove]=GAUCHE_MAJ;
            return true;
        }
    }
    return false;
}
