http-client: http-client.c
	gcc -I/Users/diorahman/Experiments/misc/libuv/src/libuv-1.10.0/include http-client.c -L/Users/diorahman/Experiments/misc/libuv/src/libuv-1.10.0/out/Release -luv -o http-client

http-client-dns: http-client-dns.c
	gcc -I/Users/diorahman/Experiments/misc/libuv/src/libuv-1.10.0/include http-client-dns.c -L/Users/diorahman/Experiments/misc/libuv/src/libuv-1.10.0/out/Release -luv -o http-client-dns

all: http-client http-client-dns

clean:
	rm http-client http-client-dns
