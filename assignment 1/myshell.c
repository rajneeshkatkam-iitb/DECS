#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>

static char *builtinCommands[2] = {"cd", "exit"};
static char *customCommands[10] = {"checkcpupercentage", "checkresidentmemory", "listFiles", "sortFile"};
static int commantCount = 4;
static int builtinCount = 2;

// 0->Not builtin or custom. 1->builtin. 2->custom
int CommandsChecker(char *command)
{
    if(command!=NULL)
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
    }

    return 0;
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
    if (filewithSpace == NULL)
        return;

    if (filewithSpace[0] != NULL)
    {
        finalFile[0] = strtok(filewithSpace[0], " ");
        while (strcmp(finalFile[0], " ") == 0)
        {
            finalFile[0] = strtok(NULL, " ");
        }
    }

    if (filewithSpace[1] != NULL)
    {
        finalFile[1] = strtok(filewithSpace[1], " ");
        while (strcmp(finalFile[1], " ") == 0)
        {
            finalFile[1] = strtok(NULL, " ");
        }
    }
}

void tokenizeCommandsAndArguments(char *icommand, char **str1Tokens, char **tempfile, char **file)
{
    removeSpacesAndCreateTokens(icommand, str1Tokens);
    fileRemoveSpacesAndCreateTokens(tempfile, file);
}

///IO Parser for without Pipe  ---- 1 argument passed
void checkIOParser(char *str1, char **file, char **str1Tokens)
{

    if(str1!=NULL)
    {
        char *icommand = NULL, *tempfile[2] = {NULL, NULL};
        char *in_checker = NULL, *out_checker = NULL;

        in_checker = strrchr(str1, '<');
        out_checker = strrchr(str1, '>');

        if (out_checker == NULL && in_checker == NULL)
        {
            icommand = str1;
            //printf("icommand: %s\n",icommand);
        }
        else if (in_checker != NULL && out_checker == NULL)
        {
            icommand = strtok(str1, "<");
            tempfile[0] = strtok(NULL, "<");
            //printf("icommand: %s\n tempfile[0]: %s\n",icommand,tempfile[0]);
        }
        else if (out_checker != NULL && in_checker == NULL)
        {
            icommand = strtok(str1, ">");
            tempfile[1] = strtok(NULL, ">");
            //printf("icommand: %s\n tempfile[1]: %s\n",icommand,tempfile[1]);
        }
        else
        {
            int len = strlen(out_checker) - strlen(in_checker);
            //printf("length : %d\n",len);

            if (len > 0)
            {

                icommand = strtok(str1, ">");
                char *temp = strtok(NULL, ">");
                tempfile[1] = strtok(temp, "<");
                tempfile[0] = strtok(NULL, "<");
                //printf("icommand: %s\n temp: %s\n tempfile[0]: %s\n tempfile[1]: %s\n",icommand,temp,tempfile[0],tempfile[1]);
            }
            else
            {
                icommand = strtok(str1, "<");
                char *temp = strtok(NULL, "<");
                tempfile[0] = strtok(temp, ">");
                tempfile[1] = strtok(NULL, ">");
                //printf("icommand: %s\n temp: %s\n tempfile[0]: %s\n tempfile[1]: %s\n",icommand,temp,tempfile[0],tempfile[1]);
            }
        }

        //printf("io direction parsing completed \n");
        tokenizeCommandsAndArguments(icommand, str1Tokens, tempfile, file);
        //printf("executeCommands: file[0]:%s    file[1]:%s\n",file[0],file[1]);

    }
}

void checkCustomCommands(char **str1Tokens)
{
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
}


void doubleQuotesRemovalFromTokens(char **strToken){
    for (int i = 0; i < 200; i++)
    {
        if(strToken[i]==NULL)
            return;

        if(strToken[i][0]=='"')
        {
            //printf("Before %d: %s \t",i,strToken[i]);
            strToken[i]=&strToken[i][1];
            int j=1;
            while(strToken[i][j]!='"')
            {
                if(strToken[i][j]=='\0')
                    break;

                j++;
            }
            if(strToken[i][j]=='"')
                strToken[i][j]='\0';
           //printf("After %d: %s \n",i,strToken[i]);
        }
    }

}

