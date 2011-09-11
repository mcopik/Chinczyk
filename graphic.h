/*!
 * @file graphic.h
 * Zawiera implementację funkcji i struktur do obsługi tworzenia okien i renderowania grafiki.@n
 * @par Chińczyk
 * @author Marcin Copik
 * @date 2011.09.10
 * @version 1.0
*/
#ifndef GRAPHIC_H
#define	GRAPHIC_H
#include <stdint.h>
#include "game.h"
/*!
 * Liczba renderowanych klatek na sekundę.@n
 */
#define FPS 25
/*!
 * Szybkość przesuwania kamery.@n
 */
#define CAMERA_SPEED 0.25f
/*!
 * Minimalna przybliżenie planszy.@n
 */
#define MIN_DISTANCE 10.0f
/*!
 * Maksymalne oddalenie planszy.@n
 */
#define MAX_DISTANCE 50.0f
/*!
 * Szerokość planszy.@n
 */
#define W 7.0f
/*!
 * Wysokość planszy.@n
 */
#define H 0.5f
/*!
 * Głębokość planszy.@n
 */
#define D 7.0f
/*!
 * Promień koła pola gry.@n
 */
#define FIELD_RADIUS (float)(MIN(W,D)*0.03)
/*!
 * Promień podstawy stożka oraz sfery, z których składa się pionek.@n
 */
#define PAWN_RADIUS	(FIELD_RADIUS - 0.01f)
/*!
 * Wysokość pionka.@n
 */
#define PAWN_HEIGHT	0.3f
/*!
 * Stała matematyczna PI.@n
 */
#define PI 3.1415926f
/*!
 * Liczba tekstur.@n
 */
#define NUMBER_OF_TEXTURES 2
/*!
 * Położenie kostki na osi X.@n
 */
#define CUBE_X	1.2f
/*!
 * Położenie kostki na osi Y.@n
 */
#define CUBE_Y	-0.8f
/*!
 * Położenie kostki na osi Z.@n
 */
#define CUBE_Z	-3.0f
/*!
 * Wielkość kostki.@n
 */
#define CUBE_SIZE 0.4f
/*!
 * Numer kostki.@n
 */
#define CUBE_HIT 99
/*!
 * Numer planszy.@n
 */
#define BOARD_HIT 0
/*!
 * Częstotliwość animacji migania pionka/pola(w klatkach).@n
 */
#define FREQUENCY 
/*!
 * Makro wyznaczające wartość minimalną.@n
 */
#define MIN(a,b) a < b ? a : b

/*!
 * Wyliczenie obejmujące akcje modyfikujące położenie kamery.@n
 */
enum Camera_Operations{
    CAMERA_LEFT,	//!< Obrót kamery w lewo
    CAMERA_RIGHT,	//!< Obrót kamery w prawo
    CAMERA_UP,	//!< Podniesienie kamery
    CAMERA_DOWN,	//!< Obniżenie kamery
    CAMERA_FARTHER,	//!< Przybliżenie kamery
    CAMERA_CLOSER	//!< Oddalenie kamery
};
/*!
 * Wyliczenie obejmujące operacje na strukturze kamery.@n
 */
enum Camera_Actions{
    CAMERA_SET,	//!< Ustawienie nowych wartości kamery
    CAMERA_GET,	//!< Pobranie wartości kamery
    CAMERA_CHANGE	//!< Zmiana położenia kamery
};

enum {
    FULL_NOT = 0,
    FULLSCREEN
};
/*!
 * Wyliczenie obejmujące:@n
 * - operacje na buforze wyświetlanych tekstów@n
 * - położenie na ekranie tekstu(normalne<->wyśrodkowane).@n
 */
enum Text_Operations{
    TEXT_INIT,	//!< Inicjalizacja bufora tekstowego
    TEXT_ADD,	//!< Dodanie tekstu do bufora
    TEXT_REMOVE,	//!< Usunięcie tekstu do bufora
    TEXT_CLEAN,	//!< Czyszczenie bufora tekstu
    TEXT_NORMAL,	//!< Normalne wyświetlanie napisu
    TEXT_CENTER	//!< Wyśrodkowanie napisu
};
/*!
 * Struktura przechowująca załadowaną teksturę.@n
 */
typedef struct {
	unsigned int Width;		//!< Szerokość tekstury
	unsigned int Height;	//!< Wysokość tekstury
	char * Data;	//!< Tablica z danymi
} Image;
/*!
 * Struktura pola gry.@n
 */
typedef struct {
	float Position[2];	//!< Pozycja(X,Y) na polu gry
	float Radius;	//!< Promień koła
} Field;
/*!
 * Struktura ukrywająca wewnętrzną implementację pól.@n
 */
