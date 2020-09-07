#include <stdbool.h>

TCoupReq construireCoup(int socket,TSensTetePiece sens,int partie,bool* breaker);
TSensTetePiece debutPartie(int sock, TPartieReq partieReq, TPartieRep partieRep);
TCoupReq receptionAdverse(int sock,bool* breaker);
void receptionValidation(int sock,bool* breaker);
CoupAdvJava construireMove(TCoupReq coupReqAdversaire,bool* breaker);


