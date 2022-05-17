#include "TaquinSDL.h"
#include "AStar.h"
#include <time.h>
#include "SDL_gfxprimitives.h"


// Fonction permettant de créer le taquin en SDL à partir de sa taille et du fichier BMP à utiliser
// Cette fonction crée aussi la fenêtre SDL et la stocke dans la structure TaquinSDL
int createTaquinSDL(TaquinSDL * pTaquinSDL,int hauteur, int largeur, char * pathBMPfile)
{
	// Test pour vérifier que les données passées ne sont pas corrompues
	if(!pTaquinSDL) return 0;

	// initialisation de la fenêtre SDL
	pTaquinSDL->pWindow = NULL;
	// initialisation de l'image à NULL
	pTaquinSDL->pFond = NULL;
	// On crée le taquin qui sera utilisé pour jouer
	pTaquinSDL->taquin.plateau = NULL;
	createTaquin(&(pTaquinSDL->taquin),hauteur,largeur);

	// On met à jour la taille du taquin
	largeur = pTaquinSDL->taquin.largeur;
	hauteur = pTaquinSDL->taquin.hauteur;

	// On initialise la SDL
	// On initialise non seulement le mode vidéo mais aussi la gestion du temps pour pouvoir utiliser SDL_GetTicks()
	if(SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER ))
	{
		freeTaquinSDL(pTaquinSDL);
		return 0;
	}

	// On charge le BMP qui servira de fond au taquin
	pTaquinSDL->pFond= SDL_LoadBMP(pathBMPfile);
	if(!pTaquinSDL->pFond) 
	{
		freeTaquinSDL(pTaquinSDL);
		return 0;
	}

	// Initialisation de la fenêtre

	// On récupère la taille d'une case
	pTaquinSDL->resX = pTaquinSDL->pFond->w/largeur;
	pTaquinSDL->resY = pTaquinSDL->pFond->h/hauteur;

	// On crée la fenêtre en fonction de la résolution de l'image d'entrée
	pTaquinSDL->pWindow = SDL_SetVideoMode(pTaquinSDL->resX*largeur, pTaquinSDL->resY*hauteur, 32, SDL_DOUBLEBUF|SDL_HWSURFACE);
	if(!pTaquinSDL->pWindow)
	{
		freeTaquinSDL(pTaquinSDL);
		return 0;
	}


	SDL_WM_SetCaption("TAQUIN","TAQUIN" );

	SDL_WM_SetIcon(NULL,NULL);

	return 1;
	
}


int displayCaseTaquin(TaquinSDL * pTaquinSDL,Uint8 caseTaquin, SDL_Rect * pDest, int x, int y, int refresh)
{
	// TODO: displayCaseTaquin
	// on définit où on veut la dessiner


	// Si la case n'est pas vide ...
	// on calcule où est la case "caseTaquin" dans l'image initiale pour -par la suite - venir découper la zone qui correspond à la case
	
	// ... 

	// On dessine la case dans la fenêtre (en découpant dans l'image initiale avec la zone définie ci-dessus)

	if(refresh) SDL_UpdateRect(pTaquinSDL->pWindow,pDest->x,pDest->y,pDest->w,pDest->h);

	return 1;
}

// fonction pour rendre le taquin dans son état actuel
int displayTaquinSDL(TaquinSDL * pTaquinSDL)
{
	// Test pour vérifier que les données passées ne sont pas corrompues
	if(!pTaquinSDL || !pTaquinSDL->taquin.plateau || !pTaquinSDL->pWindow ) return 0;

	// On dessine les cases une par une en allant découper l'image de fond avec un SDL_Rect
	{


		// On dessine le taquin terminé pour afficher quelque chose mais il faudra le changer
		SDL_BlitSurface(pTaquinSDL->pFond,NULL,pTaquinSDL->pWindow,NULL);

		// TODO: displayTaquinSDL
		// ...



		// On met à jour la fenêtre complète
		SDL_UpdateRect(pTaquinSDL->pWindow,0,0,0,0);

	}


	return 1;
}

