//Copyright (C) 2014-2015  Corwin Hansen
#include "main.h"//main header


int main(int argc, char *argv[]){
	renderer = NULL;//set values to null
	window = NULL;

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0){//start SDL and get any error if it dosen't.
		printf("Could not load SDL: %s\n", SDL_GetError());//print out error
		exit(EXIT_FAILURE);//exit
	}




	// load support for PNG image format
	int flags = IMG_INIT_PNG;//flag for PNG image loading
	int initted = IMG_Init(flags);//get initialised flags
	if ((initted&flags) != flags) {//if it caused error
		printf("Could not initialised png support: %s\n", IMG_GetError());//print error message
		exit(EXIT_FAILURE);//exit
	}


	//initialise ttf
	if (TTF_Init() == -1) {//if it causes error
		printf("could not initialised ttf support: %s\n", TTF_GetError());//print error message
		exit(EXIT_FAILURE);//exit
	}


	//filter events
	SDL_SetEventFilter(EventFilter, NULL);


	//quit SDL at exit
	atexit(Quit);



	Resize();//get display


	//Create window
	window = SDL_CreateWindow(
		WINDOW_NAME,                  // window title
		SDL_WINDOWPOS_CENTERED,           // initial x position
		SDL_WINDOWPOS_CENTERED,           // initial y position
		width,                               // width, in pixels
		height,                               // height, in pixels
		WINDOWFLAGS        //use these flags
		);


	// Check that the window was made
	if (window == NULL) {
		// In the event that the window could not be made...
		printf("Could not create window: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);//exit
	}


	//Create renderer
	renderer = SDL_CreateRenderer(window, -1, 0);


	// Check that the renderer was made
	if (renderer == NULL) {
		// In the event that the window could not be made...
		printf("Could not create renderer: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);//exit
	}


	//initialise and load stuff
	delay = DELAY;//10 ms delay
	somethingwentwrong = GetTextTexture(font_64, "somethingwentwrong", 0, 0, 0);//image to display if something went wrong

	//loading message
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);//draw white
	SDL_RenderClear(renderer);//clear screen
	SDL_Texture *loading = GetTextTexture(font_16, "Loading", 0, 0, 0);//loading message
	DrawText(loading, 0.5, 0.5, NULL, 1);//draw loading message
	SDL_RenderPresent(renderer);//present loading message
	SDL_DestroyTexture(loading);//don't need this texture

	//set initial values
	LoadFile();//load file

	//load textures
	PlayerIMG = GetTexture(PLAYERIMG);//get player image
	EnemyIMG = GetTexture(ENEMYIMG);//get enemy image
	LevelButtonIMG = GetTexture(LEVELIMG);//get level button image
	StartIMG = GetTexture(STARTIMG);//get starting point image
	ExitIMG = GetTexture(EXITIMG);//get exiting point image
	Resize();//reload display




	//main loop
	while (1) {
		while (SDL_PollEvent(&e)) {//for each e
			char str[1024];//string to fill for values
			switch (e.type) {//for each event type
			case SDL_QUIT://quit everything
				Quit();//quit everything
				break;//get out
			case SDL_MOUSEBUTTONDOWN://when clicking down
				if (e.button.which != SDL_TOUCH_MOUSEID){//if not touch event
					MouseX = (double)(e.button.x) / maxside;//set x and y position of mouse from square normalised
					MouseY = (double)(e.button.y) / maxside;
					Clicked();//run clicked function 
					Draged();//run draged function 
				}
				break;//get out
			case  SDL_FINGERDOWN://when finger touched
				MouseX = (double)(e.tfinger.x) * ws;//set x and y position of mouse from square normalised
				MouseY = (double)(e.tfinger.y) * hs;
				Clicked();//run clicked function 
				Draged();//run draged function 
				break;//get out
			case SDL_MOUSEBUTTONUP://when clicking up
				if (e.button.which != SDL_TOUCH_MOUSEID){//if not touch event
					MouseX = (double)(e.button.x) / maxside;//set x and y position of mouse from square normalised
					MouseY = (double)(e.button.y) / maxside;
					select = 0;//deselect 
				}
				break;//get out
			case  SDL_FINGERUP://when finger lifted
				MouseX = (double)(e.tfinger.x) * ws;//set x and y position of mouse from square normalised
				MouseY = (double)(e.tfinger.y) * hs;
				select = 0;//deselect
				break;//get out
			case SDL_MOUSEMOTION://when mouse moved
				if (e.motion.which != SDL_TOUCH_MOUSEID){//if not touch event
					MouseX = (double)(e.motion.x) / maxside;//set x and y position of mouse from square normalised
					MouseY = (double)(e.motion.y) / maxside;
					if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)){//if mouse is pressed down
						Draged();//run draged function 
					}
				}
				break;//get out
			case SDL_FINGERMOTION://when finger moved
					MouseX = (double)(e.tfinger.x) * ws;//set x and y position of mouse from square normalised
					MouseY = (double)(e.tfinger.y) * hs;
					Draged();//run draged function 
				break;//get out
			case SDL_WINDOWEVENT://when window was changed
				Resize();//resize stuff
				break;//get out
			case SDL_TEXTINPUT://when text is inputed
				break;//get out
			case SDL_TEXTEDITING://when editing text
				break;
			case SDL_KEYDOWN://key is down
				break;//get out
			default://for everything else
				//ignore event
				break;//get out
			}
		}
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);//draw white
		SDL_RenderClear(renderer);//clear screen
		DrawBase();//draw background image






		SDL_RenderPresent(renderer);//present rendered
		if (difftime(time(NULL), lasttime) >= 1){//if 1 second passed
			fps = frame - lastframe;//set fps
			lasttime = time(NULL);//set new lasttime
			lastframe = frame;
#ifdef FPS
			printf("%d fps\n", fps);
#endif
		}
		frame++;//increment frame

		SDL_Delay(delay);//delay some time
	}

	exit(EXIT_SUCCESS);//if it somehow reaches here
}

















