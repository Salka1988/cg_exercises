#!/bin/bash



for i in /home/salka1988/Documents/CG/computer-graphics-1-task1b-01531697/data/task1b/cone_of_destiny.json
do
  ~/Documents/CG/computer-graphics-1-task1b-01531697/build/bulb/bin/task1b "$i" -t;

if command ; then
    echo "Command succeeded"
else
    echo "Command failed"
fi
done

