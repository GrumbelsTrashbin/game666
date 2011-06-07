env = Environment()
env.ParseConfig("sdl-config --libs --cflags")
env.Append(LIBS = ["SDL_image","SDL_gfx"] )
env.Append(CXXFLAGS =[ "-std=c++0x",
                       "-Wall",
                       "-ansi",
                       "-pedantic" ])
env.Program("game666", "game666.cpp")

# EOF #
