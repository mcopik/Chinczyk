/*!
 * @mainpage main
 * Gra Chińczyk.@n
 * Liczba graczy: 4-6.@n
 * Poziomy trudności: Łatwy, Średni, Trudny.@n
 * Rozdzielczość: 640x480,800x600,1024x768.@n
 * Gra w pełnym ekranie: Tak.@n
 * Uruchomienie gry z argumentem '--default' powoduje ominięcie etapu wczytywania opcji gry z pliku.@n
 * @par Chińczyk
 * @author Marcin Copik
 * @date 2011.09.10
 * @version 1.0
*/
#ifdef _WIN32
# include <GL/freeglut.h>
#else
# include <GL/glut.h>
#endif
#include <string.h>
#include "game.h"
#include "input.h"
#include "graphic.h"
/*!
*	Główna funkcja.@n
*	Konfiguruje funkcje używane przez GLUT.@n
*/
int main(int argc, char** argv) 
{
	int i;
	int Default_Flag = 0;
	for(i=0;i<argc;i++)
	{
		if(!strcmp(DEFAULT_ARGUMENT,argv[i]))
			Default_Flag = 1;
	}
    glutInit(&argc, argv);
	Init_Process(Default_Flag);
	//funkcja rysoująca
    glutDisplayFunc(&Draw_Render);
	//funkcja pętli gry
    glutIdleFunc(&Process);
	//funkcja zmiany rozmiaru okna
    glutReshapeFunc(&Reshape_Window);
	//funkcja obsługi zdarzeń klawiatury
    glutKeyboardFunc(&Key_Pressed);
	//funkcja obsługi specjalnych klawiszy
    glutSpecialFunc(&Special_Key_Pressed);
	//funkcja obsługi zdarzeń myszy
	glutMouseFunc(&Mouse_Event);
	//funkcja zamknięcia gry
	glutCloseFunc(&Close_Game);
	//uruchomienie pętli
    glutMainLoop();
    return 0;
}