void fileDoubleQuotesRemovalFromTokens(char **file){

    if(file[0]!=NULL)
    {
        if(file[0][0]=='"')
        {
            //printf("File[0] Before: %s \t",file[0]);
            file[0]=&file[0][1];
            int j=1;
            while(file[0][j]!='"')
            {
                if(file[0][j]=='\0')
                    break;

                j++;
            }
            if(file[0][j]=='"')
                file[0][j]='\0';
            //printf("File[0] After: %s \n",file[0]);
        }
    }

    if(file[1]!=NULL)
    {
        if(file[1][0]=='"')
        {
            //printf("File[1] Before: %s \t",file[1]);
            file[1]=&file[1][1];
            int j=1;
            while(file[1][j]!='"')
            {
                if(file[1][j]=='\0')
                    break;

                j++;
            }
            if(file[1][j]=='"')
                file[1][j]='\0';
            //printf("File[1] After: %s \n",file[1]);
        }
    }

}

//Execute individual commands after parsing it with ';' seperator
void executeCommand(char *str)
{

    //Check if pipe '|' is present in the command str and split accordingly into two seperate strings to be processed in pipe functionality
    char *str1 = NULL, *str2 = NULL;
    char *str1Tokens[200], *str2Tokens[200];

    str1 = strtok(str, "|");

    if (str1 != NULL)
    {
        str2 = strtok(NULL, "|");
        // if(strcmp(str2," ")|| strcmp(str2,"")==0 || strcmp(str2,"\n")==0)
        // {
        //     printf("​executeCommand: Illegal command or arguments\n");
        //     return;
        // }
            
    }    

    // If no pipe/ grep is present
    if (str2 == NULL)
    {
        char *file[2] = {NULL, NULL};
        //No Pipe is present. Perform execution using one child
        checkIOParser(str1, file, str1Tokens);

        if(str1Tokens[0]==NULL)
        {
            printf("Illegal command or arguments\n");
            return;
        }

        doubleQuotesRemovalFromTokens(str1Tokens);
        fileDoubleQuotesRemovalFromTokens(file);

        //check str1Tokens[0] with custom commands. If not present then execvp...if it throws errors, then you through Illegal commands error
        int commandCheckerFlag = CommandsChecker(str1Tokens[0]);

        checkCustomCommands(str1Tokens);

        /*
        if(file[0]!=NULL)
            printf("file[0]: %s\t",file[0]);

        if(file[1]!=NULL)
            printf("file[1]: %s\n",file[1]);
        ///printing tokens
        
        printf("Tokens are printed below:\n");
        int i = 0;
        while (str1Tokens[i] != NULL)
        {
            printf("%s\n", str1Tokens[i]);
            i++;
        }
        */
        //printf("main: file[0]:%s    file[1]:%s\n",file[0],file[1]);

        if (commandCheckerFlag == 1)
        {
            // runBuiltIn(str1Tokens, file, direction);
            //printf("run builtin command \n");

            //This is for checkcpupercentage and checkresidentmemory
            if (strcmp(str1Tokens[0],"cd")==0)
            {
                if (chdir(str1Tokens[1]) != 0){
                    printf("Illegal command or arguments\n");
                }
            }
            else if (strcmp(str1Tokens[0],"exit")==0)
            {
                exit(0);
            }
            else{
                printf("Illegal command or arguments\n");
            }
            // char *args[]={"cd","..",NULL};
            // chdir("..");
        }
        else
        {

            pid_t pid = fork();
            if (pid == -1)
            {
                perror("fork failed");
                exit(1);
            }

            if (pid == 0)
            {
                //child process
                
                int fd_input, fd_output;
                //Checking if I/O are present and change input and output file descriptors accordingly.
                //Input filestream
                if (file[0] != NULL)
                {
                    fd_input = open(file[0], O_RDONLY, 0777);
                    fflush(stdin);
                    if (fd_input > 0)
                    {
                        if(dup2(fd_input, 0)==-1){
                            printf("error in duplication");
                        }
                        //printf("Input stream dupped with integer: %d\n",fd_input);
                        close(fd_input);

                        if(commandCheckerFlag!=2)
                        {
                            int i = 0;
                            while (str1Tokens[i] != NULL)
                                i++;
                            
                            str1Tokens[i]=file[0];
                            str1Tokens[i+1]=NULL;
                        }
                    }

                }

                //Output filestream
                if (file[1] != NULL)
                {
                    fd_output = open(file[1], O_WRONLY | O_CREAT | O_TRUNC, 0666);
                    fflush(stdout);
                    if (fd_output > 0)
                    {
                        dup2(fd_output, 1);
                        //printf("Output stream dupped with integer: %d\n",fd_output);
                    }
                    close(fd_output);
                }

                //printf("run custom command or executable command \n");
                if (commandCheckerFlag == 2 && strcmp(str1Tokens[0], "checkcpupercentage") == 0)
                {
                    // Concatinating ./ to the first argument to make it executable custom command
                    char a[100] = "./";
                    strcat(a, str1Tokens[0]);
                    str1Tokens[0] = a;

                    char io_input[100];    
                    if(file[0]!=NULL){
                        scanf("%s", io_input);

                        int i = 0;
                        while (str1Tokens[i] != NULL)
                            i++;
                        str1Tokens[i]=io_input;
                        str1Tokens[i+1]=NULL;
                    }
                }

                //This is for listFiles command
                if (strcmp(str1Tokens[0], "ls") == 0 && commandCheckerFlag == 2)
                {
                    int fd;
                    if(file[1]==NULL)
                    {
                        //printf("file[1] is null. \n");
                        fd = open("files.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);
                        dup2(fd, 1);
                        close(fd);
                    }
                    if (execvp(str1Tokens[0], str1Tokens) < 0)
                    { 
                        printf("​Illegal command or arguments\n");
                        exit(1);
                    }
                }

                if (strcmp(str1Tokens[0], "sort") == 0 && commandCheckerFlag == 2)
                {
                    int fd;
                    if(file[0]!=NULL){
                        fd = open(file[0], O_RDONLY);
                    }
                    else{
                        fd = open(str1Tokens[1], O_RDONLY);
                    }
                     
                    dup2(fd, 0);
                    close(fd);
                    
                    str1Tokens[1] = NULL;
                    if (execvp(str1Tokens[0], str1Tokens) < 0)
                    {
                        printf("​Illegal command or arguments\n");
                        exit(1);
                    }
                }

                

                if (strcmp(str1Tokens[0], "ps") == 0 && commandCheckerFlag == 2 && file[0]!=NULL){
                    //printf("Entered ps for file[0]\n");
                    int fd = open(file[0], O_RDONLY);
                    dup2(fd,0);
                    close(fd);
                    
                    char *io_input=(char *)calloc(100,sizeof(char));
                    scanf("%s", io_input);
                    str1Tokens[3]=io_input;
                    //printf("io_input is: %s\n",io_input);

                }


                //This is for checkcpupercentage and checkresidentmemory
                if (execvp(str1Tokens[0], str1Tokens) < 0)
                {
                    printf("​Illegal command or arguments\n");
                    exit(1);
                }
            }
            else
            {
                //parent process
                wait(NULL);
            }
        }
    }
    else
    {
        //printf("Pipe Found \n string1 : %s \n string2 : %s\n", str1, str2);

        char *file1[2] = {NULL, NULL};
        char *file2[2] = {NULL, NULL};
        //  Pipe is present. Perform execution using two child
        checkIOParser(str1, file1, str1Tokens);
        checkIOParser(str2, file2, str2Tokens);
        
        if(str1Tokens[0]==NULL || str2Tokens[0]==NULL)
        {
            printf("Illegal command or arguments\n");
            return;
        }

        doubleQuotesRemovalFromTokens(str1Tokens);
        fileDoubleQuotesRemovalFromTokens(file1);
        doubleQuotesRemovalFromTokens(str2Tokens);
        fileDoubleQuotesRemovalFromTokens(file2);


        //check str1Tokens[0] with custom commands. If not present then execvp...if it throws errors, then you through Illegal commands error
        int commandCheckerFlag1 = CommandsChecker(str1Tokens[0]);
        int commandCheckerFlag2 = CommandsChecker(str2Tokens[0]);

        checkCustomCommands(str1Tokens);
        checkCustomCommands(str2Tokens);


/*
        printf("Tokens 1 are printed below:\n");
        int i = 0;
        while (str1Tokens[i] != NULL)
        {
            printf("%s\n", str1Tokens[i]);
            i++;
        }

        printf("\n");

        printf("Tokens 2 are printed below:\n");
        i = 0;
        while (str2Tokens[i] != NULL)
        {
            printf("%s\n", str2Tokens[i]);
            i++;
        }

        printf("\n");
*/

        if (commandCheckerFlag1 == 1)
        {
            // runBuiltIn(str1Tokens, file, direction);
            if (strcmp(str1Tokens[0],"cd")==0)
            {
                if (chdir(str1Tokens[1]) != 0){
                    printf("Illegal command or arguments\n");
                }
            }
            else if (strcmp(str1Tokens[0],"exit")==0)
            {
                exit(0);
            }
            else{
                printf("Illegal command or arguments\n");
            }
            // char *args[]={"cd","..",NULL};
            // chdir("..");
        }
        else
        {

            int fdpipes[2];

            pipe(fdpipes);

            pid_t pid = fork();
            if (pid == -1)
            {
                perror("fork failed");
                exit(1);
            }

            if (pid == 0)
            {
                if (file1[1] == NULL)
                {
                    dup2(fdpipes[1], 1);
                    //printf("pipes fdpipes[1] done..\n");
                }

                int fd_input, fd_output;
                //Checking if I/O are present and change input and output file descriptors accordingly.
                //Input filestream
                if (file1[0] != NULL)
                {
                    fd_input = open(file1[0], O_RDONLY, 0777);
                    fflush(stdin);
                    if (fd_input > 0)
                    {
                        if(dup2(fd_input, 0)==-1){
                            printf("error in duplication");
                        }
                        //printf("Input stream dupped with integer: %d\n",fd_input);
                        if(fd_input>=0)
                            close(fd_input);

                        if(commandCheckerFlag1!=2)
                        {
                            int i = 0;
                            while (str1Tokens[i] != NULL)
                                i++;
                            
                            str1Tokens[i]=file1[0];
                            str1Tokens[i+1]=NULL;
                        }
                    }
                }

                //Output filestream
                if (file1[1] != NULL)
                {
                    fd_output = open(file1[1], O_WRONLY | O_CREAT | O_TRUNC, 0666);
                    fflush(stdout);
                    if (fd_output > 0)
                    {
                        dup2(fd_output, 1);
                        close(fd_output);
                        //printf("Output stream dupped with integer: %d\n",fd_output);
                    }
                }


                //printf("run custom command or executable command \n");
                if (commandCheckerFlag1 == 2 && strcmp(str1Tokens[0], "checkcpupercentage") == 0)
                {
                    // Concatinating ./ to the first argument to make it executable custom command
                    char a[100] = "./";
                    strcat(a, str1Tokens[0]);
                    str1Tokens[0] = a;

                    char io_input[100];    
                    if(file1[0]!=NULL){
                        scanf("%s", io_input);

                        int i = 0;
                        while (str1Tokens[i] != NULL)
                            i++;
                        str1Tokens[i]=io_input;
                        str1Tokens[i+1]=NULL;
                    }
                }

                //This is for listFiles command
                if (strcmp(str1Tokens[0], "ls") == 0 && commandCheckerFlag1 == 2)
                {   int fd;
                    if(file1[1]==NULL)
                    {
                         fd = open("files.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);
                        dup2(fd, 1);
                        close(fd);
                    }
                    if (execvp(str1Tokens[0], str1Tokens) < 0)
                    {
                        printf("Illegal command or arguments\n");
                        exit(1);
                    }
                }

                if (strcmp(str1Tokens[0], "sort") == 0 && commandCheckerFlag1 == 2)
                {
                    int fd;
                    if(file1[0]!=NULL){
                         fd = open(file1[0], O_RDONLY);
                    }
                    else{
                        fd = open(str1Tokens[1], O_RDONLY);
                    }  
                    dup2(fd, 0);
                    close(fd);
                    
                    str1Tokens[1] = NULL;
                    if (execvp(str1Tokens[0], str1Tokens) < 0)
                    {
                        printf("Illegal command or arguments\n");
                        exit(1);
                    }
                }

                if (strcmp(str1Tokens[0], "ps") == 0 && commandCheckerFlag1 == 2 && file1[0]!=NULL){
                    int fd = open(file1[0], O_RDONLY);
                    dup2(fd,0);
                    char io_input[100];
                    scanf("%s", io_input);
                    close(fd);
                    str1Tokens[3]=io_input;
                }

                //This is for checkcpupercentage and checkresidentmemory
                if (execvp(str1Tokens[0], str1Tokens) < 0)
                {
                    printf("Illegal command or arguments\n");
                    exit(1);
                }
            }
            else
            {
                wait(NULL);
                close(fdpipes[1]);


                // char std_output[100];
                // read(fdpipes[0],std_output,100);
                // printf("Pipes output is: %s \n",std_output);
                
                
               
                
                if (file1[1] == NULL)
                {


                    pid_t pid_pipe = fork();
                    if (pid_pipe == -1)
                    {
                        perror("fork failed");
                        exit(1);
                    }

                    if (pid_pipe == 0)
                    {

                        if (file2[0] == NULL)
                        {
                            dup2(fdpipes[0], 0);
                            close(fdpipes[0]);

                            /* if(commandCheckerFlag2!=2)
                                {
                                    char *a=(char *)calloc(3000,sizeof(char));
                                    read(0,a,3000);
                                    int i = 0;
                                    while (str2Tokens[i] != NULL)
                                        i++;
                                    
                                    str2Tokens[i]=a;
                                    str2Tokens[i+1]=NULL;
                                } */
                            //printf("pipes fdpipes[0] done..\n");
                        }


                        int fd_input, fd_output;
                        //Checking if I/O are present and change input and output file descriptors accordingly.
                        //Input filestream
                        if (file2[0] != NULL)
                        {
                            fd_input = open(file2[0], O_RDONLY), 0777;
                            fflush(stdin);
                            if (fd_input > 0)
                            {
                                if(dup2(fd_input, 0)==-1){
                                    printf("error in duplication");
                                }
                                //printf("Input stream dupped with integer: %d\n",fd_input);
                                close(fd_input);
                                if(commandCheckerFlag2!=2)
                                {
                                    int i = 0;
                                    while (str2Tokens[i] != NULL)
                                        i++;
                                    
                                    str2Tokens[i]=file2[0];
                                    str2Tokens[i+1]=NULL;
                                }
                                
                            }
                        }

                        //Output filestream
                        if (file2[1] != NULL)
                        {
                            fd_output = open(file2[1], O_WRONLY | O_CREAT | O_TRUNC, 0666);
                            fflush(stdout);
                            if (fd_output > 0)
                            {
                                dup2(fd_output, 1);
                                close(fd_output);
                                //printf("Pipe closed \n");
                                //printf("Output stream dupped with integer: %d\n",fd_output);
                            }
                        }


                        //printf("run custom command or executable command \n");
                        if (commandCheckerFlag2 == 2 && strcmp(str2Tokens[0], "checkcpupercentage") == 0)
                        {
                            // Concatinating ./ to the first argument to make it executable custom command
                            char a[100] = "./";
                            strcat(a, str2Tokens[0]);
                            str2Tokens[0] = a;

                            char io_input[100];    
                            scanf("%s", io_input);

                            int i = 0;
                            while (str1Tokens[i] != NULL)
                                i++;
                            str1Tokens[i]=io_input;
                            str1Tokens[i+1]=NULL;
                            
                        }


                        //This is for listFiles command
                        if (strcmp(str2Tokens[0], "ls") == 0 && commandCheckerFlag2 == 2)
                        {
                            int fd;
                            if(file2[1]==NULL)
                            {
                                fd = open("files.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);
                                dup2(fd, 1);
                                close(fd);
                            }
                            if (execvp(str2Tokens[0], str2Tokens) < 0)
                            {
                                printf("​Illegal command or arguments\n");
                                exit(1);
                            }
                        }



                        if (strcmp(str2Tokens[0], "sort") == 0 && commandCheckerFlag2 == 2)
                        {
                            int fd;
                            if(file2[0]!=NULL){
                                fd = open(file2[0], O_RDONLY);
                                dup2(fd, 0);
                                close(fd);
                            }
                            char *io_input=(char *)calloc(3000,sizeof(char));
                            read(0,io_input,3000);
                            str2Tokens[1] = io_input;
                            if (execvp(str2Tokens[0], str2Tokens) < 0)
                            {
                                printf("Illegal command or arguments\n");
                                exit(1);
                            }
                        }

                        if (strcmp(str2Tokens[0], "ps") == 0 && commandCheckerFlag2 == 2){
                            //printf("Entered ps for 2nd child\n");
                            int fd;
                            if(file2[0]!=NULL)
                            {
                                fd = open(file2[0], O_RDONLY);
                                dup2(fd,0);
                                close(fd);
                            }
                            char *io_input=(char *)calloc(100,sizeof(char));
                            scanf("%s", io_input);
                            close(fd);
                            str2Tokens[3]=io_input;
                            //printf("io_input is: %s\n",io_input);
                        }

                        /*
                        printf("Tokens are printed below:\n");
                        int i = 0;
                        while (str2Tokens[i] != NULL)
                        {
                            printf("%s\n", str2Tokens[i]);
                            i++;
                        }
                        */
                            
                        //char *args[]={"grep","tommy",NULL};
                        //This is for checkcpupercentage and checkresidentmemory
                        if (execvp(str2Tokens[0], str2Tokens) < 0)
                        {
                            printf("Illegal command or arguments\n");
                            exit(1);
                        }


                    }else
                    {
                        wait(NULL);
                        close(fdpipes[0]);
                    }
                    
                }  
            }
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
        if (pid == -1)
        {
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
            if (pid1 == -1)
            {
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
void sigint_handler(int sig)
{

    printf("​\nthe program is interrupted, do you want to exit [Y/N] ");
    fflush(stdout);
    fflush(stdin);
    char *buffer = (char *)calloc(100, sizeof(char));
    read(0, buffer, 100);

    //printf("​ the pid is: %d and  buffer string is : %s", getpid(),first_command);

    if (strcmp(buffer, "Y\n") == 0)
    {
        //kill(getpid(), SIGKILL);
        exit(0);
    }
    else
    {
        fflush(stdout);
        printf("myShell> ");
        fflush(stdout);
    }
}

// Signal Handler for SIGTERM
void sigterm_handler(int sig)
{
    printf("​\nGot SIGTERM-Leaving\n");
    //kill(getpid(), SIGKILL);
    exit(0);
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

            if(first_command!=NULL)
            {

                //printf("first command is:%s \n", first_command);

                //If input cmd is "exit", then exit the program without executing further
                if (strcmp(first_command, "exit") == 0 || strcmp(first_command, "\"exit\"") == 0)
                    return 0;

                if (strcmp(first_command, "executeCommands") == 0)
                {
                    char *str1Tokens[5];
                    tokenizeCommandsAndArguments(input, str1Tokens, NULL, NULL);

                    // Characters to read
                    int buffer_size = 5000;

                    char *buffer = (char *)calloc(buffer_size, sizeof(char));
                    int fd = open(str1Tokens[1], O_RDONLY);
                    if (fd < 0)
                    {
                        printf("​Illegal command or arguments​ \n");
                    }
                    else
                    {

                        int charCount = read(fd, buffer, buffer_size);
                        close(fd);
                        //printf("buffer content is: %s \n", buffer);
                        char *single_command;
                        for (int i = 0; i < buffer_size; i++)
                        {
                            single_command = strsep(&buffer, "\n");

                            if (single_command == NULL || strcmp(single_command, "") == 0)
                                break;

                            if (commaChecker(single_command) == 0)
                            {
                                executeCommand(single_command);
                            }
                        }
                    }
                }
                else if (commaChecker(input) == 0)
                {
                    executeCommand(input);
                }


            }
            else
            {
                printf("​Illegal command or arguments​ \n");
            }
          

        }
        else
        {
            printf("​Illegal command or arguments​ \n");
        }
    }

    return 0;
}