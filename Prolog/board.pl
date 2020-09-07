%:-initialGrid/1
%Définit la grille initiale
initialGrid(G):-
	initialGrid(1,P1G),
	initialGrid(-1,P2G),
	G = [P1G,P2G,[],[]].

%:-gridCase/1
%Renvoie une case de la grille
gridCase([A,B]):-
	member(A,[0,1,2,3,4]),
	member(B,[0,1,2,3,4,5]).

%:-initialGrid/2
%Définit la grille initiale pour le joueur donné.
%initialGrid(J,G) : 
				%J = Identifiant du joueur
				%G = Grille initiale pour ce joueur [O]
initialGrid(1,PG):-
	piece(Kodama,kodama),
	piece(Oni,oni),
	piece(Kirin,kirin),
	piece(Koro,koropokkuru),
	PG = [
					[[1,2],Kodama],
					[[2,2],Kodama],
					[[3,2],Kodama],
					[[0,0],Oni],
					[[4,0],Oni],
					[[1,0],Kirin],
					[[3,0],Kirin],
					[[2,0],Koro]
	].
initialGrid(-1,PG):-
	piece(Kodama,kodama),
	piece(Oni,oni),
	piece(Kirin,kirin),
	piece(Koro,koropokkuru),
	PG = [
					[[1,3],Kodama],
					[[2,3],Kodama],
					[[3,3],Kodama],
					[[0,5],Oni],
					[[4,5],Oni],
					[[1,5],Kirin],
					[[3,5],Kirin],
					[[2,5],Koro]
	].

%:-updateGrid/5
%Met à jour la grille après le déplacement de P vers NP
%updateGrid(G,P,J,NP,NG) : 
				%G = Grille avant coup joué
				%P = Pièce avant coup joué
				%J = Identifiant du joueur
				%NP = Pièce après coup joué
				%NG = Grille après coup joué [O]
updateGrid([[P|G],IG,C1,C2],P,1,NP,[[NP|G],IG,C1,C2]).
updateGrid([[P2|G],IG,C1,C2],P,1,NP,GR):-
	P \= P2,
	updateGrid([G,IG,C1,C2],P,1,NP,[G1,G2,C3,C4]),
	GR = [[P2|G1],G2,C3,C4].
updateGrid([IG,[P|G],C1,C2],P,-1,NP,[IG,[NP|G],C1,C2]).
updateGrid([IG,[P2|G],C1,C2],P,-1,NP,GR):-
	P \= P2,
	updateGrid([IG,G,C1,C2],P,-1,NP,[G1,G2,C3,C4]),
	GR = [G1,[P2|G2],C3,C4].

%:-dropGrid/5
%Met à jour la grille après un parachutage
%dropGrid(N, J, G, T, NG):-
				%N = Identifiant de la pièce à parachuter
				%J = Identifiant du joueur
				%G = Grille avant parachutage
				%T = Coordonnées de parachutage
				%NG = Grille après parachutage [O]
dropGrid(N,1,[P1,P2,C1,C2],T,[P,P2,C,C2]):-
	deleteSingle(C1,N,C),
	append([[T,N]],P1,P).
dropGrid(N,-1,[P1,P2,C1,C2],T,[P1,P,C1,C]):-
	deleteSingle(C2,N,C),
	append([[T,N]],P2,P).


%:-removePieceFromGrid/4
%Retire une pièce de la grille de jeu
%removePieceFromGrid(T,J,G,NG) : 
				%T = Coordonnées de la Pièce
				%J = Identifiant du joueur
				%G = Grille de jeu
				%NG = Grille après retrait de la pièce [O]
removePieceFromGrid(T,1,[[[T,P]|G],IG,C1,C2],[G,IG,C1,[P2|C2]]):-
	downgrade(P,P2).
removePieceFromGrid(T,1,[[[T2,P2]|G],IG,C1,C2],[NG,IG,C3,C4]):-
	T \= T2,
	removePieceFromGrid(T,1,[G,IG,C1,C2],[NNG,IG,C3,C4]),
	NG = [[T2,P2]|NNG].
removePieceFromGrid(T,-1,[IG,[[T,P]|G],C1,C2],[IG,G,[P2|C1],C2]):-
	downgrade(P,P2).
removePieceFromGrid(T,-1,[IG,[[T2,P2]|G],C1,C2],[IG,NG,C3,C4]):-
	T \= T2,
	removePieceFromGrid(T,-1,[IG,G,C1,C2],[IG,NNG,C3,C4]),
	NG = [[T2,P2]|NNG].

%:-downgrade/2
%Remet la pièce à son état inital, si elle a été promue
%downgrade(P,P2) :
				%P = Identifiant de la pièce
				%P2 = Identifiant de la pièce dépromue [O]
