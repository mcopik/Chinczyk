#include <stdlib.h>
#include <stdio.h>
#include <GL/glut.h>
#include "input.h"
#include "graphic.h"
#include "fifo.h"


int Find_Hit(int _value,int _hits,unsigned int * _buffer)
{
	int i, j;	
	int Number;
	//forma odczytu wynika z formatu zapisu danych przez mechanizm 'picking' OpenGL
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
/*!
 * Wewnętrzna funkcja bufora zdarzeń klawiatury.@n
 * @param[in] _type Flaga określają działanie funkcji
 * @param[out] _pointer Wskaźnik pod którym zostanie zapisane zdarzenie klawiaturys
 */
void _Key_Pressed(int _type, Key_Action ** _pointer)
{
	//tablica przechowująca wskaźniki do zapisanych zdarzeń
	static Key_Action * Events[EVENTS_BUFFER];
	static int Size = 0;
	//kolejka zawierająca kolejne położenia zdarzeń w tablicy(chronologia analizy)
	static FIFO * Queue = NULL;//= FIFO_Create();
	int i;
	
	if(!Queue)
	{
		for(i = 0;i < EVENTS_BUFFER;i++)
			Events[i] = NULL;
		Queue = FIFO_Create();
	}
	//dodanie zdarzenia, nie nastąpi przepełnienie bufora
	if(_type == EVENT_SET && Size < EVENTS_BUFFER)
	{
		for(i = 0;i < EVENTS_BUFFER;i++)
				if(Events[i] == NULL)
				{
					Events[i] = *_pointer;
					FIFO_Push(Queue,i);
					break;
				}
		Size++;
		Event_Set(EVENT_KEY);
	}
	//odczyt zdarzenia
	else if(_type == EVENT_GET)
	{
		if(Size > 0)
		{
			i = FIFO_Pop(Queue);
			*_pointer = Events[i];
			Events[i] = NULL;
			Size--;
		}
		else
			*_pointer = NULL;
	}
	else if(_type == -1)
		FIFO_Delete(Queue);
}

void Key_Pressed(unsigned char _key,int _x,int _y)
{
	Key_Action * Action;

	Action = malloc(sizeof(*Action));
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
	_Key_Pressed(-1,NULL);
}

void Special_Key_Pressed(int _key,int _x,int _y){
	//klawisze odpowiedzialne za pracę kamery
    switch(_key){
        case GLUT_KEY_LEFT:
			Change_Camera(CAMERA_LEFT);
        break;
        case GLUT_KEY_RIGHT:
			Change_Camera(CAMERA_RIGHT);
        break;
        case GLUT_KEY_UP:
			Change_Camera(CAMERA_UP);
        break;
        case GLUT_KEY_DOWN:
			Change_Camera(CAMERA_DOWN);
        break;      
        case GLUT_KEY_PAGE_UP:
			Change_Camera(CAMERA_CLOSER);
        break;
        case GLUT_KEY_PAGE_DOWN:
			Change_Camera(CAMERA_FARTHER);
        break;
    }
}
/*!
 * Wewnętrzna funkcja bufora zdarzeń myszy.@n
 * @param[in] _type Flaga określają działanie funkcji
 * @param[out] _pointer Wskaźnik pod którym zostanie zapisane zdarzenie myszy
 */
void _Mouse_Event(int _type, Mouse_Action ** _pointer)
{
	//tablica przechowująca wskaźniki do zapisanych zdarzeń
	static Mouse_Action * Events[EVENTS_BUFFER];
	static int Size = 0;
	//kolejka zawierająca kolejne położenia zdarzeń w tablicy(chronologia analizy)
	static FIFO * Queue = NULL;
	int i;
	
	if(!Queue)
	{
		Queue = FIFO_Create();
		for(i = 0;i < EVENTS_BUFFER;i++)
			Events[i] = NULL;
	}
	if(_type == EVENT_SET && Size < EVENTS_BUFFER)
	{
		unsigned int Buffer[BUFFER_SIZE];
		int Hits;
		int Viewport[4];
		int i;
		//OpenGL picking
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
		//rendering w trybie analizy kliknięcia
		Draw_Select();
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPickMatrix((GLdouble)(*_pointer)->X, (GLdouble)(Viewport[3]-(*_pointer)->Y),PICK_SIZE, PICK_SIZE, Viewport);
		//rendering tekstów w trybie analizy kliknięcia
		Draw_Text(GL_SELECT);
		glPopMatrix();
		Hits = glRenderMode(GL_RENDER);
		if(Hits > 0)
		{
			for(i=0;i < BUFFER_SIZE;i++)
				(*_pointer)->Buffer[i] = Buffer[i];
			(*_pointer)->Hits = Hits;
			for(i = 0;i < EVENTS_BUFFER;i++)
				if(Events[i] == NULL)
				{
					Events[i] = *_pointer;
					FIFO_Push(Queue,i);
					break;
				}
			Size++;
			Event_Set(EVENT_MOUSE);
		}
		else
			free(*_pointer);
	}
	//pobranie zdarzenia myszy
	else if(_type == EVENT_GET)
	{
		if(Size > 0)
		{
			i = FIFO_Pop(Queue);
			*_pointer = Events[i];
			Events[i] = NULL;
			Size--;
		}
		else
			*_pointer = NULL;
	}
	else if(_type == -1)
		FIFO_Delete(Queue);
}

void Mouse_Event(int _button,int _state,int _x,int _y)
{
	//pobranie zdarzenia tylko wtedy, gdy zostanie puszczony przycisk myszy
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
	_Mouse_Event(-1,NULL);
}
/*!
 * Wewnętrzna funkcja bufora zdarzeń.@n
 * @param[in] _type Flaga określają działanie funkcji
 * @return Wartość na dnie bufora, gdy wywołano funkcję z parametrem EVENT_GET; w przeciwnym wypadku -1
 */
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
	return -1;
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
