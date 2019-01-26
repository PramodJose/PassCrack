UNSHADOW = unshadow.c
UNSHADOW_EXEC = unshadow.out
GWORD_SRC = guessword.c
GWORD_EXEC = guessword.out

PASSWD_FILE = training-passwd.txt
SHADOW_FILE = training-shadow.txt
DICTIONARY = top250.txt
MERGED_FILE = passwordfile.txt
FINAL_OUT = allcrackedpasswords.txt
SPECIAL_DICT = rockyou.txt


default:
	gcc $(UNSHADOW) -o $(UNSHADOW_EXEC)
	gcc $(GWORD_SRC) -o $(GWORD_EXEC) -lpthread -lcrypt

run: $(PASSWD_FILE) $(SHADOW_FILE) $(DICTIONARY)
	./$(UNSHADOW_EXEC) -p $(PASSWD_FILE) -s $(SHADOW_FILE) -o $(MERGED_FILE)
	./$(GWORD_EXEC) -m c -d $(DICTIONARY) -i $(MERGED_FILE) -o $(FINAL_OUT)

special: $(PASSWD_FILE) $(SHADOW_FILE) $(SPECIAL_DICT)
	./$(UNSHADOW_EXEC) -p $(PASSWD_FILE) -s $(SHADOW_FILE) -o $(MERGED_FILE)
	./$(GWORD_EXEC) -m s -d $(SPECIAL_DICT) -i $(MERGED_FILE) -o $(FINAL_OUT)

clean:
	rm $(UNSHADOW_EXEC)
	rm $(GWORD_EXEC)

cleanall:
	rm $(UNSHADOW_EXEC)
	rm $(GWORD_EXEC)
	rm $(MERGED_FILE)
	rm $(FINAL_OUT)
