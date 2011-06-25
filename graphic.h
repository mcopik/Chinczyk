/* 
 * File:   graphic.h
 * Author: mcopik
 *
 * Created on February 7, 2011, 10:14 PM
 */

#ifndef GRAPHIC_H
#define	GRAPHIC_H

#include <GL/glut.h>
#include <stdint.h>
#include "game.h"
/** Constants */
#define FPS 25
#define CAMERA_SPEED 0.25f
#define MIN_DISTANCE 10.0f
#define MAX_DISTANCE 50.0f
#define W 7.0f
#define H 0.5f
#define D 7.0f
#define PAWN_RADIUS	0.21f
#define PAWN_HEIGHT	0.3f
#define FIELDS_NUMBER_4_PLAYERS 64
#define PI 3.1415926f
#define NUMBER_OF_TEXTURES 1
#define CUBE_X	1.2f
#define CUBE_Y	-0.8f
#define CUBE_Z	-3.0f
#define CUBE_SIZE 0.4f

#define CUBE_HIT 99
#define BOARD_HIT 0

#define MIN(a,b) a < b ? a : b
#define FREQUENCY 5
#define TEXT_BUFFER_SIZE 10

#ifdef	__cplusplus
extern "C" {
#endif
#define ROTATE \
if(_axis)	\
{	\
	if(_axis == 1)	\
		glRotatef(_angle,1.0f,0.0f,0.0f);	\
	else if(_axis == 2)	\
		glRotatef(_angle,0.0f,1.0f,0.0f);	\
	else \
		glRotatef(_angle,0.0f,0.0f,1.0f);	\
}	\
    
enum{
    LEFT,
    RIGHT,
    UP,
    DOWN,
    FARTHER,
    CLOSER
};
enum{
	FULL_NOT,
	FULLSCREEN
};
enum{
	SET,
	GET,
	CHANGE
};
enum{
	TEXT_INIT,
	TEXT_ADD,
	TEXT_REMOVE,
	TEXT_CLEAN
};

typedef struct {
	unsigned int Width, Height;
	char * Data;
} Image;

typedef struct{
		float Position[2];
		float Radius;
} Field;

typedef struct{
		int Player_Number;
		int Field_Number;
		int * Pawns_Numbers;
} Blink_Info;

typedef struct{
	char * String;
	int X,Y,Select_Number;
	void * Font;
}Text;
/**
 * 0 - Rotation X axis
 * 1 - Rotation Y axis
 * 2 - Distance on Z axis
 */
void Change_Camera(int _change);
void Set_Camera(float * _camera);
float * Get_Camera();
int Load_Image(Image * _image,const char * _path);
void Close_Image(Image * _image);
void Interval(float * number, int min,int max);
int Init_GL(int _width, int _height, char * _label,int _fullscr);
void Draw_Render();
void Draw_Select();
void Draw_Init(Field * fields,Player * players,int players_number,int * _randomized);
void Blink_Set_Pawn(int _number,int * _pawns);
void Blink_Set_Field(int _number,int _pawn,int _field);
void Disable_Blink();
void Enable_FullScr(int _width,int _height);
void Disable_FullScr(int _width,int _height);
void Reshape_Window(int Width, int Height);
void Draw_Circle(float radius, float x, float y,float _z);
void Draw_Pawn(float x,float z,float * colors,int _blink);
void Text_Init(int _width,int _height);
void Text_Add(Text * _text,const char * _name);
void Text_Remove(const char * _name);
void Text_Clean();
Text * Text_Create(int _x,int _y,int _select,const char * _string,void * _font);
void Draw_Fields(int _type, Field * array,int players_number,int _blink);
Field * Generate(int Players_Number);
Field * Generate_4_Players();
Field * Generate_5_Players();
Field * Generate_6_Players();
void Close(Field * Pointer);
void Text_Create_Player(const char * _name);
void Text_Create_Draw(int _size,int * _tab);
void Text_Create_FPS(int _fps);
void Draw_Cube_Pips(float _radius, int _number);
#ifdef	__cplusplus
}
#endif

#endif	/* GRAPHIC_H */
