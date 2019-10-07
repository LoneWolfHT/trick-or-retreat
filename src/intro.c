#ifndef LD45_INTRO
#define LD45_INTRO

#include <time.h>
#include <unistd.h>

#include "window.c"
#include "main_menu.c"
#include "util.c"

extern int WIN_W, WIN_H;
extern sfTexture *ufo_flying, *ufo_landing, *ufo_beaming;
extern sfTexture *alien_facing, *alien_right, *alien_left, *alien_dead;
extern sfTexture *helicopter_1_right, *helicopter_2_right;
extern sfTexture *text_bubble;

static sfSprite *background;
static sfSprite *txtbubble;
static sfSprite *ufo;
static sfSprite *alien;
static sfSprite *heli;

static sfText *txtbubbletxt;

#define OK 1
#define QUIT 0

#define V2F (sfVector2f)
#define UW 96 // UFO Width
#define AW 32 // Alien Width
#define HL 128 // Helicopter Length
#define TW 64 // Text Width

void intro_refresh();

int show_intro()
{
	int helitoggle = 0;
	double delta, delta2;
	double last_time, current_time;
	sfBool done = sfFalse;
	sfVector2f destination = {WIN_W/2, WIN_H - (UW*1.2)};

	background = sfSprite_create();
	txtbubble = sfSprite_create();
	ufo = sfSprite_create();
	alien = sfSprite_create();
	heli = sfSprite_create();

	txtbubbletxt = sfText_create();
	sfText_setFont(txtbubbletxt, font);
	sfText_setScale(txtbubbletxt, V2F{0.6, 0.6});
	sfText_setColor(txtbubbletxt, sfBlack);
	sfText_setString(txtbubbletxt, "Alien sighted! All units engage!");
	sfText_setPosition(txtbubbletxt, V2F{WIN_W, WIN_H});

	printf("\nShowing intro...");

	window_set_info(" ");

	sfSprite_setTexture(background, background_png, sfTrue);
	sfSprite_setScale(background, V2F{WIN_W/960.0, WIN_H/540.0});

	sfSprite_setTexture(txtbubble, text_bubble, sfTrue);
	sfSprite_setPosition(txtbubble, V2F{HL*2.9, (WIN_H/2) - 30});
	sfSprite_setPosition(txtbubble, V2F{WIN_W, WIN_H});
	sfSprite_setScale(txtbubble, V2F{5.7, 2});

	sfSprite_setTexture(ufo, ufo_flying, sfTrue);
	sfSprite_setPosition(ufo, V2F{WIN_W-UW, 0});
	sfSprite_setScale(ufo, V2F{3, 3});

	sfSprite_setTexture(heli, helicopter_1_right, sfTrue);
	sfSprite_setPosition(heli, V2F{-HL, WIN_H/2});
	sfSprite_setScale(heli, V2F{2, 2});

	sfSprite_setTexture(alien, alien_facing, sfTrue);
	sfSprite_setPosition(alien, V2F{1000, 1000});


	// UFO Flies Down

	delta = 0.0;
	last_time = current_time = clock();
	while(sfRenderWindow_isOpen(window))
	{
		current_time = clock();
		delta += (double)((current_time - last_time)/CLOCKS_PER_SEC);
		last_time = current_time;

		if (delta > 0.018)
		{
			sfSprite_move(ufo, v2fMultiply(v2fDir(sfSprite_getPosition(ufo), destination), delta * 200));
			delta = 0.0;
		}

		if (!done && v2fDist(sfSprite_getPosition(ufo), destination) <= 100)
		{
			done = sfTrue;
			sfSprite_setTexture(ufo, ufo_landing, sfTrue);
		}

		if (v2fDist(sfSprite_getPosition(ufo), destination) <= 3)
			break;

		intro_refresh();
	}

	// UFO shoots beam

	sfSprite_setTexture(ufo, ufo_beaming, sfTrue);
	intro_refresh();
	sleep(1);

	// Alien appears

	sfSprite_setPosition(alien, V2F{destination.x+AW+2, destination.y+AW+AW});
	intro_refresh();
	sleep(1);

	// Beam Stops

	sfSprite_setTexture(ufo, ufo_landing, sfTrue);
	intro_refresh();
	sleep(1);
	sfSprite_setTexture(alien, alien_right, sfTrue);

	// UFO flies off. Alien walks off

	done = sfFalse;

	delta = 0.0;
	last_time = current_time = clock();
	while(sfRenderWindow_isOpen(window))
	{
		current_time = clock();
		delta += (double)((current_time - last_time)/CLOCKS_PER_SEC);
		last_time = current_time;

		if (delta > 0.018)
		{
			if (sfSprite_getPosition(ufo).y >= -UW)
				sfSprite_move(ufo, v2fMultiply(V2F{0, -1}, delta * 200));
			else
				break;

			if (sfSprite_getPosition(alien).x <= WIN_W + AW/2)
				sfSprite_move(alien, v2fMultiply(V2F{1, 0}, delta * 200));

			delta = 0.0;
		}

		if (!done && sfSprite_getPosition(ufo).y <= 75)
		{
			done = sfTrue;
			sfSprite_setTexture(ufo, ufo_flying, sfTrue);
		}

		intro_refresh();
	}

	// Helicopter flies into scene

	delta = delta2 = 0.0;
	last_time = current_time = clock();
	while(sfRenderWindow_isOpen(window))
	{
		current_time = clock();
		delta += (double)((current_time - last_time)/CLOCKS_PER_SEC);
		delta2 += (double)((current_time - last_time)/CLOCKS_PER_SEC);
		last_time = current_time;

		if (delta2 > 0.05)
		{
			if (helitoggle)
			{
				sfSprite_setTexture(heli, helicopter_1_right, sfTrue);
				--helitoggle;
			}
			else
			{
				sfSprite_setTexture(heli, helicopter_2_right, sfTrue);
				++helitoggle;
			}
			delta2 = 0.0;
		}

		if (delta > 0.018)
		{
			if (sfSprite_getPosition(heli).x <= HL * 2)
				sfSprite_move(heli, v2fMultiply(V2F{1, 0}, delta * 200));
			else
				break;

			delta = 0.0;
		}

		intro_refresh();
	}

	// Radio message pops up

	sfSprite_setPosition(txtbubble, V2F{HL*2.9, (WIN_H/2) - 30});
	sfText_setPosition(txtbubbletxt, sfSprite_getPosition(txtbubble));
	sfText_move(txtbubbletxt, V2F{50, 15});

	intro_refresh();

	delta = delta2 = 0.0;
	last_time = current_time = clock();
	while(delta2 <= 1.7)
	{
		current_time = clock();
		delta += (current_time - last_time)/(CLOCKS_PER_SEC);
		delta2 += (current_time - last_time)/(CLOCKS_PER_SEC);
		last_time = current_time;

		if (delta > 0.05)
		{
			if (helitoggle)
			{
				sfSprite_setTexture(heli, helicopter_1_right, sfTrue);
				--helitoggle;
			}
			else
			{
				sfSprite_setTexture(heli, helicopter_2_right, sfTrue);
				++helitoggle;
			}

			delta = 0.0;
		}

		intro_refresh();
	}

	sfSprite_setPosition(txtbubble, V2F{WIN_W, WIN_H});
	sfText_setPosition(txtbubbletxt, V2F{WIN_W, WIN_H});

	intro_refresh();

	// Helicopter follows alien

	delta = delta2 = 0.0;
	last_time = current_time = clock();
	while(sfRenderWindow_isOpen(window))
	{
		current_time = clock();
		delta += (double)((current_time - last_time)/CLOCKS_PER_SEC);
		delta2 += (double)((current_time - last_time)/CLOCKS_PER_SEC);
		last_time = current_time;

		if (delta2 > 0.1)
		{
			if (helitoggle)
			{
				sfSprite_setTexture(heli, helicopter_1_right, sfTrue);
				--helitoggle;
			}
			else
			{
				sfSprite_setTexture(heli, helicopter_2_right, sfTrue);
				++helitoggle;
			}
		}

		if (delta > 0.018)
		{
			if (!(sfSprite_getPosition(heli).x >= HL * 6))
				sfSprite_move(heli, v2fMultiply(V2F{1, 0}, delta * 200));
			else
				break;

			delta = 0.0;
		}

		intro_refresh();
	}

	return(OK);
}

void intro_refresh()
{
	window_clear();
	sfRenderWindow_drawSprite(window, background, NULL);
	sfRenderWindow_drawSprite(window, ufo, NULL);
	sfRenderWindow_drawSprite(window, alien, NULL);
	sfRenderWindow_drawSprite(window, heli, NULL);
	sfRenderWindow_drawSprite(window, txtbubble, NULL);
	sfRenderWindow_drawText(window, txtbubbletxt, NULL);
	window_refresh();
}

#endif
