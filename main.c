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
	
#include "wes.h" // STANDARD HEADER DEL PROGRAMA

/* MÓDULOS DE LA CLASE PRINCIPAL QUE USA EL PROGRAMA (wordTable)*/

/*	FUNCIÓN QUE REALIZA LA IMPRESION DE LA ESTRUCTURA DE DATOS 
	MANEJADO POR wordTable 
*/
void wordTable::printTable()
{
	tempWord = firstWord;

	printf("Tabla de palabras:\n\n");

	while(tempWord != NULL)
	{

		printf("* %s @:", tempWord->wordString);
		tempLine = tempWord->firstLine;

		while(tempLine != NULL) 
		{
			if(tempLine->nextLine == NULL)
			printf("%d", tempLine->lineNum);
			else
			printf("%d, ", tempLine->lineNum);	

			tempLine = tempLine->nextLine;
		}

		tempWord = tempWord->nextWord;
		printf("\n");
	}
}

/*	CONSTRUCTOR DE wordTable */
wordTable::wordTable()
{

	firstWord = NULL;

}

/*	FUNCION QUE REALIZA LA INSERCION DE LEXEMAS O PALABRAS 
	EN LA ESTRUCTURA DE DATOS
*/
void wordTable::insertWord(char *inWord)
{
	
	
	tempWord = new Word;
	strncpy ( tempWord->wordString, inWord , MAX_WORD_SIZE ); 
	tempWord->firstLine = NULL;

			
	if(firstWord == NULL){
		
		tempWord->nextWord = firstWord;
		firstWord = tempWord;


		if(wsParamVerbose){
			printf("Ingresando en la tabla de palabras: %s\n", tempWord->wordString); 
			
		}

	}
	else {
	
		Word *cmpWord; 
		bool wordEquals;
		cmpWord = firstWord;	

		while (cmpWord != NULL){

		wordEquals = FALSE ;

			if(!strcmp(cmpWord->wordString, tempWord->wordString)){

				if(wsParamVerbose)
					printf(">Palabra '%s' coincide con otra. Palabra Descartada\n", tempWord->wordString);

				wordEquals = TRUE;
				palDesc++;
				break;
				
			}
			else cmpWord= cmpWord->nextWord;			
			
		}		
		

		if(!wordEquals){
			
			tempWord->nextWord = firstWord;
			firstWord = tempWord;
	
			if(wsParamVerbose)
			printf("Ingresado en la tabla de palabras: %s\n", tempWord->wordString); 
		
		}

	}
	
	wordSort();


}

/*	FUNCION QUE SE ENCARGA DE LA INSERCIÓN DE LINEAS DE TEXTO
	PARA SU POSTERIOR ANÁLISIS CON RESPECTO A LA ESTRUCTURA 
	DE DATOS DE wordTable
*/
void wordTable::insertLine(char *inLine, int numLine){

	if(wsParamVerbose)
		printf("Procesando linea ... \n");

	ofstream SwapWrite;
	SwapWrite.open("SWAP");

	if(wsParamVerbose)
		printf("Creando archivo de swap ...\n");

	SwapWrite << inLine <<endl;

	SwapWrite.close();
	
	ifstream SwapRead;
	SwapRead.open("SWAP");

	char tempLineWord[MAX_WORD_SIZE];

	Line *cmpLine;
	bool lineExists;

	while (!SwapRead.eof()){

		tempWord = firstWord;

		while(tempWord != NULL){

			if(!strcmp(tempWord->wordString,tempLineWord)){ 
			
				
				cmpLine = tempWord->firstLine;
				lineExists = FALSE;

				if(cmpLine != NULL){

					if(cmpLine->lineNum == numLine)
						lineExists = TRUE;
				}

				if(!lineExists){

					tempLine = new Line;
					tempLine->lineNum = numLine;

					tempLine->nextLine = tempWord->firstLine;
					tempWord->firstLine = tempLine;
				} 

			break;

			}

			else tempWord = tempWord->nextWord;

		}

		SwapRead >> tempLineWord;
		
	}
	
	lineSort();

}

/* FUNCION QUE SE ENCARGA DE ORDENAR LAS PALABRAS 
	DENTRO DE LA ESTRUCTURA DE DATOS 
*/
void wordTable::wordSort()
{

	char tmpString[MAX_WORD_SIZE];
	Word *alphaWord;

	tempWord = firstWord;
	alphaWord = firstWord->nextWord;

	if( firstWord != NULL){

		while (alphaWord != NULL){

			if( strcmp(tempWord->wordString, alphaWord->wordString) > 0){

				strncpy(tmpString,alphaWord->wordString,MAX_WORD_SIZE);
				strncpy(alphaWord->wordString,tempWord->wordString,MAX_WORD_SIZE);
				strncpy(tempWord->wordString,tmpString,MAX_WORD_SIZE);

				tempWord = tempWord->nextWord;
				alphaWord = alphaWord->nextWord;

			}
			else break;
				
		}
	}


}

