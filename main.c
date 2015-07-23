//Copyright (C) 2014-2015  Corwin Hansen
#include "main.h"//main header


int main(int argc, char *argv[]){
	renderer = NULL;//set values to null
	window = NULL;

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO) != 0){//start SDL and get any error if it dosen't.
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
	displaymode = MENU;//display menu at start
	displayd = 0;
	levelview = 0;

	//loading message
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);//draw white
	SDL_RenderClear(renderer);//clear screen
	SDL_Texture *loading = GetTextTexture(font_16, "Loading", 0, 0, 0);//loading message
	DrawText(loading, 0.5 * ws, 0.5 * hs, NULL, 1);//draw loading message
	SDL_RenderPresent(renderer);//present loading message
	SDL_DestroyTexture(loading);//don't need this texture

	//set initial values
	LoadFile();//load file

	//do audio stuff
	char string[1024];//string to fill
	sprintf(string, "%s%s", RESOURCES, SOUND);//get file path
	SDL_zero(want);
	want.freq = 48000;
	want.format = AUDIO_S16LSB;
	want.channels = 1;
	want.samples = 4096;
	want.callback = AudioCallback;  // you wrote this function elsewhere.
	want.userdata = NULL;

	dev = SDL_OpenAudioDevice(NULL, 0, &want, NULL, SDL_AUDIO_ALLOW_FORMAT_CHANGE);
	if (dev == 0) {
		printf("Failed to open audio: %s\n", SDL_GetError());
	}
	else {
		SDL_PauseAudioDevice(dev, 0);  // start audio playing.
	}
	deviceFormat = want.format;//set device format

	if (SDL_LoadWAV(string, &want, &mixData, &wav_length) == NULL) {
		fprintf(stderr, "Could not open sound file: %s\n", SDL_GetError());
	}
	wav_current = wav_length;//copy length
	audio_pos = mixData;//copy data


	//load textures
	PlayerIMG = GetTexture(PLAYERIMG);//get player image
	int i;
	for (i = 0; i < ENEMYIMGSIZE; i++){//for each enemy
		char string[1024];//string to fill
		sprintf(string, "%s%d.png", ENEMYIMG, i);//load file name
		EnemyIMG[i] = GetTexture(string);//get enemy image
	}
	LevelButtonIMG = GetTexture(LEVELIMG);//get level button image
	StartIMG = GetTexture(STARTIMG);//get starting point image
	ExitIMG = GetTexture(EXITIMG);//get exiting point image
	Sound = GetTexture(SOUNDIMG);//get sound icon image
	Mute = GetTexture(MUTEIMG);//get mute icon image
	MenuBackground = GetTexture(MENUBACKGROUND);//get Menu background image
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

		//calculate enemy movement
		switch (displaymode){//switch for each thing to display
		case GAME:
			if (hypot(player.x - exitpos.x, player.y - exitpos.y) < PLAYERSIZE){//if in right range of exit
				displaymode = WIN;//display win screen
				displayd = 0;
			}
			double PM = hypot(PlayerMovement.x, PlayerMovement.y);//get player movement
			double HD = GetHome();//get distance to closest home
			for (i = 0; i < ENEMY; i++){//for each enemy
				if (enemys[i].image){//if there is enemy there
					double x = enemys[i].cur.x += enemys[i].speed * fabs(PM) * -((enemys[i].cur.x - player.x) / hypot(enemys[i].cur.x - player.x, enemys[i].cur.y - player.y));//get x position and copy value to x
					double y = enemys[i].cur.y += enemys[i].speed * fabs(PM) * -((enemys[i].cur.y - player.y) / hypot(enemys[i].cur.x - player.x, enemys[i].cur.y - player.y));//get y position and copy value to y
					x = x < 0 ? 0 : x > 1 ? 1 : x;//get x in to range of 0 to 1
					y = y < 0 ? 0 : y > 1 ? 1 : y;//get y in to range of 0 to 1
					enemys[i].cur.x = x;//return copy
					enemys[i].cur.y = y;
					if (hypot(x - enemys[i].start.x, y - enemys[i].start.y) > HD * enemys[i].range){//if out of range
						enemys[i].cur.x += (hypot(x - enemys[i].start.x, y - enemys[i].start.y) - HD * enemys[i].range) * -((x - enemys[i].start.x) / hypot(x - enemys[i].start.x, y - enemys[i].start.y));//get x position
						enemys[i].cur.y += (hypot(x - enemys[i].start.x, y - enemys[i].start.y) - HD * enemys[i].range) * -((y - enemys[i].start.y) / hypot(x - enemys[i].start.x, y - enemys[i].start.y));//get y position
					}
					if (hypot(player.x - enemys[i].cur.x, player.y - enemys[i].cur.y) < PLAYERSIZE){//if in right range of exit
						displaymode = LOSE;//display lose screen
						displayd = 0;
					}
				}
			}
			PlayerMovement.x = 0;//reset player movement
			PlayerMovement.y = 0;
			break;
		}


		Draw();//draw screen
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
	int i;
	for (i = 0; i < ENEMYIMGSIZE; i++){
		SDL_DestroyTexture(EnemyIMG[i]);
	}
	SDL_DestroyTexture(LevelButtonIMG);
	SDL_DestroyTexture(StartIMG);
	SDL_DestroyTexture(ExitIMG);
	SDL_DestroyTexture(Menu);
	SDL_DestroyTexture(Level);
	SDL_DestroyTexture(Background);
	SDL_DestroyTexture(MenuBackground);

	SaveFile();//save file
	SDL_CloseAudioDevice(dev);//close audio
	SDL_FreeWAV(mixData);//free wav file

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

	cx = (ws - 1) / 2;//get centerd square x and y
	cy = (hs - 1) / 2;

	return;//exit function
}

