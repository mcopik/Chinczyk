#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include "input.h"
#include "graphic.h"
#include "fifo.h"




int Find_Hit(int _value,int _hits,unsigned int * _buffer)
{
	int i, j;
	printf("%d hits:\n", _hits);
 
 	for (i = 0; i < _hits; i++)
 		printf(	"Number: %d\n"
		
 				"Min Z: %d\n"
 				"Max Z: %d\n"
 				"Name on stack: %d\n",
 				(GLubyte)_buffer[i * 4],
 				(GLubyte)_buffer[i * 4 + 1],
 				(GLubyte)_buffer[i * 4 + 2],
 				(GLubyte)_buffer[i * 4 + 3]
 				);
 
 	printf("\n");
	int Number;
	for (i = 0; i < _hits; i++){
		Number = *_buffer;
		_buffer += 3;
		for (j = 0; j < Number; j++){
			if(*_buffer == _value)
				return 1;
			_buffer++;
		}
	}
	return 0;
}

void _Key_Pressed(int _type, Key_Action ** _pointer)
{
	static Key_Action * Events[EVENTS_BUFFER];
	static int Position = -1;
	
	if(_type == EVENT_SET && Position < EVENTS_BUFFER)
	{
		Position++;
		Events[Position] = *_pointer;
		Event_Set(EVENT_KEY);
	}
	else if(_type == EVENT_GET)
	{
		if(Position >= 0)
		{
			*_pointer = Events[Position];
			Position--;
		}
		else
			*_pointer = NULL;
	}		
}

void Key_Pressed(unsigned char _key,int _x,int _y)
{
	Key_Action * Action = malloc(sizeof(*Action));
	Action->Key = _key;
	Action->X = _x;
	Action->Y = _y;
	_Key_Pressed(EVENT_SET,&Action);
}

void Check_Key_Event(Key_Action ** _pointer)
{
	_Key_Pressed(EVENT_GET,_pointer);
}

void Clean_Key_Event()
{
	Key_Action * pointer;
	_Key_Pressed(EVENT_GET,&pointer);
	while(pointer){
		free(pointer);
		_Key_Pressed(EVENT_GET,&pointer);
	}
}

void Special_Key_Pressed(int key,int x,int y){

    switch(key){
        case GLUT_KEY_LEFT:
			Change_Camera(LEFT);
        break;
        case GLUT_KEY_RIGHT:
			Change_Camera(RIGHT);
        break;
        case GLUT_KEY_UP:
			Change_Camera(UP);
        break;
        case GLUT_KEY_DOWN:
			Change_Camera(DOWN);
        break;      
        case GLUT_KEY_PAGE_UP:
			Change_Camera(CLOSER);
        break;
        case GLUT_KEY_PAGE_DOWN:
			Change_Camera(FARTHER);
        break;
    }
}



void _Mouse_Event(int _type, Mouse_Action ** _pointer)
{
	static Mouse_Action * Events[EVENTS_BUFFER];
	static int Position = -1;
	
	if(_type == EVENT_SET)
	{
		unsigned int Buffer[BUFFER_SIZE];
		int Hits;
		int Viewport[4];
		int i;
		glSelectBuffer(BUFFER_SIZE, Buffer);
		glRenderMode(GL_SELECT);
		glGetIntegerv(GL_VIEWPORT, Viewport);
		glInitNames();
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		gluPickMatrix((GLdouble)(*_pointer)->X, (GLdouble)(Viewport[3]-(*_pointer)->Y),PICK_SIZE, PICK_SIZE, Viewport);
		gluPerspective(45.0f,(GLdouble)Viewport[2]/(GLdouble)Viewport[3],0.1f,100.0f);
		glMatrixMode(GL_MODELVIEW);
		Draw_Select();
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPickMatrix((GLdouble)(*_pointer)->X, (GLdouble)(Viewport[3]-(*_pointer)->Y),PICK_SIZE, PICK_SIZE, Viewport);
		Draw_Text(GL_SELECT);
		glPopMatrix();
		Hits = glRenderMode(GL_RENDER);
		if(Hits > 0)
		{
			for(i=0;i < BUFFER_SIZE;i++)
				(*_pointer)->Buffer[i] = Buffer[i];
			(*_pointer)->Hits = Hits;
			if(Position < EVENTS_BUFFER)
			{
				Position++;
				Events[Position] = *_pointer;
				Event_Set(EVENT_MOUSE);
			}
		}
		else
			free(*_pointer);
	}
	else
	{
		if(Position >= 0)
		{
			*_pointer = Events[Position];
			Position--;
		}
		else
			*_pointer = NULL;
	}
}

void Mouse_Event(int _button,int _state,int _x,int _y)
{
	if(_state == GLUT_DOWN)
	{
		Mouse_Action * Action;
		Action = malloc(sizeof(*Action));
		Action->Button = _button;
		Action->X = _x;
		Action->Y = _y;
		_Mouse_Event(EVENT_SET,&Action);
	}
}

void Check_Mouse_Event(Mouse_Action ** _pointer)
{
	_Mouse_Event(EVENT_GET,_pointer);
}

void Clean_Mouse_Event()
{
	Mouse_Action * pointer;
	_Mouse_Event(EVENT_GET,&pointer);
	while(pointer){
		free(pointer);
		_Mouse_Event(EVENT_GET,&pointer);
	}
}
int _Event(int _type)
{
	static FIFO * Buffer = NULL;
	if(!Buffer)
	{
		Buffer = FIFO_Create();
	}
	if(_type == EVENT_GET)
	{
		return FIFO_Pop(Buffer);
	}
	else if(_type == EVENT_CLEAN)
	{
		FIFO_Delete(Buffer);
	}
	else
		FIFO_Push(Buffer,_type);
	return;
}
int Event_Get()
{
	return _Event(EVENT_GET);
}

void Event_Set(int _type)
{
	_Event(_type);
}

void Event_Clean()
{
	_Event(EVENT_CLEAN);
}
