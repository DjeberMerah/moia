%:-moveForward/2
%:-moveBackward/2
%:-moveRight/2
%:-moveLeft/2
%:-moveForwardRight/2
%:-moveForwardLeft/2
%:-moveBackwardRight/2
%:-moveBackwardLeft/2
%Définit les différents mouvement possibles
%moveDir([X,Y],J,[X2,Y2]) : 
					%[X,Y] = Coordonnées initiales
					%J = Joueur (1 pour joueur du bas, -1 pour joueur du dessus)
					%[X2,Y2] = Coordonnées d'arrivée [O]
moveForward([X,Y],J,[X,Y2]) :- Y2 is Y+J.
moveBackward([X,Y],J,[X,Y2]) :- Y2 is Y-J.
moveRight([X,Y],J,[X2,Y]) :- X2 is X+J.
moveLeft([X,Y],J,[X2,Y]) :- X2 is X-J.
moveForwardRight([X,Y],J,[X2,Y2]) :- X2 is X+J,Y2 is Y+J.
moveForwardLeft([X,Y],J,[X2,Y2]) :- X2 is X-J,Y2 is Y+J.
moveBackwardRight([X,Y],J,[X2,Y2]) :- X2 is X+J,Y2 is Y-J.
moveBackwardLeft([X,Y],J,[X2,Y2]) :- X2 is X-J,Y2 is Y-J.

%:-moveKd/3
%Move Kodama
moveKd(A,J,R) :- moveForward(A,J,R).

%:-moveS/3
%Move Haut Bas Gauche Droite Diagonale Avant
moveS(A,J,R) :- moveForward(A,J,R).
moveS(A,J,R) :- moveBackward(A,J,R).
moveS(A,J,R) :- moveRight(A,J,R).
moveS(A,J,R) :- moveLeft(A,J,R).
moveS(A,J,R) :- moveForwardRight(A,J,R).
moveS(A,J,R) :- moveForwardLeft(A,J,R).

%:-moveKdS/3
%Move Kodama Samourai
moveKdS(A,J,R) :- moveS(A,J,R).

%:-moveKr/3
%Move Kirin
moveKr(A,J,R) :- moveS(A,J,R).

%:-moveSO/3
%Move Super Oni
moveSO(A,J,R) :- moveS(A,J,R).

%:-moveKk/3
%Move Koropokkuru
moveKk(A,J,R) :- moveS(A,J,R).
moveKk(A,J,R) :- moveBackwardRight(A,J,R).
moveKk(A,J,R) :- moveBackwardLeft(A,J,R).

%:-moveO/3
%Move Oni
moveO(A,J,R) :- moveForward(A,J,R).
moveO(A,J,R) :- moveForwardRight(A,J,R).
moveO(A,J,R) :- moveForwardLeft(A,J,R).
moveO(A,J,R) :- moveBackwardRight(A,J,R).
moveO(A,J,R) :- moveBackwardLeft(A,J,R).

%:-validCoorGrid/1
%Vérifie que deux coordonnées sont bien dans la grille
validCoorGrid([X,Y]):-
	X >= 0,
	Y >= 0,
	5 > X,
	5 >= Y.%TODO Rajouter la gestion des interdictions + deux yokai sur la même case


%:-validSuper/3
%Vérfie qu'une pièce n'en superpose pas une autre
%validSuper(C,J,G) :
				%C = Coordonnées de la pièce
				%J = Identifiant du joueur
				%G = Grille de jeu
validSuper(C,1,[P1,_,_,_]):-
	validSuperBis(C,P1).
validSuper(C,-1,[_,P2,_,_]):-
	validSuperBis(C,P2).

%:-validSuperBis/2
%Fonction auxiliaire permettant la vérification qu'aucun couple de coordonnées
%de la liste ne soit celui passé en paramètre.
%validSuperBis(C,L) :
			%C = Coordonnées de la pièce
			%L = Liste à tester
validSuperBis(_,[]).
validSuperBis([X,Y],[[[X2,Y2],_]|G]):-
	[X,Y] \= [X2,Y2],
	validSuperBis([X,Y],G).


%:-upgradeZone/2
%Définit la zone d'amélioration par joueur
%upgradeZone(J,L) :
				%J = Identifiant du Joueur
				%L = Liste des valeurs de Y correspondant à la zone d'amélioration [O]
upgradeZone(1,[4,5]).
upgradeZone(-1,[0,1]).

%:-upgrade/3
%Si la piece est un kodama ou un oni,
%et se trouve après mouvement dans la zone d'amélioration
%on la passe à la catégorie d'après
%upgrade(P,J,R) : 
				%P = Pièce sous la forme [C,I] :
							%C = Coordonnées de la pièce
							%I = Identifiant de la pièce
				%J = Identifiant du joueur
				%R = Pièce après "upgrade" de la pièce [O]
upgrade([[X,Y],N],J,R):-
	piece(A,kodama),
	piece(B,oni),
	member(N,[A,B]),
	upgradeZone(J,L),
	member(Y,L),
	!, %Pour éviter un run sans l'upgrade par la suite
	M is N+1,
	R = [[X,Y],M].
%Permet la continuation du programme si la pièce n'est pas upgradable
upgrade(A,_,A).

%:-move/3
%Fonction générale de mouvement
%move(P,J,R) : 
				%P = Pièce sous la forme [C,I] :
							%C = Coordonnées de la pièce
							%I = Identifiant de la pièce
				%J = Identifiant du joueur
				%R = Coordonnées après mouvement de la pièce [O]
move([C,I],J,R):-
	piece(I,kodama),
	moveKd(C,J,R).
