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
		return 1;

	copyTaquin(pTaquin, &(node->pTaquin));
	moveTaquin(&(node->pTaquin), d);

	node->g = gValue;
	node->f = fValue;
	node->prev_d = d;
	node->prev_node = pPrevPlay;
	return node;
}

// Insertion dans la liste de façon triée ou non
// si on passe le paramètre tri à 0, on insère en tête de liste
int insertList(ptrListAStar* ppHead, ptrListAStar pNode, int tri)
{
	if (!pNode)
		return 1;
	if (!tri || !(*ppHead)) {
		pNode->post_node = (*ppHead);
		ppHead = &(pNode);
		return 0;
	}
	ptrListAStar* cursor = ppHead;
	while ((*cursor)->post_node && pNode->f > (*cursor)->post_node->f)
		cursor = &(*cursor)->post_node;

	pNode->post_node = (*cursor)->post_node;
	(*cursor)->post_node = pNode;

	return 0;
}

// Fonction pour prélever le noeud en tête de liste
// Retourne le noeud prélevé
ptrListAStar popList(ptrListAStar* ppHead)
{
	if (!ppHead || !(*ppHead))
		return NULL;
	ptrListAStar node = (*ppHead);
	ppHead = &(node->post_node);

	return node;
}

// fonction qui retourne le noeud dans lequel on trouve le taquin passé en paramètre (pointeur sur le pointeur dans la liste)
ptrListAStar* isInList(ptrListAStar* ppHead, Taquin* pTaquin)
{
	ptrListAStar* cursor = ppHead;
	if (!cursor || !(*cursor))
		return NULL;

	while (!equalTaquin(pTaquin, &((*cursor)->pTaquin)))
	{
		if (!((*cursor)->post_node))
			cursor = &((*cursor)->post_node);
		else
			return NULL;
	}
	return cursor;
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
	ptrListAStar closed = NULL;
	ptrListAStar open = createNodeList(pTaquin, 0, h(pTaquin), AUCUN, NULL);
	ptrListAStar cursor = NULL;
	ptrListAStar cursorchild = NULL;
	ptrListAStar compare = NULL;
	int g = 0;
	int end = 0;
	while (!end) 
	{
		++g;
		cursor = popList(&open);
		insertList(&closed, cursor, 0);

		for (int i = 1; i < 5; i++)
		{
			cursorchild = createNodeList(&(cursor->pTaquin), g, g + h(&(cursor->pTaquin)), i, cursor);
			if (equalTaquin((&cursorchild->pTaquin), InitialTaquin(&(cursor->pTaquin))))
			{
				copyTaquin(&(cursorchild->pTaquin), pTaquin);
				
				printf("PRIVATE\n");
				displayTaquin(&(cursorchild->pTaquin), 0);
				//displayTaquin(InitialTaquin(&(cursor->pTaquin)), 0);


				end = 1;
				break;
			}
			compare = isInList(&open, &(cursorchild->pTaquin));
			if (compare) //si la board existe deja bon on s'en bas un peu la race a mais quoique deuxieme est ce que le score mais au final flemme je suppose
				continue; //on abandonne l'enfant
			if (isInList(&closed, &(cursorchild->pTaquin)))
				continue; //on abandonne l'enfant
			cursorchild = cursorchild->post_node;

			insertList(&open, cursorchild, 1);
		}
		//NOW need to recompose the way



	}
	freeList(&open);
	freeList(&closed);

	return 1;
}

// fonction d'évaluation pour la résolution avec AStar
int h(Taquin* pTaquin)
{
	int where, xx, yy, tot = 0;
	for (int x = 0; x < pTaquin->hauteur; ++x)
		for (int y = 0; y < pTaquin->largeur; ++y) {
			where = pTaquin->plateau[x][y];
			xx = where % pTaquin->largeur;
			yy = where / pTaquin->largeur;
			tot += abs(xx - x) + abs(yy - y);
		}

	return tot;
}

void freeList(ptrListAStar* ppHead)
{
	if (!ppHead || !(*ppHead))
		return;
	
	freeList(&((*ppHead)->post_node));
	freeTaquin(&((*ppHead)->pTaquin));
	
	free((*ppHead));
	*ppHead == NULL;
	
	return;
}