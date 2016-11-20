# Part1
This is Part 1 project for K23a lesson at DI

# Requirements
Install valgrind for memory checks

#Usage
1. make executable and run tests:
$ make
(test.out gives test results)

2. make executable only without tests:
$ make SKIP_TEST=true

3. make executable and test exec and run memtests for both:
$ make memtest

#For unit testing framework
1. Under test folder add your test_suite as example test_suite_0
2. Include your test suite in main_test.c
3. No modifications are needed in Makefile for adding only a test suite
4. If a file under src is created, then this file should be included in the corresponding targets

#Logging
There are logs for different levels
1. FATAL
2. ERROR LOGS
3. TRACE
4. ALL

#How to use logs
1. Fatal logs are always printed, you don't have to do anything
2. Error logs are printed along with error logs           : $ make D=error
3. Trace logs are printed along with fatal and error logs : $ make D=trace
4. All logs are printed                                   : $ make D=all







