#ifndef LD45_CITY
#define LD45_CITY

#include <time.h>
#include <unistd.h>

#include "window.c"
#include "main_menu.c"
#include "util.c"

#define V2F (sfVector2f)

#define UW 96 // UFO Width
#define AW 32 // Alien Width

#define BULLETSPEED 600
#define AS 200 // Alien Speed
#define US 777 // UFO Speed

float HS = 90; // Helicopter Speed
float HFS = 2.1; // Helicopter Fire Speed

#define DW 27 // Door Width
#define BW 16 // Bullet/Boom Width
#define HL 128 // Helicopter Length
#define HH 189 // Helicopter Fly Height

#define RIGHT 1
#define LEFT 0

#define HPOS sfSprite_getPosition(heli)
#define APOS sfSprite_getPosition(alien)

#define RIGHT_1 0
#define RIGHT_2 1
#define LEFT_1 2
#define LEFT_2 3

static sfSprite *background;
static sfSprite *ufo;
static sfSprite *alien;
static sfSprite *heli;
static sfSprite *doorsteps[4];

static sfSprite *bullets[100];
static int bulletpos = 0;

struct
{
	sfSprite *sprite;
	double life;
} booms[50];
static int boompos = 0;

extern sfTexture *ufo_flying, *ufo_landing, *ufo_beaming;
extern sfTexture *alien_facing, *alien_right, *alien_left, *alien_dead;
extern sfTexture *helicopter_1_right, *helicopter_2_right, *helicopter_1_left, *helicopter_2_left;
extern sfTexture *bullet, *boom;

double doorspos[4] = {147.6, 410.6, 671.6, 925.6};

int score = -1;
sfBool ALIEN_DEAD = sfFalse;
static int hstate;

extern int WIN_W, WIN_H;

extern int getlen(), find();

void city_refresh(), retreat_refresh();
int retreat();

void bullets_fire(), bullets_remove_all();
int bullets_boom();
void booms_add(), booms_remove(), booms_remove_all();

