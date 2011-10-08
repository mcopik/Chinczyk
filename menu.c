/*!
 * @file menu.c
 * Zawiera implementację wszystkich funkcji używanych do obsługi menu w grze.@n
 * Menu do rysowania wykorzystuje obsługę struktur Text zaimplementowaną w graphic.@n
 * @par Chińczyk
 * @author Marcin Copik
 * @date 2011.09.10
 * @version 1.0
*/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#ifdef _WIN32
# include <GL/freeglut.h>
#else
# include <GL/glut.h>
#endif
#include "menu.h"
#include "input.h"
#include "graphic.h"
#include "game.h"
#include "ai.h"

void _Menu(Iterator * _game_it,Iterator * _graph_it,Mouse_Action * _m_event,int * flag);
/*!
 * Funkcja uruchomienia nowej gry.@n
 * @param[in] _game_it Wskaźnik na iterator przypisany do tablicy opcji gry
 * @param[in] _graph_it Wskaźnik na iterator przypisany do tablicy opcji grafiki
 * @param[in,out] _menu_active Wskaźnik na strukturę aktywnego menu
 * @param[in] _position Kliknięta pozycja w menu
 * @return Numer zmiany dokonanej przez kliknięcie.
 */
int Menu_New_Game(Iterator * _game_it,Iterator * _graph_it,Menu ** _menu_active,int _position)
{
	return MENU_NEW_GAME;
}
/*!
 * Funkcja zamknięcia gry.@n
 * @param[in] _game_it Wskaźnik na iterator przypisany do tablicy opcji gry
 * @param[in] _graph_it Wskaźnik na iterator przypisany do tablicy opcji grafiki
 * @param[in,out] _menu_active Wskaźnik na strukturę aktywnego menu
 * @param[in] _position Kliknięta pozycja w menu
 * @return Numer zmiany dokonanej przez kliknięcie.
 */
int Menu_Close_Game(Iterator * _game_it,Iterator * _graph_it,Menu ** _menu_active,int _position)
{
	return MENU_CLOSE;
}
/*!
 * Funkcja przejścia do podmenu.@n
 * @param[in] _game_it Wskaźnik na iterator przypisany do tablicy opcji gry
 * @param[in] _graph_it Wskaźnik na iterator przypisany do tablicy opcji grafiki
 * @param[in,out] _menu_active Wskaźnik na strukturę aktywnego menu
 * @param[in] _position Kliknięta pozycja w menu
 * @return Numer zmiany dokonanej przez kliknięcie.
 */
int Menu_Go_Next(Iterator * _game_it,Iterator * _graph_it,Menu ** _menu_active,int _position)
{
	*_menu_active = (*_menu_active)->Children[_position];
	return MENU_NO_CHANGE;
}
/*!
 * Funkcja powrotu do menu nadrzędnego.@n
 * @param[in] _game_it Wskaźnik na iterator przypisany do tablicy opcji gry
 * @param[in] _graph_it Wskaźnik na iterator przypisany do tablicy opcji grafiki
 * @param[in,out] _menu_active Wskaźnik na strukturę aktywnego menu
 * @param[in] _position Kliknięta pozycja w menu
 * @return Numer zmiany dokonanej przez kliknięcie.
 */
int Menu_Go_Previous(Iterator * _game_it,Iterator * _graph_it,Menu ** _menu_active,int _position)
{
	*_menu_active = (*_menu_active)->Parent;
	return MENU_NO_CHANGE;
}
/*!
 * Funkcja zmiany poziomu trudności.@n
 * @param[in] _game_it Wskaźnik na iterator przypisany do tablicy opcji gry
 * @param[in] _graph_it Wskaźnik na iterator przypisany do tablicy opcji grafiki
 * @param[in,out] _menu_active Wskaźnik na strukturę aktywnego menu
 * @param[in] _position Kliknięta pozycja w menu
 * @return Numer zmiany dokonanej przez kliknięcie.
 */
int Menu_Level_Change(Iterator * _game_it,Iterator * _graph_it,Menu ** _menu_active,int _position)
{
	int temp;
	char buffer[STRING_SIZE+1];
	
	Find(_game_it,"LEVEL");
	temp = Get_ValueI(_game_it);
	temp++;
	if(temp > AI_HARD)
		temp = AI_EASY;
	Set_Value(_game_it,&temp);
	strcpy(buffer,"Poziom trudnosci: ");
	switch(temp){
		case AI_EASY:
			sprintf(buffer,"%s latwy",buffer);
		break;
		case AI_MEDIUM:
			sprintf(buffer,"%s sredni",buffer);
		break;
		case AI_HARD:
			sprintf(buffer,"%s trudny",buffer);
		break;
		default:
			sprintf(buffer,"%s blad!",buffer);
		break;
	}
	strcpy((*_menu_active)->Captions[_position],buffer);
	return MENU_NO_CHANGE;
}
/*!
 * Funkcja zmiany rozdzielczości.@n
 * @param[in] _game_it Wskaźnik na iterator przypisany do tablicy opcji gry
 * @param[in] _graph_it Wskaźnik na iterator przypisany do tablicy opcji grafiki
 * @param[in,out] _menu_active Wskaźnik na strukturę aktywnego menu
 * @param[in] _position Kliknięta pozycja w menu
 * @return Numer zmiany dokonanej przez kliknięcie.
 */
