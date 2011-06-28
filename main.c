/**
 * @mainpage
 * @par Chińczyk
 * @author Marcin Copik
 * @date 2011.05.09
 * @version 0.1
*/
#include <GL/glut.h>
#include <string.h>
#include "game.h"
#include "input.h"
#include "graphic.h"

/**
 * Main function
 * Initialization of all OpenGL's systems and loading data
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
    glutDisplayFunc(&Draw_Render);
    glutIdleFunc(&Process);
    glutReshapeFunc(&Reshape_Window);
    glutKeyboardFunc(&Key_Pressed);
    glutSpecialFunc(&Special_Key_Pressed);
	glutMouseFunc(&Mouse_Event);
	glutCloseFunc(&Close_Game);
    glutMainLoop();
    return (EXIT_SUCCESS);
}

