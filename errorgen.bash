#!/bin/bash
COUNTER=1
while [	$COUNTER -lt 2	]; do
	nano cases/errors/error$COUNTER.in
	gcc -std=c99 compiler.c
	./a.out -f cases/errors/error$COUNTER.in
	cp io/vm_out.txt cases/errors/error$COUNTER.out

	let COUNTER=COUNTER+1
done
