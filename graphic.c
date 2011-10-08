/*!
 * @file graphic.c
 * Zawiera implementację funkcji i struktur do obsługi tworzenia okien i renderowania grafiki.@n
 * @par Chińczyk
 * @author Marcin Copik
 * @date 2011.09.10
 * @version 1.0
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#include <assert.h>
#ifdef _WIN32
# include <GL/freeglut.h>
#else
# include <GL/glut.h>
#endif
#include "graphic.h"
#include "game.h"


/*!
 * Wewnętrzna funkcja operaowania kamerą.@n
 * @param[in] _type Argument określający zachowanie funkcji
 * @param[in] _camera Wskaźnik na tablicę danych kamery
 * @param[in] _action Zmienna określająca zmianę położenia kamery
 * @return Wskaźnik do struktury danych kamery, gdy żądano pobrania jej; w przeciwnym wypadku NULL
 */
float * Camera_Action(int _type,float * _camera,int _action)
{
	static float Camera[3];
	static float * pt;
	switch(_type){
		case CAMERA_SET:
			memcpy(Camera,_camera,sizeof(*Camera)*3);
		break;
		case CAMERA_GET:
			pt = (float*)malloc(sizeof(*pt)*3);
			memcpy(pt,Camera,sizeof(*Camera)*3);
			return pt;
		break;
		case CAMERA_CHANGE:
			switch(_action){
				case CAMERA_UP:
					Camera[0] += CAMERA_SPEED;
				break;
				case CAMERA_DOWN:
					Camera[0] -= CAMERA_SPEED;
				break;
				case CAMERA_LEFT:
					Camera[1] += CAMERA_SPEED;
				break;
				case CAMERA_RIGHT:
					Camera[1] -= CAMERA_SPEED;
				break;
				case CAMERA_FARTHER:
					Camera[2] += CAMERA_SPEED;
				break;
				case CAMERA_CLOSER:
					Camera[2] -= CAMERA_SPEED;
				break;
			}
			//kontrolne sprawdzenie, aby położenie kamery nie przyjęło
			//niedozwolnych wartości
			//nachylenie kamery nad planszą max 90 stopni, min 0 stopni
			Interval(&Camera[0],0,90);
			//maksymalne oddalenie, minimalne przybliżenie
			Interval(&Camera[2],MIN_DISTANCE,MAX_DISTANCE);
			while(Camera[1] >= 360.0f)
				Camera[1] -= 360.0f;
			Set_Change();
		break;
	}
	return NULL;
}

void Change_Camera(int _change)
{
	Camera_Action(CAMERA_CHANGE,NULL,_change);
}

void Set_Camera(float * _camera)
{
	Camera_Action(CAMERA_SET,_camera,0);
}

float * Get_Camera()
{
	return Camera_Action(CAMERA_GET,NULL,0);
}

void Interval(float * _number,float _min,float _max)
{
    if (*_number < _min)
        *_number = _min;
    else if(*_number > _max)
        *_number = _max;
}

int Load_Image(Image * _image,const char * _path){

	FILE * file;
	unsigned short int bpp,temp;
	size_t size;
	int i;

	file = fopen(_path,"rb");
	if(!file)
		ERROR_MACRO(1,"Bitmap doesn't exist");
	//nagłówek pliku
	fseek(file,18,SEEK_CUR);
	//szerokość obrazka
	if(fread(&_image->Width,4,1,file) != 1)
		ERROR_MACRO(1,"Error reading data");
	//wysokość obrazka
	if(fread(&_image->Height,4,1,file) != 1)
		ERROR_MACRO(1,"Error reading data");
		
	//liczba płaszczyzn
	fseek(file,2,SEEK_CUR);
	//liczba bitów na piksel
	if(fread(&bpp,2,1,file) != 1)
		ERROR_MACRO(1,"Error reading data");
		
	//dzielenie BPP przez 8 da nam liczbę bajtów
	size = _image->Width*_image->Height*bpp/8;
	_image->Data = (char*)malloc(sizeof(*_image->Data)*size);

	//inne dane
	fseek(file, 24, SEEK_CUR);
	//dane obrazka(piksele)
	if(fread(_image->Data,size,1,file) != 1)
		ERROR_MACRO(1,"Error reading data");
	//BMP przechowuje dane pikseli jako Blue,Green,Red
	//OpenGL wymaga danych w formacie Red,Green,Blue
	for(i = 0;i < size;i += 3){
		temp = _image->Data[i];
		_image->Data[i] = _image->Data[i+2];
		_image->Data[i+2] = temp;
	}
	fclose(file);
	return 0;
}


void  Close_Image(Image * _image){
    free(_image->Data);
}
/*!
 * Makro rysowania pola.@n
 * Pierwsza linijka dokonuje translacji o wektor o współczynnikach równych położeniu pola w przestrzeni.@n
 * Druga linijka dokonuje obrotu o 90 stopni na osi X, aby pole leżało na planszy.@n
 * Trzecia linijka rysuje okrąg.@n
 * @param[in] quadratic Obiekt typu GLUquadricObj
 * @param[in] array Wskanik na strukturę pól
 * @param[in] number Numer pola
 */
#define DRAW_FIELD(quadratic,array,number)	\
glTranslatef(Fields_Get_X(array,number),(float)(H/2+0.01),-Fields_Get_Y(array,number));	\
glRotatef(90.0f,1.0f,0.0f,0.0f); \
gluDisk(quadratic, 0, Fields_Get_Radius(array,number), 50, 10);
/*!
 * Wewnętrzna funkcja rysująca.@n
 * @param[in] _type Flaga zachowania funkcji
 * @param[in] _fields Wskaźnik na strukturę pól
 * @param[in] _players Wskaźnik na tablicę graczy
 * @param[in] _players_number Liczba graczy
 * @param[in] _randomized Wskaźnik na zmienną sygnalizująca ostatni wynik losowania
 * @param[in] _info Wskaźnik na strukturę danych animacji migania
 */
