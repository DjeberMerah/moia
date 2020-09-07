%:-simu/8
%Effectue une simulation d'une partie
%simu(Grid, Turn, Player, MyPlayer, MoveList,_, NewMoveList, Winner) :
					%Grid = La grille de jeu au tour Turn
					%Turn = Le numéro du tour actuel
					%Player = Le joueur dont c'est le tour
					%MyPlayer = Joueur représenté par l'IA
					%MoveList = Une structure correspondant au stockage de l'arbre des rollouts de la forme
						%TODO À modifier si usage des tables de commutations
							%[[Move, Throughs, WinP1, WinP2, Draw, ValueUCB1, KeyList, MoveList]*] avec : 
									%Move de la forme [[X,Y],[X2,Y2]] symbolisant un coup traité
									%Throughs = Le nombre de simulation passant par ce noeud
									%WinP1 = Le nombre de victoires de P1 après être passé par ce noeud
									%WinP2 = Le nombre de victoires de P2 après être passé par ce noeud
									%Draw  = Le nombre d'égalité après être passé par ce noeud
									%ValueUCB1 = La valeur UCB1 du noeud, afin d'optimiser l'expansion des rollouts
									%KeyList = La liste des moves fils déjà traités (pour accelérer le traitement UCT)
									%MoveList = L'arbre des rollouts à partir de ce  noeud
					%_ = Argument obsolète
					%NewMoveList = Valeur de moveList après simulation [O]
					%Winner = Gagnant de la simulation [O]
simu(_, Turn, _,_, L,_,L2, 0) :-
	Turn > 60,!,incrThroughs(L,L2).

simu(Grid, _, _,_, L,_,L2, Opp):-
	isCheck(Grid,Y),
	Opp is -Y,
	hasWin(Grid,Opp),!,incrThroughs(L,L2).

simu(Grid, _, _,_, L,_,L2, Opp):-
	Grid = [P1,_,[],_],
	length(P1,1),
	Opp is -1,
	allAvailablePlays(Grid, 1, []),!,incrThroughs(L,L2).

simu(Grid, _, _,_, L,_,L2, Opp):-
	Grid = [_,P2,_,[]],
	length(P2,1),
	Opp is 1,
	allAvailablePlays(Grid, -1, []),!,incrThroughs(L,L2).

