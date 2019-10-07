#ifndef LD45_WINDOW
#define LD45_WINDOW

#include <SFML/Graphics.h>

#define WINDOW_LIST_LIMIT 500 // How many of an object that the window can refresh at once

sfText *debug;
sfText *info;
sfRenderWindow *window;
sfFont* font;
sfTexture *background_png, *background_city_png;

void window_clear();

int window_create()
{
	sfVideoMode mode = {WIN_W, WIN_H+64, 32};

    font = sfFont_createFromFile("resources/xolonium/regular.ttf");
    debug = sfText_create();
    sfText_setFont(debug, font);
    sfText_setCharacterSize(debug, 18);
    sfText_setPosition(debug, (sfVector2f){0, 0});
	sfText_setColor(debug, sfColor_fromRGB(255, 147, 0));

	info = sfText_create();
    sfText_setFont(info, font);
    sfText_setCharacterSize(info, 18);
    sfText_setPosition(info, (sfVector2f){0, WIN_H+5});
	sfText_setColor(info, sfColor_fromRGB(255, 147, 0));

	background_png = sfTexture_createFromFile("resources/background.png", NULL);
	background_city_png = sfTexture_createFromFile("resources/background_city.png", NULL);
    window = sfRenderWindow_create(mode, "Trick or Retreat (LD45)", sfClose, NULL);

	return(window != NULL);
}

void window_clear_event_queue()
{
	sfEvent event;
	while(sfRenderWindow_pollEvent(window, &event) != sfFalse);
}

void window_refresh()
{
	sfRenderWindow_drawText(window, debug, NULL);
	sfRenderWindow_drawText(window, info, NULL);
	sfRenderWindow_display(window);
}

void window_clear()
{
	sfRenderWindow_clear(window, sfBlack);
}

void window_close()
{
	sfRenderWindow_destroy(window);
}

void window_set_debug(char *text)
{
	sfText_setString(debug, text);
}

void window_set_info(char *text)
{
	sfText_setString(info, text);
}

#endif