void _Draw(int _type,Fields_Structure * _fields,Player * _players,int _players_number,int * _randomized,Blink_Info * _info)
{
	static Fields_Structure * Fields = NULL;
	static Player * Players = NULL;
	static int Number_of_Players = 0;
	//nazwa listy wyświetlania
	static int List_Name;
	//kamera
	static float * Camera = NULL;
	//aktualny wskaźnik częstotliwości migania
	static int Frequency = -1;
	//ostatnio wylosowana pozycja
	static int * Randomized;
	//obiekt niezbędny do rysowania kwadryków(sfera,dysk,stożek)
	static GLUquadricObj * Quadric = NULL;
	//tekstury
	static unsigned int Texture[NUMBER_OF_TEXTURES];
	//migające pionki
	static int Blink_Pawns[4] = {-1,-1,-1,-1};
	//migające pole
	static int Blink_Field = -1;
	//aktywny gracz
	static int Active_Player = -1;
	//wartości na dwóch dodatkowych ściankach(na głównej wynik rzutu)
	static int Cube_Pips[2][6] = {{3,3,5,5,3,3},{2,6,6,1,1,5}};
	Image * Image1; 
	char buffer[STRING_SIZE+1];
	int i,j;
	//pierwsze uruchomienie funkcji
	if(!Camera)
	{
		Camera = malloc(sizeof(*Camera)*3);
		//początkowe ustawienie kamery
		Camera[0] = 45.0f;
		Camera[1] = -45.0f;
		Camera[2] = MIN_DISTANCE;
		Set_Camera(Camera);
		free(Camera);
		//generowanie tekstur
		glEnable(GL_TEXTURE_2D);
		Image1 = malloc(sizeof(*Image1));
		for(i = 0;i < NUMBER_OF_TEXTURES;i++){
			sprintf(buffer,"texture%d.bmp",i+1);
			Load_Image(Image1,buffer);
			glGenTextures(1, &Texture[i]);
			glBindTexture(GL_TEXTURE_2D, Texture[i]);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT); 
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
			glTexImage2D(GL_TEXTURE_2D, 0, 3, Image1->Width, Image1->Height, 0, GL_RGB, GL_UNSIGNED_BYTE, Image1->Data);
			Close_Image(Image1);
		}
		glDisable(GL_TEXTURE_2D);
		free(Image1);
		//generowanie list wyświetlania
		List_Name = glGenLists(3);
		//rysuje planszę
		glNewList(List_Name, GL_COMPILE);
			glBegin(GL_QUADS);
				glColor3f(0.2f,0.27f,0.08f);
				for(i=1;i<3 ;i++){
					glVertex3f(-W/2,H/2,D/2*POW(-1,i));
					glVertex3f(-W/2,-H/2,D/2*POW(-1,i));  
					glVertex3f(W/2,-H/2,D/2*POW(-1,i));
					glVertex3f(W/2,H/2,D/2*POW(-1,i));
				}
				for(i=1;i<3 ;i++){
					glVertex3f(POW(-1,i)*W/2,H/2,D/2);
					glVertex3f(POW(-1,i)*W/2,-H/2,D/2);  
					glVertex3f(POW(-1,i)*W/2,-H/2,-D/2);
					glVertex3f(POW(-1,i)*W/2,H/2,-D/2);
				}
			glEnd();
			//tekstura trawy
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, Texture[0]);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
			glBegin(GL_QUADS);
				glTexCoord2f(0.0f, 0.0f);	glVertex3f(-W/2,H/2,-D/2);
				glTexCoord2f(5.0f, 0.0f);	glVertex3f(W/2,H/2,-D/2);
				glTexCoord2f(5.0f, 5.0f);	glVertex3f(W/2,H/2,D/2);
				glTexCoord2f(0.0f, 5.0f);	glVertex3f(-W/2,H/2,D/2);
			glEnd();
			glDisable(GL_TEXTURE_2D);
		glEndList();
		
		// Rysuje pionek
		Quadric = gluNewQuadric();
		gluQuadricDrawStyle(Quadric, GLU_FILL);
		gluQuadricTexture(Quadric, GL_TRUE);
		gluQuadricNormals(Quadric, GLU_SMOOTH);
		glNewList(List_Name+1,GL_COMPILE);
			//podstawa pionka
			glTranslatef(0,H/2+0.01f,0);
			glRotatef(-90.0f,1.0f,0.0f,0.0f);
			gluCylinder(Quadric,PAWN_RADIUS,PAWN_RADIUS*0.2,PAWN_HEIGHT,32,32);
			//kulka na górze pionka
			glRotatef(90.0f,1.0f,0.0f,0.0f);
			glTranslatef(0.0f,(float)(PAWN_HEIGHT+PAWN_RADIUS/2),0.0f);
			glRotatef(-90.0f,1.0f,0.0f,0.0f);
			gluSphere(Quadric,PAWN_RADIUS/2,32,32);
		glEndList();
		gluDeleteQuadric(Quadric);
		
		//Rysuje kostkę
		glNewList(List_Name+2,GL_COMPILE);
			glBegin(GL_QUADS);

					glVertex3f(CUBE_SIZE/2,-CUBE_SIZE/2,-CUBE_SIZE/2);
					glVertex3f(-CUBE_SIZE/2,-CUBE_SIZE/2,-CUBE_SIZE/2);
					glVertex3f(-CUBE_SIZE/2,-CUBE_SIZE/2,CUBE_SIZE/2);
					glVertex3f(CUBE_SIZE/2,-CUBE_SIZE/2,CUBE_SIZE/2);

					glVertex3f(-CUBE_SIZE/2,CUBE_SIZE/2,-CUBE_SIZE/2);
					glVertex3f(CUBE_SIZE/2,CUBE_SIZE/2,-CUBE_SIZE/2);
					glVertex3f(CUBE_SIZE/2,-CUBE_SIZE/2,-CUBE_SIZE/2);
					glVertex3f(-CUBE_SIZE/2,-CUBE_SIZE/2,-CUBE_SIZE/2);

					glVertex3f(CUBE_SIZE/2,-CUBE_SIZE/2,-CUBE_SIZE/2);
					glVertex3f(CUBE_SIZE/2,CUBE_SIZE/2,-CUBE_SIZE/2);
					glVertex3f(CUBE_SIZE/2,CUBE_SIZE/2,CUBE_SIZE/2);
					glVertex3f(CUBE_SIZE/2,-CUBE_SIZE/2,CUBE_SIZE/2);
					
					glVertex3f(-CUBE_SIZE/2,-CUBE_SIZE/2,-CUBE_SIZE/2);
					glVertex3f(-CUBE_SIZE/2,CUBE_SIZE/2,-CUBE_SIZE/2);
					glVertex3f(-CUBE_SIZE/2,CUBE_SIZE/2,CUBE_SIZE/2);
					glVertex3f(-CUBE_SIZE/2,-CUBE_SIZE/2,CUBE_SIZE/2);
					
					glVertex3f(-CUBE_SIZE/2,CUBE_SIZE/2,CUBE_SIZE/2);
					glVertex3f(CUBE_SIZE/2,CUBE_SIZE/2,CUBE_SIZE/2);
					glVertex3f(CUBE_SIZE/2,-CUBE_SIZE/2,CUBE_SIZE/2);
					glVertex3f(-CUBE_SIZE/2,-CUBE_SIZE/2,CUBE_SIZE/2);
					
					glVertex3f(CUBE_SIZE/2,CUBE_SIZE/2,-CUBE_SIZE/2);
					glVertex3f(-CUBE_SIZE/2,CUBE_SIZE/2,-CUBE_SIZE/2);
					glVertex3f(-CUBE_SIZE/2,CUBE_SIZE/2,CUBE_SIZE/2);
					glVertex3f(CUBE_SIZE/2,CUBE_SIZE/2,CUBE_SIZE/2);
			glEnd();
			glColor3f(0.0f,0.0f,0.0f);
			//linie wyznaczające brzegi kostki
			glBegin(GL_LINES);
				glVertex3f(-CUBE_SIZE/2,CUBE_SIZE/2,CUBE_SIZE/2);
				glVertex3f(-CUBE_SIZE/2,CUBE_SIZE/2,-CUBE_SIZE/2);
				glVertex3f(-CUBE_SIZE/2,CUBE_SIZE/2,CUBE_SIZE/2);
				glVertex3f(CUBE_SIZE/2,CUBE_SIZE/2,CUBE_SIZE/2);
				glVertex3f(-CUBE_SIZE/2,CUBE_SIZE/2,CUBE_SIZE/2);
				glVertex3f(-CUBE_SIZE/2,-CUBE_SIZE/2,CUBE_SIZE/2);
			glEnd();
			glColor3f(1.0f,1.0f,1.0f);
		glEndList();
	}
	//inicjalizacja lub ustawienie danych migania
	if(_type != GL_SELECT && _type != GL_RENDER)
	{
		if(_fields)
		{
			Fields = _fields;
			Players = _players;
			Number_of_Players = _players_number;
			Randomized = _randomized;
		}
		else if(_info)
		{
			Active_Player = _info->Player_Number;
			//miganie pionków
			if(_info->Field_Number != -1)
			{
				Blink_Field = _info->Field_Number;
				*Blink_Pawns = *_info->Pawns_Numbers;
			}
			//migania pola
			else
			{
				for(i = 0;i < NUMBER_OF_PAWNS;i++){
					if(_info->Pawns_Numbers[i] != -1)
						Blink_Pawns[i] = 1;
					else
						Blink_Pawns[i] = 0;
				}
				Blink_Field = -1;
			}
			//włączenie migania
			Frequency = 0;
		}
		//wyłączenie migania
		else
		{
			Active_Player = -1;
			Blink_Field = -1;
			Frequency = -1;
		}
	}
	else
	{
		/* renderujemy gdy:
		 * - zmienia się stan migania(pełny-przeźroczysty) i trzeba narysować ponownie
		 * - renderowanie w trybie analizy kliknięcia
		 * - gra sygnalizuje, że doszło do zmiany wymagającej przerysowania ekranu
		 */
		if(!(Frequency % FREQUENCY)||_type==GL_SELECT||Check_Change())
		{
			glMatrixMode(GL_MODELVIEW);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
			glLoadIdentity();
			Quadric = gluNewQuadric();
			gluQuadricDrawStyle(Quadric, GLU_FILL);
			gluQuadricTexture(Quadric, GL_TRUE);
			gluQuadricNormals(Quadric, GLU_SMOOTH);
			
			
			//Ustawienie kamery
			Camera = Get_Camera();
			glTranslatef(0.0f,0.0f,-Camera[2]);
			glRotatef(Camera[0],1.0,0.0,0.0);
			glRotatef(Camera[1],0.0,1.0,0.0);
			free(Camera);
			
			glColor3f(1.0f,1.0f,1.0f);
			//rysowanie planszy
			if(_type == GL_SELECT)
				//podanie nazwy rysowanego obiektu, niezbędne w trybie analizy kliknięcia
				glPushName(BOARD_HIT);
			glCallList(List_Name);
			//włączenie teksturowania
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, Texture[1]);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
			
			//rysowanie pól, gdy nie jest wymagane rysowanie któregoś pola przeźroczystego
			if(Active_Player == -1 || Frequency < FREQUENCY || Blink_Field == -1)
			{			
				for(i = 0;i < Fields->Number_of_Fields;i++){
					if(_type == GL_SELECT)
						glLoadName(i+1);
					glColor3f(1.0f,1.0f,1.0f);
					glPushMatrix();
					DRAW_FIELD(Quadric,Fields,i);
					glPopMatrix();
				}
			}
			//rysowanie pola z pominięciem tego, które ma być rysowane jako przeźroczyste
			else
			{
				for(i = 0;i < Fields->Number_of_Fields;i++){
					if(_type == GL_SELECT)
						glLoadName(i+1);
					if(i != Blink_Field)
					{
						glColor3f(1.0f,1.0f,1.0f);
						glPushMatrix();
						DRAW_FIELD(Quadric,Fields,i);
						glPopMatrix();
					}
				}
			}
			glDisable(GL_TEXTURE_2D);
			
			//rysowanie pionków
			for(i = 0;i < Number_of_Players;i++){
				
				for(j = 0;j < NUMBER_OF_PAWNS;j++){
					//rysowanie z pominięciem przeźroczystych pionków
					if( !(Frequency >= FREQUENCY && Active_Player == i &&\
						( (Blink_Field != -1 && *Blink_Pawns == j) ||\
						 (Blink_Field == -1 && Blink_Pawns[j]) ) ) )
					{
						glPushMatrix();
						if(_type == GL_SELECT)
							glLoadName(100+i*10+j);
						glColor3f(Players->Color[0],Players->Color[1],Players->Color[2]);
						glTranslatef(Fields_Get_X(Fields,Players->Position[j]),0,-Fields_Get_Y(Fields,Players->Position[j]));
						glCallList(List_Name+1);
						glPopMatrix();
					}
				}
				Players++;
			}
			Players -= Number_of_Players;
			//włączenie blendowania, niezbędne do rysowania przeźroczystego
			glEnable(GL_BLEND);
			glDepthMask(GL_FALSE);
			glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
			glColor4f(1.0f,1.0f,1.0f,0.5f);
			
			if(Frequency >= FREQUENCY)
			{
				//"migające" pionki
				if(Blink_Field == -1)
				{	
					for(i = 0;i < NUMBER_OF_PAWNS;i++){
						if(Blink_Pawns[i])
						{
							if(_type == GL_SELECT)
								glLoadName(100+Active_Player*10+i);
							glPushMatrix();
							glTranslatef(Fields_Get_X(Fields,Players[Active_Player].Position[i]),0,-Fields_Get_Y(Fields,Players[Active_Player].Position[i]));
							glCallList(List_Name+1);
							glPopMatrix();
						}
					}
				}
				//migające pole
				else
				{
					glPushMatrix();
					if(_type == GL_SELECT)
						glLoadName(100+Active_Player*10+*Blink_Pawns);
					glTranslatef(Fields_Get_X(Fields,Players[Active_Player].Position[*Blink_Pawns]),0,-Fields_Get_Y(Fields,Players[Active_Player].Position[*Blink_Pawns]));
					glCallList(List_Name+1);
					glPopMatrix();
					glEnable(GL_TEXTURE_2D);
					glBindTexture(GL_TEXTURE_2D, Texture[1]);
					glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
					glPushMatrix();
					if(_type == GL_SELECT)
						glLoadName(Blink_Field+1);
					DRAW_FIELD(Quadric,Fields,Blink_Field);
					glDisable(GL_TEXTURE_2D);
					glPopMatrix();
				}
			}
			//rysowanie kostki
			if(_type == GL_SELECT)
				glLoadName(CUBE_HIT);
			glLoadIdentity();
			glTranslatef(CUBE_X,CUBE_Y,CUBE_Z);
			glRotatef(20.0f,1.0f,0.0f,0.0f);
			glRotatef(15.0f,0.0f,1.0f,0.0f);
			glCallList(List_Name+2);
			glColor3f(0.0f,0.0f,0.0f);
			//rysowanie oczek na bokach kostek
			glPushMatrix();
				glTranslatef(0.0f,CUBE_SIZE/2+0.01f,0.0f);
				glRotatef(-90.0f,1.0f,0.0f,0.0f);
				Draw_Cube_Pips(CUBE_SIZE/(float)10,*Randomized);
			glPopMatrix();
			glPushMatrix();
				glTranslatef(0.0f,0.0f,CUBE_SIZE/2+0.01f);
				Draw_Cube_Pips(CUBE_SIZE/(float)10,Cube_Pips[1][*Randomized-1]);
			glPopMatrix();
			glPushMatrix();
				glTranslatef(-CUBE_SIZE/2-0.01f,0.0f,0.0f);
				glRotatef(-90.0f,0.0f,1.0f,0.0f);
				Draw_Cube_Pips(CUBE_SIZE/10,Cube_Pips[0][*Randomized-1]);
			glPopMatrix();
			
			if(_type == GL_SELECT)
				glPopName();
			
			glDepthMask(GL_TRUE);
			glDisable(GL_BLEND);
			gluDeleteQuadric(Quadric);
			if(_type != GL_SELECT)
				Draw_Text(_type);
		}
		if(Frequency != -1 && _type != GL_SELECT)
		{
			++Frequency;
			Frequency %= 2*FREQUENCY;
		}
	}
    glutSwapBuffers();
}

