./start 0
sleep 1
./producer 1 2 &
./consumer 1 &
