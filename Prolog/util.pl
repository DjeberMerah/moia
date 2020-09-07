%coorToString/3
%Transpose le stockage d'un coup en coordonnées vers un tableau de la forme [A,5,B,6]
%coorToString(C1,C2,S) : 
				%C1 = Coordonnées initiales
				%C2 = Coordonnées après coup
				%S = Coup sous la forme d'un tableau
coorToString([X,Y],[X2,Y2],S):-
	xToString(X,S1),
	xToString(X2,S3),
	yToString(Y,S2),
	yToString(Y2,S4),
	atom_chars(A1,S1),
	atom_chars(A2,S2),
	atom_chars(A3,S3),
	atom_chars(A4,S4),
	atom_concat(A1,A2,A5),
	atom_concat(A3,A4,A6),
	atom_concat(A5,A6,A7),
	atom_chars(A7,S).

%xToString/1
%Transposition du numéro de ligne en lettre
xToString(0,"A").
xToString(1,"B").
xToString(2,"C").
xToString(3,"D").
xToString(4,"E").

%yToString/1
%Transposition du numéro de colonne en numéro de quadrillage
yToString(0,"6").
yToString(1,"5").
yToString(2,"4").
yToString(3,"3").
yToString(4,"2").
yToString(5,"1").

%reverse/3
%Renverse une liste
reverse([],Z,Z).
reverse([H|T],Z,Acc) :- reverse(T,Z,[H|Acc]).

%deleteSingle/3
%Retire un seul élément de la liste
deleteSingle([],_,[]).
deleteSingle([X|L],X,L):-
	!.
deleteSingle([P|L],X,[P|LR]):
	deleteSingle(L,X,LR).