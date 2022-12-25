#!/bin/sh

# Pass the strategy number as a parameter to the binary. 0 Secuencial, 1 RR, 2 SHORTEST, 3 USTEDES
i=0
while(true)
do
    while [ $i -lt 1000 ]
        do
            #if i between 0 249 test_tp_sistemas 0
            #if i between 250 499 test_tp_sistemas 1
            #if i between 500 749 test_tp_sistemas 2
            #if i between 750 999 test_tp_sistemas 3
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
        exit 0
done