#undef DRAW_FIELD

void Disable_Blink()
{
	_Draw(0,NULL,NULL,0,NULL,NULL);
}

void Draw_Init(Fields_Structure * _fields,Player * _players,int _players_number, int * _randomized)
{
	if(_fields != NULL && _players != NULL && _players_number >= 0)
		_Draw(0,_fields,_players,_players_number,_randomized,NULL);
}
/*!
 * Wewnętrzna funkcja rysująca teksty.@n
 * @param[in] _type Flaga zachowania funkcji
 * @param[in] _text Wskaźnik na strukturę tekstu
 * @param[in] _name Nazwa tekstu
 */
void _Draw_Text(int _type,Text * _text,const char * _name)
{
	static Array * Texts = NULL;
	Iterator * it;
	Text * Ptr;
	int len,bitmap_w,bitmap_h,i,width,height;
	//inicjalizacja
	if(_type == TEXT_INIT)
		Texts = Create_Array();
	//dodanie tekstu
	else if(_type == TEXT_ADD)
	{
		Add_Element(Texts,_name,_text,1,ARRAY_TEXT);
		free(_text);
	}
	//usunięcie tekstu z tablicy
	else if(_type == TEXT_REMOVE)
	{
		Iterator * it = Find_Element(Texts,_name);
		if(it->Position)
		{
			if(((Text*)it->Position->Value)->String)
				free(((Text*)it->Position->Value)->String);
			Erase(it);
		}
		Delete_Iterator(it);
	}
	//wyczyszczenie zaalokowanej pamięci
	else if(_type == TEXT_CLEAN)
	{
		Iterator * it = Create_Iterator(Texts);
		Get_First(it);
		while(1){
			if(((Text*)(it->Position->Value))->String)
				free(((Text*)(it->Position->Value))->String);
			if(!it->Position->Next)
				break;
			Get_Next(it);
		}
		Delete_Iterator(it);
		Free_Array(Texts);
	}
	//rysowanie tekstów
	else
	{
		if(Texts)
		{
			it = Create_Iterator(Texts);
			Get_First(it);
			if(it->Position)
			{
				if(_type != GL_SELECT)
				{
					glMatrixMode(GL_PROJECTION);
					glPushMatrix();
					glLoadIdentity();
				}
				width = glutGet(GLUT_WINDOW_WIDTH);
				height = glutGet(GLUT_WINDOW_HEIGHT);
				//rysujemy na macierzy ekranu dwuwymiarowego
				gluOrtho2D(0, width,height, 0);
				glMatrixMode(GL_MODELVIEW);
				glPushMatrix();
				glLoadIdentity();
				if(_type == GL_SELECT)
					glPushName(0);
				while(1){
					Ptr = (Text*)Get_Value(it);
					len = strlen(Ptr->String);
					bitmap_w = 0;
					//musimy wyrenderować dodatkowy prostokąt(patrz niżej)
					//lub renderujemy napis wycentrowany, więc musimy znać jego szerokość
					if(_type == GL_SELECT || Ptr->Position == TEXT_CENTER)
					{
						bitmap_h = Font_Height(Ptr->Font);
						for(i = 0;i < len;i++)
							bitmap_w += glutBitmapWidth((void*)Ptr->Font,Ptr->String[i]);
					}
					if(_type == GL_SELECT)
					{
						//stworzenie prostokąta odwzorowującego położenie napisu na ekranie
						//kliknięcie na niego sygnalizuje kliknięcie na napis
						glLoadName(Ptr->Select_Number);
						glBegin(GL_TRIANGLE_STRIP);
							if(Ptr->Position == TEXT_CENTER)
							{
								glVertex2f(Ptr->X*width-bitmap_w/2, Ptr->Y*height);
								glVertex2f(Ptr->X*width-bitmap_w/2, Ptr->Y*height-bitmap_h);
								glVertex2f(Ptr->X*width+bitmap_w/2, Ptr->Y*height);
								glVertex2f(Ptr->X*width+bitmap_w/2, Ptr->Y*height-bitmap_h);
							}
							else
							{
								glVertex2f(Ptr->X*width, Ptr->Y*height);
								glVertex2f(Ptr->X*width, Ptr->Y*height-bitmap_h);
								glVertex2f(Ptr->X*width+bitmap_w, Ptr->Y*height);
								glVertex2f(Ptr->X*width+bitmap_w, Ptr->Y*height-bitmap_h);
							}
						glEnd();
					}
					glColor3f(1.0f,1.0f,1.0f);
					//ustawienie pozycji napisu
					glRasterPos2f(Ptr->X*width-bitmap_w/2, Ptr->Y*height);
					//rysowanie wszystkich liter po kolei
					for (i = 0; i < len; i++) {
						glutBitmapCharacter(Ptr->Font, Ptr->String[i]);
					}
					if(!it->Position->Next)
						break;
					Get_Next(it);
				}
				glMatrixMode(GL_PROJECTION);
				glPopMatrix();
				glMatrixMode(GL_MODELVIEW);
				glPopMatrix();
			}
			Delete_Iterator(it);
		}
	}
}

