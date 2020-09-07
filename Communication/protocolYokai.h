/************************************************************
 *
 * Programme : protocolYokai.h
 *
 * Synopsis : entete du protocole d'acces au serveur arbitre
 *            pour le jeu Yokai No-Mori
 *
 * Ecrit par : VF, FB
 * Date :  25 / 02 / 19
 * 
 ************************************************************/

#ifndef _protocolYokai_h
#define _protocolYokai_h

#include <stdbool.h>
/* Taille des chaines de caracteres pour les noms */
#define T_NOM 30

/* Nombre de coups maximum par joueur */
#define NB_COUP_MAX 30

/* Identificateurs des requetes */
typedef enum { PARTIE, COUP } TIdReq;

/* Types d'erreur */
typedef enum { ERR_OK,      /* Validation de la requete */
	       ERR_PARTIE,  /* Erreur sur la demande de partie */
	       ERR_COUP,    /* Erreur sur le coup joue */
	       ERR_TYP      /* Erreur sur le type de requete */
} TCodeRep;

/* 
 * Structures demande de partie
 */ 
typedef enum { NORD, SUD } TSensTetePiece;

typedef struct {
  TIdReq idReq;               /* Identificateur de la requete */
  char nomJoueur[T_NOM];      /* Nom du joueur */
  TSensTetePiece piece;       /* Sens de la tete souhaite pour la piece */
} TPartieReq;

typedef enum { OK, KO } TValidSensTete;
typedef struct {
  TCodeRep err;                   /* Code d'erreur */
  char nomAdvers[T_NOM];          /* Nom du joueur adverse */
  TValidSensTete validSensTete;   /* Validation sens de la tete pour la piece */
} TPartieRep;


/* 
 * Definition d'une position de case
 */
typedef enum { UN, DEUX, TROIS, QUATRE, CINQ, SIX } TLg;
typedef enum { A, B, C, D, E } TCol;

typedef struct {
  TCol c;          /* Colonne de la position d'une piece */
  TLg l;           /* Ligne de la position d'une piece */
} TCase;

/* 
 * Definition de structure pour le deplacement de piece
 */
typedef struct {
  TCase  caseDep;   /* Position de depart de la piece */
  TCase  caseArr;   /* Position d'arrivee de la piece */
  bool estCapt;     /* Vrai si le deplacement capture une piece de l'adversaire */
} TDeplPiece;

/* 
 * Definition de structure pour le placement de piece capturee
 */
typedef TCase TDeposerPiece;

/* 
 * Structures coup du joueur 
 */

/* Precision des types de coups */
typedef enum { DEPLACER, DEPOSER,  AUCUN } TCoup;

/* Informations sur la piece a jouer */
typedef enum { KODAMA, KODAMA_SAMOURAI, KIRIN, KOROPOKKURU, ONI, SUPER_ONI } TTypePiece;

typedef struct {
  TSensTetePiece sensTetePiece;     /* Sens de la tete pour la piece */
  TTypePiece typePiece;             /* Type de la piece jouee */
} TPiece;

typedef struct {
  TIdReq     idRequest;     /* Identificateur de la requete */
  int        numPartie;     /* Numero de la partie (commencant a 1) */
  TCoup      typeCoup;      /* Type du coup : deplacement, placement ou aucune action */
  TPiece     piece;         /* Info de la piece jouee */
  union {
    TDeplPiece deplPiece;        /* Deplacement de piece */
    TDeposerPiece deposerPiece;  /* Placement d'une piece capturee */
  } params;
} TCoupReq;

/* Validite du coup */
typedef enum { VALID, TIMEOUT, TRICHE } TValCoup;

/* Propriete des coups */
typedef enum { CONT, GAGNE, NUL, PERDU } TPropCoup;

/* Reponse a un coup */
typedef struct {
  TCodeRep  err;            /* Code d'erreur */
  TValCoup  validCoup;      /* Validite du coup */
  TPropCoup propCoup;       /* Propriete du coup */
} TCoupRep;

//provisoire 

typedef struct
{
  int idReq;
  int numPartie;
  int typeCoup;
  int sensPiece;
  int typePiece;
  int colonneDep;
  int ligneDep;
  int colonneArr;
  int ligneArr;
  int capture;
  
}Coup;

typedef struct 
{
  int originX;
  int originY;
  int piece;
  int destX;
  int destY;
  int capture;
  
}CoupAdvJava;

#endif


