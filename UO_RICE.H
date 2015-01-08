#ifndef ___________
#define ___________

#include "StdAfx.h"
#include "windows.h"
	
HANDLE hco;
CONSOLE_SCREEN_BUFFER_INFO csbi;
   
void clearscreen( void )
{

	unsigned long int y;

	#ifndef __unix__
	COORD xy;

	xy.X=0;
	xy.Y=0;
	FillConsoleOutputAttribute(hco, ( /*BACKGROUND_BLUE |*/ BACKGROUND_GREEN /*BACKGROUND_RED*/ | BACKGROUND_INTENSITY), csbi.dwSize.X*csbi.dwSize.Y, xy, &y);
	FillConsoleOutputCharacter(hco, ' ', csbi.dwSize.X*csbi.dwSize.Y, xy, &y);
	SetConsoleCursorPosition(hco, xy);
	#endif
}

void constart( void )
{

  hco=GetStdHandle(STD_OUTPUT_HANDLE);
  GetConsoleScreenBufferInfo(hco, &csbi);
  SetConsoleTextAttribute(hco, ( BACKGROUND_BLUE /*| BACKGROUND_GREEN |*/ | BACKGROUND_RED | BACKGROUND_INTENSITY) );
  SetConsoleTitle("Tryptamine power!");
  clearscreen();

}

#endif