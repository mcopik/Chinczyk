/*!
 * @file menu.h
 * Zawiera definicje struktury menu oraz wszystkich funkcji używanych do obsługi menu w grze.@n
 * Menu do rysowania wykorzystuje obsługę struktur Text zaimplementowaną w graphic.@n
 * @par Chińczyk
 * @author Marcin Copik
 * @date 2011.09.10
 * @version 1.0
*/
#ifndef MENU_H
#define	MENU_H
#include "array.h"
#include "input.h"
typedef struct Menu_Struct Menu;
/*!
 * Główna struktura menu.@n
 */
struct Menu_Struct{
	char ** Captions;	//!< Tablica wyświetlanych nazw pozycji
	Menu ** Children;	//!< Tablica wskaźników na podmenu
	int (**Action)(Iterator*,Iterator*,Menu **,int);	//!< Tablica wskaźników na funkcje odpowiedzialne za kliknięcie pozycji
	int Number_of_Positions;	//!< Liczba pozycji w menu
	Menu * Parent;	//!< Wskaźnik na menu nadrzędne
};
/*!
 * Wyliczenie obejmujące wartości zwracane przez menu po analizie kliknięcia.@n
 */
enum Menu_Return_Values{
	MENU_NO_CHANGE = 0,	//!< Brak zmiany w grzes
	MENU_NEW_GAME,	//!< Uruchomienie nowej gry
	MENU_CLOSE,	//!< Zamknięcie gry
	MENU_GRAPH_CHANGE_RES,	//!< Zmiana rozdzielczości
	MENU_GRAPH_CHANGE_FULL,	//!< Zmiana opcji pełnego ekranu
};
/*!
 * Funkcja dokonuje konfiguracji menu.@n
 * Jednorazowe wywołanie jest warunkiem koniecznym i wystarczającym do prawidłowej pracy menu w grze.@n
 * @param[in] _game_it Iterator przypisany do tablicy z opcjami gry
 * @param[in] _graph_it Iterator przypisany do tablicy z opcjami grafiki
 */
void Menu_Config(Iterator * _game_it,Iterator * _graph_it);
/*!
 * Funkcja aktywuje wyświetlanie menu.@n
 */
void Menu_Active();
/*!
 * Funkcja deaktywuje wyświetlanie menu.@n
 */
void Menu_Disactive();
/*!
 * Funkcja niszczy stworzone menu i zwalnia pamięć.@n
 */
void Menu_Clean();
/*!
 * Funkcja dokonuje operacji związanych z kliknięciem pozycji w menu.@n
 * @param[in] _m_event Wskaźnik na strukturę kliknięcia myszy
 * @return Numer zmiany dokonanej przez kliknięcie.
 */
int Menu_Click(Mouse_Action * _m_event);
/*!
 * Funkcja tworzy menu.
 * @param[out] _main Wskaźnik, do którego zostanie przypisany adres stworzonego menu
 * @param[in] _game_it Iterator przypisany do tablicy z opcjami gry
 * @param[in] _graph_it Iterator przypisany do tablicy z opcjami grafiki
 */
void Menu_Init(Menu ** _main,Iterator * _game_it,Iterator * _graph_it);
#endif