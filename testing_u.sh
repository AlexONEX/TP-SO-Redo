#!/bin/sh

# Pass the strategy number as a parameter to the binary. 0 Secuencial, 1 RR, 2 SHORTEST, 3 USTEDES
while(true)
    do
    i=0
    while [ $i -lt 50 ]
        do
        ./test_tp_sistemas 3
        echo "Ustedes"
        echo $i
        i=$((i+1))
        done
        echo "Fin de los tests"
    done
    i=0