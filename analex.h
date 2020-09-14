/*********** Librerias utilizadas **************/
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>

/************* Definiciones ********************/

//Codigos
#define STRING		256
#define NUMBER		257
#define L_CORCHETE	258
#define R_CORCHETE	259
#define L_LLAVE		260
#define R_LLAVE		261
#define COMA		262
#define DOS_PUNTOS	263
#define PR_TRUE		264
#define PR_FALSE	265
#define PR_NULL		266
// Fin Codigos
#define TAMBUFF 	5
#define TAMLEX 		50

/************* Estructuras ********************/

typedef struct {
	int compLex;
	char *comp;
	char *lexema;
} token;

/************* Prototipos ********************/
void sigLex();
