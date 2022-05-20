#include "AStar.h"
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include "SDL.h"
#include "SDL_image.h"

Taquin* InitialTaquin(Taquin* _pTaquin)
{
	static Taquin* taq = NULL;
	if (taq)
		return taq;


	taq = calloc(1, sizeof(Taquin));
	if (!taq)
		return NULL;

	createTaquin(taq, _pTaquin->hauteur, _pTaquin->largeur);
	initTaquin(taq);

	return taq;
}

// fonction pour créer (allouer) un noeud de liste et l'initialiser avec le taquin passé en paramètre
ptrListAStar createNodeList(Taquin* pTaquin, int gValue, int fValue, deplacement d, ptrListAStar pPrevPlay)
{
	ptrListAStar node = calloc(1, sizeof(ListAStar));
	if (!node)
		return NULL;

	copyTaquin(pTaquin, &(node->pTaquin));
	if (moveTaquin(&(node->pTaquin), d)) {
		freeTaquin(&(node->pTaquin));
		free(node);
		return NULL;
	}
	node->g = gValue;
	node->f = gValue + h(&(node->pTaquin));
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

	while (cursor && !equalIdTaquin(pTaquin, &(cursor->pTaquin)))
	{
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
	ptrListAStar open = createNodeList(pTaquin, 0, h(pTaquin), AUCUN, NULL);
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
		if (parcouredNoded > accu)
		{
			accu += 5000;
			printf("g =~ %d || nodes = %d\n",closed->g, parcouredNoded);

		}
		cursor = open;
		open = open->post_node;
		cursor->post_node = NULL;

		insertList(&closed, cursor, 0);

		for (int i = 0; i < 4; i++)
		{
			cursorchild = createNodeList(&(cursor->pTaquin), cursor->g + 1, 0, i + 1 , cursor);
			if (cursorchild)
			{
				if (equalIdTaquin(&(cursorchild->pTaquin), InitialTaquin(&(cursor->pTaquin))))
				{
					*pTimeElapsed = SDL_GetTicks() - startTime;
					nbcount = cursorchild->g ;
					*pNbDeplacements = nbcount +1;
					* pTabDeplacement = calloc(nbcount+1, sizeof(deplacement));
					if (!(* pTabDeplacement))
					{
						freeList(&cursorchild);
						freeList(&open);
						freeList(&closed);
						return 0;
					}
					parent = cursorchild;
					
					while (nbcount) {
						printf("Parent : %d", parent->g);
						displayTaquin(&(parent->pTaquin), 0);
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

				if (cursor->prev_node && equalIdTaquin(&(cursorchild->pTaquin), &(cursor->prev_node->pTaquin))) {
					freeList(&cursorchild);
					continue;
				}

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
	int tot = +1, index = 0;;
	for (int x = 0; x < pTaquin->hauteur; ++x)
		for (int y = 0; y < pTaquin->largeur; ++y) {
			if (index != pTaquin->plateau[x][y]) {
				tot += 1 + abs(x - pTaquin->plateau[x][y] % pTaquin->largeur) + abs(y - pTaquin->plateau[x][y] / pTaquin->largeur);
				if (x)
					tot += (pTaquin->plateau[x - 1][y] == index && pTaquin->plateau[x][y] == (x - 1 + y * pTaquin->hauteur));
				if (x < pTaquin->largeur - 1)
					tot += (pTaquin->plateau[x + 1][y] == index && pTaquin->plateau[x][y] == (x + 1 + y * pTaquin->hauteur));
				if (y)
					tot += (pTaquin->plateau[x][y - 1] == index && pTaquin->plateau[x][y] == (x + (y - 1) * pTaquin->hauteur));
				if (y < pTaquin->hauteur - 1)
					tot += (pTaquin->plateau[x][y + 1] == index && pTaquin->plateau[x][y] == (x + (y + 1) * pTaquin->hauteur));
			}
			else
				tot -= index;
			++index;
		}

	if (pTaquin->hauteur== 2)
		return tot;

	int hauteur_1 = pTaquin->hauteur - 1;
	for (int x = 0; x < pTaquin->largeur; x++)
		if (pTaquin->plateau[hauteur_1][x] != (hauteur_1 * (hauteur_1 + 1) + x))
			return tot;
	
	tot -= 10;
	if (hauteur_1 == 2)
		return tot;
	--hauteur_1;

	for (int x = 0; x < pTaquin->largeur; x++)
		if (pTaquin->plateau[hauteur_1][x] != (hauteur_1 * (hauteur_1 + 1) + x))
			return tot;
	tot -= 50;

	return tot;
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