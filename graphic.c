#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <GL/glut.h>
#include <math.h>
#include <assert.h>
#include "graphic.h"
#include "game.h"

void Text_Draw(int _x,int _y,int _select_name,void *_font,int _position_type,\
				const char * _name,const char * _format,...);

float * Camera_Action(int _type,float * _camera,int _action)
{
	static float Camera[3];
	static float * pt;
	switch(_type){
		case CAMERA_SET:
			memcpy(Camera,_camera,sizeof(*Camera)*3);
		break;
		case CAMERA_GET:
			pt = malloc(sizeof(*pt)*3);
			memcpy(pt,Camera,sizeof(*Camera)*3);
			return pt;
		break;
		case CAMERA_CHANGE:
			switch(_action){
				case UP:
					Camera[0] += CAMERA_SPEED;
				break;
				case DOWN:
					Camera[0] -= CAMERA_SPEED;
				break;
				case LEFT:
					Camera[1] += CAMERA_SPEED;
				break;
				case RIGHT:
					Camera[1] -= CAMERA_SPEED;
				break;
				case FARTHER:
					Camera[2] += CAMERA_SPEED;
				break;
				case CLOSER:
					Camera[2] -= CAMERA_SPEED;
				break;
			}
			Interval(&Camera[0],0,90);
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

void Interval(float * number, int min,int max){

    if (*number < min)
        *number = min;
    else if(*number > max)
        *number = max;

}
int Load_Image(Image * _image,const char * _path){

	FILE * file;
	uint8_t bpp,temp;
	size_t size;
	int i;

	file = fopen(_path,"rb");
	if(!file)
		ERROR(1,"Bitmap doesn't exist");
	/** First part of header */
	fseek(file,18,SEEK_CUR);

	if(fread(&_image->Width,4,1,file) != 1)
		ERROR(1,"Error reading data");
	if(fread(&_image->Height,4,1,file) != 1)
		ERROR(1,"Error reading data");
		
	/** number of planes - unnecessary */
	fseek(file,2,SEEK_CUR);
	if(fread(&bpp,2,1,file) != 1)
		ERROR(1,"Error reading data");
		
	/**bpp is size of data for every pixel in bites
		dividing it by 8 gives size in bytes */
	size = _image->Width*_image->Height*bpp/8;
	_image->Data = malloc(size);

	if(!_image->Data)
		ERROR(1,"Error during memory allocation");
	/**other data */
	fseek(file, 24, SEEK_CUR);

	if(fread(_image->Data,size,1,file) != 1)
		ERROR(1,"Error reading data");
	/** for some strange reason bitmap contains
	 * colors as blue,green,red */
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

void _Draw(int _type,Field * _fields,Player * _players,int _players_number,int * _randomized,Blink_Info * _info)
{
	static Field * Fields = NULL;
	static Player * Players = NULL;
	static int Number_of_Players = 0;
	static int init = 0;
	static int List_Name;
	static float * Camera;
	static int Frequency = -1;
	static int * Randomized;
	static GLUquadricObj *quadratic = NULL;
	/** \var static unsigned int Texture[3];
    *	\brief Contains textures
	* 	0 - grass
	* 	1 - circle of field
	*/
	static unsigned int Texture[3];
	static int Blink_Pawns[4] = {-1,-1,-1,-1};
	static int Blink_Field = -1;
	static int Active_Player = -1;
	static int Cube_Pips[2][6] = {{3,3,5,5,3,3},{2,6,6,1,1,5}};
	char buffer[STRING_SIZE+1];
	int i,j;
	
	if(!init)
	{
		Camera = malloc(sizeof(*Camera)*3);
		init = 1;
		Camera[0] = 45.0f;
		Camera[1] = -45.0f;
		Camera[2] = MIN_DISTANCE;
		Set_Camera(Camera);
		free(Camera);
		
		glEnable(GL_TEXTURE_2D);
		Image * Image1 = malloc(sizeof(*Image1));
		for(i = 0;i < NUMBER_OF_TEXTURES;i++){
			sprintf(buffer,"texture%d.bmp",i+1);
			Load_Image(Image1,buffer);
			glGenTextures(1, &Texture[0]);
			glBindTexture(GL_TEXTURE_2D, Texture[0]);   // 2d texture (x and y size)
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); // cheap scaling when image bigger than texture
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT); // cheap scaling when image bigger than texture
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT); // cheap scaling when image smalled than texture
			glTexImage2D(GL_TEXTURE_2D, 0, 3, Image1->Width, Image1->Height, 0, GL_RGB, GL_UNSIGNED_BYTE, Image1->Data);
			Close_Image(Image1);
		}
		glDisable(GL_TEXTURE_2D);
		free(Image1);
		
		List_Name = glGenLists(3);
		/** Board */
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
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, Texture[0]);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
			glBegin(GL_QUADS);
				glTexCoord2f(0.0f, 0.0f);	glVertex3f(-W/2,H/2,-D/2);
				glTexCoord2f(5.0f, 0.0f);	glVertex3f(W/2,H/2,-D/2);
				glTexCoord2f(5.0f, 5.0f);	glVertex3f(W/2,H/2,D/2);
				glTexCoord2f(0.0f, 5.0f);	glVertex3f(-W/2,H/2,D/2);
			glEnd();
			//glFlush();
			glDisable(GL_TEXTURE_2D);
		glEndList();
		
		/** Draws pawn */
		quadratic = gluNewQuadric();
		gluQuadricNormals(quadratic, GLU_SMOOTH);
		glNewList(List_Name+1,GL_COMPILE);
			glTranslatef(0,H/2+0.01f,0);
			glRotatef(-90.0f,1.0f,0.0f,0.0f);
			gluCylinder(quadratic,PAWN_RADIUS,PAWN_RADIUS*0.2,PAWN_HEIGHT,32,32);
	
			glRotatef(90.0f,1.0f,0.0f,0.0f);
			glTranslatef(0.0f,PAWN_HEIGHT+PAWN_RADIUS/2,0.0f);//PAWN_RADIUS*3/2,0.0f);
	
			glRotatef(-90.0f,1.0f,0.0f,0.0f);
			gluSphere(quadratic,PAWN_RADIUS/2,32,32);
		glEndList();
		free(quadratic);
		
		/** Draws cube */
		glNewList(List_Name+2,GL_COMPILE);
			glBegin(GL_QUADS);/*
				for(i=1;i<3;i++){
					glVertex3f(-CUBE_SIZE/2,CUBE_SIZE/2,-CUBE_SIZE/2*POW(-1,i));
					glVertex3f(CUBE_SIZE/2,CUBE_SIZE/2,-CUBE_SIZE/2*POW(-1,i));
					glVertex3f(CUBE_SIZE/2,-CUBE_SIZE/2,-CUBE_SIZE/2*POW(-1,i));
					glVertex3f(-CUBE_SIZE/2,-CUBE_SIZE/2,-CUBE_SIZE/2*POW(-1,i));
				}
				for(i=1;i<3;i++){
					glVertex3f(CUBE_SIZE/2*POW(-1,i+1),-CUBE_SIZE/2,-CUBE_SIZE/2);
					glVertex3f(CUBE_SIZE/2*POW(-1,i+1),CUBE_SIZE/2,-CUBE_SIZE/2);
					glVertex3f(CUBE_SIZE/2*POW(-1,i+1),CUBE_SIZE/2,CUBE_SIZE/2);
					glVertex3f(CUBE_SIZE/2*POW(-1,i+1),-CUBE_SIZE/2,CUBE_SIZE/2);
				}

				
				//glColor3f(0.5f,0.5f,0.5f);
				for(i=1;i<3;i++){
					glVertex3f(CUBE_SIZE/2,CUBE_SIZE/2*POW(-1,i),-CUBE_SIZE/2);
					glVertex3f(-CUBE_SIZE/2,CUBE_SIZE/2*POW(-1,i),-CUBE_SIZE/2);
					glVertex3f(-CUBE_SIZE/2,CUBE_SIZE/2*POW(-1,i),CUBE_SIZE/2);
					glVertex3f(CUBE_SIZE/2,CUBE_SIZE/2*POW(-1,i),CUBE_SIZE/2);
				}*///tył
					glVertex3f(CUBE_SIZE/2,-CUBE_SIZE/2,-CUBE_SIZE/2);
					glVertex3f(-CUBE_SIZE/2,-CUBE_SIZE/2,-CUBE_SIZE/2);
					glVertex3f(-CUBE_SIZE/2,-CUBE_SIZE/2,CUBE_SIZE/2);
					glVertex3f(CUBE_SIZE/2,-CUBE_SIZE/2,CUBE_SIZE/2);
					//tył lewy
					glVertex3f(-CUBE_SIZE/2,CUBE_SIZE/2,-CUBE_SIZE/2);
					glVertex3f(CUBE_SIZE/2,CUBE_SIZE/2,-CUBE_SIZE/2);
					glVertex3f(CUBE_SIZE/2,-CUBE_SIZE/2,-CUBE_SIZE/2);
					glVertex3f(-CUBE_SIZE/2,-CUBE_SIZE/2,-CUBE_SIZE/2);
					//tył prawy
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
			if(_info->Field_Number != -1)
			{
				Blink_Field = _info->Field_Number;
				*Blink_Pawns = *_info->Pawns_Numbers;
			}
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
			Frequency = 0;
		}
		else
		{
			Active_Player = -1;
			Blink_Field = -1;
			Frequency = -1;
		}
	}
	else
	{
		if(!(Frequency % FREQUENCY)||_type==GL_SELECT||Check_Change())
		{
			
		glMatrixMode(GL_MODELVIEW);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
		glLoadIdentity();
		quadratic = gluNewQuadric();
		gluQuadricNormals(quadratic, GLU_SMOOTH);
		
		
		/** Camera */
		Camera = Get_Camera();
		//glTranslatef(0.0f,1.5f,0.0f);
		glTranslatef(0.0f,0.0f,-Camera[2]);
		glRotatef(Camera[0],1.0,0.0,0.0);
		glRotatef(Camera[1],0.0,1.0,0.0);
		free(Camera);
		
		glColor3f(1.0f,1.0f,1.0f);
		/** Board */
		if(_type == GL_SELECT)
			glPushName(BOARD_HIT);
		glCallList(List_Name);
		
		/** Fields */
		if(Active_Player == -1 || Frequency < FREQUENCY || Blink_Field == -1)
		{			
			for(i = 0;i < FIELDS_NUMBER_4_PLAYERS;i++){
				if(_type == GL_SELECT)
					glLoadName(i+1);
				glColor3f(1.0f,1.0f,1.0f);
				if(Fields[i].Radius)
					Draw_Circle(Fields[i].Radius,Fields[i].Position[0],H/2+0.01,-Fields[i].Position[1]);
			}
		}
		else
		{
			for(i = 0;i < FIELDS_NUMBER_4_PLAYERS;i++){
				if(_type == GL_SELECT)
					glLoadName(i+1);
				glColor3f(1.0f,1.0f,1.0f);
				if(Fields[i].Radius && i != Blink_Field)
					Draw_Circle(Fields[i].Radius,Fields[i].Position[0],H/2+0.01,-Fields[i].Position[1]);
			}
		}
	
		/** Pawns */
		for(i = 0;i < Number_of_Players;i++){
			
			for(j = 0;j < NUMBER_OF_PAWNS;j++){
					
				if( !(Frequency >= FREQUENCY && Active_Player == i &&\
					( (Blink_Field != -1 && *Blink_Pawns == j) ||\
					 (Blink_Field == -1 && Blink_Pawns[j]) ) ) )
				{
					glPushMatrix();
					if(_type == GL_SELECT)
						glLoadName(100+i*10+j);
					glColor3f(Players->Color[0],Players->Color[1],Players->Color[2]);
					glTranslatef(Fields[Players->Position[j]].Position[0],0,-Fields[Players->Position[j]].Position[1]);
					glCallList(List_Name+1);
					glPopMatrix();
				}
			}
			Players++;
		}
		
		Players -= Number_of_Players;
		glEnable(GL_BLEND);
		glDepthMask(GL_FALSE);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		//glDisable(GL_DEPTH_TEST);
		glColor4f(1.0f,1.0f,1.0f,0.5f);
		
		if(Frequency >= FREQUENCY)
		{
			if(Blink_Field == -1)
			{	
				for(i = 0;i < NUMBER_OF_PAWNS;i++){
					if(Blink_Pawns[i])
					{
						if(_type == GL_SELECT)
							glLoadName(100+Active_Player*10+i);
						glPushMatrix();
						//glColor3f(Players[Active_Player].Color[0],Players[Active_Player].Color[1],Players[Active_Player].Color[2]);
						glTranslatef(Fields[Players[Active_Player].Position[i]].Position[0],0,-Fields[Players[Active_Player].Position[i]].Position[1]);
						glCallList(List_Name+1);
						glPopMatrix();
					}
				}
			}
			else
			{
				glPushMatrix();
				if(_type == GL_SELECT)
					glLoadName(100+Active_Player*10+*Blink_Pawns);
				//glColor3f(Players[Active_Player].Color[0],Players[Active_Player].Color[1],Players[Active_Player].Color[2]);
				glTranslatef(Fields[Players[Active_Player].Position[*Blink_Pawns]].Position[0],0,-Fields[Players[Active_Player].Position[*Blink_Pawns]].Position[1]);
				glCallList(List_Name+1);
				glPopMatrix();
				glPushMatrix();
				if(_type == GL_SELECT)
					glLoadName(Blink_Field+1);
				//glColor3f(1.0f,1.0f,1.0f);
				if(Fields[i].Radius)
					Draw_Circle(Fields[Blink_Field].Radius,Fields[Blink_Field].Position[0],H/2+0.01,-Fields[Blink_Field].Position[1]);
				glPopMatrix();
			}
		}
		/** Cube */
		if(_type == GL_SELECT)
			glLoadName(CUBE_HIT);
		glLoadIdentity();
		glTranslatef(CUBE_X,CUBE_Y,CUBE_Z);
		glRotatef(20.0f,1.0f,0.0f,0.0f);
		glRotatef(15.0f,0.0f,1.0f,0.0f);
		glCallList(List_Name+2);
		glColor3f(0.0f,0.0f,0.0f);
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
		
		//glEnable(GL_DEPTH_TEST);
		free(quadratic);
		if(_type != GL_SELECT)
			Draw_Text(_type);
		}
		if(Frequency != -1 && _type != GL_SELECT)
			Frequency = ++Frequency % (2*FREQUENCY);
	}
    glutSwapBuffers();
}