downgrade(P,P2):-
	piece(KS, kodama_samourai),
	piece(SO, super_oni),
	member(P,[KS,SO]),
	!,
	P2 is P-1.
downgrade(P,P).

%:-firstHasWin/2
%Vérifie si un joueur a gagné la partie (no si aucune victoire)
%Utilise une vérification basée sur la prise du Roi
%firstHasWin(G,J) : 
				%G = Grille de jeu
				%J = Joueur gagnant [O]
firstHasWin([[],_,_,_],-1).
firstHasWin([_,[],_,_],1).
firstHasWin([_,[[_,N]|G],_,_], 1):-
	\+ piece(N,koropokkuru),
	firstHasWin([[1],G,[],[]], 1).
firstHasWin([[[_,N]|G],_,_,_], -1):-
	\+ piece(N,koropokkuru),
	firstHasWin([G,[1],[],[]], -1).

%:-hasWin/2
%Vérifie si un joueur a gagné la partie (no si aucune victoire)
%Utilise une vérification basée sur l'echec et mat
%hasWin(G,J):
				%G = Grille de jeu
				%J = Joueur Gagnant [O]
hasWin([P1,P2,_,_], Opp):-
	%Récupérer les coordonnées du roi de J
	%Regarder tous les moves des pièces de J dans un rayon de 2 du roi + plus le roi (pour tous, il doit y avoir capture possible par -J tout de même)
	%Regarder les moves de toutes les pièces de -J dans un rayon du 1 de nouveau roi
	J is -Opp,
	getKing(P1,P2,J,K),
	getPiecesWithDistance(K,2,P1,P2,J,Ps),
	verifMat(Ps,P1,P2,J).

%:-isCheck/2
%Vérifie si un joueur est en echec
%isCheck(G, J):
				%G = Grille de jeu
				%J = Joueur en echec [O]
isCheck([P1, P2, _, _], J):-
	getKing(P1,P2,J,K),
	Opp is -J,
	getPiecesWithDistance(K,sqrt(2)+1/10,P1,P2,Opp,Ps), %sqrt(2) pour diagonale
	isAttackable(K, Opp ,Ps).

%:-isAttackable/3
%Vérifie si une pièce peut être attaqué par une pièce d'une liste
%isAttackable(T,J,Ps) :
				%T = Coordonnées de la pièce attaquable
				%J = Identifiant du joueur
				%Ps = Liste des pièces attaquantes [O]
isAttackable(_, _,[]):-fail,!.
isAttackable(T, J,[P|_]):-
	move(P,J,T),
	!.
isAttackable(K, J,[_|Ps]):-
	isAttackable(K,J,Ps).

%:-getKing/4
%Récupère les coordonées du roi
%getKint(G1, G2, J, T) :
				%G1 = Pièces du joueur 1
				%G2 = Pièces du joueur 2
				%J = Identifiant du joueur
				%T = Coordonnées du roi [O]
getKing([[A,Koro]|_],_,1,A):-
	piece(Koro,koropokkuru),!.
getKing([_|P],_,1,K):-
	getKing(P,0,1,K).
getKing(_,[[A,Koro]|_],-1,A):-
	piece(Koro,koropokkuru),!.
getKing(_,[_|P],-1,K):-
	getKing(0,P,-1,K).

%:-getPiecesWithDistance/6
%Récupère la liste des pièces à moins d'une certaine distance d'une certaine pièce
%getPiecesWithDistance(K,D,G1,G2,J,Ps) : 
				%K = Coordonnées de la pièce "centrale"
				%D = Distance maximale
				%G1 = Pièces du joueur 1
				%G2 = Pièces du joueur 2
				%J = Identifiant du joueur
				%Ps = Liste des pièces à moins de D de K [O]
getPiecesWithDistance(K,D,[[C,P]|P1],P2,1,Ps):-
	getDistance(K,C,DK),
	D >= DK,!,
	getPiecesWithDistance(K,D,P1,P2,1,Pss),
	append([[C,P]],Pss,Ps).
getPiecesWithDistance(K,D,[_|P1],P2,1,Ps):-
	getPiecesWithDistance(K,D,P1,P2,1,Ps).
getPiecesWithDistance(_,_,[],_,1,[]).
getPiecesWithDistance(K,D,P1,[[C,P]|P2],-1,Ps):-
	getDistance(K,C,DK),
	D >= DK,!,
	getPiecesWithDistance(K,D,P1,P2,-1,Pss),
	append([[C,P]],Pss,Ps).
getPiecesWithDistance(K,D,P1,[_|P2],-1,Ps):-
	getPiecesWithDistance(K,D,P1,P2,-1,Ps).
