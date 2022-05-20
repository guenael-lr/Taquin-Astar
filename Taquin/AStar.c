#include "AStar.h"
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include "SDL.h"
#include "SDL_image.h"
//fonction qui va créer le taquin de réussite en static 
//On utilise le static afin de ne pas à avoir le recalculer à chaque fois que l'on appelle la fonction
Taquin* InitialTaquin(Taquin* _pTaquin)
{
	static Taquin* taq = NULL; 
	if (taq)
		return taq;
	
	//Ainsi, les lignes suivantes ne se feront qu'une fois
	taq = calloc(1, sizeof(Taquin));
	if (!taq)
		return NULL;

	createTaquin(taq, _pTaquin->hauteur, _pTaquin->largeur);
	initTaquin(taq);

	return taq; 
}

// fonction pour créer (allouer) un noeud de liste et l'initialiser avec le taquin passé en paramètre
ptrListAStar createNodeList(Taquin* pTaquin, int gValue, deplacement d, ptrListAStar pPrevPlay)
{
	ptrListAStar node = calloc(1, sizeof(ListAStar));
	if (!node)
		return NULL;

	copyTaquin(pTaquin, &(node->pTaquin));

	//on effectue le mouvement dans la creation du noeud
	//si le mouvement est impossible, on retourne un noeud NULL
	//sinon on le remplie
	if (moveTaquin(&(node->pTaquin), d)) { 
		freeTaquin(&(node->pTaquin));
		free(node);
		return NULL;
	}

	node->g = gValue;
	node->f = gValue + h(&(node->pTaquin)); //pour plus de rapidité on appelle de h ici, pas besoin de prendre le f en entrée
	node->prev_d = d;
	node->prev_node = pPrevPlay;

	return node;
}

// Insertion dans la liste de façon triée ou non
// si on passe le paramètre tri à 0, on insère en tête de liste
int insertList(ptrListAStar* ppHead, ptrListAStar pNode, int tri)
{
	if (!pNode || !ppHead)
		return 1;
	if (!tri || !(*ppHead)) {
		pNode->post_node = (*ppHead);
		(*ppHead) = pNode;
		return 0;
	}

	ptrListAStar cursor = (*ppHead);

	while (cursor->post_node != NULL && pNode->f > cursor->post_node->f)
		cursor = cursor->post_node;

	pNode->post_node = cursor->post_node;
	cursor->post_node = pNode;

	return 0;
}

// Fonction pour prélever le noeud en tête de liste
// Retourne le noeud prélevé
ptrListAStar popList(ptrListAStar* ppHead)
{
	if (!ppHead || !(*ppHead))
		return NULL;
	ptrListAStar node = (*ppHead);
	(*ppHead) = node->post_node;

	return node;
}

// fonction qui retourne le noeud dans lequel on trouve le taquin passé en paramètre (pointeur sur le pointeur dans la liste)
ptrListAStar isInList(ptrListAStar* ppHead, Taquin* pTaquin)
{
	if (!ppHead || !(*ppHead))
		return NULL;

	if (equalIdTaquin(pTaquin, &((*ppHead)->pTaquin))) 
		return *ppHead;

	ptrListAStar cursor = (*ppHead);

	while (cursor->post_node) {

		if (equalIdTaquin(pTaquin, &(cursor->pTaquin)))
			return cursor;
		cursor = cursor->post_node;
	}
	
	return NULL;
}

// fonction pour afficher une liste
// si on met displayFGH à 0 les valeur de F, G et H ne sont pas affichées
int displayList(ptrListAStar pHead, int displayFGH)
{
	int i = 0;
	while (pHead != NULL)
	{
		if (displayFGH)
		{
			printf("F=%d, G=%d, H=\n", pHead->f, pHead->g);
		}
		printf("count = %d, Prev deplace: %d\n", i++, pHead->prev_d);
		displayTaquin(&(pHead->pTaquin), 0);
		pHead = pHead->post_node;

	}

	return 0;
}

