#include <iostream>
#include <math.h>
#include <string>
//#include <list>
#include <vector>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_gfxPrimitives.h>

#include "game666.h"

int randint(int min, int max){
	return rand() % (max-min+1)+min;
}
void limit_fps(float framerate) {
	static float last_time = 0.0f;
	float current_time=SDL_GetTicks()*0.001f;
	//printf("---------------\nTime difference: %f\nTarget time per frame: %f\nGonna sleep for: %f\n",current_time-last_time,(1.0f/framerate),(1.0f/framerate)-(current_time-last_time));
	if((1.0f/framerate)-(current_time-last_time)>0) {
		usleep(1000*1000*((1.0f/framerate)-(current_time-last_time)));
	}
	last_time=current_time;
}

SDL_Surface* load_image(std::string path) {
	SDL_Surface* texture;
	texture=IMG_Load(path.c_str());
	return SDL_DisplayFormat(texture);
}

class two_coords {
	public:
		int x;
		int y;
		two_coords() {}
		two_coords(int x, int y) {this->x=x;this->y=y;}
};
typedef two_coords point;
typedef two_coords size;
typedef two_coords vector;

typedef unsigned int tiletype;
typedef short tile_variable;
SDL_Color make_color(Uint8 r, Uint8 g, Uint8 b) {
	SDL_Color color;
	color.r=r;
	color.g=g;
	color.b=b;
	return color;
}
SDL_Color multiply_color(SDL_Color in_color, float multiplier) {
	SDL_Color out_color;
	out_color.r=std::min(int(in_color.r*multiplier),255);
	out_color.g=std::min(int(in_color.g*multiplier),255);
	out_color.b=std::min(int(in_color.b*multiplier),255);
	return out_color;
}
Uint32 uint_color(SDL_Surface* surface, SDL_Color color) {
	return SDL_MapRGB(surface->format, color.r, color.g, color.b);
}

class rengine_params {
	public:
		float color_multiplier;
		int value1;
		int value2;
		SDL_Color color;
		rengine_params() {
			this->color_multiplier=1.0f;
			this->value1=1.0;
			this->value2=1.0;
		}
		rengine_params(float color_multiplier) {
			this->color_multiplier=color_multiplier;
			this->value1=1.0;
			this->value2=1.0;
		}
		rengine_params(float color_multiplier, int value1) {
			this->color_multiplier=color_multiplier;
			this->value1=value1;
			this->value2=1.0;
		}
		rengine_params(float color_multiplier, int value1, int value2) {
			this->color_multiplier=color_multiplier;
			this->value1=value1;
			this->value2=value2;
		}
		rengine_params(SDL_Color color) {
			this->color=color;
			this->color_multiplier=1.0f;
			this->value1=1.0;
			this->value2=1.0;
		}
		rengine_params(SDL_Color color, float color_multiplier) {
			this->color=color;
			this->color_multiplier=color_multiplier;
			this->value1=1.0;
			this->value2=1.0;
		}
		rengine_params(SDL_Color color, float color_multiplier, int value1) {
			this->color=color;
			this->color_multiplier=color_multiplier;
			this->value1=value1;
			this->value2=1.0;
		}
		rengine_params(SDL_Color color, float color_multiplier, int value1, int value2) {
			this->color=color;
			this->color_multiplier=color_multiplier;
			this->value1=value1;
			this->value2=value2;
		}

};

