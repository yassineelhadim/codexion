how is a thread created???:
pthread_create()
      ↓
glibc/pthread implementation
      ↓
allocate/setup thread's user-space data
      ↓
prepare a new stack
      ↓
clone/clone3 system call
      ↓
KERNEL
      ↓
create a new schedulable task
      ↓
set its CPU state
      ↓
new thread becomes runnable
      ↓
CPU scheduler eventually runs it
      ↓
your start function executes





















what is a thread in low-level understanding? watch youtube video
what is a process in low level understanding? watch youtube video
what is a posix thread?





learn how to make a deadlock then solve it
learn how to make a starvation then solve it
learn how to make a race condition then solve
learn how to use heap, do exercises on it