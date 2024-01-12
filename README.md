
## COP290-IITD MAZE GAME

## BY USING SDL Libraries in C++ and socket programming

# Authors: 
1. Stitiprajna sahoo (2020CS10394)
2. Ananya Aakriti (2020CS10321)

## About the game

- The game is a maze game that is set on the map of IIT Delhi.
- It is a 2-player game.
- It is made by using different libraries of SDL in C++. 


## How to run

- Type the command make and run the makefile to run the game
- If you are the server, uncomment the server's line and vice cersa for client.


## Technicalities


- [SDL2] - Library used for graphics rendering in C++!
- [SDL2_image] - Library used for loading images and rendering them on the window. Used to render the map, buttons and sprites.
- [SDL2_ttf] - Library used for loading text and rendering them on the window. used to display scores and different components.
- [SDL2_mixer] - Library used for loading audio and rendering them on the window. Used to add audio effects.
- [sockets] - Using socket networking to play the game between two players over wifi connection.


## Installation

To install sdl2.0 on your ubuntu system:

```
sudo apt-get install libsdl2-2.0
```

To install sdl_image on your ubuntu system:

```
sudo apt update
sudo apt install libsdl2-image-dev
```

To install sdl_ttf on your ubuntu system:

```
sudo apt update
sudo apt install libsdl2-ttf-dev
```
To install sdl_mixer on your ubuntu system:

```
sudo apt update
sudo apt install libsdl2-mixer-dev
```
## Reference and Links:

The map has been made by the following website:  https://deepnight.net/tools/rpg-map/

The sprites have been made by pixel art:  https://www.pixilart.com/draw
     
These are reference links used to learn the basics of game programming, and how to use sockets and sdl. 

1. https://lazyfoo.net/tutorials/SDL/index.php (SDL)
2. https://galdin.dev/blog/getting-started-with-sdl/ (SDL)
3. https://www.geeksforgeeks.org/socket-programming-cc/?ref=lbp (SOCKETS)
4. https://www.geeksforgeeks.org/tcp-server-client-implementation-in-c/?ref=lbp (NETWORKING)

 