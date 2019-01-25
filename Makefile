CRACKER = guessword.c
CRACKER_EXEC = guessword.out

default:
	gcc $(CRACKER) -o $(CRACKER_EXEC) -lpthread
run:
	./$(CRACKER_EXEC)
runall:
	gcc $(CRACKER) -o $(CRACKER_EXEC) -lpthread
	./$(CRACKER_EXEC)
clean:
	rm $(CRACKER_EXEC)
