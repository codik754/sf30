all:
	g++ -g -o main *.cpp -lpthread -fsanitize=thread
