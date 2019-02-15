#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include "GrilleSDL.h"
#include "Ressources.h"

// Dimensions de la grille de jeu
#define NB_LIGNES   21
#define NB_COLONNES 20

// Macros utilisees dans le tableau tab
#define VIDE                     0
#define BARRE                    -1
#define BILLE                    -2
#define BONUS                    -3

// Autres macros
#define NO                       100001
#define NE                       100002
#define SO                       100003
#define SE                       100004

int tab[NB_LIGNES][NB_COLONNES]
={ {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};

#define NB_BONUS_JAUNE  5      // la raquette s'allonge
#define NB_BONUS_VERT   5      // la raquette se retrecit
#define NB_BONUS_MAUVE  10     // multi-billes

typedef struct
{
  int L;
  int C;
  int couleur;
  int nbTouches;
  int brise;
  int bonus;
} S_BRIQUE;

#define NB_BRIQUES      56     // nombre de briques au depart
S_BRIQUE Briques[]
={ {2,2,GRIS,1,0,0},{2,4,GRIS,1,0,0},{2,6,GRIS,1,0,0},{2,8,GRIS,1,0,0},
   {2,10,GRIS,1,0,0},{2,12,GRIS,1,0,0},{2,14,GRIS,1,0,0},{2,16,GRIS,1,0,0},
   {3,2,MAUVE,2,0,0},{3,4,MAUVE,2,0,0},{3,6,MAUVE,2,0,0},{3,8,MAUVE,2,0,0},
   {3,10,MAUVE,2,0,0},{3,12,MAUVE,2,0,0},{3,14,MAUVE,2,0,0},{3,16,MAUVE,2,0,0},
   {4,2,GRIS,1,0,0},{4,4,GRIS,1,0,0},{4,6,GRIS,1,0,0},{4,8,GRIS,1,0,0},
   {4,10,GRIS,1,0,0},{4,12,GRIS,1,0,0},{4,14,GRIS,1,0,0},{4,16,GRIS,1,0,0},
   {5,2,VERT,2,0,0},{5,4,VERT,2,0,0},{5,6,VERT,2,0,0},{5,8,VERT,2,0,0},
   {5,10,VERT,2,0,0},{5,12,VERT,2,0,0},{5,14,VERT,2,0,0},{5,16,VERT,2,0,0},
   {6,2,GRIS,1,0,0},{6,4,GRIS,1,0,0},{6,6,GRIS,1,0,0},{6,8,GRIS,1,0,0},
   {6,10,GRIS,1,0,0},{6,12,GRIS,1,0,0},{6,14,GRIS,1,0,0},{6,16,GRIS,1,0,0},
   {7,2,BLEU,2,0,0},{7,4,BLEU,2,0,0},{7,6,BLEU,2,0,0},{7,8,BLEU,2,0,0},
   {7,10,BLEU,2,0,0},{7,12,BLEU,2,0,0},{7,14,BLEU,2,0,0},{7,16,BLEU,2,0,0},
   {8,2,GRIS,1,0,0},{8,4,GRIS,1,0,0},{8,6,GRIS,1,0,0},{8,8,GRIS,1,0,0},
   {8,10,GRIS,1,0,0},{8,12,GRIS,1,0,0},{8,14,GRIS,1,0,0},{8,16,GRIS,1,0,0}};

void Attente(int milli);

///////////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc,char* argv[])
{
  EVENT_GRILLE_SDL event;
  char ok;
 
  srand((unsigned)time(NULL));

  // Ouverture de la fenetre graphique
  printf("(THREAD MAIN %d) Ouverture de la fenetre graphique\n",pthread_self()); fflush(stdout);
  if (OuvertureFenetreGraphique() < 0)
  {
    printf("Erreur de OuvrirGrilleSDL\n");
    fflush(stdout);
    exit(1);
  }

  // Exemple d'utilisation de GrilleSDL et Ressources --> code a supprimer
  DessineRaquette(17,7,5);  // Attention !!! tab n'est pas modifie --> a vous de le faire !!!
  DessineChiffre(2,1,9);
  DessineBrique(5,9,ROUGE,0);
  DessineBrique(7,4,VERT,1);
  DessineBille(10,5,JAUNE);
  DessineDiamant(12,16,MAUVE);
  
  ok = 0;
  while(!ok)
  {
    event = ReadEvent();
    if (event.type == CROIX) ok = 1;
    if (event.type == CLAVIER)
    {
      switch(event.touche)
      {
        case 'q' : ok = 1; break;
        case KEY_RIGHT : printf("Fleche droite !\n"); break;
        case KEY_LEFT : printf("Fleche gauche !\n"); break;
        case KEY_SPACE : EffaceCarre(12,16); printf("Barre espace !\n"); break;
      }
    }
  }
  printf("Attente de 1500 millisecondes...\n");
  Attente(1500);

  // Fermeture de la fenetre
  printf("(THREAD MAIN %d) Fermeture de la fenetre graphique...",pthread_self()); fflush(stdout);
  FermetureFenetreGraphique();
  printf("OK\n"); fflush(stdout);

  exit(0);
}

//*********************************************************************************************
void Attente(int milli)
{
  struct timespec delai;
  delai.tv_sec = milli/1000;
  delai.tv_nsec = (milli%1000)*1000000;
  nanosleep(&delai,NULL);
}