void Draw_Text(int _type)
{
	_Draw_Text(_type,NULL,NULL);
}

void Text_Init(int _width,int _height)
{
	_Draw_Text(TEXT_INIT,NULL,NULL);
}
void Text_Add(Text * _text,const char * _name)
{
	_Draw_Text(TEXT_ADD,_text,_name);
}
void Text_Remove(const char * _name)
{
	_Draw_Text(TEXT_REMOVE,NULL,_name);
}
void Text_Clean()
{
	_Draw_Text(TEXT_CLEAN,NULL,NULL);
}

Text * Text_Create(float _x,float _y,int _select,const char * _string,void * _font)
{
	Text * ret = malloc(sizeof(*ret));
	ret->X = _x;
	ret->Y = _y;
	ret->Select_Number = _select;
	ret->Position = TEXT_NORMAL;
	ret->String = malloc(sizeof(*ret->String)*(strlen(_string)+1));
	strcpy(ret->String,_string);
	ret->Font = _font;
	return ret;
}

void Draw_Select()
{
	_Draw(GL_SELECT,NULL,NULL,0,NULL,NULL);
}

void Draw_Render()
{
	_Draw(GL_RENDER,NULL,NULL,0,NULL,NULL);
}

int Init_GL(int _width, int _height, char * _label,int _fullscr)
{
	int Window_Number;
	//inicjalizacja GLUTa
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);
	//ustawienie żądanych wymiarów okna
    glutInitWindowSize(_width, _height);
	//ustawienie pozycji początkowej okna
    glutInitWindowPosition(0, 0);
	//stworzenie okna
    Window_Number = glutCreateWindow(_label);
	//czyszczenie buforów(koloru, głęboci)
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    glViewport(0, 0, _width, _height);
	//macierz projekcji
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f,(GLfloat)_width/(GLfloat)_height,0.1f,100.0f);
	//macierz modelowania
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	glColor4f(1.0f, 1.0f, 1.0f, 0.5);
	glDisable(GL_BLEND);
	//renderowanie w oknie/trybie pełnoekranowym
	if(_fullscr == FULLSCREEN)
		glutFullScreen();
		
    /*glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);  // add lighting. (ambient)
    glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);  // add lighting. (diffuse).
    glLightfv(GL_LIGHT1, GL_POSITION,LightPosition); // set light position.
    glEnable(GL_LIGHT1);                            // turn light 1 on.
    glBlendFunc(GL_SRC_ALPHA,GL_ONE);			// Set The Blending Function For Translucency
    glColor4f(1.0f, 1.0f, 1.0f, 0.5); */  
	return Window_Number;
}