void Clicked(void){//x and y positions clicked
	int i;
	if (MouseX > ws - 0.1 && MouseY > hs - 0.1){//if clicking sound
		sound = !sound;//set sound to opposit
		displayd = 0;
	}
	switch (displaymode){//switch for each thing to display
	case MENU:
		if (MouseY > 0.3 * hs && MouseY < 0.5 * hs){//if hitting play button
			displaymode = GAME;//go in to game mode
			level = maxlevel;//set to next level
			displayd = 0;
			Load();//load game
		}
		else if (MouseY < 0.7 * hs && MouseY > 0.5 * hs){//if hitting levels button
			displaymode = LEVEL;//go in to game mode
			displayd = 0;
			levelview = 0;
		}
		break;
	case LEVEL:
		if (levelview){//if not in first view
			if (MouseX < 0.1){
				displayd = 0;
				levelview--;
			}
		}
		if ((levelview + 1) * (1 / LEVELBUTTONSPACING) * (1 / LEVELBUTTONSPACING) < maxlevel){//if not in last view
			if (MouseX > ws - 0.1){
				displayd = 0;
				levelview++;
			}
		}
		for (i = 1; i + (int)(levelview * (1 / LEVELBUTTONSPACING) * (1 / LEVELBUTTONSPACING)) <= maxlevel; i++){//for each button
			if (hypot((cx + LEVELBUTTONSPACING / 2 + ((i - 1) % (int)(1 / LEVELBUTTONSPACING)) * LEVELBUTTONSPACING) - MouseX, (cy + LEVELBUTTONSPACING / 2 + ((i - 1) / (int)(1 / LEVELBUTTONSPACING)) * LEVELBUTTONSPACING) - MouseY) < LEVELBUTTON / 2){//if within a clicking range
				displaymode = GAME;//go in to game mode
				level = i + (int)(levelview * (1 / LEVELBUTTONSPACING) * (1 / LEVELBUTTONSPACING));//set to next level
				displayd = 0;
				Load();//load game
				break;
			}
		}
		if (MouseX > ws - 0.15 && MouseY < 0.1){//if clicked menu
			displaymode = MENU;//display menu
			displayd = 0;
		}
		break;
	case GAME:
		#ifndef MOBILE
		if (hypot(MouseX - (player.x * (1 - PLAYERSIZE) + cx + PLAYERSIZE / 2), MouseY - (player.y * (1 - PLAYERSIZE) + cy + PLAYERSIZE / 2)) < PLAYERSIZE / 2){//if clicked on player
			select = 1;//make player selected
			double x, y;//x and y position of mouse in in game coordinate
			x = (MouseX - cx - PLAYERSIZE / 2) / (1 - PLAYERSIZE);//get position
			y = (MouseY - cy - PLAYERSIZE / 2) / (1 - PLAYERSIZE);
			x = x < 0 ? 0 : x > 1 ? 1 : x;//get x in to range of 0 to 1
			y = y < 0 ? 0 : y > 1 ? 1 : y;//get y in to range of 0 to 1
			PlayerMovement.x += fabs(player.x - x);//get player movement
			PlayerMovement.y += fabs(player.y - y);
			last.x = player.x = x;//set last and player position to mouse position
			last.y = player.y = y;
		}
		#endif
		if (MouseX > ws - 0.15 && MouseY < 0.1){//if clicked menu
			displaymode = EXIT;//display menu
			displayd = 0;
		}
		#ifdef MOBILE
		select = 1;//make player selected
		last.x = MouseX;//set last position to mouse position
		last.y = MouseY;
		#endif
		break;
	case WIN:
		if (level != 1){//if not in level
			if (MouseX < 0.1){
				displayd = 0;
				level--;
				displaymode = GAME;//display menu
				Load();//load game
			}
		}
		if (level != MAXLEVEL){//if not in last level possible
			if (MouseX > ws - 0.1){
				displayd = 0;
				level++;
				displaymode = GAME;//display menu
				Load();//load game
			}
		}
		if (MouseX > ws - 0.15 && MouseY < 0.1){//if clicked menu
			displaymode = MENU;//display menu
			displayd = 0;
		}
		if (MouseY < hs * 0.65 && MouseY > hs * 0.55){//if clicked replay
			displayd = 0;
			displaymode = GAME;//display game
			Load();//load game
		}
		break;
	case EXIT:
		if (MouseY < hs * 0.65 && MouseY > hs * 0.55){//if clicked in range
			if (MouseX < ws * 0.5){//if clicked on yes
				displaymode = MENU;//display menu
				displayd = 0;
			}
			else{//if clicked on no
				displayd = 0;
				displaymode = GAME;//display game
			}
		}
		break;
	case LOSE:
		if (MouseY < hs * 0.65 && MouseY > hs * 0.55){//if clicked in range
			displayd = 0;
			displaymode = GAME;//display game
			Load();//load game
		}
		if (MouseX > ws - 0.15 && MouseY < 0.1){//if clicked menu
			displaymode = MENU;//display menu
			displayd = 0;
		}
		break;
	default://if it is other value
		displaymode = MENU;//return to menu
		break;
	}
	
	return;//exit function
}