typedef struct {
	Field * Data;	//!< Tablica z wskaźnikami do pól
	int Number_of_Players;	//!< Liczba graczy
	int Number_of_Fields;	//!< Liczba pól
} Fields_Structure;
/*!
 * Struktura przechowująca informacje o animacji migania.@n
 * Przechowywana jest informacja o:@n
 * - numerach migających pionków(możliwe ruchy pionkami)@n
 * - numerze migającego pionka oraz pola(zaznaczony pionek,możliwy ruch)@n
 */
typedef struct {
	int Player_Number;	//!< Numer gracza
	int Field_Number;	//!< Numer pola
	int * Pawns_Numbers;	//!< Tablica z numerami pionków
} Blink_Info;
/*!
 * Struktura przechowująca informacje o wyświetlanym tekście.@n
 */
typedef struct {
	char * String;	//!< Wyświetlany napis
	float X;	//!< Położenie(X) na ekranie
	float Y;	//!< Położenie(Y) na ekranie
	int Select_Number;	//!< Numer obiektu
	int Position;	//!< Pozycja na ekranie
	void * Font;	//!< Czcionka
} Text;
/*!
* Funkcja zmiany położenia kamery.@n
* @param[in] _change Zmiana położenia podawana za pomocą typu wyliczeniowego.
*/
void Change_Camera(int _change);
/*!
* Funkcja zmiany położenia kamery.@n
* @param[in] _camera Tablica trzech wartości zmiennoprzecinkowych:
* 0 - Obrót na osi X
* 1 - Obrót na osi Y
* 2 - Oddalenie na osi Z
*/
void Set_Camera(float * _camera);
/*!
* Funkcja pobrania wartości kamery.@n
* @return Tablica trzech wartości zmiennoprzecinkowych:
* 0 - Obrót na osi X
* 1 - Obrót na osi Y
* 2 - Oddalenie na osi Z
*/
float * Get_Camera();
/*!
 * Funkcja wczytania grafiki do struktury tekstury.@n
 * @param[out] _image Wskaźnik na strukturę tekstury
 * @param[in] _path Ścieżka do pliku
 * @return 0 jeśli wczytanie przebiegło bez problemów. W przeciwnym wypadku 1.@n
 */
int Load_Image(Image * _image,const char * _path);
/*!
 * Funkcja zwalnia pamięć przydzieloną podczas wczytania grafiki.@n
 * @param[in] _image Wskaźnik na strukturę tekstury
 */
void Close_Image(Image * _image);
/*!
 * Funkcja pomocnicza modyfikująca podaną wartość, aby mieściła się w zadanym przedziale.@n
 * @param[in,out] _number Wskaźnik na podaną wartość
 * @param[in] _min Dolna granica przedziału
 * @param[in] _max Górna granica przedziału
 */
void Interval(float * _number,float _min,float _max);
/*!
 * Funkcja inicjująca mechanizmy OpenGL.@n
 * @param[in] _width Szerokość okna
 * @param[in] _height Wysokość okna
 * @param[in] _label Etykieta okna
 * @param[in] _fullscr Wartość wyliczeniowa określająca renderowanie w okienku/pełnym oknie
 * @return Numer okna
 */
int Init_GL(int _width, int _height, char * _label,int _fullscr);
/*!
 * Renderowanie w trybie rysowania.@n
 */
void Draw_Render();
/*!
 * Renderowanie w trybie analizy kliknięcia myszą.@n
 */
void Draw_Select();
/*!
 * Funkcja inicjalizująca mechanizm rysowania.@n
 * @param[in] _fields Wskaźnik na strukturę pól
 * @param[in] _players Wskaźnik na tablicę graczy
 * @param[in] _players_number Liczba graczy
 * @param[in] _randomized Wskaźnik na zmienną przechowująca ostatnią wylosowaną wartość.
 */
void Draw_Init(Fields_Structure * _fields,Player * _players,int _players_number,int * _randomized);
/*!
 * Funkcja ustawiająca animację migania.@n
 * @param[in] _number Numer gracza
 * @param[in] _pawns Wskaźnik na tablicę migania pionków
 */
void Blink_Set_Pawn(int _number,int * _pawns);
/*!
 * Funkcja ustawiająca animację migania.@n
 * @param[in] _number Numer gracza
 * @param[in] _pawn Numer pionka
 * @param[in] _field sNumer pola
 */
void Blink_Set_Field(int _number,int _pawn,int _field);
/*!
 * Funkcja wyłączająca miganie.@n
 */
void Disable_Blink();
/*!
 * Zmiana rozdzielczości okna z pozycjonowaniem okna.@n
 * @param[in] _width Nowa szerokość
 * @param[in] _height Nowa wysokość
 */
