#!/bin/sh

# Pass the strategy number as a parameter to the binary. 0 Secuencial, 1 RR, 2 SHORTEST, 3 USTEDES
while(true)
    do
    i=0
    while [ $i -lt 400 ]
        do
            if [ $i -ge 0 ] && [ $i -le 99 ]
                then
                    ./test_tp_sistemas 0
                    echo "Secuencial"
            fi
            if [ $i -ge 100 ] && [ $i -le 199 ]
                then
                    ./test_tp_sistemas 1
                    echo "Round Robin"
                    echo $i
            fi
            if [ $i -ge 200 ] && [ $i -le 299 ]
                then
                    ./test_tp_sistemas 2
                    echo "Shortest"
            fi
            if [ $i -ge 300 ] && [ $i -le 399 ]
                then
                    ./test_tp_sistemas 3
                    echo "Ustedes"
            fi
            i=$((i+1))
        done
        echo "Fin de los tests"
    done
    exit 0