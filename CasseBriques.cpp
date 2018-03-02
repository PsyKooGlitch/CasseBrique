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

typedef struct
{
	int L;
	int C;
	int longeur;
	bool billeSurRaquette;
} S_RAQUETTE;

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
void effacer(int l,int c, int longeur);


//Raquette
void * raquetteThread (void *);
void destructeurraq(void *p);
void HandleRaquetteSig(int sig);

//Event
void * eventThread (void *);

//Def
pthread_key_t macle;
pthread_mutex_t mutextab;
///////////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc,char* argv[])
{
	struct sigaction SigAct;
	sigset_t mask;
	pthread_t HanleRaquette;
	pthread_t HandleEvent;
  
  EVENT_GRILLE_SDL event;
  char ok;
  pthread_mutex_init(&mutextab,NULL);
  srand((unsigned)time(NULL));
	
  // Ouverture de la fenetre graphique
  printf("(THREAD MAIN %d) Ouverture de la fenetre graphique\n",pthread_self()); fflush(stdout);
  if (OuvertureFenetreGraphique() < 0)
  {
    printf("Erreur de OuvrirGrilleSDL\n");
    fflush(stdout);
    exit(1);
  }
	
	//Masque les signaux
	sigemptyset(&mask);
	sigaddset(&mask, SIGUSR1);
	sigaddset(&mask, SIGUSR2);
	sigaddset(&mask, SIGHUP);
	sigprocmask(SIG_SETMASK, &mask,NULL);
	
	
	pthread_create(&HanleRaquette, NULL, (void *(*) (void *))raquetteThread, NULL);
	pthread_create(&HandleEvent, NULL, (void *(*) (void *))eventThread, NULL);
  // Exemple d'utilisation de GrilleSDL et Ressources --> code a supprimer
  //DessineRaquette(17,7,5);  // Attention !!! tab n'est pas modifie --> a vous de le faire !!!
  pthread_mutex_lock(&mutextab);
  DessineChiffre(2,1,9);
  DessineBrique(5,9,ROUGE,0);
  DessineBrique(7,4,VERT,1);
  DessineBille(10,5,JAUNE);
  DessineDiamant(12,16,MAUVE);
  pthread_mutex_unlock(&mutextab);
  
//  ok = 0;
//  while(!ok)
//  {
//    event = ReadEvent();
//    if (event.type == CROIX) ok = 1;
//    if (event.type == CLAVIER)
//    {
//      switch(event.touche)
//      {
//        case 'q' : ok = 1; break;
//        case KEY_RIGHT : printf("Fleche droite !\n"); break;
//        case KEY_LEFT : printf("Fleche gauche !\n"); break;
//        case KEY_SPACE : EffaceCarre(12,16); printf("Barre espace !\n"); break;
//      }
//    }
//  }
//  printf("Attente de 1500 millisecondes...\n");
//  Attente(1500);

  pthread_join(HandleEvent,NULL);

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


void * raquetteThread (void *)
{
	sigset_t mask;
	struct sigaction SigAct;
	S_RAQUETTE * raquette;
	pthread_key_create(&macle, destructeurraq);

	 raquette = (S_RAQUETTE *)malloc(sizeof(S_RAQUETTE));
	 raquette->L = 19;
	 raquette->C = 10;
	 raquette->longeur = 5;
	 raquette->billeSurRaquette = true;
	 pthread_setspecific(macle,(void *)raquette);
	 
	 
	 	//Masque signal
	sigemptyset(&mask);
	sigprocmask(SIG_SETMASK, &mask,NULL);
	//Arme les sig
	SigAct.sa_handler = HandleRaquetteSig;
	SigAct.sa_flags = 0;
	sigaction(SIGUSR1,&SigAct, NULL);
	sigaction(SIGUSR2,&SigAct, NULL);
	
	 pthread_mutex_lock(&mutextab);
	 DessineRaquette(raquette->L,raquette->C,raquette->longeur);
	 DessineBille(raquette->L-1,raquette->C,ROUGE);
	 pthread_mutex_unlock(&mutextab);
	 while(1);
}

void destructeurraq(void *p)
{
puts("Je me libere (Raquette)");
free(p);
}


void * eventThread (void *)
{
	EVENT_GRILLE_SDL event;
	while(1)
	{
		event = ReadEvent();
		if(event.type == CROIX){pthread_exit(0);}
		switch (event.touche){
		case (KEY_LEFT):
			kill(getpid(), SIGUSR1);
			break;
		case KEY_RIGHT:
			kill(getpid(), SIGUSR2);
			break;
		case KEY_SPACE:
			kill(getpid(), SIGHUP);
			break;
		default:
		break;
			}
	}	
	
	
	return 0;
}

void HandleRaquetteSig(int sig)
{
	char buffer[80];
	int positiondebut;
	int positionfin;
	S_RAQUETTE * raquettept;
	//Recup de la variable specifique
	raquettept =  (S_RAQUETTE *)pthread_getspecific(macle);
	positiondebut = raquettept->C - ((raquettept->longeur-1)/2);
	positionfin = raquettept->C + ((raquettept->longeur-1)/2);
	if(sig == SIGUSR1){
		 if(positiondebut > 0)
		 {
		 	
		 	effacer(19,raquettept->C,raquettept->longeur);
		 	raquettept->C = raquettept->C - 1;
		 	if(raquettept->billeSurRaquette == true)
		 	{	
		 		effacer(raquettept->L-1,raquettept->C+1,1);
		 		pthread_mutex_lock(&mutextab);
		 		DessineBille(raquettept->L-1,raquettept->C,ROUGE);
		 		pthread_mutex_unlock(&mutextab);
		 	}
		 	
		 }
	}
	else
	{
		if(sig == SIGUSR2){
			if(positionfin < 19)
			{
				//pthread_mutex_lock(&mutextab);
				effacer(19,positiondebut,raquettept->longeur);
				raquettept->C = raquettept->C + 1;
				if(raquettept->billeSurRaquette == true)
		 		{	
		 			effacer(raquettept->L-1,raquettept->C-1,1);
		 			pthread_mutex_lock(&mutextab);
		 			DessineBille(raquettept->L-1,raquettept->C,ROUGE);
		 			pthread_mutex_unlock(&mutextab);
		 		}
				
			}
		}
	}
	positiondebut = raquettept->C - ((raquettept->longeur-1)/2);
	pthread_mutex_lock(&mutextab);
	DessineRaquette(raquettept->L,raquettept->C,raquettept->longeur);
	pthread_mutex_unlock(&mutextab);
}

void effacer(int l,int c, int longeur)
{
	int i;
	pthread_mutex_lock(&mutextab);
	for(i=0;i<longeur;i++)
	{
		EffaceCarre(l,c);
		c++;
	}
	pthread_mutex_unlock(&mutextab);
}


