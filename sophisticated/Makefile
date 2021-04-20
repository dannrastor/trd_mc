test : ./src/*.cpp ./include/*.h test.cpp
	g++ -o test test.cpp ./src/*.cpp -I./include -std=c++1z

main :  ./src/*.cpp ./include/*.h main.cpp
	g++ -o main main.cpp ./src/*.cpp -I./include -std=c++1z `root-config --libs --cflags`
