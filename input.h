/* 
 * File:   input.h
 * Author: mcopik
 *
 * Created on February 7, 2011, 11:02 PM
 */

#ifndef INPUT_H
#define	INPUT_H

#ifdef	__cplusplus
extern "C" {
#endif

#define BUFFER_SIZE 20
#define EVENTS_BUFFER 5
#define PICK_SIZE 5.0f
#define KEY_ESCAPE 27

enum{
	EVENT_KEY = 0,
	EVENT_MOUSE,
	EVENT_GET,
	EVENT_CLEAN
};

typedef struct{
	unsigned char Key;
	int X;
	int Y;
} Key_Action;

typedef struct{
	int Button;
	int X;
	int Y;
	unsigned int Buffer[BUFFER_SIZE];
	int Hits;
} Mouse_Action;
void Special_Key_Pressed(int key,int x,int y);
void Mouse_Event(int _button,int _state,int _x,int _y);
void Check_Mouse_Event(Mouse_Action ** _pointer);
void Check_Key_Event(Key_Action ** _pointer);
void Key_Pressed(unsigned char _key,int _x,int _y);
int Event_Get();
void Event_Set(int _type);
void Event_Clean();
int Find_Hit(int _value,int _hits,unsigned int * _buffer);
void Clean_Key_Event();
void Clean_Mouse_Event();
#ifdef	__cplusplus
}
#endif

#endif	/* INPUT_H */

