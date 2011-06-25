#ifndef FIFO_H
#define	FIFO_H


typedef struct _FIFO_Element FIFO_Element;
typedef struct{
	struct _FIFO_Element{
		int Value;
		FIFO_Element * Next;
	} * Top,*Bottom;
}FIFO;

FIFO * FIFO_Create();
void FIFO_Push(FIFO * _FIFO, int _value);
int FIFO_Pop(FIFO * _FIFO);
int FIFO_Check(FIFO * _FIFO);
void FIFO_Delete(FIFO * _FIFO);
void FIFO_Get_All(FIFO * _queue,int * _size,int ** _tab);
void FIFO_Clean(FIFO * _queue);

#endif