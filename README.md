# Taquin

**Comment jouer ?**

Vous pouvez jouer avec les flèches ou wasd pour déplacer le carré vide.

  Vous pouvez également jouer avec la souris en cliquant sur la case souhaitée, la case vide viendra la remplacer, si possible.

  F1 - permet de lancer une résolution A* en utilisant le chemin trouvé depuis le taquin de base.
       Le temps d'execution est très rapide mais le chemin trouvé n'est pas optimal.

  F2 - permet de ne faire qu'un seul déplacement d'une résolution A*, permet ainsi de recalculer le chemin 
       après chaque coup effectué par l'IA. 
       Le temps de calcul est plus long mais le chemin trouvé est techniquement l'un des meilleurs
       
  ENTREE - Lorsqu'une partie est terminée et que l'image est completement affichée, 
  	   appuyer sur la touche vous permet de remixer le jeu et de relancer une nouvelle partie.

_**Notre heuristique utilisée :**_
  Nous avons mixé plusieurs heuristiques afin d'être le plus précis possible.
  Dans un premier temps : 
      On ajoute 1 si la case n'est pas bien placée
  Dans un second temps :
      Nous calculons la distance de Manhattan (distance horizontale plus verticale)
  Ensuite :
     
	on augmente l'heuristique si les voisins du noeud sont inversibles avec lui meme 
	schema exemple : 
	 2 5 8     0 1 2
	 6 1 4     3 4 5
	 3 0 7     6 7 8
	 On peut observer que 6 & 3 sont inversibles et sont donc moins interessants a traiter

	if (x)
		total += (pTaquin->plateau[x - 1][y] == index && pTaquin->plateau[x][y] == (x - 1 + y * pTaquin->hauteur));
	if (x < pTaquin->largeur - 1)
		total += (pTaquin->plateau[x + 1][y] == index && pTaquin->plateau[x][y] == (x + 1 + y * pTaquin->hauteur));
	if (y)
		total += (pTaquin->plateau[x][y - 1] == index && pTaquin->plateau[x][y] == (x + (y - 1) * pTaquin->hauteur));
	if (y < pTaquin->hauteur - 1)
		total += (pTaquin->plateau[x][y + 1] == index && pTaquin->plateau[x][y] == (x + (y + 1) * pTaquin->hauteur));
          
  Pour finir :
      Nous baissons l'heuristique si les dernieres lignrs sont completes ou non.
      Pour une éxé c'est inutile.
      Pour un 3x3, la derniere suffit.
      Pour un 4x4 et une 5x5, ce sont les deux dernieres lignes qui sont regardées.
      Si on fait plus, l'IA ne pourra pas jouer correctement.

  Nous sous-évaluons l'heuristique en mettant le total à -1 dès le lacement du calcul de celle-ci.

_**Les quelques optimisation que nous avons pu trouver**_

  Nous avions dans un premier temps optimisé la fonction de compariason de grille 
  en utilisant le fonction < *memcmp()* > pour comparer les lignes des grilles
  Ainsi, nous evitons de faire de la comparaison case par case et de perdre du temps étant donné que c'est la plus appelée.
  
  Ensuite nous avons opté pour une autre solution :
  Nous avons donné à ajouté une ID unique à chaque grille existante.
  Cette fonction s'appelle < *equalIdUnique()* >.
  Et demande juste a comparer les des ID qui sont des Uint64 soit des *unsinged long long*
  
  Dans le < *solveTaquin()* > :
    On compare le mouvement actuel et son grand parent, si ils sont identiques on abandonne l'enfant
		On gagne un peu de temps à ne pas chercher ce cas dans les listes, qui sont parfois longues. 
  
  Toutes le instruction et conditions faites dans le < *solveTaquin()* > et les fonctions qu'elle appele 
  sont faites pour minimiser le temps de calcul et le nombre d'actions à realiser.
   
