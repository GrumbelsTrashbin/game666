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
//{
	//public:
		//int x;
		//int y;
		//two_coords();
		//two_coords(int x, int y);
//};
typedef two_coords point;
typedef two_coords size;
typedef two_coords vector;

typedef unsigned int tiletype;
typedef short tile_variable;
SDL_Color make_color(Uint8 r, Uint8 g, Uint8 b);
SDL_Color multiply_color(SDL_Color in_color, float multiplier);

class render_engine;
//{
	//private:
		//float color_multiplier;
	//public:
		//render_engine();
		//render_engine(float color_multiplier);
		//void render(SDL_Surface& surface, SDL_Color color, vector offset);
	//private:
		//void _render(SDL_Surface& surface, SDL_Color color, vector offset);
//};
class compound_render_engine: public render_engine;
 //{
	//private:
		//float color_multiplier;
		//static render_engine* renderers[0];
	//public:
		//compound_render_engine();
		//compound_render_engine(float color_multiplier);
	//private:
		//void init_subengines();
//};
namespace render;
 //{
	//class solid: public render_engine {
		//void _render(SDL_Surface* surface, SDL_Color color, vector offset);
	//};
//};

class tile_type;
 //{
	//public:
		//SDL_Color color;
		//render_engine* engine;
		//tile_type(tile_factory* factory, SDL_Color color, render_engine* engine);
//};

struct tile;
 //{
	//public:
		//tiletype type;
		//tile_variable var;
//};

class tile_factory;
 //{
	//private:
		//tiletype next_id;
		//std::list<tile_type*> tiletypes;
	//public:
		//tiletype register_tiletype(tile_type* new_type);
//};

class chunk;
//{
	//private:
		//tile_factory* factory;
		//bool tile_exists(int x, int y);
	//public:
		//tile terrain_tile[CHUNK_WIDTH][CHUNK_HEIGHT];
		//chunk(tile_factory* factory);
		//tile get_tile(int x, int y);
		//bool set_tile(int x, int y, tile value);
		//bool set_tile_type(int x, int y, tiletype value);
		//bool set_tile_var(int x, int y, tile_variable value);
//};

class game_data;
 //{
	//public:
		//SDL_Surface* texture;
		//gameData();
		//bool load(std::string path);
		//bool unload();
		//bool unload(std::string resource);
//};

class game_world;
//{
	//public:
		//void simulate();
//};

class game666;
 //{
	//private:
		//bool		running;
		//SDL_Event	event;
		//SDL_Surface*	screen;
		//gameData	data;
		//gameWorld*	world;
		//tile*		a_tile;
	//public:
		//game666();
		//int run();
	//private:
		//void simulate();
		//void initialize(point resolution);
		//void check_events();
		//void load_data(std::string resource);
		//void render();
		//void unload_data(std::string resource);
		//int quit();
//};

int main(int argc, char** argv);