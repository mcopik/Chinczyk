/*!
 * @file game.h
 * Zawiera interfejs funkcji podstawowych gry, w tym główną pętlę.@n
 * Funkcje zaimplementowane w tym pliku są odpowiedzialne za konfigurację gry oraz prawidłowe działanie.@n
 * @par Chińczyk
 * @author Marcin Copik
 * @date 2011.09.10
 * @version 1.0
*/
#ifndef GAME_H
#define	GAME_H
#include "array.h"
#include "input.h"
#include "fifo.h"
/*!
 * Numer przycisku menu.@n
 */
#define MENU_BUTTON_HIT 230
/*!
 * Argument uruchamiający plik z domyślnymi opcjami gry.@n
 */
#define DEFAULT_ARGUMENT "--default"
/*!
 * Makro realizujące przerwanie funkcji i wyświetlenie komunikatu o błędzie.@n
 * @param[in] ret Zwracana wartość
 * @param[in] ... Komunikat w postaci dla funkcji printf z biblioteki standardowej
 */
#define ERROR_MACRO(ret,...)		\
{							\
	printf(__VA_ARGS__);	\
	return ret;				\
}			
/*!
 * Makro realizujące wyświetlenie komunikatu ostrzeżenia.@n
 * @param[in] ... Komunikat w postaci dla funkcji printf z biblioteki standardowej
 */	
#define WARNING(...)		\
	printf(__VA_ARGS__);
/*!
 * Makro realizujące wyświetlenie komunikatu o uruchomieniu funkcji lub narzędzia niedostępnego w tej wersji gry.@n
 */		
#define NOT_IMPLEMENTED		\
{							\
	printf("Feature not implemented yet\n"); \
	return;					\
}
/*!
 * Ścieżka do pliku opcji.@n
 */
#define PATH "options.txt"
/*!
 * Liczba pionków.@n
 */
#define NUMBER_OF_PAWNS 4
/*!
 * Maksymalna liczba graczy.@n
 */
#define MAX_PLAYERS 6
/*!
 * Minimalna liczba graczy.@n
 */
#define MIN_PLAYERS 4
/*!
 * Liczba pól do przejścia na planszy przypadająca na każdego gracza.@n
 */
#define NUMBER_OF_FIELDS_PER_PLAYER 8
/*!
 * Czcionka numer 1.@n
 */
#define FONT1 GLUT_BITMAP_9_BY_15
/*!
 * Czcionka numer 2.@n
 */
#define FONT2 GLUT_BITMAP_TIMES_ROMAN_24
/*!
 * Czcionka numer 3.@n
 */
#define FONT3  GLUT_STROKE_MONO_ROMAN
/*!
 * Nazwa napisu wyświetlającego wyniki rzutu kością.@n
 */
#define DRAW_MSG "Draw_Msg"
/*!
 * Nazwa napisu wyświetlającego imię gracza.@n
 */
#define NAME_MSG "Name_Msg"
/*!
 * Nazwa napisu wyświetlającego liczbę klatek na sekundę.@n
 */
#define FPS_MSG "FPS_Msg"
/*!
 * Nazwa napisu wyświetlającego napis Menu.@n
 */
#define MENU_MSG "Menu_Msg"
/*!
 * Nazwa napisu wyświetlającego komunikat o zwycięstwie.@n
 */
#define WIN_MSG "Win_Msg"
/*!
 * Tekst napisu Menu.@n
 */
#define MENU_TEXT "Menu"
/*!
 * Tekst poprzedzający liczbę klatek na sekundę.@n
 */
#define FPS_TEXT "FPS: "
/*!
 * Tekst poprzedzający wyniki rzutu kością.@n
 */
#define DRAWING_TEXT "Wylosowano:"
/*!
 * Kod tworzący napis wyświetlający nazwę gracza.@n
 */
#define TEXT_DRAW_PLAYER 									\
Text_Draw(0.05f,0.1f,255,(void*)FONT1,TEXT_NORMAL,			\
			NAME_MSG,"%s",Players[Active_Player].Name);
