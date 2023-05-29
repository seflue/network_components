#!/usr/bin/env bash
BASE_STATION_EXECUTABLE="./base_station"
ADDRESS_LIST_FILE="base_stations.txt"

while IFS= read -r address || [[ -n "$address" ]]; do
  if echo "$address" | grep -qP '^(\d{1,3}\.){3}\d{1,3}$'; then
    echo "Run: ./base_station --ip-address \"${address}\" &"
    ./base_station --ip-address "$address" &
    PID=$!
    echo "$PID" >> pid_list.txt
  else
    echo "Invalid IP address: \"$address\""
  fi
done < base_stations.txt

read -rp "Press Enter to stop the services..."

while IFS= read -r pid || [[ -n "$pid" ]]; do
  kill "$pid"
done < pid_list.txt

rm pid_list.txt