void Reshape_Window(int _width, int _height)
{
	//funkcja dokonuje zmian niezbędnych dla nowych wartości rozmiaru okna
	glViewport(0, 0, _width, _height);		

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(45.0f,(GLfloat)_width/(GLfloat)_height,0.1f,100.0f);
    glMatrixMode(GL_MODELVIEW);
}

void Change_Display(int _width,int _height)
{
	glutPositionWindow(0,0);
	glutReshapeWindow(_width,_height);
	glutPostRedisplay();
	Reshape_Window(_width,_height);
}

void Enable_FullScr()
{
	glutFullScreen();
}

void Blink_Set_Pawn(int _number,int * _pawns)
{
	Blink_Info * ptr = malloc(sizeof(*ptr));
	//funkcja ustawia miganie pionków
	ptr->Player_Number = _number;
	//tablica z informacją, które pionki mają migać
	ptr->Pawns_Numbers = _pawns;
	ptr->Field_Number = -1;
	_Draw(0,NULL,NULL,0,NULL,ptr);
	free(ptr);
}

void Blink_Set_Field(int _number, int _pawn, int _field)
{
	Blink_Info * ptr = malloc(sizeof(*ptr));
	//funkcja ustawia miganie pola
	ptr->Player_Number = _number;
	//numer migającego pionka
	ptr->Pawns_Numbers = &_pawn;
	//numer pola
	ptr->Field_Number = _field;
	_Draw(0,NULL,NULL,0,NULL,ptr);
	free(ptr);
}

void Fields_Generate_4_Players(Field * _pointer)
{
	float Width,Radius;
	int i,j,shift;
	
	Width = MIN(W,D)/2;
	Radius = FIELD_RADIUS;
	//"bazy"
	for(i = 0;i < 4;i++)
	{
			for(j = 0;j < NUMBER_OF_PAWNS;j++)
			{
				_pointer[i*2*NUMBER_OF_PAWNS+j].Position[0] = (float)(0.9*Width - Radius);
				_pointer[i*2*NUMBER_OF_PAWNS+j].Position[1] = (float)(0.9*Width - Radius);
				_pointer[i*2*NUMBER_OF_PAWNS+j].Radius = Radius;
			}
	}
	for(i = 0;i < 4;i++)
		_pointer[i*2*NUMBER_OF_PAWNS+1].Position[0] -= 2*Radius;
	for(i = 0;i < 4;i++)
		_pointer[i*2*NUMBER_OF_PAWNS+2].Position[1] -= 2*Radius;
	for(i = 0;i < 4;i++)
	{
		_pointer[i*2*NUMBER_OF_PAWNS+3].Position[0] -= 2*Radius;
		_pointer[i*2*NUMBER_OF_PAWNS+3].Position[1] -= 2*Radius;
	}
	for(i = 0;i < NUMBER_OF_PAWNS;i++)
		_pointer[2*NUMBER_OF_PAWNS+i].Position[1] *= -1;		
	for(i = 0;i < NUMBER_OF_PAWNS;i++)
	{
		_pointer[4*NUMBER_OF_PAWNS+i].Position[0] *= -1;
		_pointer[4*NUMBER_OF_PAWNS+i].Position[1] *= -1;
	}
	for(i = 0;i < NUMBER_OF_PAWNS;i++)
		_pointer[6*NUMBER_OF_PAWNS+i].Position[0] *= -1;
		
	//mety - gracz 1 i 3
	for(i = 0;i < 2;i++)
	{
		for(j = 0;j < NUMBER_OF_PAWNS;j++)
		{
			_pointer[2*2*NUMBER_OF_PAWNS*i+NUMBER_OF_PAWNS+j].Position[0] = (float)(0.0f);
			_pointer[2*2*NUMBER_OF_PAWNS*i+NUMBER_OF_PAWNS+j].Position[1] = \
							(float)((0.9*Width - (2+j)*Radius*2)*POW(-1,i));
			_pointer[2*2*NUMBER_OF_PAWNS*i+NUMBER_OF_PAWNS+j].Radius = Radius;
		}
	}
	//mety - gracz 2 i 4
	for(i = 0;i < 2;i++)
	{
		for(j = 0;j < NUMBER_OF_PAWNS;j++)
		{
			_pointer[2*2*NUMBER_OF_PAWNS*i+3*NUMBER_OF_PAWNS+j].Position[0] = \
							(float)((0.9*Width - (2+j)*Radius*2)*POW(-1,i));
			_pointer[2*2*NUMBER_OF_PAWNS*i+3*NUMBER_OF_PAWNS+j].Position[1] = 0.0f;
			_pointer[2*2*NUMBER_OF_PAWNS*i+3*NUMBER_OF_PAWNS+j].Radius = Radius;
		}
	}
	shift = NUMBER_OF_PAWNS*2*4;
	//pola - gracz 1 i 3
	for(i = 0;i < 4;i += 2)
	{
		for(j = 0;j < NUMBER_OF_FIELDS_PER_PLAYER;j++)
		{
			_pointer[shift+i*NUMBER_OF_FIELDS_PER_PLAYER+j].Position[0] = \
								(float)(j*(0.9*Width-Radius)/8);
			_pointer[shift+i*NUMBER_OF_FIELDS_PER_PLAYER+j].Position[1] = \
								(float)((0.9*Width - Radius) - j* (0.9*Width-Radius)/8);
			_pointer[shift+i*NUMBER_OF_FIELDS_PER_PLAYER+j].Radius = Radius;
		}	
	}
	//pola gracz 2 i 4
	for(i = 0;i < 4;i += 2)
	{
		for(j = 0;j < NUMBER_OF_FIELDS_PER_PLAYER;j++)
		{
			_pointer[shift+(i+1)*NUMBER_OF_FIELDS_PER_PLAYER+j].Position[0] = \
								(float)((0.9*Width-Radius) - j*(0.9*Width-Radius)/8);
			_pointer[shift+(i+1)*NUMBER_OF_FIELDS_PER_PLAYER+j].Position[1] = \
								(float)(-j*(0.9*Width-Radius)/8);
			_pointer[shift+(i+1)*NUMBER_OF_FIELDS_PER_PLAYER+j].Radius = Radius;
		}	
	}
	//pola - gracz 3 
	for(i = 0;i < NUMBER_OF_FIELDS_PER_PLAYER;i++)
	{
		_pointer[shift+NUMBER_OF_FIELDS_PER_PLAYER*2+i].Position[0] *= -1;
		_pointer[shift+NUMBER_OF_FIELDS_PER_PLAYER*2+i].Position[1] *= -1;
	}
	//pola - gracz 4
	for(i = 0;i < NUMBER_OF_FIELDS_PER_PLAYER;i++)
	{
		_pointer[shift+NUMBER_OF_FIELDS_PER_PLAYER*3+i].Position[0] *= -1;
		_pointer[shift+NUMBER_OF_FIELDS_PER_PLAYER*3+i].Position[1] *= -1;
	}
}

