/*!
 * @file array.h
 * Zawiera implementację zaawansowanej struktury danych.@n
 * Umożliwia tworzenie tablic przechowujących wartości o kilku typach i dowolnym rozmiarze.@n
 * @par Chińczyk
 * @author Marcin Copik
 * @date 2011.09.10
 * @version 1.0
*/
#ifndef ARRAY_H
#define	ARRAY_H
#include <stddef.h>
/*!
 * Długość napisu znakowego.@n
 */
#define STRING_SIZE 32
/*!
 * Długość napisu znakowego.@n
 */
typedef struct _Array_Element Array_Element;
/*!
 * Struktura podstawowego elementu przechowujące dane.@n
 */
struct _Array_Element{
	void * Value;	//!< Przechowywana wartość
	unsigned short int Type;	//!< Typ przechowywanych danych
	char Key[STRING_SIZE+1];	//!< Klucz, według którego tablica sortuje dane
	unsigned int Size;	//!< Liczba elementów
	Array_Element *Next;	//!< Wskaźnik na następny element w tablicy
	Array_Element *Previous;	//!< Wskaźnik na poprzedni element w tablicy
};
/*!
 * Struktura tablicy.@n
 */
typedef struct{
	Array_Element * First;	//!< Wskaźnik na pierwszy element w tablicy
	Array_Element * Last;	//!< Wskaźnik na ostatni element w tablicy
	size_t Size;	//!< Rozmiar tablicy w bajtach
	int Length;	//!< Rozmiar tablicy w postaci liczby elementów
}Array;
/*!
 * Struktura iteratora.@n
 */
typedef struct{
	Array_Element * Position;	//!< Aktualna pozycja iteratora
	Array * Object;	//!< Tablica, do której przypisany jest iterator
}Iterator;
/*!
 * Wyliczenie obejmujące typy danych przechowywane przez tablicę.@n
 */
enum Data_Types{
	ARRAY_BOOLEAN = 0,	//!< Wartość typu logicznego
	ARRAY_CHAR,	//!< Wartość typu znakowego
	ARRAY_INTEGER,	//!< Wartość typu całkowitego
	ARRAY_FLOAT,	//!< Wartość typu zmiennoprzecinkowego
	ARRAY_TEXT	//!< Struktura Text
};
/*!
 * Funkcja zwracająca rozmiar konkretnego typu danych.@n
 * @param[in] _type Typ danych
 * @return Rozmiar w bajtach
 */
size_t Type_Size(int _type);
/*!
 * Funkcja tworząca tablicę.@n
 * @return Wskaźnik na tablicę
 */
Array * Create_Array();
/*!
 * Funkcja zwracająca rozmiar tablicy.@n
 * @param[in] _array Wskaźnik na tablicę
 * @return Rozmiar tablicy w bajtach.
 */
size_t Get_Size(Array * _array);
/*!
 * Funkcja dodająca element do tablicy.@n
 * @param[in] _array Wskaźnik na tablicę
 * @param[in] _name Nazwa elementu
 * @param[in] _value Wskaźnik na przechowywaną wartość
 * @param[in] _size Liczba elementów
 * @param[in] _type Typ danych
 * @return 0, jeśli dodanie elementu przebiegło bez problemów; w przeciwnym wypadku - 1
 */
int Add_Element(Array * _array,const char * _name,void * _value,int _size,int _type);
/*!
 * Funkcja dodająca(jeśli nie zostały dodane przy tworzeniu elementu)/zastępująca wartość w elemencie.@n
 * @param[in] _array Wskaźnik na tablicę
 * @param[in] _name Nazwa elementu
 * @param[in] _value Wskaźnik na wartość
 * @return 1, jeśli element nie istnieje w tablicy; 0, jeśli wartość została dodana bez problemu
 */
int Insert_Value(Array * _array,const char * _name,void * _value);
/*!
 * Funkcja wczytująca tablicę z pliku.@n
 * @param[in] _label Nazwa tablicy
 * @param[in] _path Ścieżka do tablicy
 * @return Wskaźnik na tablicę utworzoną na podstawie danych z pliku; NULL jeśli wczytanie danych się nie powiodło
 */
Array * Load(const char * _label,const char * _path);
/*!
 * Funkcja wczytująca tablicę z pliku.@n
 * @param[in] _array Wskaźnik do tablicy
 * @param[in] _label Nazwa tablicy
 * @param[in] _path Ścieżka do tablicy
 * @return 1, jeśli zapis się nie powiódł; w przeciwnym wypadku - 0
 */
int Save(Array * _array,const char * _label,const char * _path);
/*!
 * Funkcja zwalniająca zaalokowaną pamięć i niszcząca tablicę.@n
 * @param[in] _array Wskaźnik do tablicy
 */
