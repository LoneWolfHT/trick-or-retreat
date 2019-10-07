#ifndef LD45_MAIN_MENU
#define LD45_MAIN_MENU

#include <SFML/Graphics.h>
#include <time.h>

#define OK 1
#define QUIT 0

#define PLAY 0
#define EXIT 1

#define V2F (sfVector2f)

#include "window.c"
#include "city.c"
#include "util.c"

extern int WIN_W, WIN_H;
extern int score;

extern sfTexture *background_png;
static sfSprite *buttons[2];
static sfSprite *background;
static sfText *labels[2];
static sfText *title;
static sfText *scorelabel;

static sfTexture *button_up;
static sfTexture *button_down;

void main_menu_refresh();

int show_main_menu()
{
	sfEvent event;
	int selected = 0;

	background = sfSprite_create();
	labels[PLAY] = sfText_create();
	labels[EXIT] = sfText_create();
	button_up = sfTexture_createFromFile("resources/button_grey_up.png", NULL);
	button_down = sfTexture_createFromFile("resources/button_grey_down.png", NULL);

	title = sfText_create();
	scorelabel = sfText_create();

	window_set_debug(" ");
	window_clear_event_queue();

	if (score >= 0)
	{
		char scoretext[40];

		sfText_setFont(scorelabel, font);
		sfText_setCharacterSize(scorelabel, 36);
		sfText_setPosition(title, V2F{(WIN_W/2) - 250, WIN_H/10});
		sfText_setOutlineColor(scorelabel, sfColor_fromRGB(255, 147, 0));
		sfText_setColor(scorelabel, sfGreen);
		sfText_setOutlineThickness(scorelabel, 2);

		if (!ALIEN_DEAD)
		{
			sprintf(scoretext, "You retreated. Your score: %d", score);
			sfText_move(scorelabel, V2F{275 - (getsize(score)*17), 175});
		}
		else
		{
			sprintf(scoretext, "You were caught and taken to Area 51");
			sfText_move(scorelabel, V2F{275 - (6*17), 175}); // 17 is the rough size of a character
		}

		sfText_setString(scorelabel, scoretext);
	}

	sfText_setString(title, "Trick Or Retreat");
	sfText_setFont(title, font);
	sfText_setCharacterSize(title, 50);
	sfText_setPosition(title, V2F{(WIN_W/2) - 250, WIN_H/10});
	sfText_setColor(title, sfColor_fromRGB(255, 147, 0));
	sfText_setOutlineColor(title, sfRed);
	sfText_setOutlineThickness(title, 2);
	sfText_move(title, V2F{45, 11});

	sfText_setString(labels[PLAY], "Play");
	sfText_setFont(labels[PLAY], font);
	sfText_setPosition(labels[PLAY], V2F{(WIN_W/2) - (32*2.5), WIN_H/2.7});
	sfText_move(labels[PLAY], V2F{47, 11+50});

	sfText_setString(labels[EXIT], "Exit");
	sfText_setFont(labels[EXIT], font);
	sfText_setPosition(labels[EXIT], V2F{(WIN_W/2) - (32*2.5), (WIN_H/2.7) + (30*4)});
	sfText_move(labels[EXIT], V2F{49, 11+50});

	if (background_png == NULL)
	{
		printf("\n[ERROR] Couldn't find texture");
		return(QUIT);
	}

	sfSprite_setTexture(background, background_png, sfFalse);
	sfSprite_setPosition(background, V2F{0, 0});
	sfSprite_setScale(background, V2F{WIN_W/960.0, WIN_H/540.0});

	buttons[PLAY] = sfSprite_create();
	buttons[EXIT] = sfSprite_create();

	sfSprite_setTexture(buttons[PLAY], button_up, sfFalse);
	sfSprite_setPosition(buttons[PLAY], V2F{(WIN_W/2) - (32*2.5), WIN_H/2.7});
	sfSprite_setScale(buttons[PLAY], V2F{5, 4});
	sfSprite_move(buttons[PLAY], V2F{0, 50});

	sfSprite_setTexture(buttons[EXIT], button_up, sfFalse);
	sfSprite_setPosition(buttons[EXIT], V2F{(WIN_W/2) - (32*2.5), (WIN_H/2.7) + (30*4)});
	sfSprite_setScale(buttons[EXIT], V2F{5, 4});
	sfSprite_move(buttons[EXIT], V2F{0, 50});

	printf("\nShowing main menu...");

	sfSprite_setTexture(buttons[selected], button_down, sfFalse);
	sfText_setColor(labels[selected], sfRed);
	sfText_setColor(labels[1], sfColor_fromRGB(255, 147, 0));

	main_menu_refresh();

	while(sfRenderWindow_isOpen(window))
	{
		while (sfRenderWindow_pollEvent(window, &event))
		{
			main_menu_refresh();

			if (event.type == sfEvtClosed)
			{
				printf("\nRecieved window close request. Closing...\n\n");
				return(QUIT);
			}
			else if (event.type == sfEvtKeyPressed)
			{
				if (selected > 0 && (event.key.code == sfKeyUp || event.key.code == sfKeyW))
				{
					printf("\nButton pressed, moving selection up...");
					sfSprite_setTexture(buttons[selected], button_up, sfFalse);
					sfText_setColor(labels[selected], sfColor_fromRGB(255, 147, 0));
					--selected;
					sfSprite_setTexture(buttons[selected], button_down, sfFalse);
					sfText_setColor(labels[selected], sfRed);
					sfSound_play(menu_tick);
				}
				else if (selected < 1 && (event.key.code == sfKeyDown || event.key.code == sfKeyS))
				{
					printf("\nButton pressed, moving selection down...");
					sfSprite_setTexture(buttons[selected], button_up, sfFalse);
					sfText_setColor(labels[selected], sfColor_fromRGB(255, 147, 0));
					++selected;
					sfSprite_setTexture(buttons[selected], button_down, sfFalse);
					sfText_setColor(labels[selected], sfRed);
					sfSound_play(menu_tick);
				}
				else if (event.key.code == sfKeySpace || event.key.code == sfKeyEnter)
				{
					if (selected == 0)
					{
						printf("\nSpace/Enter key pressed. Play selected");
						sfSound_play(menu_tick);
						return(OK);
					}
					else if (selected == 1)
					{
						printf("\nSpace/Enter key pressed. Exit selected");
						return(QUIT);
					}
				}
			}
		}
	}

	printf("\nExiting at bottom of main menu func. Something may be wrong");

	return(OK);
}

void main_menu_refresh()
{
	window_clear();
	sfRenderWindow_drawSprite(window, background, NULL);
	sfRenderWindow_drawSprite(window, buttons[PLAY], NULL);
	sfRenderWindow_drawSprite(window, buttons[EXIT], NULL);
	sfRenderWindow_drawText(window, labels[PLAY], NULL);
	sfRenderWindow_drawText(window, labels[EXIT], NULL);
	sfRenderWindow_drawText(window, title, NULL);
	sfRenderWindow_drawText(window, scorelabel, NULL);
	window_refresh();
}

#endif
