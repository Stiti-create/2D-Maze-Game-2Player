all:
	g++ header.cpp main_client.cpp -lSDL2_image -lSDL2_ttf -lSDL2_mixer -g `sdl2-config --cflags --libs` -o client
	g++ header.cpp main_server.cpp -lSDL2_image -lSDL2_ttf -lSDL2_mixer -g `sdl2-config --cflags --libs` -o server


	# for server
	# ./server 

	# for client
	# ./client
		
clean:
	rm -f server client
	



