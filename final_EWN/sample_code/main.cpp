#include <stdio.h>
#include <string.h>
#include "MyAI.h"

int main(){
  srand(time(NULL));
  char read[1024], write[1024], output[2048], *token;
  const char *data[20];
  bool isFailed;
  MyAI myai;
  do{
    // read command
    fgets(read, 1024, stdin);
    fprintf(stderr, "%s", read);
    // remove newline(\n)
    read[strlen(read) - 1] = '\0';
    // get command name
    token = strtok(read, " ");
    // get command data
    int i = 0;
    while(token != NULL){
      data[i++] = token;
      token = strtok(NULL, " ");
    }
    // empty the char array
    write[0] = '\0'; 

    if (!strcmp(data[0], "name"))
    {
        myai.Name(data, write);
    }
    else if (!strcmp(data[0], "version"))
    {
        myai.Version(data, write);
    }
    else if (!strcmp(data[0], "time_setting"))
    {
        myai.Time_setting(data, write);
    }
    else if (!strcmp(data[0], "board_setting"))
    {
        myai.Board_setting(data, write);
    }
    else if (!strcmp(data[0], "ini"))
    {
        myai.Ini(data, write);
    }
    else if (!strcmp(data[0], "get"))
    {
        myai.Get(data, write);
    }
    else if (!strcmp(data[0], "exit"))
    {
        myai.Exit(data, write);
        break;
    }

    sprintf(output, "%s\n", write);

    fprintf(stdout, "%s", output);
    fprintf(stderr, "%s", output);
    // important, do not delete
    fflush(stdout);
    fflush(stderr);

  }while(true);

  return 0;
}
