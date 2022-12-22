#!/bin/sh

#This is a script to automate testing of the binary in this directory

#First we loop constantly and execute the binary 1000 times
#And aftear each loop print the value of i in the standard output
#Define i as 0
i=0
while [ $i -lt 500 ]
    do
        ./test_tp_sistemas
        echo $i
        i=$((i+1))
    done
