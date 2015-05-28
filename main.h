//Copyright (C) 2014-2015  Corwin Hansen
#ifndef MAIN_H//include protection
	#define MAIN_H


#include <stdio.h>//input and output
#include <stdlib.h>//standard stuff
#include <math.h>//all the math functions
#include <string.h>//string related things
#include <time.h>//time on system
#include "SDL.h"//include sdl
#include "SDL_ttf.h"//include sdl_ttf
#include "SDL_image.h"//include sdl_image
#include "corwins.h"


#define FULLLSCREEN //define if fullscreen
//#define FPS //print out fps

#ifdef FULLLSCREEN
#define WINDOWFLAGS SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN | SDL_WINDOW_BORDERLESS | SDL_WINDOW_RESIZABLE    //fullscreen borderless resizable window 
#define WIDTH display.w //width and height of window
#define HEIGHT display.h
#else
#define WINDOWFLAGS SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE        //resizable window
#define WIDTH display.w/2 //width and height of window
#define HEIGHT display.h/2;
#endif


#define TEXT_INPUT_SHIFT 0 //y shift when inputting text

#define WINDOW_NAME "Slow Escape"
#define DELAY 10 //default delay
#define DELAY_SLOW 100 //dealy while in background

//paths
#define RESOURCES "resources/" //media resources path. 
#define FONTS "fonts/" //fonts path. Inside media resources path.
#define FONT "OpenSans-Regular.ttf" //font path
#define SAVE "resources/save.txt" //save file full path
#define PLAYERIMG "Player.png" //player image
#define ENEMYIMG "Enemy" //enemy image prefix
#define LEVELIMG "Level.png" //level button image
#define STARTIMG "Start.png" //starting point image
#define EXITIMG "Exit.png" //exit point image
#define SOUNDIMG "Speaker_Icon.png" //speaker icon
#define MUTEIMG "Mute_Icon.png" //mute icon
#define SOUND "BGM.wav" //BGM sound file
#define MENUBACKGROUND "MenuBackground.png"

//number of levels
#define MAXLEVEL 36//36 levels max

//sizes of array
#define HOME 8//8 home positions
#define ENEMY 32//32 enemy
#define ENEMYIMGSIZE 8//8 different enemy images


//in game values
#define LEVELBUTTON (1.0/8)//size of level button
#define LEVELBUTTONSPACING (1.0/6)//spacing of level button
#define ENEMYSIZE (1.0/12)//size of enemy
#define PLAYERSIZE (1.0/12)//size of player


//What screen to display
#define MENU 0 //display menu
#define LEVEL 1 //display level chooser
#define GAME 2 //display game
#define WIN 3 //display win message
#define EXIT 4 //warning for exiting to menu

int displaymode;//what screen to display
int level;//what level you are playing
int maxlevel;//what level you have unlocked

int select;//if player is selected or not
double playerX, playerY;//player x and y position

typedef struct{//position structure
	double x, y;//position
}pos;

typedef struct{//enemy position structure
	int image;//which image to display. 0 for no enemy
	pos start;//starting position
	pos cur;//current position
	double speed;//speed in ratio to player movement
	double range;//range in ratio to player range
}enemy;

pos home[HOME];//home position for players
pos exitpos;//exit position
enemy enemys[ENEMY];//enemys

int displayd;//if displayd or not
int levelview;//which level screen you are in

int sound;//0 for mute 1 for play

SDL_Window *window;//sdl window
SDL_Renderer *renderer;//sdl renderer
SDL_Event e;//event

double cx, cy;//x and y of centered square
int width, height, maxside;//width and hight and smallest of 2 side of window
double ws, hs;//width and height scaled to maxside
time_t lasttime;//last time mesured
unsigned long int lastframe;//last frame recorded
unsigned long int frame;//number of frames from start
unsigned long int fps;//frames per second

SDL_Texture *somethingwentwrong;//image to display if something goes wrong
SDL_Texture *PlayerIMG;//player image
SDL_Texture *EnemyIMG[ENEMYIMGSIZE];//enemy image array
SDL_Texture *LevelButtonIMG;//level button image
SDL_Texture *StartIMG;//starting point image
SDL_Texture *ExitIMG;//exit point image
SDL_Texture *Sound;//when sound id on
SDL_Texture *Mute;//when sound is muted
SDL_Texture *Menu;//nemu button
SDL_Texture *Level;//level label
SDL_Texture *Background;//background image
SDL_Texture *MenuBackground;//menu background image

//audio stuff
SDL_AudioFormat deviceFormat;//device format
Uint8 *mixData;//sound data
SDL_AudioSpec want, have;
SDL_AudioDeviceID dev;
Uint32 wav_length;//sound length
Uint32 wav_current;//current position 
Uint8 *audio_pos;//audio position

double scale;//scale up or down
double xshift;//shift in x direction
double yshift;//shift in y direction
double XShiftAll;//shift everything drawn in x direction
double YShiftAll;//shift everything drawn in x direction



uint32_t delay;//amount of delay for each frame
double MouseX, MouseY;//x and y position of mouse / touch normalised



//fonts for different sizes (1/n of maxside)
TTF_Font* font_4;
TTF_Font* font_6;
TTF_Font* font_8;
TTF_Font* font_16;
TTF_Font* font_24;
TTF_Font* font_32;
TTF_Font* font_46;
TTF_Font* font_64;


int EventFilter(void* userdata, SDL_Event* e);//event filter
void Quit(void);//quit everything
void GetDisplay(void);//get display
void Clicked(void);//x and y positions clicked
void Draged(void);//draged function to call when mouse is moved while it is down
void Resize(void);//recalculate numbers related to size and load texts
SDL_Texture* GetTexture(const char *file);//make texture from this file
TTF_Font* GetFont(const char *file, int size);//get font from file
SDL_Texture* GetTextTexture(TTF_Font* font, const char* text, int r, int g, int b);//make texture from font text and rgb
void DrawBase(void);//draw basic stuff
void DrawText(SDL_Texture *texture, double x, double y, SDL_Rect *rect, int center);//draw rect of texture at x and y position normalised. Null rect for whole texture. set center to 1 to center to x and y. Draws texture at full size
void DrawIMG(SDL_Texture *texture, double x, double y, SDL_Rect *rect, double w, double h, int center);//draw rect of texture at x and y position normalised at scale from maxside. Null rect for whole texture. set center to 1 to center to x and y
void Draw(void);//draw/update screen
void LoadFile(void);//load from save file
void SaveFile(void);//save to save file
void Load(void);//load level from file
void AudioCallback(void *udata, Uint8 *stream, int len);//audio callback function



#endif
