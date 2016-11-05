http-client: http-client.c
	gcc -I/Users/diorahman/Experiments/misc/libuv/src/libuv-1.10.0/include http-client.c -L/Users/diorahman/Experiments/misc/libuv/src/libuv-1.10.0/out/Release -luv -o http-client
