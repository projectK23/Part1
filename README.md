# Part1
This is Part 1 project for K23a lesson at DI

# Requirements
Install valgrind for memory checks

#Usage
1. make executable and run tests:
$ make
!test.out gives test results

2. make executable only without tests:
$ make SKIP_TEST=true

3. make executable and test exec and run memtests for both:
$ make memtest

#For unit testing framework
1. Under test folder add your test_suite as example test_suite_0
2. Include your test suite in main_test.c
3. No modifications are needed in Makefile for adding only a test suite
4. If a file under src is created, the this file should be included in the corresponding targets

