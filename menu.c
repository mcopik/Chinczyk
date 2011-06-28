#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "menu.h"
#include "input.h"
#include "graphic.h"

void _Menu(Iterator * _game_it,Iterator * _graph_it,Mouse_Action * _m_event,int * flag);

int Menu_New_Game(Iterator * _game_it,Iterator * _graph_it,Menu ** _menu_active,int _position)
{
	return MENU_NEW_GAME;
}

int Menu_Close_Game(Iterator * _game_it,Iterator * _graph_it,Menu ** _menu_active,int _position)
{
	return MENU_CLOSE;
}

int Menu_Go_Next(Iterator * _game_it,Iterator * _graph_it,Menu ** _menu_active,int _position)
{
	*_menu_active = (*_menu_active)->Children[_position];
	return MENU_NO_CHANGE;
}

int Menu_Go_Previous(Iterator * _game_it,Iterator * _graph_it,Menu ** _menu_active,int _position)
{
	*_menu_active = (*_menu_active)->Parent;
	return MENU_NO_CHANGE;
}

int Menu_Graph_Resolution_Change(Iterator * _game_it,Iterator * _graph_it,\
									Menu ** _menu_active,int _position)
{
	int width,height,i;
	const char * label = "Rozdzielczosc: ";
	char * buffer;
	static int Display[4][2] = {{640,480},{800,600},{1024,768},{1280,960}};
	Find(_graph_it,"WIDTH");
	width = Get_ValueI(_graph_it);
	for(i = 0;i < 4;i++)
		if(width == Display[0][i])
			break;
	i = ++i % 4;
	width = Display[0][i];
	Set_Value(_graph_it,(void*)&width);
	Find(_graph_it,"HEIGHT");
	height = Display[1][i];
	Set_Value(_graph_it,(void*)&height);
	buffer = (char*) malloc(sizeof(*buffer)*(STRING_SIZE+1));
	sprintf(buffer,"%s %dx%d",label,width,height);
	strcpy((*_menu_active)->Captions[_position],buffer);
	free(buffer);
	return MENU_GRAPH_CHANGE;
}

int Menu_Graph_Fullscr_Change(Iterator * _game_it,Iterator * _graph_it,\
								Menu ** _menu_active,int _position)
{
	const char * fullscr_label = "Pelny ekran: ";
	char * buffer;
	int temp;
	buffer = (char*) malloc(sizeof(*buffer)*(STRING_SIZE+1));
	Find(_graph_it,"FULLSCREEN");
	if(Get_ValueB(_graph_it))
	{
		temp = 0;
		Set_Value(_graph_it,(void*)&temp);
		sprintf(buffer,"%s NIE",fullscr_label);
	}
	else
	{
		temp = 1;
		Set_Value(_graph_it,(void*)&temp);
		sprintf(buffer,"%s TAK",fullscr_label);
	}
	
	strcpy((*_menu_active)->Captions[_position],buffer);
	free(buffer);
	return MENU_GRAPH_CHANGE;
}

void Menu_Draw(Iterator * _graph_it,Menu * _menu)
{
	int i;
    int width,height;
	char * buffer;
	buffer = malloc(sizeof(*buffer)*(STRING_SIZE+1));
    Find(_graph_it,"WIDTH");
	width = Get_ValueI(_graph_it);
    Find(_graph_it,"HEIGHT");
	height = Get_ValueI(_graph_it);
	Text_Draw(0.85*width,0.05*height,MENU_BUTTON_HIT,FONT2,TEXT_CENTER,MENU_MSG,MENU_TEXT);
    for(i = 0;i < _menu->Number_of_Positions;i++)
	{
		sprintf(buffer,"MENU_TEXT_%d",i);
		Text_Draw(0.85*width,(0.1+0.05*i)*height,200+i,FONT1,TEXT_CENTER,buffer,_menu->Captions[i]);
	}
	free(buffer);
    Set_Change();
}

void Menu_Clean_Drawing(Menu *_menu)
{
	int i;
	char * buffer;
	buffer = malloc(sizeof(*buffer)*(STRING_SIZE+1));
	Text_Remove(MENU_MSG);
	for(i = 0;i < _menu->Number_of_Positions;i++){
		sprintf(buffer,"MENU_TEXT_%d",i);
		Text_Remove(buffer);
	}
	free(buffer);
	Set_Change();
}

void Clean(Menu * _menu)
{
	int i;
	for(i = 0;i < _menu->Number_of_Positions;i++){
		if(_menu->Children[i])
			Clean(_menu->Children[i]);
	}
	for(i = 0;i < _menu->Number_of_Positions;i++)
		free(_menu->Captions[i]);
	free(_menu->Captions);
	free(_menu->Children);
	free(_menu->Action);
	free(_menu);
}

