#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SFML/Graphics.h>
#include <SFML/Audio.h>

int WIN_W = 1060;
int WIN_H = 600;

#define OK 1
#define QUIT 0

sfTexture *ufo_flying, *ufo_landing, *ufo_beaming;
sfTexture *alien_facing, *alien_right, *alien_left, *alien_win, *alien_dead;
sfTexture *helicopter_1_right, *helicopter_2_right, *helicopter_1_left, *helicopter_2_left;
sfTexture *text_bubble;
sfTexture *bullet, *boom;

sfMusic *main_menu, *city_1, *city_2, *city_3, *intro;

sfSoundBuffer *menu_tick_buf, *explode_buf, *pickup_buf;
sfSound *menu_tick, *explode, *pickup;

#include "window.c"
#include "main_menu.c"
#include "intro.c"
#include "city.c"

extern int show_main_menu();
void load_textures(), load_sounds(), unload_sounds();

int main()
{
	time_t t;
	srand((unsigned) time(&t));

	printf("\nHello World :D\n");

	window_create();
	window_clear();

	load_textures();
	load_sounds();

	while (1)
	{
		window_set_info("W/A - Up/Down keys to navigate. Space or Enter to activate");

		sfMusic_play(main_menu);

		if (show_main_menu() == QUIT)
		{
			printf("\nQuit recieved from main menu. Exiting...");
			break;
		}

		sfMusic_stop(main_menu);
		sfMusic_play(intro);

		if (show_intro() == QUIT)
		{
			printf("\nQuit recieved from intro. Exiting...");
			break;
		}

		score = 0;
		sfMusic_stop(intro);
		sfMusic_play(city_1);

		if (start_city() == QUIT)
		{
			printf("\nQuit recieved from city. Exiting...");
			break;
		}

		sfMusic_stop(city_1);
		sfMusic_stop(city_2);
		sfMusic_stop(city_3);
	}

	unload_sounds();

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

void load_sounds()
{
	menu_tick_buf = sfSoundBuffer_createFromFile("resources/menu_tick.wav");
	menu_tick = sfSound_create();
	sfSound_setBuffer(menu_tick, menu_tick_buf);

	explode_buf = sfSoundBuffer_createFromFile("resources/explode.wav");
	explode = sfSound_create();
	sfSound_setBuffer(explode, explode_buf);

	pickup_buf = sfSoundBuffer_createFromFile("resources/pickup.wav");
	pickup = sfSound_create();
	sfSound_setBuffer(pickup, pickup_buf);

	main_menu = sfMusic_createFromFile("resources/music/main_menu.wav");
	sfMusic_setLoop(main_menu, sfTrue);
	sfMusic_setVolume(main_menu, 50);

	city_1 = sfMusic_createFromFile("resources/music/city_1.wav");
	sfMusic_setLoop(city_1, sfTrue);
	sfMusic_setVolume(city_1, 25);
	city_2 = sfMusic_createFromFile("resources/music/city_2.wav");
	sfMusic_setLoop(city_2, sfTrue);
	sfMusic_setVolume(city_2, 35);
	city_3 = sfMusic_createFromFile("resources/music/city_3.wav");
	sfMusic_setLoop(city_3, sfTrue);
	sfMusic_setVolume(city_3, 50);

	intro = sfMusic_createFromFile("resources/music/intro.wav");
	sfMusic_setLoop(intro, sfTrue);
	sfMusic_setVolume(intro, 15);
}

void unload_sounds()
{
	sfSoundBuffer_destroy(menu_tick_buf);
	sfSound_destroy(menu_tick);

	sfSoundBuffer_destroy(explode_buf);
	sfSound_destroy(explode);

	sfSoundBuffer_destroy(pickup_buf);
	sfSound_destroy(pickup);

	sfMusic_destroy(main_menu);

	sfMusic_destroy(city_1);
	sfMusic_destroy(city_2);
	sfMusic_destroy(city_3);

	sfMusic_destroy(intro);
}