void Draged(void){
	if (displaymode == GAME && select){//if player is selected
		#ifdef MOBILE
		double x, y;//x and y position of mouse in in game coordinate
		x = player.x - (last.x - MouseX)*SPEED;//get position
		y = player.y - (last.y - MouseY)*SPEED;
		x = x < 0 ? 0 : x > 1 ? 1 : x;//get x in to range of 0 to 1
		y = y < 0 ? 0 : y > 1 ? 1 : y;//get y in to range of 0 to 1
		PlayerMovement.x += fabs(player.x - x);//get player movement
		PlayerMovement.y += fabs(player.y - y);
		player.x = x;//set player position
		player.y = y;
		last.x = MouseX;//set last x and y position
		last.y = MouseY;
		#else
		double x, y;//x and y position of mouse in in game coordinate
		x = (MouseX - cx - PLAYERSIZE / 2) / (1 - PLAYERSIZE);//get position
		y = (MouseY - cy - PLAYERSIZE / 2) / (1 - PLAYERSIZE);
		x = x < 0 ? 0 : x > 1 ? 1 : x;//get x in to range of 0 to 1
		y = y < 0 ? 0 : y > 1 ? 1 : y;//get y in to range of 0 to 1
		PlayerMovement.x += fabs(player.x - x);//get player movement
		PlayerMovement.y += fabs(player.y - y);
		player.x = x;//set player position to mouse position
		player.y = y;
		#endif
	}
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
		SDL_DestroyTexture(Menu);
		Menu = GetTextTexture(font_24, "Menu", 0, 0, 255);//menu button
		displayd = 0;
		Draw();//redraw everything
	}
}

