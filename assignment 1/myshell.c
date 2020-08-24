#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>


static char *builtinCommands[3]={"cd","help","exit"};
static char *customCommands[10]={"","","","",""};

//IO Parser for with Pipe ---- 2 arguments passed 
void checkIOParserPipe(char *str1,char *str2){



}


void removeSpacesAndCreateTokens(char *tokensWithSpaces, char **finalTokens){

}

void fileRemoveSpacesAndCreateTokens(char *filewithSpace, char *finalFile){

}

void tokenizeCommandsAndArguments(char *icommand, char **str1Tokens, char *tempfile, char *file, int direction){


    removeSpacesAndCreateTokens(icommand,str1Tokens);
    
    if(direction!=0)
        fileRemoveSpacesAndCreateTokens(tempfile, file);

}



///IO Parser for without Pipe  ---- 1 argument passed
int checkIOParser(char *str1, char *file,char **str1Tokens){

    char *icommand=NULL, *tempfile=NULL;
    int direction=0;  /// 1->output file is present 2->input file is present  0-> no I/O direction is present


    if(strrchr(str1,'>')!=NULL)
    {
        direction=1;

        icommand= strtok(str1, ">");
        if(icommand!=NULL)
            tempfile=strtok(NULL,">");

        
    }
    else if(strrchr(str1,'<')!=NULL)
    {
        direction=2;

        icommand= strtok(str1, "<");
        if(icommand!=NULL)
            tempfile=strtok(NULL,"<");

    }

    if(direction!=0)
        printf("checkIOParser- icommand is: %s  \t file is: %s \n", icommand,file);
    // tokenizeCommandsAndArguments(icommand, str1Tokens, tempfile, file, direction);

    return direction;
}


// 0->Not builtin or custom. 1->builtin. 2->custom
int CommandsChecker(char *command){

    //builtin commands checker
    for(int i=0;i<3;i++){
        if(command==builtinCommands[i])
            return 1;
    }

    //custom commands checker
    for(int i=0;i<10;i++){
        if(command==customCommands[i])
            return 2;
    }

    return 0;
}



/////Execute individual commands after parsing it with ';' seperator
void executeCommand(char *str){

    //Check if pipe '|' is present in the command str and split accordingly into two seperate strings to be processed in pipe functionality
    char *str1=NULL, *str2=NULL;
    char *str1Tokens[200], *str2Tokens[200];
    char *file=NULL;



    str1= strtok(str, "|");
    
    if(str1!=NULL)
        str2=strtok(NULL,"|");


    if(str2==NULL){

        ////No Pipe is present. Perform execution using one child
        printf("executeCommand- Without pipe str1 is: %s \n", str1);
        int direction = checkIOParser(str1, file,str1Tokens);

        printf("executeCommand- direction is: %d \n", direction);

        // //check str1Tokens[0] with custom commands. If not present then execvp...if it throws errors, then you through Illegal commands error
        // int commandCheckerFlag=CommandsChecker(str1Tokens[0]);


        // if(commandCheckerFlag==1)
        // {
        //     // runBuiltIn(str1Tokens, file, direction);
        // }
        // else if(commandCheckerFlag==2)
        // {
        //     // runCustomCommands(str1Tokens, file, direction);
        // }
        // else{
        //     // runExecutable(str1Tokens, file, direction);
        // }





    }
    else{
        ////Pipe is present. Execute the first command and output of it, feed it to the input of second command
        
        printf("executeCommand- With Pipe str1 is: %s \n", str1);
        printf("executeCommand- With Pipe str2 is: %s \n", str2);
        //checkIOParserPipe(str1, str2);

    }

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
        fflush(stdin);
        char c = getchar();
        if(c=='\n')
            input[0]='\n';
        while(c!='\n' && c!=EOF){
            input[input_index++]=c;
            
            //Buffer overflow check and extending the size of input using realloc
            if(input_index==max_input_length){
                max_input_length+=extend_length;
                input=realloc(input,sizeof(char)*max_input_length);
            }
            c=getchar();
        }

        //If input is null
        if(input!=NULL && input[0]!='\n'){


            //If input cmd is "exit", then exit the program without executing further
            if(strstr(input,"exit"))
                return 0;



            // Command Seperator function i.e. when it detects ";", then it seperates the command (tokens) and stores them in the commands pointer array 
            char *command1=NULL, *command2=NULL;
            command1 = strtok(input, ";");


            if(command1!=NULL)
                command2 = strtok(NULL, ";");


            if(command2==NULL)
            {
                
                printf("\n main- command1 is: %s \n", command1);
                printf("main- only command 1 executed without fork \n");
                executeCommand(command1);
            }
            else
            {
                printf("\n main- command1 is: %s \n", command1);
                printf("main- command2 is: %s \n", command2);
                pid_t pid=fork();
                if(pid==0){
                    
                    printf("main- command 1 executed with fork \n");
                    executeCommand(command1);
                    exit(0);
                }
                else{
                    pid_t pid1=fork();
                    if(pid1==0){
                        printf("main- command 2 executed with fork \n");
                        executeCommand(command2);
                        exit(0);
                    }else{
                        wait(NULL);
                        wait(NULL);
                    }
                }


            }

        }
        else
        {
            printf("​ Illegal command or arguments​ \n");
        }
        
    }   

    return 0;
}