#include "AStar.h"
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include "SDL.h"
#include "SDL_image.h"



// fonction pour créer (allouer) un noeud de liste et l'initialiser avec le taquin passé en paramètre
ptrListAStar createNodeList(Taquin * pTaquin, int gValue, int fValue, deplacement d, ptrListAStar pPrevPlay)
{
	ptrListAStar node = calloc(1, sizeof(ListAStar));
	node->g = gValue;
	node->f = fValue;
	node->pTaquin;
	node->prev_d = d;
	node->prev_node = pPrevPlay;
	return node;
}

// Insertion dans la liste de façon triée ou non
// si on passe le paramètre tri à 0, on insère en tête de liste
int insertList(ptrListAStar * ppHead, ptrListAStar pNode, int tri)
{
	if (!tri || (*ppHead)->f > pNode->f) {
		pNode->post_node = (*ppHead);
		ppHead = &(pNode);
		return 0;
	}
	ptrListAStar* cursor = ppHead;
	while (pNode->f >= (*cursor)->post_node->f)
		cursor = (*cursor)->post_node;
	pNode->post_node = (*cursor)->post_node;
	(*cursor)->post_node = pNode;

	return 0;
}

// Fonction pour prélever le noeud en tête de liste
// Retourne le noeud prélevé
ptrListAStar popList(ptrListAStar * ppHead)
{
	ptrListAStar node = (*ppHead);
	ppHead = &(node->post_node);

	return node;
}

// fonction qui retourne le noeud dans lequel on trouve le taquin passé en paramètre (pointeur sur le pointeur dans la liste)
ptrListAStar * isInList(ptrListAStar * ppHead, Taquin * pTaquin)
{
	ptrListAStar* cursor = ppHead;
	while (memcmp(&(pTaquin), &((*cursor)->pTaquin), sizeof(Taquin)))
		cursor = &((*cursor)->post_node);
	
	return cursor;
}

// fonction pour afficher une liste
// si on met displayFGH à 0 les valeur de F, G et H ne sont pas affichées
int displayList(ptrListAStar pHead, int displayFGH)
{
	return 1;
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
int solveTaquin(Taquin *pTaquin, deplacement ** pTabDeplacement, unsigned long *pNbDeplacements, unsigned long * pNbTaquinsGeneres, unsigned long * pTimeElapsed, int stepByStep, SDL_Surface * pWindow)
{
	int heu = h(pTaquin);
	ptrListAStar closed = createNodeList(pTaquin, 0, 0, AUCUN, NULL);
	ptrListAStar open = createNodeList(pTaquin, 0, heu, AUCUN, NULL);
	ptrListAStar cursor = NULL;

	while (open) {
		cursor = popList(&open);
		insertList(&closed, cursor,1);



	}


	return 1;
}

// fonction d'évaluation pour la résolution avec AStar
int h(Taquin * pTaquin)
{

	return 0;
}