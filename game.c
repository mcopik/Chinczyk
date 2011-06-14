#include "game.h"
#include "graphic.h"
#include "array.h"
#include "input.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>


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

inline int POW(int a, int b){
		
		int i;
		int j = a;
		if(b == 0)
		return 1;
		for(i = 1;i < b;i++)
			j *= a;
		return j;
		
}

Array * Default_Game_Options()
{
	int i,temp;
	float temp2[3] = {1.0f,0.0f,0.0f};
	char buffer[STRING_SIZE+1],buffer2[STRING_SIZE+2];
	remove(PATH);
	Array * array = Create_Array();
	temp = 4;
	Add_Element(array,"NUMBER_OF_PLAYERS",&temp,1,INTEGER);
	temp = 0;
	Add_Element(array,"PLAYER0_AI",&temp,1,BOOLEAN);
	Add_Element(array,"PLAYER0_NAME","Gracz",6,CHAR);
	Add_Element(array,"PLAYER0_COLOR",&temp2,3,FLOAT);
	temp = 1;
	for(i = 1;i < 4;i++){
		sprintf(buffer,"PLAYER%d_AI",i);
		Add_Element(array,buffer,&temp,1,BOOLEAN);
		sprintf(buffer,"PLAYER%d_NAME",i);
		sprintf(buffer2,"Komputer %d",i);
		Add_Element(array,buffer,buffer2,11,CHAR);
		temp2[i-1] = 0.43f;
		sprintf(buffer,"PLAYER%d_COLOR",i);
		Add_Element(array,buffer,&temp2,3,FLOAT);
	}
	return array;
}

Array * Default_Graphic_Options()
{
	int temp;
	Array * array = Create_Array();
	temp = 800;
	Add_Element(array,"WIDTH",&temp,1,INTEGER);
	temp = 600;
	Add_Element(array,"HEIGHT",&temp,1,INTEGER);
	Add_Element(array,"LABEL","Chinczyk",9,CHAR);
	temp = 0;
	Add_Element(array,"FULLSCREEN",&temp,1,BOOLEAN);
	return array;
}

void Create_Player(Player * _player,const char * _name,float * _colors,int _type)
{
	_player->Name = malloc(sizeof(*_name)*(STRING_SIZE+1));
	strcpy(_player->Name,_name);
	memcpy(_player->Color,_colors,sizeof(*_colors)*3);
	_player->Type = _type;
}

void Set_Positions(Player * _player,int _number_of_player,int _players_number)
{
	int temp = _number_of_player*_players_number*2;
	int i;
	for(i=0;i < NUMBER_OF_PAWNS;i++){
		_player->Position[i] = temp++;
	}
}

void Delete_Player(Player * _player)
{
	free(_player->Name);
}

void Init_Game(Iterator ** _it,Player ** _players,Field ** _fields,int * _number_of_players,int ** _first_move)
{
	int i;
	float * temp2;
	Find(_it,"NUMBER_OF_PLAYERS");
	*_number_of_players = Get_ValueI(*_it);
	*_fields = Generate(*_number_of_players);
	*_players = malloc(*_number_of_players*sizeof(**_players));
	*_first_move = malloc(sizeof(**_first_move)*(*_number_of_players));
	for(i = 0;i < *_number_of_players;i++)
		(*_first_move)[i] = 3;
	char * buffer = malloc(STRING_SIZE*sizeof(char));
	char * buffer2;
	for(i = 0;i < *_number_of_players;i++)
	{
		sprintf(buffer,"PLAYER%d_COLOR",i);
		Find(_it,buffer);
		temp2 = Get_Value_ArrayF(*_it);
		sprintf(buffer,"PLAYER%d_AI",i);
		Find(_it,buffer);
		if(Get_ValueB(*_it))
		{
			sprintf(buffer,"PLAYER%d_NAME",i);
			Find(_it,buffer);
			buffer2 = Get_Value_ArrayC(*_it);
			Create_Player(*_players,buffer2,temp2,AI);
		}
		else
		{
			sprintf(buffer,"PLAYER%d_NAME",i);
			Find(_it,buffer);
			buffer2 = Get_Value_ArrayC(*_it);
			Create_Player(*_players,buffer2,temp2,HUMAN);
		}
		Set_Positions(*_players,i,*_number_of_players);
		(*_players)++;
		free(temp2);
		free(buffer2);
	}
	*_players -= *_number_of_players;
	free(buffer);
}

