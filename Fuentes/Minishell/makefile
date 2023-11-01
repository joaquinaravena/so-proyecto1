all: minishell.o comms/showcontent.o comms/mkdir.o comms/rmdir.o comms/touch.o  comms/chmod.o  comms/clear.o comms/ls.o comms/help.o
	gcc minishell.o -o minishell
	gcc comms/showcontent.o -o comms/showcontent
	gcc comms/touch.o -o comms/touch
	gcc comms/mkdir.o -o comms/mkdir
	gcc comms/rmdir.o -o comms/rmdir
	gcc comms/chmod.o -o comms/chmod
	gcc comms/clear.o -o comms/clear
	gcc comms/ls.o -o comms/ls
	gcc comms/help.o -o comms/help
	gcc -c minishell.c -o minishell.o
	./minishell
	rm minishell.o minishell comms/*.o comms/showcontent comms/chmod comms/touch comms/mkdir comms/rmdir comms/clear comms/ls  comms/help
minishell.o: minishell.c
	gcc -c minishell.c -o minishell.o
clean:
	rm minishell.o minishell comms/*.o comms/showcontent comms/chmod comms/touch comms/mkdir comms/rmdir comms/clear comms/ls  comms/help
run:
	./minishell
	
	

	
