#!/bin/sh

# Pass the strategy number as a parameter to the binary. 0 Secuencial, 1 RR, 2 SHORTEST, 3 USTEDES
i=0
while [ $i -lt 1 ]
    do
        ./test_tp_sistemas 3
        #echo $i
        i=$((i+1))
    done
