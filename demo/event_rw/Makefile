
bin=demo_rw client

all:demo_rw client

demo_rw: demo_rw.c
	gcc -g -o demo_rw demo_rw.c -levent
client:client.c
	gcc -g -o client client.c

clean:
	rm -rf *.o *.bak *.sw? demo_rw client
