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

    char *token = strtok(str, "|");
    str1=token;
    printf("Token remaining is: %s",str);
    token=strtok(str,"|");
    str2=token;

    if(str2==NULL){

        ////No Pipe is present. Perform execution using one child
    }
    else{
        ////Pipe is present. Execute the first command and output of it, feed it to the input of second command
        
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
        char *commands[10];
        int command_count=0;
        char *token = strtok(input, ";");

        while( token != NULL ) {
            commands[command_count++]=token;
            token = strtok(NULL, ";");
        }

        printf("command count: %d \n", command_count);

        ////// Individual Command printer
        // int print_index=0;
        // printf("Commands are: \n");
        // while(print_index<command_index){
        //     printf("%s \n",commands[print_index++]);
        // }

        for(int i=0; i<command_count;i++){
            if(executeCommand(commands[i])==0)
            {
                printf("​ Illegal command or arguments");
                break;
            }
        }


    }

    return 0;
}