FinalProduct:test.exe

LIBS=-lpthread -L ./CommandParser -lcli
OBJS=gluethread/glthread.o			 \
                  graph.o                  	 \
                  topologies.o		 \
                  net.o			 \
                  networkcli.o		 \
                  communication.o
                  
test.exe:testapp.o ${OBJS} CommandParser/libcli.a
	gcc -g testapp.o ${OBJS} -o test.exe ${LIBS}

testapp.o:testapp.c
	gcc -g -c testapp.c -o testapp.o

gluethread/glthread.o:gluethread/glthread.c
	gcc -g -c -I gluethread gluethread/glthread.c -o gluethread/glthread.o
graph.o:graph.c
	gcc -g -c -I . graph.c -o graph.o
topologies.o:topologies.c
	gcc -g -c -I . topologies.c -o topologies.o
net.o:net.c
	gcc -g -c -I . net.c -o net.o
networkcli.o:networkcli.c
	gcc -g -c -I . networkcli.c -o networkcli.o
communication.o:communication.c
	gcc -g -c -I . communication.c -o communication.o
CommandParser/libcli.a:
	(cd CommandParser; make)

clean:
	rm *.o
	rm gluethread/glthread.o
	rm *exe
	(cd CommandParser; make clean)
all:
	make
	(cd CommandParser; make)





