#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "SDL2/SDL_render.h"
#include "astar.h"
#include "map.h"
#include "settings.h"

#include "font/vcr_osd_mono.h"

#define APP_NAME "INF1771 T1"

#define SCALE		5

struct _sdl2_app {
	size_t width, height;
	size_t m_width, m_height;
	TTF_Font *font;
	SDL_Window *window;
	SDL_Renderer *win_renderer;
	SDL_Texture *map_texture, *win_texture;
	const char *waypoints;
	size_t waypoint_num;
};
typedef struct _sdl2_app SDL2_App;

static size_t diff(const char c);

static void sdl2_fatal(const char *const prompt);
static void ttf_fatal(const char *const prompt);
static int mainloop(SDL2_App app);
static void scale_pixels(uint32_t *dest, uint32_t *src,
					const uint64_t dest_width,
					const uint64_t dest_height,
					const uint64_t src_width,
					const uint64_t src_height);

static int mainloop(SDL2_App app) {
	SDL_Surface *ui_surface;
	SDL_Texture *ui_texture;
	double stage_time = -1, total_time = 0;
	char buff[256];
	Astar *astar = NULL;
	size_t astar_index = 0;
	uint8_t keep_alive = 1;
	uint8_t solved = 0;
	uint8_t step = 1;
	uint8_t runtoend = 0;
	uint8_t showpath = 0;

	uint32_t *pixels = (uint32_t *)calloc(app.width * app.height,
											sizeof(uint32_t));
	uint32_t *map_pixels = (uint32_t *)calloc(app.m_width * app.m_height,
												sizeof(uint32_t));
	uint32_t *path_pixels = (uint32_t *)calloc(app.m_width * app.m_height,
												sizeof(uint32_t));

	Map *map = map_create_from_file(app.m_width, app.m_height, stdin);

	SDL_Rect stage_ui_rect, total_ui_rect;

	TTF_SizeText(app.font, "STAGE 00/00 (12345)", &stage_ui_rect.w, &stage_ui_rect.h);
	stage_ui_rect.x = 5;
	stage_ui_rect.y = app.height - stage_ui_rect.h - 5;

	TTF_SizeText(app.font, "TOTAL: 00000", &total_ui_rect.w, &total_ui_rect.h);
	total_ui_rect.x = 5;
	total_ui_rect.y = app.height - stage_ui_rect.h - total_ui_rect.h - 5;

	memset(pixels, 0xFF, app.width * app.height * sizeof(uint32_t));

	//astar = astar_init(map_get_buff(map), M_WIDTH, M_HEIGHT, '1', '2', &diff);
	while (keep_alive) {
		SDL_Event e;
		SDL_SetRenderTarget(app.win_renderer, app.win_texture);

		if (astar != NULL && !step && !solved)
			solved = astar_step(astar);

		if (solved && stage_time < 0) {
			stage_time = astar_time(astar);
			total_time += stage_time;
			astar_markpath(astar, path_pixels);

			if (runtoend) {
				if (astar_index < app.waypoint_num-1) {
					stage_time = -1;
					astar_free(astar);
					astar = NULL;
					++astar_index;
					stage_time = -1;
					solved = 0;
					step = 0;
					astar = astar_init(	map_get_buff(map),
										app.m_width,
										app.m_height,
										app.waypoints[astar_index],
										app.waypoints[astar_index + 1],
										&diff);
				} else {
					runtoend = 0;
				}
			}
		}

		if (astar != NULL)
			astar_to_pixels(astar, map_pixels, &map_cell_colour);
		else
			map_to_pixels(map, map_pixels);

		if (showpath) {
			size_t x, y;
			for (y = 0; y < app.m_height; ++y) {
				for (x = 0; x < app.m_width; ++x) {
					if (path_pixels[y * app.m_width + x]) {
						map_pixels[y * app.m_width + x] = 0xFFB37400;
					}
				}
			}
		}

		scale_pixels(pixels, map_pixels, app.width, app.height, map_get_width(map),
			   map_get_height(map));

		SDL_UpdateTexture(app.map_texture, NULL, pixels, app.width * sizeof(uint32_t));

		while (SDL_PollEvent(&e)) {
			switch (e.type) {
				case SDL_QUIT:
					keep_alive = 0;
					break;

				case SDL_KEYDOWN:
					switch (e.key.keysym.sym) {
						case SDLK_RETURN:
							if (solved) {
								stage_time = -1;
								astar_free(astar);
								astar = NULL;
								if (++astar_index == app.waypoint_num) {
									astar_index = 0;
									total_time = 0;
								}
							} else if (astar != NULL) {
								step = 0;
							}

							if (astar == NULL) {
								stage_time = -1;
								solved = 0;
								step = 1;
								astar = astar_init(	map_get_buff(map),
													app.m_width,
													app.m_height,
													app.waypoints[astar_index],
													app.waypoints[astar_index + 1],
													&diff);
							}
							break;

						case SDLK_TAB:
							if (astar != NULL && !solved) {
								solved = astar_step(astar);
							}
							break;

						case SDLK_r:
							runtoend = !runtoend;
							step = !runtoend;
							break;

						case SDLK_p:
							showpath = !showpath;
							break;

						case SDLK_ESCAPE:
							if (astar != NULL) {
								total_time = 0;
								stage_time = -1;
								solved = 0;
								step = 1;
								astar_index = 0;
								astar_free(astar);
								astar = NULL;
							}
							break;
					}
					break;
			}
		}

		SDL_RenderCopy(app.win_renderer, app.map_texture, NULL, NULL);

		if (stage_time > -1)
			snprintf(buff, sizeof(buff), "STAGE %02lu/%02lu (%5.0f)", astar_index+1, app.waypoint_num, stage_time);
		else
			snprintf(buff, sizeof(buff), "STAGE %02lu/%02lu (  ?  )", astar_index+1, app.waypoint_num);

		ui_surface = TTF_RenderText_Solid(app.font, buff, (SDL_Color){0x0, 0x0, 0x0});
		if (ui_surface == NULL)
			ttf_fatal("TTF_RenderText_Solid()");

		ui_texture = SDL_CreateTextureFromSurface(app.win_renderer, ui_surface);
		if (ui_texture == NULL)
			sdl2_fatal("SDL_CreateTextureFromSurface()");

		SDL_RenderCopy(app.win_renderer, ui_texture, NULL, &stage_ui_rect);

		SDL_FreeSurface(ui_surface);
		SDL_DestroyTexture(ui_texture);

		snprintf(buff, sizeof(buff), "TOTAL: %5.0f", total_time);

		ui_surface = TTF_RenderText_Solid(app.font, buff, (SDL_Color){0x0, 0x0, 0x0});
		if (ui_surface == NULL)
			ttf_fatal("TTF_RenderText_Solid()");

		ui_texture = SDL_CreateTextureFromSurface(app.win_renderer, ui_surface);
		if (ui_texture == NULL)
			sdl2_fatal("SDL_CreateTextureFromSurface()");

		SDL_RenderCopy(app.win_renderer, ui_texture, NULL, &total_ui_rect);

		SDL_SetRenderTarget(app.win_renderer, NULL);
		SDL_RenderClear(app.win_renderer);
		SDL_RenderCopy(app.win_renderer, app.win_texture, NULL, NULL);
		SDL_RenderPresent(app.win_renderer);

		SDL_FreeSurface(ui_surface);
		SDL_DestroyTexture(ui_texture);
	}

	if (astar != NULL)
		astar_free(astar);
	free(pixels);
	free(path_pixels);
	free(map_pixels);
	map_destroy(map);
	return EXIT_SUCCESS;
}

