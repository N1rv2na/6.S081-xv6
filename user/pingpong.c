#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int p1[2];  // pipe1: child to parent, 0 for read and 1 for write
  int p2[2];  // pipe2: parent to child
  pipe(p1);
  pipe(p2);

  char buf = 'A';  // send byte and buffer

  int pid = fork();
  if (pid < 0) {
    fprintf(2, "pingpong: fork() failed!");
    close(p1[0]);
    close(p1[1]);
    close(p2[0]);
    close(p2[1]);
    exit(1);
  } else if (pid == 0) {  // child process
    close(p1[0]);  // close unused pipe1 read port
    close(p2[1]);  // close unused pipe2 write port

    read(p2[0], &buf, 1);  // read from parent through pipe2's read port
    fprintf(1, "%d: received ping\n", getpid());
    write(p1[1], &buf, 1);  // write to parent through pipe1's write port
    
    // clear
    close(p2[0]);
    close(p1[1]);
    exit(0);
  } else {  // parent process
    close(p1[1]);  // close unused pipe1 write port
    close(p2[0]);  // close unused pipe1 read port

    write(p2[1], &buf, 1);  // write to child through pipe2's write port
    read(p1[0], &buf, 1);  // read from child through pipe1's read port
    fprintf(1, "%d: received pong\n", getpid());
    
    // clear
    close(p1[0]);
    close(p2[1]);
    wait(0);
  }

  exit(0);
}
