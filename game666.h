#ifndef __GAME666_H__
#define __GAME666_H__

#include <iostream>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <string>
#include <math.h>
#include <list>

#ifndef CACHE_IMAGES
	#define CACHE_IMAGES 8
#endif
#ifndef CACHE_IMAGES_EDGES
	#define CACHE_IMAGES_EDGES CACHE_IMAGES/2
#endif
#ifndef CACHE_IMAGES_CORENRS
	#define CACHE_IMAGES_CORNERS CACHE_IMAGES_EDGES/2
#endif
#ifndef FRAMERATE
	#define FRAMERATE 30
#endif
#ifndef CHUNK_SIZE
	#define CHUNK_SIZE 32
#endif
#ifndef CHUNK_WIDTH
	#define CHUNK_WIDTH CHUNK_SIZE
#endif
#ifndef CHUNK_HEIGHT
	#define CHUNK_HEIGHT CHUNK_SIZE
#endif
#ifndef TILE_SIZE
	#define TILE_SIZE 24
#endif
#ifndef TILE_WIDTH
	#define TILE_WIDTH TILE_SIZE
#endif
#ifndef TILE_HEIGHT
	#define TILE_HEIGHT TILE_SIZE
#endif

int randint(int min, int max);
void limit_fps(int framerate);
SDL_Surface* load_image(std::string path);

class two_coords;

typedef two_coords point;
typedef two_coords size;
typedef two_coords vector;

typedef unsigned int tiletype;
typedef short tile_variable;
SDL_Color make_color(Uint8 r, Uint8 g, Uint8 b);
SDL_Color multiply_color(SDL_Color in_color, float multiplier);

class render_engine;
class compound_render_engine;
class tile_type;
struct tile;

class tile_factory {
	private:
		tiletype next_id;
		std::list<tile_type*> tiletypes;
	public:
		tiletype register_tiletype(tile_type* new_type);
};


class chunk;
class game_data;
class game_world;
class game666;
int main(int argc, char** argv);

#endif
