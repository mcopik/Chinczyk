#include <stdlib.h>
#include "game.h"
#include "input.h"
#include "ai.h"

void _AI(int _active_player, Mouse_Action ** _m_event,AI_Info * _info)
{
	//podstawowe informacje niezbędne do prawidłowej pracy algorytmu
	static AI_Info Info;
	//liczba pionków na planszy
	static int Counter = NUMBER_OF_PAWNS;
	//aktywny gracz
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
	//funkcja wywołana aby wybrać pionka do ruchu
	else if(_active_player >= 0)
	{
		//zmiana gracza, konieczne wyliczenie liczby pionków na splanszy
		if(Active_Player != _active_player)
		{
			Active_Player = _active_player;
			Counter = NUMBER_OF_PAWNS;
			for(i = 0;i < NUMBER_OF_PAWNS;i++)
				if(!Get_Distance(Info.Players[Active_Player].Position[i],Active_Player,Info.Number_of_Players))
						Counter--;
		}
		// jeden albo zero pionków w grze, wypada wprowadzić więcej na planszę
		if(Counter < 2)
		{
			for(i = 0;i < NUMBER_OF_PAWNS;i++){
				//dystans od nowej pozycji do końca planszy jest równy długość planszy -1 <=>
				//<=> gdy nowa pozycja jest pierwszą na planszy 
				if(Get_Distance(Info.Active_Pawns[i],Active_Player,\
					Info.Number_of_Players) == Info.Number_of_Players*NUMBER_OF_FIELDS_PER_PLAYER -1)
				{
						Counter++;
						//przygotowanie zdarzenia myszy odpowiadającego za wprowadzenie pionka na planszę
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
			//dwa pionki na planszy, stabilna sytuacja
			//lub jeden pionek, ale nie da rady więcej
			max = -1.0f;
			max_index = -1;
			for(i = 0;i < NUMBER_OF_PAWNS;i++){
				if(Info.Active_Pawns[i] != -1)
				{
					//sprawdzenie czy ruszenie tego pionka zabije innego
					//jeśli jest więcej niż jeden pionek do zabicia, trzeba wybrać tego bardziej opłacalnego
					if(Check_Occupied(Info.Players,Info.Active_Pawns[i],\
					Active_Player,Info.Number_of_Players) > 0)
					{	
						//pionek do bicia; jeśli nie ma bardziej opłacalnej opcji, wybieramy tą
						//"opłacalność" = współczynnik agresywności komputera
						if(max < Info.Level)
						{
							max = Info.Level;
							max_index = i;
						}
						//jest już przynajmniej jeden pionek do bicia
						else if(max == Info.Level)
						{
							//wybieramy ten pionek przeciwnika, który jest bliżej bazy
							//większy zysk dla nas, większa strata dla przeciwnika
							if(Get_Distance(Info.Players[Active_Player].Position[max_index],Active_Player,\
								Info.Number_of_Players) > \
								Get_Distance(Info.Players[Active_Player].Position[i],Active_Player,\
								Info.Number_of_Players))
								max_index = i;
						}
					}
					//jeśli ruch nie bije nikogo innego, to wybieramy pionek bliższy bazie
					//współczynnik opłacalności = 1 - dystans_do_domu/długość_planszy
					//współczynnik jest po to, aby móc porównać czyt bardziej opłacalny jest ruch pionkiem blisko bazy
					//czy bicie innego
					else
					{
						temp2 = (float)Get_Distance(Info.Players[Active_Player].Position[i],Active_Player,\
								Info.Number_of_Players);
						temp2 = 1.0f - (float)(temp2/(Info.Number_of_Players*NUMBER_OF_FIELDS_PER_PLAYER));
						//drugi warunek jest po to, aby uniknąć sytuacji, gdy jeden z pionków zbliża się do bazy
						//a drugi stoi na pierwszym polu; rozsądna gra wymaga równomiernego prowadzenia pionków
						if(max < temp2 || max/temp2 >= 3.0f)
						{	
							max = temp2;
							max_index = i;
						}
					}
				}
			}
			//jeśli wprowadzamy pionek na planszę
			if(Get_Distance(Info.Active_Pawns[max_index],Active_Player,\
					Info.Number_of_Players) == Info.Number_of_Players*NUMBER_OF_FIELDS_PER_PLAYER -1)
				Counter++;
			//jeśli wyprowadzamy pionek z planszy
			if(!Get_Distance(Info.Active_Pawns[max_index],Active_Player,\
					Info.Number_of_Players))
				Counter--;
			//przygotowanie zdarzenia myszy odpowiadającego za wprowadzenie pionka na planszę
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
			//zero pionków na planszy, ale być może możliwy ruch na mecie
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
	//funkcja wywołana aby ruszyć wybrany pionek
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
	//przekształcenie poziomu trudności na współczynnik agresywności komputera
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