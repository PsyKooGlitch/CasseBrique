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

//Macro random
#define random(n)(rand()%(n)+1)
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

#define NB_BONUS_JAUNE  15      // la raquette s'allonge
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

typedef struct
{
	int L;
	int C;
	int dir;
	int couleur;
} S_BILLE;

typedef struct
{
	int L;
	int C;
	int couleur;
}S_BONUS;


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
void DessineRaquette2(int l, int c, int longeur);
int raquettepid;
//Bille
void destructeurbille(void *p);
void * raquetteThread (S_BILLE *);
void avancebille(S_BILLE * pbille);
void DessineBille2(int l, int c, int couleur);
void impacte(int qui);
int temps=200;
void HandleBilleSig(int sig);
void initCleBille();
pthread_once_t billecontroler = PTHREAD_ONCE_INIT;

//Brique
void DessineBrique2(int l, int c, int couleur, int brise);
void * briqueThread (S_BRIQUE *);
void HandleBriqueSig(int sig);
void destructeurbri(void *p);
pthread_once_t briquecontroler = PTHREAD_ONCE_INIT;
void initCleBrique();

//Score
void * scoreThread ();
int score;
pthread_mutex_t mutexScore;
pthread_cond_t condScore;
bool MAJScore = true;

//Niveau
void * niveauThread();
void DessineVie(int vie);
void DessineNiveau(int niveau);
int nbBriques;
int nbBilles;
bool niveauFini;

//Event
void * eventThread (void *);

//Bonus
void * bonusThread(S_BONUS * bonuspt);
void DessineBonus2(int l, int c, int couleur);
void effacerBonus(int l, int c);