void DrawBase(void){//draw basic stuff
	SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);//draw yellow
	SDL_RenderClear(renderer);//clear screen
	DrawIMG(sound ? Sound : Mute, ws - 0.05, hs - 0.05, NULL, 0.05, 0.05, 1);//draw sound toggle
	if (displaymode == GAME) DrawIMG(Background, 0.5 * ws, 0.5 * hs, NULL, 1, 1, 1);//draw background in center
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
	SDL_Texture *Buttons;//button text
	char string[1024];//string to fill
	int i;
	switch (displaymode){//switch for each thing to display
	case MENU:
		if (displayd) break;
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);//draw white
		SDL_RenderClear(renderer);//clear screen
		DrawBase();//draw background image
		DrawIMG(MenuBackground, 0.5 * ws, 0.5 * hs, NULL, hs * 2, hs, 1);//draw Menu background
		Buttons = GetTextTexture(font_16, "Play", 0, 0, 255);//play button
		DrawText(Buttons, 0.5 * ws, 0.4 * hs, NULL, 1);//draw play button
		SDL_DestroyTexture(Buttons);//destroy texture
		Buttons = GetTextTexture(font_16, "Levels", 0, 0, 255);//play button
		DrawText(Buttons, 0.5 * ws, 0.6 * hs, NULL, 1);//draw levels button
		SDL_DestroyTexture(Buttons);//destroy texture
		displayd = 1;//set displayd
		SDL_RenderPresent(renderer);//present rendered
		break;
	case LEVEL:
		if (displayd) break;
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);//draw white
		SDL_RenderClear(renderer);//clear screen
		DrawBase();//draw background image
		if (levelview){//if not in first view
			Buttons = GetTextTexture(font_8, "<", 0, 0, 255);//back button
			DrawText(Buttons, 0.05, 0.5 * hs, NULL, 1);//draw back button
			SDL_DestroyTexture(Buttons);//destroy texture
		}
		if ((levelview + 1) * (1 / LEVELBUTTONSPACING) * (1 / LEVELBUTTONSPACING) < maxlevel){//if not in last view
			Buttons = GetTextTexture(font_8, ">", 0, 0, 255);//next button
			DrawText(Buttons, ws - 0.05, 0.5 * hs, NULL, 1);//draw next button
			SDL_DestroyTexture(Buttons);//destroy texture
		}
		for (i = 1; i + (int)(levelview * (1 / LEVELBUTTONSPACING) * (1 / LEVELBUTTONSPACING)) <= maxlevel; i++){//for each button
			DrawIMG(LevelButtonIMG, cx + LEVELBUTTONSPACING / 2 + ((i - 1) % (int)(1 / LEVELBUTTONSPACING)) * LEVELBUTTONSPACING, cy + LEVELBUTTONSPACING / 2 + ((i - 1) / (int)(1 / LEVELBUTTONSPACING)) * LEVELBUTTONSPACING, NULL, LEVELBUTTON, LEVELBUTTON, 1);//draw circle
			sprintf(string, "%d", i + (int)(levelview * (1 / LEVELBUTTONSPACING) * (1 / LEVELBUTTONSPACING)));//load level name
			Buttons = GetTextTexture(font_24, string, 0, 0, 255);//next button
			DrawText(Buttons, cx + LEVELBUTTONSPACING / 2 + ((i - 1) % (int)(1 / LEVELBUTTONSPACING)) * LEVELBUTTONSPACING, cy + LEVELBUTTONSPACING / 2 + ((i - 1) / (int)(1 / LEVELBUTTONSPACING)) * LEVELBUTTONSPACING, NULL, 1);//draw next button
			SDL_DestroyTexture(Buttons);//destroy texture
		}
		DrawText(Menu, ws - 0.075, 0.05, NULL, 1);//draw menu button
		displayd = 1;//set displayd
		SDL_RenderPresent(renderer);//present rendered
		break;
	case GAME:
		if (!displayd){//if not desplayd
			SDL_DestroyTexture(Level);
			sprintf(string, "Level %d", level);//generate level label
			Level = GetTextTexture(font_24, string, 0, 0, 255);//level label
		}
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);//draw white
		SDL_RenderClear(renderer);//clear screen
		DrawBase();//draw background image
		DrawIMG(ExitIMG, exitpos.x * (1 - PLAYERSIZE) + cx + PLAYERSIZE / 2, exitpos.y * (1 - PLAYERSIZE) + cy + PLAYERSIZE / 2, NULL, PLAYERSIZE, PLAYERSIZE, 1);//draw exit
		for (i = 0; i < ENEMY; i++){//for each enemy
			if (enemys[i].image){//if there is enemy there
				DrawIMG(EnemyIMG[enemys[i].image - 1], enemys[i].cur.x * (1 - PLAYERSIZE) + cx + PLAYERSIZE / 2, enemys[i].cur.y * (1 - PLAYERSIZE) + cy + PLAYERSIZE / 2, NULL, PLAYERSIZE, PLAYERSIZE, 1);//draw enemy
			}
		}
		DrawIMG(PlayerIMG, player.x * (1 - PLAYERSIZE) + cx + PLAYERSIZE / 2, player.y * (1 - PLAYERSIZE) + cy + PLAYERSIZE / 2, NULL, PLAYERSIZE, PLAYERSIZE, 1);//draw player
		DrawText(Menu, ws - 0.075, 0.05, NULL, 1);//draw menu button
		DrawText(Level, 0.1, 0.05, NULL, 1);//draw level label
		displayd = 1;//set displayd
		SDL_RenderPresent(renderer);//present rendered
		break;
	case WIN:
		if (displayd) break;
		if (level == maxlevel && level < MAXLEVEL){//if solved newest level
			maxlevel++;//unlock next level
		}
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);//draw white
		SDL_RenderClear(renderer);//clear screen
		DrawBase();//draw background image
		sprintf(string, "You Escaped Level %d", level);//generate message
		Buttons = GetTextTexture(font_16, string, 0, 0, 255);//win message
		DrawText(Buttons, 0.5 * ws, 0.5 * hs, NULL, 1);//draw win message
		SDL_DestroyTexture(Buttons);//destroy texture
		Buttons = GetTextTexture(font_16, "Replay", 0, 0, 255);//Replay button
		DrawText(Buttons, 0.5 * ws, 0.5 * hs + 0.1, NULL, 1);//draw Replay button
		SDL_DestroyTexture(Buttons);//destroy texture
		if (level > 1){//if not in first level
			Buttons = GetTextTexture(font_8, "<", 0, 0, 255);//back button
			DrawText(Buttons, 0.05, 0.5 * hs, NULL, 1);//draw back button
			SDL_DestroyTexture(Buttons);//destroy texture
		}
		if (level < MAXLEVEL){//if not in last level possible
			Buttons = GetTextTexture(font_8, ">", 0, 0, 255);//next button
			DrawText(Buttons, ws - 0.05, 0.5 * hs, NULL, 1);//draw next button
			SDL_DestroyTexture(Buttons);//destroy texture
		}
		DrawText(Menu, ws - 0.075, 0.05, NULL, 1);//draw menu button
		displayd = 1;//set displayd
		SDL_RenderPresent(renderer);//present rendered
		break;
	case EXIT:
		if (displayd) break;
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);//draw white
		SDL_RenderClear(renderer);//clear screen
		DrawBase();//draw background image
		Buttons = GetTextTexture(font_16, "Do you really want to go to menu?", 0, 0, 255);//go to menu question message
		DrawText(Buttons, 0.5 * ws, 0.5 * hs, NULL, 1);//draw go to menu question message
		SDL_DestroyTexture(Buttons);//destroy texture
		Buttons = GetTextTexture(font_16, "Yes", 0, 0, 255);//yes button
		DrawText(Buttons, 0.3 * ws, 0.6 * hs, NULL, 1);//draw yes button
		SDL_DestroyTexture(Buttons);//destroy texture
		Buttons = GetTextTexture(font_16, "No", 0, 0, 255);//no button
		DrawText(Buttons, 0.7 * ws, 0.6 * hs, NULL, 1);//draw no button
		SDL_DestroyTexture(Buttons);//destroy texture
		displayd = 1;//set displayd
		SDL_RenderPresent(renderer);//present rendered
		break;
	case LOSE:
		if (displayd) break;
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);//draw white
		SDL_RenderClear(renderer);//clear screen
		DrawBase();//draw background image
		Buttons = GetTextTexture(font_16, "You Lost", 0, 0, 255);//lose message
		DrawText(Buttons, 0.5 * ws, 0.5 * hs, NULL, 1);//draw lose message
		SDL_DestroyTexture(Buttons);//destroy texture
		Buttons = GetTextTexture(font_16, "Replay", 0, 0, 255);//Replay button
		DrawText(Buttons, 0.5 * ws, 0.5 * hs + 0.1, NULL, 1);//draw Replay button
		SDL_DestroyTexture(Buttons);//destroy texture
		DrawText(Menu, ws - 0.075, 0.05, NULL, 1);//draw menu button
		displayd = 1;//set displayd
		SDL_RenderPresent(renderer);//present rendered
		break;
	}
}

