all:
	gcc -o myserver.o my_server.c -lpthread -lrt
	gcc -o client.o client.c -lpthread -lrt
runserver:
		  ./myserver.o	
runclient:
	   ./client.o	
clean:
	  rm *.o