void Main_Loop(int _type)
{
	static Field * Fields = NULL;
	static Player * Players = NULL;
	static int Number_of_Players = 0;
	static int Game_Status = 0;
	static int Active_Player = 0;
	static int Selected_Pawn = -1;
	static int Available_Move = -1;
	static Array * Game_Options = NULL;
	static Array * Graphic_Options = NULL;
	static int i;
	static int Window_Number;
	static int Next_Draw = 0;
	static int Previous_State = 0;
	static Iterator * game_it,* graph_it = NULL;
	static Mouse_Action * m_event;
	static Key_Action * k_event;
	static int Width = 0,Height = 0;
	static char * Label;
	static FIFO * Randomized = NULL;
	static int Accumulator = 0;
	static int Frame_Length = 0;
	static int FPS_Time = 0.0f;
	static int FPS_Counter = 0;
	static int Time;
	static int * First_Move;
	int Next_Time;
	int Buffer;
	int * tab;
	int temp;
	char buff[STRING_SIZE];
	
	if(Game_Status == CONFIG)
	{
		if(_type == INIT_DEF)
		{
			Game_Options = Default_Game_Options();
			Graphic_Options = Default_Graphic_Options();
		}
		else
		{
			Game_Options = Load("GAME_OPTIONS",PATH);
			Graphic_Options = Load("GRAPHIC_OPTIONS",PATH);
		}
		game_it = Create_Iterator(Game_Options);
		graph_it = Create_Iterator(Graphic_Options);
		Frame_Length = 1000/FPS;
		Time = glutGet(GLUT_ELAPSED_TIME);
		/** Gettin' basic info about window */
		Find(&graph_it,"WIDTH");
		Width = Get_ValueI(graph_it);
		Find(&graph_it,"HEIGHT");
		Height = Get_ValueI(graph_it);
		Find(&graph_it,"LABEL");
		Label = Get_Value_ArrayC(graph_it);
		Find(&graph_it,"FULLSCREEN");
		if(Get_ValueB(graph_it))
			Window_Number = Init_GL(Width,Height,Label,FULLSCREEN);
		else
			Window_Number = Init_GL(Width,Height,Label,FULL_NOT);
			
		/** Gettin game options */
		//Menu_Config(graph_it,game_it);
		Game_Status = START;
	}
	
	if(_type == CLOSE)
	{
		Game_Status = QUIT;
		Next_Time = Frame_Length;
	}
	else
		Next_Time = glutGet(GLUT_ELAPSED_TIME);
		
	Buffer = Next_Time - Time;
	Time += Buffer;
	Accumulator += Buffer;
	FPS_Time += Buffer;
	
	//while(Accumulator >= Frame_Length){
		FPS_Counter++;
		if(FPS_Time >= 1000)
		{
			FPS_Time -= 1000;
                        Text_Create_FPS(FPS_Counter);
			FPS_Counter = 0;
		}
		//Accumulator -= Frame_Length;
		temp = Event_Get();
		if(temp == EVENT_MOUSE)
			Check_Mouse_Event(&m_event);
		else if(temp == EVENT_KEY)
			Check_Key_Event(&k_event);
		
		if(k_event)
		{
			if(k_event->Key == KEY_ESCAPE)
			{
				free(k_event);
				k_event = NULL;
				//Game_Status = RANDOMIZE;
			}
		}
		switch(Game_Status){
			
			case START:
			
				Init_Game(&game_it,&Players,&Fields,&Number_of_Players,&First_Move);
				Draw_Init(Fields,Players,Number_of_Players);
				Text_Init(Width,Height);
				Text_Create_Draw(0,NULL);
                                Text_Create_Player(Players[Active_Player].Name);
                                Text_Create_FPS(0);
				Set_Change();
				Previous_State = START;
				Game_Status = WAIT;
				
			case MENU:/*
				if(m_event)
					Menu(m_event);
				switch(Menu_Action()){
					case START_GAME:
					break;
					case CHANGE_GRAPH;
					case QUIT_GAME:
					Game_Status = QUIT;
					break;
				}*/
			break;
			
			case WAIT:
				if(m_event)
				{
					if(Find_Hit(CUBE_HIT,m_event->Hits,m_event->Buffer))
					{	
						Game_Status = RANDOMIZE;
						Previous_State = WAIT;
					}
				}
				Draw_Render();
			break;
			
			case RANDOMIZE:
				if(!Randomized)
					Randomized = FIFO_Create();
				srand(time(NULL));
				temp = 6;
				FIFO_Push(Randomized,temp);//rand()%6);
				if(FIFO_Check(Randomized) == 6 || First_Move[Active_Player])
				{
					Next_Draw = 1;
					First_Move[Active_Player] ? First_Move[Active_Player]-- : 0;
				}
				else
					Next_Draw = 0;
				FIFO_Get_All(Randomized,&temp,&tab);
				Text_Create_Draw(temp,tab);
				free(tab);
				tab = malloc(sizeof(*tab)*NUMBER_OF_PAWNS);
				for(i=0;i < NUMBER_OF_PAWNS;i++){
					temp = Check_Move(Players[Active_Player].Position[i],FIFO_Check(Randomized), \
							Number_of_Players,Active_Player);
					if(temp >= 0 &&\
					Check_Occupied(Players,temp,Active_Player,Number_of_Players) != -1)
							tab[i] = 1;
					else
						tab[i] = 0;
				}
				Blink_Set_Pawn(Active_Player,tab);
				free(tab);
				if(Previous_State == WAIT || Previous_State == WAIT_SELECT)
					Game_Status = WAIT_SELECT;
				else
					Game_Status = WAIT_DEC;
			break;
			
			case WAIT_SELECT:
				if(m_event)
				{
					if(Next_Draw && Find_Hit(CUBE_HIT,m_event->Hits,m_event->Buffer))
						Game_Status = RANDOMIZE;
					else
					{
						for(i=0;i<NUMBER_OF_PAWNS;i++){
							if(Find_Hit(100+Active_Player*10+i,m_event->Hits,m_event->Buffer))
							{
								Selected_Pawn = i;
								temp = Check_Move(Players[Active_Player].Position[Selected_Pawn],\
										FIFO_Check(Randomized),\
										Number_of_Players,Active_Player);
								if(temp >= 0 &&\
								Check_Occupied(Players,temp,Active_Player,Number_of_Players) != -1)
								{
										Available_Move = temp;
										Blink_Set_Field(Active_Player,Selected_Pawn,Available_Move);
										Game_Status = WAIT_DEC;
								}
								else
									Selected_Pawn = -1;
								break;
							}
						}
					}
				}
				Draw_Render();
			break;
			case WAIT_DEC:
				if(m_event)
				{
					if(Find_Hit(Available_Move+1,m_event->Hits,m_event->Buffer))
					{
						Players[Active_Player].Position[Selected_Pawn] = Available_Move;
						if(Check_All_Base(&Players[Active_Player],Active_Player,Number_of_Players))
						{	Game_Status = QUIT;break;}
						Selected_Pawn = -1;
						Available_Move = -1;
						temp = FIFO_Pop(Randomized);
						FIFO_Get_All(Randomized,&temp,&tab);
                                                Text_Create_Draw(temp,tab);
                                                if(FIFO_Check(Randomized) == -1 && Next_Draw)
						{
							Game_Status = QUIT;//WAIT;
							Set_Change();
							Disable_Blink();
						}
						else if(FIFO_Check(Randomized) == -1)
						{	
							Active_Player = ++Active_Player % Number_of_Players;
                                                        Text_Create_Player(Players[Active_Player].Name);
                                                        Game_Status = WAIT;
							Set_Change();
							Disable_Blink();
						}
						else
						{
							tab = malloc(sizeof(*tab)*NUMBER_OF_PAWNS);
							for(i=0;i < NUMBER_OF_PAWNS;i++){
								temp = Check_Move(Players[Active_Player].Position[i],FIFO_Check(Randomized), \
								Number_of_Players,Active_Player);
								if(temp >= 0 &&\
								Check_Occupied(Players,temp,Active_Player,Number_of_Players) != -1)
									tab[i] = 1;
								else
									tab[i] = 0;
							}
							Blink_Set_Pawn(Active_Player,tab);
							Game_Status = WAIT_SELECT;
							free(tab);
						}
					}
					else if(Next_Draw && Find_Hit(CUBE_HIT,m_event->Hits,m_event->Buffer))
					{
						Previous_State = WAIT_DEC;	
						Game_Status = RANDOMIZE;
					}
					else
					{
						for(i=0;i<NUMBER_OF_PAWNS;i++){
							if(Find_Hit(100+Active_Player*10+i,m_event->Hits,m_event->Buffer))
							{
								if(i != Selected_Pawn)
								{
									Selected_Pawn = i;
									temp = Check_Move(Players[Active_Player].Position[Selected_Pawn],\
											FIFO_Check(Randomized),\
											Number_of_Players,Active_Player);
									if(temp >= 0 &&\
									Check_Occupied(Players,temp,Active_Player,Number_of_Players) != -1)
									{
											Available_Move = temp;
											Blink_Set_Field(Active_Player,Selected_Pawn,Available_Move);
									}
									else
										Selected_Pawn = -1;
									break;
								}
							}
						}
					}
				}
				Draw_Render();
			break;
			case INGAME_MENU:
				Draw_Render();
				Game_Status = QUIT;
			break;
			case QUIT:
				if(Fields)
					free(Fields);
				for(i=0;i<Number_of_Players;i++)
					Delete_Player(&Players[i]);
				if(Players)
					free(Players);
				if(Label)
					free(Label);
				if(Randomized)
					FIFO_Delete(Randomized);
				Text_Clean();
				Event_Clean();
				Delete_Iterator(graph_it);
				Delete_Iterator(game_it);
				Save(Game_Options,"GAME_OPTIONS",PATH);
				Save(Graphic_Options,"GRAPHIC_OPTIONS",PATH);
				Free_Array(Game_Options);
				Free_Array(Graphic_Options);
				glutDestroyWindow(Window_Number);
			break;
		}
		if(m_event)
		{	
			free(m_event);
			m_event = NULL;
		}
	//}
}