void Menu_Init(Menu ** _main,Iterator * _game_it,Iterator * _graph_it)
{
	Menu * ptr;
	int width,height;
	const char * display_label = "Rozdzielczosc: ",
				* fullscr_label = "Pelny ekran: ";
	char * buffer;
	int i;
	buffer = (char*) malloc(sizeof(*buffer)*(STRING_SIZE+1));
	ptr = (Menu*) malloc(sizeof(*ptr));
	ptr->Parent = NULL;
	ptr->Number_of_Positions = 4;
	ptr->Captions = (char**) malloc(ptr->Number_of_Positions*sizeof(*(ptr->Captions)));
	for(i = 0;i < ptr->Number_of_Positions;i++)
		ptr->Captions[i] = (char*) malloc((STRING_SIZE+1)*\
										sizeof(ptr->Captions[i]));
	strcpy(ptr->Captions[0],"Nowa gra");
	strcpy(ptr->Captions[1],"Opcje gry");
	strcpy(ptr->Captions[2],"Opcje grafiki");
	strcpy(ptr->Captions[3],"Wyjscie");
	ptr->Children = (Menu**) malloc(ptr->Number_of_Positions*sizeof(*(ptr->Children)));
	ptr->Action = (int (**)(Iterator*,Iterator*,Menu *,int)) \
					malloc(ptr->Number_of_Positions*sizeof(*(ptr->Action)));
	
	ptr->Children[0] = NULL;
	ptr->Action[0] = &Menu_New_Game;
	
	ptr->Children[1] = NULL;
	ptr->Action[1] = NULL;
	
	ptr->Children[2] = (Menu*) malloc(sizeof(*ptr->Children[2]));
	ptr->Children[2]->Parent = ptr;
	ptr->Children[2]->Number_of_Positions = 3;
	ptr->Children[2]->Captions = (char**) malloc(ptr->Children[2]->Number_of_Positions*\
									sizeof(*(ptr->Children[2]->Captions)));
	ptr->Children[2]->Children = (Menu**) malloc(ptr->Children[2]->Number_of_Positions*\
									sizeof(*(ptr->Children[2]->Children)));
	ptr->Children[2]->Action = (int (**)(Iterator*,Iterator*,Menu *,int)) \
	malloc(ptr->Children[2]->Number_of_Positions*sizeof(*(ptr->Children[2]->Action)));
	for(i = 0;i < ptr->Children[2]->Number_of_Positions;i++)
		ptr->Children[2]->Captions[i] = (char*) malloc((STRING_SIZE+1)*\
										sizeof(ptr->Children[2]->Captions[i]));
	Find(_graph_it,"WIDTH");
	width = Get_ValueI(_graph_it);
	Find(_graph_it,"HEIGHT");
	height = Get_ValueI(_graph_it);
	sprintf(buffer,"%s %dx%d",display_label,width,height);
	strcpy(ptr->Children[2]->Captions[0],buffer);
	Find(_graph_it,"FULLSCREEN");
	if(Get_ValueB(_graph_it))
		sprintf(buffer,"%s TAK",fullscr_label);
	else
		sprintf(buffer,"%s NIE",fullscr_label);
	strcpy(ptr->Children[2]->Captions[1],buffer);
	strcpy(ptr->Children[2]->Captions[2],"Wroc");
	
	for(i = 0;i < ptr->Children[2]->Number_of_Positions;i++)
		ptr->Children[2]->Children[i] = NULL;
	ptr->Children[2]->Action[0] = &Menu_Graph_Resolution_Change;
	ptr->Children[2]->Action[1] = &Menu_Graph_Fullscr_Change;
	ptr->Children[2]->Action[2] = &Menu_Go_Previous;
	ptr->Action[2] = Menu_Go_Next;
	
	ptr->Children[3] = NULL;
	ptr->Action[3] = &Menu_Close_Game;
	
	free(buffer);
	*_main = ptr;
}

void Menu_Active()
{
	int temp = 1;
	_Menu(NULL,NULL,NULL,&temp);
}

void Menu_Disactive()
{
	int temp = 0;
	_Menu(NULL,NULL,NULL,&temp);
}

void Menu_Clean()
{
	int temp = -1;
	_Menu(NULL,NULL,NULL,&temp);
}

void Menu_Config(Iterator *_game_it,Iterator * _graph_it)
{
	int temp = -1;
	_Menu(_game_it,_graph_it,NULL,&temp);
}

int Menu_Click(Mouse_Action * _m_event)
{
	int temp = 0;
	_Menu(NULL,NULL,_m_event,&temp);
	return temp;
}

void _Menu(Iterator * _game_it,Iterator * _graph_it,Mouse_Action * _m_event,int * flag)
{
	static Iterator * Game_Iterator = NULL;
	static Iterator * Graph_Iterator = NULL;
	static Menu * Main;
	static Menu * Active_Menu;
	int i;
	if(_game_it)
	{
		Game_Iterator = _game_it;
		Graph_Iterator = _graph_it;
		Menu_Init(&Main,_game_it,_graph_it);
		Active_Menu = Main;
	}
	else if(_m_event)
	{
		for(i = 0;i < Active_Menu->Number_of_Positions;i++){
			if(Find_Hit(200+i,_m_event->Hits,_m_event->Buffer))
			{
				Menu_Clean_Drawing(Active_Menu);
				if(Active_Menu->Action[i])
					*flag = Active_Menu->Action[i](Game_Iterator,Graph_Iterator,&Active_Menu,i);
				else
					*flag = MENU_NO_CHANGE;
				Menu_Draw(Graph_Iterator,Active_Menu);
			}
		}
	}
	else if(*flag == -1)
	{
		Clean(Main);
	}
	else
	{
		if(*flag)
			Menu_Draw(Graph_Iterator,Active_Menu);
		else
			Menu_Clean_Drawing(Active_Menu);
	}
}