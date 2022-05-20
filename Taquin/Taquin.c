// Inclusion des bibliothèques utiles
#include "taquin.h"
#include "AStar.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <assert.h>

Uint64 hash(Taquin* _pTaquin) //djb2 algorithm http://www.cse.yorku.ca/~oz/hash.html
{
	
	Uint64 hash = 0;

	for (int x = 0; x < _pTaquin->largeur; x++)
		for (int y = 0; y < _pTaquin->hauteur; y++)
		{
			hash = (hash << 4) ^ _pTaquin->plateau[x][y];
		}

	return hash;

}


// Fonction pour copier un plateau de taquin pSrc vers pDest
int copyTaquin(Taquin* _pSrc, Taquin* _pDest)
{
	memcpy(_pDest, _pSrc, sizeof(Taquin));

	_pDest->plateau = (Uint8**)calloc(_pDest->largeur, sizeof(Uint8*));
	if (!_pDest->plateau)
		return 1;
	for (int i = 0; i < _pDest->largeur; i++)
	{
		(_pDest->plateau)[i] = (Uint8*)calloc(_pDest->hauteur, sizeof(Uint8));
		if (!(_pDest->plateau)[i])
		{
			freeTaquin(_pDest);
			return 1;
		}
		memcpy(_pDest->plateau[i], _pSrc->plateau[i], sizeof(Uint8) * _pSrc->hauteur);
	}

	return 0;
}

// fonction qui renvoie 1 si les 2 taquins sont identiques
// 0 sinon
// -1 si il y a une erreur au passage des paramètres
int equalTaquin(Taquin* _pTaquin1, Taquin* _pTaquin2)
{
	if (!_pTaquin1 || !_pTaquin2)
		return -1;

	for (int x = 0; x < _pTaquin1->largeur; x++)
	{
		if (memcmp(_pTaquin1->plateau[x], _pTaquin2->plateau[x], _pTaquin1->largeur * sizeof(Uint8)))
			return 0;

	}
	return 1;

}
// fonction qui renvoie 1 si les 2 taquins sont identiques
// 0 sinon
// -1 si il y a une erreur au passage des paramètres
int equalIdTaquin(Taquin* _pTaquin1, Taquin* _pTaquin2)
{
	//if (!_pTaquin1 || !_pTaquin2) //askip ça prend un max de temps proco
	//	return -1;

	return _pTaquin1->id == _pTaquin2->id;
}


// Fonction qui crée un plateau de taquin 
// ATTENTION le plateau doit être NULL avant l'appel à cette fonction 
// Si le plateau n'est pas NULL la fonction essayera de libérer la zone mémoire occupée par le plateau et cela peut donc aboutir à un crash si le plateau n'a pas été initialisé
int createTaquin(Taquin* _pTaquin, Uint8 _hauteur, Uint8 _largeur)
{
	// Test pour vérifier que les données passées ne sont pas corrompues
	if (!_pTaquin) return 0;

	if (_hauteur < SIZE_MINI) _hauteur = SIZE_MINI;
	if (_hauteur > SIZE_MAXI) _hauteur = SIZE_MAXI;
	if (_largeur < SIZE_MINI) _largeur = SIZE_MINI;
	if (_largeur > SIZE_MAXI) _largeur = SIZE_MAXI;

	freeTaquin(_pTaquin);

	_pTaquin->hauteur = _hauteur;
	_pTaquin->largeur = _largeur;

	_pTaquin->plateau = (Uint8**)calloc(_hauteur, sizeof(Uint8*));
	if (!_pTaquin->plateau) 
		return 0;

	for (int i = 0; i < _hauteur; i++)
	{
		_pTaquin->plateau[i] = (Uint8*)calloc(_largeur, sizeof(Uint8));
		if (!_pTaquin->plateau[i])
		{
			freeTaquin(_pTaquin);
			return 1;
		}
	}

	if (initTaquin(_pTaquin)) 
		return 1;

	return 0;
}


// Fonction qui initialise le taquin
int initTaquin(Taquin* _pTaquin)
{
	for (int x = 0; x < _pTaquin->largeur; x++)
		for (int y = 0; y < _pTaquin->hauteur; y++)
			_pTaquin->plateau[x][y] = (Uint8)(x + y * _pTaquin->hauteur);

	_pTaquin->x = 0;
	_pTaquin->y = 0;
	_pTaquin->id = hash(_pTaquin);

	return 0;
}

