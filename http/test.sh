#!/bin/sh
#make
#./server &
#sleep 0.01
#./client

java -client  -jar ./imdgtest-client-1.0-SNAPSHOT.jar -id 0 -server localhost:8080 exec-trans -batch 1 trx.txt