/*	FUNCION QUE SE ENCARGA DE ORDENAR LOS NÚMEROS 
	DE LINEA POR CADA PALABRA QUE SE ENCUENTRE EN 
	LA ESTRUCTURA DE DATOS 
*/
void wordTable::lineSort(){
	
	tempWord = firstWord;
	tempLine = firstWord->firstLine;

	int tmpLineNum;

	Line *alphaLine;

	while( tempWord != NULL){

		tempLine = tempWord->firstLine;

		if(tempLine != NULL){
			
			alphaLine = tempLine->nextLine;
			
			while(alphaLine != NULL){

				if( tempLine->lineNum > alphaLine->lineNum){

					tmpLineNum = alphaLine->lineNum;
					alphaLine->lineNum = tempLine->lineNum;
					tempLine->lineNum = tmpLineNum;
					tempLine = tempLine->nextLine;
					alphaLine = alphaLine->nextLine;

				}

				else  break;
			}
		
		}

		tempWord  = tempWord->nextWord;
		
	}

}

/*	FUNCION QUE SE ENCARGA DE ESCRIBIR 
	A PARTIR DE LA ESTRUCTURA DE DATOS
	DE wordTable EL ARCHIVO DE SALIDA
	CON SUS RESPECTIVOS RESULTADOS 
*/
void wordTable::writeOutputFile(){

	OutputFile.open("output_ws.wsd");
	OutputFile.setf(ios::fixed);
	
	OutputFile <<	"WORDSORTER: ANALIZADOR LEXICO V1.0 (JUN 2005)\n"
					"Output File: output_ws.wsd\n"
					"Descripcion: archivo de salida de " << FileString << "\n" 
				<< endl;

	OutputFile <<	OUTPUTFILE_SEPARATORLINE	<<endl;

	tempWord = firstWord;
	
	while(tempWord != NULL){

		tempLine = tempWord->firstLine;

		OutputFile << "* " << tempWord->wordString << " @: ";

		while(tempLine != NULL){

			if(tempLine->nextLine != NULL)
				OutputFile << tempLine->lineNum << " ,";
			else
				OutputFile << tempLine->lineNum << endl;

			tempLine = tempLine->nextLine;
		}

		tempWord = tempWord->nextWord;
	}

	OutputFile << OUTPUTFILE_SEPARATORLINE	<<endl;

}

/*	MODULOS GLOBALES DEL PROGRAMA */

/*	FUNCION QUE SE ENCARGA DE IMPRIMIR EL SPLASH
	O BIEN LA PRESENTACION DEL PROGRAMA
*/
void wsSplash()	
{
		
	system( "cls" );

	SetConsoleTextAttribute(hConsole, COLOR_FOREGROUND_RED);
	printf( "WORDSORTER: ANALIZADOR LEXICO V1.0 (JUN 2005)");
	
	SetConsoleTextAttribute( hConsole, COLOR_DEFAULT );
	printf(	"\nDisenado y codificado por Alejandro Ricoveri (C.I. 16149964)"
			"\nUniversidad Rafael Belloso Chacin (URBE)"
			"\nFacultad de Ingenieria"
			"\nEscuela de Computacion"
			"\nCatedra: Estructura de Lenguajes y Compiladores\n\n"
			);	
}


/*	FUNCION QUE SE ENCARGA DE PROCESAR
	LOS PARAMETROS DE ENTRADA A PARTIR 
	DEL COMMAND LINE EN LA EJECUCIÓN
	DEL PROGRAMA
*/
bool checkParameters(	char *param[], 
						int paramCount	)	
{

	bool paramExists;

	for	( wsCounter = 1 ; wsCounter < paramCount ; wsCounter++ ){

		for ( int x = 0 ; x < PARAM_NUM ; x++ ) {
			
			if  ( strcmp( param[wsCounter], PARAM_LIST[x] ) ){

				if ( wsParamFile && (!FileString) ) {
					FileString = param[wsCounter];
					paramExists = TRUE;
					break;
				}

				else
					paramExists = FALSE ;

			}
			else{

					paramExists = TRUE;
					wsActivateParameter(param[wsCounter]);
					break;

			}
			
		}

		if( !paramExists )
			wsWarning(WARNING_STRING_PNE, WARNING_CODE_PNE, &param[wsCounter] );
	}
	

	if( wsParamHelp ){

		printHelp();
		return FALSE ;

	}
	else{

		if( !param[1] )
			wsWarning(WARNING_STRING_NO_PARAM);	

		if( !wsParamFile )
			wsError(ERROR_STRING_FPNE, ERROR_CODE_FPNE);

		if( !FileString )
			wsError(ERROR_STRING_FNS,ERROR_CODE_FNS);

		if( wsParamChart )
			wsPrintChart();
	}
	
	if (!wsParamFile || !FileString)
		return FALSE ;
	else 
		return TRUE ;

}

