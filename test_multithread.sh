#!/bin/bash

# Start a sleep request in the background (should take ~10 seconds)
{
  echo -e "GET /sleep/10 HTTP/1.1\r\nHost: localhost\r\n\r\n"
  sleep 1
} | nc localhost 8080 > sleep_output.txt &

# Give the server a tiny bit of time to start sleeping
sleep 1

# Now do a quick calc request in the foreground
{
  echo -e "GET /calc/add/3/4 HTTP/1.1\r\nHost: localhost\r\n\r\n"
} | nc localhost 8080 > calc_output.txt

# Wait for the sleep request to finish
wait

# Show results
echo "--- calc_output.txt ---"
cat calc_output.txt
echo
echo "--- sleep_output.txt ---"
cat sleep_output.txt