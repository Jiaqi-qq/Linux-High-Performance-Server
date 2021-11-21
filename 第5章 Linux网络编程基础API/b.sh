#!/bin/sh
i="0"
while (( $i<"10" ))
do
	./a.out 39.101.244.246 12346
	let "i++"
done
