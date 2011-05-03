all:	server client

CC=cc
CFLAGS= -pedantic -Wall

#For debuggin un-comment the next line
#DFLAGS=-DDEBUG_TRACE=1 -g

DBM_INC_PATH=/usr/include/mysql
DBM_LIB_PATH=/usr/lib/mysql
DBM_LIB_FILE=mysqlclient

.c.o:
	$(CC) $(CFLAGS) -I$(DBM_INC_PATH) $(DFLAGS) -c $<

app_ui.o: app_ui.c bank_data.h
db_sql.o: db_sql.c bank_data.h
clientif.o: clientif.c bank_data.h cliser.h
pipe_imp.o: pipe_imp.c bank_data.h cliser.h
server.o: server.c bank_data.h cliser.h

client: app_ui.o clientif.o pipe_imp.o
	$(CC) -o client $(DFLAGS) app_ui.o clientif.o pipe_imp.o

server: server.o db_sql.o pipe_imp.o
	$(CC) -o server $(DFLAGS) server.o db_sql.o pipe_imp.o \
	-L$(DBM_LIB_PATH) -l$(DBM_LIB_FILE)

clean:
	rm -f server client *.o
