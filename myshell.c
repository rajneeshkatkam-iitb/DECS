#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

//This is to execute individual commands after parsing it through ; seperated filter
//return 0 if the code fails due to syntatic error or execution error.
//return 1 if code succeds
int executeCommand(char *str){

    //Check if pipe '|' is present in the command str and split accordingly into two seperate strings to be processed in pipe functionality
    char *str1=NULL, *str2=NULL;

    str1= strtok(str, "|");
    
    if(str1!=NULL)
        str2=strtok(NULL,"|");


    if(str2==NULL){

        ////No Pipe is present. Perform execution using one child

        //checkIOParser(str1);
        printf("Without pipe str1 is: %s \n", str1);

    }
    else{
        ////Pipe is present. Execute the first command and output of it, feed it to the input of second command

        //checkIOParserPipe(str1, str2);
                printf("With Pipe str1 is: %s \n", str1);
                printf("With Pipe str2 is: %s \n", str2);

    }

    return 1;
}

int main(){

    while(1){

        //Input string lengths and extend length variable
        unsigned int extend_length=64, max_input_length=256, input_index=0;

        // Malloc declaration for input command string
        char *input=(char *) malloc(sizeof(char)*max_input_length);

        //command prompt print
        fflush(stdout);
        printf("myShell> ");
        fflush(stdout);

        //Command Input loop
        char c = getchar();
        while(c!='\n' && c!=EOF){
            input[input_index++]=c;
            
            //Buffer overflow check and extending the size of input using realloc
            if(input_index==max_input_length){
                max_input_length+=extend_length;
                input=realloc(input,sizeof(char)*max_input_length);
            }
            c=getchar();
        }

        //If input cmd is "exit", then exit the program without executing further
        if(!strcmp(input,"exit"))
            return 0;


        // Command Seperator function i.e. when it detects ";", then it seperates the command (tokens) and stores them in the commands pointer array 
        char *command1=NULL, *command2=NULL;
        command1 = strtok(input, ";");


        if(command1!=NULL)
            command2 = strtok(NULL, ";");


        if(command2==NULL)
        {
            
            printf("command1 is: %s \n", command1);
            printf("only command 1 executed without fork \n");
            executeCommand(command1);
        }
        else
        {
            printf("command1 is: %s \n", command1);
            printf("command2 is: %s \n", command2);
            pid_t pid=fork();
            if(pid==0){
                
                printf("command 1 executed with fork \n");
                executeCommand(command1);
                exit(0);
            }
            else{
                pid_t pid1=fork();
                if(pid1==0){
                    printf("command 2 executed with fork \n");
                    executeCommand(command2);
                    exit(0);
                }else{
                    wait(NULL);
                    wait(NULL);
                }
            }


        }


    }   

    return 0;
}