void Change_Display(int _width,int _height);
/*!
 * Włączenie renderowania w pełnym oknie.@n
 */
void Enable_FullScr();
/*!
 * Zmiana rozdzielczości okna.@n
 * @param[in] _width Nowa szerokość
 * @param[in] _height Nowa wysokość
 */
void Reshape_Window(int _width, int _height);
/*!
 * Inicjalizacja bufora tekstów.@n
 * @param[in] _width Szerokość okna
 * @param[in] _height Wysokość okna
 */
void Text_Init(int _width,int _height);
/*!
 * Dodanie tekstu do bufora.@n
 * @param[in] _text Wskaźnik na strukturę tekstu
 * @param[in] _name Nazwa tekstu
 */
void Text_Add(Text * _text,const char * _name);
/*!
 * Usunięcie tekstu z bufora.@n
 * @param[in] _name Nazwa tekstu
 */
void Text_Remove(const char * _name);
/*!
 * Wyczyszczenie bufora tekstów.@n
 */
void Text_Clean();
/*!
 * Funkcja tworząca tekst.@n
 * @param[in] _x Położenie(X) w postaci % ekranu(0.0-1.0)
 * @param[in] _y Położenie(Y) w postaci % ekranu(0.0-1.0)
 * @param[in] _select Numer tekstu
 * @param[in] _string Napis
 * @param[in] _font Czcionka
 * @return Wskaźnik na utworzoną strukturę.
 */
Text * Text_Create(float _x,float _y,int _select,const char * _string,void * _font);
/*!
 * Renderowanie napisów na ekranie.@n
 * @param[in] _type Typ renderowania
 */
void Draw_Text(int _type);
/*!
 * Funkcja tworząca strukturę pól dla odpowiedniej ilości graczy.@n
 * @param[in] _number_of_players Liczba graczy
 * @return Wskaźnik na strukturę pól
 */
Fields_Structure * Fields_Generate(int _number_of_players);
/*!
 * Funkcja tworząca dane pola dla 4 graczy.@n
 * @return Wskaźnik na tablicę pól
 */
void Fields_Generate_4_Players(Field * _pointer);
/*!
 * Funkcja tworząca dane pola dla 5 graczy.@n
 * @return Wskaźnik na tablicę pól
 */
void Fields_Generate_5_Players(Field * _pointer);
/*!
 * Funkcja tworząca dane pola dla 6 graczy.@n
 * @return Wskaźnik na tablicę pól
 */
void Fields_Generate_6_Players(Field * _pointer);
/*!
 * Funkcja zwracająca promień koła pola.@n
 * @param[in] _fields Wskaźnik na strukturę pól
 * @param[in] _number Numer pola
 * @return Promień
 */
float Fields_Get_Radius(Fields_Structure * _fields,int _number);
/*!
 * Funkcja zwracająca położenie(X) pola na planszy.@n
 * @param[in] _fields Wskaźnik na strukturę pól
 * @param[in] _number Numer pola
 * @return Położenie X
 */
float Fields_Get_X(Fields_Structure * _fields,int _number);
/*!
 * Funkcja zwracająca położenie(Y) pola na planszy.@n
 * @param[in] _fields Wskaźnik na strukturę pól
 * @param[in] _number Numer pola
 * @return Położenie Y
 */
float Fields_Get_Y(Fields_Structure * _fields,int _number);
/*!
 * Funkcja zwalniająca zaalokowaną pamięć w trakcie tworzenia pól.@n
 * @param[in] _fields Wskaźnik na strukturę pól
 */
void Fields_Close(Fields_Structure * _fields);
/*!
 * Funkcja służąca do obsługi dodania napisu do bufora napisów.@n
 * @param[in] _x Położenie(X) na ekranie
 * @param[in] _y Położenie(Y) na ekranie
 * @param[in] _select_name Numer
 * @param[in] _font Czcionka
 * @param[in] _position_type Położenie na ekranie
 * @param[in] _name Nazwa
 * @param[in] _format Format napisu:
 * - %d liczba całkowita
 * - %f liczba zmiennoprzecinkowa
 * - %s napis
 * - %c znak
 * @param[in] ... Dodatkowe zmienne używane w napisie
 */
void Text_Draw(float _x,float _y,int _select_name,void *_font,int _position_type,\
               const char * _name,const char * _format,...);
/*!
 * Funkcja renderująca oczka na kości.@n
 * @param[in] _radius Promień oczka
 * @param[in] _number Wylosowana wartość
 */
void Draw_Cube_Pips(float _radius, int _number);
/*!
 * Funkcja zwracająca wysokość użytej czcionki.@n
 * @param[in] _font Czcionka
 * @return Wysokość czcionki
 */
int Font_Height(void * _font);
#endif