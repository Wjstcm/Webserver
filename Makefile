OBJ = main.o hash.o
EXEC = hash
$(EXEC) : $(OBJ)
	gcc -o $(EXEC) $(OBJ)
main.o : main.c hash.h
	gcc -c main.c
hash.o : hash.c hash.h
	gcc -c hash.c
.PHONY:clean
clean:
	rm -vf $(OBJ) $(EXEC) 
	
