del game.exe
g++ -c src/main.cpp -Iinclude/ -std=c++11
g++ main.o -o game -Llib -lsfml-graphics -lsfml-window -lsfml-system
del main.o
pause