#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include<signal.h> 

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
void fileRemoveSpacesAndCreateTokens(char **filewithSpace, char **finalFile)
{

    if(filewithSpace[0]!=NULL)
    {
        finalFile[0] = strtok(filewithSpace[0], " ");
        while (strcmp(finalFile[0], " ") == 0)
        {
            finalFile[0] = strtok(NULL, " ");
        }
    }

    if(filewithSpace[1]!=NULL)
    {
        finalFile[1] = strtok(filewithSpace[1], " ");
        while (strcmp(finalFile[1], " ") == 0)
        {
            finalFile[1] = strtok(NULL, " ");
        }
    }
    
    //finalFile[0]="input test";
    //finalFile[1]="output test";
    //printf("fileRemoveSpacesAndCreateTokens: filewithSpace[0]:%s    filewithSpace[1]:%s\n",filewithSpace[0],filewithSpace[1]);
    
}

void tokenizeCommandsAndArguments(char *icommand, char **str1Tokens, char **tempfile, char **file)
{
    removeSpacesAndCreateTokens(icommand, str1Tokens);
    fileRemoveSpacesAndCreateTokens(tempfile, file);
}

///IO Parser for without Pipe  ---- 1 argument passed
void checkIOParser(char *str1, char **file, char **str1Tokens)
{

    char *icommand = NULL, *tempfile[2]={NULL,NULL};
    char *in_checker=NULL,*out_checker=NULL;


    in_checker=strrchr(str1, '<');
    out_checker=strrchr(str1, '>');

    if(out_checker==NULL && in_checker==NULL)
    {
        icommand=str1;
        printf("icommand: %s\n",icommand);
    }
    else if(in_checker!=NULL && out_checker==NULL)
    {
        icommand=strtok(str1,"<");
        tempfile[0]=strtok(NULL,"<");
        printf("icommand: %s\n tempfile[0]: %s\n",icommand,tempfile[0]);
    }
    else if(out_checker!=NULL && in_checker==NULL)
    {
        icommand=strtok(str1,">");
        tempfile[0]=strtok(NULL,">");
        printf("icommand: %s\n tempfile[0]: %s\n",icommand,tempfile[0]);

    }else
    {
        icommand=str1;

        int len=strlen(out_checker)-strlen(in_checker);
        printf("length : %d\n",len);
        
        if(len>0){
            
            icommand=strtok(str1,">");
            char *temp=strtok(NULL,">");
            tempfile[1]=strtok(temp,"<");
            tempfile[0]=strtok(NULL,"<");
            printf("icommand: %s\n temp: %s\n tempfile[0]: %s\n tempfile[1]: %s\n",icommand,temp,tempfile[0],tempfile[1]);

        }
        else
        {
            icommand=strtok(str1,"<");
            char *temp=strtok(NULL,"<");
            tempfile[0]=strtok(temp,">");
            tempfile[1]=strtok(NULL,">");
            printf("icommand: %s\n temp: %s\n tempfile[0]: %s\n tempfile[1]: %s\n",icommand,temp,tempfile[0],tempfile[1]);


        }
    }

    tokenizeCommandsAndArguments(icommand, str1Tokens, tempfile, file);
    //printf("executeCommands: file[0]:%s    file[1]:%s\n",file[0],file[1]);

}

//Execute individual commands after parsing it with ';' seperator
void executeCommand(char *str)
{

    //Check if pipe '|' is present in the command str and split accordingly into two seperate strings to be processed in pipe functionality
    char *str1 = NULL, *str2 = NULL;
    char *str1Tokens[200], *str2Tokens[200];

    int customFlag=0; //to differntiate btw custom Listfiles ls and builtin ls...similarly for residentmemory and ps

    str1 = strtok(str, "|");

    if (str1 != NULL)
        str2 = strtok(NULL, "|");

    if (str2 == NULL)
    {
        char *file[2]={NULL,NULL};
        //No Pipe is present. Perform execution using one child
        checkIOParser(str1, file, str1Tokens);

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
        
        printf("Tokens are printed below:\n");
        int i = 0;
        while (str1Tokens[i] != NULL)
        {
            printf("%s\n", str1Tokens[i]);
            i++;
        }

        printf("main: file[0]:%s    file[1]:%s\n",file[0],file[1]);
        

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
            if (pid == -1) {
                perror("fork failed");
                exit(1);
            }


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
                        printf("​Child: Illegal command or arguments\n");
                        exit(0);
                    }
                }

                if(strcmp(str1Tokens[0], "sort") == 0 && commandCheckerFlag==2){

                    int fd=open(str1Tokens[1],O_RDONLY);
                    dup2(fd,0);
                    str1Tokens[1]=NULL;
                    if (execvp(str1Tokens[0], str1Tokens) < 0)
                    {
                        printf("​Child: Illegal command or arguments\n");
                        exit(0);
                    }

                }

                //This is for checkcpupercentage and checkresidentmemory
                if (execvp(str1Tokens[0], str1Tokens) < 0)
                {
                    printf("​Child: Illegal command or arguments\n");
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
    }else
    {
        printf("Pipe Found \n string1 : %s \n string2 : %s\n", str1, str2);
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
        if (pid == -1) {
            perror("fork failed");
            exit(1);
        }

        if (pid == 0)
        {

            //printf("main- command 1 executed with fork \n");
            executeCommand(command1);
            exit(0);
        }
        else
        {
            pid_t pid1 = fork();
            if (pid1 == -1) {
               perror("fork failed");
               exit(1);
            }

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

// Signal Handler for SIGINT
void sigint_handler(int sig){

    printf("​\nthe program is interrupted, do you want to exit [Y/N]");
    fflush(stdout);
    fflush(stdin);
    char *buffer=calloc(100,sizeof(char));
    read(0,buffer,100);

    //printf("​ the pid is: %d and  buffer string is : %s", getpid(),first_command);

    if(strcmp(buffer,"Y\n")==0){
        kill(getpid(),SIGKILL);
    }else{
        fflush(stdout);
        printf("myShell> ");
        fflush(stdout);
    }
}

// Signal Handler for SIGTERM
void sigterm_handler(int sig){
    printf("​\nGot SIGTERM-Leaving\n");
    kill(getpid(),SIGKILL);
}


int main()
{

    signal(SIGINT, sigint_handler); 
    signal(SIGTERM, sigterm_handler); 
    
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
                tokenizeCommandsAndArguments(input, str1Tokens, NULL, NULL);
                
                // Characters to read
                int buffer_size=5000;

                char *buffer=calloc(buffer_size,sizeof(char));
                int fd=open(str1Tokens[1],O_RDONLY);
                if(fd<0){
                    printf("​main: Illegal command or arguments​ \n");
                }else{

                    int charCount=read(fd,buffer,buffer_size);
                        close(fd);
                    //printf("buffer content is: %s \n", buffer);
                    char *single_command;
                    for (int i = 0; i < buffer_size; i++)
                    {
                        single_command = strsep(&buffer, "\n");

                        if (single_command == NULL || strcmp(single_command,"")==0)
                            break;

                        if(commaChecker(single_command)==0){
                            executeCommand(single_command);
                        }
                    }

                }

            }
            else if(commaChecker(input)==0){
                executeCommand(input);
            }
           
        }
        else
        {
            printf("​Illegal command or arguments​ \n");
        }
    }

    return 0;
}