// fonction permettant de faire le rendu SDL du taquin et de jouer (gestion des évènements à l'intérieur de la fonction)
int gameLoopSDL(int hauteur,int largeur, char * pathBMPfile, int minRandom, int maxRandom)
{
	int end = 0;
	TaquinSDL t;

	// On crée le taquin et la fenêtre pour le dessiner
	if(!createTaquinSDL(&t,hauteur,largeur,pathBMPfile)) return 0;


	// On boucle sur le jeu tant qu'on a pas demandé de quitter
	while(end>=0)
	{
		int tick = SDL_GetTicks();

		// On mélange le taquin
		mixTaquin(&(t.taquin), minRandom, maxRandom);

		// On affiche le taquin mélangé
		displayTaquinSDL(&t);

		end = 0;

		// On boucle tant que la solution n'a pas été trouvée
		while(!end )
		{
			// Gestion des évènements
			SDL_Event e;

			int dt = SDL_GetTicks()-tick;
			tick +=dt;

			while(!end && SDL_PollEvent(&e))
			{
				switch(e.type)
				{
					case SDL_KEYDOWN:
						{
							deplacement d = AUCUN;
							
							switch(e.key.keysym.sym)
							{
								// On définit le mouvement à effectuer
								case SDLK_q:		;
								case SDLK_LEFT:		d = GAUCHE;
									break;
								case SDLK_d:
								case SDLK_RIGHT:	d = DROITE;
									break;
								case SDLK_z:
								case SDLK_UP:		d = HAUT;
									break;
								case SDLK_s:
								case SDLK_DOWN:		d = BAS;
									break;
								case SDLK_ESCAPE:	
									// On quitte l'application
									end = -1;
									break;
								case SDLK_F1:
									// On déclenche la résolution du taquin
									// résolution SDL à faire par la suite pour laisser la main à l'utilisateur :
									// - Arrêter la résolution (appui sur n'importe qu'elle touche
									// - Quitter l'application (ECHAP ou CROIX en haut à droite)
									{
										
										//on initialise les variables permettant de récupérer les informations issues de la résolution
										deplacement * tabDeplacements = NULL;
										unsigned long nbDeplacements = 0;
										unsigned long nbSommetsParcourus = 0;
										unsigned long timeElapsed = 0;

										// On demande la résolution du taquin à l'ordinateur
										if(solveTaquin(&(t.taquin),&tabDeplacements,&nbDeplacements, &nbSommetsParcourus, &timeElapsed, 0, t.pWindow))
										{
											// Si on a trouvé une solution on affiche les informations issues de la résolution
											unsigned long i;
											int res = 0;
											printf("Nombre de deplacements = %d\n",nbDeplacements);
											printf("Nombre de sommets parcourus = %d\n",nbSommetsParcourus);
											printf("Temps ecoule = %d ms\n",timeElapsed);

											// On affiche la solution étape par étape
											for(i=0; i < nbDeplacements; i++)
											{
												// On effectue le déplacement, on affiche le nouveau plateau et on attend un appui sur une touche pour continuer
												if(tabDeplacements[i]!=AUCUN)
												{
													if(moveTaquin(&(t.taquin),tabDeplacements[i])) displayTaquinSDL(&t);
													else break;
												}
											}
										}
										// Si la résolution n'a pas fonctionné, on affiche le taquin tel qu'il était avant résolution (on efface l'icone de "progression" si elle avait été dessinée)
										else displayTaquinSDL(&t);

										if(tabDeplacements) 
										{
											free(tabDeplacements);
											tabDeplacements = NULL;
										}
									}

									break;
							}
							
							if(d!=AUCUN && moveTaquin(&(t.taquin),d)) displayTaquinSDL(&t);

						}
						break;

					case SDL_MOUSEBUTTONDOWN:
						{
							// On récupère la position et l'état des boutons de la souris
							int x,y;
							Uint8 state;
							state = SDL_GetMouseState(&x,&y);

							if(state & SDL_BUTTON_LEFT)
							{

								deplacement d = AUCUN;

								// On récupère la position de la case dans le taquin
								int posX = x/t.resX;
								int posY = y/t.resY;

								// TODO: On définit le déplacement à effectuer (différence en X de +/- 1 ou en Y de +/-1 => DEPLACEMENT sinon AUCUN)
								
								// ...
									
								// On applique le déplacement
								if(moveTaquin(&(t.taquin),d)) displayTaquinSDL(&t);
							}
						}
						break;

					case SDL_QUIT:
						// On quitte
						end = -1;
						break;
				}
			}

			// On relache du temps processeur pour les autres processus
			SDL_Delay(1);

			if(!end) end = endTaquin(&(t.taquin));

		}

		// Si on n'a pas demandé à quitter c'est qu'on a terminé le taquin
		if(end>0)
		{
			// Affichage de l'image complète par dessus le tout

			SDL_Delay(1000);
		}

		// On réinitialise le taquin pour le prochain tour de jeu
		initTaquin(&(t.taquin));
	}

	// On libère le taquin et les surfaces SDL
	freeTaquinSDL(&t);

	/* Shut them both down */
	SDL_Quit();


	return 1;
}

// fonction pour libérer le taquins et les surfaces SDL utilisées
int freeTaquinSDL(TaquinSDL * pTaquinSDL)
{
	// test pour savoir si les données passées ne sont pas corrompues
	if(!pTaquinSDL) return 0;

	// On libère la fenêtre SDL
	if(pTaquinSDL->pWindow) SDL_FreeSurface(pTaquinSDL->pWindow);

	// On libère l'image de fond
	if(pTaquinSDL->pFond) SDL_FreeSurface(pTaquinSDL->pFond);

	// On libère le taquin (mode console)
	freeTaquin(&(pTaquinSDL->taquin));

	return 1;
}