/*!
 * @file fifo.h
 * Zawiera implementację prostej kolejki(First In, First Out).@n
 * @par Chińczyk
 * @author Marcin Copik
 * @date 2011.09.10
 * @version 1.0
*/

#include <stdio.h>
#include <stdlib.h>
#include "fifo.h"

FIFO * FIFO_Create()
{
	FIFO * ret;
	
	ret = (FIFO*)malloc(sizeof(*ret));
	ret->Top = NULL;
	ret->Bottom = NULL;
	return ret;
}

void FIFO_Push(FIFO * _FIFO, int _value)
{
	FIFO_Element * ptr;

	ptr = (FIFO_Element*)malloc(sizeof(*ptr));
	ptr->Value = _value;
	ptr->Next = NULL;
	if(_FIFO->Top)
		_FIFO->Top->Next = ptr;
	else
		_FIFO->Bottom = ptr;
	_FIFO->Top = ptr;
}

int FIFO_Pop(FIFO * _FIFO)
{
	int ret;
	FIFO_Element * ptr;

	if(_FIFO->Bottom)
	{
		ret = _FIFO->Bottom->Value;
		ptr = _FIFO->Bottom;
		_FIFO->Bottom = ptr->Next;
		if(!_FIFO->Bottom)
			_FIFO->Top = NULL;
		free(ptr);
		return ret;
	}
	return -1;
}

int FIFO_Check(FIFO * _FIFO)
{
	if(_FIFO->Bottom)
		return _FIFO->Bottom->Value;
	return -1;
}

void FIFO_Delete(FIFO * _FIFO)
{
	FIFO_Element * ptr;

	while(_FIFO->Bottom){
		ptr = _FIFO->Bottom;
		_FIFO->Bottom = ptr->Next;
		free(ptr);
	}
	free(_FIFO);
}

void FIFO_Get_All(FIFO * _queue,int * _size,int ** _tab)
{
	int i;
	FIFO_Element * ptr;

	*_size = 0;
	ptr = _queue->Bottom;
	while(ptr != NULL){
		(*_size)++;
		ptr = ptr->Next;
	}
	*_tab = (int *)malloc(sizeof(**_tab)*(*_size));
	ptr = _queue->Bottom;
	for(i = 0;i < *_size;i++){
		(*_tab)[i] = ptr->Value;
		ptr = ptr->Next;
	}
}

void FIFO_Clean(FIFO * _queue)
{
	while(FIFO_Pop(_queue) != -1);
}