/*	FUNCION QUE A PARTIR DE checkParameters()
	ESTABLECE CUALES PARAMETROS DEBERAN SER 
	ACTIVADOS
*/
void wsActivateParameter(char *paramString)	
{

	if(!(strcmp( paramString, "-help")))
		wsParamHelp = TRUE;

	if(!(strcmp( paramString, "-dev")))
		wsParamDev = TRUE;
		
	if(!(strcmp( paramString, "-verbose")))
		wsParamVerbose = TRUE;

	if(!(strcmp( paramString, "-file")))
		wsParamFile = TRUE;

	if(!(strcmp( paramString, "-chart")))
		wsParamChart = TRUE;

}


/* FUNCION QUE IMPRIME LA AYUDA PARA EL USUARIO
	(SE ACTIVA CON EL PARAMETRO -help
	EN EL COMMAND LINE)
*/
void printHelp()	
{

	printf(	"Parametros disponibles para ejecucion:\n"
			"-file <nombreArchivo>\t:\tParametro de especificacion\n"
			"\t\t\t\tde Archivo de nombre <nombreArchivo>\n"
			" -dev\t\t\t:\tModo Desarrollador\n"
			"-verbose\t\t:\tModo Verbose (Detalle)\n"
			"-chart\t\t\t:\tImpresion de cartelera parametrica\n");
}


/*	FUNCION QUE IMPRIME EL CONJUNTO
	DE LOS PARÁMETROS ESPECIFICANDO
	POR CADA UNO SI FUE ACTIVADO O NO
	CONJUNTAMENTE CON LA IMPRESION DEL NOMBRE DEL ARCHIVO
	INGRESADO ( PUEDE SER ACTIVADO CON -chart EN EL COMMAND LINE
*/
void wsPrintChart()	
{
	
	printf(	"[BEGIN CHART]\n" 
			"-----\n"); 
	printf("Modo Desarrollador = %d\n",(int)wsParamDev);
	printf("Modo Verbose\t   = %d\n",(int)wsParamVerbose);

	if( FileString )
		printf("Archivo : \"%s\"\n", FileString); 
	else
		printf("Archivo : NO ESPECIFICADO\n"); 


	printf(	"-----\n"
			"[END CHART]\n"); 
	
			
}


/* FUNCION QUE SE ENCARGA DE IMPRIMIR LOS ERRORES
	DE EJECUCION EN EL PROGRAMA
*/
void wsError(	char errorString[], 
				int errorCode		)	
{

	SetConsoleTextAttribute( hConsole, COLOR_RED );
	printf( "ERROR %d: %s\n",errorCode,errorString );
	SetConsoleTextAttribute( hConsole, COLOR_DEFAULT );

}


/* FUNCION QUE SE ENCARGA DE IMPRIMIR LAS ADVERTENCIAS
	DURANTE LA EJECUCION DEL PROGRAMA
*/
void wsWarning (char errorString[])	
{
	
	SetConsoleTextAttribute( hConsole, COLOR_YELLOW );
	printf( "ADVERTENCIA: %s\n",errorString );
	SetConsoleTextAttribute( hConsole, COLOR_DEFAULT );
}



void wsWarning (	char errorString[], 
					int errorCode		)	
{
	
	SetConsoleTextAttribute( hConsole, COLOR_YELLOW );
	printf(	"ADVERTENCIA %d: %s\n",errorCode,errorString );
	SetConsoleTextAttribute( hConsole, COLOR_DEFAULT );

}



void wsWarning (	char errorString[], 
					int errorCode, 
					char *paramString[]	)	
{
	
	SetConsoleTextAttribute( hConsole, COLOR_YELLOW );
	printf( "ADVERTENCIA %d: %s (%s)\n",errorCode,errorString,*paramString );
	SetConsoleTextAttribute( hConsole, COLOR_DEFAULT );

}


/* FUNCION QUE SE ENCARGA DE LA SALIDA DEL PROGRAMA */
void wsExit()	
{

	if( wsParamDev )
		system( "PAUSE" );

	exit(0);

}

