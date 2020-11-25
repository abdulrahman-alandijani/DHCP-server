server: server.c 
	gcc -w server.c -o server -lm
	chmod +x server

client: client.c
	gcc -w client.c -o client -lm
	chmod +x client

clean:
	rm server client