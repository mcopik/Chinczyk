#include <stdlib.h>
#include "game.h"
#include "input.h"
#include "ai.h"

#define LEVEL_EASY 0.25f
#define LEVEL_MEDIUM 0.5f
#define LEVEL_HARD 0.75f




void _AI(int _active_player, Mouse_Action ** _m_event,AI_Info * _info)
{
	static AI_Info Info;
	static int Counter = NUMBER_OF_PAWNS;
	static int Active_Player = -1;
	int i,temp,max_index;
	float max,temp2;
	
	if(_info)
	{
		Info.Players = _info->Players;
		Info.Available_Move = _info->Available_Move;
		Info.Active_Pawns = _info->Active_Pawns;
		Info.Randomized = _info->Randomized;
		Info.Level = _info->Level;
		Info.Number_of_Players = _info->Number_of_Players;
	}
	else if(_active_player >= 0)
	{
		if(Active_Player != _active_player)
		{
			Active_Player = _active_player;
			Counter = NUMBER_OF_PAWNS;
			temp = Active_Player*NUMBER_OF_PAWNS*2;
			for(i = 0;i < NUMBER_OF_PAWNS;i++)
				if(Info.Players[Active_Player].Position[i] >= temp &&\
					Info.Players[Active_Player].Position[i] < temp+2*NUMBER_OF_PAWNS)
						Counter--;
		}
		// there is only one(or zero) pawns on board
		// the best way is to get more pawns in game
		if(Counter < 2)
		{
			for(i = 0;i < NUMBER_OF_PAWNS;i++){
				//distance to home from new position is equal to length-1 <=>
				//<=> when new position is first field on board
				if(Get_Distance(Info.Active_Pawns[i],Active_Player,\
					Info.Number_of_Players) == Info.Number_of_Players*NUMBER_OF_FIELDS_PER_PLAYER -1)
				{
						Counter++;
						*_m_event = malloc(sizeof(**_m_event));
						(*_m_event)->Hits = 1;
						(*_m_event)->Buffer[0] = 1;
						(*_m_event)->Buffer[1] = 0;
						(*_m_event)->Buffer[2] = 0;
						(*_m_event)->Buffer[3] = 100 + Active_Player*10 + i;
						return;
				}
			}
		}
		if(Counter > 0)
		{
			//at least two pawns on board, situation is safe
			//or one pawn on board, but there's no way to get more
			max = -1.0f;
			max_index = -1;
			for(i = 0;i < NUMBER_OF_PAWNS;i++){
				// computer's not going to move pawn which can't move
				if(Info.Active_Pawns[i] != -1)
				{
					//moving this pawn will "kill" another pawn
					//when there are two pawns which can attack, we're going to choose
					//this one which is closer to "home"
					if(Check_Occupied(Info.Players,Info.Active_Pawns[i],\
					Active_Player,Info.Number_of_Players) > 0)
					{	
						if(max < Info.Level)
						{
							max = Info.Level;
							max_index = i;
						}
						else if(max == Info.Level)
						{
							if(Get_Distance(Info.Players[Active_Player].Position[max_index],Active_Player,\
								Info.Number_of_Players) > \
								Get_Distance(Info.Players[Active_Player].Position[i],Active_Player,\
								Info.Number_of_Players))
								max_index = i;
						}
					}
					//when move has no effect on other pawns
					//we're going to choose this one which is closer to "home"
					//ratio = 1 - distance to home/length of board
					else
					{
						temp2 = (float)Get_Distance(Info.Players[Active_Player].Position[i],Active_Player,\
								Info.Number_of_Players);
						temp2 = 1.0f - (float)(temp2/(Info.Number_of_Players*NUMBER_OF_FIELDS_PER_PLAYER));
						//second condition is for avoiding situation, when one pawn is moving forward
						//and second one is still on first field
						if(max < temp2 || max/temp2 >= 3.0f)
						{	
							max = temp2;
							max_index = i;
						}
					}
				}
			}
			//if new position if first field on board
			if(Get_Distance(Info.Active_Pawns[max_index],Active_Player,\
					Info.Number_of_Players) == Info.Number_of_Players*NUMBER_OF_FIELDS_PER_PLAYER -1)
				Counter++;
			//if new position is in home
			if(!Get_Distance(Info.Active_Pawns[max_index],Active_Player,\
					Info.Number_of_Players))
				Counter--;
			//simulating click on selected pawn
			*_m_event = malloc(sizeof(**_m_event));
			(*_m_event)->Hits = 1;
			(*_m_event)->Buffer[0] = 1;
			(*_m_event)->Buffer[1] = 0;
			(*_m_event)->Buffer[2] = 0;
			(*_m_event)->Buffer[3] = 100 + Active_Player*10 + max_index;
			return;
		}
		else
		{
			for(i = 0;i < NUMBER_OF_PAWNS;i++)
			{
				if(Info.Active_Pawns[i] != -1)
				{
					*_m_event = malloc(sizeof(**_m_event));
					(*_m_event)->Hits = 1;
					(*_m_event)->Buffer[0] = 1;
					(*_m_event)->Buffer[1] = 0;
					(*_m_event)->Buffer[2] = 0;
					(*_m_event)->Buffer[3] = 100 + Active_Player*10 + i;
					return;
				}
			}
		}
	}
	//simulating click to move selected pawn
	else
	{
		*_m_event = malloc(sizeof(**_m_event));
		(*_m_event)->Hits = 1;
		(*_m_event)->Buffer[0] = 1;
		(*_m_event)->Buffer[1] = 0;
		(*_m_event)->Buffer[2] = 0;
		(*_m_event)->Buffer[3] = *(Info.Available_Move) + 1;
		return;
	}
		
}



void AI_Process_Select(Mouse_Action ** _m_event,int _active_player)
{
	_AI(_active_player,_m_event,NULL);
}

void AI_Process_Decision(Mouse_Action ** _m_event)
{
	_AI(-1,_m_event,NULL);
}

void AI_Init(FIFO * _randomized,Player * _players,int * _a_pawns,int * _a_move,\
 int _number_of_players, int _level)
{
	AI_Info temp;
	temp.Players = _players;
	temp.Available_Move = _a_move;
	temp.Active_Pawns = _a_pawns;
	temp.Randomized = _randomized;
	temp.Number_of_Players = _number_of_players;
	switch(_level)
	{
		case AI_EASY:
			temp.Level = LEVEL_EASY;
		break;
		case AI_MEDIUM:
			temp.Level = LEVEL_MEDIUM;
		break;
		case AI_HARD:
			temp.Level = LEVEL_HARD;
		break;
	}
	
	_AI(-1,NULL,&temp);
}