void Disable_Blink()
{
	_Draw(0,NULL,NULL,0,NULL,NULL);
}

void Draw_Init(Field * _fields,Player * _players,int _players_number, int * _randomized)
{
	if(_fields != NULL && _players != NULL && _players_number >= 0)
	_Draw(0,_fields,_players,_players_number,_randomized,NULL);
}

void Draw_Text(int _type)
{
	_Draw_Text(_type,NULL,NULL,0,0);
}

void _Draw_Text(int _type,Text * _text,const char * _name,int _width,int _height)
{
	static int Width = 0;
	static int Height = 0;
	static Array * Texts = NULL;
	Iterator * it;
	Text * Ptr;
	int len,bitmap_w,bitmap_h,i;
	if(_type == TEXT_INIT)
	{
		Width = _width;
		Height = _height;
		Texts = Create_Array();
	}
	else if(_type == TEXT_ADD)
	{
		Add_Element(Texts,_name,_text,1,TEXT);
		free(_text);
	}
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
				gluOrtho2D(0, Width, Height, 0);
				glMatrixMode(GL_MODELVIEW);
				glPushMatrix();
				glLoadIdentity();
				if(_type == GL_SELECT)
					glPushName(0);
				while(1){
					Ptr = Get_Value(it);
					len = strlen(Ptr->String);
					if(_type == GL_SELECT || Ptr->Position == TEXT_CENTER)
					{
						bitmap_h = Font_Height(Ptr->Font);
						bitmap_w = 0;
						for(i = 0;i < len;i++)
							bitmap_w += glutBitmapWidth(Ptr->Font,Ptr->String[i]);
					}
					if(_type == GL_SELECT)
					{
						glLoadName(Ptr->Select_Number);
						glBegin(GL_TRIANGLE_STRIP);
						if(Ptr->Position == TEXT_CENTER)
						{
							glVertex2f(Ptr->X-bitmap_w/2, Ptr->Y);
							glVertex2f(Ptr->X-bitmap_w/2, Ptr->Y-bitmap_h);
							glVertex2f(Ptr->X+bitmap_w/2, Ptr->Y);
							glVertex2f(Ptr->X+bitmap_w/2, Ptr->Y-bitmap_h);
						}
						else
						{
							glVertex2f(Ptr->X, Ptr->Y);
							glVertex2f(Ptr->X, Ptr->Y-bitmap_h);
							glVertex2f(Ptr->X+bitmap_w, Ptr->Y);
							glVertex2f(Ptr->X+bitmap_w, Ptr->Y-bitmap_h);
						}
						glEnd();
					}
					glColor3f(1.0f,1.0f,1.0f);
					if(Ptr->Position == TEXT_CENTER)
						glRasterPos2f(Ptr->X-bitmap_w/2, Ptr->Y);
					else
						glRasterPos2f(Ptr->X-bitmap_w/2, Ptr->Y);
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

void Text_Init(int _width,int _height)
{
	_Draw_Text(TEXT_INIT,NULL,NULL,_width,_height);
}
void Text_Add(Text * _text,const char * _name)
{
	_Draw_Text(TEXT_ADD,_text,_name,0,0);
}
void Text_Remove(const char * _name)
{
	_Draw_Text(TEXT_REMOVE,NULL,_name,0,0);
}
void Text_Clean()
{
	_Draw_Text(TEXT_CLEAN,NULL,NULL,0,0);
}


Text * Text_Create(int _x,int _y,int _select,const char * _string,void * _font)
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
	//_Draw_Text(GL_SELECT,NULL,NULL,0,0);
}

void Draw_Render()
{
	_Draw(GL_RENDER,NULL,NULL,0,NULL,NULL);
	//_Draw_Text(GL_RENDER,NULL,NULL,0,0);
}

void Draw_Pawn(float x,float z,float * colors,int _blink)
{
	glPushMatrix();
	glColor3f(colors[0],colors[1],colors[2]);
	GLUquadricObj *quadratic;
	quadratic=gluNewQuadric();			
	gluQuadricNormals(quadratic, GLU_SMOOTH);	
	
	glTranslatef(x,H/2+0.01f,-z);
	
	glRotatef(-90.0f,1.0f,0.0f,0.0f);
	gluCylinder(quadratic,PAWN_RADIUS,0.0f,PAWN_HEIGHT,32,32);
	
	glRotatef(90.0f,1.0f,0.0f,0.0f);
	glTranslatef(0.0f,PAWN_RADIUS*3/2,0.0f);
	
	glRotatef(-90.0f,1.0f,0.0f,0.0f);
	gluSphere(quadratic,PAWN_RADIUS/2,32,32);
	gluDeleteQuadric(quadratic);
	glPopMatrix();
}

int Init_GL(int _width, int _height, char * _label,int _fullscr)
{
	
	int Window_Number;
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);
    glutInitWindowSize(_width, _height);
    glutInitWindowPosition(0, 0);
    Window_Number = glutCreateWindow(_label);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);	// This Will Clear The Background Color To Black
    glClearDepth(1.0);				// Enables Clearing Of The Depth Buffer
    glDepthFunc(GL_LESS);			// The Type Of Depth Test To Do
    glEnable(GL_DEPTH_TEST);			// Enables Depth Testing
    glShadeModel(GL_SMOOTH);			// Enables Smooth Color Shading
    glViewport(0, 0, _width, _height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();				// Reset The Projection Matrix

    gluPerspective(45.0f,(GLfloat)_width/(GLfloat)_height,0.1f,100.0f);	// Calculate The Aspect Ratio Of The Window

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	glColor4f(1.0f, 1.0f, 1.0f, 0.5);
	glDisable(GL_BLEND);
    // set up light number 1.
    /*glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);  // add lighting. (ambient)
    glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);  // add lighting. (diffuse).
    glLightfv(GL_LIGHT1, GL_POSITION,LightPosition); // set light position.
    glEnable(GL_LIGHT1);                             // turn light 1 on.
    glBlendFunc(GL_SRC_ALPHA,GL_ONE);			// Set The Blending Function For Translucency
    glColor4f(1.0f, 1.0f, 1.0f, 0.5);  */
	return Window_Number;
}