class render_engine {
	protected:
		rengine_params params;
	public:
		render_engine() {}
		render_engine(rengine_params params) { this->params=params; }
		void render(SDL_Surface* surface, vector offset) {
			this->_render(surface, multiply_color(this->params.color,params.color_multiplier), offset);
		}
		void render(SDL_Surface* surface, SDL_Color color, vector offset) {
			this->_render(surface, multiply_color(color,params.color_multiplier), offset);
		}
	protected:
		virtual void _render(SDL_Surface* surface, SDL_Color color, vector offset) = 0;
};
class compound_render_engine: public render_engine {
	private:
		render_engine** engines;
		int num_of_engines;
	public:
		compound_render_engine(int num_of_engines, render_engine* engines[]) {
			this->engines=engines;
			this->num_of_engines=num_of_engines;
		}
		void render(SDL_Surface* surface, vector offset) {
			for(int i=0; i<num_of_engines; i++) {
				this->engines[i]->render(surface, offset);
			}
		}
		void render(SDL_Surface* surface, SDL_Color color, vector offset) {
			for(int i=0; i<num_of_engines; i++) {
				this->engines[i]->render(surface, color, offset);
			}
		}
	private:
		void _render(SDL_Surface* surface, SDL_Color color, vector offset) {} //De-virtualize this or C++ won't let us create an instance
};
namespace render {
	class nothing: public render_engine {
		public:
			nothing() {}
			nothing(rengine_params params) { this->params=params; }
			void _render(SDL_Surface* surface, SDL_Color color, vector offset) { }
	};
	class solid: public render_engine {
		public:
			solid(rengine_params params) { this->params=params; }
			void _render(SDL_Surface* surface, SDL_Color color, vector offset) {
				Uint32 u_color=SDL_MapRGB(surface->format, color.r, color.g, color.b);
				//Uint32 u_color=uint_color(surface, color);
				SDL_FillRect(surface, NULL, u_color);
			}
	};
	class rings: public render_engine {
		public:
			rings(rengine_params params) { this->params=params; }
			void _render(SDL_Surface* surface, SDL_Color color, vector offset) {
				for(int i=0;i < params.value1;i++) {
					circleRGBA(surface, randint(0,surface->w), randint(0,surface->h), params.value2, color.r, color.g, color.b, 255);
				}
			}
	};
	class spots: public render_engine {
		public:
			spots(rengine_params params) { this->params=params; }
			void _render(SDL_Surface* surface, SDL_Color color, vector offset) {
				//Uint32 u_color=SDL_MapRGB(surface->format, 255, 255, 255);
				for(int i=0;i < params.value1;i++) {
					filledCircleRGBA(surface, randint(0,surface->w), randint(0,surface->h), params.value2, color.r, color.g, color.b, 255); //uint_color(surface, color)
				}
			}
	};
	typedef compound_render_engine compound;
}

class tile_factory {
	private:
		tiletype next_id;
		std::vector<tile_type*> tiletypes;
	public:
		tile_factory() { next_id=0; }
		tiletype register_tiletype(tile_type* new_type);
		tile_type* get_tiletype(tiletype n) {
			if(n>tiletypes.size()) {return tiletypes.front(); /*So you better declare a null/air tiletype!*/}
			return tiletypes[n];
		}
};

class image_collection {
	public:
		SDL_Surface* image_center[CACHE_IMAGES];
		SDL_Surface* image_edge[CACHE_IMAGES_EDGES];
		SDL_Surface* image_corner[CACHE_IMAGES_CORNERS];
};

class tile_type {
	public:
		image_collection* images;
	protected:
		render_engine* engine;
	public:
		tile_type() {}
		tile_type(tile_factory* factory, render_engine* engine) {
			this->engine=engine;
			factory->register_tiletype(this);
			size* s = new size(TILE_WIDTH, TILE_HEIGHT);
			this->render(engine, *s);
			delete s;
			//ERROR: Gets here, but doesn't get back out in case of second tile_type.
		}
		void render(render_engine* renderer, size tile_size) {
			Uint32 rmask, gmask, bmask, amask;
			#if SDL_BYTEORDER == SDL_BIG_ENDIAN
				rmask = 0xff000000;
				gmask = 0x00ff0000;
				bmask = 0x0000ff00;
				amask = 0x000000ff;
			#else
				rmask = 0x000000ff;
				gmask = 0x0000ff00;
				bmask = 0x00ff0000;
				amask = 0xff000000;
			#endif
			vector* offset=new vector(0,0);
			for(int i=0; i<CACHE_IMAGES;i++) {
				images->image_center[i]=SDL_CreateRGBSurface(SDL_SWSURFACE, tile_size.x, tile_size.y, 32, rmask, gmask, bmask, amask); //new SDL_Surface(size.x,size.y);
				renderer->render(images->image_center[i], *offset);
			}
		}
		SDL_Surface* get_image(vector offset, int variation) {
			if(offset.x||offset.y) { return images->image_center[variation % (CACHE_IMAGES)]; } //center
			else if(offset.x&offset.y) { return images->image_corner[variation % (CACHE_IMAGES_CORNERS)]; } //corners
			else { return images->image_edge[variation % (CACHE_IMAGES_EDGES)]; } //edges
		}
};