//Def
pthread_key_t macle;
pthread_key_t cleBille;
pthread_key_t cleBrique;
pthread_mutex_t mutextab;
pthread_mutex_t mutexBilleBrique;
pthread_mutex_t mutexNiveauFinit;
pthread_mutex_t mutexRaquettepid;
pthread_cond_t condBilleBrique;
pthread_cond_t condBrique;
//
int main(int argc,char* argv[])
{
	struct sigaction SigAct;
	pthread_t HanleRaquette;
	pthread_t HandleEvent;
	pthread_t HandleNiveau;
   pthread_t HandleScore;
  EVENT_GRILLE_SDL event;
  char ok;

  pthread_mutex_init(&mutextab,NULL);
  pthread_mutex_init(&mutexBilleBrique,NULL);
  pthread_mutex_init(&mutexNiveauFinit,NULL);
  pthread_mutex_init(&mutexScore,NULL);
  pthread_mutex_init(&mutexRaquettepid,NULL);
  pthread_cond_init(&condBilleBrique, NULL);
  pthread_cond_init(&condBrique, NULL);
  pthread_cond_init(&condScore, NULL);
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
	sigset_t mask;
	
	sigemptyset(&mask);
	sigaddset(&mask, SIGUSR1);
	sigaddset(&mask, SIGUSR2);
	sigaddset(&mask, SIGHUP);
	sigaddset(&mask, SIGTRAP);
	sigaddset(&mask, SIGINT);
	sigaddset(&mask, SIGURG);
	sigaddset(&mask, SIGPIPE);
	sigaddset(&mask, SIGSYS);
	sigaddset(&mask, SIGALRM);
	sigprocmask(SIG_SETMASK, &mask,NULL);
	
	pthread_create(&HandleScore, NULL, (void *(*) (void *))scoreThread, NULL);
	pthread_create(&HanleRaquette, NULL, (void *(*) (void *))raquetteThread, NULL);
	pthread_create(&HandleEvent, NULL, (void *(*) (void *))eventThread, NULL);

  pthread_create(&HandleNiveau, NULL, (void *(*) (void *))niveauThread, NULL);
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

void * billeThread(S_BILLE * pbillept)
{
	S_BILLE * pbille;
	pbille = (S_BILLE *)malloc(sizeof(S_BILLE));
	memcpy(pbille, pbillept, sizeof(S_BILLE));
	sigset_t mask;
	
	sigemptyset(&mask);
	sigaddset(&mask, SIGUSR1);
	sigaddset(&mask, SIGUSR2);
	sigaddset(&mask, SIGHUP);
	sigaddset(&mask, SIGTRAP);
	sigaddset(&mask, SIGINT);
	sigaddset(&mask, SIGURG);
	sigaddset(&mask, SIGPIPE);
	sigaddset(&mask, SIGSYS);
	sigaddset(&mask, SIGALRM);
	sigprocmask(SIG_SETMASK, &mask,NULL);


	struct sigaction SigAct;
	
	SigAct.sa_handler = HandleBilleSig;
	SigAct.sa_flags = SA_SIGINFO;
	sigaction(SIGEMT,&SigAct, NULL);

	pthread_once(&billecontroler, initCleBille);
	pthread_setspecific(cleBille,pbille);
	
	
	pthread_mutex_lock(&mutextab);
	DessineBille(pbille->L,pbille->C,pbille->couleur);
	tab[pbille->L][pbille->C] = -2;
	pthread_mutex_unlock(&mutextab);
	int letemps = temps;
	while(1)
	{
		Attente(letemps);
		effacer(pbille->L,pbille->C,1);
		//Test dessous
		if(pbille->L==20)
		{
		pthread_exit(0);
		}
		//Test bords
		//Bords gauche
		if(pbille->C == 0 or pbille->C == 19){
			switch (pbille->dir){
				case NE:
					pbille->dir = NO;
				break;
				case NO:
					pbille->dir = NE;
				break;
				case SE:
					pbille->dir = SO;
				break;
				case SO:
					pbille->dir = SE;
				break;
				}
		}
		//test dessus
		if(pbille->L == 1)
		{
		switch (pbille->dir){
			case NE:
				pbille->dir = SE;
			break;
			case NO:
				pbille->dir = SO;
			break;
		}
		}
		
	//Mutex pour verif obs
	pthread_mutex_lock(&mutextab);		
	//Objet au dessus
	if(pbille->dir == NE or pbille->dir == NO)
	{
		if(tab[pbille->L-1][pbille->C] !=0)
		{
			if(pbille->dir == NE){pbille->dir = SE;}
			if(pbille->dir == NO){pbille->dir = SO;}
			impacte(tab[pbille->L-1][pbille->C]);
		}
	}
	
	//Objet en dessous
		if(pbille->dir == SE or pbille->dir == SO)
	{
		if(tab[pbille->L+1][pbille->C] !=0)
		{
			if(pbille->dir == SE){pbille->dir = NE;}
			if(pbille->dir == SO){pbille->dir = NO;}
			impacte(tab[pbille->L+1][pbille->C]);
		}
	}

	//Objet a droite
	if(pbille->dir == SE or pbille->dir == NE)
	{
		if(tab[pbille->L][pbille->C+1] !=0)
		{
			if(pbille->dir == SE){pbille->dir = SO;}
			if(pbille->dir == NE){pbille->dir = NO;}
			impacte(tab[pbille->L][pbille->C+1]);
		}
	}
	
	//Objet a gauche
	if(pbille->dir == NO or pbille->dir == SO)
	{
		if(tab[pbille->L][pbille->C-1] !=0)
		{
			if(pbille->dir == NO){pbille->dir = NE;}
			if(pbille->dir == SO){pbille->dir = SE;}
			impacte(tab[pbille->L][pbille->C-1]);
		}
	}
	
	//Objet en bas a droite
	if(pbille->dir == SE)
	{
		if(tab[pbille->L+1][pbille->C+1] !=0 and tab[pbille->L+1][pbille->C+1] !=-1){pbille->dir = SO;
		impacte(tab[pbille->L+1][pbille->C+1]);
		}
		
	}
	//Objet en bas a gauche
	if(pbille->dir == SO)
	{
		if(tab[pbille->L+1][pbille->C-1] !=0 and tab[pbille->L+1][pbille->C-1] !=-1){pbille->dir = SE;
		impacte(tab[pbille->L+1][pbille->C-1]);
		}
	}
	//Objet au dessus a droite
	if(pbille->dir == NE)
	{
		if(tab[pbille->L-1][pbille->C+1] !=0 and tab[pbille->L-1][pbille->C+1] !=-1){pbille->dir = NO;
		impacte(tab[pbille->L-1][pbille->C+1]);
		}
	}
	//objet au dessus a gauche
	if(pbille->dir == NO)
	{
		if(tab[pbille->L-1][pbille->C-1] !=0 and tab[pbille->L-1][pbille->C-1] !=-1){pbille->dir = NE;
		impacte(tab[pbille->L-1][pbille->C-1]);
		}
	}
	pthread_mutex_unlock(&mutextab);
	
	
		
		avancebille(pbille);
		DessineBille2(pbille->L,pbille->C,pbille->couleur);
	}
	
pthread_exit(0);
return NULL;
}

void avancebille(S_BILLE * pbille)
{
switch (pbille->dir){
	case NE:
		pbille->C = pbille->C +1;
		pbille->L = pbille->L -1;
		break;
	case NO:
		pbille->C = pbille->C -1;
		pbille->L = pbille->L -1;
		break;
	case SE:
		pbille->C = pbille->C +1;
		pbille->L = pbille->L +1;
	break;
	case SO:
		pbille->C = pbille->C -1;
		pbille->L = pbille->L +1;
	break;
	}
	
	
	return;
}

void * briqueThread (S_BRIQUE * briquept)
{	

	S_BRIQUE * brique;
	pthread_once(&briquecontroler, initCleBrique);
	brique = (S_BRIQUE *)malloc(sizeof(S_BRIQUE));
	memcpy(brique, briquept, sizeof(S_BRIQUE));
	pthread_setspecific(cleBrique,(void *)brique);
	DessineBrique2(brique->L, brique->C, brique->couleur, brique->brise);
	
	//Masque signal
	sigset_t mask;
	
	sigemptyset(&mask);
	sigaddset(&mask, SIGUSR1);
	sigaddset(&mask, SIGUSR2);
	sigaddset(&mask, SIGHUP);
	sigprocmask(SIG_SETMASK, &mask,NULL);
	//Arme les sig
	struct sigaction SigAct;
	
	SigAct.sa_handler = HandleBriqueSig;
	SigAct.sa_flags = SA_SIGINFO;
	sigaction(SIGTRAP,&SigAct, NULL);
	sigaction(SIGURG,&SigAct, NULL);
	
	
	
	while(1)
	{
	pause();
	}
}


void * raquetteThread (void *)
{
	sigset_t mask;
	struct sigaction SigAct;
	S_RAQUETTE * raquette;
	pthread_key_create(&macle, destructeurraq);
	pthread_mutex_lock(&mutexRaquettepid);
	raquettepid = pthread_self();
	pthread_mutex_unlock(&mutexRaquettepid);
	 raquette = (S_RAQUETTE *)malloc(sizeof(S_RAQUETTE));
	 raquette->L = 19;
	 raquette->C = 10;
	 raquette->longeur = 5;
	 raquette->billeSurRaquette = true;
	 pthread_setspecific(macle,(void *)raquette);
	 
	 
	 	//Masque signal
	sigemptyset(&mask);
	sigaddset(&mask, SIGTRAP);
	sigprocmask(SIG_SETMASK, &mask,NULL);
	//Arme les sig
	SigAct.sa_handler = HandleRaquetteSig;
	SigAct.sa_flags = SA_SIGINFO;
	sigaction(SIGUSR1,&SigAct, NULL);
	sigaction(SIGUSR2,&SigAct, NULL);
	sigaction(SIGHUP,&SigAct, NULL);
	sigaction(SIGINT,&SigAct, NULL);
	sigaction(SIGSYS,&SigAct, NULL);
	sigaction(SIGPIPE,&SigAct, NULL);
	sigaction(SIGALRM,&SigAct, NULL);
	
	DessineRaquette2(raquette->L,raquette->C,raquette->longeur);
	DessineBille2(raquette->L-1,raquette->C,ROUGE);

	while(1)
	{
	 pause();
	}
}

void destructeurraq(void *p)
{
puts("Je me libere (Raquette)");

free(p);
}

void destructeurbille(void *p)
{
puts("Je me libere (Bille)");
pthread_mutex_lock(&mutexBilleBrique);
nbBilles = nbBilles -1;
pthread_mutex_unlock(&mutexBilleBrique);
pthread_cond_signal(&condBilleBrique);

free(p);
}


void destructeurbri(void *p)
{

puts("Je me libere (Brique)");

pthread_mutex_lock(&mutexBilleBrique);
nbBriques = nbBriques - 1;
pthread_mutex_unlock(&mutexBilleBrique);
pthread_cond_signal(&condBilleBrique);
pthread_cond_signal(&condBrique);

free(p);
}

void * eventThread (void *)
{
	EVENT_GRILLE_SDL event;
	while(1)
	{
		event = ReadEvent();
		if(event.type == CROIX){pthread_exit(0);}
		if (event.type != CLAVIER){continue;}
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


void HandleBilleSig(int sig)
{
	
	puts("bille handler");
	S_BILLE * bille;
 	bille = (S_BILLE *)pthread_getspecific(cleBille);
 	if(bille == NULL)
 	{
 	puts("error");
 	}
 	else
 	{
 	 	char buffer[50];
 		sprintf(buffer, "Ma couleur: %d",bille->L);
 		puts(buffer);
 	}

}


void HandleRaquetteSig(int sig)
{
	char buffer[80];
	int positiondebut;
	int positionfin;
	S_RAQUETTE * raquettept;
	pthread_t HandleBille;
	//Recup de la variable specifique
	raquettept =  (S_RAQUETTE *)pthread_getspecific(macle);
	positiondebut = raquettept->C - ((raquettept->longeur-1)/2);
	positionfin = raquettept->C + ((raquettept->longeur-1)/2);
	if(sig==SIGUSR1 or sig==SIGUSR2){
	if(sig == SIGUSR1){
		 if(positiondebut > 0)
		 {
		 	
		 	effacer(19,raquettept->C,raquettept->longeur);
		 	raquettept->C = raquettept->C - 1;
		 	if(raquettept->billeSurRaquette == true)
		 	{	
		 		effacer(raquettept->L-1,raquettept->C+1,1);
		 		DessineBille2(raquettept->L-1,raquettept->C,ROUGE);

		 	}
		 	
		 }
	}
	else
	{
		if(sig == SIGUSR2){
			if(positionfin < 19)
			{
				effacer(19,positiondebut,raquettept->longeur);
				raquettept->C = raquettept->C + 1;
				if(raquettept->billeSurRaquette == true)
		 		{	
		 			effacer(raquettept->L-1,raquettept->C-1,1);
		 			DessineBille2(raquettept->L-1,raquettept->C,ROUGE);
		 		}
				
			}
		}
	}
	positiondebut = raquettept->C - ((raquettept->longeur-1)/2);
	DessineRaquette2(raquettept->L,raquettept->C,raquettept->longeur);
	}
	
	
	if(sig==SIGHUP and raquettept->billeSurRaquette == true)
	{
		raquettept->billeSurRaquette = false;
		S_BILLE * pbille;
		pbille = (S_BILLE *)malloc(sizeof(S_BILLE));
		if(pbille==NULL){puts("Erreur mallocc");}
		pbille->L = raquettept->L-1;
		pbille->C = raquettept->C;
		pbille->couleur = ROUGE;
		int randomdir = random(2);
		if(randomdir == 1){pbille->dir = NO;}
		else{pbille->dir = NE;}
		pthread_create(&HandleBille, NULL, (void *(*) (void *))billeThread, pbille);
		
	}
	
	if(sig==SIGINT)
	{
	 effacer(19,positiondebut,raquettept->longeur);
	 raquettept->billeSurRaquette = true;
	 DessineBille2(raquettept->L-1,raquettept->C,ROUGE);
	 DessineRaquette2(raquettept->L,raquettept->C,raquettept->longeur);
	}
	
	if(sig==SIGSYS)
	{
		puts("recu bonus jaune");//S'allonge
		alarm(0);
		raquettept->longeur  = 7;
		alarm(15);
	}
	
	if(sig==SIGPIPE)
	{
		puts("recu bonus vert");//Retrecit
		alarm(0);
		raquettept->longeur  = 3;
		alarm(15);
	}
	
	if(sig==SIGALRM)
	{
		puts("recu alarm");//Retour normal
		raquettept->longeur  = 5;
	}
	
}

void DessineRaquette2(int l, int c, int longeur)
{
	int i;
	int positiondebut = c- ((longeur-1)/2);
	pthread_mutex_lock(&mutextab);
	for(i=0;i<longeur;i++)
	{
		tab[l][positiondebut+i] = pthread_self();
	}
	DessineRaquette(l,c,longeur);
	pthread_mutex_unlock(&mutextab);
}

void DessineBille2(int l, int c, int couleur)
{
	pthread_mutex_lock(&mutextab);
	tab[l][c] = -2;
	DessineBille(l,c,couleur);
	pthread_mutex_unlock(&mutextab);
}



void DessineBrique2(int l, int c, int couleur, int brise)
{
	pthread_mutex_lock(&mutextab);
	tab[l][c] = pthread_self();
	tab[l][c+1] = pthread_self();
	DessineBrique(l,c,couleur,brise);
	pthread_mutex_unlock(&mutextab);
}

void effacer(int l,int c, int longeur)
{
	int i;
	pthread_mutex_lock(&mutextab);
	for(i=0;i<longeur;i++)
	{
		EffaceCarre(l,c);
		tab[l][c] = 0;
		c++;
	}
	pthread_mutex_unlock(&mutextab);
}

void HandleBriqueSig(int sig)
{

	S_BRIQUE * brique;
 	brique = (S_BRIQUE *)pthread_getspecific(cleBrique);
 	

	if(sig == SIGURG)
	{
		DessineBrique(brique->L, brique->C, brique->couleur ,brique->brise);
	
	}
	else
	{
			//Toucher 
		if(brique->nbTouches < 2)
			{
				effacer(brique->L,brique->C,2);
				//Thread bonnus
				pthread_t HandleBonus;
				S_BONUS bonus;
				bonus.L = brique->L;
				bonus.C = brique->C;
				bonus.couleur = brique->bonus;
				pthread_create(&HandleBonus, NULL, (void *(*) (void *))bonusThread, &bonus);
				pthread_exit(0);
			
			
			
			}
		if(brique->nbTouches >= 2)
		{
			brique->nbTouches = brique->nbTouches-1;
			brique->brise = 1;
			DessineBrique(brique->L, brique->C, brique->couleur ,brique->brise);
		}
	}


}

void impacte(int qui)
{
	pthread_mutex_lock(&mutexRaquettepid);
	if(qui != raquettepid and qui > 0)
	{
		pthread_kill(qui,SIGTRAP);
		pthread_mutex_lock(&mutexScore);
		score = score +1;
		MAJScore = true;
		pthread_mutex_unlock(&mutexScore);
		pthread_cond_signal(&condScore);
	}
	pthread_mutex_unlock(&mutexRaquettepid);
}

void initCleBrique()
{
	pthread_key_create(&cleBrique, destructeurbri);
}

void initCleBille()
{
	pthread_key_create(&cleBille, destructeurbille);
}

void * niveauThread()
{
	int nbVies =2;
	int niveau = 1;
	int niveauterm=1;
	DessineVie(nbVies);
	DessineNiveau(niveau);
	pthread_t HandleBrique[NB_BRIQUES];
	int i;
	

	
	while(1)
	{
		DessineNiveau(niveau);
		nbBilles = 1;
		if(niveauterm==1)
		{
			niveauterm=0;
			nbBriques = NB_BRIQUES;
			pthread_mutex_lock(&mutexNiveauFinit);
			niveauFini=false;
			pthread_mutex_unlock(&mutexNiveauFinit);
			for(i=0;i<NB_BRIQUES;i++)
			{
				int randombonus = random(3);
				if(randombonus==3)
				{
					Briques[i].bonus =  15;
				}else
				{
					if(randombonus == 2)
					{
						Briques[i].bonus =  10;
					}
					else
					{
						Briques[i].bonus =  5;
					}
				}
				
				pthread_create(&HandleBrique[i], NULL, (void *(*) (void *))briqueThread, &Briques[i]);
			}
		}
		pthread_mutex_lock(&mutexBilleBrique);
		while(nbBilles > 0 && nbBriques > 0)
		pthread_cond_wait(&condBilleBrique, &mutexBilleBrique);
		pthread_mutex_unlock(&mutexBilleBrique);
		
		//Verif
		pthread_mutex_lock(&mutexBilleBrique);
		if(nbBilles == 0)
		{
			if(nbVies ==0)
			{
			DessineGameOver(9,6);
			pthread_exit(0);
			}
			else
			{
			 kill(getpid(), SIGINT);
			 nbVies = nbVies -1;
			 DessineVie(nbVies);
			}
			pthread_mutex_unlock(&mutexBilleBrique);
		}
		else
		{
		pthread_mutex_unlock(&mutexBilleBrique);
		pthread_mutex_lock(&mutexNiveauFinit);
		niveauFini=true;
		pthread_mutex_unlock(&mutexNiveauFinit);
		niveauterm = 1;
		pthread_mutex_lock(&mutexBilleBrique);
		while(nbBilles > 0)
		{
		pthread_cond_wait(&condBilleBrique, &mutexBilleBrique);
		}
		pthread_mutex_unlock(&mutexBilleBrique);
		niveau = niveau + 1;
		Attente(1000);
		temps = temps - (temps/10); 
		char buffer[50];
		sprintf(buffer, "Temp: %d", temps);
		puts(buffer);
		kill(getpid(), SIGINT);
		
		}
	}
	
}

void DessineNiveau(int niveau)
{
	int dizaine, unite;
	dizaine = niveau /10;
	unite = niveau - 10*dizaine;
	EffaceCarre(0,3);
	EffaceCarre(0,4);
	if(dizaine == 0)
	{
		DessineChiffre(0,3,unite);
	}
	else
	{
		DessineChiffre(0,3,dizaine);
		DessineChiffre(0,4,unite);	
	}
	
	
}

void DessineVie(int vie)
{	

	int i;
	
	for(i=0;i<5;i++){EffaceCarre(0,(9+i));}
	for(i=0;i<vie;i++)
	{
		DessineDiamant(0,(9+i),ROUGE);
	}
	
}



void * scoreThread ()
{
int unite, dizaine,centaine, mille;
	while(1)
	{
		pthread_mutex_lock(&mutexScore);
		while(MAJScore == false)
		{
			pthread_cond_wait(&condScore, &mutexScore);
		}
	
		MAJScore = false;
		mille = score/1000;
		centaine = (score - (mille*1000)) / 100;
		dizaine = (score - (mille*1000) - (centaine*100)) / 10;
		unite = (score - (mille*1000) - (centaine*100) - (dizaine*10)); 
	
		DessineChiffre(0,16, mille);
		DessineChiffre(0,17, centaine);
		DessineChiffre(0,18, dizaine);
		DessineChiffre(0,19, unite);
		char buffer[50];
		sprintf(buffer, "Score:%d", score);
		puts(buffer);
		pthread_mutex_unlock(&mutexScore);
	}
	
}



void * bonusThread(S_BONUS * bonuspt)
{
	S_BONUS * bonus;
	bonus = (S_BONUS *)malloc(sizeof(S_BONUS));
	memcpy(bonus, bonuspt, sizeof(S_BONUS));
	int letemps=0;
	bool fin=false;
	letemps = temps;

	
	
	//Déplacement
	while(1)
	{
		Attente(letemps);
		pthread_mutex_lock(&mutexNiveauFinit);
		fin = niveauFini;
		pthread_mutex_unlock(&mutexNiveauFinit);
		if(fin == true or (NB_LIGNES-1) == bonus->L)
		{
			effacerBonus(bonus->L,bonus->C);
			pthread_exit(0);
		}
		pthread_mutex_lock(&mutextab);
		//
		if(tab[bonus->L+1][bonus->C]  >= 0)
		{
			effacerBonus(bonus->L,bonus->C);
			bonus->L = bonus->L +1;
			
		}
		DessineBonus2(bonus->L, bonus->C, bonus->couleur);
		pthread_mutex_unlock(&mutextab);
		
		
	}


}

void effacerBonus(int l, int c)
{
		if(tab[l][c] != -3 and tab[l][c] != 0)
		{

		//Demande a la brique de se redessiner
		pthread_kill(tab[l][c],SIGURG);
		
		}
		else
		{
		EffaceCarre(l,c);
		tab[l][c] = 0;
		}
}

void DessineBonus2(int l, int c, int couleur)
{
	int pidraquette;
	if(tab[l][c]>0)
	{
		//Raquette ou brique
		
		pthread_mutex_lock(&mutexRaquettepid);
		pidraquette = raquettepid ;
		pthread_mutex_unlock(&mutexRaquettepid);
		if(pidraquette == tab[l][c])
		{
			if(couleur == 15)//Jaune
			{

			pthread_kill(pidraquette, SIGSYS);
			}
			if(couleur == 5)//Vert
			{

			pthread_kill(pidraquette, SIGPIPE);
			}
			
			if(couleur == 10)//Mauve
			{
			
			kill(getpid(), SIGEMT);
			}
			pthread_mutex_unlock(&mutextab);
			pthread_exit(0);
		}	
	}
	else
	{
	
		tab[l][c] = -3;
	}
	
	int newcol=0;
	if(couleur == 15){newcol = JAUNE;}
	if(couleur == 5){newcol = VERT;}
	if(couleur == 10){newcol = MAUVE;}
	DessineDiamant(l,c,newcol);

}

