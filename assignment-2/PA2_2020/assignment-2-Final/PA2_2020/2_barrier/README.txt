
Steps to compile and execute:

    1) Open the terminal, enter "make" command and hit enter. 
        After compilation it should show the following output (example) in the terminal:
            
            cc -g -Wall -Wno-unused-value -o barrier1 barrier-testcase1.c barrier.c -lpthread
            cc -g -Wall -Wno-unused-value -o barrier2 barrier-testcase2.c barrier.c -lpthread
            cc -g -Wall -Wno-unused-value -o barrier3 barrier-testcase3.c barrier.c -lpthread
            cc -g -Wall -Wno-unused-value -o barrier4 barrier-testcase4.c barrier.c -lpthread
            cc -g -Wall -Wno-unused-value -o barrier5 barrier-testcase5.c barrier.c -lpthread 


    2) For running barrier program use following command:
            ./barrier<value>
    
    <value> - Enter the number (specified in the makefile for creating output for specific testcase).



References used:

https://www.cs.cmu.edu/afs/cs/academic/class/15492-f07/www/pthreads.html
https://computing.llnl.gov/tutorials/pthreads/