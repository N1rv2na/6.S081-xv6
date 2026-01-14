#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void
find(char *dir, char *filename)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(dir, 0)) < 0){
    fprintf(2, "find: cannot open %s\n", dir);
    return;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "find: cannot stat %s\n", dir);
    close(fd);
    return;
  }

  if(st.type != T_DIR){
    fprintf(2, "find: %s is not a directory", dir);
    close(fd);
    return;
  }  // dir must be a directory, find <directory> <filename>,

  if(strlen(dir) + 1 + DIRSIZ + 1 > sizeof buf){
      fprintf(2, "find: %s too long\n", dir);
  }

  // construct dir path
  strcpy(buf, dir);
  p = buf+strlen(buf);
  *p++ = '/';  // add / after dir, p point to next free space in buf

  // work through each dirent in dir
  while(read(fd, &de, sizeof(de)) == sizeof(de)){
    if(de.inum == 0)
      continue;
    
    memmove(p, de.name, DIRSIZ);  // now p is the file (or directory) name
    p[DIRSIZ] = 0;  // now buf is the file (or directory) path

    if(stat(buf, &st) < 0){  // call stat to get file state
        printf("find: cannot stat %s\n", buf);
        continue;
    }

    if(st.type == T_FILE && strcmp(p, filename) == 0){
      printf("%s\n", buf);
    }

    if(st.type == T_DIR && strcmp(p, ".") != 0 && strcmp(p, "..") != 0){
      find(buf, filename);
    }
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  if(argc < 3){
    fprintf(2, "Usage: find <directory> <filename>\n");
    exit(1);
  }
  find(argv[1], argv[2]);
  exit(0);
}