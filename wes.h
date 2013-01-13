/*
 * Copyright © 2005 Alejandro Ricoveri <alejandroricoveri@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */
 
/*	WORDSORTER: ANALIZADOR LEXICO V1.0 (CÓDIGO FUENTE)
	REALIZADO Y CODIFICADO POR ALEJANDRO RICÓVERI. C.I. : 16149964
	UNIVERSIDAD RAFAEL BELLOSO CHACÍN
	FACULTAD DE INGENIERÍA 
	ESCUELA DE COMPUTACIÓN
	CÁTEDRA: ESTRUCTURA DE LENGUAJES Y COMPILADORES
	SECCIÓN: C611
*/

/*	ARCHIVO DE DECLARACIONES DE TODOS LOS MODULOS, ESTRUCTURAS
	Y VARIABLES GLOBALES DEL PROGRAMA*/

// LIBRERIAS USADAS EN EL PROGRAMA
#include <string.h>
#include <time.h>


/*	F: FILE
	P: PARAMETRO
	N: NO
	E: EXISTE
	S: ESPECIFICADO
*/

//	DEFINICION DE CONSTANTES USADAS
#define WARNING_STRING_PNE			"El parametro no existe."
#define WARNING_STRING_NO_PARAM		"No hay parametros. Escriba -help para obtener ayuda."

#define WARNING_CODE_PNE			1000


#define ERROR_CODE_FNE				1000
#define ERROR_CODE_FPNE				1001
#define ERROR_CODE_FNS				1002
#define ERROR_CODE_PARAMERROR		1003
#define ERROR_CODE_FILEACCESS		1004


#define ERROR_STRING_FPNE			"Parametro -file no especificado."
#define ERROR_STRING_FNS			"Archivo no especificado (-file <nombreArchivo>)."
#define ERROR_STRING_PARAMERROR		"No hay suficientes parametros para ejecucion .\n\t\tEscriba -help para obtener ayuda."
#define ERROR_STRING_FILEACCESS		"Error de lectura de archivo. El archivo no es válido o no existe"

#define OUTPUTFILE_SEPARATORLINE	"---------------------------------------------------------------------------------------------------"
#define PARAM_NUM					5
 
#define	COLOR_FOREGROUND_RED		0xC0
#define	COLOR_RED					0xC
#define	COLOR_DEFAULT				0xF
#define COLOR_YELLOW				0xE

#define ACCESS_VIOLATION			0xC0000005

#define MAX_LINE_SIZE				1024
#define MAX_WORD_SIZE				128

const char PARAM_LIST[PARAM_NUM][16] = {"-file",
										"-dev",
										"-verbose",
										"-chart",
										"-help"
										};

//	DEFINICION DE MODULOS DEL PROGRAMA
int countWords(char *);
int countLines(char *);

void wsExit();
void printHelp();
void wsSplash();
void wsPrintChart();

void wsError(char[], int);

void wsWarning(char[]);
void wsWarning(char[], int);
void wsWarning(char[], int, char *[]);

bool checkParameters(char *[], int);
void wsActivateParameter(char *);

void readFile(char *);


// DEFINICION DE VARIABLES GLOBALES
int wsCounter, palDesc;

char *FileString;

HANDLE  hConsole;

bool	wsParamFile, 
		wsParamDev, 
		wsParamVerbose, 
		wsParamChart, 
		wsParamHelp;

ifstream InputFile;
ofstream OutputFile;

//	DEFINICION DE ESTRUCTURAS Y CLASES 
struct Line{
	int lineNum;
	Line *nextLine;
};

struct Word{
	char wordString[MAX_WORD_SIZE];
	Word *nextWord;
	Line *firstLine;
};


class wordTable{

public:
	void insertWord(char *);
	void insertLine(char *, int);
	void printTable();
	void writeOutputFile();
	wordTable();
private:
	void wordSort();
	void lineSort();
	Word *firstWord;
	Word *tempWord;
	Line *tempLine;

}wsWordTable;
