#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "array.h"
#include "game.h"
#include "graphic.h"


size_t Type_Size(int _type)
{
	switch(_type)
	{
		case BOOLEAN:	return sizeof(short int); break;
		case INTEGER:	return sizeof(int); break;
		case CHAR:		return sizeof(char); break;
		case FLOAT:		return sizeof(float); break;
		case TEXT:		return sizeof(Text); break;
		default:		ERROR(0,"Wrong type or not implemented yet!"); break;
	}
}		

/** \fn int Compare_Strings(const char * _str1,const char * _str2)
    \brief Simple internal function for comparing strings.
			Returns 0 if first is "lower", 1 if second is "lower", -1 if both strings are this same
    \param _str1 First string.
    \param _str2 Second string.
*/
int Compare_Strings(const char * _str1,const char * _str2)
{
	while(*_str1 != '\0' && *_str2 != '\0' && *_str1++ == *_str2++);

	if(*_str1 == '\0' && *_str2 == '\0')
	{
		_str1--;
		_str2--;
		if(*_str1 == *_str2)
			return -1;
		else
			return *_str1 < *_str2 ? 0 : 1;
	}
	else if(*_str1 == '\0' || *_str2 == '\0')
		return *_str1 == '\0' ? 0 : 1;
	_str1--;
	_str2--;
	return *_str1 < *_str2 ? 0 : 1;
}

Array * Create_Array()
{
	Array * ret = NULL;
	ret = malloc(sizeof(*ret));
	if(ret != NULL)
	{
		ret->Size = 0;
		ret->First = NULL;
		ret->Last = NULL;
		ret->Length = 0;
	}
	return ret;
}
	
int Add_Element(Array * _array,const char * _name,void * _value,int _size,int _type)
{
	Array_Element * ptr = NULL;
	Iterator * it = Create_Iterator(_array);
	int position = 0,left = 0,right = 0,temp=0;
	char * buffer;
	void * value = NULL;
	int i;
	
	if(!_array->First)
	{
		ptr = malloc(sizeof(*ptr));
		ptr->Previous = NULL;
		ptr->Next = NULL;
		_array->First = ptr;
		_array->Last = ptr;
	}
	else
	{
		left = 0;
		right = _array->Length - 1;
		while(left < right){
			position = (left+right)/2;
			if(position < _array->Length/2)
			{
				Get_First(it);
				for(i=0;i<position;i++)
					Get_Next(it);
			}
			else
			{
				Get_Last(it);
				for(i=0;i<_array->Length-position;i++)
					Get_Previous(it);
			}
			temp = Compare_Strings(_name,it->Position->Key);
			if(temp == 0)
				right = position-1;
			else if(temp == 1)
				left = position+1;
			else
				ERROR(0,"Element exists already in array");
		}
		ptr = malloc(sizeof(*ptr));
		Get_First(it);
		for(i=0;i<left;i++)
			Get_Next(it);
		if(!Compare_Strings(_name,it->Position->Key))
		{
				ptr->Previous = it->Position->Previous;
				ptr->Next = it->Position;
				if(it->Position->Previous)
				it->Position->Previous->Next = ptr;
				it->Position->Previous = ptr;
				if(!left)
					_array->First = ptr;
		}
		else
		{
			ptr->Previous = it->Position;
			ptr->Next = it->Position->Next;
			if(it->Position->Next)
			it->Position->Next->Previous = ptr;
			it->Position->Next = ptr;
			if(left == _array->Length-1)
				_array->Last = ptr;
		}
	}
	buffer = malloc(sizeof(*buffer)*(STRING_SIZE+1));
	memset(buffer,0,sizeof(*buffer)*(STRING_SIZE+1));
	strcpy(buffer,_name);
	if(_value)
	{
		value = malloc(Type_Size(_type)*_size);
		memcpy(value,_value,_size*Type_Size(_type));
	}
	ptr->Size = _size;
	ptr->Type = _type;
	ptr->Value = value;
	ptr->Key = buffer;
	_array->Size += Type_Size(_type)*_size + sizeof(ptr->Size) \
				+ sizeof(*(ptr->Key))*STRING_SIZE + sizeof(ptr->Type);
	_array->Length++;
	Delete_Iterator(it);
	return 0;
}