move([C,I],J,R):-
	piece(I,kodama_samourai),
	moveKdS(C,J,R).
move([C,I],J,R):-
	piece(I,kirin),
	moveKr(C,J,R).
move([C,I],J,R):-
	piece(I,koropokkuru),
	moveKk(C,J,R).
move([C,I],J,R):-
	piece(I,oni),
	moveO(C,J,R).
move([C,I],J,R):-
	piece(I,super_oni),
	moveSO(C,J,R).

%:-piece/2
%Définit les identifiants des pièces
piece(1,kodama).
piece(2,kodama_samourai).
piece(3,kirin).
piece(4,koropokkuru).
piece(5,oni).
piece(6,super_oni).

%:-own/3
%Vérifie que le joueur possède bien la pièce
%own(P,J,G) : 
				%P = Pièce sous la forme [C,I] : 
						%C = Coordonnées de la pièce
						%I = Identifiant de la pièce
				%J = Identifiant du joueur
				%G = Grille de jeu
own(P,1,[G,_,_,_]):-
	member(P,G).
own(P,-1,[_,G,_,_]):-
	member(P,G).

ownDrop(P,1,[_,_,C,_]):-
	member(P,C).
ownDrop(P,-1,[_,_,_,C]):-
	member(P,C).

%:-attack/3
%Met à jour la grille si une attaque a lieu
%attack(P,J,G,NG) :
				%P = Pièce sous la forme [C,I] : 
						%C = Coordonnées de la pièce
						%I = Identifiant de la pièce
				%J = Identifiant du joueur
				%G = Grille de jeu
				%NG = Grille de jeu après attaque [O]
attack([T,_],J,G,G):-
	Opp is -J,
	validSuper(T,Opp,G).
attack([T,_],J,G,NG):-
	Opp is -J,
	\+ validSuper(T,Opp,G),
	removePieceFromGrid(T,Opp,G,NG).


%:-movePiece/4
%Déplace et upgrade si besoin une pièce
%movePiece(P,J,G,GR) :
				%P = Pièce sous la forme [C,I] : 
						%C = Coordonnées de la pièce
						%I = Identifiant de la pièce
				%J = Identifiant du joueur
				%G = Grille de jeu
				%GR = Grille de jeu après mouvement [O]
movePiece([A,N],J,G,GR):-
	own([A,N],J,G),
	move([A,N],J,T),
	validCoorGrid(T),
	validSuper(T,J,G),
	upgrade([T,N],J,R),
	attack(R,J,G,NG),
	updateGrid(NG,[A,N],J,R,GR).

%:-availableMovePiece/4
%Renvoie un déplacement possible d'une pièce
%availableMovePiece(P,J,G,T) :
				%P = Pièce sous la forme [C,I] : 
						%C = Coordonnées de la pièce
						%I = Identifiant de la pièce
				%J = Identifiant du joueur
				%G = Grille de jeu
				%T = déplacement suggéré [O]
availableMovePiece([A,N], J, G, T):-
	own([A,N],J,G),
	move([A,N],J,T),
	validCoorGrid(T),
	validSuper(T,J,G),
	actuallyMovePiece([A,N],J,G,T,GT)
	,\+ isCheck(GT,J)
	.

%:-availableDrop/4
%Renvoie un drop possible d'une pièce
%availableDrop(P,J,G,T) :
				%P = Pièce sous la forme [C,I] : 
						%C = Coordonnées de la pièce
						%I = Identifiant de la pièce
				%J = Identifiant du joueur
				%G = Grille de jeu
				%T = Drop suggéré [O]
availableDrop(N, J, G, T):-
	ownDrop(N,J,G),
	gridCase(T),
	Opp is -J,
	validSuper(T,J,G),
	validSuper(T,Opp,G),
	validKodamaDrop(N, J, G, T),
	dropGrid(N,J,G,T,GT),
	\+isCheck(GT,J).

%:-actuallyMovePiece/5
%Déplace et upgrade si besoin une pièce
%movePiece(P,J,G,T,GR) :
				%P = Pièce sous la forme [C,I] : 
						%C = Coordonnées de la pièce
						%I = Identifiant de la pièce
				%J = Identifiant du joueur
				%G = Grille de jeu
				%T = Coordonnées d'arrivées du coup
				%GR = Grille de jeu après mouvement [O]
actuallyMovePiece([[-1,-1],N],J,G,T,GR):-
	!,
	dropGrid(N, J, G, T, GR).
actuallyMovePiece([A,N], J, G, T, GR):-
	upgrade([T,N], J, R),
	attack(R,J,G,NG),
	updateGrid(NG,[A,N],J,R,GR).

%:-validKodamaDrop/4
%Vérifie qu'un kodama n'est pas parachuté sur une colonne possédant déjà un kodama
%validKodamaDrop(P, J, G, T) :
				%P = Identifiant de la pièce
				%J = Identifiant du joueur
				%G = Grille de jeu
				%T = Coordonnées de drop
validKodamaDrop(Kodama, 1, [P1,_,_,_], [A,B]):-
	piece(Kodama, kodama),
	!,
	B < 5,
	validColumn(A,P1).
validKodamaDrop(Kodama, -1,[_,P2,_,_], [A,B]):-
	piece(Kodama, kodama),
	!,
	B > 0,
	validColumn(A,P2).
validKodamaDrop(_,_,_,_).

%:-validColumn/2
%Vérifie qu'il n'y a pas de kodama sur la colonne donnée
%validColumn(A,L) :
				%A = Numéro de colonne
				%L = Liste de pièces
validColumn(_,[]).
validColumn(A,[[[A,_],1]|_]):-
	!,fail.
validColumn(A,[_|P]):-
	validColumn(A,P).