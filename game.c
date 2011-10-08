/*!
 * @file game.c
 * Zawiera implementację funkcji podstawowych gry, w tym główną pętlę.@n
 * Funkcje zaimplementowane w tym pliku są odpowiedzialne za konfigurację gry oraz prawidłowe działanie.@n
 * @par Chińczyk
 * @author Marcin Copik
 * @date 2011.09.10
 * @version 1.0
*/

#include "game.h"
#include "graphic.h"
#include "array.h"
#include "input.h"
#include "fifo.h"
#include "ai.h"
#include "menu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifdef _WIN32
# include <GL/freeglut.h>
#else
# include <GL/glut.h>
#endif

int POW(int _a, int _b)
{	
		int i;
		int j;
		
		j = _a;
		if(_b == 0)
		return 1;
		for(i = 1;i < _b;i++)
			j *= _a;
		return j;
}

Array * Default_Game_Options()
{
	int i,temp;
	float temp2[3] = {1.0f,0.0f,0.0f};
	char buffer[STRING_SIZE+1],buffer2[STRING_SIZE+2];
	Array * array;
	//usunięcie starego pliku opcji
	remove(PATH);
	array = Create_Array();
	//liczba graczy
	temp = 4;
	Add_Element(array,"NUMBER_OF_PLAYERS",&temp,1,ARRAY_INTEGER);
	//poziom trudności
	temp = AI_EASY;
	Add_Element(array,"LEVEL",&temp,1,ARRAY_INTEGER);
	//gracz 1 - człowiek
	temp = 0;
	Add_Element(array,"PLAYER0_AI",&temp,1,ARRAY_BOOLEAN);
	sprintf(buffer,"Gracz %d",1);
	Add_Element(array,"PLAYER0_NAME",buffer,strlen(buffer)+1,ARRAY_CHAR);
	Add_Element(array,"PLAYER0_COLOR",&temp2,3,ARRAY_FLOAT);
	temp = 1;
	//pozostali gracze = komputera
	for(i = 1;i < MAX_PLAYERS;i++){
		sprintf(buffer,"PLAYER%d_AI",i);
		Add_Element(array,buffer,&temp,1,ARRAY_BOOLEAN);
		sprintf(buffer,"PLAYER%d_NAME",i);
		sprintf(buffer2,"Gracz %d",i+1);
		Add_Element(array,buffer,buffer2,strlen(buffer2)+1,ARRAY_CHAR);
		if(i < 4)
			temp2[i-1] = 0.43f;
		else
			temp2[i-3] = 0.6f;
		sprintf(buffer,"PLAYER%d_COLOR",i);
		Add_Element(array,buffer,&temp2,3,ARRAY_FLOAT);
	}
	return array;
}

Array * Default_Graphic_Options()
{
	int temp;
	Array * array;
	//rozdzielczość 800x600
	//renderowanie w oknie
	array = Create_Array();
	temp = 800;
	Add_Element(array,"WIDTH",&temp,1,ARRAY_INTEGER);
	temp = 600;
	Add_Element(array,"HEIGHT",&temp,1,ARRAY_INTEGER);
	Add_Element(array,"LABEL","Chinczyk",strlen("Chinczyk")+1,ARRAY_CHAR);
	temp = 0;
	Add_Element(array,"FULLSCREEN",&temp,1,ARRAY_BOOLEAN);
	return array;
}

void Create_Player(Player * _player,const char * _name,float * _colors,int _type)
{
	_player->Name = (char*)malloc(sizeof(*_player->Name)*(STRING_SIZE+1));
	strcpy(_player->Name,_name);
	memcpy(_player->Color,_colors,sizeof(*_colors)*3);
	_player->Type = _type;
}

void Set_Positions(Player * _player,int _number_of_player,int _players_number)
{
	int temp;
	int i;
	
	temp = _number_of_player*NUMBER_OF_PAWNS*2;
	for(i=0;i < NUMBER_OF_PAWNS;i++){
		_player->Position[i] = temp++;
	}
}

