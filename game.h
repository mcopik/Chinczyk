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

#include <GL/glut.h>
#include "array.h"
#include "input.h"
#include "fifo.h"

#define MENU_BUTTON_HIT 230
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
	printf("Feature not implemented yet\n"); \
	return;					\
}
#define PATH "options.txt"
#define NUMBER_OF_PAWNS 4
#define MAX_PLAYERS 6
#define MIN_PLAYERS 4
#define NUMBER_OF_FIELDS_PER_PLAYER 8
#define FONT1 GLUT_BITMAP_9_BY_15
#define FONT1_SIZE 9
#define FONT2 GLUT_BITMAP_TIMES_ROMAN_24 
#define FONT2_SIZE 24
#define FONT3  GLUT_STROKE_MONO_ROMAN
#define DRAW_MSG "Draw_Msg"
#define NAME_MSG "Name_Msg"
#define FPS_MSG "FPS_Msg"
#define MENU_MSG "Menu_Msg"
#define MENU_TEXT "Menu"
#define FPS_TEXT "FPS: "
#define DRAWING_TEXT "Wylosowano:"

#define TEXT_DRAW_PLAYER 									\
Text_Draw(0.05,0.1,255,FONT1,TEXT_NORMAL,					\
			NAME_MSG,"%s",Players[Active_Player].Name);
		
#define TEXT_DRAW_FPS 										\
Text_Draw(0.05,0.05,255,FONT1,TEXT_NORMAL,					\
			FPS_MSG,"%s %d",FPS_TEXT,FPS_Counter);
		
#define TEXT_DRAW_RAND(buffer,number,tab) 					\
buffer = (char *) malloc(sizeof(*buffer)*STRING_SIZE*2);	\
strcpy(buffer,DRAWING_TEXT);								\
for(i = 0;i < number;i++){									\
	sprintf(buffer,"%s %d",buffer,tab[i]);					\
}															\
Text_Draw(0.05,0.15,255,(void*)FONT1,						\
			TEXT_NORMAL,DRAW_MSG,"%s",buffer);				\
free(buffer);
		
#define TEXT_DRAW_MENU_BUTTON								\
Text_Draw(0.85,0.05,MENU_BUTTON_HIT,						\
			(void*)FONT2,MENU_MSG,"%s",MENU_TEXT);		
	
	enum{
		LOOP_CONFIG,
		LOOP_START,
		LOOP_MENU,
		LOOP_INGAME_MENU,
		LOOP_WAIT,
		LOOP_RANDOMIZE,
		LOOP_WAIT_DEC,
		LOOP_WAIT_SELECT,
		LOOP_END,
		LOOP_NEXT_PLAYER,
		LOOP_QUIT
	};
	
	enum{
		HUMAN,
		AI
	};
	
	enum{
		INIT,
		INIT_DEF,
		LOOP,
		CLOSE
	};
	enum{
		SET_CHANGE,
		CHECK_CHANGE
	};
	/**
	 * 
	 * value of field 'Position':
	 * 0 <-> _number_of_players*NUMBER_OF_PAWNS*2 - 1 -> start and final positions
	 * _number_of_players*NUMBER_OF_PAWNS*2 <-> NUM.._PER_PLAYER*_number_of_players - 1 -> 
	 * for all positions in game
	 * 
	 */
	typedef struct{
		char * Name;
		float Color[3];
		int Position[NUMBER_OF_PAWNS];
		int Type;
	} Player;


	void Create_Player(Player * _player,const char * _name,float * _colors,int _type);
	void Set_Positions(Player * _player,int _number_of_player,int _players_number);
	void Delete_Player(Player * _player);
	int Check_Occupied(Player * _players, int _move, int _active_player, int _number_of_players);
	void Process();
	void Init_Process(int _default);
	void Set_Change();
	int Check_Change();
	int POW(int a, int b);
	int Check_Move(int _position,int _value,int _number_of_players,int _player_number);
	Array * Default_Game_Options();
	Array * Default_Graphic_Options();
	int Check_All_Base(Player * _player,int _active_player,int _number_of_players);
	int Find_First_Free(Player * _player,int _number_of_player,int _number_of_players);
	int Get_Distance(int _position,int _player_number,int _number_of_players);
	void Close_Game();
#ifdef	__cplusplus
}
#endif

#endif	/* GAME_H */

