%----------------------------------
%				Units Tests util.pl
%----------------------------------
:-begin_tests(coorToStringTest).
test('coorToString - 0,0 -> 1,0', [true(S == "A6B6")]):-
	coorToString([0,0],[1,0],S).
test('coorToString - 4,5 -> 4,4', [true(S == "E1E2")]):-
	coorToString([4,5],[4,4],S).
:-end_tests(coorToStringTest).
:-begin_tests(reverseTest).
test('reverse - empty',[true(S==[])]):-
	reverse([],S,[]).
test('reverse - one element',[true(S==[1])]):-
	reverse([1],S,[]).
test('reverse - two element',[true(S==[1,2])]):-
	reverse([2,1],S,[]).
:-end_tests(reverseTest).

%----------------------------------
%				Units Tests MoveNode.pl
%----------------------------------

:-begin_tests(moveTest).
test('getMove - list', [true(Move == [[[2,2],1],[2,3]])]):-
	getMove([[[[2,2],1],[2,3]], 42, 12, 10, 20, 0, [], []],Move).
test('getMove - value', [true(Move == 0)]):-
	getMove([0,0,0,0,0,0,0,0],Move).
:-end_tests(moveTest).

:-begin_tests(throughsTest).
test('incrThroughs - negative',[true(Throughs == -1)]):-
	incrThroughs([0,-2,0,0,0,0,0,0],[0,Throughs,0,0,0,0,0,0]).
test('incrThroughs - zero',[true(Throughs == 1)]):-
	incrThroughs([0,0,0,0,0,0,0,0],[0,Throughs,0,0,0,0,0,0]).
test('incrThroughs - positive',[true(Throughs == 5)]):-
	incrThroughs([0,4,0,0,0,0,0,0],[0,Throughs,0,0,0,0,0,0]).
test('getThroughs - negative',[true(Throughs == -2)]):-
	getThroughs([0,-2,0,0,0,0,0,0],Throughs).
test('getThroughs - zero',[true(Throughs == 0)]):-
	getThroughs([0,0,0,0,0,0,0,0],Throughs).
test('getThroughs - positive',[true(Throughs == 5)]):-
	getThroughs([0,5,0,0,0,0,0,0],Throughs).
:-end_tests(throughsTest).

:-begin_tests(winP1Test).
test('incrWinP1 - negative',[true(WinP1 == -1)]):-
	incrWinP1([0,0,-2,0,0,0,0,0],[0,0,WinP1,0,0,0,0,0]).
test('incrWinP1 - zero',[true(WinP1 == 1)]):-
	incrWinP1([0,0,0,0,0,0,0,0],[0,0,WinP1,0,0,0,0,0]).
test('incrWinP1 - positive',[true(WinP1 == 5)]):-
	incrWinP1([0,0,4,0,0,0,0,0],[0,0,WinP1,0,0,0,0,0]).
test('getWinP1 - negative',[true(WinP1 == -2)]):-
	getWinP1([0,0,-2,0,0,0,0,0],WinP1).
test('getWinP1 - zero',[true(WinP1 == 0)]):-
	getWinP1([0,0,0,0,0,0,0,0],WinP1).
test('getWinP1 - positive',[true(WinP1 == 5)]):-
	getWinP1([0,0,5,0,0,0,0,0],WinP1).
:-end_tests(winP1Test).

:-begin_tests(winP2Test).
test('incrWinP2 - negative',[true(WinP2 == -1)]):-
	incrWinP2([0,0,0,-2,0,0,0,0],[0,0,0,WinP2,0,0,0,0]).
test('incrWinP2 - zero',[true(WinP2 == 1)]):-
	incrWinP2([0,0,0,0,0,0,0,0],[0,0,0,WinP2,0,0,0,0]).
test('incrWinP2 - positive',[true(WinP2 == 5)]):-
	incrWinP2([0,0,0,4,0,0,0,0],[0,0,0,WinP2,0,0,0,0]).
test('getWinP2 - negative',[true(WinP2 == -2)]):-
	getWinP2([0,0,0,-2,0,0,0,0],WinP2).
test('getWinP2 - zero',[true(WinP2 == 0)]):-
	getWinP2([0,0,0,0,0,0,0,0],WinP2).
test('getWinP2 - positive',[true(WinP2 == 5)]):-
	getWinP2([0,0,0,5,0,0,0,0],WinP2).
:-end_tests(winP2Test).

:-begin_tests(drawTest).
test('incrDraw - negative',[true(Draw == -1)]):-
	incrDraw([0,0,0,0,-2,0,0,0],[0,0,0,0,Draw,0,0,0]).
test('incrDraw - zero',[true(Draw == 1)]):-
	incrDraw([0,0,0,0,0,0,0,0],[0,0,0,0,Draw,0,0,0]).
test('incrDraw - positive',[true(Draw == 5)]):-
	incrDraw([0,0,0,0,4,0,0,0],[0,0,0,0,Draw,0,0,0]).
test('getDraw - negative',[true(Draw == -2)]):-
	getDraw([0,0,0,0,-2,0,0,0],Draw).
test('getDraw - zero',[true(Draw == 0)]):-
	getDraw([0,0,0,0,0,0,0,0],Draw).
test('getDraw - positive',[true(Draw == 5)]):-
	getDraw([0,0,0,0,5,0,0,0],Draw).
:-end_tests(drawTest).

:-begin_tests(maxMoveTest).
test('getMaxMove - Val1',[true(M = 1)]):-
	getMaxMove(1,2,2,1,M).
test('getMaxMove - Eq',[true(M = 1)]):-
	getMaxMove(1,-1,2,-1,M).
test('getMaxMove - Val2',[true(M = 2)]):-
	getMaxMove(1,1,2,3/2,M).
:-end_tests(maxMoveTest).

%----------------------------------
%				Units Tests MoveNode.pl
%----------------------------------

:-begin_tests(matTest).
test('shouldBeWin, 1',[true(J=1)]):-
	piece(Kodama, kodama),
	piece(Oni, oni),
	piece(SO, super_oni),
	piece(KP, koropokkuru),
	piece(KI, kirin),
	piece(KS, kodama_samourai),

	P1 = [
				[[0,2],Kodama],
				[[0,3],Oni],
				[[1,4],SO],
				[[3,0],KP],
				[[3,1],KI],
				[[3,3],Oni]
			 ],
	P2 = [
				[[0,0],KS],
				[[1,1],KS],
				[[1,2],KI],
				[[2,2],Kodama],
				[[2,5],KP],
				[[3,5],KI],
				[[4,5],Oni]
			 ],
	G = [P1,P2,[],[]],
	hasWin(G,J).
test('shoudNotBeWin',fail):-
	initialGrid(G),
	hasWin(G,_).
:-end_tests(matTest).