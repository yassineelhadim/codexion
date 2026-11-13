*This project has been created as part of the 42 curriculum by yel-hadi.*

# Codexion

## Description

Codexion is a POSIX-thread simulation of coders sharing a circular set of USB
dongles. A coder needs both adjacent dongles to compile, then debugs and
refactors before requesting them again. The simulation ends when every coder
has completed the requested number of compiles or when one coder burns out.

The program supports `fifo` arbitration, ordered by request arrival, and `edf`
arbitration, ordered by the earliest compile deadline. Each dongle has a
mandatory cooldown after release.

## Instructions

Compile the program:

```sh
make
```

Run it:

```sh
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

All durations are milliseconds. `scheduler` must be `fifo` or `edf`.

Example:

```sh
./codexion 5 800 100 100 100 3 10 edf
```

Use `make clean` to remove objects, `make fclean` to remove the executable,
and `make re` for a clean rebuild.

## Blocking cases handled

- A coder queues for both dongles in one critical section and is granted both
  together, so no coder holds one dongle while waiting for another.
- FIFO and EDF queues use a binary heap. A coder is granted only when it is
  first in every required dongle queue.
- Released dongles remain unavailable until their cooldown expires.
- The monitor wakes at least once per millisecond and stops the simulation when
  the earliest deadline is reached.
- Output is emitted while holding the simulation mutex, which keeps each state
  message intact and prevents output after a burnout.

## Thread synchronization mechanisms

The simulation mutex protects global state: coder states, deadlines, compile
counts, scheduler ordering, and output. Each dongle also has its own
`pthread_mutex_t`, which protects its ownership, cooldown, and waiting heap.
`pthread_cond_t` wakes coders after a grant, release, cooldown tick, or
shutdown. It is also used with timed waits so sleeping phases and the monitor
can stop promptly.

A coder adds its request to the two relevant heaps while holding the mutex,
then waits on the condition variable. The monitor and release path update the
same protected state, grant eligible pairs, and broadcast the event. This gives
the coder and monitor a single thread-safe view of ownership and deadlines.

## Resources

- [POSIX Threads Programming](https://man7.org/linux/man-pages/man7/pthreads.7.html)
- [pthread condition variables](https://man7.org/linux/man-pages/man3/pthread_cond_wait.3.html)
- [Dining philosophers problem](https://en.wikipedia.org/wiki/Dining_philosophers_problem)

AI was used to review the project against the supplied subject, identify
concurrency defects, simplify duplicated request code, and improve this
documentation. The resulting synchronization design and changes should be
understood and reviewed before submission.