int start_city()
{
	window_clear();

	double delta, delta2, delta3, delta4;
	double dhold;
	double last_time, current_time;
	int dir = RIGHT;
	int i;

	hstate = RIGHT_1;

	ALIEN_DEAD = sfFalse;

	if (HS <= AS)
		HS += 10;

	if (HFS >= 0.5)
		HFS -= 0.1;

	if (HFS >= 1.0 && HFS < 1.5 && sfMusic_getStatus(city_2) != sfPlaying)
	{
		sfMusic_stop(city_1);
		sfMusic_play(city_2);
	}
	else if (HFS >= 0.5 && HFS < 0.7 && sfMusic_getStatus(city_3) != sfPlaying)
	{
		sfMusic_stop(city_2);
		sfMusic_play(city_3);
	}

	sfTexture *doorstep = sfTexture_createFromFile("resources/doorstep.png", NULL);
	sfTexture *doorstep_looted = sfTexture_createFromFile("resources/doorstep_looted.png", NULL);

	background = sfSprite_create();
	ufo = sfSprite_create();
	heli = sfSprite_create();
	alien = sfSprite_create();

	sfSprite_setTexture(heli, helicopter_1_right, sfTrue);
	sfSprite_setScale(heli, V2F{2, 2});

	if (mrand(1, 20) > 12)
		sfSprite_setPosition(heli, V2F{-HL, HH});
	else
		sfSprite_setPosition(heli, V2F{WIN_W, HH});

	for (i = 0; i <= 3; ++i)
	{
		doorsteps[i] = sfSprite_create();
		sfSprite_setTexture(doorsteps[i], doorstep, sfTrue);
		sfSprite_setPosition(doorsteps[i], V2F{doorspos[i], WIN_H - 34.0});
	}

	window_set_info("A/D|Left/Right keys to navigate. Space or Enter to activate/Ask for Candy. W/Up keys to retreat");

	sfSprite_setTexture(background, background_city_png, sfTrue);
	sfSprite_setScale(background, V2F{WIN_W/960.0, WIN_H/540.0});

	sfSprite_setTexture(ufo, ufo_landing, sfTrue);
	sfSprite_setScale(ufo, V2F{3, 3});
	sfSprite_setPosition(ufo, V2F{-UW, 0});

	sfSprite_setTexture(alien, alien_right, sfTrue);
	sfSprite_setPosition(alien, V2F{0, WIN_H - (AW+20)});

	bullets_remove_all();

	// It's Trick or Retreat time!

	delta = delta2 = delta3 = delta4 = 0.0;
	last_time = current_time = clock();
	while(sfRenderWindow_isOpen(window))
	{
		current_time = clock();
		dhold = (double)((current_time - last_time)/CLOCKS_PER_SEC);
		delta += dhold;
		delta2 += dhold;
		delta3 += dhold;
		delta4 += dhold;
		last_time = current_time;

		if (delta > 0.018)
		{
			if ((sfKeyboard_isKeyPressed(sfKeyA) || sfKeyboard_isKeyPressed(sfKeyLeft)) && sfSprite_getPosition(alien).x > 0)
			{
				sfSprite_move(alien, v2fMultiply(V2F{-1, 0}, delta * AS));
				if (dir != LEFT)
				{
					dir = LEFT;
					sfSprite_setTexture(alien, alien_left, sfTrue);
				}
			}
			else if ((sfKeyboard_isKeyPressed(sfKeyD) || sfKeyboard_isKeyPressed(sfKeyRight)) && sfSprite_getPosition(alien).x <= WIN_W)
			{
				sfSprite_move(alien, v2fMultiply(V2F{1, 0}, delta * AS));
				if (dir != RIGHT)
				{
					dir = RIGHT;
					sfSprite_setTexture(alien, alien_right, sfTrue);
				}
			}
			else if (sfKeyboard_isKeyPressed(sfKeyW) || sfKeyboard_isKeyPressed(sfKeyUp))
			{
				return(retreat());
			}
			else if (sfKeyboard_isKeyPressed(sfKeyEscape))
				return(QUIT);
			else if (sfKeyboard_isKeyPressed(sfKeySpace) || sfKeyboard_isKeyPressed(sfKeyEnter))
				for (i = 0; i <= 3; ++i)
				{
					sfFloatRect alienbox = sfSprite_getGlobalBounds(alien);
					if (sfFloatRect_contains(&alienbox, doorspos[i]+16, WIN_H - 35.0) && sfSprite_getTexture(doorsteps[i]) != doorstep_looted)
					{
						sfSprite_setTexture(doorsteps[i], doorstep_looted, sfTrue);
						sfSound_play(pickup);
						++score;
					}
				}

			if (HPOS.x+(HL/2) > APOS.x)
			{
				if (abs(HPOS.x+(HL/2) - APOS.x) > 1 && hstate != LEFT_1 && hstate != LEFT_2)
				{
					sfSprite_setTexture(heli, helicopter_1_left, sfTrue);
					hstate = LEFT_1;
				}

				sfSprite_move(heli, v2fMultiply(V2F{-1, 0}, delta * HS));
			}
			else
			{
				if (abs(HPOS.x+(HL/2) - APOS.x) > 1 && hstate != RIGHT_1 && hstate != RIGHT_2)
				{
					sfSprite_setTexture(heli, helicopter_1_right, sfTrue);
					hstate = RIGHT_1;
				}

				sfSprite_move(heli, v2fMultiply(V2F{1, 0}, delta * HS));
			}

			for (i = 0; i < bulletpos; ++i)
			{
				if (sfSprite_getPosition(bullets[i]).y < APOS.y + BW)
					sfSprite_move(bullets[i], v2fMultiply(V2F{0, 1}, delta * BULLETSPEED));
				else if (bullets_boom(i) == 0) // Alien was hit
				{
					sfSprite_setTexture(alien, alien_dead, sfFalse);
					ALIEN_DEAD = sfTrue;
					bullets_remove_all();
					city_refresh();
					sleep(1);
					return(OK);
				}
			}

			delta = 0.0;
		}

		if (delta2 >= 0.05)
		{
			switch(hstate)
			{
				case RIGHT_1:
					sfSprite_setTexture(heli, helicopter_2_right, sfTrue);
					hstate = RIGHT_2;
					break;
				case RIGHT_2:
					sfSprite_setTexture(heli, helicopter_1_right, sfTrue);
					hstate = RIGHT_1;
					break;
				case LEFT_1:
					sfSprite_setTexture(heli, helicopter_2_left, sfTrue);
					hstate = LEFT_2;
					break;
				case LEFT_2:
					sfSprite_setTexture(heli, helicopter_1_left, sfTrue);
					hstate = LEFT_1;
					break;
				default:
					break;
			}

			delta2 = 0.0;
		}

		if (delta3 >= HFS)
		{
			switch(hstate)
			{
				case RIGHT_1:
					bullets_fire(HPOS.x+(HL*0.63));
					break;
				case RIGHT_2:
					bullets_fire(HPOS.x+(HL*0.63));
					break;
				case LEFT_1:
					bullets_fire(HPOS.x+(HL*0.4));
					break;
				case LEFT_2:
					bullets_fire(HPOS.x+(HL*0.4));
					break;
				default:
					break;
			}
			delta3 = 0.0;
		}

		if (delta4 >= 0.1)
		{
			for (i = 0; i < boompos; ++i)
				if ((current_time - booms[i].life) / CLOCKS_PER_SEC > 0.1)
					booms_remove(i);

			delta4 = 0.0;
		}

		if (sfSprite_getPosition(alien).x >= WIN_W)
			return(start_city());

		city_refresh();
	}

	return(OK);
}