int sdl2_app(size_t m_width, size_t m_height, size_t width, size_t height,
			 const char *waypoints, size_t waypoint_num) {
	SDL_RWops *font_mem;
	SDL2_App app;
	int ret;

	app.m_width			= m_width	> 0 ? m_width	: D_WIDTH;
	app.m_height		= m_height	> 0 ? m_height	: D_HEIGHT;
	app.width			= width		> 0 ? width		: app.m_width	* SCALE;
	app.height			= height	> 0 ? height	: app.m_height	* SCALE;
	app.waypoints		= waypoints;
	app.waypoint_num	= waypoint_num;

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		sdl2_fatal("SDL_Init()");
	if (TTF_Init() < 0)
		ttf_fatal("TTF_Init()");

	font_mem = SDL_RWFromConstMem(	VCR_OSD_MONO_ttf,
									VCR_OSD_MONO_ttf_len);
	if (font_mem == NULL)
		sdl2_fatal("SDL_RWFromConstMem()");

	app.font = TTF_OpenFontIndexRW(font_mem, 1, 24, 0);
	if (app.font == NULL)
		ttf_fatal("TTF_OpenFontIndexRW()");

	app.window = SDL_CreateWindow(APP_NAME, SDL_WINDOWPOS_CENTERED,
				SDL_WINDOWPOS_CENTERED, app.width, app.height, 0);
	if (app.window == NULL)
		sdl2_fatal("SDL_CreateWindow()");

	app.win_renderer = SDL_CreateRenderer(app.window, -1, 0);
	if (app.win_renderer == NULL)
		sdl2_fatal("SDL_CreateRenderer()");

	app.win_texture = SDL_CreateTexture(app.win_renderer, SDL_PIXELFORMAT_ARGB8888,
								 SDL_TEXTUREACCESS_TARGET, app.width, app.height);
	if (app.win_texture == NULL)
		sdl2_fatal("SDL_CreateTexture()");

	SDL_SetTextureBlendMode(app.win_texture, SDL_BLENDMODE_BLEND);

	app.map_texture = SDL_CreateTexture(app.win_renderer, SDL_PIXELFORMAT_ARGB8888,
								 SDL_TEXTUREACCESS_STREAMING, app.width, app.height);
	if (app.map_texture == NULL)
		sdl2_fatal("SDL_CreateTexture()");

	ret = mainloop(app);

	SDL_DestroyTexture(app.win_texture);
	SDL_DestroyTexture(app.map_texture);
	SDL_DestroyRenderer(app.win_renderer);
	SDL_DestroyWindow(app.window);
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
	TTF_CloseFont(app.font);
	TTF_Quit();
	SDL_Quit();

	return ret;
}

static void sdl2_fatal(const char *const prompt) {
	fprintf(stderr, "%s: %s\n", prompt, SDL_GetError());
	exit(EXIT_FAILURE);
}

static void ttf_fatal(const char *const prompt) {
	fprintf(stderr, "%s: %s\n", prompt, TTF_GetError());
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

static size_t diff(const char c) {
	switch (c) {
		case '.':
			return 1;

		case 'R':
			return 5;

		case 'F':
			return 10;

		case 'A':
			return 15;

		case 'M':
			return 200;

		default:
			return 0;
			break;
	}
}