Iterator * Find_Element(Array * _array,const char * _name)
{
	Iterator * it = Create_Iterator(_array);
	int position = 0,left = 0,right = 0,temp=0;
	int i;
	
	left = 0;
	right = _array->Length - 1;
	while(left < right){
		position = (right+left)/2;
		if(position < _array->Length/2)
		{
			Get_First(it);
			for(i=0;i<position;i++)
				Get_Next(it);
		}
		else
		{
			Get_Last(it);
			for(i=0;i<_array->Length-position-1;i++)
				Get_Previous(it);
		}
		temp = Compare_Strings(_name,it->Position->Key);
		if(temp == 0)
			right = position;
		else if(temp == 1)
			left = position+1;
		else
			break;
	}
	Get_First(it);
	if(left == right)
	{
		for(i=0;i<left;i++)
			Get_Next(it);
	}
	else
	{
		for(i=0;i<position;i++)
			Get_Next(it);
	}
        if(it->Position && Compare_Strings(it->Position->Key,_name) != -1)
            it->Position = NULL;
        return it;
}

int Insert_Value(Array * _array,const char * _name,void * _value)
{
	Iterator * it = Find_Element(_array,_name);
	
	if(it->Position)
	{
		if(Get_Value(it))
			free(Get_Value(it));
		Set_Value(it,_value);
	}
	else
		ERROR(1,"Element doesn't exist");
	Delete_Iterator(it);
	return 0;
}

void Free_Array(Array * _array)
{
	Iterator * it = Create_Iterator(_array);
	Get_First(it);
	if(it->Position != _array->Last)
	{
		while(it->Position != _array->Last){
			Get_Next(it);
			free(it->Position->Previous->Key);
			free(it->Position->Previous->Value);
			free(it->Position->Previous);
		}
		free(it->Position->Key);
		free(it->Position->Value);
		free(it->Position);
	}
	else
	{
		free(it->Position->Key);
		free(it->Position->Value);
		free(it->Position);
	}
	Delete_Iterator(it);
	free(_array);
}

size_t Get_Size(Array * _array)
{
	return _array->Size;
}

Iterator * Create_Iterator(Array * _array)
{
	Iterator * ptr = NULL;
	ptr = malloc(sizeof(*ptr));
	if(ptr != NULL)
	{
		ptr->Object = _array;
		ptr->Position = _array->First;
	}
	return ptr;
}

void Erase(Iterator * _it)
{
	Array_Element * temp;
	if(_it->Position)
	{
		if(_it->Position->Next && _it->Position->Previous)
		{
			_it->Position->Next->Previous = _it->Position->Previous;
			_it->Position->Previous->Next = _it->Position->Next;
		}
		else if(_it->Position->Next)
		{
			_it->Object->First = _it->Position->Next;
			_it->Position->Next->Previous = NULL;
		}
		else if(_it->Position->Previous)
		{
			_it->Object->Last = _it->Position->Previous;
			_it->Position->Previous->Next = NULL;
		}
		else
			_it->Object->First = _it->Object->Last = NULL;
			
		_it->Object->Size -= Type_Size(_it->Position->Type)*_it->Position->Size \
                        + sizeof(_it->Position->Size) \
			+ sizeof(*(_it->Position->Key))*STRING_SIZE + sizeof(_it->Position->Type);
		_it->Object->Length--;
		free(_it->Position->Key);
		free(_it->Position->Value);
		free(_it->Position);
	}
}

void Get_First(Iterator * _it)
{
	_it->Position = _it->Object->First;
}

void Get_Next(Iterator * _it)
{
	if(_it->Position->Next)
		_it->Position = _it->Position->Next;
}

void Get_Previous(Iterator * _it)
{
	if(_it->Position->Previous)
	_it->Position = _it->Position->Previous;
}

