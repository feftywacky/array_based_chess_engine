# array_based_chess_engine
chess engine in c++ using SFML GUI <br>
uses 2d array data structure <br>
uses negamax for search, alpha-beta pruning for optimization <br>
uses piece-square tables for evaluation <br>

to run: <br>
install SFML-2.5.1<br>
> g++ -Ofast -c main.cpp ai.cpp -IC:\SFML-2.5.1\include <br>
> g++ main.o ai.o -o chess -LC:\SFML-2.5.1\lib -lsfml-graphics -lsfml-window -lsfml-system <br>
> ./chess <br>
