
Steps to compile and execute:

    1) Open the terminal, enter "make" command and hit enter. 
        After compilation it should show the following output in the terminal:
            
            cc -g -Wall -Wno-unused-value -o hare_tortoise hare_tortoise_runner.c -lpthread 

    2) Run the command as follow:
        ./hare_tortoise test-file=<filename>.txt
    
    <filename> - Enter the text file name


References used:

https://www.cs.cmu.edu/afs/cs/academic/class/15492-f07/www/pthreads.html
https://computing.llnl.gov/tutorials/pthreads/