#!/bin/bash

command1() {
  sleep 1
  echo 'command1'
  date
} 
command2() {
  sleep 2
  echo 'command2'
  date
} 
command3() {
  sleep 3
  echo 'command3'
  date
} 

command1 &
pid1=${!}
command2 &
pid2=${!}
command3 &
pid3=${!}

wait ${pid1} ${pid2} ${pid3}
echo 'done'