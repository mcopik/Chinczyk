/*!
 * @file input.h
 * Zawiera interfejs funkcji i struktur do obsługi wejścia w postaci myszki i klawiatury.@n
 * @par Chińczyk
 * @author Marcin Copik
 * @date 2011.09.10
 * @version 1.0
*/
#ifndef INPUT_H
#define	INPUT_H
/*!
 * Rozmiar bufora nazw używanego przez OpenGL.@n
 */
#define BUFFER_SIZE 20
/*!
 * Rozmiar bufora zdarzeń myszy/klawiatury.@n
 */
#define EVENTS_BUFFER 5
/*!
 * Wielkość obszaru kliknięcia.@n
 */
#define PICK_SIZE 5.0f
/*!
 * Wartość klawisza "Escape".@n
 */
#define KEY_ESCAPE 27

/*!
 * Wyliczenie używane przez bufor zdarzeń.@n
 */
enum Even_Operations{
	EVENT_KEY = 0,	//!< Zdarzenie klawiatury
	EVENT_MOUSE,	//!< Zdarzenie myszy
	EVENT_SET,	//!< Ustawienie wartości w buforze zdań
	EVENT_GET,	//!< Pobranie wartości w buforze zdarzeń
	EVENT_CLEAN	//!< Wyczyszczenie bufora zdarzeń
};
/*!
 * Struktura zdarzenia - nacisnięcie klawisza.@n
 */
typedef struct{
	unsigned char Key;	//!< Numer klawisza
	int X;	//!< Pozycja na ekranie - X
	int Y;	//!< Pozycja na ekranie - Y
} Key_Action;
/*!
 * Struktura zdarzenia - kliknięcie myszą.@n
 */
typedef struct{
	int Button;	//!< Numer przycisku
	int X;	//!< Pozycja na ekranie - X
	int Y;	//!< Pozycja na ekranie - Y
	unsigned int Buffer[BUFFER_SIZE]; //!< Bufor nazw klikniętych elementów
	int Hits;	//!< Liczba klikniętych elementów
} Mouse_Action;
/*!
 * Funkcja używana przez GLUT do obsługi specjalnych klawiszy.@n
 * @param[in] _key Numer klawisza
 * @param[in] _x Pozycja myszy w oknie - X
 * @param[in] _y Pozycja myszy w oknie - Y
 */
void Special_Key_Pressed(int _key,int _x,int _y);
/*!
 * Funkcja używana przez GLUT do obsługi klawiszy.@n
 * @param[in] _key Numer klawisza
 * @param[in] _x Pozycja myszy w oknie - X
 * @param[in] _y Pozycja myszy w oknie - Y
 */
void Key_Pressed(unsigned char _key,int _x,int _y);
/*!
 * Funkcja używana przez GLUT do obsługi kliknięć myszy.@n
 * @param[in] _button Numer przycisku myszy
 * @param[in] _state Stan przycisku myszy
 * @param[in] _x Pozycja myszy w oknie - X
 * @param[in] _y Pozycja myszy w oknie - Y
 */
void Mouse_Event(int _button,int _state,int _x,int _y);
/*!
 * Funkcja sprawdza zdarzenia myszy.@n
 * @param[out] _pointer Wskaźnik, pod którym zostaje zapisany struktura zdarzenia myszy
 */
void Check_Mouse_Event(Mouse_Action ** _pointer);
/*!
 * Funkcja sprawdza zdarzenia klawiatury.@n
 * @param[out] _pointer Wskaźnik, pod którym zostaje zapisany struktura zdarzenia klawiatury
 */
void Check_Key_Event(Key_Action ** _pointer);
/*!
 * Funkcja sprawdza bufor zdarzeń.@n
 * @return Wartość odpowiadająca zdarzeniu w buforze. -1 gdy bufor pusty.
 */
int Event_Get();
/*!
 * Funkcja ustawia bufor zdarzeń.@n
 * @param[in] _type Wartość odpowiadająca zdarzeniu zapisywanemu w buforze.
 */
void Event_Set(int _type);
/*!
 * Czyści bufor zdarzeń.@n
 */
void Event_Clean();
/*!
 * Funkcja sprawdzająca, czy został kliknięty obiekt o zadanym numerze.@n
 * @param[in] _value Szukany numer
 * @param[in] _hits Liczba kliknięć
 * @param[in] _buffer Bufor danych
 * @return 1 gdy, wartość została znaleziona. W przeciwnym wypadku 0.
 */
int Find_Hit(int _value,int _hits,unsigned int * _buffer);
/*!
 * Funkcja czyszcząca bufor zdarzeń klawiatury.@n
 */
void Clean_Key_Event();
/*!
 * Funkcja czyszcząca bufor zdarzeń myszy.@n
 */
void Clean_Mouse_Event();
#endif