void Get_Last(Iterator * _it)
{
	_it->Position = _it->Object->Last;
}

void Find(Iterator * _it,const char * _label)
{
	Iterator * it;
	it = Find_Element(_it->Object,_label);
	_it->Position = it->Position;
	Delete_Iterator(it);
}

void Delete_Iterator(Iterator * _it)
{
	if(_it)
		free(_it);
}

#define GET(name,type,variable,err)		\
type name(Iterator * _it)				\
{										\
	if(_it->Position)					\
		return _it->Position->variable;	\
	else								\
		return err;						\
}

GET(Get_Type,unsigned short int,Type,-1);
GET(Get_Value,void *,Value,NULL);
GET(Get_Value_Size,unsigned int,Size,0);
GET(Get_Name,char *,Key,NULL);

void Set_Value(Iterator * _it,void * _value)
{
	//free(_it->Position->Value);
	memcpy(_it->Position->Value,_value,_it->Position->Size*Type_Size(_it->Position->Type));
}

Array * Load(const char * _label,const char * _path)
{
	FILE * file;
	char buffer[STRING_SIZE+1],name[STRING_SIZE+1];
	unsigned short int type;
	unsigned int len;
	void * value;
	size_t size,type_size,begin_size;
	Array * ret = NULL;
	
	file = fopen(_path,"rb");
	if(!file)
		ERROR(NULL,"Couldn't open file %s!",_path);
	fgetc(file);
	while(!feof(file)){
		fseek(file,-1,SEEK_CUR);
		if(fread(buffer,sizeof(char),STRING_SIZE,file) != STRING_SIZE)
			ERROR(NULL,"Error: couldn't find section label!");
		if(fread(&size,sizeof(size),1,file) != 1)
			ERROR(NULL,"Error: couldn't find size of section!");
		begin_size = size;
		if(!strcmp(buffer,_label))
		{
			ret = Create_Array();
			while(begin_size >= size && size > 0){
				
				if(fread(name,sizeof(char),STRING_SIZE,file) != STRING_SIZE)
					ERROR(NULL,"Error: couldn't find variable name!");
				size -= STRING_SIZE*sizeof(char);		
	
				if(fread(&type,sizeof(type),1,file) != 1)
					ERROR(NULL,"Error: couldn't find type of %s!",name);
				size -= sizeof(type);
				
				type_size = Type_Size(type);
				if(!type_size)
					ERROR(NULL," ");

				if(fread(&len,sizeof(len),1,file) != 1)
					ERROR(NULL,"Error: couldn't read number of data in %s!",name);
				size -= sizeof(len);
				
				value = malloc(type_size*len);
				if(fread(value,type_size,len,file) != len)
					ERROR(NULL,"Error: wrong number of data in %s!",name);

				Add_Element(ret,name,value,len,type);
				free(value);
				size -= type_size*len;	
			}
			if(size)
				WARNING("Size of loaded data is different of size declared \
				in file. Loaded values may be corrupted");
			if(ret->Length == 0)
				free(ret);
			break;
		}
		else
		{
			fseek(file,size,SEEK_CUR);
			fgetc(file);
		}
	}
	fclose(file);
	return ret;
}

