/*
 *  Analizador Léxico	
 *  Curso: Compiladores y Lenguajes de Bajo de Nivel
 *  Práctica de Programación Nro. 1
 *	
 *  Descripcion:
 *  Implementa un analizador léxico que reconoce números, literales 
 *  en cadena y signos de puntuación. para un lenguaje con sintaxis 
 *  tipo JSON.
 *	
 */

/*********** Inclusión de cabecera **************/
#include "analex.h"


/************* Variables globales **************/

token t;				// token global para recibir componentes del Analizador Lexico

// variables para el analizador lexico

FILE *archivo;			// Fuente json
char buff[2*TAMBUFF];	// Buffer para lectura de archivo fuente
char id[TAMLEX];		// Utilizado por el analizador lexico
int numLinea=1;			// Numero de Linea
int numAux=1; 			// Numero de Linea auxiliar utilizada para impresion

/**************** Funciones **********************/


// Rutinas del analizador lexico

void error(const char* mensaje)
{
    printf("\n\nLin %d: Error Lexico. %s.\n",numLinea,mensaje);	
}


void nextLex()
{
    int i=0;
    char c=0;
    int acepto=0;
    int estado=0;
    char msg[41];

    while((c=fgetc(archivo))!=EOF)
    {
        if (c==' ' || c=='\t'){

            continue;	//eliminar espacios en blanco
        }
        else if(c=='\n')
        {
            //incrementar el numero de linea
            numLinea++;
            continue;
        }
        else if (c==':')
        {
            t.compLex=DOS_PUNTOS;
            t.comp="DOS_PUNTOS";
			t.lexema=":";
            break;
        }
        else if (c=='{')
        {
            t.compLex=L_LLAVE;
            t.comp="L_LLAVE";
			t.lexema="{";
            break;
        }
        else if (c=='}')
        {
            t.compLex=R_LLAVE;
            t.comp="R_LLAVE";
			t.lexema="}";
            break;
        }
        else if (c=='[')
        {
            t.compLex=L_CORCHETE;
            t.comp="L_CORCHETE";
			t.lexema="[";
            break;
        }
        else if (c==']')
        {
            t.compLex=R_CORCHETE;
            t.comp="R_CORCHETE";
			t.lexema="]";
            break;
        }
        else if (c==',')
        {
            t.compLex=COMA;
            t.comp="COMA";
			t.lexema=",";
            break;
        }
        else if (c == '"')
        {
            //es un STRING /*VERIFICA QUE SE INGRESA UN STRING VALIDO*/
            //fgetc lee del archivo caracter por caracter a partir del puntero indicado en el archivo
            c=fgetc(archivo);
			i = 0;
			id[i]= c; 
            while(c!=EOF){
                if(c == '"'){
					id[i] = '\0';
                    t.compLex=STRING;
					t.comp="STRING";
					t.lexema = id;
                    break;
                }else if(i>=TAMLEX){
					ungetc(c, archivo);
				}
				else{
                    c=fgetc(archivo);
					id[++i] = c;
                }
            }
            if (c==EOF)
                error("Se llego al fin sin cerrar el String");
			else if(i>=TAMLEX){
				error("Longitud de Identificador excede tamaño de buffer");
			}
            break;
        }
        else if (isdigit(c))
        {
            //es un numero

            i=0;
            estado=0;
            acepto=0;
            id[i]=c;

            while(!acepto)
            {
                switch(estado){
                    case 0: //una secuencia netamente de digitos, puede ocurrir . o e
                        c=fgetc(archivo);
                        if (isdigit(c))
                        {
                            id[++i]=c;
                            estado=0;
                        }
                        else if(c=='.'){
                            id[++i]=c;
                            estado=1;
                        }
                        else if(tolower(c)=='e'){
                            id[++i]=c;
                            estado=3;
                        }
                        else{
                            estado=6;
                        }
                        break;

                    case 1://un punto, debe seguir un digito (caso especial de array, puede venir otro punto)
                        c=fgetc(archivo);						
                        if (isdigit(c))
                        {
                            id[++i]=c;
                            estado=2;
                        }
                        else if(c=='.')
                        {
                            i--;
                            fseek(archivo,-1,SEEK_CUR);
                            estado=6;
                        }
                        else{
                            sprintf(msg,"No se esperaba '%c'",c);
                            estado=-1;
                        }
                        break;
                    case 2://la fraccion decimal, pueden seguir los digitos o e
                        c=fgetc(archivo);
                        if (isdigit(c))
                        {
                            id[++i]=c;
                            estado=2;
                        }
                        else if(tolower(c)=='e')
                        {
                            id[++i]=c;
                            estado=3;
                        }
                        else
                            estado=6;
                        break;
                    case 3://una e, puede seguir +, - o una secuencia de digitos
                        c=fgetc(archivo);
                        if (c=='+' || c=='-')
                        {
                            id[++i]=c;
                            estado=4;
                        }
                        else if(isdigit(c))
                        {
                            id[++i]=c;
                            estado=5;
                        }
                        else{
                            sprintf(msg,"No se esperaba '%c'",c);
                            estado=-1;
                        }
                        break;
                    case 4://necesariamente debe venir por lo menos un digito
                        c=fgetc(archivo);
                        if (isdigit(c))
                        {
                            id[++i]=c;
                            estado=5;
                        }
                        else{
                            sprintf(msg,"No se esperaba '%c'",c);
                            estado=-1;
                        }
                        break;
                    case 5://una secuencia de digitos correspondiente al exponente
                        c=fgetc(archivo);
                        if (isdigit(c))
                        {
                            id[++i]=c;
                            estado=5;
                        }
                        else{
                            estado=6;
                        }break;
                    case 6://estado de aceptacion, devolver el caracter correspondiente a otro componente lexico
                        if (c!=EOF)
                            ungetc(c,archivo);
                        else
                            c=0;
							id[++i]='\0';
							acepto=1;
							t.compLex=NUMBER;
							t.comp="NUMBER";
							t.lexema = id;
                        break;
                    case -1:
                        if (c==EOF)
                            error("No se esperaba el fin de archivo");
                        else
                            error(msg);
                    }
                }
            break;
        }
        else if (isalpha(c)) {
            i = 0;
            do{
                id[i] = c;
                i++;
                c = fgetc(archivo);
            }while (isalpha(c));
            id[i]='\0';
            if (c!=EOF)
                ungetc(c,archivo);
            if(strcmp(id, "true") == 0 || strcmp(id, "TRUE") == 0){
                t.compLex=PR_TRUE;
                t.comp="PR_TRUE";
				t.lexema = "true";
                break;
            }
            else if (strcmp(id, "false") == 0 || strcmp(id, "FALSE") == 0){
                t.compLex=PR_FALSE;
                t.comp="PR_FALSE";
				t.lexema = "false";
                break;
            }
            else if(strcmp(id, "null") == 0 || strcmp(id, "NULL") == 0){
                t.compLex=PR_NULL;
                t.comp="PR_NULL";
				t.lexema = "null";
                break;
            }
            else{
                sprintf(msg,"No se esperaba '%s'",id);
                error(msg);				
            }
        }
        else {
            sprintf(msg,"No se esperaba '%c'",c);
            error(msg);
        }
    }
    if (c==EOF)
    {
        t.compLex=EOF;
        t.comp="EOF";

    }
}

