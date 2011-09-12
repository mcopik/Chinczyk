/*!
 * @file
 * Zawiera implementację prostego algorytmu "sztucznej inteligencji".@n
 * Losowość gry jest bardzo duża, algorytm stara się głównie utrzymać stabilną pozycję na planszy(minimum dwa pionki).@n
 * Poziom trudności gry decyduje o agresywności komputera.@n
 * @par Chińczyk
 * @author Marcin Copik
 * @date 2011.09.10
 * @version 1.0
*/
#ifndef AI_H
#define	AI_H
#include "input.h"
#include "fifo.h"
/*!
 * Współczynnik agresywności komputera - poziom łatwy.@n
 */
#define LEVEL_EASY 0.25f
/*!
 * Współczynnik agresywności komputera - poziom średni.@n
 */
#define LEVEL_MEDIUM 0.5f
/*!
 * Współczynnik agresywności komputera - poziom trudny.@n
 */
#define LEVEL_HARD 0.75f
/*!
 * Wyliczenie obejmujące poziomy trudności gry.@n
 */
enum AI_Levels{
	AI_EASY,	//!< Łatwy
	AI_MEDIUM,	//!< Średni
	AI_HARD	//!< Trudny
};
/*!
 * Struktura przechowująca dane niezbędne algorytmowi gry komputera.@n
 */
typedef struct{
	Player * Players;	//!< Wskaźnik na tablicę graczy
	int Number_of_Players;	//!< Liczba graczy
	FIFO * Randomized;	//!< Kolejka wylosowanych wartości
	int * Available_Move;	//!< Wskaźnik na zmienną przechowującą informację o możliwym ruchu pionka
	int * Active_Pawns;	//!< Wskaźnik na tablicę przechowująca informacje o możliwych ruchach pionków
	float Level;	//!< Poziom trudności gry
} AI_Info;
/*!
 * Funkcja inicjalizująca algorytm.@n
 * @param[in] _randomized Wskaźnik na kolejkę wylosowanych wartości
 * @param[in] _players Wskaźnik na tablicę graczy
 * @param[in] _a_pawns Wskaźnik na tablicę aktywnych pionków
 * @param[in] _a_move Wskaźnik na zmienną możliwego ruchu
 * @param[in] _number_of_players Liczba graczy
 * @param[in] _level Poziom trudności
 */
void AI_Init(FIFO * _randomized,Player * _players,int * _a_pawns,int * _a_move,\
	int _number_of_players, int _level);
/*!
 * Funkcja analizująca sytuację w grze i podejmująca decyzję o wybraniu pionka.@n
 * Funkcja przyjmuje jako argument wskaźnik na wskaźnik, pod którym zostanie zapisane zdarzenie myszy.@n
 * Realizacja tego zdarzenia wykonuje zaplanowany ruch.
 * @param[out] _m_event Wskaźnik na zdarzenie myszy
 * @param[in] _active_player Aktywny gracz
 */
void AI_Process_Select(Mouse_Action ** _m_event,int _active_player);
/*!
 * Funkcja analizująca sytuację w grze i podejmująca decyzję o ruchu pionkiem.@n
 * Funkcja przyjmuje jako argument wskaźnik na wskaźnik, pod którym zostanie zapisane zdarzenie myszy.@n
 * Realizacja tego zdarzenia wykonuje zaplanowany ruch.
 * @param[out] _m_event Wskaźnik na zdarzenie myszy
 */
void AI_Process_Decision(Mouse_Action ** _m_event);
#endif