void LoadFile(void){//load from save file
	FILE *file = fopen(SAVE, "rb");//open source file
	if (file == NULL){//if file could not be opened
		printf("Save file %s could not be opened. Creating new save file\n", SAVE);//send error message
		maxlevel = 1;//start from 1
		sound = 1;
		SaveFile();//create new save file
		return;//exit function
	}
	char string[1024];//string to fill
	char *str;//string copy
	str = fgets(string, 1024, file);//get string
	if (str == NULL){//if at end of file
		printf("Invalid save file.\n");//send error message
		maxlevel = 1;//start from 1
		sound = 1;
		SaveFile();//create new save file
		return;//exit function
	}
	maxlevel = atoi(str);//get max level
	str = fgets(string, 1024, file);//get string
	if (str == NULL){//if at end of file
		printf("Invalid save file.\n");//send error message
		maxlevel = 1;//start from 1
		sound = 1;
		SaveFile();//create new save file
		return;//exit function
	}
	sound = atoi(str);//get sound
	fclose(file);//close file
}

void SaveFile(void){//save to save file
	FILE *file = fopen(SAVE, "wb");//open source file
	if (file == NULL){//if file could not be opened
		printf("Save file %s could not be made\n", SAVE);//send error message
		return;//exit function
	}
	fprintf(file, "%d\n", maxlevel);//save file
	fprintf(file, "%d", sound);
	fclose(file);//close file
}

