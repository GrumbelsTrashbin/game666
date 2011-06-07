#include <iostream>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_gfxPrimitives.h>
#include <string>
#include <math.h>
#include <list>

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
};

class render_engine {
	protected:
		rengine_params params;
	public:
		render_engine() {}
		render_engine(rengine_params params) { this->params=params; }
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
		void render(SDL_Surface* surface, SDL_Color color, vector offset) {
			for(int i=0; i<num_of_engines; i++) {
				this->engines[i]->render(surface, color, offset);
			}
		}
};
namespace render {
	class solid: public render_engine {
		public:
			solid(rengine_params params) { this->params=params; }
			void _render(SDL_Surface* surface, SDL_Color color, vector offset) {
			Uint32 u_color=SDL_MapRGB(surface->format, color.r, color.g, color.b);
			SDL_FillRect(surface, NULL, u_color);
		}
	};
	class points: public render_engine {
		public:
			points(rengine_params params) { this->params=params; }
			void _render(SDL_Surface* surface, SDL_Color color, vector offset) {
				for(int i=0;i < params.value1;i++) {
					printf("ok");
					circleColor(surface, randint(0,surface->w), randint(0,surface->h), params.value2, uint_color(surface, color));
				}
			}
	};
};

class tile_type {
	public:
		SDL_Color color;
		render_engine* engine;
		tile_type() {}
		tile_type(tile_factory* factory, SDL_Color color, render_engine* engine) {
			this->color=color;
			this->engine=engine;
			factory->register_tiletype(this);
		}
};

struct tile {
	public:
		tiletype type;
		tile_variable var;
};

class tile_factory {
	private:
		tiletype next_id;
		std::list<tile_type*> tiletypes;
	public:
		tiletype register_tiletype(tile_type* new_type) {
			tiletypes.push_front(new_type);
			return ++next_id;
		}
};

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
		SDL_Surface* texture;
		game_data() {};
		bool load(std::string path) {
			if((texture=load_image(path))==NULL) {return false;}
			return true;
		}
		bool unload() {
			//Unload all data
			SDL_FreeSurface(texture);
			return true;
		}
		bool unload(std::string resource) {/*Unload given resource.*/return false;}
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
			this->load_data("all");
			//Main loop
			while(running) {
				limit_fps(FRAMERATE);
				this->check_events();
				this->simulate();
				this->render();
				//sleep(limit_fps(FRAMERATE)*1.0);
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
		void check_events() {
			while(SDL_PollEvent(&event))
			{
				if(event.type == SDL_QUIT) {running=false;}
				else if(event.type == SDL_KEYDOWN)
				{

				}
			}
		}
		void load_data(std::string resource) {
			if(resource=="all")
			{
				this->data.load("data/background.png");
			}
		}
		void render() {
			//SDL_Rect rect;
			//rect.x = rect.y = rand() % 21;
			//SDL_BlitSurface(data.texture, NULL, screen, &rect);
			//a_tile=new tile();
			vector offset;
			//offset.x=offset.y=0;
			//SDL_BlitSurface(a_tile->get_image(1, offset), NULL, screen, &rect);
			//SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 255,0,0));
			render::points* a = new render::points(rengine_params(1.0f,20,2));
			a->render(screen, make_color(100,200,100), offset);
			SDL_Flip(screen);
		}
		void unload_data(std::string resource) {
			this->data.unload();
		}
		int quit() {
			this->unload_data("all");
			SDL_FreeSurface(screen);
			SDL_Quit();
			return 0;
		}
};

int main(int argc, char** argv) {
	srand ( time(NULL) );
	game666 GAME = game666();
	GAME.run();
	return 0;
}