// Fonction qui mélange le taquin en effectuant entre minRandom et maxRandom coups aléatoires
int mixTaquin(Taquin* _pTaquin, int _minRandom, int _maxRandom)
{
	int alea = rand() % (_maxRandom - _minRandom + 1) + _minRandom;
	int d = AUCUN, rand_d = AUCUN;
	for (int i = 0; i < alea; i++)
	{
		rand_d = rand() % 4 + 1;
		if ((d == HAUT && rand_d == BAS) || (d == BAS && rand_d == HAUT) || (d == DROITE && rand_d == GAUCHE) || (d == GAUCHE && rand_d == DROITE)) {
			--i;
			continue;
		}
		if (moveTaquin(_pTaquin, rand_d)) --i;;
		//displayTaquin(_pTaquin, 0);
		d = rand_d;
	}

	return 1;
}

// Fonction qui permet de bouger une pièce du taquin (en bougeant la case vide)
int moveTaquin(Taquin* _pTaquin, deplacement _d)
{
	if (!_d) return 0;

	int x = _pTaquin->x, y = _pTaquin->y;
	int ret = 1;

	switch (_d) {
	case GAUCHE:
		if (x > 0) {
			_pTaquin->plateau[x][y] = _pTaquin->plateau[x - 1][y];
			_pTaquin->plateau[x - 1][y] = 0;
			--_pTaquin->x;
			ret = 0;
		}
		break;
	case DROITE:
		if (x < _pTaquin->largeur - 1) {
			_pTaquin->plateau[x][y] = _pTaquin->plateau[x + 1][y];
			_pTaquin->plateau[x + 1][y] = 0;
			++_pTaquin->x;
			ret = 0;
		}
		break;
	case BAS:
		if (y < _pTaquin->hauteur - 1) {
			_pTaquin->plateau[x][y] = _pTaquin->plateau[x][y + 1];
			_pTaquin->plateau[x][y + 1] = 0;
			++_pTaquin->y;
			ret = 0;
		}
		break;
	case HAUT:
		if (y > 0) {
			_pTaquin->plateau[x][y] = _pTaquin->plateau[x][y - 1];
			_pTaquin->plateau[x][y - 1] = 0;
			--_pTaquin->y;
			ret = 0;
		}
		break;
	}
	_pTaquin->id = hash(_pTaquin);

	return ret;
}

// Fonction qui renvoie le déplacement à effectuer pour annuler le déplacement donné en paramètre
deplacement cancelMove(deplacement _d)
{
	return (_d == GAUCHE) ? DROITE : (_d == DROITE) ? GAUCHE : (_d == HAUT) ? BAS : (_d == BAS) ? HAUT : AUCUN;
}

// Fonction qui renvoie 1 si le taquin es résolu, 0 sinon
int endTaquin(Taquin* _pTaquin)
{
	for (int x = 0; x < _pTaquin->largeur; x++)
		for (int y = 0; y < _pTaquin->hauteur; y++)
			if (_pTaquin->plateau[x][y] != (Uint8)(x + y * _pTaquin->hauteur))
				return 0;

	return 1;
}

// fonction d'affichage du taquin
int displayTaquin(Taquin* _pTaquin, int _offset)
{
	for (int i = 0; i < _pTaquin->largeur; i++)
		printf("________");
	printf("\n");
	for (int y = 0; y < _pTaquin->hauteur; y++)
	{
		printf("|");
		for (int x = 0; x < _pTaquin->largeur; x++)
			printf("%hhi\t|", _pTaquin->plateau[x][y]);
		printf("\n");

		for (int i = 0; i < _pTaquin->largeur; i++)
			printf("________");
		printf("\n");
	}
	printf("\n\n\n");
	return 0;
}

// Fonction pour libérer les zones mémoires occupées par un taquin
int freeTaquin(Taquin* _pTaquin)
{
	if (_pTaquin->plateau)
	{
		// On libère le plateau ligne par ligne
		for (int i = 0; i < _pTaquin->largeur; i++)
			free(_pTaquin->plateau[i]);
		// On libère le tableau qui stockait les lignes
		free(_pTaquin->plateau);
		_pTaquin->plateau = NULL;
	}

	return 0;
}
// Boucle de jeu 
int gameLoop(int hauteur, int largeur, int minRandom, int maxRandom)
{

	// BOUCLE DE JEU ! A DEVELOPPER
	while (!_kbhit())
	{
		printf(".");
	}
	return 1;
}