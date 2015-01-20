#!/bin/sh
n=1
while [ $n -le $1 ]
do
	if [ $n -le $2 ]
	then
		./evaluate $n $(expr $n - 1) $3 $4
	else
		./evaluate $n $2 $3 $4
	fi
	n=$(expr $n + 1)
done
