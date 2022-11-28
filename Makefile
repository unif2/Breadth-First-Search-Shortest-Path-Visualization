all: compile link

compile: 
	g++ -c main.cpp Board.cpp -IC:\SFML-2.5.1-windows-gcc-7.3.0-mingw-64-bit\SFML-2.5.1\include -DSFML_STATIC

link:
	g++ main.o Board.o -o main -LC:\SFML-2.5.1-windows-gcc-7.3.0-mingw-64-bit\SFML-2.5.1\lib -lsfml-graphics-s -lsfml-window-s -lsfml-system-s -lopengl32 -lfreetype -lwinmm -lgdi32 -mwindows -lsfml-main

clean:
	del main.exe *.o