int EventFilter(void* userdata, SDL_Event* e){//event filter
	switch (e->type) {//for each event type
	case SDL_QUIT://quit everything
		exit(EXIT_SUCCESS);//exit
		return 0;//delete that event
		break;//get out

	case SDL_APP_TERMINATING://quit everything
		exit(EXIT_SUCCESS);//exit
		return 0;//delete that event
		break;//get out

	case SDL_APP_LOWMEMORY://if low in memory
		//can't do anything
		return 0;//delete that event
		break;//get out

	case SDL_APP_WILLENTERBACKGROUND://if entering background
		delay = DELAY_SLOW;//lower fps
		return 0;//delete that event
		break;//get out

	case SDL_APP_DIDENTERBACKGROUND://if in background
		//don't do anything
		return 0;//delete that event
		break;//get out

	case SDL_APP_WILLENTERFOREGROUND://if entering forgeound
		delay = DELAY;//higher fps
		return 0;//delete that event
		break;//get out

	case SDL_APP_DIDENTERFOREGROUND://if in forground
		//don't do anything
		return 0;//delete that event
		break;//get out

	default://for everything else
		return 1;//don't delete event
		break;//get out
	}
	return 1;//just in case something wiard happens
}

void Quit(void){//quit everything
	//destroy textures
	SDL_DestroyTexture(PlayerIMG);
	SDL_DestroyTexture(EnemyIMG);
	SDL_DestroyTexture(LevelButtonIMG);
	SDL_DestroyTexture(StartIMG);
	SDL_DestroyTexture(ExitIMG);





	TTF_CloseFont(font_4);//close all fonts
	TTF_CloseFont(font_6);
	TTF_CloseFont(font_8);
	TTF_CloseFont(font_16);
	TTF_CloseFont(font_24);
	TTF_CloseFont(font_32);
	TTF_CloseFont(font_46);
	TTF_CloseFont(font_64);
	SDL_DestroyRenderer(renderer);//destroy renderer
	SDL_DestroyTexture(somethingwentwrong);//destroy something went wrong texture
	SDL_DestroyWindow(window);//destroy window
	IMG_Quit();//quit SDL_Image
	TTF_Quit();//quit SDL_TTF
	SDL_Quit();//quit SDL
	SaveFile();//save file
	
	return;//exit function if it didn't exit for some reason
}

