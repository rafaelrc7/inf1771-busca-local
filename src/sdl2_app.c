#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL.h>

#include "map.h"

#define APP_NAME "INF1771 T1"

#define WIDTH	640
#define HEIGHT	480

struct _sdl2_app {
	SDL_Window *window;
	SDL_Renderer *win_renderer;
	SDL_Surface *win_surface;
	SDL_Texture *win_texture;
};
typedef struct _sdl2_app SDL2_App;

static void sdl2_fatal(const char *const prompt);
static int mainloop(int argc, char **argv, SDL2_App app);
static void scale_pixels(uint32_t *dest, uint32_t *src,
					const uint64_t dest_width,
					const uint64_t dest_height,
					const uint64_t src_width,
					const uint64_t src_height);

static int mainloop(int argc, char **argv, SDL2_App app) {
	uint8_t keep_alive = 1;

	uint32_t pixels[WIDTH * HEIGHT];
	uint32_t map_pixels[300 * 82];

	Map *map = map_create_from_file(300, 82, stdin);

	memset(pixels, 0xFF, WIDTH * HEIGHT * sizeof(uint32_t));

	while (keep_alive) {
		SDL_Event e;

		map_to_pixels(map, map_pixels);

		SDL_UpdateTexture(app.win_texture, NULL, pixels, WIDTH * sizeof(uint32_t));

		while (SDL_PollEvent(&e)) {
			switch (e.type) {
				case SDL_QUIT:
					keep_alive = 0;
					break;
			}
		}

		SDL_RenderClear(app.win_renderer);
		SDL_RenderCopy(app.win_renderer, app.win_texture, NULL, NULL);
		SDL_RenderPresent(app.win_renderer);
	}

	map_destroy(map);
	return EXIT_SUCCESS;
}

int sdl2_app(int argc, char **argv) {
	SDL2_App app;
	int ret;

	int keep_alive = 1;

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		sdl2_fatal("SDL_Init()");

	app.window = SDL_CreateWindow(APP_NAME, SDL_WINDOWPOS_CENTERED,
				SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
	if (app.window == NULL)
		sdl2_fatal("SDL_CreateWindow()");

	app.win_surface = SDL_GetWindowSurface(app.window);
	if (app.win_surface == NULL)
		sdl2_fatal("SDL_GetWindowSurface()");

	app.win_renderer = SDL_CreateRenderer(app.window, -1, 0);
	if (app.win_renderer == NULL)
		sdl2_fatal("SDL_CreateRenderer()");

	app.win_texture = SDL_CreateTexture(app.win_renderer, SDL_PIXELFORMAT_ARGB8888,
								 SDL_TEXTUREACCESS_STATIC, WIDTH, HEIGHT);

	ret = mainloop(argc, argv, app);

	SDL_DestroyTexture(app.win_texture);
	SDL_DestroyRenderer(app.win_renderer);
	SDL_DestroyWindow(app.window);
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
	SDL_Quit();

	return ret;
}

static void sdl2_fatal(const char *const prompt) {
	fprintf(stderr, "%s: %s\n", prompt, SDL_GetError());
	exit(EXIT_FAILURE);
}

