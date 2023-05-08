#!/bin/zsh

mpicc 4-1-pi.c

for i in {1..10}; do
    mpiexec -n 1 ./a.out
done

for i in {1..10}; do
    mpiexec -n 2 ./a.out
done

for i in {1..10}; do
    mpiexec -n 4 ./a.out
done

for i in {1..10}; do
    mpiexec -n 8 ./a.out
done

mpicc 4-2-pi.c

for i in {1..10}; do
    mpiexec -n 1 ./a.out
done

for i in {1..10}; do
    mpiexec -n 2 ./a.out
done

for i in {1..10}; do
    mpiexec -n 4 ./a.out
done

for i in {1..10}; do
    mpiexec -n 8 ./a.out
done