int retreat()
{
	sfVector2f apos = sfSprite_getPosition(alien);
	double delta, delta2;
	double last_time, current_time;
	sfBool done = sfFalse;

	sfSprite_setTexture(alien, alien_win, sfTrue);
	sfSprite_setPosition(ufo, V2F{-UW, (apos.y+AW)-UW});

	window_clear();
	bullets_remove_all();

	// UFO flies in

	delta = delta2 = 0.0;
	last_time = current_time = clock();
	while(sfRenderWindow_isOpen(window))
	{
		current_time = clock();
		delta += (double)((current_time - last_time)/CLOCKS_PER_SEC);
		delta2 += (double)((current_time - last_time)/CLOCKS_PER_SEC);
		last_time = current_time;

		if (delta > 0.018)
		{
			sfSprite_move(ufo, v2fMultiply(V2F{1, 0}, delta * US));
			delta = 0.0;
		}

		if (delta2 >= 0.05)
		{
			switch(hstate)
			{
				case RIGHT_1:
					sfSprite_setTexture(heli, helicopter_2_right, sfTrue);
					hstate = RIGHT_2;
					break;
				case RIGHT_2:
					sfSprite_setTexture(heli, helicopter_1_right, sfTrue);
					hstate = RIGHT_1;
					break;
				case LEFT_1:
					sfSprite_setTexture(heli, helicopter_2_left, sfTrue);
					hstate = LEFT_2;
					break;
				case LEFT_2:
					sfSprite_setTexture(heli, helicopter_1_left, sfTrue);
					hstate = LEFT_1;
					break;
				default:
					break;
			}

			delta2 = 0.0;
		}

		if (apos.x <= sfSprite_getPosition(ufo).x+(UW/2.5))
			break;

		window_set_info(" ");
		retreat_refresh();
	}

	done = sfFalse;

	sfSprite_setTexture(ufo, ufo_beaming, sfTrue);
	retreat_refresh();
	sleep(1);
	sfSprite_setPosition(alien, V2F{WIN_W, WIN_H});
	sfSprite_setTexture(ufo, ufo_landing, sfTrue);
	retreat_refresh();

	// UFO flies off

	last_time = current_time = clock();
	while(sfRenderWindow_isOpen(window))
	{
		current_time = clock();
		delta += (double)((current_time - last_time)/CLOCKS_PER_SEC);
		last_time = current_time;

		if (delta > 0.018)
		{
			sfSprite_move(ufo, v2fMultiply(V2F{0, -1}, delta * US));
			delta = 0.0;
		}

		if (!done && sfSprite_getPosition(ufo).y <= 100)
		{
			done = sfTrue;
			sfSprite_setTexture(ufo, ufo_flying, sfTrue);
		}

		if (sfSprite_getPosition(ufo).y <= -UW)
			break;

		window_set_info(" ");
		retreat_refresh();
	}

	window_set_debug(" ");
	window_clear_event_queue();

	return(OK);
}