getPiecesWithDistance(_,_,_,[],-1,[]).

%:-verifMat/4
%Vérifie que malgré la liste de pièce donnée, echec et Mat au joueur J
%verifMat(Ps, P1, P2, J) : 
				%Ps = Liste des pièces à vérifier
				%P1 = Pièces du joueur 1
				%P2 = Pièces du joueur 2
				%J = Identifiant du joueur possiblement echec et mat
verifMat(Ps,P1,P2,J):-
	pieceAvailableMoves(Ps,[P1,P2,[],[]],J,LR),
	execAndVerif(LR, P1, P2, J).


%:-execAndVerif/4
%Vérifie que les déplacement de la liste n'emêche pas la capture du roi
%execAndVerif(Ps, P1, P2, J) :
				%Ps = Liste des déplacement à effectuer
				%P1 = Pièces du joueur 1
				%P2 = Pièces du joueur 2
				%J = Joueur possiblement echec et mat
execAndVerif([[P,M] | L], P1, P2, J):-
	actuallyMovePiece(P,J,[P1,P2,_,_],M,[PP1,PP2,_,_]),
	getKing(PP1,PP2,J,K),
	%Opp is -J,
	%S is sqrt(2)+1/10,
	%getPiecesWithDistance(K,S,PP1,PP2,Opp,Ps),
	%pieceAvailableMoves(Ps, [PP1, PP2, [], []],Opp,LR),
	%verifCapt(LR,K),
	%execAndVerif(L,P1,P2,J).
	isCheck([PP1,PP2,_,_],J),
	execAndVerif(L,P1,P2,J).
execAndVerif([],_,_,_).

%:-verifCapt/2
%Vérifie qu'une liste de move capture une pièce aux coordonées données
%verifCapt(L, P) : 
				%L = Liste de déplacement
				%P = Pièce à capturer
verifCapt([],_):-fail,!.
verifCapt([[_,K]|_],K):-!.
verifCapt([_|L],K):-
	verifCapt(L,K).

%:-getDistance/3
%Calcule la distance entre deux pièces
%getDistance(P1, P2, D) : 
				%P1 = Pièce 1
				%P2 = Pièce 2
				%D = Distance entre P1 et P2 [O]
getDistance([X,Y],[X2,Y2],D):-
	D is sqrt((X-X2)*(X-X2)+(Y-Y2)*(Y-Y2)).

%:-isCapture/4
%Vérifie si un coup donné est une capture
%isCapture(M, J, G, C) :
				%M = Move à vérifier
				%J = Identifiant du joueur
				%G = Grille
				%C = Capture ou non [O]
isCapture([_,T],J, G, 1):-
	Opp is -J,
	\+ validSuper(T,Opp,G),!.
isCapture(_,_,_,0).

%:-startPlayer/3
%Renvoie l'Identifiant du joueur ayant commencé la partie
%startPlayer(T, J, FJ) : 
				%T = Le numéro du tour
				%J = L'Identifiant du joueur dont c'est le tour actuellement
				%FJ = L'Identifiant du premier joueur [O]
startPlayer(T,J,FJ):-
	M is T mod 2,
	M \= 0,
	!,
	FJ is -J.
startPlayer(_,J,J).

%:-reApplyMoves/4
%Réapplique l'ensemble des coups d'une liste à une grille
%reApplyMoves(L, J, G, GR) :
				%L = Liste de coups
				%J = Identifiant du joueur dont c'est le tour
				%G = Grille initiale
				%GR = Grille après application des coups [O]
reApplyMoves([],_,G,G).
reApplyMoves([[M,T]|MH],J,TG,G):-
	correct([M,T],[M2,T2]),
	actuallyMovePiece(M2,J,TG,T2,GR),
	Opp is -J,
	reApplyMoves(MH,Opp,GR,G).

%:-correct/2
%Corrige le décalage entre coup Prolog et coup Protocole
%correct(A,B) :
				%A = Coup correct d'après la définition Prolog
				%B = Coup correct d'après la définition Protocole
correct([[[A,B],C],[D,E]],[[[A,F],C],[D,G]]):-
	F is 5-B,
	G is 5-E.

%:-getMaxWinRate/2
%Renvoie le move présentant le plus haut taux de victoire parmi une liste
%getMaxWinRate(L,R) :
				%L = Liste de coups
				%R = Move avec le plus haut winrate
getMaxWinRate([M|L],R):-
	getMaxWinRate(L,T),
	getWinP1(M,WM),
	getThroughs(M,TM),
	getWinP1(T,WT),
	getThroughs(T,TT),
	ValM is WM/TM,
	ValT is WT/TT,
	getMaxMove(M,ValM,T,ValT,R).
getMaxWinRate([M],M):-!.