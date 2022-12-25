#!/bin/sh

# Pass the strategy number as a parameter to the binary. 0 Secuencial, 1 RR, 2 SHORTEST, 3 USTEDES
i=0
while [ $i -lt 1000 ]
    do
        if [ $i -ge 0 ] && [ $i -le 249 ]
            then
                ./test_tp_sistemas 0
                echo "Secuencial"
        fi
        if [ $i -ge 250 ] && [ $i -le 499 ]
            then
                ./test_tp_sistemas 1
                echo "Round Robin"
        fi
        if [ $i -ge 500 ] && [ $i -le 749 ]
            then
                ./test_tp_sistemas 2
                echo "Shortest"
        fi
        if [ $i -ge 750 ] && [ $i -le 999 ]
            then
                ./test_tp_sistemas 3
                echo "Ustedes"
        fi
        i=$((i+1))
    done
    echo "Fin de los tests"
    exit 0