int Menu_Graph_Resolution_Change(Iterator * _game_it,Iterator * _graph_it,\
									Menu ** _menu_active,int _position)
{
	int width,height,i;
	const char * label = "Rozdzielczosc: ";
	char * buffer;
	//Rozdzielczości możliwe do wybrania
	static int Display[3][2] = {{640,480},{800,600},{1024,768}};
	
	Find(_graph_it,"FULLSCREEN");
	if(!Get_ValueB(_graph_it))
	{
		Find(_graph_it,"WIDTH");
		width = Get_ValueI(_graph_it);
		for(i = 0;i < 3;i++)
			if(width == Display[i][0])
				break;
		++i;
		i %= 3;
		width = Display[i][0];
		Set_Value(_graph_it,(void*)&width);
		Find(_graph_it,"HEIGHT");
		height = Display[i][1];
		Set_Value(_graph_it,(void*)&height);
		buffer = (char*) malloc(sizeof(*buffer)*(STRING_SIZE+1));
		sprintf(buffer,"%s %dx%d",label,width,height);
		strcpy((*_menu_active)->Captions[_position],buffer);
		free(buffer);
		return MENU_GRAPH_CHANGE_RES;
	}
	//niemożliwa zmiana rozdzielczości w trybie pełnoekranowym
	else
		return MENU_NO_CHANGE;
}
/*!
 * Funkcja uruchomienia/wyłączenia renderingu w trybie pełnoekranowym.@n
 * @param[in] _game_it Wskaźnik na iterator przypisany do tablicy opcji gry
 * @param[in] _graph_it Wskaźnik na iterator przypisany do tablicy opcji grafiki
 * @param[in,out] _menu_active Wskaźnik na strukturę aktywnego menu
 * @param[in] _position Kliknięta pozycja w menu
 * @return Numer zmiany dokonanej przez kliknięcie.
 */
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
	return MENU_GRAPH_CHANGE_FULL;
}
/*!
 * Funkcja zmiany liczby graczy.@n
 * @param[in] _game_it Wskaźnik na iterator przypisany do tablicy opcji gry
 * @param[in] _graph_it Wskaźnik na iterator przypisany do tablicy opcji grafiki
 * @param[in,out] _menu_active Wskaźnik na strukturę aktywnego menu
 * @param[in] _position Kliknięta pozycja w menu
 * @return Numer zmiany dokonanej przez kliknięcie.
 */
int Menu_Game_Number_Change(Iterator * _game_it,Iterator * _graph_it,\
								Menu ** _menu_active,int _position)
{
	int temp,i;
	char buffer[STRING_SIZE+1];
	Find(_game_it,"NUMBER_OF_PLAYERS");
	temp = Get_ValueI(_game_it);
	temp++;
	if(temp > MAX_PLAYERS)
	{
		temp = MIN_PLAYERS;
		Set_Value(_game_it,&temp);
		for(i = MIN_PLAYERS+3;i < MAX_PLAYERS+3;i++)
			strcpy((*_menu_active)->Captions[i],"");
	}
	else
	{
		Set_Value(_game_it,&temp);
		sprintf(buffer,"PLAYER%d_AI",temp-1);
		Find(_game_it,buffer);
		if(!Get_ValueB(_game_it))
			sprintf(buffer,"Gracz %d: Czlowiek",temp);
		else
			sprintf(buffer,"Gracz %d: Komputer",temp);
		strcpy((*_menu_active)->Captions[temp+2],buffer);
	}
	sprintf(buffer,"Liczba graczy: %d",temp);
	strcpy((*_menu_active)->Captions[_position],buffer);
	return MENU_NO_CHANGE;
}
/*!
 * Funkcja zmiany typu gracza.@n
 * @param[in] _game_it Wskaźnik na iterator przypisany do tablicy opcji gry
 * @param[in] _graph_it Wskaźnik na iterator przypisany do tablicy opcji grafiki
 * @param[in,out] _menu_active Wskaźnik na strukturę aktywnego menu
 * @param[in] _position Kliknięta pozycja w menu
 * @return Numer zmiany dokonanej przez kliknięcie.
 */
