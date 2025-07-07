#!/bin/bash

set -e

make

./zappy_server -p 4242 -x 20 -y 20 -n team1 team2 -c 6 -f 100 & SERVER_PID=$!
sleep 1

./zappy_gui -p 4242 -h localhost -g 3d & GUI_PID=$!
sleep 1

for i in {1..3}; do
    ./zappy_ai -p 4242 -n team1 -h localhost & TEAM1_PIDS[$i]=$!
    sleep 0.2
done

for i in {1..3}; do
    ./zappy_ai -p 4242 -n team2 -h localhost & TEAM2_PIDS[$i]=$!
    sleep 0.2
done

echo "Serveur, GUI et 2 équipes de 3 IA lancés."
trap "echo 'Arrêt...'; kill $SERVER_PID $GUI_PID ${TEAM1_PIDS[*]} ${TEAM2_PIDS[*]}; exit" SIGINT SIGTERM

wait