void Process()
{
	Main_Loop(LOOP);
}

void Init_Process(int _default)
{
	if(_default)
		Main_Loop(INIT_DEF);
	else
		Main_Loop(INIT);
}

int _Change(int _type)
{
	static int Change = 0;
	
	if(_type == SET_CHANGE)
		Change = 1;
	else
	{
		if(!Change)
			return 0;
		else
		{
			Change = 0;
			return 1;
		}
	}
	return 0;
}

void Set_Change()
{
	_Change(SET_CHANGE);
}

int Check_Change()
{
	return _Change(CHECK_CHANGE);
}

FIFO * FIFO_Create()
{
	FIFO * ret = malloc(sizeof(*ret));
	ret->Top = NULL;
	ret->Bottom = NULL;
	return ret;
}

void FIFO_Push(FIFO * _FIFO, int _value)
{
	FIFO_Element * ptr;
	ptr = malloc(sizeof(*ptr));
	ptr->Value = _value;
	ptr->Next = NULL;
	if(_FIFO->Top)
		_FIFO->Top->Next = ptr;
	else
		_FIFO->Bottom = ptr;
	_FIFO->Top = ptr;
}

int FIFO_Pop(FIFO * _FIFO)
{
	if(_FIFO->Bottom)
	{
		int ret = _FIFO->Bottom->Value;
		FIFO_Element * ptr;
		ptr = _FIFO->Bottom;
		_FIFO->Bottom = ptr->Next;
		if(!_FIFO->Bottom)
			_FIFO->Top = NULL;
		free(ptr);
		return ret;
	}
	return -1;
}