void Delete_Player(Player * _player)
{
	free(_player->Name);
}
/*!
 * Funkcja inicjalizująca niezbędne struktury danymi startowymi.@n
 * @param[in] _it Wskaźnik na iterator opcji gry
 * @param[out] _players Wskaźnik pod którym zostanie zapisana tablicy graczy
 * @param[out] _fields Wskaźnik pod którym zostanie zapisana tablicy struktury pól
 * @param[out] _number_of_players Wskaźnik pod którym zostanie zapisana liczba graczy
 * @param[out] _first_move Wskaźnik pod którym zostanie zapisana tablicy bonusu startowego
 * @return Dystans do bazy końcowej; 0, jeśli pozycja należy do bazy startowej/końcowej
 */
void Init_Game(Iterator * _it,Player ** _players,Fields_Structure ** _fields,int * _number_of_players,int ** _first_move)
{
	int i;
	float * temp2;
	char * buffer;
	char * buffer2;
	//pobranie liczby graczy
	Find(_it,"NUMBER_OF_PLAYERS");
	*_number_of_players = Get_ValueI(_it);
	//tworzenie struktury pól
	*_fields = Fields_Generate(*_number_of_players);
	//tworzenie tablicy graczy
	*_players = malloc(*_number_of_players*sizeof(**_players));
	//tworzenie tablicy - 3 bonusowe ruchy na start
	*_first_move = malloc(sizeof(**_first_move)*(*_number_of_players));
	for(i = 0;i < *_number_of_players;i++)
		(*_first_move)[i] = 2;
	buffer = (char*)malloc(STRING_SIZE*sizeof(char));
	for(i = 0;i < *_number_of_players;i++)
	{
		sprintf(buffer,"PLAYER%d_COLOR",i);
		Find(_it,buffer);
		temp2 = Get_Value_ArrayF(_it);
		sprintf(buffer,"PLAYER%d_AI",i);
		Find(_it,buffer);
		if(Get_ValueB(_it))
		{
			sprintf(buffer,"PLAYER%d_NAME",i);
			Find(_it,buffer);
			buffer2 = Get_Value_ArrayC(_it);
			Create_Player(*_players,buffer2,temp2,PLAYER_AI);
		}
		else
		{
			sprintf(buffer,"PLAYER%d_NAME",i);
			Find(_it,buffer);
			buffer2 = Get_Value_ArrayC(_it);
			Create_Player(*_players,buffer2,temp2,PLAYER_HUMAN);
		}
		Set_Positions(*_players,i,*_number_of_players);
		(*_players)++;
		free(temp2);
		free(buffer2);
	}
	*_players -= *_number_of_players;
	free(buffer);
}
/*!
 * Wewnętrzna funkcja pętli gry.@n
 * @param[in] _type Argument startowy określający zachowanie funkcji
 */