struct tile {
	public:
		tiletype type;
		tile_variable var;
};

tiletype tile_factory::register_tiletype(tile_type* new_type) {
	tiletypes.push_back(new_type);
	return ++next_id;
}

class chunk {
	private:
		tile_factory* factory;
		bool tile_exists(int x, int y) {
			if((x>CHUNK_WIDTH)^(y>CHUNK_HEIGHT)^(x<0)^(y<0)) {return false;}
			return true;
		}
	public:
		tile terrain_tile[CHUNK_WIDTH][CHUNK_HEIGHT];
		chunk(tile_factory* factory) {
			this->factory=factory;
		}
		tile get_tile(int x, int y) {
			if (tile_exists(x,y)) { return terrain_tile[x][y]; }
			return terrain_tile[-1][-1];
		}
		bool set_tile(int x, int y, tile value) {
			if (tile_exists(x,y)) { terrain_tile[x][y]=value; return true;}
			return false;
		}
		bool set_tile_type(int x, int y, tiletype value) {
			if (tile_exists(x,y)) { terrain_tile[x][y].type=value; return true;}
			return false;
		}
		bool set_tile_var(int x, int y, tile_variable value) {
			if (tile_exists(x,y)) { terrain_tile[x][y].var=value; return true;}
			return false;
		}
};

class game_data {
	public:
		tile_factory* tileset;
	public:
		game_data() { tileset = new tile_factory(); }
		bool load() {
			//Air/null tile
			new tile_type(tileset, new render::nothing());
			//Grassy earth
			render::solid* a = new render::solid(rengine_params(make_color(114,85,53)));
			render::spots* b = new render::spots(rengine_params(make_color(100,200,100),1.0f,5,2));
			render_engine* c[2] = {a, b};
			render::compound* d = new render::compound(2,c);
			new tile_type(tileset, d);
			return true;
		}
		bool unload() {
			return false;
		}
		tile_type* get_tiletype(int n) {
			return tileset->get_tiletype(n);
		};
};

class game_world {
	private:
	public:
		void simulate() {

		}
};

class game666 {
	private:
		bool		running;
		SDL_Event	event;
		SDL_Surface*	screen;
		game_data	data;
		game_world*	world;
	public:
		game666() {}
		int run() {
			screen = NULL;
			this->initialize(point(320,240));
			this->data.load();
			//Main loop
			while(running) {
				limit_fps(FRAMERATE);
				this->check_events();
				this->simulate();
				this->render();
			}
			return this->quit();
		}
	private:
		void simulate() {
			this->world->simulate();
		}
		void initialize(point resolution) {
			SDL_Init(SDL_INIT_EVERYTHING);
			screen = SDL_SetVideoMode(resolution.x, resolution.y, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
		}
		int quit() {
			this->data.load();
			SDL_FreeSurface(screen);
			SDL_Quit();
			return 0;
		}
		void check_events() {
			while(SDL_PollEvent(&event)) {
				if(event.type == SDL_QUIT) {running=false;}
				else if(event.type == SDL_KEYDOWN)
				{

				}
			}
		}
		void render() {
			vector offset;
			offset.x=offset.y=0;
			this->data.get_tiletype(0)->get_image(offset, 1);
			SDL_Flip(screen);
		}
};

int main(int argc, char** argv) {
	srand ( time(NULL) );
	game666 GAME = game666();
	GAME.run();
	return 0;
}