void Reshape_Window(int _width, int _height)
{

    glViewport(0, 0, _width, _height);		

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(45.0f,(GLfloat)_width/(GLfloat)_height,0.1f,100.0f);
    glMatrixMode(GL_MODELVIEW);
}

void Enable_FullScr(int _width,int _height)
{
	if(_width && _height)
	Reshape_Window(_width,_height);
	glutFullScreen();
}

void Disable_FullScr(int _width,int _height)
{
	if(_width && _height)
	Reshape_Window(_width,_height);
	glutPositionWindow(0,0);
}

void Draw_Fields(int _type,Field * array,int players_number,int _blink)
{
	
}
	
void Blink_Set_Pawn(int _number,int * _pawns)
{
	Blink_Info * ptr = malloc(sizeof(*ptr));
	ptr->Player_Number = _number;
	ptr->Pawns_Numbers = _pawns;
	ptr->Field_Number = -1;
	_Draw(0,NULL,NULL,0,NULL,ptr);
	free(ptr);
}

void Blink_Set_Field(int _number, int _pawn, int _field)
{
	Blink_Info * ptr = malloc(sizeof(*ptr));
	ptr->Player_Number = _number;
	ptr->Pawns_Numbers = &_pawn;
	ptr->Field_Number = _field;
	_Draw(0,NULL,NULL,0,NULL,ptr);
	free(ptr);
}

