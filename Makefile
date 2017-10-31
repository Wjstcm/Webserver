OBJ = cgi.o server.o
CGI = cgi.cgi
SERVER = server
CC = gcc
#CROSS = arm-linux-
CGIOBJ = cgic.c cgi.c
SERVEROBJ = main.c server.c threadpool.c
PTHREAD = -lpthread 
CFLAGS = -g

exec : $(CGI) $(SERVER)
	mv $(CGI) ../exec/cgi-bin/
	mv $(SERVER) ../exec/
$(CGI):$(CGIOBJ) cgic.h
	$(CROSS)$(CC) -g -o $(CGI) $(CGIOBJ) $(PTHREAD)
$(SERVER):$(SERVEROBJ) server.h threadpool.h
	$(CROSS)$(CC) -g -o $(SERVER) $(SERVEROBJ) $(PTHREAD)

.PHONY:clean
clean:
	rm -vf ../exec/$(SERVER) ../exec/cgi-bin/$(CGI)
	rm -vf *.o