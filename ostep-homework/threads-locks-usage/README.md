See this for more info on timer

https://users.pja.edu.pl/~jms/qnx/help/watcom/clibref/qnx/clock_gettime.html

There are 12 cpus on this system.
How to determine the number of cpus on your system in C:

```c
long numcpus = sysconf(_SC_NPROCESSORS_ONLN);
```

https://stackoverflow.com/questions/4586405/how-to-get-the-number-of-cpus-in-linux-using-c

Homework responses
From: https://pages.cs.wisc.edu/~remzi/OSTEP/threads-locks-usage.pdf
Page: 16

1. I was able to put together an effective timer using `clock_gettime`.
   For example:

```c
clock_gettime(CLOCK_REALTIME, start);
```

I tested it using the `sleep()` function. The timer accurately reflected how long "sleep" was active.

2. There are 12 cpus on my system. I found this out using the `lscpu` command. When I try to create 12
   threads in my counter program, I get the following:

```
Approximate seconds elapsed: 1
*** stack smashing detected ***: <unknown> terminated
[1]    1070 abort      ./counter
```

I have to reduce the number of threads created (and counting) to 3 in order to not get this error.
