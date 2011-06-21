 /*
 * File:   game.h
 * Author: mcopik
 *
 * Created on February 7, 2011, 7:44 PM
 */

#ifndef GAME_H
#define	GAME_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <GL/gl.h>
#include "array.h"
#include "input.h"

#define MAX_PLAYERS 3
#define DEFAULT_ARGUMENT "--default"
#define ERROR(ret,...)		\
{							\
	printf(__VA_ARGS__);	\
	return ret;				\
}				
#define WARNING(...)		\
	printf(__VA_ARGS__);		
#define NOT_IMPLEMENTED		\
{							\
	printf("Feature not implemented yet");\
	return;					\
}
#define PATH "options.txt"
#define NUMBER_OF_PAWNS 4
#define NUMBER_OF_FIELDS_PER_PLAYER 8
#define FONT1 GLUT_BITMAP_9_BY_15
#define DRAW_MSG "Draw_Msg"
#define NAME_MSG "Name_Msg"
#define FPS_MSG "FPS_Msg"
#define FPS_TEXT "FPS: "
#define DRAWING_TEXT "Wylosowano:"
	enum{
		CONFIG,
		START,
		MENU,
		INGAME_MENU,
		WAIT,
		RANDOMIZE,
		WAIT_DEC,
		WAIT_SELECT,
		END,
		NEXT_PLAYER,
		QUIT
	};
	enum{
		HUMAN,
		AI
	};
	/**
	 * Position gets a value calculated in two ways:
	 * i*8 + [0..7] - where 'i' is player's number(between 0-[3..5]) and second value depends on position in start and final point
	 * 8*[4..6] + [0..([4..6]*(8-([4..6]-6)))] - 1 - it looks quite complicated, but.. it's not. first part is 32, 40 or 48 fields for start and final positions for every player.
	 * second part is number of fields in game - 32, 35 or 36; number of fields for each player is decreasing from 8 to 6, when number of players is increasing.
	 *
	 *	Player's color is made from three values(RGB)
	 * Values for shine:
	 * 
	 */
typedef struct{
	char * Name;
	float Color[3];
	int Position[NUMBER_OF_PAWNS];
	int Type;
} Player;


typedef struct _FIFO_Element FIFO_Element;
typedef struct{
	struct _FIFO_Element{
		int Value;
		FIFO_Element * Next;
	} * Top,*Bottom;
}FIFO;
FIFO * FIFO_Create();
void FIFO_Push(FIFO * _FIFO, int _value);
int FIFO_Pop(FIFO * _FIFO);
int FIFO_Check(FIFO * _FIFO);
void FIFO_Delete(FIFO * _FIFO);
void FIFO_Get_All(FIFO * _queue,int * _size,int ** _tab);
void FIFO_Clean(FIFO * _queue);
	/**
	 */
void Create_Player(Player * _player,const char * _name,float * _colors,int _type);
void Set_Positions(Player * _player,int _number_of_player,int _players_number);
void Delete_Player(Player * _player);
int Check_Occupied(Player * _players, int _move, int _active_player, int _number_of_players);
void Process();
void Init_Process(int _default);
void Set_Change();
int Check_Change();
inline int POW(int a, int b);
int Check_Move(int _position,int _value,int _number_of_players,int _player_number);
Array * Default_Game_Options();
Array * Default_Graphic_Options();
int Check_All_Base(Player * _player,int _active_player,int _number_of_players);
int Find_First_Free(Player * _player,int _number_of_player,int _number_of_players);
void Close_Game();
#ifdef	__cplusplus
}
#endif

#endif	/* GAME_H */

