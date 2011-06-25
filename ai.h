#ifndef AI_H
#define	AI_H
#include "input.h"
#include "fifo.h"

enum{
	AI_EASY,
	AI_MEDIUM,
	AI_HARD
};

typedef struct{
	Player * Players;
	int Number_of_Players;
	FIFO * Randomized;
	int * Available_Move;
	int * Active_Pawns;
	float Level;
} AI_Info;

void AI_Init(FIFO * _randomized,Player * _players,int * _a_pawns,int * _a_move,\
	int _number_of_players, int _level);
void AI_Process_Select(Mouse_Action ** _m_event,int _active_player);
void AI_Process_Decision(Mouse_Action ** m_event);
#endif