void Free_Array(Array * _array);
/*!
 * Funkcja tworząca iterator.@n
 * @param[in] _array Wskaźnik na tablicę
 * @return Wskaźnik na iterator
 */
Iterator * Create_Iterator(Array * _array);
/*!
 * Funkcja wyszukująca element w tablicy.@n
 * @param[in] _array Wskaźnik na tablicę
 * @param[in] _name Nazwa elementu
 * @return Wskaźnik na iterator
 */
Iterator * Find_Element(Array * _array,const char * _name);
/*!
 * Funkcja ustawiająca iterator na pierwszy element w tablicy.@n
 * @param[in] _it Wskaźnik na iterator
 */
void Get_First(Iterator * _it);
/*!
 * Funkcja ustawiająca iterator na następny element w tablicy.@n
 * @param[in] _it Wskaźnik na iterator
 */
void Get_Next(Iterator * _it);
/*!
 * Funkcja ustawiająca iterator na poprzedni element w tablicy.@n
 * @param[in] _it Wskaźnik na iterator
 */
void Get_Previous(Iterator * _it);
/*!
 * Funkcja ustawiająca iterator na ostatni element w tablicy.@n
 * @param[in] _it Wskaźnik na iterator
 */
void Get_Last(Iterator * _it);
/*!
 * Funkcja wyszukująca element w tablicy.@n
 * @param[in] _it Wskaźnik na iterator
 * @param[in] _label Nazwa elementu
 */
void Find(Iterator * _it,const char * _label);
/*!
 * Funkcja niszcząca iterator i zwalniająca zaalokowaną pamięć.@n
 * @param[in] _it Wskaźnik na iterator
 */
void Delete_Iterator(Iterator * _it);
/*!
 * Funkcja zwracająca typ danych w elemencie.@n
 * @param[in] _it Wskaźnik na iterator
 * @return Typ danych
 */
unsigned short int Get_Type(Iterator * _it);
/*!
 * Funkcja zwracająca wartość przechowywaną w tablicy.@n
 * @param[in] _it Wskaźnik na iterator
 * @return Wskaźnik na wartość
 */
void * Get_Value(Iterator * _it);
/*!
 * Funkcja zwracająca rozmiar danych w tablicy.@n
 * @param[in] _it Wskaźnik na iterator
 * @return Rozmiar danych
 */
unsigned int Get_Value_Size(Iterator * _it);
/*!
 * Funkcja zwracająca nazwę elementu.@n
 * @param[in] _it Wskaźnik na iterator
 * @return Nazwa elementu
 */
char * Get_Name(Iterator * _it);
/*!
 * Funkcja zapisująca nową wartość w elemencie.@n
 * @param[in] _it Wskaźnik na iterator
 * @param[in] _value Wskaźnik na nową wartość
 */
void Set_Value(Iterator * _it,void * _value);
/*!
 * Funkcja usuwająca element z tablicy.@n
 * @param[in] _it Wskaźnik na iterator
 */
void Erase(Iterator * _it);
/*!
 * Funkcja zwracająca wartość zapisaną w elemencie.@n
 * @param[in] _it Wskaźnik na iterator
 * @return Wartość typu logicznego
 */
short int Get_ValueB(Iterator * _it);
/*!
 * Funkcja zwracająca wartość zapisaną w elemencie.@n
 * @param[in] _it Wskaźnik na iterator
 * @return Wartość typu całkowitego
 */
int Get_ValueI(Iterator * _it);
/*!
 * Funkcja zwracająca wartość zapisaną w elemencie.@n
 * @param[in] _it Wskaźnik na iterator
 * @return Wartość typu zmiennoprzecinkowego
 */
float Get_ValueF(Iterator * _it);
/*!
 * Funkcja zwracająca wartość zapisaną w elemencie.@n
 * @param[in] _it Wskaźnik na iterator
 * @return Wskaźnik na tablicę wartości typu logicznego
 */
short int * Get_Value_ArrayB(Iterator * _it);
/*!
 * Funkcja zwracająca wartość zapisaną w elemencie.@n
 * @param[in] _it Wskaźnik na iterator
 * @return Wskaźnik na tablicę wartości typu całkowitego
 */
int * Get_Value_ArrayI(Iterator * _it);
/*!
 * Funkcja zwracająca wartość zapisaną w elemencie.@n
 * @param[in] _it Wskaźnik na iterator
 * @return Wskaźnik na tablicę wartości typu znakowego
 */
char * Get_Value_ArrayC(Iterator * _it);
/*!
 * Funkcja zwracająca wartość zapisaną w elemencie.@n
 * @param[in] _it Wskaźnik na iterator
 * @return Wskaźnik na tablicę wartości typu zmiennoprzecinkowego
 */
float * Get_Value_ArrayF(Iterator * _it);
#endif