int Menu_Game_Player_Change(Iterator * _game_it,Iterator * _graph_it,\
								Menu ** _menu_active,int _position)
{
	int temp;
	char buffer[STRING_SIZE+1];
	
	sprintf(buffer,"PLAYER%d_AI",_position-3);
	Find(_game_it,buffer);
	if(Get_ValueB(_game_it))
	{
		temp = 0;
		sprintf(buffer,"Gracz %d: Czlowiek",_position-2);
	}
	else
	{
		temp = 1;
		sprintf(buffer,"Gracz %d: Komputer",_position-2);
	}
	Set_Value(_game_it,&temp);
	strcpy((*_menu_active)->Captions[_position],buffer);
	return MENU_NO_CHANGE;
}
/*!
 * Funkcja włączająca rysowanie menu.@n
 * @param[in] _graph_it Wskaźnik na iterator przypisany do tablicy opcji grafiki
 * @param[in] _menu Wskaźnik na strukturę rysowanego menu
 */
void Menu_Draw(Iterator * _graph_it,Menu * _menu)
{
	int i;
	char * buffer;
	
	buffer = (char*)malloc(sizeof(*buffer)*(STRING_SIZE+1));
    for(i = 0;i < _menu->Number_of_Positions;i++)
	{
		//gra nie rysuje pustych pozycji w menu
		if(strcmp(_menu->Captions[i],""))
		{
			sprintf(buffer,"MENU_TEXT_%d",i);
			Text_Draw(0.8f,(float)(0.1+0.05*i),200+i,(void*)FONT1,\
				TEXT_CENTER,buffer,_menu->Captions[i]);
		}
	}
	free(buffer);
    Set_Change();
}
/*!
 * Funkcja wyłączająca rysowanie menu.@n
 * @param[in] _menu Wskaźnik na strukturę aktywnego menu
 */
void Menu_Clean_Drawing(Menu *_menu)
{
	int i;
	char * buffer;
	buffer = malloc(sizeof(*buffer)*(STRING_SIZE+1));
	for(i = 0;i < _menu->Number_of_Positions;i++){
		sprintf(buffer,"MENU_TEXT_%d",i);
		Text_Remove(buffer);
	}
	free(buffer);
	Set_Change();
}
/*!
 * Funkcja zwalniająca zaalokowaną pamięć.@n
 * @param[in] _menu Wskaźnik na strukturę menu
 */
