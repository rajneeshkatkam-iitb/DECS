
Steps to compile and execute:

    1) Open the terminal, enter "make" command and hit enter. 
        After compilation it should show the following output in the terminal:
            
            gcc -o threads threads.c -lpthread
            Compiled threads.c successfully
            gcc -o nlocks nlocks.c -lpthread
            Compiled nlocks.c successfully


    2) For running threads.c program use following command:
            ./threads <value>
    
    <value> - Enter the argument (number of threads) in integer



    3) For running nlocks.c program use following command:
            ./nlocks


References used:

https://www.cs.cmu.edu/afs/cs/academic/class/15492-f07/www/pthreads.html
https://computing.llnl.gov/tutorials/pthreads/