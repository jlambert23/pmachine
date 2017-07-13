#!/bin/bash
COUNTER=1
while [	$COUNTER -lt 28	]; do
	touch cases/errors/error$COUNTER.in
	nano cases/errors/error$COUNTER.in
	gcc -std=c99 compiler.c
	./a.out -f cases/errors/error$COUNTER.in
	cp io/par_out.txt cases/errors/error$COUNTER.out
	cat cases/errors/error$COUNTER.out
	read -rsp $'Press enter to continue...\n'	

	let COUNTER=COUNTER+1
done
