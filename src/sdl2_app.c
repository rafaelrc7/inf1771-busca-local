#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL.h>

#include "map.h"

#define APP_NAME "INF1771 T1"

#define SCALE		5
#define M_WIDTH		300
#define M_HEIGHT	82

struct _sdl2_app {
	size_t width, height;
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
	FILE *mf;
	uint8_t keep_alive = 1;

	uint32_t *pixels = (uint32_t *)calloc(app.width * app.height,
											sizeof(uint32_t));
	uint32_t map_pixels[M_WIDTH * M_HEIGHT];

	if (argc > 2) {
		mf = fopen(argv[2], "r");
		if (mf == NULL) {
			perror("fopen()");
			free(pixels);
			return EXIT_FAILURE;
		}
	} else {
		mf = stdin;
	}
	Map *map = map_create_from_file(300, 82, mf);

	memset(pixels, 0xFF, app.width * app.height * sizeof(uint32_t));

	while (keep_alive) {
		SDL_Event e;

		map_to_pixels(map, map_pixels);
		scale_pixels(pixels, map_pixels, app.width, app.height, map_get_width(map),
			   map_get_height(map));

		SDL_UpdateTexture(app.win_texture, NULL, pixels, app.width * sizeof(uint32_t));

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

	free(pixels);
	map_destroy(map);
	return EXIT_SUCCESS;
}

int sdl2_app(int argc, char **argv) {
	SDL2_App app;
	int ret;

	app.width = M_WIDTH * SCALE;
	app.height = M_HEIGHT * SCALE;

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		sdl2_fatal("SDL_Init()");

	app.window = SDL_CreateWindow(APP_NAME, SDL_WINDOWPOS_CENTERED,
				SDL_WINDOWPOS_CENTERED, app.width, app.height, 0);
	if (app.window == NULL)
		sdl2_fatal("SDL_CreateWindow()");

	app.win_surface = SDL_GetWindowSurface(app.window);
	if (app.win_surface == NULL)
		sdl2_fatal("SDL_GetWindowSurface()");

	app.win_renderer = SDL_CreateRenderer(app.window, -1, 0);
	if (app.win_renderer == NULL)
		sdl2_fatal("SDL_CreateRenderer()");

	app.win_texture = SDL_CreateTexture(app.win_renderer, SDL_PIXELFORMAT_ARGB8888,
								 SDL_TEXTUREACCESS_STATIC, app.width, app.height);

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

static void scale_pixels(uint32_t *dest, uint32_t *src,
						  const uint64_t dest_width,
						  const uint64_t dest_height,
						  const uint64_t src_width,
						  const uint64_t src_height)
{
	uint64_t src_h, src_w;
	uint64_t dest_h, dest_w;
	uint64_t hr = dest_height / src_height;
	uint64_t wr = dest_width  / src_width;

	for (src_h = 0, dest_h = 0; src_h < src_height; ++src_h, dest_h += hr) {
		for (src_w = 0, dest_w = 0; src_w < src_width; ++src_w, dest_w += wr) {
			uint64_t w, h;
			for (h = dest_h; h < (dest_h + hr); ++h) {
				for (w = dest_w; w < (dest_w + wr); ++w) {
					dest[h * dest_width + w] = src[src_h * src_width + src_w];
				}
			}

		}
	}
}