int Save(Array * _array,const char * _label,const char * _path)
{
	FILE * file;
	char * first_p = NULL,*second_p = NULL;
	unsigned short int type;
	unsigned int size,i;
	size_t size_1,size_2,size_b,position;
	char buffer[17];
	Iterator * it;
	
	file = fopen(_path,"rb");
	/** First case - file doesn't exist or empty */
	if(file)
	{
		fgetc(file);
		if(!feof(file))
		{
			/** Search for our label */
			while(!feof(file)){
				fseek(file,-1,SEEK_CUR);
				position = ftell(file);
				if(fread(buffer,sizeof(char),STRING_SIZE,file) != STRING_SIZE)
					break;
				if(fread(&size_b,sizeof(size_b),1,file) != 1)
					break;
				/** Label found ? */
				if(!strcmp(buffer,_label))
				{
					size_1 = position;
					/** Sth before our label */
					if(size_1 != 0)
					{
						first_p = malloc(sizeof(*first_p)*size_1);
						fseek(file,0,SEEK_SET);
						fread(first_p,sizeof(*first_p),size_1,file);
						fseek(file,STRING_SIZE+sizeof(size_b),SEEK_CUR);
					}
					fseek(file,size_b,SEEK_CUR);
					position = ftell(file);
					fseek(file,0,SEEK_END);
					size_2 = ftell(file) - position;
					/** Sth after our label */
					if(size_2 != 0)
					{
						second_p = malloc(sizeof(*second_p)*(size_2));
						fseek(file,-size_2,SEEK_END);
						fread(second_p,sizeof(*second_p),size_2,file);
					}
					fgetc(file);
				}
				else
				{
					fseek(file,size_b,SEEK_CUR);
					fgetc(file);
				}
			}
			/** Label hasn't been found */
			if(first_p == NULL && second_p == NULL)
			{
				fseek(file,0,SEEK_END);
				position = ftell(file);
				first_p = malloc(sizeof(*first_p)*position);
				fseek(file,0,SEEK_SET);
				size_1 = fread(first_p,sizeof(*first_p),position,file);
			}
			fclose(file);
			remove(_path);
		}
	}
	
	/** Write our data */
	file = fopen(_path,"wb");
	
	if(first_p != NULL)
		fwrite(first_p,sizeof(*first_p),size_1,file);
		
	if(fwrite(_label,sizeof(char),STRING_SIZE,file) != STRING_SIZE)
		ERROR(1,"Error during writing data");
	size_b = Get_Size(_array);
	if(fwrite(&size_b,sizeof(size_b),1,file) != 1)
		ERROR(1,"Error during writing data");
		
	it = Create_Iterator(_array);
	Get_First(it);
	for(i=0;i<_array->Length;i++)
	{
		type = Get_Type(it);
		size = Get_Value_Size(it);
		if(fwrite(Get_Name(it),sizeof(char),STRING_SIZE,file) != STRING_SIZE && \
			fwrite(&type,sizeof(type),1,file) != 1 && fwrite(&size,sizeof(size),1,file) != 1 \
			&& fwrite(Get_Value(it),Type_Size(type),size,file) != size)
			ERROR(1,"Error during writing data");
		Get_Next(it);
	}
	
	if(second_p != NULL)
		fwrite(second_p,sizeof(*second_p),size_2,file);
		
	free(first_p);
	free(second_p);
	Delete_Iterator(it);
	fclose(file);
	return 0;
}

#define GET_VALUE(name,type,_type)						\
type name(Iterator * _it)								\
{														\
	if(!_it->Position)									\
		ERROR(-1,"Empty iterator");						\
	if(_it->Position->Type != _type)					\
		ERROR(-1,"Wrong type in option");				\
	return *((type*)_it->Position->Value);				\
}
GET_VALUE(Get_ValueB,short int,BOOLEAN);
GET_VALUE(Get_ValueI,int,INTEGER);
GET_VALUE(Get_ValueF,float,FLOAT);

#define GET_VALUE_ARRAY(name,type,_type)				\
type * name(Iterator * _it)								\
{														\
	type * ptr = NULL;									\
	if(!_it->Position)									\
		ERROR(NULL,"Empty iterator");					\
	if(_it->Position->Type != _type)					\
		ERROR(-1,"Wrong type in option");				\
	ptr = malloc(sizeof(*ptr)*_it->Position->Size);		\
	memset(ptr,0,_it->Position->Size*sizeof(*ptr));		\
	memcpy(ptr,_it->Position->Value,_it->Position->Size*sizeof(*ptr));\
	return ptr;									\
}
GET_VALUE_ARRAY(Get_Value_ArrayB,int,BOOLEAN);
GET_VALUE_ARRAY(Get_Value_ArrayI ,int,INTEGER);
GET_VALUE_ARRAY(Get_Value_ArrayF,float,FLOAT);
GET_VALUE_ARRAY(Get_Value_ArrayC,char,CHAR);