// Fonction pour résoudre le taquin en utilisant l'algorithme A*
// La fonction prend comme taquin initial le taquin spécifié par pTaquin
// elle remplit 
//   - pTabDeplacement (un tableau contenant les déplacements à effectuer pour résoudre le taquin)
//   - pNbDeplacements
//   - pNbTaquinsGeneres
//   - pTimeElapsed
// Si stepByStep est différent de 0 on affiche dans la console toutes les étapes de la résolution
// pWindow
int solveTaquin(Taquin* pTaquin, deplacement** pTabDeplacement, unsigned long* pNbDeplacements, unsigned long* pNbTaquinsGeneres, unsigned long* pTimeElapsed, int stepByStep, SDL_Surface* pWindow)
{
	ptrListAStar closed= NULL, parent = NULL;
	ptrListAStar open = createNodeList(pTaquin, 0, AUCUN, NULL);
	ptrListAStar cursor = NULL;
	ptrListAStar cursorchild = NULL;
	ptrListAStar compare = NULL;
	int end = 0;
	int nbcount = 0;
	int parcouredNoded = 0;
	unsigned long startTime = SDL_GetTicks();
	int accu = 5000;
	while (!end) 
	{
		//if (parcouredNoded > accu)
		//{
		//	accu += 5000;
		//	printf("g =~ %d || nodes = %d\n",closed->g, parcouredNoded);
		//
		//}
		
		//l'etape suivante corresponds au popList
		//on gagne du temps en le faisant à la main plutot que d'appeller la fonction
		cursor = open;
		open = open->post_node; 
		cursor->post_node = NULL;

		insertList(&closed, cursor, 0);

		for (int i = 0; i < 4; i++)
		{
			//faire le noeuf des le debut permet d'avoir un endroit ou tout stocker 
			//de plus celui ci return null si il ne peut pas se deplacer
			//du moins si on lui demande de faire un mouvement AUCUN (0) il ne va pas return NULL 
			//car il y a le premier mouvement qui n'a pas de mouvement
			cursorchild = createNodeList(&(cursor->pTaquin), cursor->g + 1, i + 1 , cursor); 
			if (cursorchild)
			{
				//Verification si le taquin est equivalent à celui de fin 
				if (equalTaquin(&(cursorchild->pTaquin), InitialTaquin(&(cursor->pTaquin))))
				{
					*pTimeElapsed = SDL_GetTicks() - startTime;
					nbcount = cursorchild->g;
					*pNbDeplacements = nbcount;
					* pTabDeplacement = calloc(nbcount+1, sizeof(deplacement));

					//si il a un probleme dans la creation de la liste de deplacement
					if (!*pTabDeplacement)
					{
						freeList(&cursorchild);
						freeList(&open);
						freeList(&closed);
						return 0;
					}
					parent = cursorchild;
					//on remplie la liste à l'envers vu que l'on remonte la liste des parents
					while (parent->prev_node) {
						(*pTabDeplacement)[nbcount] = parent->prev_d;
						parent = parent->prev_node;
						--nbcount;
					}
					
					*pNbTaquinsGeneres = parcouredNoded;
					//printf("g = %d, NodesPar=%d\nChemin : \n", cursorchild->g, parcouredNoded);
					freeList(&cursorchild);
					end = 1;
					break;
				}

				//on compare le mouvement actuel et son grand parent, si ils sont identiques on abandonne l'enfant
				//on gagne un peu de temps à ne pas chercher ce cas dans les listes, qui sont parfois longues

				if (cursor->prev_node && equalIdTaquin(&(cursorchild->pTaquin), &(cursor->prev_node->pTaquin))) {
					freeList(&cursorchild);
					continue;
				}

				//on regarde si le noeud exite deja dans les liste closed et opened 
				//si, dans l'un des deux cas, on en trouve un similaire
				//alors si le trouvé est moins bien, on remplace ses valeurs par celles de l'enfant
				compare = isInList(&closed, &(cursorchild->pTaquin));
				if (compare)
				{
					if (compare->f > cursorchild->f) 
					{
						compare->f = cursorchild->f;
						compare->g = cursorchild->g;
						compare->prev_node = cursorchild->prev_node;
					}
					freeList(&cursorchild);
					continue; //on abandonne l'enfant
				}
				
				compare = isInList(&open, &(cursorchild->pTaquin));
				if (compare)
				{
					if (compare->f > cursorchild->f)
					{
						compare->f = cursorchild->f;
						compare->g = cursorchild->g;
						compare->prev_node = cursorchild->prev_node;
					}
					freeList(&cursorchild);
					continue; //on abandonne l'enfant
				}

				insertList(&open, cursorchild, 1);
				parcouredNoded++;
			}
		}
	}
	freeList(&open);
	freeList(&closed);
	
	return 1;
}