void Menu_Free(Menu * _menu)
{
	int i;
	//rekurencyjne zwalnianie pamięci w wszystkich podmenu
	for(i = 0;i < _menu->Number_of_Positions;i++){
		if(_menu->Children[i])
			Menu_Free(_menu->Children[i]);
	}
	for(i = 0;i < _menu->Number_of_Positions;i++)
		free(_menu->Captions[i]);
	free(_menu->Captions);
	free(_menu->Children);
	free(_menu->Action);
	free(_menu);
}
/*!
 * Funkcja inicjalizująca menu.@n
 * @param[out] _main Wskaźnik do którego zostanie przypisane stworzone menu
 * @param[in] _game_it Wskaźnik na iterator przypisany do tablicy opcji gry
 * @param[in] _graph_it Wskaźnik na iterator przypisany do tablicy opcji grafiki
 */
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
	//główne menu
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
	ptr->Action = (int (**)(Iterator*,Iterator*,Menu **,int)) \
					malloc(ptr->Number_of_Positions*sizeof(*(ptr->Action)));
	//podmenu nr 1
	ptr->Children[0] = NULL;
	ptr->Action[0] = &Menu_New_Game;
	//podmenu nr 2
	ptr->Children[1] = (Menu*) malloc(sizeof(*ptr->Children[1]));
	ptr->Children[1]->Parent = ptr;
	Find(_game_it,"NUMBER_OF_PLAYERS");
	ptr->Children[1]->Number_of_Positions = Get_ValueI(_game_it) + 3;
	ptr->Children[1]->Captions = (char**) malloc((MAX_PLAYERS+3)*\
									sizeof(*(ptr->Children[1]->Captions)));
	ptr->Children[1]->Children = (Menu**) malloc((MAX_PLAYERS+3)*\
									sizeof(*(ptr->Children[1]->Children)));
	ptr->Children[1]->Action = (int (**)(Iterator*,Iterator*,Menu **,int)) \
	malloc((MAX_PLAYERS+3)*sizeof(*(ptr->Children[1]->Action)));
	
	for(i = 0;i < MAX_PLAYERS+3;i++)
		ptr->Children[1]->Captions[i] = (char*) malloc((STRING_SIZE+1)*\
									sizeof(ptr->Children[1]->Captions[i]));
	Find(_game_it,"LEVEL");
	strcpy(buffer,"Poziom trudnosci: ");
	switch(Get_ValueI(_game_it)){
		case AI_EASY:
			sprintf(buffer,"%s latwy",buffer);
		break;
		case AI_MEDIUM:
			sprintf(buffer,"%s sredni",buffer);
		break;
		case AI_HARD:
			sprintf(buffer,"%s trudny",buffer);
		break;
		default:
			sprintf(buffer,"%s blad!",buffer);
		break;
	}
	strcpy(ptr->Children[1]->Captions[0],buffer);
	ptr->Children[1]->Action[0] = &Menu_Level_Change;
	ptr->Children[1]->Children[0] = NULL;
	sprintf(buffer,"Liczba graczy: %d",ptr->Children[1]->Number_of_Positions-3);
	strcpy(ptr->Children[1]->Captions[1],buffer);
	ptr->Children[1]->Action[1] = &Menu_Game_Number_Change;
	ptr->Children[1]->Children[1] = NULL;
	strcpy(ptr->Children[1]->Captions[2],"Wroc");
	ptr->Children[1]->Action[2] = &Menu_Go_Previous;
	ptr->Children[1]->Children[2] = NULL;
	for(i = 0;i < ptr->Children[1]->Number_of_Positions-3;i++){
		sprintf(buffer,"PLAYER%d_AI",i);
		Find(_game_it,buffer);
		if(Get_ValueB(_game_it))
			sprintf(buffer,"Gracz %d: Komputer",i+1);
		else
			sprintf(buffer,"Gracz %d: Czlowiek",i+1);
		strcpy(ptr->Children[1]->Captions[i+3],buffer);
		ptr->Children[1]->Children[i+3] = NULL;
		ptr->Children[1]->Action[i+3] = &Menu_Game_Player_Change;
	}
	for(i = ptr->Children[1]->Number_of_Positions;i < MAX_PLAYERS+3;i++){
		ptr->Children[1]->Children[i] = NULL;
		ptr->Children[1]->Action[i] = &Menu_Game_Player_Change;
		strcpy(ptr->Children[1]->Captions[i],"");
	}
	ptr->Children[1]->Number_of_Positions = MAX_PLAYERS+3;
	ptr->Action[1] = &Menu_Go_Next;
	//podmenu nr 3
	ptr->Children[2] = (Menu*) malloc(sizeof(*ptr->Children[2]));
	ptr->Children[2]->Parent = ptr;
	ptr->Children[2]->Number_of_Positions = 3;
	ptr->Children[2]->Captions = (char**) malloc(ptr->Children[2]->Number_of_Positions*\
									sizeof(*(ptr->Children[2]->Captions)));
	ptr->Children[2]->Children = (Menu**) malloc(ptr->Children[2]->Number_of_Positions*\
									sizeof(*(ptr->Children[2]->Children)));
	ptr->Children[2]->Action = (int (**)(Iterator*,Iterator*,Menu **,int)) \
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
		sprintf(buffer,"%s: TAK",fullscr_label);
	else
		sprintf(buffer,"%s: NIE",fullscr_label);
	strcpy(ptr->Children[2]->Captions[1],buffer);
	strcpy(ptr->Children[2]->Captions[2],"Wroc");
	
	for(i = 0;i < ptr->Children[2]->Number_of_Positions;i++)
		ptr->Children[2]->Children[i] = NULL;
	ptr->Children[2]->Action[0] = &Menu_Graph_Resolution_Change;
	ptr->Children[2]->Action[1] = &Menu_Graph_Fullscr_Change;
	ptr->Children[2]->Action[2] = &Menu_Go_Previous;
	ptr->Action[2] = &Menu_Go_Next;
	//podmenu nr 3
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
/*!
 * Główna wewnętrzna funkcja menu.@n
 * @param[in] _game_it Wskaźnik na iterator przypisany do tablicy opcji gry
 * @param[in] _graph_it Wskaźnik na iterator przypisany do tablicy opcji grafiki
 * @param[in] _m_event Wskaźnik na strukturę zdarzenia myszy
 * @param[in] flag Flaga używana do wyboru działania funkcji
 */
void _Menu(Iterator * _game_it,Iterator * _graph_it,Mouse_Action * _m_event,int * flag)
{
	static Iterator * Game_Iterator = NULL;
	static Iterator * Graph_Iterator = NULL;
	static Menu * Main;
	static Menu * Active_Menu;
	int i;
	//inicjalizacja danych statycznych oraz samego menu
	if(_game_it)
	{
		Game_Iterator = _game_it;
		Graph_Iterator = _graph_it;
		Menu_Init(&Main,_game_it,_graph_it);
		Active_Menu = Main;
	}
	//analiza kliknięca w menu
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
	//czyszczenie pamięci
	else if(*flag == -1)
	{
		Menu_Free(Main);
	}
	else
	{
		//włączenie menu
		if(*flag)
			Menu_Draw(Graph_Iterator,Active_Menu);
		//wyłączenie menu
		else
			Menu_Clean_Drawing(Active_Menu);
	}
}