void Fields_Generate_5_Players(Field * _pointer)
{
	float Width,Radius;
	int i,j,shift;
	
	Width = MIN(W,D)/2;
	Radius = FIELD_RADIUS;
	//"bazy"
	for(i = 0;i < 5;i++)
	{
			for(j = 0;j < NUMBER_OF_PAWNS;j++)
			{
				_pointer[i*2*NUMBER_OF_PAWNS+j].Position[0] = Width - Radius;
				_pointer[i*2*NUMBER_OF_PAWNS+j].Position[1] = Width - Radius;
				_pointer[i*2*NUMBER_OF_PAWNS+j].Radius = Radius;
			}
	}
	for(i = 0;i < 5;i++)
		_pointer[i*2*NUMBER_OF_PAWNS+1].Position[0] -= 2*Radius;
	for(i = 0;i < 5;i++)
		_pointer[i*2*NUMBER_OF_PAWNS+2].Position[1] -= 2*Radius;
	for(i = 0;i < 5;i++)
	{
		_pointer[i*2*NUMBER_OF_PAWNS+3].Position[0] -= 2*Radius;
		_pointer[i*2*NUMBER_OF_PAWNS+3].Position[1] -= 2*Radius;
	}
	for(i = 0;i < NUMBER_OF_PAWNS;i++)
	{
		_pointer[2*NUMBER_OF_PAWNS+i].Position[1] *= -1;	
		_pointer[2*NUMBER_OF_PAWNS+i].Position[1] += Radius*2*4;
	}
	
	for(i = 0;i < NUMBER_OF_PAWNS;i++)
	{
		_pointer[2*2*NUMBER_OF_PAWNS+i].Position[1] *= -1;	
		_pointer[2*2*NUMBER_OF_PAWNS+i].Position[0] = Radius*POW(-1,i);
	}
	
	for(i = 0;i < NUMBER_OF_PAWNS;i++)
	{
		_pointer[2*3*NUMBER_OF_PAWNS+i].Position[0] *= -1;	
		_pointer[2*3*NUMBER_OF_PAWNS+i].Position[1] *= -1;	
		_pointer[2*3*NUMBER_OF_PAWNS+i].Position[1] += Radius*2*4;
	}
	
	for(i = 0;i < NUMBER_OF_PAWNS;i++)
	{
		_pointer[2*4*NUMBER_OF_PAWNS+i].Position[0] *= -1;	
	}
	//2 i 4 gracz - mety
	for(i = 1;i < 4;i += 2)
	{
		for(j = 0;j < NUMBER_OF_PAWNS;j++)
		{
			_pointer[(1+2*i)*NUMBER_OF_PAWNS+j].Position[1] = 0.0f;
			_pointer[(1+2*i)*NUMBER_OF_PAWNS+j].Position[0] = (float)(0.8*Width - (2+j)*Radius*2);
			_pointer[(1+2*i)*NUMBER_OF_PAWNS+j].Radius = Radius;
		}
	}
	for(j = 0;j < NUMBER_OF_PAWNS;j++)
		_pointer[(1+3*2)*NUMBER_OF_PAWNS+j].Position[0] *= -1;
	//3 gracz
	for(i = 0;i < NUMBER_OF_PAWNS;i++)
	{
			_pointer[2*2*(NUMBER_OF_PAWNS+1)+i].Position[0] = 0.0f;
			_pointer[2*2*(NUMBER_OF_PAWNS+1)+i].Position[1] = (float)(-(0.7*Width - (2+i)*Radius*2));
			_pointer[2*2*(NUMBER_OF_PAWNS+1)+i].Radius = Radius;
	}
	//1 i 5 gracz
	for(i = 0;i < 2;i++)
	{
		for(j = 0;j < NUMBER_OF_PAWNS;j++)
		{
			_pointer[(4*2*i+2)*NUMBER_OF_PAWNS-j-1].Position[0] = POW(-1,i)*Radius*(j+1);
			_pointer[(4*2*i+2)*NUMBER_OF_PAWNS-j-1].Position[1] = 2*Radius*(j+1);
			_pointer[(4*2*i+2)*NUMBER_OF_PAWNS-j-1].Radius = Radius;
		}
	}
	shift = 2*NUMBER_OF_PAWNS*5;
	//pierwszy gracz - pola
	for(i = 0;i < NUMBER_OF_FIELDS_PER_PLAYER;i++)
	{
		
		if(i < 6)
			_pointer[shift+i].Position[0] = Radius*(NUMBER_OF_PAWNS+1) + 2*Radius*i;
		else
			_pointer[shift+i].Position[0] = _pointer[shift+i-1].Position[0] - Radius*(i-6);
		if(i < 6)
		_pointer[shift+i].Position[1] = 2*Radius*(NUMBER_OF_PAWNS+1)*\
				(NUMBER_OF_FIELDS_PER_PLAYER-i)/NUMBER_OF_FIELDS_PER_PLAYER + Radius;
		else
			_pointer[shift+i].Position[1] = _pointer[shift+i-1].Position[1] - 2*Radius;
		_pointer[shift+i].Radius = Radius;
	}
	shift += NUMBER_OF_FIELDS_PER_PLAYER;
	//2 gracz
	_pointer[shift].Position[0] = _pointer[3*NUMBER_OF_PAWNS].Position[0] + 2.5f*Radius;
	_pointer[shift].Position[1] = 0.0f;
	_pointer[shift].Radius = Radius;
	for(i = 1;i < NUMBER_OF_FIELDS_PER_PLAYER;i++)
	{
		if(i < 6)
			_pointer[shift+i].Position[0] = _pointer[shift+i-1].Position[0] - Radius;
		else
			_pointer[shift+i].Position[0] = _pointer[shift+i-1].Position[0] - 2*Radius;
		if(i < 6)
			_pointer[shift+i].Position[1] = _pointer[shift+i-1].Position[1] - 2*Radius;
		else
			_pointer[shift+i].Position[1] = _pointer[shift+i-1].Position[1];
		_pointer[shift+i].Radius = Radius;
	}
	shift += NUMBER_OF_FIELDS_PER_PLAYER;
	//3 gracz
	_pointer[shift].Position[0] = _pointer[shift-1].Position[0] - 3*Radius;
	_pointer[shift].Position[1] = _pointer[shift-1].Position[1];
	_pointer[shift].Radius = Radius;
	for(i = 1;i < NUMBER_OF_FIELDS_PER_PLAYER;i++)
	{
		_pointer[shift+i].Position[0] = _pointer[shift-i].Position[0]*-1;
		_pointer[shift+i].Position[1] = _pointer[shift-i].Position[1];
		_pointer[shift+i].Radius = Radius;
	}
	shift += NUMBER_OF_FIELDS_PER_PLAYER;
	//4 gracz
	_pointer[shift].Position[0] = _pointer[7*NUMBER_OF_PAWNS].Position[0] - 2.5f*Radius;
	_pointer[shift].Position[1] = 0.0f;
	_pointer[shift].Radius = Radius;
	for(i = 1;i < NUMBER_OF_FIELDS_PER_PLAYER;i++)
	{
		_pointer[shift+i].Position[0] = _pointer[shift-2*NUMBER_OF_FIELDS_PER_PLAYER-i].Position[0]*-1;
		_pointer[shift+i].Position[1] = _pointer[shift-2*NUMBER_OF_FIELDS_PER_PLAYER-i].Position[1];
		_pointer[shift+i].Radius = Radius;
	}
	shift += NUMBER_OF_FIELDS_PER_PLAYER;
	//5 gracz
	_pointer[shift].Position[0] = _pointer[shift - 4*NUMBER_OF_FIELDS_PER_PLAYER].Position[0]*-1;
	_pointer[shift].Position[1] = _pointer[shift - 4*NUMBER_OF_FIELDS_PER_PLAYER].Position[1];
	_pointer[shift].Radius = Radius;
	for(i = 1;i < 3;i++)
	{
		_pointer[shift+i].Position[0] = _pointer[shift+i-1].Position[0];
		_pointer[shift+i].Position[1] = _pointer[shift+i-1].Position[1] + 2*Radius;
		_pointer[shift+i].Radius = Radius;
	}
	for(i = 3;i < 6;i++)
	{
		_pointer[shift+i].Position[0] = _pointer[shift+i-1].Position[0] + 2.5f*Radius;
		_pointer[shift+i].Position[1] = _pointer[shift+i-1].Position[1];
		_pointer[shift+i].Radius = Radius;
	}
	for(i = 6;i < 8;i++)
	{
		_pointer[shift+i].Position[0] = _pointer[shift+i-1].Position[0];
		_pointer[shift+i].Position[1] = _pointer[shift+i-1].Position[1] - 2*Radius;
		_pointer[shift+i].Radius = Radius;
	}
}

