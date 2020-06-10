FinalProduct:test.exe

OBJS=gluethread/glthread.o			 \
                  graph.o                  	 \
                  topologies.o		 \
                  net.o
                  
test.exe:testapp.o ${OBJS}
	gcc -g testapp.o ${OBJS} -o test.exe

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

clean:
	rm *.o
	rm gluethread/glthread.o
	rm *exe