void GetDisplay(void){//get display
	SDL_DisplayMode display;//display mode 
	if (window == NULL){
		//get desplay mode of the desktop if there is no window yet
		if (SDL_GetDesktopDisplayMode(0, &display) != 0) {
			printf("Could not get display mode: %s\n", SDL_GetError());//take care of errors
			exit(EXIT_FAILURE);//exit
		}
		width = WIDTH;//set display width and height
		height = HEIGHT;
	}
	else{
		//get desplay mode of the window if there is window
		SDL_GetWindowSize(window, &width, &height);//get width and height
	}

	if (width > height){//get smallest of 2 sides
		maxside = height;
	}
	else {
		maxside = width;
	}

	ws = (double)width / (double)maxside;//get width and height 
	hs = (double)height / (double)maxside;

	return;//exit function
}

void Clicked(void){//x and y positions clicked
	switch (displaymode){//switch for each thing to display
	case MENU:

		break;
	case LEVEL:

		break;
	case GAME:

		break;
	case WIN:

		break;
	case EXIT:

		break;
	}
	
	return;//exit function
}

void Draged(void){

	return;//exit function
}

SDL_Texture* GetTexture(const char *file){//make texture from this file
	char image[256] = RESOURCES;//folder path
	strcat(image, file);//append path
	SDL_Surface* surface = IMG_Load(image);//load surface
	if (surface == NULL){//if it could not be loaded
		printf("could not load image: %s\n", IMG_GetError());//error message
		return somethingwentwrong;//something went wrong
	}
	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);//get texture from loaded image
	if (texture == NULL){//if it could not be converted to texture
		printf("could not create texture: %s\n", SDL_GetError());//error message
		return somethingwentwrong;//something went wrong
	}
	SDL_FreeSurface(surface);//free surface
	return texture;//return texture
}

TTF_Font* GetFont(const char *file, int size){//get font from file
	char fontfile[256] = RESOURCES;//folder path
	strcat(fontfile, FONTS);//append fonts path
	strcat(fontfile, file);//append path
	TTF_Font *font = TTF_OpenFont(fontfile, size);//get font
	if (font == NULL){//if it could not be loaded
		printf("could not load font: %s\n", TTF_GetError());//error message
		exit(EXIT_FAILURE);//exit
	}
	return font;//return font
}

SDL_Texture* GetTextTexture(TTF_Font* font, const char* text, int r, int g, int b){//make texture from font text and rgb
	SDL_Color color;//set color
	color.r = r;
	color.g = g;
	color.b = b;
	color.a = 255;
	char texturetext[256];//text to display for texture
	strcpy(texturetext, text);//copy text
	strtok(texturetext, "\n");//remove newline
	SDL_Surface* surface = TTF_RenderText_Blended(font, texturetext, color);//rendere text as surface
	if (surface == NULL){//if it could not be loaded
		printf("could not load text: %s\n", TTF_GetError());//error message
		exit(EXIT_FAILURE);//exit
	}
	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);//get texture from loaded image
	if (texture == NULL){//if it could not be converted to texture
		printf("could not create texture: %s\n", SDL_GetError());//error message
		exit(EXIT_FAILURE);//exit
	}
	SDL_FreeSurface(surface);//free surface
	return texture;//return created texture
}