Field * Generate(int Players_Number){
	
	switch(Players_Number){
		case 4:
			return Generate_4_Players();
		break;
		case 5:
			return Generate_5_Players();
		break;
		case 6:
			return Generate_6_Players();
		break;
		default:
			return NULL;
		break;
	}
}




Field * Generate_5_Players(){
	NOT_IMPLEMENTED;
}
	
Field * Generate_6_Players(){
	NOT_IMPLEMENTED;
}
Field * Generate_4_Players(){
	
	float radius;
	int i,j;
	Field * pointer;
	float Width;
	Width = MIN(W,D)/2;

	radius = 0.03*Width*2;
	pointer = malloc(sizeof(Field)*64);
	for(i = 0;i < 4;i++)
	{
			for(j = 0;j < 4;j++)
			{
				pointer[i*8+j].Position[0] = 0.9*Width - radius;
				pointer[i*8+j].Position[1] = 0.9*Width - radius;
				pointer[i*8+j].Radius = radius;
			}
	}
	for(i = 0;i < 4;i++)
		pointer[i*8+1].Position[0] -= 2*radius;
	for(i = 0;i < 4;i++)
		pointer[i*8+2].Position[1] -= 2*radius;
	for(i = 0;i < 4;i++)
	{
		pointer[i*8+3].Position[0] -= 2*radius;
		pointer[i*8+3].Position[1] -= 2*radius;
	}
	for(i = 0;i < 4;i++)
		pointer[8+i].Position[1] *= -1;		
	for(i = 0;i < 4;i++)
	{
		pointer[16+i].Position[0] *= -1;
		pointer[16+i].Position[1] *= -1;
	}
	for(i = 0;i < 4;i++)
		pointer[24+i].Position[0] *= -1;
	
	for(i = 0;i < 2;i++)
	{
		for(j = 0;j < 4;j++)
		{
			pointer[16*i+4+j].Position[0] = 0.0f;
			pointer[16*i+4+j].Position[1] = (0.9*Width - (2+j)*radius*2)*POW(-1,i);
			pointer[16*i+4+j].Radius = radius;
		}
	}
	
	for(i = 0;i < 2;i++)
	{
		for(j = 0;j < 4;j++)
		{
			pointer[16*i+12+j].Position[0] = (0.9*Width - (2+j)*radius*2)*POW(-1,i);
			pointer[16*i+12+j].Position[1] = 0.0f;
			pointer[16*i+12+j].Radius = radius;
		}
	}
	
	for(i = 0;i < 4;i += 2)
	{
		for(j = 0;j < 8;j++)
		{
			pointer[32+i*8+j].Position[0] = 0.0f + j* (0.9*Width-radius)/8;
			pointer[32+i*8+j].Position[1] = (0.9*Width - radius) - j* (0.9*Width-radius)/8;
			pointer[32+i*8+j].Radius = radius;
		}	
	}
	for(i = 0;i < 4;i += 2)
	{
		for(j = 0;j < 8;j++)
		{
			pointer[40+i*8+j].Position[0] = (0.9*Width-radius) - j*(0.9*Width-radius)/8;
			pointer[40+i*8+j].Position[1] = 0.0f - j* (0.9*Width-radius)/8;
			pointer[40+i*8+j].Radius = radius;
		}	
	}
		
	for(i = 0;i < 8;i++)
	{
		pointer[48+i].Position[0] *= -1;
		pointer[48+i].Position[1] *= -1;
	}
	
	for(i = 0;i < 8;i++)
	{
		pointer[56+i].Position[0] *= -1;
		pointer[56+i].Position[1] *= -1;
	}
	return pointer;
}	
void Close(Field * Pointer)
{
	free(Pointer);
}