/*	FUNCION QUE SE ENCARGA DE LA LECTURA Y PROCESAMIENTO 
	DEL ARCHIVO DE ENTRADA EN EL PROGRAMA 
*/
void readFile(char *fileName)
{	
	
	char InputWord[MAX_WORD_SIZE];
	char InputLine[MAX_LINE_SIZE];

	int wordCount, lineNum = 0;
	int pCount = 1;
	
	wordCount = countWords ( fileName );
	InputFile.open(fileName);


	printf( "Archivo %s cargado exitosamente ! .\n"
			"Procesando archivo...\n", fileName		);
	
	printf("Procesando palabras...\n");
	


	InputFile >> InputWord;
	wsWordTable.insertWord(InputWord);
		
	while (!InputFile.eof()){
		
		pCount++;

		if(wsParamVerbose)
			printf( "Palabra %d: %s\n", pCount, InputWord);

		InputFile >> InputWord;
			
		wsWordTable.insertWord( InputWord );
		
		if(!wsParamDev && !wsParamVerbose )

			printf("%d/%d %.1f pps (%.1f %s)  \r", pCount, 
			wordCount, pCount/(float)(clock()/CLK_TCK),
			(float)pCount*100/wordCount, "%"
			);
		
	}
	
	printf("\n");

	InputFile.close();

	float tWord;
	tWord = (float) clock();

	int tLineCount;
	tLineCount = countLines(fileName);

	InputFile.open(fileName);
	
	if(wsParamVerbose)
		printf("Leyendo linea # %d ...\n", lineNum);

	InputFile.getline(InputLine, MAX_LINE_SIZE);

	if(wsParamVerbose)
		printf("%s\n",InputLine);


	
	wsWordTable.insertLine(InputLine, lineNum);
	
	printf("Procesando lineas...\n");

	while (!InputFile.eof()){

		lineNum++;

		if(wsParamVerbose)
			printf("Leyendo linea # %d ...\n", lineNum);

		InputFile.getline(InputLine, MAX_LINE_SIZE);

		if(wsParamVerbose)
			printf("%s\n",InputLine);

		wsWordTable.insertLine(InputLine, lineNum);

		if(!wsParamDev && !wsParamVerbose)

			printf("%d/%d %.1f lps (%.1f %s)  \r", lineNum+1 , 
			tLineCount, tLineCount/(float)((clock() - tWord)/CLK_TCK),
			(float)(lineNum+1)*100/tLineCount, "%"
			);
		
	}

	printf("\n");
	if(wsParamDev)
		wsWordTable.printTable();

	printf("\nArchivo %s leido.\n",fileName);
	printf("\n%d palabra(s) encontrada(s), %d palabra(s) descartada(s).\n",  wordCount, palDesc);
	printf("%d palabra(s) capturada(s).\n", wordCount - palDesc);
	printf("%d linea(s) procesada(s).\n", lineNum+1);
	wsWordTable.writeOutputFile();
	printf("Archivo de salida \"output_ws.wsd\" generado.\n");
	printf("%.4f segundo(s) transcurrido(s).\n", (float)clock()/CLK_TCK);	
		
	InputFile.close();
}


/*	FUNCION QUE DEVUELVE EL 
	NUMERO TOTAL DE LINEAS
	QUE HAY EN EL ARCHIVO 
*/
int countWords (char *cFileName){

	
	int wCount = 1;
	char temp[MAX_WORD_SIZE];

	InputFile.open( cFileName );
	InputFile >> temp;
	
	while (!InputFile.eof()){
		wCount++;
		InputFile >> temp ;
	}

	InputFile.close();

	return wCount;

	
}


/*	FUNCION QUE DEVUELVE EL 
	NUMERO TOTAL DE LINEAS
	QUE HAY EN EL ARCHIVO 
*/
int countLines (char *cFileName){
	
	char tmpLine[MAX_LINE_SIZE];
	int lCount = 1;

	InputFile.open(cFileName);
	InputFile.getline(tmpLine, MAX_LINE_SIZE);
	
	while (!InputFile.eof()){

		lCount++;
		InputFile.getline(tmpLine, MAX_LINE_SIZE)	;
	}
	
	InputFile.close();
	return lCount;
}


/* FUNCION MAIN DEL PROGRAMA */
void main (	int argc, 
			char *argv[], 
			char **envp )
{
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	
	wsSplash();

	if( checkParameters(argv,argc) ){

		printf(	"[BEGIN WS]\n-----\n"); 
		readFile(FileString);
		printf(	"-----\n[END WS]\n");
		
	}
	else if(!wsParamHelp){
		wsError(ERROR_STRING_PARAMERROR, ERROR_CODE_PARAMERROR);
	}
	
	wsExit();
	
}