void Fields_Generate_6_Players(Field * _pointer)
{
	float Width,Radius;
	int i,j,shift;
	
	Width = MIN(W,D)/2;
	Radius = FIELD_RADIUS;
	//bazy
	for(i = 0;i < 6;i++)
	{
			for(j = 0;j < NUMBER_OF_PAWNS;j++)
			{
				_pointer[i*2*NUMBER_OF_PAWNS+j].Position[0] = Width - Radius;
				_pointer[i*2*NUMBER_OF_PAWNS+j].Position[1] = Width - Radius;
				_pointer[i*2*NUMBER_OF_PAWNS+j].Radius = Radius;
			}
	}
	for(i = 0;i < 6;i++)
		_pointer[i*2*NUMBER_OF_PAWNS+1].Position[0] -= 2*Radius;
	for(i = 0;i < 6;i++)
		_pointer[i*2*NUMBER_OF_PAWNS+2].Position[1] -= 2*Radius;
	for(i = 0;i < 6;i++)
	{
		_pointer[i*2*NUMBER_OF_PAWNS+3].Position[0] -= 2*Radius;
		_pointer[i*2*NUMBER_OF_PAWNS+3].Position[1] -= 2*Radius;
	}
	
	for(i = 0;i < NUMBER_OF_PAWNS;i++)
		_pointer[2*NUMBER_OF_PAWNS+i].Position[1] *= -1;	
	
	for(i = 0;i < NUMBER_OF_PAWNS;i++)
	{
		_pointer[2*2*NUMBER_OF_PAWNS+i].Position[1] *= -1;	
		_pointer[2*2*NUMBER_OF_PAWNS+i].Position[0] = Radius*POW(-1,i);
	}
	
	for(i = 0;i < NUMBER_OF_PAWNS;i++)
	{
		_pointer[2*3*NUMBER_OF_PAWNS+i].Position[0] *= -1;	
		_pointer[2*3*NUMBER_OF_PAWNS+i].Position[1] *= -1;	
	}
	for(i = 0;i < NUMBER_OF_PAWNS;i++)
		_pointer[2*4*NUMBER_OF_PAWNS+i].Position[0] *= -1;
	
	for(i = 0;i < NUMBER_OF_PAWNS;i++)
		_pointer[2*5*NUMBER_OF_PAWNS+i].Position[0] = Radius*POW(-1,i);
		
	//1 i 2 gracz - mety
	for(i = 0;i < 2;i++)
	{
		for(j = 0;j < NUMBER_OF_PAWNS;j++)
		{
			_pointer[(2*i+2)*NUMBER_OF_PAWNS-j-1].Position[1] = POW(-1,i)*Radius*(j+1);
			_pointer[(2*i+2)*NUMBER_OF_PAWNS-j-1].Position[0] = 2*Radius*(j+1);
			_pointer[(2*i+2)*NUMBER_OF_PAWNS-j-1].Radius = Radius;
		}
	}
	//3 gracz
	for(i = 0;i < NUMBER_OF_PAWNS;i++)
	{
			_pointer[(2*2+1)*NUMBER_OF_PAWNS+i].Position[0] = 0.0f;
			_pointer[(2*2+1)*NUMBER_OF_PAWNS+i].Position[1] = (float)(-(0.7*Width - (2+i)*Radius*2));
			_pointer[(2*2+1)*NUMBER_OF_PAWNS+i].Radius = Radius;
	}
	//4 i 5 gracz
	for(i = 0;i < 2;i++)
	{
		for(j = 0;j < NUMBER_OF_PAWNS;j++)
		{
			_pointer[(8+2*i)*NUMBER_OF_PAWNS-j-1].Position[0] = \
					_pointer[(2*i+2)*NUMBER_OF_PAWNS-j-1].Position[0] * -1;
			_pointer[(8+2*i)*NUMBER_OF_PAWNS-j-1].Position[1] = \
						_pointer[(2*i+2)*NUMBER_OF_PAWNS-j-1].Position[1];
			_pointer[(8+2*i)*NUMBER_OF_PAWNS-j-1].Radius = Radius;
		}
	}
	//6 gracz
	for(i = 0;i < NUMBER_OF_PAWNS;i++)
	{
			_pointer[(2*5+1)*NUMBER_OF_PAWNS+i].Position[0] = 0.0f;
			_pointer[(2*5+1)*NUMBER_OF_PAWNS+i].Position[1] = (float)(0.7*Width - (2+i)*Radius*2);
			_pointer[(2*5+1)*NUMBER_OF_PAWNS+i].Radius = Radius;
	}
	shift = 2*NUMBER_OF_PAWNS*6;
	//1 gracz - pola
	_pointer[shift].Position[0] = 2*Radius*(NUMBER_OF_PAWNS+1);
	_pointer[shift].Position[1] = Radius*(NUMBER_OF_PAWNS+1);
	_pointer[shift].Radius = Radius;
	for(i = 1;i < 3;i++)
	{
		_pointer[shift+i].Position[0] = _pointer[shift+i-1].Position[0] + 2*Radius;
		_pointer[shift+i].Position[1] = _pointer[shift+i-1].Position[1];
		_pointer[shift+i].Radius = Radius;
	}
	for(i = 3;i < 6;i++)
	{
		_pointer[shift+i].Position[0] = _pointer[shift+i-1].Position[0];
		_pointer[shift+i].Position[1] = _pointer[shift+i-1].Position[1] - 2.5f*Radius;
		_pointer[shift+i].Radius = Radius;
	}
	for(i = 6;i < 8;i++)
	{
		_pointer[shift+i].Position[0] = _pointer[shift+i-1].Position[0] - 2*Radius;
		_pointer[shift+i].Position[1] = _pointer[shift+i-1].Position[1];
		_pointer[shift+i].Radius = Radius;
	}
	shift += NUMBER_OF_FIELDS_PER_PLAYER;
	//2 gracz
	for(i = 0;i < 4;i++)
	{
		_pointer[shift+i].Position[0] = _pointer[shift+i-1].Position[0];
		_pointer[shift+i].Position[1] = _pointer[shift+i-1].Position[1] - 2*Radius;
		_pointer[shift+i].Radius = Radius;
	}
	for(i = 4;i < 8;i++)
	{
		_pointer[shift+i].Position[0] = _pointer[shift+i-1].Position[0] - 2*Radius;
		_pointer[shift+i].Position[1] = _pointer[shift+i-1].Position[1];
		_pointer[shift+i].Radius = Radius;
	}
	shift += NUMBER_OF_FIELDS_PER_PLAYER;
	//3 gracz
	_pointer[shift].Position[0] = _pointer[shift-1].Position[0] - 2*Radius;
	_pointer[shift].Position[1] = _pointer[shift-1].Position[1];
	_pointer[shift].Radius = Radius;
	for(i = 1;i < NUMBER_OF_FIELDS_PER_PLAYER;i++)
	{
		_pointer[shift+i].Position[0] = _pointer[shift-i].Position[0]*-1;
		_pointer[shift+i].Position[1] = _pointer[shift-i].Position[1];
		_pointer[shift+i].Radius = Radius;
	}
	shift += NUMBER_OF_FIELDS_PER_PLAYER;
	//4 gracz
	for(i = 0;i < NUMBER_OF_FIELDS_PER_PLAYER;i++)
	{
		_pointer[shift+i].Position[0] = _pointer[shift - 2*NUMBER_OF_FIELDS_PER_PLAYER-i]\
									.Position[0]*-1;
		_pointer[shift+i].Position[1] = _pointer[shift - 2*NUMBER_OF_FIELDS_PER_PLAYER-i]\
									.Position[1];
		_pointer[shift+i].Radius = Radius;
	}
	shift += NUMBER_OF_FIELDS_PER_PLAYER;
	//5 i 6 gracz
	_pointer[shift].Position[0] = 2*Radius*(NUMBER_OF_PAWNS+1)*-1;
	_pointer[shift].Position[1] = Radius*(NUMBER_OF_PAWNS+1);
	_pointer[shift].Radius = Radius;
	for(i = 1;i < 4;i++)
	{
		_pointer[shift+i].Position[1] = _pointer[shift+i-1].Position[1] + 2*Radius;
		_pointer[shift+i].Position[0] = _pointer[shift+i-1].Position[0];
		_pointer[shift+i].Radius = Radius;
	}
	for(i = 4;i < 2*NUMBER_OF_FIELDS_PER_PLAYER-2;i++)
	{
		_pointer[shift+i].Position[1] = _pointer[shift+i-1].Position[1];
		_pointer[shift+i].Position[0] = _pointer[shift+i-1].Position[0] + 2*Radius;
		_pointer[shift+i].Radius = Radius;
	}
	for(i = 2*NUMBER_OF_FIELDS_PER_PLAYER-2;i < 2*NUMBER_OF_FIELDS_PER_PLAYER;i++)
	{
		_pointer[shift+i].Position[1] = _pointer[shift+i-1].Position[1] - 2*Radius;
		_pointer[shift+i].Position[0] = _pointer[shift+i-1].Position[0];
		_pointer[shift+i].Radius = Radius;
	}
	shift += NUMBER_OF_FIELDS_PER_PLAYER;
}

