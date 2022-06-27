// init: The initial user-level program

#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/spinlock.h"
#include "kernel/sleeplock.h"
#include "kernel/fs.h"
#include "kernel/file.h"
#include "user/user.h"
#include "kernel/fcntl.h"
#include "kernel/md5.h"

char *argv[] = { "sh", 0 };

int
main(void)
{
  int pid, wpid;

  if(open("console", O_RDWR) < 0){
    mknod("console", CONSOLE, 0);
    open("console", O_RDWR);
  }
  dup(0);  // stdout
  dup(0);  // stderr
  
  // username
  char username[100];
  printf("username: ");
  gets(username, 100);
  
  if (strlen(username) > 0 && username[strlen(username)-1] =='\n') {
    username[strlen(username)-1] = 0;
  }
  
  char processed_username[110];
  strcpy(processed_username, "passwords-");
  
  int i;
  for (i = 0; i < strlen(username); i++) {
    processed_username[10+i] = username[i];
  }
  
  int fd;
  fd = open("passwords-default", O_RDONLY);
  if (fd <= 0) {
    fd = open("passwords-default", O_CREATE | O_WRONLY);
    char default_hashed[100];
    char raw_pass[100];
    strcpy(raw_pass, "default");
    getmd5(raw_pass, 64, default_hashed);
    write(fd, default_hashed, 100);
    close(fd);
  }
  
  fd = open(processed_username, O_RDONLY);
  if (fd > 0) {
    char static_password[100];
    read(fd, static_password, 100);
    
    char password[100];
    printf("password: ");
    gets(password, 100);
    
    if (strlen(password) > 0 && password[strlen(password)-1] =='\n') {
      password[strlen(password)-1] = 0;
    }
    
    char hashed[100];
    getmd5(password, 64, hashed);
    
    if (strcmp(hashed, static_password) != 0) {
      printf("wrong password\n");
      exit(0);
    }
  } else {
    printf("user %s doesn't exist\n", username);
    exit(0);
  }
  close(fd);
  
  printf("you are logged in as %s\n", username);

  for(;;){
    printf("init: starting sh\n");
    pid = fork();
    if(pid < 0){
      printf("init: fork failed\n");
      exit(1);
    }
    if(pid == 0){
      exec("sh", argv);
      printf("init: exec sh failed\n");
      exit(1);
    }

    for(;;){
      // this call to wait() returns if the shell exits,
      // or if a parentless process exits.
      wpid = wait((int *) 0);
      if(wpid == pid){
        // the shell exited; restart it.
        break;
      } else if(wpid < 0){
        printf("init: wait returned an error\n");
        exit(1);
      } else {
        // it was a parentless process; do nothing.
      }
    }
  }
}
