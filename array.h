/*! \file array.h
    \brief Simple data structure for easy memory allocation.
*/
#ifndef ARRAY_H
#define	ARRAY_H
#include <stddef.h>
#define STRING_SIZE 32

typedef struct _Array_Element Array_Element;
/** Structure to represent basic element of array */
struct _Array_Element{
	void * Value;
	unsigned short int Type;
	char Key[STRING_SIZE+1];//char * Key;
	unsigned int Size; 
	Array_Element *Next,*Previous;
};

typedef struct{
	Array_Element * First,*Last;
	size_t Size; //in bytes
	int Length;
}Array;

typedef struct{
	Array_Element * Position;
	Array * Object;
}Iterator;

/** Types of data which are implemented */
enum{
	ARRAY_BOOLEAN = 0,
	ARRAY_CHAR,
	ARRAY_INTEGER,
	ARRAY_FLOAT,
	ARRAY_TEXT
};

/** \fn size_t Type_Size(int _type);
    \brief Returns size of data type.
	Returns 0 if unknown type.

    \param _path Type of data
*/	
size_t Type_Size(int _type);
	
/** \fn Array * Create_Array();
    \brief Returns newly created array.
		NULL is memory wasn't allocated.
*/
Array * Create_Array();

/** \fn size_t Get_Size(Options * _options);
    \brief Returns size in bytes of all allocated memory by array.
		NULL if memory wasn't allocated.
*/
size_t Get_Size(Array * _array);

/** \fn int Add_Element(Array * _array,const char * _name,void * _value,int _size,int _type);
    \brief Adds new element to array.
	
	\param _array Pointer to array.
	\param _name Name of element.
	\param _type Type of data.
	\param _size Number of values in element.
*/
int Add_Element(Array * _array,const char * _name,void * _value,int _size,int _type);

/** \fn Iterator * Find_Element(Array * _array,const char * _name);
    \brief Returns pointer to iterator, which indicates to element with given name.
			NULL if name wasn't found.
	
	\param _name Name of element.
*/
Iterator * Find_Element(Array * _array,const char * _name);

/** \fn void Insert_Value(Array * _array,const char * _name,void * _value);
    \brief Adds(or replaces) values in element.
			Old value is deleted.
	
	\param _array Pointer to array object.
	\param _name Name of array.
	\param _value Pointer to array with values.
*/
int Insert_Value(Array * _array,const char * _name,void * _value);

/** \fn Array * Load(const char * _path,const char * _label)
    \brief Loads array data from file

    \param _path Path of file with data.
    \param _label Name of section with data.
*/
Array * Load(const char * _label,const char * _path);

/** \fn int Save(Array * _array,const char * _path,const char * _label);
    \brief Saves options to file
			Return 1 if failed.

    \param _path Path of file.
    \param _label Name of section.
    \param _array Pointer to array.
*/
int Save(Array * _array,const char * _label,const char * _path);

/** \fn void Free_Array(Array * _array);
    \brief Deletes data and array.

    \param _array Pointer to array.
*/
void Free_Array(Array * _array);

Iterator * Create_Iterator(Array * _array);
void Get_First(Iterator * _it);
void Get_Next(Iterator * _it);
void Get_Previous(Iterator * _it);
void Get_Last(Iterator * _it);
void Find(Iterator * _it,const char * _label);
void Delete_Iterator(Iterator * _it);
unsigned short int Get_Type(Iterator * _it);
void * Get_Value(Iterator * _it);
unsigned int Get_Value_Size(Iterator * _it);
char * Get_Name(Iterator * _it);
void Set_Value(Iterator * _it,void * _value);
void Erase(Iterator * _it);
short int Get_ValueB(Iterator * _it);
int Get_ValueI(Iterator * _it);
float Get_ValueF(Iterator * _it);
short int * Get_Value_ArrayB(Iterator * _it);
int * Get_Value_ArrayI(Iterator * _it);
char * Get_Value_ArrayC(Iterator * _it);
float * Get_Value_ArrayF(Iterator * _it);


#endif