void Resize(void){//recalculate numbers related to size and load texts
	GetDisplay();//get display



	TTF_CloseFont(font_4);//close all fonts
	TTF_CloseFont(font_6);
	TTF_CloseFont(font_8);
	TTF_CloseFont(font_16);
	TTF_CloseFont(font_24);
	TTF_CloseFont(font_32);
	TTF_CloseFont(font_46);
	TTF_CloseFont(font_64);

	//load fonts on different sizes (1/n of maxside)
	font_4 = GetFont(FONT, maxside / 4);
	font_6 = GetFont(FONT, maxside / 6);
	font_8 = GetFont(FONT, maxside / 8);
	font_16 = GetFont(FONT, maxside / 16);
	font_24 = GetFont(FONT, maxside / 24);
	font_32 = GetFont(FONT, maxside / 32);
	font_46 = GetFont(FONT, maxside / 46);
	font_64 = GetFont(FONT, maxside / 64);


	if (renderer != NULL){//if there is a renderer
		
	}
}

void DrawBase(void){//draw basic stuff
}

void DrawText(SDL_Texture *texture, double x, double y, SDL_Rect *rect, int center){//draw rect of texture at x and y position normalised. Null rect for whole texture. set center to 1 to center to x and y. Draws texture at full size
	if (texture == NULL) {//if texture passed dosen't exist
		texture = somethingwentwrong;//set texture to something went wrong
	}
	SDL_Rect dest;
	int w, h, access;//value to fill up
	long format;
	SDL_QueryTexture(texture, &format, &access, &w, &h);//get text box size
	dest.w = (int) w;//set width and height
	dest.h = (int) h;
	dest.x = (int)(x * maxside);//set x and y
	dest.y = (int)(y * maxside);

	if (center){
		dest.x = dest.x - dest.w / 2;//set x and y centered to x and y
		dest.y = dest.y - dest.h / 2;
	}

	dest.x += XShiftAll * maxside;//shift x and y
	dest.y += YShiftAll * maxside;

	SDL_RenderCopy(renderer, texture, rect, &dest);//draw texture

}

void DrawIMG(SDL_Texture *texture, double x, double y, SDL_Rect *rect, double w, double h, int center){//draw rect of texture at x and y position at scale from maxside normalised. Null rect for whole texture. set center to 1 to center to x and y
	if (texture == NULL) {//if texture passed dosen't exist
		texture = somethingwentwrong;//set texture to something went wrong
	}	SDL_Rect dest;
	dest.w = (int) (maxside * w);//set width and height
	dest.h = (int) (maxside * h);
	dest.x = (int)(x * maxside);//set x and y
	dest.y = (int)(y * maxside);

	if (center){
		dest.x = dest.x - dest.w / 2;//set x and y centered to x and y
		dest.y = dest.y - dest.h / 2;
	}

	dest.x += XShiftAll * maxside;//shift x and y
	dest.y += YShiftAll * maxside;

	SDL_RenderCopy(renderer, texture, rect, &dest);//draw texture

}

void Draw(void){//draw/update screen
	switch (displaymode){//switch for each thing to display
	case MENU:

		break;
	case LEVEL:

		break;
	case GAME:
		
		break;
	case WIN:

		break;
	case EXIT:

		break;
	}
}

void LoadFile(void){//load from save file

}

void SaveFile(void){//save to save file
	FILE *file = fopen(SAVE, "wb");//open source file
	if (file == NULL){//if file could not be opened
		printf("Save file %s could not be made\n", SAVE);//send error meaaage
		exit(EXIT_SUCCESS);//exit program
	}
	fprintf(file, "%d", maxlevel);//save file
}

void Load(void){//load level from file
	FILE *file = fopen(SAVE, "rb");//open source file
	if (file == NULL){//if file could not be opened
		maxlevel = 1;//start from 1
	}
	char string[1024];//string to fill
	maxlevel = atoi(fgets(string, 1024, file));//get max level
}


















































































