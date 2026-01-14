#include "kernel/types.h"
#include "user/user.h"
#include "kernel/param.h"

int
main(int argc, char *argv[])
{
  if(argc < 2){
    fprintf(2, "Usage: xargs <command>\n");
    exit(1);
  }

  char *args[MAXARG];  // argv array for exec
  for(int i = 1; i < argc; ++i){
    args[i - 1] = argv[i];
  }  // copy command and params of xargs

  char c;
  char buf[512];  // used to save a line
  char *p = buf;

  // read from stdin
  while(read(0, &c, sizeof(char)) == sizeof(char)){
    if(c != '\n'){
      *p++=c;  // save charactor into buffer
    } else {  // met \n, means the line ends
      *p = 0;  // add string ender

      if(fork() == 0){  // every time reading a line finished, fork a child process to exec
        args[argc - 1] = buf;
        args[argc] = 0;  // add args ender
        exec(args[0], args);
        fprintf(2, "xargs: exec failed! \n");  // if exec returns, then exec failed
        exit(1);
      } else {
        wait(0);
        p = buf;  // reset the buffer point
      }
    }
  }
  exit(0);
}