// fonction d'évaluation pour la résolution avec AStar
int h(Taquin* pTaquin)
{
	//on sous-hestime legerement l'heuristique en commencant le total à -1
	int total = -1, index = 0; 
	for (int x = 0; x < pTaquin->hauteur; ++x)
		for (int y = 0; y < pTaquin->largeur; ++y) {
			if (index != pTaquin->plateau[x][y]) {
				//distance de Manhattan + 1 car le noeud est mal rangé
				total += 1 + abs(x - pTaquin->plateau[x][y] % pTaquin->largeur) + abs(y - pTaquin->plateau[x][y] / pTaquin->largeur);
				
				//heuristique choisies expliquées dans le README.md

				if (x)
					total += (pTaquin->plateau[x - 1][y] == index && pTaquin->plateau[x][y] == (x - 1 + y * pTaquin->hauteur));
				if (x < pTaquin->largeur - 1)
					total += (pTaquin->plateau[x + 1][y] == index && pTaquin->plateau[x][y] == (x + 1 + y * pTaquin->hauteur));
				if (y)
					total += (pTaquin->plateau[x][y - 1] == index && pTaquin->plateau[x][y] == (x + (y - 1) * pTaquin->hauteur));
				if (y < pTaquin->hauteur - 1)
					total += (pTaquin->plateau[x][y + 1] == index && pTaquin->plateau[x][y] == (x + (y + 1) * pTaquin->hauteur));
			}
			else
				total -= index;
			++index;
		}



	//Les prochains calculs permettent de baisser l'heuristique si la(es) ligne(s) du bas sont complétées

	if (pTaquin->hauteur== 2) //inutile sur une 2x2
		return total;

	//on regarde si la derniere ligne est complete ou non 
	//identique pour toutes les tailles au dessus de 2x2
	int hauteur_1 = pTaquin->hauteur - 1;
	for (int x = 0; x < pTaquin->largeur; x++)
		if (pTaquin->plateau[hauteur_1][x] != (hauteur_1 * (hauteur_1 + 1) + x)) 
			return total;
	
	total -= 10; //le coefficient est choisi apres quelques tests
	if (hauteur_1 == 2) // si il s'agit d'une 3x3 on s'arrete ici pour ne regarder que la derniere ligne
		return total;
	--hauteur_1;
	
	// on regarde l'avant derniere ligne pour les 4x4 et 5x5 aussi afin de baisser l'heuristique
	for (int x = 0; x < pTaquin->largeur; x++)
		if (pTaquin->plateau[hauteur_1][x] != (hauteur_1 * (hauteur_1 + 1) + x))
			return total;
	total -= 50; //de meme ici, purement arbitraire

	return total;
}

void freeList(ptrListAStar* ppHead)
{
	//ptrListAStar* iterator = ppHead;
	ptrListAStar tmp = NULL;

	while (*ppHead)
	{
		freeTaquin(&((*ppHead)->pTaquin));
		tmp = (*ppHead)->post_node;
		free((*ppHead));
		(*ppHead) = tmp;
	}
	
	return;
}