/*!
 * Kod tworzący napis wyświetlający liczbę klatek na sekundę.@n
 */
#define TEXT_DRAW_FPS 										\
Text_Draw(0.05f,0.05f,255,(void*)FONT1,TEXT_NORMAL,			\
			FPS_MSG,"%s %d",FPS_TEXT,FPS_Counter);
/*!
 * Kod tworzący napis wyświetlający wynik rzutu kością.@n
 */
#define TEXT_DRAW_RAND(buffer,number,tab) 					\
buffer = (char *) malloc(sizeof(*buffer)*STRING_SIZE*2);	\
strcpy(buffer,DRAWING_TEXT);								\
for(i = 0;i < number;i++){									\
	sprintf(buffer,"%s %d",buffer,tab[i]);					\
}															\
Text_Draw(0.05f,0.15f,255,(void*)FONT1,						\
			TEXT_NORMAL,DRAW_MSG,"%s",buffer);				\
free(buffer);
/*!
 * Kod tworzący napis wyświetlający napis Menu.@n
 */
#define TEXT_DRAW_MENU_BUTTON								\
Text_Draw(0.8f,0.05f,MENU_BUTTON_HIT,						\
			(void*)FONT2,TEXT_CENTER,MENU_MSG,"%s",MENU_TEXT);		
/*!
 * Wyliczenie obejmujące stany gry.@n
 */
enum Loop_States{
	LOOP_CONFIG,	//!< Konfiguracja gry
	LOOP_START,	//!< Start gry
	LOOP_MENU,	//!< Wyświetlanie menu
	LOOP_INGAME_MENU,	//!< Menu w grze
	LOOP_WAIT,	//!< Oczekiwanie na losowanie
	LOOP_RANDOMIZE,	//!< Losowanie
	LOOP_WAIT_SELECT,	//!< Oczekiwanie na wybranie pionka
	LOOP_WAIT_DEC,	//!< Oczekiwanie na decyzję o ruchu
	LOOP_NEXT_PLAYER,	//!< Zmiana gracza na następnego
	LOOP_QUIT	//!< Koniec gry
};
/*!
 * Wyliczenie obejmujące typy kontroli gracza(gracz/komputer).@n
 */
enum Player_Types{
	PLAYER_HUMAN,	//!< Człowiek
	PLAYER_AI	//!< Komputer
};
/*!
 * Wyliczenie obejmujące argumenty dla funkcji pętli.@n
 */
enum Loop_Operations{
	GAME_INIT,	//!< Inicjalizacja gry
	GAME_INIT_DEF,	//!< Inicjalizacja gry z domyślnymi argumentami
	GAME_LOOP,	//!< Kontynuowani pętli
	GAME_CLOSE	//!< Zakończenie gry
};
/*!
 * Wyliczenie obejmujące argumenty dla funkcji kontroli zmiany gry.@n
 */
enum Change_Operations{
	CHANGE_SET, //!< Ustawienie nowej wartości zmiennej stanu
	CHANGE_CHECK	//!< Pobranie wartości zmiennej stanu
};
/**
 * Struktura gracza.@n
 */
typedef struct{
	char * Name;	//!< Nazwa gracza
	float Color[3];	//!< Kolor gracza
	int Position[NUMBER_OF_PAWNS];	//!< Pozycje pionków na planszy
	int Type;	//!< Typ gracza
} Player;
/*!
 * Funkcja tworząca gracza.@n
 * @param[in,out] _player Wskaźnik na strukturę gracza, do której zostaną zapisane dane
 * @param[in] _name Nazwa gracza
 * @param[in] _colors Wskaźnik na tablicę kolorów
 * @param[in] _type Typ gracza
 */
void Create_Player(Player * _player,const char * _name,float * _colors,int _type);
/*!
 * Funkcja inicjalizująca pozycje pionków.@n
 * @param[in,out] _player Wskaźnik na strukturę gracza, do której zostaną zapisane dane
 * @param[in] _number_of_player Liczba graczy
 * @param[in] _players_number Numer gracza
 */
