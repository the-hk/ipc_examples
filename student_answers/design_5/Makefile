all:
	gcc -o myserver.o my_server.c -lpthread
	gcc -o client.o client.c -lpthread
runserver:
		  ./myserver.o	
runclient:
	   ./client.o	
clean:
	  rm *.o