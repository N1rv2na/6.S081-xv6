#include "kernel/types.h"
#include "user/user.h"

void pipeline(int lpipe[2])
{
  int prime, n;
  // read first number from left neighbor, it is a prime
  if (read(lpipe[0], &prime, sizeof(int)) == sizeof(int))
  {
    fprintf(1, "prime %d\n", prime);
  }

  int rpipe[2];
  pipe(rpipe);
  int pid = fork();
  if (pid == 0)
  { 
    // child process, it reads the undropped numbers, it will be the next stage of pipeline
    close(rpipe[1]);
    close(lpipe[0]);
    pipeline(rpipe);
  } else
  {
    // parent process, filter primes, it will drop numbers that can be devided by the prime
    close(rpipe[0]);
    while (read(lpipe[0], &n, sizeof(int)) > 0)
    {
      if (n % prime) 
      {
        write(rpipe[1], &n, sizeof(int));
      }
    }
    close(lpipe[0]);
    close(rpipe[1]);
    wait(0);
  }
}

int
main(int argc, char *argv[])
{
  int p[2];
  pipe(p);

  int pid = fork();
  if (pid == 0)
  {
    close(p[1]);
    pipeline(p);
  } else 
  {
    // main process, generate numbers and write to pipe
    close(p[0]);
    for (int i = 2; i < 36; ++i) 
    {
      write(p[1], &i, sizeof(int));
    }
    close(p[1]);
    wait(0);
  }
  
  exit(0);
}