void Load(void){//load level from file
	char string[1024];//string to fill
	char *str;//string copy
	sprintf(string, "%slevel%d.txt", RESOURCES, level);//load file name
	FILE *file = fopen(string, "rb");//open source file
	if (file == NULL){//if file could not be opened
		printf("Level file %s could not be opened.\n", string);//send error message
		displaymode = MENU;//display menu
		displayd = 0;
		return;//exit function
	}
	str = fgets(string, 1024, file);//get string
	if (str == NULL){//if at end of file
		printf("Invalid level file.\n");//send error message
		displaymode = MENU;//display menu
		displayd = 0;
		return;//exit function
	}
	SDL_DestroyTexture(Background);//destroy texture
	Background = GetTexture(strtok(str, "\n\t\r :;,{}[]\"\""));//get background image texture
	fgets(string, 1024, file);//skip line
	int i;
	for (i = 0; i < HOME; i++){//for each home position
		str = fgets(string, 1024, file);//get string
		if (str == NULL){//if at end of file
			printf("Invalid level file.\n");//send error message
			displaymode = MENU;//display menu
			displayd = 0;
			return;//exit function
		}
		home[i].x = atof(strtok(str, " ,:;()<>\t\n\'\"\\/?&*^%$#@!~_=qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM"));//get character omitting everything other that number
		home[i].y = atof(strtok(NULL, " ,:;()<>\t\n\'\"\\/?&*^%$#@!~_=qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM"));//get character omitting everything other that number
	}
	player = home[0];//get player starting position
	fgets(string, 1024, file);//skip line
	str = fgets(string, 1024, file);//get string
	if (str == NULL){//if at end of file
		printf("Invalid level file.\n");//send error message
		displaymode = MENU;//display menu
		displayd = 0;
		return;//exit function
	}
	exitpos.x = atof(strtok(str, " ,:;()<>\t\n\'\"\\/?&*^%$#@!~_=qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM"));//get character omitting everything other that number
	exitpos.y = atof(strtok(NULL, " ,:;()<>\t\n\'\"\\/?&*^%$#@!~_=qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM"));//get character omitting everything other that number
	fgets(string, 1024, file);//skip line
	for (i = 0; i < ENEMY; i++){//for each enemy 
		str = fgets(string, 1024, file);//get string
		if (str == NULL){//if at end of file
			printf("Invalid level file.\n");//send error message
			displaymode = MENU;//display menu
			displayd = 0;
			return;//exit function
		}
		enemys[i].image = atoi(strtok(str, " ,:;()<>\t\n\'\"\\/?&*^%$#@!~_=qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM"));//get character omitting everything other that number
		str = fgets(string, 1024, file);//get string
		if (str == NULL){//if at end of file
			printf("Invalid level file.\n");//send error message
			displaymode = MENU;//display menu
			displayd = 0;
			return;//exit function
		}
		enemys[i].cur.x = enemys[i].start.x = atof(strtok(str, " ,:;()<>\t\n\'\"\\/?&*^%$#@!~_=qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM"));//get character omitting everything other that number
		enemys[i].cur.y = enemys[i].start.y = atof(strtok(NULL, " ,:;()<>\t\n\'\"\\/?&*^%$#@!~_=qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM"));//get character omitting everything other that number
		str = fgets(string, 1024, file);//get string
		if (str == NULL){//if at end of file
			printf("Invalid level file.\n");//send error message
			displaymode = MENU;//display menu
			displayd = 0;
			return;//exit function
		}
		enemys[i].speed = atof(strtok(str, " ,:;()<>\t\n\'\"\\/?&*^%$#@!~_=qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM"));//get character omitting everything other that number
		str = fgets(string, 1024, file);//get string
		if (str == NULL){//if at end of file
			printf("Invalid level file.\n");//send error message
			displaymode = MENU;//display menu
			displayd = 0;
			return;//exit function
		}
		enemys[i].range = atof(strtok(str, " ,:;()<>\t\n\'\"\\/?&*^%$#@!~_=qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM"));//get character omitting everything other that number
		fgets(string, 1024, file);//skip line
	}
	fclose(file);//close file
}

void AudioCallback(void *udata, Uint8 *stream, int len){//audio callback function
	if (wav_current == 0){
		wav_current = wav_length;//copy length
		audio_pos = mixData;//copy data
	}

	SDL_memset(stream, 0, len);  // make sure this is silence.
	/* Mix as much data as possible */
	len = (len > wav_current ? wav_current : len);
	// mix audio against the silence, at 100% volume.
	SDL_MixAudioFormat(stream, audio_pos, deviceFormat, len, SDL_MIX_MAXVOLUME * sound);
	audio_pos += len;
	wav_current -= len;
}

double GetHome(void){//get distance to closest home
	double distance;//distance to home
	int i;
	for (i = 0; i < HOME; i++){//for each home
		if (i == 0 || hypot(home[i].x - player.x, home[i].y - player.y) < distance){//if it is the first loop or home is closer then last ones checked
			distance = hypot(home[i].x - player.x, home[i].y - player.y);//set distance
		}
	}
	return distance;
}


















































































