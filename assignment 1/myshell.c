#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

static char *builtinCommands[3] = {"cd", "help", "exit"};
static char *customCommands[10] = {"checkcpupercentage", "checkresidentmemory", "listFiles", "sortFile"};
static int commantCount = 4;
static int builtinCount = 3;

// 0->Not builtin or custom. 1->builtin. 2->custom
int CommandsChecker(char *command)
{

    //builtin commands checker
    for (int i = 0; i < builtinCount; i++)
    {
        if (strcmp(command, builtinCommands[i]) == 0)
            return 1;
    }

    //custom commands checker
    for (int i = 0; i < commantCount; i++)
    {
        if (strcmp(command, customCommands[i]) == 0)
            return 2;
    }

    return 0;
}

//IO Parser for with Pipe ---- 2 arguments passed
void checkIOParserPipe(char *str1, char *str2)
{
}

//Remove spacing in commands
void removeSpacesAndCreateTokens(char *tokensWithSpaces, char **finalTokens)
{
    for (int i = 0; i < 200; i++)
    {
        finalTokens[i] = strsep(&tokensWithSpaces, " ");

        if (finalTokens[i] == NULL)
            break;
        if (strlen(finalTokens[i]) == 0)
            i--;
    }
}

//Remove spacing in filestream
void fileRemoveSpacesAndCreateTokens(char *filewithSpace, char *finalFile)
{

    finalFile = strsep(&filewithSpace, " ");
    while (finalFile == NULL || (strcmp(finalFile, "") == 0))
    {
        finalFile = strsep(&filewithSpace, " ");
    }
}

void tokenizeCommandsAndArguments(char *icommand, char **str1Tokens, char *tempfile, char *file, int direction)
{

    removeSpacesAndCreateTokens(icommand, str1Tokens);

    if (direction != 0)
    {
        fileRemoveSpacesAndCreateTokens(tempfile, file);
        //printf("file: %s \n tempfile:%s", file, tempfile);
    }
}

///IO Parser for without Pipe  ---- 1 argument passed
int checkIOParser(char *str1, char *file, char **str1Tokens)
{

    char *icommand = NULL, *tempfile = NULL;
    int direction = 0; /// 1->output file is present 2->input file is present  0-> no I/O direction is present

    if (strrchr(str1, '>') != NULL)
    {
        direction = 1;

        icommand = strtok(str1, ">");
        if (icommand != NULL)
            tempfile = strtok(NULL, ">");
    }
    else if (strrchr(str1, '<') != NULL)
    {
        direction = 2;

        icommand = strtok(str1, "<");
        if (icommand != NULL)
            tempfile = strtok(NULL, "<");
    }
    else
    {
        icommand = str1;
    }

    tokenizeCommandsAndArguments(icommand, str1Tokens, tempfile, file, direction);

    //if (direction != 0)
    //    printf("checkIOParser- icommand is: %s  \t file is: %s \n", icommand, file);

    return direction;
}