Fields_Structure * Fields_Generate(int _number_of_players){
	
	Fields_Structure * ret;

	if(!(_number_of_players >= 4 && _number_of_players <= 6))
		return NULL;

	ret = (Fields_Structure*) malloc(sizeof(*ret));
	//liczba pól na planszy
	ret->Number_of_Fields = (NUMBER_OF_PAWNS*2*_number_of_players+\
							_number_of_players*NUMBER_OF_FIELDS_PER_PLAYER);
	ret->Number_of_Players = _number_of_players;
	ret->Data = (Field*) malloc(sizeof(*(ret->Data))*ret->Number_of_Fields);
	switch(_number_of_players){
		case 4:
			Fields_Generate_4_Players(ret->Data);
		break;
		case 5:
			Fields_Generate_5_Players(ret->Data);
		break;
		case 6:
			Fields_Generate_6_Players(ret->Data);
		break;
	}
	
	return ret;
}	

void Fields_Close(Fields_Structure * _fields)
{
	free(_fields->Data);
	free(_fields);
}

float Fields_Get_Radius(Fields_Structure * _fields,int _number)
{
	return _fields->Data[_number].Radius;
}

float Fields_Get_X(Fields_Structure * _fields,int _number)
{
	return _fields->Data[_number].Position[0];
}

float Fields_Get_Y(Fields_Structure * _fields,int _number)
{
	return _fields->Data[_number].Position[1];
}

void Text_Draw(float _x,float _y,int _select_name,void *_font,int _position_type,\
				const char * _name,const char * _format,...)
{
	char * buffer;
	Text * ptr;
	//generowanie napisu za pomocą makra funkcji o n argumentach
	va_list ap;
	buffer = (char*) malloc(sizeof(*buffer)*2*STRING_SIZE);
	memset(buffer,0,sizeof(*buffer)*2*STRING_SIZE);
	va_start(ap,_format);
	while(*_format != '\0'){
		//zwykły znak
		if(*_format != '%')
			sprintf(buffer,"%s%c",buffer,*_format);
		else
		{
			switch(*++_format){
				//napis
				case 's':
					sprintf(buffer,"%s%s",buffer,va_arg(ap,char *));
				break;
				//liczba całkowita
				case 'd':
					sprintf(buffer,"%s%d",buffer,va_arg(ap,int));
				break;
				//znak
				case 'c':
					sprintf(buffer,"%s%c",buffer,va_arg(ap,int));
				break;
				//liczba zmiennoprzecinkowa
				case 'f':
					sprintf(buffer,"%s%f",buffer,va_arg(ap,double));
				break;
			}
		}
		_format++;
	}
	ptr = Text_Create(_x,_y,_select_name,buffer,_font);
	ptr->Position = _position_type;
	free(buffer);
	Text_Remove(_name);
	Text_Add(ptr,_name);
	Set_Change();
}

void Draw_Cube_Pips(float _radius, int _number)
{
	int i;
	
	GLUquadricObj * Quadric = gluNewQuadric();
	gluQuadricDrawStyle(Quadric, GLU_FILL);
	gluQuadricNormals(Quadric, GLU_SMOOTH);
	switch(_number){
		
		case 1:
			gluDisk(Quadric, 0, _radius, 50, 10);
		break;
		case 2:
			for(i = 0;i < 2;i++){
				glPushMatrix();
					glTranslatef(2*_radius*POW(-1,i),2*_radius*POW(-1,i),0.0f);
					gluDisk(Quadric, 0, _radius, 50, 10);
				glPopMatrix();
			}
		break;
		case 3:
			for(i = 0;i < 2;i++){
				glPushMatrix();
					glTranslatef(2*_radius*POW(-1,i),2*_radius*POW(-1,i),0.0f);
					gluDisk(Quadric, 0, _radius, 50, 10);
				glPopMatrix();
			}
			gluDisk(Quadric, 0, _radius, 50, 10);
		break;
		case 4:
			for(i = 0;i < 4;i++){
				glPushMatrix();
					glTranslatef(3*_radius*(!(i % 2) ? -1 : 1),3*_radius*(i > 1 ? -1 : 1),0.0f);
					gluDisk(Quadric, 0, _radius, 50, 10);
				glPopMatrix();
			}
		break;
		case 5:
			for(i = 0;i < 4;i++){
				glPushMatrix();
					glTranslatef(2*_radius*(!(i % 2) ? -1 : 1),2*_radius*(i > 1 ? -1 : 1),0.0f);
					gluDisk(Quadric, 0, _radius, 50, 10);
				glPopMatrix();
			}
			gluDisk(Quadric, 0, _radius, 50, 10);
		break;
		case 6:
			for(i = 0;i < 4;i++){
				glPushMatrix();
					glTranslatef(2*_radius*(!(i % 2) ? -1 : 1),3*_radius*(i > 1 ? -1 : 1),0.0f);
					gluDisk(Quadric, 0, _radius, 50, 10);
				glPopMatrix();
			}
			for(i = 0;i < 2;i++){
				glPushMatrix();
				glTranslatef(2*_radius*POW(-1,i),0.0f,0.0f);
				gluDisk(Quadric, 0, _radius, 50, 10);
				glPopMatrix();
			}
		break;
	}
	gluDeleteQuadric(Quadric);
}

int Font_Height(void * _font)
{
	if(_font == GLUT_BITMAP_9_BY_15)
		return 15;
	else if(_font == GLUT_BITMAP_TIMES_ROMAN_24)
		return 24;
	else
		return -1;
}