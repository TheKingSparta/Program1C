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
#include <stdio.h> // for printf
#include <stdlib.h> // for exit
#include <unistd.h>  //Includes fork() execlp()
#include <sys/types.h>
#include <sys/wait.h>
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

int main(int argc, char *argv[])
{
   extern char **environ;
   if(argc < 2) {
      cerr << "Too few args!";
      return(1);
   }
   //Provided code
   enum {RD, WR}; // pipe fd index RD=0, WR=1
   int n, fd[2], fd2[2];  //n is used to store the num chars in a received message, fd is the file descriptors
   pid_t pid;
   char buf[100];

   if ((pid = fork()) < 0) // If the child is not created
      perror("fork error");
   else if (pid == 0) { //If this is the child, aka wc -l

      //Create grandchild,
      if ((pid = fork()) < 0) // If the grandchild is not created
         perror("fork error");
      else if (pid == 0) { //If this is the grandchild, aka grep
         //Create pipe to share with greatgrandchild
         if( pipe(fd2) < 0 ) { // If the pipe fails to be created
            perror("pipe error");
         }
         //Create greatgrandchild,
         if ((pid = fork()) < 0) // If the greatgrandchild is not created
            perror("fork error");
         else if (pid == 0) { //If this is the greatgrandchild, aka ps - A
            //Write connection to pipe
            dup2(fd2[WR], WR);

            //Do wc -l
            //cerr << "ps -A reached\n";
            execlp("ps", "ps", "-A", (char *) NULL);
            cerr << "ps failed\n";
         }
         else {   //If this is the grandchild, aka grep arg
            //wait for greatgrandchild
            wait(NULL);

            //Close pipe fd we don't need anymore
            close(fd2[WR]);

            //Read from pipe
            dup2(fd2[RD], RD);

            //Write to the pipe
            dup2(fd[WR], WR);

            //Do grep argv[1]
            //cerr << "grep reached\n";
            //cerr << "argv[1]: " + (string) argv[1];
            //cerr << "\n";
            //cerr << "cin: ";
            //std::string cont;
            //cin >> cont;
            //cerr << cont;
            execlp("grep", "grep", argv[1], (char *) NULL);
            cerr << "grep failed\n";
         }
      }
      else {   //If this is the child, aka wcl
         //wait for grandchild
         //cerr << "wc -l reached, waiting\n";
         wait(NULL);
         //cerr << "wait completed\n";

         //Close pipe fd we don't need anymore
         close(fd[WR]);
         close(fd2[RD]);

         //Read from pipe
         dup2(fd[RD], RD);

         //cerr << "dup completed\n";

         //Do wc -l
         //cerr << "wc -l reached\n";
         execlp("wc", "wc", "-l", (char *) NULL);
         cerr << "wc failed\n";
      }
   }
   else {   //If this is the parent
      //Wait for child
      wait(NULL);

      //Close pipe fd we don't need anymore
      close(fd[RD]);
   }
   return 0;
}
