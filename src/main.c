#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SFML/Graphics.h>

int WIN_W = 1060;
int WIN_H = 600;

#define OK 1
#define QUIT 0

sfTexture *ufo_flying, *ufo_landing, *ufo_beaming;
sfTexture *alien_facing, *alien_right, *alien_left, *alien_win, *alien_dead;
sfTexture *helicopter_1_right, *helicopter_2_right, *helicopter_1_left, *helicopter_2_left;
sfTexture *text_bubble;
sfTexture *bullet, *boom;

#include "window.c"
#include "main_menu.c"
#include "intro.c"
#include "city.c"

extern int show_main_menu();
void load_textures();

int main()
{
	time_t t;
	srand((unsigned) time(&t));

	printf("\nHello World :D\n");

	window_create();
	window_clear();

	load_textures();

	while (1)
	{
		window_set_info("W/A - Up/Down keys to navigate. Space or Enter to activate");

		if (show_main_menu() == QUIT)
		{
			printf("\nQuit recieved from main menu. Exiting...");
			break;
		}

		if (show_intro() == QUIT)
		{
			printf("\nQuit recieved from intro. Exiting...");
			break;
		}

		score = 0;

		if (start_city() == QUIT)
		{
			printf("\nQuit recieved from city. Exiting...");
			break;
		}
	}

	printf("\n\nGoodye World :(\n\n");

	window_close(window);

	return(0);
}

void load_textures()
{
	ufo_flying = sfTexture_createFromFile("resources/ufo_flying.png", NULL);
	ufo_landing = sfTexture_createFromFile("resources/ufo_landing.png", NULL);
	ufo_beaming = sfTexture_createFromFile("resources/ufo_beaming.png", NULL);

	alien_facing = sfTexture_createFromFile("resources/alien_facing.png", NULL);
	alien_right = sfTexture_createFromFile("resources/alien_right.png", NULL);
	alien_left = sfTexture_createFromFile("resources/alien_left.png", NULL);
	alien_win = sfTexture_createFromFile("resources/alien_win.png", NULL);
	alien_dead = sfTexture_createFromFile("resources/alien_dead.png", NULL);

	helicopter_1_right = sfTexture_createFromFile("resources/helicopter_1_right.png", NULL);
	helicopter_1_left = sfTexture_createFromFile("resources/helicopter_1_left.png", NULL);
	helicopter_2_right = sfTexture_createFromFile("resources/helicopter_2_right.png", NULL);
	helicopter_2_left = sfTexture_createFromFile("resources/helicopter_2_left.png", NULL);

	bullet = sfTexture_createFromFile("resources/bullet.png", NULL);
	boom = sfTexture_createFromFile("resources/boom.png", NULL);

	text_bubble = sfTexture_createFromFile("resources/text_bubble.png", NULL);
}
