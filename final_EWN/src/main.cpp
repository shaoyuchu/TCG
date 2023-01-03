#include <cstring>
#include <iostream>

#include "agent.hpp"
using namespace std;

bool executeCommand(const char* commandName, const char* data[], char* response,
                    Agent& agent) {
    if (!strcmp(commandName, "name")) {
        agent.name(data, response);
    } else if (!strcmp(commandName, "version")) {
        agent.version(data, response);
    } else if (!strcmp(commandName, "time_setting")) {
        agent.timeSetting(data, response);
    } else if (!strcmp(commandName, "board_setting")) {
        agent.boardSetting(data, response);
    } else if (!strcmp(commandName, "ini")) {
        agent.init(data, response);
    } else if (!strcmp(commandName, "get")) {
        agent.get(data, response);
    } else if (!strcmp(commandName, "exit")) {
        agent.get(data, response);
        return true;
    }
    return false;
}

int main() {
    char read[1024], write[1024], *token;
    const char* data[20];
    Agent agent;

    while (true) {
        // read command, remove newline
        fgets(read, 1024, stdin);
        read[strlen(read) - 1] = '\0';

        // get command name
        token = strtok(read, " ");

        // get command data
        int i = 0;
        while (token != NULL) {
            data[i++] = token;
            token = strtok(NULL, " ");
        }

        // execute command
        write[0] = '\0';
        if (executeCommand(data[0], data, write, agent)) break;

        // write output to stdout
        fprintf(stdout, "%s\n", write);
        fflush(stdout);
    }

    return 0;
}