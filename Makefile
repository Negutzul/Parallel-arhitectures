
build:
	gcc game_of_life.c -o game_of_life -lpthread
	gcc game_of_lifeOLD.c -o game_serial
	time ./game_of_life input.txt > paralel
	time ./game_serial input.txt > serial
clean:
	rm game_of_life