void retreat_refresh()
{
	window_clear();
	int i;
	sfRenderWindow_drawSprite(window, background, NULL);

	for (i = 0; i <= 3; ++i)
		sfRenderWindow_drawSprite(window, doorsteps[i], NULL);

	sfRenderWindow_drawSprite(window, ufo, NULL);
	sfRenderWindow_drawSprite(window, alien, NULL);
	sfRenderWindow_drawSprite(window, heli, NULL);

	window_refresh();
}

void city_refresh()
{
	window_clear();
	int i;
	char score_string[100];

	sfRenderWindow_drawSprite(window, background, NULL);

	for (i = 0; i < bulletpos; ++i)
		sfRenderWindow_drawSprite(window, bullets[i], NULL);

	for (i = 0; i <= 3; ++i)
		sfRenderWindow_drawSprite(window, doorsteps[i], NULL);

	for (i = 0; i < boompos; ++i)
		sfRenderWindow_drawSprite(window, booms[i].sprite, NULL);

	sfRenderWindow_drawSprite(window, ufo, NULL);
	sfRenderWindow_drawSprite(window, alien, NULL);
	sfRenderWindow_drawSprite(window, heli, NULL);

	sprintf(score_string, "[SCORE] %d", score);
	window_set_debug(score_string);

	window_refresh();
}

void booms_add(double xpos)
{
	booms[boompos].sprite = sfSprite_create();
	booms[boompos].life = clock();
	sfSprite_setTexture(booms[boompos].sprite, boom, sfTrue);
	sfSprite_setScale(booms[boompos].sprite, V2F{2.5, 1.5});
	sfSprite_setPosition(booms[boompos++].sprite, V2F{(xpos+(BW/2))-(BW*1.25), APOS.y + 7});
	sfSound_play(explode);
}

void bullets_fire(double xpos)
{
	bullets[bulletpos] = sfSprite_create();
	sfSprite_setTexture(bullets[bulletpos], bullet, sfTrue);
	sfSprite_setPosition(bullets[bulletpos++], V2F{xpos, HH + (BW*0.78)});
}

int bullets_boom(int bullet)
{
	int i;
	sfVector2f boomp = sfSprite_getPosition(bullets[bullet]);

	booms_add(boomp.x);

	if (abs((APOS.x + (AW/2)) - (boomp.x + (BW/1.25))) <= AW)
	{
		return(0); // Alien was hit
	}

	sfSprite_destroy(bullets[bullet]);

	for (i = bullet; i < bulletpos-1; ++i)
		bullets[i] = bullets[i+1];

	--bulletpos;
	return(1);
}


void booms_remove(int boom)
{
	int i;

	sfSprite_destroy(booms[boom].sprite);

	for (i = boom; i < boompos-1; ++i)
		booms[i] = booms[i+1];

	--boompos;
}

void bullets_remove_all()
{
	int i;

	for (i = 0; i < bulletpos; ++i)
		sfSprite_destroy(bullets[i]);

	bulletpos = 0;
}

void booms_remove_all()
{
	int i;

	for (i = 0; i < boompos; ++i)
		sfSprite_destroy(booms[i].sprite);

	bulletpos = 0;
}

#endif
