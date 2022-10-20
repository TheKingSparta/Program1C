/**
 * Written by Zachary Clow
 * Last updatedOctober 19, 2022
 *
 * Takes a single argument from the command line and generates output equivalent
 * to the Linux command string:
 * ps -A | grep argument | wc -l
 *
 * This command outputs the number of processes running that contain argument
 *
 * This is accomplished via pipes and multithreading
 */

#include <iostream>
//#include <stdio.h> // for printf
//#include <stdlib.h> // for exit
#include <unistd.h>  //Includes fork() execlp()
using namespace std;

/*
 * Useful functions:
 *
 * pid_t fork( void );
 * creates a child process that differs from the parent process only in terms
 * of their process IDs.
 *
 * int execlp( const char *file, const char *arg, ..., (char *)0 );
 * replaces the current process image with a new process image that will be
 * loaded from file. The first argument arg must be the same as file
 *
 * int pipe( int filedes[2] );
 * creates a pair of file descriptors (which point to a pipe structure), and
 * places them in the array pointed to by filedes. filedes[0] is for reading
 * data from the pipe, filedes[1] is for writing data to the pipe.
 *
 * int dup2( int oldfd, int newfd );
 * creates in newfd a copy of the file descriptor oldfd.
 * This system call redirects the flow of standard input and output to be
 * input and output into the pipe. Oldfd is the file descriptor that points
 * to the pipe, and newfd is the standard input and output fd that you want to
 * redirect to the pipe.  Read about dup2
 *
 * pid_t wait( int *status );
 * waits for process termination.
 *
 * int close( int fd );
 * closes a file descriptor.
 */

int main()
{
   //Provided code
   enum {RD, WR}; // pipe fd index RD=0, WR=1
   int n, fd[2];
   pid_t pid;
   char buf[100];
   if( pipe(fd) < 0 ) // 1: pipe created
      perror("pipe error");
   else if ((pid = fork()) < 0) // 2: child forked
      perror("fork error");
   else if (pid == 0) {
      close(fd[WR]);// 4: child's fd[1] closed
      n = read(fd[RD], buf, 100);
      write(STDOUT_FILENO, buf, n);
   }
   else {
      close(fd[RD]); // 3: parent's fd[0] closed
      write(fd[WR], "Hello my child\n", 15);
      wait(NULL);
   }
   return 0;
}