simu(Grid, Turn, Player,MyPlayer, MoveList, _, NewMoveList,Winner):-
	allAvailablePlays(Grid,Player,Moves),
	toExpand(Moves,MoveList,MyPlayer,TmpMoveList,[P,T]),
	actuallyMovePiece(P,Player,Grid,T,GR),
	incrThroughs(TmpMoveList, Tmp2MoveList),
	getCorrectList([P,T], Tmp2MoveList, ListToTreat),
	Turn1 is Turn+1,
	NextPlayer is -Player,
	getThroughs(Tmp2MoveList, Throughs),
	simu(GR, Turn1, NextPlayer,MyPlayer, ListToTreat, Throughs, TmpNewMoveList, Winner),
	getMoveList(Tmp2MoveList, TmpNewNewMoveList),
	changeMoveList([P,T], TmpNewNewMoveList, TmpNewMoveList , UpdatedMoveList),
	setMoveList(Tmp2MoveList, UpdatedMoveList, CompleteUpdatedMoveList),
	updateValueWin(CompleteUpdatedMoveList, Winner, MyPlayer, NewMoveList).
	%Ici, on regarde dans notre moveList si on a un move possible qui n'est pas traité à partir de notre Grid
	%Si c'est le cas, on choisi un de ces moves au random
	%Sinon, on utilise les différentes valeurs UCB1 pour choisir le noeud à traiter
	%Puis, on effectue la simulation au niveau plus bas et on actualise enfin notre bordel avec le through and co ...(Il va falloir séparer la vérification et l'application du move)

%:-notAlreadyPassedBy/1
%Vérifie qu'aucune simulation n'a continué après ce noeud
notAlreadyPassedBy([_,_,_,_,_,_,[],[]]).

%:-isTimeout/1
%Vérifie que l'on n'ai pas dépassé la valeur du timeout
isTimeout(Depart):-
	statistics(runtime, [Fin,_]),
	Time is (Fin-Depart)/1000,
	Time < 3/2. %Modifier ici la valeur du Timeout (en seconde)

%:-simuUntilTimeout/7
%Effectue des simulations jusqu'à dépasser le timeout.
%simuUntilTimeout(Depart, MoveList, Grid, Turn, FirstPlayer, MyPlayer, FinalMoveList) :
					%Depart = TimeStamp du démarrage des simulations
					%MoveList = MoveNode avant simulation
					%Grid = Grille au tour actuel, au démarrage de la simulation
					%Turn = Numéro du tour
					%FirstPlayer = Joueur dont c'est le tour
					%MyPlayer = Joueur représenté par l'IA
					%FinalMoveList = MoveNode après simulation [O]
simuUntilTimeout(Depart, MoveList, Grid, Turn, FirstPlayer, MyPlayer, FinalMoveList):-
	isTimeout(Depart),
	!,
	simu(Grid,Turn,FirstPlayer,MyPlayer, MoveList,1,TmpMoveList,_),
	simuUntilTimeout(Depart, TmpMoveList, Grid, Turn, FirstPlayer,MyPlayer, FinalMoveList).

simuUntilTimeout(_, MoveList, _, _, _,_, MoveList).

%:-allAvailablePlays/3
%Génère l'ensemble des coups disponibles (drops et déplacement)
%allAvailablePlays(G,J,R) :
				%G = Grille actuelle
				%J = Joueur dont c'est le tour
				%R = Liste de coups possibles [O]
allAvailablePlays(G,J,R):-
	allAvailableMoves(G,J,MR),
	allAvailableDrops(G,J,DR),
	append(MR,DR,R).


%:-allAvailableMoves/3
%Génère l'ensemble des déplacement disponibles
%allAvailableMoves(G,J,R) : 
				%G = Grille actuelle
				%J = Joueur dont c'est le tour
				%R = Liste de déplacement possibles [O]
allAvailableMoves(G, 1, LR) :-
	G = [P1,_,_,_],
	pieceAvailableMoves(P1, G, 1, LR).
allAvailableMoves(G, -1, LR) :-
	G = [_,P2,_,_],
	pieceAvailableMoves(P2, G, -1, LR).

%:-pieceAvailableMoves/4
%Génère la liste des déplacements disponibles pour une liste de pièce
%pieceAvailableMoves(L, G, J, R):
				%L = Liste des pièces disponibles
				%G = Grille actuelle
				%J = Joueur dont c'est le tour
				%R = Liste de déplacement possiles [O]
pieceAvailableMoves([],_,_,[]).
pieceAvailableMoves([P|L], G, J, LR):-
	findall(T, availableMovePiece(P,J,G,T), LPR),
	createMoves(LPR,P,LLR),
	pieceAvailableMoves(L,G,J,LTR),
	append(LLR,LTR,LR).

%:-createMoves/3
%Crée des structures moves à partir d'une liste de destinations et d'une origine
%createMoves(D, O, R) : 
				%D = Liste de destinations
				%O = origine et identifiant de pièce
				%R = List de moves [O]
createMoves([],_,[]).
createMoves([T|L],P,LR):-
	createMoves(L,P,LLR),
	append([[P,T]],LLR,LR).

%:-allAvailableDrops/3
%Génère l'ensemble des drops disponibles
%allAvailableDrops(G,J,R) : 
				%G = Grille actuelle
				%J = Joueur dont c'est le tour
				%R = Liste de drops possibles [O]
allAvailableDrops(G,1,LR):-
	G = [_,_,P1,_],
	pieceAvailableDrops(P1,G,1,LR).
allAvailableDrops(G,-1,LR):-
	G = [_,_,_,P2],
	pieceAvailableDrops(P2,G,-1,LR).

%:-pieceAvailableDrops/4
%Génère la liste des drops disponibles pour une liste de pièce
%pieceAvailableDrops(L, G, J, R):
				%L = Liste des pièces disponibles
				%G = Grille actuelle
				%J = Joueur dont c'est le tour
				%R = Liste de drops possibles [O]
pieceAvailableDrops([],_,_,[]).
pieceAvailableDrops([P|L], G, J, LR):-
	findall(T, availableDrop(P,J,G,T), LPR),
	createDrops(LPR, P,LLR),
	pieceAvailableDrops(L, G, J, LTR),
	append(LLR, LTR, LR).

%:-createMoves/3
%Crée des structures moves à partir d'une liste de destinations et d'un type de pièce
%createMoves(D, O, R) : 
				%D = Liste de destinations
				%O = identifiant de pièce
				%R = List de moves [O]
createDrops([],_,[]).
createDrops([T|L],P,LR):-
	createDrops(L,P,LLR),
	append([[[[-1,-1],P],T]],LLR,LR).

%:-toExpand/5
%Détermine la branche où pratiquer l'extension, ou au hasard, ou par UCB
%toExpand(Moves, MoveNode, MJ, NewMovelist, MoveToExpand) :
				%Moves = Liste des coups disponibles
				%MoveNode = Noeud parent
				%MJ = identifiant du joueur représenté par l'IA
				%NewMovelist = MoveNode modifiée, si besoin est (rajout d'un fils) [O]
				%MoveToExpand = Le noeud à appliquer [O]
toExpand(Moves, MoveList, _, NewMoveList, MoveToExpand) :-
	getKeyList(MoveList, KeyList),
	notAlreadyTreated(Moves, KeyList, NotTreatedMoves),
	NotTreatedMoves \= [],
	!, % Pour ne pas recalculer notAlreadyTreated
	length(NotTreatedMoves, Size),
	random(0,Size,Index),
	nth0(Index, NotTreatedMoves,  MoveToExpand),
	addMove(MoveToExpand, MoveList, NewMoveList).
toExpand(_, MoveList, MJ, MoveList, MoveToExpand) :-
	maxValue(MoveList, MJ, BigMoveToExpand),
	getMove(BigMoveToExpand, MoveToExpand).

%:-notAlreadyTreated/3
%Renvoie la liste des coups non traités parmi les coups disponibles
%notAlreadyTreated(Ms, Ks, NTM) :
				%Ms = Liste des coups disponibles
				%Ks = Liste des coups déjà traités
				%NTM = Liste des coups non encore traités [O]
notAlreadyTreated([], _, []).
notAlreadyTreated([M|Moves], KeyList, NotTreatedMoves):-
	\+ member(M, KeyList),
	!,
	notAlreadyTreated(Moves,KeyList,TmpNotTreatedMoves),
	append([M], TmpNotTreatedMoves, NotTreatedMoves).
notAlreadyTreated([_|Moves], KeyList, NotTreatedMoves):-
	notAlreadyTreated(Moves,KeyList,NotTreatedMoves).