//Execute individual commands after parsing it with ';' seperator
void executeCommand(char *str)
{

    //Check if pipe '|' is present in the command str and split accordingly into two seperate strings to be processed in pipe functionality
    char *str1 = NULL, *str2 = NULL;
    char *str1Tokens[200], *str2Tokens[200];
    char *file = NULL;

    int customFlag=0; //to differntiate btw custom Listfiles ls and builtin ls...similarly for residentmemory and ps

    str1 = strtok(str, "|");

    if (str1 != NULL)
        str2 = strtok(NULL, "|");

    if (str2 == NULL)
    {

        //No Pipe is present. Perform execution using one child
        //printf("executeCommand- Without pipe str1 is: %s \n", str1);
        int direction = checkIOParser(str1, file, str1Tokens);

        //printf("executeCommand- direction is: %d \n", direction);

        // //check str1Tokens[0] with custom commands. If not present then execvp...if it throws errors, then you through Illegal commands error
        int commandCheckerFlag = CommandsChecker(str1Tokens[0]);

        if (strcmp(str1Tokens[0], "checkresidentmemory") == 0)
        {
            //printf("checkresidentmemory code entered \n");
            str1Tokens[0] = "ps";
            str1Tokens[3] = str1Tokens[1];
            str1Tokens[1] = "-o";
            str1Tokens[2] = "rss=";
            str1Tokens[4] = NULL;
        }
        else if (strcmp(str1Tokens[0], "listFiles") == 0)
        {
            //printf("listFiles code entered \n");
            str1Tokens[0] = "ls";
            str1Tokens[1] = NULL;
        }
        else if (strcmp(str1Tokens[0], "sortFile") == 0)
        {
            //printf("sortFile code entered \n");
            str1Tokens[0] = "sort";
            str1Tokens[2] = NULL;
        }

        ///printing tokens
        /*
        int i = 0;
        while (str1Tokens[i] != NULL)
        {
            printf("%s\n", str1Tokens[i]);
            i++;
        }
         */

        if (commandCheckerFlag == 1)
        {
            // runBuiltIn(str1Tokens, file, direction);
            printf("run builtin command \n");
        }
        else
        {
            //printf("run custom command or executable command \n");

            if (commandCheckerFlag==2 && strcmp(str1Tokens[0], "checkcpupercentage") == 0)
            {
                // Concatinating ./ to the first argument to make it executable custom command
                char a[100] = "./";
                strcat(a, str1Tokens[0]);
                str1Tokens[0] = a;
            }

            pid_t pid = fork();

            if (pid == 0)
            {
                //child process

                //This is for listFiles command
                if (strcmp(str1Tokens[0], "ls") == 0 && commandCheckerFlag==2)
                {
                    int fd = open("files.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);
                    dup2(fd, 1);
                    if (execvp(str1Tokens[0], str1Tokens) < 0)
                    {
                        printf("​Child: Illegal command or arguments \n");
                        exit(0);
                    }
                }

                if(strcmp(str1Tokens[0], "sort") == 0 && commandCheckerFlag==2){

                    int fd=open(str1Tokens[1],O_RDONLY);
                    dup2(fd,0);
                    str1Tokens[1]=NULL;
                    if (execvp(str1Tokens[0], str1Tokens) < 0)
                    {
                        printf("​Child: Illegal command or arguments \n");
                        exit(0);
                    }

                }

                //This is for checkcpupercentage and checkresidentmemory
                if (execvp(str1Tokens[0], str1Tokens) < 0)
                {
                    printf("​Child: Illegal command or arguments \n");
                    exit(0);
                }
            }
            else
            {
                //parent process
                wait(NULL);
            }
            //runCustomCommands(str1Tokens, file, direction);
        }
    }
}

//Custom Command Pipe Execution
int commaChecker(char *input)
{

    // Command Seperator function i.e. when it detects ";", then it seperates the command (tokens) and stores them in the commands pointer array
    char *command1 = NULL, *command2 = NULL;
    command1 = strtok(input, ";");

    if (command1 != NULL)
        command2 = strtok(NULL, ";");

    if (command2 != NULL)
    {

        //printf("\n main- command1 is: %s \n", command1);
        //printf("main- command2 is: %s \n", command2);
        pid_t pid = fork();
        if (pid == 0)
        {

            //printf("main- command 1 executed with fork \n");
            executeCommand(command1);
            exit(0);
        }
        else
        {
            pid_t pid1 = fork();
            if (pid1 == 0)
            {
                //printf("main- command 2 executed with fork \n");
                executeCommand(command2);
                exit(0);
            }
            else
            {
                wait(NULL);
                wait(NULL);
                return 1;
            }
        }
    }

    return 0;
}

int main()
{

    while (1)
    {

        //Input string lengths and extend length variable
        unsigned int extend_length = 64, max_input_length = 256, input_index = 0;

        // Malloc declaration for input command string
        char *input = (char *)malloc(sizeof(char) * max_input_length);

        //command prompt print
        fflush(stdout);
        printf("myShell> ");
        fflush(stdout);

        //Command Input loop
        fflush(stdin);
        char c = getchar();
        if (c == '\n')
            input[0] = '\n';
        while (c != '\n' && c != EOF)
        {
            input[input_index++] = c;

            //Buffer overflow check and extending the size of input using realloc
            if (input_index == max_input_length)
            {
                max_input_length += extend_length;
                input = realloc(input, sizeof(char) * max_input_length);
            }
            c = getchar();
        }

        //If input is null
        if (input != NULL && input[0] != '\n')
        {

            //checking first command;
            char input_copy[input_index];
            strcpy(input_copy, input);
            char *first_command = strtok(input_copy, " ");
            while (first_command != NULL && strcmp(first_command, " ") == 0)
            {
                first_command = strtok(input_copy, " ");
            }

            //printf("first command is:%s \n", first_command);

            //If input cmd is "exit", then exit the program without executing further
            if (strcmp(first_command, "exit") == 0)
                return 0;

            if(strcmp(first_command,"executeCommands")==0){
                char *str1Tokens[5];
                tokenizeCommandsAndArguments(input, str1Tokens, NULL, NULL, 0);

                

                ///printing tokens
                ///*
                int i = 0;
                while (str1Tokens[i] != NULL)
                {
                    printf("%s\n", str1Tokens[i]);
                    i++;
                }
                //*/


            }
            else if(commaChecker(input)==0){
                executeCommand(input);
            }
           
        }
        else
        {
            printf("​ Illegal command or arguments​ \n");
        }
    }

    return 0;
}