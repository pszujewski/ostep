See this for more info on timer

https://users.pja.edu.pl/~jms/qnx/help/watcom/clibref/qnx/clock_gettime.html

There are 12 cpus on this system.
How to determine the number of cpus on your system in C:

```c
long numcpus = sysconf(_SC_NPROCESSORS_ONLN);
```

https://stackoverflow.com/questions/4586405/how-to-get-the-number-of-cpus-in-linux-using-c