void Set_Positions(Player * _player,int _number_of_player,int _players_number);
/*!
 * Funkcja czyszcząca zaalokowaną pamięć przez strukturę gracza.@n
 * @param[in] _player Wskaźnik na strukturę gracza
 */
void Delete_Player(Player * _player);
/*!
 * Funkcja ustawiająca stan sygnalizujący zmianę w grze i konieczność renderingu.@n
 */
void Set_Change();
/*!
 * Funkcja sprawdzająca, czy zaszła zmiana w grze i zachodzi konieczność ponownego renderingu.@n
 */
int Check_Change();
/*!
 * Funkcja potęgująca.@n
 * @param[in] _a Podstawa
 * @param[in] _b Wykładnik
 * @return Podstawa^Wykładnik
 */
int POW(int _a, int _b);
/*!
 * Funkcja tworząca tablicę z domyślnymi ustawieniami gry.@n
 * @return Tablica z opcjami
 */
Array * Default_Game_Options();
/*!
 * Funkcja tworząca tablicę z domyślnymi ustawieniami grafiki.@n
 * @return Tablica z opcjami
 */
Array * Default_Graphic_Options();
/*!
 * Funkcja uruchamiająca kolejną iterację pętli.@n
 */
void Process();
/*!
 * Funkcja inicjalizująca pętlę gry.@n
 * @param[in] _default Zmienna domyślnych opcji gry
 */
void Init_Process(int _default);
/*!
 * Funkcja zamykająca pętlę gry.@n
 */
void Close_Game();
/*!
 * Funkcja sprawdzająca czy jest możliwy ruch.@n
 * @param[in] _position Pozycja na planszy
 * @param[in] _value Ilość wyrzuconych oczek
 * @param[in] _number_of_players Liczba graczy
 * @param[in] _player_number Numer gracza
 * @return Nowa pozycja na planszy, jeśli jest możliwy ruch; w przeciwnym wypadku -1
 */
int Check_Move(int _position,int _value,int _number_of_players,int _player_number);
/*!
 * Funkcja sprawdzająca czy pole, na które jest wykonywany ruch, jest zajęte.@n
 * @param[in] _players Tablica z strukturami graczy
 * @param[in] _move Pozycja po ruchu
 * @param[in] _active_player Numer gracza
 * @param[in] _number_of_players Liczba graczy
 * @return 10*Numer gracza + Numer pionka + 1, jeśli pole jest zajęte przez innego gracza; 0, jeśli pole jest puste; -1, jeśli pole jest zajęte przez inny pionek gracza
 */
int Check_Occupied(Player * _players, int _move, int _active_player, int _number_of_players);
/*!
 * Funkcja sprawdzająca czy wszystkie pionki są na polach końcowych.@n
 * @param[in] _player Wskaźnik na strukturę gracza
 * @param[in] _active_player Numer gracza
 * @param[in] _number_of_players Liczba graczy
 * @return 1, jeśli wszystkie pionki są w bazie końcowej; w przeciwnym wypadku - 0
 */
int Check_All_Base(Player * _player,int _active_player,int _number_of_players);
/*!
 * Funkcja znajdująca pierwszą wolną pozycję w bazie startowej.@n
 * @param[in] _player Wskaźnik na strukturę gracza
 * @param[in] _number_of_player Numer gracza
 * @param[in] _number_of_players Liczba graczy
 * @return Numer wolnej pozycji; -1 jeśli wszystkie pionki gracza są w bazie startowej
 */
int Find_First_Free(Player * _player,int _number_of_player,int _number_of_players);
/*!
 * Funkcja wyliczająca dystans do bazy końcowej.@n
 * @param[in] _position Pozycja na planszy
 * @param[in] _player_number Numer gracza
 * @param[in] _number_of_players Liczba graczy
 * @return Dystans do bazy końcowej; 0, jeśli pozycja należy do bazy startowej/końcowej
 */
int Get_Distance(int _position,int _player_number,int _number_of_players);
#endif
