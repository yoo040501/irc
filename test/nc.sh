#!/bin/bash

BASE_NICK="nick"
COUNTER_FILE="counter.txt"

if [ -f "$COUNTER_FILE" ]; then
  COUNTER=$(cat "$COUNTER_FILE")
else
  COUNTER=1
fi

trap "rm -f $COUNTER_FILE" EXIT

{
  echo -e "PASS 1234"
  echo -e "USER aa aa aa aa"

  NICK="$BASE_NICK$COUNTER"
  echo -e "NICK $NICK"

  let COUNTER++
  echo $COUNTER > "$COUNTER_FILE"
  sleep 60
    while true; do
    echo -e "PING :server"
    sleep 60
  done
} | nc localhost 6667