all: minishell.o bin/cat.o bin/mkdir.o bin/rmdir.o bin/touch.o  bin/chmod.o  bin/clear.o bin/ls.o bin/help.o
	gcc minishell.o -o minishell
	gcc bin/cat.o -o bin/cat
	gcc bin/touch.o -o bin/touch
	gcc bin/mkdir.o -o bin/mkdir
	gcc bin/rmdir.o -o bin/rmdir
	gcc bin/chmod.o -o bin/chmod
	gcc bin/clear.o -o bin/clear
	gcc bin/ls.o -o bin/ls
	gcc bin/help.o -o bin/help
minishell.o: minishell.c
	gcc -c minishell.c -o minishell.o
clean:
	rm minishell.o minishell bin/*.o bin/cat bin/chmod bin/touch bin/mkdir bin/rmdir bin/clear bin/ls  bin/help
run:
	./minishell
	
	

	
