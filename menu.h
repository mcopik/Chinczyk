#include "array.h"
#include "input.h"
#define CHANGE "Change"


typedef struct Menu_Struct Menu;
struct Menu_Struct{
	char ** Captions;
	Menu ** Children;
	int (**Action)(Iterator*,Iterator*,Menu **,int);
	int Number_of_Positions;
	Menu * Parent;
};

enum{
	MENU_NO_CHANGE = 0,
	MENU_NEW_GAME,
	MENU_CLOSE,
	MENU_GRAPH_CHANGE_RES,
	MENU_GRAPH_CHANGE_FULL,
	MENU_GAME_CHANGE
};

void Menu_Init(Menu ** _main,Iterator * _game_it,Iterator * _graph_it);
void Menu_Config(Iterator * _game_it,Iterator * _graph_it);
void Menu_Active();
void Menu_Disactive();
void Menu_Clean();
void Menu_Config(Iterator *_game_it,Iterator * _graph_it);
int Menu_Click(Mouse_Action * _m_event);