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

typedef struct
{
	int L;
	int C;
	int dir;
	int couleur;
} S_BILLE;



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
//Bille
void destructeurbille(void *p);
void * raquetteThread (S_BILLE *);
void avancebille(S_BILLE * pbille);
void DessineBille2(int l, int c, int couleur);
void impacte(int qui);

//Brique
void DessineBrique2(int l, int c, int couleur, int brise);
void * briqueThread (S_BRIQUE *);
void HandleBriqueSig(int sig);
void destructeurbri(void *p);
pthread_once_t briquecontroler = PTHREAD_ONCE_INIT;
void initCleBrique();


//Niveau
void * niveauThread();
void DessineVie(int vie);
void DessineNiveau(int niveau);
int nbBriques;
int nbBilles;

//Event
void * eventThread (void *);

//Def
pthread_key_t macle;
pthread_key_t cleBille;
pthread_key_t cleBrique;
pthread_mutex_t mutextab;
//
int main(int argc,char* argv[])
{
	struct sigaction SigAct;
	sigset_t mask;
	pthread_t HanleRaquette;
	pthread_t HandleEvent;
	pthread_t HandleNiveau;
  
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
	sigaddset(&mask, SIGTRAP);
	sigprocmask(SIG_SETMASK, &mask,NULL);
	
	
	pthread_create(&HanleRaquette, NULL, (void *(*) (void *))raquetteThread, NULL);
	pthread_create(&HandleEvent, NULL, (void *(*) (void *))eventThread, NULL);
  // Exemple d'utilisation de GrilleSDL et Ressources --> code a supprimer
  //DessineRaquette(17,7,5);  // Attention !!! tab n'est pas modifie --> a vous de le faire !!!
//  DessineChiffre(2,1,9);
    //DessineBrique2(5,9,ROUGE,0);
//  DessineBrique(7,4,VERT,1);
//  DessineBille(10,5,JAUNE);
//  DessineDiamant(12,16,MAUVE);

  
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

void * billeThread(S_BILLE * pbille)
{

	pthread_key_create(&cleBille, destructeurbille);
	pthread_setspecific(cleBille,pbille);
	pthread_mutex_lock(&mutextab);
	DessineBille(pbille->L,pbille->C,pbille->couleur);
	tab[pbille->L][pbille->C] = -2;
	pthread_mutex_unlock(&mutextab);
	while(1)
	{
		Attente(200);
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
	sigset_t mask;
	struct sigaction SigAct;
	S_BRIQUE * brique;
	pthread_once(&briquecontroler, initCleBrique);
	brique = (S_BRIQUE *)malloc(sizeof(S_BRIQUE));
	memcpy(brique, briquept, sizeof(S_BRIQUE));
	pthread_setspecific(cleBrique,(void *)brique);
	DessineBrique2(brique->L, brique->C, brique->couleur, brique->brise);
	
	//Masque signal
	sigemptyset(&mask);
	sigaddset(&mask, SIGUSR1);
	sigaddset(&mask, SIGUSR2);
	sigaddset(&mask, SIGHUP);
	sigprocmask(SIG_SETMASK, &mask,NULL);
	//Arme les sig
	SigAct.sa_handler = HandleBriqueSig;
	SigAct.sa_flags = 0;
	sigaction(SIGTRAP,&SigAct, NULL);
	
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
	SigAct.sa_flags = 0;
	sigaction(SIGUSR1,&SigAct, NULL);
	sigaction(SIGUSR2,&SigAct, NULL);
	sigaction(SIGHUP,&SigAct, NULL);

	
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
free(p);
}


void destructeurbri(void *p)
{

puts("Je me libere (Brique)");
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
		pbille->dir = NO;	
		pthread_create(&HandleBille, NULL, (void *(*) (void *))billeThread, pbille);
		
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
 	
	//Toucher 
	char buffer[50];
	sprintf(buffer, "Je suis %d et me reste %d pos: %d;%d",pthread_self(),brique->nbTouches, brique->L, brique->C);
	puts(buffer);
	if(brique->nbTouches < 2)
		{
			effacer(brique->L,brique->C,2);
			pthread_exit(0);
			
			
			
		}
	if(brique->nbTouches >= 2)
	{
		brique->nbTouches = brique->nbTouches-1;
		brique->brise = 1;
		DessineBrique(brique->L, brique->C, brique->couleur ,brique->brise);
	}

}

void impacte(int qui)
{
	pthread_kill(qui,SIGTRAP);
}

void initCleBrique()
{
	pthread_key_create(&cleBrique, destructeurbri);
}


void * niveauThread()
{
	int nbVies =2;
	int niveau = 1;
	DessineVie(nbVies);
	DessineNiveau(niveau);
	pthread_t HandleBrique[NB_BRIQUES];
	int i;
	
	
	while(1)
	{
		nbBriques = NB_BRIQUES;
		nbBilles = 1;
		for(i=0;i<NB_BRIQUES;i++)
		{
			pthread_create(&HandleBrique[i], NULL, (void *(*) (void *))briqueThread, &Briques[i]);
		}
	pause();
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
