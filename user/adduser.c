#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/spinlock.h"
#include "kernel/sleeplock.h"
#include "kernel/fs.h"
#include "kernel/file.h"
#include "user/user.h"
#include "kernel/fcntl.h"
#include "kernel/md5.h"

int main(int argc, char *argv[])
{
  int i;
  int fd;
  char temp_username[110];
  strcpy(temp_username, "passwords-");
  for (i = 0; i < strlen(argv[1]); i++) {
    temp_username[10+i] = argv[1][i];
  }
  fd = open(temp_username, O_RDONLY);
  if (fd > 0) {
    printf("user %s already exists", argv[1]);
    close(fd);
    exit(0);
  } else {
    char processed_username[110];
    strcpy(processed_username, "passwords-");
    
    for (i = 0; i < strlen(argv[1]); i++) {
      processed_username[10+i] = argv[1][i];
    }
    fd = open(processed_username, O_CREATE | O_WRONLY);
    if (fd > 0) {
      char new_password[100];
      char new_password_confirm[100];
      printf("set password: ");
      gets(new_password, 100);
      printf("confirm password: ");
      gets(new_password_confirm, 100);
      
      if (strlen(new_password) > 0 && new_password[strlen(new_password)-1] =='\n') {
        new_password[strlen(new_password)-1] = 0;
      }
      if (strlen(new_password_confirm) > 0 && new_password_confirm[strlen(new_password_confirm)-1] =='\n') {
        new_password_confirm[strlen(new_password_confirm)-1] = 0;
      }
      
      if (strcmp(new_password, new_password_confirm) == 0) {
        
        char hashed[100];
        getmd5(new_password, 64, hashed);
        
        write(fd, hashed, strlen(hashed));
        
        char uid_name[110];
        strcpy(uid_name, "uid-");
        for (i = 0; i < strlen(argv[1]); i++) {
          uid_name[4+i] = argv[1][i];
        }
        fd = open(uid_name, O_CREATE | O_WRONLY);
        write(fd, uid_name, strlen(uid_name));
        close(fd);
        
        printf("successfully created user %s", argv[i]);
        close(0);
        exit(0);
      } else {
        printf("password confirmation failed");
        close(0);
        exit(0);
      }
    } else {
      printf("unexpected error");
      exit(0);
    }
  }
  exit(0);
}