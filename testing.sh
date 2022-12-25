#!/bin/sh
# This is a script to automate testing of the binary in this directory
# First we loop constantly and execute the binary 1000 times
# And aftear each loop print the value of i in the standard output
# Define i as 0 
# Pass the strategy number as a parameter to the binary. 0 Secuencial, 1 RR, 2 SHORTEST, 3 USTEDES
i=0
while [ $i -lt 3 ]
    do
        ./test_tp_sistemas 3
        #echo $i
        i=$((i+1))
    done