void Main_Loop(int _type)
{
	static Fields_Structure * Fields = NULL;
	static Player * Players = NULL;
	static int Number_of_Players = 0;
	//aktualny stan gry
	static int Game_Status = 0;
	//aktualny gracz
	static int Active_Player = 0;
	//aktualnie zaznaczony pionek
	static int Selected_Pawn = -1;
	//możliwy ruch na pole
	static int Available_Move = -1;
	//aktywne pionki
	static int Active_Pawns[NUMBER_OF_PAWNS];
	//tablice opcji
	static Array * Game_Options = NULL;
	static Array * Graphic_Options = NULL;
	static Iterator * Game_Iterator = NULL;
	static Iterator * Graph_Iterator = NULL;
	//numer okna
	static int Window_Number;
	//flaga sygnalizująca możliwość następnego rzutu
	static int Next_Draw = 0;
	//poprzedni stan gry
	static int Previous_State = 0;
	//struktury zdarzeń
	static Mouse_Action * m_event;
	static Key_Action * k_event;
	//dane okna
	static int Width = 0,Height = 0;
	static char * Label;
	//kolejka z danymi rzutów
	static FIFO * Randomized = NULL;
	//wynik ostatniego losowania
	static int Last_Randomized = 1;
	//akumulator gromadzący czas od ostatniej klatki
	static int Accumulator = 0;
	//długość trwania jednej klatki
	static int Frame_Length = 0;
	//czas do obliczania FPS
	static int FPS_Time = 0;
	//licznik do obliczania FPS
	static int FPS_Counter = 0;
	//czas
	static int Time;
	//tablica pierwszego ruchu w grze
	static int * First_Move = NULL;
	//czas trwania pauzy(w klatkach)
	static int Delay = 0;
	//flaga zamykania gry
	static int Quit = 0;
	//flaga pełnego ekranu
	static int Fullscreen = 0;
	//flaga zwycięstwa
	static int Win = 0;
	int i,j;
	int Next_Time;
	int Buffer;
	int * tab;
	int temp,temp2;
	int flag = 0;
	char * buffer;
	
	if(Fullscreen == -1)
	{
		Change_Display(Width,Height);
		Set_Change();
		Draw_Render();
		Fullscreen = 0;
	}
	//wstępna inicjalizacja
	if(Game_Status == LOOP_CONFIG)
	{
		if(_type == GAME_INIT_DEF)
		{
			Game_Options = Default_Game_Options();
			Graphic_Options = Default_Graphic_Options();
		}
		else
		{
			Game_Options = Load("GAME_OPTIONS",PATH);
			Graphic_Options = Load("GRAPHIC_OPTIONS",PATH);
		}
		Game_Iterator = Create_Iterator(Game_Options);
		Graph_Iterator = Create_Iterator(Graphic_Options);
		Frame_Length = 1000/FPS;
		Time = glutGet(GLUT_ELAPSED_TIME);
		//podstawowe informacje o oknie
		Find(Graph_Iterator,"WIDTH");
		Width = Get_ValueI(Graph_Iterator);
		Find(Graph_Iterator,"HEIGHT");
		Height = Get_ValueI(Graph_Iterator);
		Find(Graph_Iterator,"LABEL");
		Label = Get_Value_ArrayC(Graph_Iterator);
		Find(Graph_Iterator,"FULLSCREEN");
		if(Get_ValueB(Graph_Iterator))
		{	
			Window_Number = Init_GL(Width,Height,Label,FULLSCREEN);
			Fullscreen = 1;
		}
		else
		{
			Window_Number = Init_GL(Width,Height,Label,NO_FULLSCREEN);
			Fullscreen = 0;
		}	
		//opcje gry
		Menu_Config(Game_Iterator,Graph_Iterator);
		Randomized = FIFO_Create();
		Text_Init(Width,Height);
		Game_Status = LOOP_START;
	}
	//zamykanie gry
	if(_type == GAME_CLOSE || Game_Status == LOOP_QUIT)
	{
		Game_Status = LOOP_QUIT;
		Next_Time = Frame_Length+Time;
	}
	//pobranie nowego czasu
	else
		Next_Time = glutGet(GLUT_ELAPSED_TIME);
	//różnica czasowa
	Buffer = Next_Time - Time;
	Time += Buffer;
	Accumulator += Buffer;
	FPS_Time += Buffer;
	//pętla gry uruchamiana gdy zamykamy
	//lub gdy minął czas trwania klatki
	while(Accumulator >= Frame_Length && !Quit){
		
		FPS_Counter++;
		//obliczanie FPS
		if(FPS_Time >= 1000)
		{
			FPS_Time -= 1000;
			TEXT_DRAW_FPS;
			FPS_Counter = 0;
		}
		Accumulator -= Frame_Length;
		//pauza
		if(Delay)
		{
			Delay--;
			Draw_Render();
			return;
		}
		//nie pobieramy zdarzeń, gdy gra dopiero ma się zacząć, następuje zmiana gracza albo gra jest zamykana
		if(Game_Status != LOOP_START && Game_Status != LOOP_NEXT_PLAYER && Game_Status != LOOP_QUIT)
		{
			//pobranie właściwego zdarzenia
			temp = Event_Get();
			if(temp == EVENT_MOUSE)
				Check_Mouse_Event(&m_event);
			else if(temp == EVENT_KEY)
				Check_Key_Event(&k_event);
			//obsługa jednego, jedynego zdarzenia klawiatury
			if(k_event)
			{
				if(k_event->Key == KEY_ESCAPE)
				{
					free(k_event);
					k_event = NULL;
					Game_Status = LOOP_QUIT;
				}
			}
			if(Game_Status != LOOP_MENU)
			{
				//obsługa kliknięcia na przycisk MENU podczas gry
				if(m_event)
				{	
					if(Find_Hit(MENU_BUTTON_HIT,m_event->Hits,m_event->Buffer))
					{
						Previous_State = Game_Status;
						Game_Status = LOOP_MENU;
						Menu_Active();
						Set_Change();
						free(m_event);
						m_event = NULL;
						return;
					}
				}
				//obsługa algorytmu gry komputera
				if(Players[Active_Player].Type == PLAYER_AI)
				{
					switch(Game_Status){
						case LOOP_WAIT:
							if(Next_Draw)
							{
								Game_Status = LOOP_RANDOMIZE;
								Previous_State = LOOP_WAIT;
								Delay = FPS/2;
							}
						break;
						case LOOP_WAIT_SELECT:
							if(Next_Draw)
							{	
								Game_Status = LOOP_RANDOMIZE;
								Previous_State = LOOP_WAIT;
								Delay = FPS/2;
							}
							else
							{
								AI_Process_Select(&m_event,Active_Player);
								Delay = FPS/2;
							}
						break;
						case LOOP_WAIT_DEC:
							AI_Process_Decision(&m_event);
						break;
					}
				}
			}
		}
		//rozpatrywanie kolejncyh stanów gry
		switch(Game_Status){
			//inicjalizacja wszystkich rzeczy niezbędnych do działania gry
			case LOOP_START:
				Init_Game(Game_Iterator,&Players,&Fields,&Number_of_Players,&First_Move);
				Draw_Init(Fields,Players,Number_of_Players,&Last_Randomized);
				srand(time(NULL));
				Find(Game_Iterator,"LEVEL");
				AI_Init(Randomized,Players,Active_Pawns,&Available_Move,Number_of_Players,\
							Get_ValueI(Game_Iterator));
				TEXT_DRAW_FPS;
				TEXT_DRAW_MENU_BUTTON;
				Menu_Active();
				Set_Change();
				Previous_State = LOOP_MENU;
				Game_Status = LOOP_MENU;
			break;
			case LOOP_MENU:
				if(m_event)
				{
					//powrót z pauzy, jeśli jest aktywna gra w tle
					if(Find_Hit(MENU_BUTTON_HIT,m_event->Hits,m_event->Buffer))
					{
						if(Previous_State != LOOP_MENU)
						{	
							Game_Status = Previous_State;
							Menu_Disactive();
							Set_Change();
						}
					}
					else
					{
						switch(Menu_Click(m_event)){
							//rozpoczęcie nowej gry
							case MENU_NEW_GAME:
								Find(Game_Iterator,"NUMBER_OF_PLAYERS");
								temp = Get_ValueI(Game_Iterator);
								if(temp != Number_of_Players || Win)
								{
									if(Fields)
										Fields_Close(Fields);
									for(i=0;i<Number_of_Players;i++)
										Delete_Player(&Players[i]);
									if(Players)
										free(Players);
									if(First_Move)
										free(First_Move);
									Init_Game(Game_Iterator,&Players,&Fields,&Number_of_Players,&First_Move);
									Draw_Init(Fields,Players,Number_of_Players,&Last_Randomized);
									Find(Game_Iterator,"LEVEL");
									AI_Init(Randomized,Players,Active_Pawns,&Available_Move,Number_of_Players,\
												Get_ValueI(Game_Iterator));
								}
								if(Win)
								{
									Win = 0;
									Text_Remove(WIN_MSG);
								}
								Menu_Disactive();
								TEXT_DRAW_PLAYER;
								TEXT_DRAW_RAND(buffer,0,tab);
								Set_Change();
								Game_Status = LOOP_WAIT;
							break;
							//zmiana rozdzielczości - następuje natychmiast
							case MENU_GRAPH_CHANGE_RES:
								Find(Graph_Iterator,"WIDTH");
								Width = Get_ValueI(Graph_Iterator);
								Find(Graph_Iterator,"HEIGHT");
								Height = Get_ValueI(Graph_Iterator);
								Change_Display(Width,Height);
								Set_Change();
							break;
							//tryb pełnoekranowy - zmiana następuje natychmiast
							case MENU_GRAPH_CHANGE_FULL:
								Find(Graph_Iterator,"FULLSCREEN");
								if(Get_ValueB(Graph_Iterator) != Fullscreen)
								{
									Fullscreen = !Fullscreen;
									if(Fullscreen)
										Enable_FullScr();
									else
									{
										Change_Display(800,600);
										Set_Change();
										Fullscreen = -1;
									}
								}
							break;
							//koniec gry
							case MENU_CLOSE:
								Game_Status = LOOP_QUIT;
							break;
						}
					}
				}
			break;
			//oczekiwanie na rzut kością
			case LOOP_WAIT:
				if(m_event)
				{
					//kliknięcie na kość
					if(Find_Hit(CUBE_HIT,m_event->Hits,m_event->Buffer))
					{	
						Game_Status = LOOP_RANDOMIZE;
						Previous_State = LOOP_WAIT;
					}
					//kliknięcie na MENU
					else if(Find_Hit(MENU_BUTTON_HIT,m_event->Hits,m_event->Buffer))
					{
						Game_Status = LOOP_MENU;
						Previous_State = LOOP_WAIT;
						Menu_Active();
					}
				}
			break;
			//rzut kością
			case LOOP_RANDOMIZE:
				temp = rand() % 6 + 1;
				FIFO_Push(Randomized,temp);
				Last_Randomized = temp;
				//nie był to pierwszy rzut kością, powrót do odpowiedniego stanu gry
				if(Previous_State == LOOP_WAIT_DEC || Previous_State == LOOP_WAIT_SELECT)
				{
					Game_Status = Previous_State;
				}
				else
				{
					FIFO_Get_All(Randomized,&temp,&tab);
					flag = 0;
					//analizowanie, czy jest możliwy ruch jakimkolwiek pionkiem
					for(i = 0;i < temp;i++){
						for(j=0;j < NUMBER_OF_PAWNS;j++){
							temp2 = Check_Move(Players[Active_Player].Position[j],tab[i],\
								Number_of_Players,Active_Player);
							if(temp2 >= 0 &&\
							Check_Occupied(Players,temp2,Active_Player,Number_of_Players) != -1)
							{
								Active_Pawns[j] = temp2;
								flag = 1;
							}
							else
								Active_Pawns[j] = -1;
						}
						if(flag)
						{
							for(j = 0;j < i;j++)
								FIFO_Pop(Randomized);
							Blink_Set_Pawn(Active_Player,Active_Pawns);
							Game_Status = LOOP_WAIT_SELECT;
							break;
						}
					}
					//jeśli ruch niemożliwy, przejście do następnego gracza...
					if(!flag)
					{
						//... chyba że mamy bonus w postaci wylosowania "6" lub pierwszych losowań w grze
						if(Last_Randomized == 6 || First_Move[Active_Player])
							Game_Status = LOOP_WAIT;
						else
							Game_Status = LOOP_NEXT_PLAYER;
					}
					free(tab);
				}
				//bonus dodatkowe losowania
				if(Last_Randomized == 6)
					Next_Draw = 1;
				//bonus startowy
				else if(First_Move[Active_Player] > 0)
				{	
					First_Move[Active_Player]--;
					Next_Draw = 1;
				}
				else
					Next_Draw = 0;
				FIFO_Get_All(Randomized,&temp,&tab);
				TEXT_DRAW_RAND(buffer,temp,tab);
				Set_Change();
				free(tab);
			break;
			case LOOP_WAIT_SELECT:
				if(m_event)
				{
					//ponowne losowanie, ale gdy jest to możliwe
					if(Next_Draw && Find_Hit(CUBE_HIT,m_event->Hits,m_event->Buffer))
					{	
						Game_Status = LOOP_RANDOMIZE;
						Previous_State = LOOP_WAIT_SELECT;
					}
					//kliknięcie MENU
					else if(Find_Hit(MENU_BUTTON_HIT,m_event->Hits,m_event->Buffer))
					{
						Game_Status = LOOP_MENU;
						Previous_State = LOOP_WAIT_SELECT;
						Menu_Active();
					}
					else
					{
						//sprawdzenie czy kliknięto pionek, który się może ruszyć
						for(i=0;i<NUMBER_OF_PAWNS;i++){
							if(Find_Hit(100+Active_Player*10+i,m_event->Hits,m_event->Buffer))
							{
								if(Active_Pawns[i] != -1){
									Selected_Pawn = i;
									Available_Move = Active_Pawns[i];
									Blink_Set_Field(Active_Player,Selected_Pawn,Available_Move);
									Game_Status = LOOP_WAIT_DEC;
								}
							}
						}
					}
				}
			break;
			//oczekiwanie na ruch zaznaczonym pionkiem
			case LOOP_WAIT_DEC:
				if(m_event)
				{
					//sprawdzenie, czy ruszono się na zaznaczone pole
					if(Find_Hit(Available_Move+1,m_event->Hits,m_event->Buffer))
					{
						temp = Check_Occupied(Players,Available_Move,Active_Player,Number_of_Players);
						//odesłanie zbitego pionka do domu
						if(temp > 0)
						{
							Players[(temp-1)/10].Position[(temp-1)%10] = Find_First_Free(Players,(temp-1)/10,Number_of_Players);
						}
						//właściwe przesunięcie pionka
						Players[Active_Player].Position[Selected_Pawn] = Available_Move;
						//czy wszystkie pionki w bazie?
						if(Check_All_Base(&Players[Active_Player],Active_Player,Number_of_Players))
						{	
							Delay = 2*FPS;
							Text_Draw(0.5,0.1,255,FONT2,TEXT_CENTER,WIN_MSG,\
									"Zwyciestwo: %s",Players[Active_Player].Name);
							Menu_Active();
							Disable_Blink();
							Set_Change();
							Game_Status = LOOP_MENU;
							Previous_State = LOOP_MENU;
							Win = 1;
							break;
						}
						Selected_Pawn = -1;
						Available_Move = -1;
						temp = FIFO_Pop(Randomized);
						FIFO_Get_All(Randomized,&temp,&tab);
						TEXT_DRAW_RAND(buffer,temp,tab);
						free(tab);
						//możemy losować, kolejka pusta
						if(FIFO_Check(Randomized) == -1 && Next_Draw)
						{
							Game_Status = LOOP_WAIT;
							Set_Change();
							Disable_Blink();
						}
						//kolejka pusta, nie możemy losować
						else if(FIFO_Check(Randomized) == -1)
							Game_Status = LOOP_NEXT_PLAYER;
						//kolejka nie jest pusta, następny ruch
						else
						{
							//pętla wykonywana tak długo, aż będzie możliwy ruch lub będzie pusta kolejka losowań
							flag = 0;
							while(!flag && FIFO_Check(Randomized) != -1){
								for(i=0;i < NUMBER_OF_PAWNS;i++){
									temp = Check_Move(Players[Active_Player].Position[i],FIFO_Check(Randomized), \
									Number_of_Players,Active_Player);
									if(temp >= 0 &&\
									Check_Occupied(Players,temp,Active_Player,Number_of_Players) != -1)
									{	
										Active_Pawns[i] = temp;
										flag = 1;
									}
									else
										Active_Pawns[i] = -1;
								}
								if(!flag)
									FIFO_Pop(Randomized);
							}
							if(!flag)
							{
								if(Next_Draw)
									Game_Status = LOOP_WAIT;
								else
									Game_Status = LOOP_NEXT_PLAYER;
							}
							else
							{
								Blink_Set_Pawn(Active_Player,Active_Pawns);
								Game_Status = LOOP_WAIT_SELECT;
							}
						}
					}
					//ponowne losowanie
					else if(Next_Draw && Find_Hit(CUBE_HIT,m_event->Hits,m_event->Buffer))
					{
						Previous_State = LOOP_WAIT_DEC;	
						Game_Status = LOOP_RANDOMIZE;
					}
					//kliknięcie MENU
					else if(Find_Hit(MENU_BUTTON_HIT,m_event->Hits,m_event->Buffer))
					{
						Game_Status = LOOP_MENU;
						Previous_State = LOOP_WAIT_DEC;
						Menu_Active();
					}
					else
					{
						//zmiana zaznaczonego pionka?
						for(i=0;i<NUMBER_OF_PAWNS;i++){
							if(Find_Hit(100+Active_Player*10+i,m_event->Hits,m_event->Buffer))
							{
								if(i != Selected_Pawn && Active_Pawns[i] != -1)
								{
									Selected_Pawn = i;
										
									Available_Move = Active_Pawns[i];
									Blink_Set_Field(Active_Player,Selected_Pawn,Available_Move);
								}
							}
						}
					}
				}
			break;
			//przejście do następnego gracza
			case LOOP_NEXT_PLAYER:
				if(Players[Active_Player].Type == PLAYER_AI)
				{
					++Active_Player;
					Active_Player %= Number_of_Players;
					//czyszczenie bufora zdarzeń
					//akcje, które miały miejsce podczas gry komputera
					//nie zajdą teraz
					if(Players[Active_Player].Type == PLAYER_HUMAN)
					{
						temp = Event_Get();
						while(temp != -1){
							if(temp == EVENT_MOUSE)
							{
								Check_Mouse_Event(&m_event);
								free(m_event);
								m_event = NULL;
							}
							else if(temp == EVENT_KEY)
							{
								Check_Key_Event(&k_event);
								free(k_event);
								k_event = NULL;
							}
							temp = Event_Get();
						}
					}
				}
				else
				{	
					++Active_Player;
					Active_Player %= Number_of_Players;
				}
				if(Players[Active_Player].Type == PLAYER_AI)
					Delay = FPS;
				TEXT_DRAW_PLAYER;
				TEXT_DRAW_RAND(buffer,0,tab);
				FIFO_Clean(Randomized);
				Game_Status = LOOP_WAIT;
				Next_Draw = 1;
				Set_Change();
				Disable_Blink();
			break;
			//koniec gry, czyszczenie pamięci
			case LOOP_QUIT:
				if(Fields)
					Fields_Close(Fields);
				for(i=0;i<Number_of_Players;i++)
					Delete_Player(&Players[i]);
				if(Players)
					free(Players);
				if(Label)
					free(Label);
				if(First_Move)
					free(First_Move);
				if(Randomized)
					FIFO_Delete(Randomized);
				Text_Clean();
				Menu_Clean();
				Delete_Iterator(Game_Iterator);
				Delete_Iterator(Graph_Iterator);
				Save(Game_Options,"GAME_OPTIONS",PATH);
				Save(Graphic_Options,"GRAPHIC_OPTIONS",PATH);
				Free_Array(Game_Options);
				Free_Array(Graphic_Options);
				Clean_Mouse_Event();
				Clean_Key_Event();
				Event_Clean();
				Quit = 1;
				//GAME_CLOSE, gdy GLUT wywołał funkcję zamykającą grę(zdarzenie ręcznego zamknięcia okna)
				if(_type != GAME_CLOSE)
					glutDestroyWindow(Window_Number);
			break;
		}
		if(m_event)
		{	
			free(m_event);
			m_event = NULL;
		}
		if(Game_Status != LOOP_QUIT)
		{
			Draw_Render();
		}
	}
}

