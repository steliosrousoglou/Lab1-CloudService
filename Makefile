default: server.c 
	gcc -g3 -o server mongoose.c hashtable.c server.c