int FIFO_Check(FIFO * _FIFO)
{
	if(_FIFO->Bottom)
		return _FIFO->Bottom->Value;
	return -1;
}

void FIFO_Delete(FIFO * _FIFO)
{
	FIFO_Element * ptr;
	while(_FIFO->Bottom){
		ptr = _FIFO->Bottom;
		_FIFO->Bottom = ptr->Next;
		free(ptr);
	}
	free(_FIFO);
}

void FIFO_Get_All(FIFO * _queue,int * _size,int ** _tab)
{
	int i;
	*_size = 0;
	FIFO_Element * ptr;
	ptr = _queue->Bottom;
	while(ptr != NULL){
		(*_size)++;
		ptr = ptr->Next;
	}
	*_tab = malloc(sizeof(**_tab)*(*_size));
	ptr = _queue->Bottom;
	for(i = 0;i < *_size;i++){
		(*_tab)[i] = ptr->Value;
		ptr = ptr->Next;
	}
}
		
int Check_Move(int _position,int _value,int _number_of_players,int _player_number)
{
	int shift = NUMBER_OF_PAWNS*_number_of_players*2;
	int base = _player_number*NUMBER_OF_FIELDS_PER_PLAYER + shift;
	int length = NUMBER_OF_FIELDS_PER_PLAYER*_number_of_players;
	int temp;
	if(_position < shift)
	{
		/** is pawn in base or in home? */
		if(_position % (2*NUMBER_OF_PAWNS) < NUMBER_OF_PAWNS)
		{
			if(_value == 6)
				return shift + NUMBER_OF_FIELDS_PER_PLAYER*_player_number;
			else
				return -1;
		}
		else
		{
			if((NUMBER_OF_PAWNS- (_position % NUMBER_OF_PAWNS) -1) >= _value)
				return _position + _value;
			else
				return -1;
		}	
	}
	else
	{
		if( _position - base >= 0)
		{	
			if( (_value+_position) - base < length )
				return _value+_position;
			else
			{
				//if pawn can go to "home"
				temp = ( (_value+_position) - base ) - length;
				if( temp < NUMBER_OF_PAWNS)
					return _player_number*NUMBER_OF_PAWNS*2 + NUMBER_OF_PAWNS + temp;
				else
					return -1;
			}
		
		}
		else
		{
			if( (length - base+shift) + (_value+_position-shift) < length)
				return _value+_position;
			else
			{
				temp = ( (length - base+shift) + (_value+_position-shift) ) - length;
				if( temp < NUMBER_OF_PAWNS)
					return _player_number*NUMBER_OF_PAWNS*2 + NUMBER_OF_PAWNS + temp;
				else
					return -1;
			}
		}
	}
}

int Check_Occupied(Player * _players, int _move, int _active_player, int _number_of_players)
{
	int i,j;
	for(i = 0;i < _number_of_players;i++){
		for(j = 0;j < NUMBER_OF_PAWNS;j++){
			if(_move == _players[i].Position[j])
			{
				if(i == _active_player)
					return -1;
				else
					return 10*i + j;
			}
		}
	}
	return 0;
}
int Check_All_Base(Player * _player,int _active_player,int _number_of_players)
{
	int i;
	for(i = 0;i < NUMBER_OF_PAWNS;i++){
		if(!(_player->Position[i] >= NUMBER_OF_PAWNS*(2*_active_player+1) &&\
			_player->Position[i] < NUMBER_OF_PAWNS*(2*_active_player+2)))
				return 0;
	}
	return 1;
}

void Close_Game()
{
	Main_Loop(CLOSE);
}


	