void Draw_Circle(float _radius, float _x, float _y,float _z)
{
	int i;
	glPushMatrix();
	glTranslatef(_x,_y,_z);
    glBegin(GL_LINES);
    float theta;
    for (i = 0; i < 180; i++)
    {
		theta = 2.0f * PI * (float)i / 180.0f;
		glVertex3f(_radius * cos(theta),0,_radius * sin(theta));
		glVertex3f(_radius * cos(theta+PI),0,_radius * sin(theta+PI));
    }
    glEnd();
	glPopMatrix();
}

void Text_Draw(int _x,int _y,int _select_name,void *_font,int _position_type,\
				const char * _name,const char * _format,...)
{
	char * buffer;
	Text * ptr;
	va_list ap;
	buffer = (char*) malloc(sizeof(*buffer)*2*STRING_SIZE);
	memset(buffer,0,sizeof(*buffer)*2*STRING_SIZE);
	va_start(ap,_format);
	while(*_format != '\0'){
		if(*_format != '%')
			sprintf(buffer,"%s%c",buffer,*_format);
		else
		{
			switch(*++_format){
				case 's':
					sprintf(buffer,"%s%s",buffer,va_arg(ap,char *));
				break;
				case 'd':
					sprintf(buffer,"%s%d",buffer,va_arg(ap,int));
				break;
				case 'c':
					sprintf(buffer,"%s%c",buffer,va_arg(ap,int));
				break;
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

void Text_Create_Draw(int _size,int * _tab)
{
    int i;
    char * buffer;
    Text * ptr;
    buffer = malloc(sizeof(*buffer)*(strlen(DRAWING_TEXT)+1+_size*2));
    strcpy(buffer,DRAWING_TEXT);
    for(i = 0;i < _size;i++)
            sprintf(buffer,"%s %d",buffer,_tab[i]);
    ptr = Text_Create(50,75,200,buffer,FONT1);
    free(buffer);
    Text_Remove(DRAW_MSG);
    Text_Add(ptr,DRAW_MSG);
    Set_Change();
}

void Text_Create_Player(const char * _name)
{
    Text * ptr = Text_Create(50,50,200,_name,FONT1);
    Text_Remove(NAME_MSG);
    Text_Add(ptr,NAME_MSG);
    Set_Change();
}

void Text_Create_FPS(int _fps)
{
    //5 digits for FPS. everything can happen
    char * buffer = malloc(sizeof(*buffer)*(strlen(DRAWING_TEXT)+6));
    sprintf(buffer,"%s %d",FPS_TEXT,_fps);
    Text * ptr = Text_Create(50,25,200,buffer,FONT1);
    free(buffer);
    Text_Remove(FPS_MSG);
    Text_Add(ptr,FPS_MSG);
    Set_Change();
}


void Draw_Cube_Pips(float _radius, int _number)
{
	int i;
	switch(_number){
		
		case 1:
			glRotatef(90.0f,1.0f,0.0f,0.0f);
			Draw_Circle(_radius,0.0f,0.0f,0.0f);
		break;
		case 2:
			for(i = 0;i < 2;i++){
				glPushMatrix();
					glTranslatef(2*_radius*POW(-1,i),2*_radius*POW(-1,i),0.0f);
					glRotatef(90.0f,1.0f,0.0f,0.0f);
					Draw_Circle(_radius,0.0f,0.0f,0.0f);
				glPopMatrix();
			}
		break;
		case 3:
			for(i = 0;i < 2;i++){
				glPushMatrix();
					glTranslatef(2*_radius*POW(-1,i),2*_radius*POW(-1,i),0.0f);
					glRotatef(90.0f,1.0f,0.0f,0.0f);
					Draw_Circle(_radius,0.0f,0.0f,0.0f);
				glPopMatrix();
			}
			glRotatef(90.0f,1.0f,0.0f,0.0f);
			Draw_Circle(_radius,0.0f,0.0f,0.0f);
		break;
		case 4:
			for(i = 0;i < 4;i++){
				glPushMatrix();
					glTranslatef(3*_radius*(!(i % 2) ? -1 : 1),3*_radius*(i > 1 ? -1 : 1),0.0f);
					glRotatef(90.0f,1.0f,0.0f,0.0f);
					Draw_Circle(_radius,0.0f,0.0f,0.0f);
				glPopMatrix();
			}
		break;
		case 5:
			for(i = 0;i < 4;i++){
				glPushMatrix();
					glTranslatef(2*_radius*(!(i % 2) ? -1 : 1),2*_radius*(i > 1 ? -1 : 1),0.0f);
					glRotatef(90.0f,1.0f,0.0f,0.0f);
					Draw_Circle(_radius,0.0f,0.0f,0.0f);
				glPopMatrix();
			}
			glRotatef(90.0f,1.0f,0.0f,0.0f);
			Draw_Circle(_radius,0.0f,0.0f,0.0f);
		break;
		case 6:
			for(i = 0;i < 4;i++){
				glPushMatrix();
					glTranslatef(2*_radius*(!(i % 2) ? -1 : 1),3*_radius*(i > 1 ? -1 : 1),0.0f);
					glRotatef(90.0f,1.0f,0.0f,0.0f);
					Draw_Circle(_radius,0.0f,0.0f,0.0f);
				glPopMatrix();
			}
			for(i = 0;i < 2;i++){
				glPushMatrix();
				glTranslatef(2*_radius*POW(-1,i),0.0f,0.0f);
				glRotatef(90.0f,1.0f,0.0f,0.0f);
				Draw_Circle(_radius,0.0f,0.0f,0.0f);
				glPopMatrix();
			}
		break;
	}
}

int Font_Height(void * _font)
{
	if(_font ==GLUT_BITMAP_9_BY_15)
		return 15;
	else if(_font == GLUT_BITMAP_TIMES_ROMAN_24)
		return 24;
	else
		return -1;
}