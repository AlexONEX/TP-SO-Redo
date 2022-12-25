#!/bin/sh

# Pass the strategy number as a parameter to the binary. 0 Secuencial, 1 RR, 2 SHORTEST, 3 USTEDES
i=0
while(true)
do
    while [ $i -lt 500 ]
        do
            ./test_tp_sistemas 1
            echo $i
            i=$((i+1))
        done
done