int main(int argc,char* args[])
{
    if(argc > 1)
    {
        if (!(archivo=fopen(args[1],"rt")))
        {
            printf("No se encontro el archivo.\n");
            exit(1);
        }
		int tabulaciones = 0;
		nextLex();
		int anterior = -1;
		while (t.compLex!=EOF){
			
			if(t.comp){
				if(t.compLex == L_LLAVE){
					if(anterior != DOS_PUNTOS){
						for(int i = 0; i < tabulaciones; i++){ printf("\t"); }						
					}
					printf("%s\n", t.comp); anterior = t.compLex; tabulaciones++;
				}
				else if(t.compLex == R_LLAVE){
					tabulaciones--;
					printf("\n");
					for(int i = 0; i < tabulaciones; i++){
						printf("\t");
					}
					printf("%s", t.comp);
					anterior = t.compLex;
				}
				else if(t.compLex == COMA){
					printf(" %s\n", t.comp);
					anterior = t.compLex;
				}
				else if(t.compLex == DOS_PUNTOS){
					printf(" %s ", t.comp);
					anterior = DOS_PUNTOS;
				}
				else if(t.compLex == STRING) {
					if(anterior == DOS_PUNTOS){
						printf("%s", t.comp);	
					}
					else if(anterior == COMA){
						for(int i = 0; i < tabulaciones; i++){ printf("\t"); }
						printf("%s", t.comp);
					}
					else if(anterior == R_LLAVE) {
						printf("\n");
						for(int i = 0; i < tabulaciones; i++){
							printf("\t");
						}
						printf("%s", t.comp);
					}
					else if(anterior == L_LLAVE){
						for(int i = 0; i < tabulaciones; i++){
							printf("\t");
						}
						printf("%s", t.comp);						
					}
					else if(anterior == R_CORCHETE) {
						printf("\n");
						for(int i = 0; i < tabulaciones; i++){
							printf("\t");
						}
						printf("%s", t.comp);
					}
					else if(anterior == R_CORCHETE){
						for(int i = 0; i < tabulaciones; i++){
							printf("\t");
						}
						printf("%s", t.comp);						
					}
					anterior = STRING;
				}else if(t.compLex == L_CORCHETE){
					tabulaciones++;
					printf("%s\n", t.comp);
					anterior = t.compLex;			
				}else if(t.compLex == R_CORCHETE){
					tabulaciones--;
					printf("\n");
					for(int i = 0; i < tabulaciones; i++){
						printf("\t");
					}
					printf("%s", t.comp);
					anterior = t.compLex;
				}
			}
			nextLex();
		}
		fclose(archivo);
    }else{
        printf("\nSe debe especificar el archivo fuente.");
        exit(1);
    }

    printf("\n");

    return 0;
}