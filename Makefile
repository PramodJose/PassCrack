CRACKER = guessword.c
CRACKER_EXEC = guessword.out

default:
	gcc $(CRACKER) -o $(CRACKER_EXEC) -lpthread
	./$(CRACKER_EXEC)

run:
	./$(CRACKER_EXEC)

runall:
	gcc $(CRACKER) -o $(CRACKER_EXEC) -lpthread
	./$(CRACKER_EXEC)

special:
	gcc $(CRACKER) -o $(CRACKER_EXEC) -lpthread
	./$(CRACKER_EXEC) -m s -d rockyou.txt

clean:
	rm $(CRACKER_EXEC)
