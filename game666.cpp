#include <iostream>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <string>
#include <math.h>

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

#define point	two_coords
#define size	two_coords
#define vector	two_coords

int randint(int min, int max){
	return rand() % (max-min+1)+min;
}

bool lock_fps(int framerate) { //Taken from Tenac on GameDev forums
	static float lastTime = 0.0f;
	float currentTime = SDL_GetTicks() * 0.001f;
	if((currentTime - lastTime) > (1.0f / framerate))
	{
		lastTime = currentTime;
		return true;
	}
	return false;
}

float limit_fps(int framerate) { //Taken from Tenac on GameDev forums
	static float lastTime = 0.0f;
	float delay=0.0f;
	float currentTime = SDL_GetTicks() * 0.001f;
	//printf("%f %f\n",currentTime-lastTime, 1.0f/framerate);
	if((currentTime - lastTime) > (1.0f / framerate)) { delay=0; }
	else { delay=((1.0f / framerate)-(currentTime - lastTime)); }
	lastTime = currentTime;
	//printf("%f\n", delay);
	sleep(delay);
	//printf("%f\n", delay);
	return delay;
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

class render_engine {
	private:
		float color_multiplier;
	public:
		render_engine()
		{
			this->color_multiplier=1.0f;
		}
		render_engine(float color_multiplier)
		{
			this->color_multiplier=color_multiplier;
		}
		void render(SDL_Surface& surface, SDL_Color color, vector offset)
		{
			_render(surface, multiply_color(color,color_multiplier), offset);
		}
	private:
		void _render(SDL_Surface& surface, SDL_Color color, vector offset) {}
};

class compound_render_engine: public render_engine {
	private:
		float color_multiplier;
		static render_engine* renderers[0];
	public:
		compound_render_engine() { color_multiplier=1.0f; init_subengines(); }
		compound_render_engine(float color_multiplier) { this->color_multiplier=color_multiplier; init_subengines(); }
	private:
		void init_subengines()
		{

		}
};

struct render {
	class solid: public render_engine {
		void _render(SDL_Surface* surface, SDL_Color color, vector offset)
		{
			Uint32 u_color=SDL_MapRGB(surface->format, color.r, color.g, color.b);
			SDL_FillRect(surface, NULL, u_color);
		}
	};
};

union tile_properties {
	Uint8 prop1;
	Uint8 prop2;
	Uint8 prop3;
};

//class tile_properties
//{
//	public:
//		union
//}

class tile {
	public:
		static int priority;
		static render_engine renderer;
		static SDL_Color color;
		static SDL_Surface* image[CACHE_IMAGES];
		static SDL_Surface* image_edge[CACHE_IMAGES_EDGES];
		static SDL_Surface* image_corner[CACHE_IMAGES_CORNERS];
		//static
		tile() {
			//color=make_color(128,128,128);
			//renderer=render_engine();
			//priority=0;
			size tile_size=size(TILE_WIDTH, TILE_HEIGHT);
			this->render(tile_size);
		}
		static void render(size tile_size) {
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
			for(int i=0; i<CACHE_IMAGES;i++)
			{
				image[i]=SDL_CreateRGBSurface(SDL_SWSURFACE, tile_size.x, tile_size.y, 32, rmask, gmask, bmask, amask); //new SDL_Surface(size.x,size.y);
				renderer.render(*image[i], color, *offset);
			}
		}
		SDL_Surface* get_image(int variation, vector offset) {
			if(offset.x||offset.y) { return image[variation % (CACHE_IMAGES)]; } //center
			else if(offset.x&offset.y) { return image_corner[variation % (CACHE_IMAGES_CORNERS)]; } //corners
			else { return image_edge[variation % (CACHE_IMAGES_EDGES)]; } //edges
		}
		void simulate(tile* neighbour[3][3]) {}
};

SDL_Surface* tile::image[CACHE_IMAGES];
SDL_Surface* tile::image_edge[CACHE_IMAGES_EDGES];
SDL_Surface* tile::image_corner[CACHE_IMAGES_CORNERS];
SDL_Color tile::color=make_color(128,128,128);
render_engine tile::renderer=render_engine();
int tile::priority=0;


class concrete: public tile
{

};

class chunk {
	private:
		tile terrain_tile [CHUNK_WIDTH][CHUNK_HEIGHT];
};

class gameData {
	public:
		SDL_Surface* texture;
		gameData() {};
		int load(std::string path)
		{
			if((texture=load_image(path))==NULL) {return -1;}
			return 0;
		}
		int unload()
		{
			//Unload all data
			SDL_FreeSurface(texture);
			return 0;
		}
		int unload(std::string resource) {/*Unload given resource.*/return 0;}
};

class gameWorld {
	private:
	public:
		void simulate()
		{

		}
};

class game666 {
	private:
		bool		running;
		SDL_Event	event;
		SDL_Surface*	screen;
		gameData	data;
		gameWorld*	world;
		tile*		a_tile;
	public:
		game666() {}
		int run() {
			screen = NULL;
			this->initialize(point(160,120));
			this->load_data("all");
			//Main loop
			while(running)
			{
				this->check_events();
				this->simulate();
				this->render();
				limit_fps(FRAMERATE);
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
			SDL_Rect rect;
			rect.x = rect.y = rand() % 21;
			SDL_BlitSurface(data.texture, NULL, screen, &rect);
			a_tile=new tile();
			//vector offset;
			//offset.x=offset.y=0;
			//SDL_BlitSurface(a_tile->get_image(1, offset), NULL, screen, &rect);
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