void Process()
{
	Main_Loop(GAME_LOOP);
}

void Init_Process(int _default)
{
	if(_default)
		Main_Loop(GAME_INIT_DEF);
	else
		Main_Loop(GAME_INIT);
}
/*!
 * Wewnętrzna funkcja bufora sygnalizującego zmianę w grze.@n
 * @param[in] _type Argument określający zachowanie funkcji
 * @return Gdy żądano zwrotu wartości przechowywanej przez funkcję - zwraca 0 lub 1(nie było/bya zmiana w grze);w przeciwnym razie zwraca 0
 */
int _Change(int _type)
{
	static int Change = 0;
	
	if(_type == CHANGE_SET)
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
	_Change(CHANGE_SET);
}

int Check_Change()
{
	return _Change(CHANGE_CHECK);
}
		
int Check_Move(int _position,int _value,int _number_of_players,int _player_number)
{
	int shift = NUMBER_OF_PAWNS*_number_of_players*2;
	int base = _player_number*NUMBER_OF_FIELDS_PER_PLAYER + shift;
	int length = NUMBER_OF_FIELDS_PER_PLAYER*_number_of_players;
	int temp;

	if(_position < shift)
	{
		//pionek w bazie
		if(_position % (2*NUMBER_OF_PAWNS) < NUMBER_OF_PAWNS)
		{
			if(_value == 6)
				return shift + NUMBER_OF_FIELDS_PER_PLAYER*_player_number;
			else
				return -1;
		}
		//pionek na mecie
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
		//algorytm sprawdza czy pionek nie zbliża się do mety
		//pierwszy IF wynika z faktu, że położenie początkowe ma różne wartości, a pionki inne niż pierwszy
		//podczas gry w pewnym momencie przejdą na pole nr 1 gracza 1, czyli z pola ostatniego na pole pierwsze
		//co wiąże się z znacznie niższym numerem
		//zadaniem algorytmu jest zanalizować na podstawie numeru gracza, czy może się ruszyć
		//oraz określić jego nową pozycję
		if( _position - base >= 0)
		{	
			//pionek daleko od mety, może się ruszyć
			if( (_value+_position) - base < length )
				return ((_value+_position-shift) % length)+shift;
			else
			{
				//czy pionek zmieści się na mecie?
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

int Find_First_Free(Player * _player,int _number_of_player,int _number_of_players)
{
	int i,shift,tab[NUMBER_OF_PAWNS];

	for(i = 0;i < NUMBER_OF_PAWNS;i++)
		tab[i] = 0;
	shift = NUMBER_OF_PAWNS*2*_number_of_player;
	for(i = 0;i < NUMBER_OF_PAWNS;i++){
		//sprawdzenie czy i-ty pionek znajduje się w bazie
		//jeśli tak, to oznaczamy jego pozycję jako zajętą
		if(_player[_number_of_player].Position[i] >= shift &&\
		_player[_number_of_player].Position[i] < shift + NUMBER_OF_PAWNS)
			tab[_player[_number_of_player].Position[i]-shift] = 1;
	}
	for(i = 0;i < NUMBER_OF_PAWNS;i++)
		if(!tab[i])
			return shift + i;
	return -1;
}

int Get_Distance(int _position,int _player_number,int _number_of_players)
{
	int shift = NUMBER_OF_PAWNS*_number_of_players*2;
	int base = _player_number*NUMBER_OF_FIELDS_PER_PLAYER + shift;
	int length = NUMBER_OF_FIELDS_PER_PLAYER*_number_of_players;
	//baza lub meta
	if(_position < shift)
			return 0;
	//dwa różne położenia na planszy, patrz na funkcję Check_Move(...)
	if(_position >= base)
			return (base-shift) + length - (_position-shift+1);
	else
			return base - _position;
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
				//10*numer gracza + numer pionka + 1
				else
					return 10*i + j + 1;
			}
		}
	}
	return 0;
}

int Check_All_Base(Player * _player,int _active_player,int _number_of_players)
{
	int i;

	for(i = 0;i < NUMBER_OF_PAWNS;i++){
		//warunek prawdziwy, gdy pionek jest na planszy
		if(!(_player->Position[i] >= NUMBER_OF_PAWNS*(2*_active_player+1) &&\
			_player->Position[i] < NUMBER_OF_PAWNS*(2*_active_player+2)))
				return 0;
	}
	return 1;
}

void Close_Game()
{
	Main_Loop(GAME_CLOSE);
}
