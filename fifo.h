/*!
 * @file fifo.h
 * Zawiera interfejs prostej kolejki(First In, First Out).@n
 * @par Chińczyk
 * @author Marcin Copik
 * @date 2011.09.10
 * @version 1.0
*/
#ifndef FIFO_H
#define	FIFO_H

typedef struct _FIFO_Element FIFO_Element;
/*!
 * Struktura kolejki.@n
 */
typedef struct{
	/*!
	* Struktura podstawowego elementu kolejki.@n
	*/
	struct _FIFO_Element{
		int Value;	//!< Przechowywana wartość
		FIFO_Element * Next;	//!< Wskaźnik na następny element
	} * Top;	//!< Wskaźnik na najwyższy element
	FIFO_Element * Bottom;	//!< Wskaźnik na dno stosu
}FIFO;
/*!
 * Funkcja tworząca kolejkę.@n
 * @return Wskaźnik na kolejkę
 */
FIFO * FIFO_Create();
/*!
 * Funkcja dodająca wartość.@n
 * @param[in] _FIFO Wskaźnik na kolejkę
 * @param[in] _value Dodawana wartość
 */
void FIFO_Push(FIFO * _FIFO, int _value);
/*!
 * Funkcja zdejmująca wartość z wierzchu stosu.@n
 * @param[in] _FIFO Wskaźnik na kolejkę
 * @return Wartość z wierzchu stosu; -1 jeśli stos pusty
 */
int FIFO_Pop(FIFO * _FIFO);
/*!
 * Funkcja zwraca wartość na wierzchu stosu, ale nie zdejmuje jej.@n
 * @param[in] _FIFO Wskaźnik na kolejkę
 * @return Wartość z wierzchu stosu; -1 jeśli stos pusty
 */
int FIFO_Check(FIFO * _FIFO);
/*!
 * Funkcja zwalnia zaalokowaną pamięć.@n
 * @param[in] _FIFO Wskaźnik na kolejkę
 */
void FIFO_Delete(FIFO * _FIFO);
/*!
 * Funkcja umożliwia zapis wszystkich wartości z kolejki do tablicy.@n
 * @param[in] _queue Wskaźnik na kolejkę
 * @param[out] _size Wskaźnik na zmienną, pod którą zostanie zapisana ilość elementów
 * @param[out] _tab Wskaźnik na tablicę, w której zostaną zapisane żądane wartości
 */
void FIFO_Get_All(FIFO * _queue,int * _size,int ** _tab);
/*!
 * Funkcja czyści kolejkę.@n
 * @param[in] _queue Wskaźnik na kolejkę
 */
void FIFO_Clean(FIFO * _queue);
#endif