*This project has been created as part of the 42 curriculum by \<mecharra>.*

# Codexion

## Description

Codexion is a C concurrency simulation in which multiple coders compete for a limited set of shared USB dongles.

Each coder is represented by a POSIX thread. Coders are arranged in a circular structure, and each coder has access to a left and a right dongle. A coder must acquire both dongles simultaneously before starting a compilation.

The simulation models three activities:

1. **Compiling** — the coder holds two dongles for `time_to_compile` milliseconds.
2. **Debugging** — after releasing the dongles, the coder spends `time_to_debug` milliseconds debugging.
3. **Refactoring** — after debugging, the coder spends `time_to_refactor` milliseconds refactoring.

After refactoring, the coder attempts to acquire two dongles and compile again.

The simulation is controlled by a separate monitor thread. The monitor continuously checks whether a coder has exceeded its burnout deadline and also checks whether every coder has completed the required number of compilations.

The project implements:

* POSIX threads with `pthread_create()` and `pthread_join()`.
* Mutex-protected shared resources.
* Condition-variable synchronization for coordinated thread startup.
* Per-dongle request queues implemented as fixed-size binary heaps.
* FIFO and EDF scheduling policies.
* Dongle cooldown periods.
* A separate monitor thread for burnout detection.
* Serialized logging.
* Thread-safe simulation stop state.
* Dynamic allocation and cleanup of coders and dongles.

The main simulation structures are defined in `codexion.h`. A `t_sim` contains the global simulation state, while each `t_coder` stores the state of an individual coder and each `t_dongle` stores the synchronization and scheduling state of a dongle.

---

## Features

### Multithreaded coders

Each coder runs in its own thread through `coder_routine()`.

The coder waits for the simulation to be released, performs its compilation/debugging/refactoring cycle, and stops when the simulation stop state becomes active.

### Two-dongle compilation

A coder must acquire both its left and right dongles before compiling.

The implementation protects both dongles by locking their mutexes in a deterministic pointer order. This prevents different coders from acquiring the same pair in opposite orders.

### FIFO scheduling

When the `fifo` scheduler is selected, each request receives an increasing sequence number.

The sequence number is stored in the request's `key` and is used by the heap ordering to serve requests according to their arrival order.

### EDF scheduling

When the `edf` scheduler is selected, the request key is based on:

```text
last_compile_start + time_to_burnout
```

This represents the coder's burnout deadline. The heap therefore gives priority to the request with the earliest deadline.

### Priority queue

Each dongle contains a request heap with a capacity of two requests.

Requests are ordered using:

1. Request key.
2. Compile count.
3. Coder ID.

This provides deterministic ordering when two requests have equivalent primary keys.

### Dongle cooldown

After a dongle is released, `cooldown_until` is updated using the current simulation time plus the configured cooldown duration.

A dongle cannot be considered ready until this timestamp has been reached.

### Burnout monitoring

A dedicated monitor thread checks every coder's time since its last compilation start.

A coder is considered burned out when:

```text
elapsed time >= time_to_burnout
```

When burnout is detected, the monitor sets the simulation stop state and prints the burnout message while holding the logging mutex.

### Completion detection

The monitor also checks whether every coder has reached `compiles_required`.

If all coders have completed the required number of compilations, the simulation stops normally.

---

## Instructions

### Compilation

The project is compiled with `cc` using:

```text
-Wall -Wextra -Werror -pthread
```

The Makefile builds the executable named `codexion`.

Compile the project with:

```bash
make
```

Clean object files:

```bash
make clean
```

Remove object files and the executable:

```bash
make fclean
```

Rebuild the project:

```bash
make re
```

### Execution

The program requires exactly eight arguments:

```text
number_of_coders
time_to_burnout
time_to_compile
time_to_debug
time_to_refactor
number_of_compiles_required
dongle_cooldown
scheduler
```

The scheduler must be either:

```text
fifo
```

or:

```text
edf
```

The parser rejects an incorrect number of arguments, non-numeric values, negative values, an invalid scheduler, fewer than one coder, and a zero compilation requirement.

Example:

```bash
./codexion 5 800 200 200 200 3 50 fifo
```

Another example using EDF:

```bash
./codexion 5 800 200 200 200 3 50 edf
```

---

## Program Arguments

| Argument                      | Description                                       |
| ----------------------------- | ------------------------------------------------- |
| `number_of_coders`            | Number of coder threads and number of dongles     |
| `time_to_burnout`             | Maximum allowed time between compilation starts   |
| `time_to_compile`             | Duration of the compilation phase                 |
| `time_to_debug`               | Duration of the debugging phase                   |
| `time_to_refactor`            | Duration of the refactoring phase                 |
| `number_of_compiles_required` | Number of compilations each coder must complete   |
| `dongle_cooldown`             | Time a released dongle remains unavailable        |
| `scheduler`                   | Dongle request scheduling policy: `fifo` or `edf` |

These values are stored in the `t_config` structure.

---

## Simulation Flow

The main execution flow is:

```text
Parse arguments
      |
      v
Initialize simulation
      |
      v
Create coder threads
      |
      v
Create monitor thread
      |
      v
Release all threads through the start condition
      |
      v
+-------------------------------+
|       Simulation running      |
|                               |
|  Acquire two dongles          |
|          |                    |
|          v                    |
|       Compile                 |
|          |                    |
|          v                    |
|    Release dongles            |
|          |                    |
|          v                    |
|       Debug                   |
|          |                    |
|          v                    |
|      Refactor                 |
|          |                    |
|          +----> Compile       |
+-------------------------------+
      |
      +----> Coder burns out
      |
      +----> All coders complete
      |
      v
Join threads
      |
      v
Cleanup
```

The main function performs argument parsing, initialization, execution, cleanup, and returns the simulation result.

---

## Blocking cases handled

### Deadlock prevention

A coder needs two dongles simultaneously. Without an ordering rule, two coders could potentially lock different dongles and then wait indefinitely for the other dongle.

The implementation prevents this by always locking the two dongle mutexes in the same deterministic order based on their addresses:

```text
if (a < b)
    lock a, then b
else
    lock b, then a
```

This prevents the circular lock-order dependency that can produce a deadlock.

In terms of the classic Coffman deadlock conditions, the consistent lock ordering specifically prevents the **circular wait** condition from forming between dongle locks.

### Starvation prevention

Requests are not handled simply by repeatedly trying to acquire a dongle.

Each dongle maintains a request heap, and the selected scheduling policy determines which waiting coder is allowed to acquire it.

For FIFO, requests are ordered by their arrival sequence.

For EDF, requests are ordered by their burnout deadline.

The request comparison also uses compile count and coder ID as tie-breakers, providing deterministic ordering.

### Dongle cooldown

A released dongle is not immediately reusable.

`dongle_release()` sets:

```text
cooldown_until = current_time + dongle_cooldown
```

`dongle_ready()` then checks the current simulation time against this value before allowing the dongle to be acquired.

This makes the cooldown part of the resource availability decision rather than treating a released dongle as immediately available.

### Precise burnout detection

The monitor thread independently checks every coder's burnout deadline.

The elapsed time is calculated from the coder's `last_compile_start`. The monitor declares burnout when the elapsed time reaches or exceeds `time_to_burnout`.

The monitor continuously performs this check with a short sleep between iterations:

```text
while (!sim_stopped(sim))
{
    scan for burnout;
    check whether all coders are done;
    usleep(1000);
}
```

This provides frequent monitoring while avoiding a completely busy loop.

### Log serialization

Logging is protected by `log_mutex`.

For example, when a coder reports its compilation state, the logging mutex is acquired before writing the messages and released afterwards.

The monitor also takes the same logging mutex before printing the burnout message.

Therefore, coder and monitor output cannot simultaneously write their log messages through these protected logging sections.

### Safe simulation stopping

The simulation stop state is shared between coder threads and the monitor.

Access to `sim->stop` and `sim->burned_out` is protected by `stop_mutex`.

`sim_stopped()` reads the stop state while holding the mutex, while `sim_set_stop()` modifies it under the same mutex.

This prevents unsynchronized access to the simulation termination state.

---

## Thread synchronization mechanisms

### `pthread_mutex_t`

The project uses several mutexes with different responsibilities.

#### Dongle mutexes

Every `t_dongle` contains its own `pthread_mutex_t`.

```text
typedef struct s_dongle
{
    pthread_mutex_t mutex;
    ...
} t_dongle;
```

The mutex protects the dongle's state, including whether it is currently taken, its cooldown timestamp, and its request heap.

When two dongles are needed simultaneously, both mutexes are acquired using the deterministic ordering described above.

#### Stop-state mutex

`stop_mutex` protects shared simulation state such as:

```text
stop
burned_out
fifo_seq
compile_count
last_compile_start
```

The code uses this mutex whenever these shared values need synchronized access.

#### Logging mutex

`log_mutex` serializes output from multiple threads.

Coder messages and monitor messages use this mutex when they need to write state changes.

#### Start mutex

`start_mutex` protects the startup state used by the condition variable:

```text
start_ready
start_failed
```

It ensures that threads see a consistent startup state.

---

### `pthread_cond_t`

The project uses `pthread_cond_t` to synchronize the beginning of the simulation.

The simulation contains:

```text
pthread_cond_t start_cond;
```

Each coder calls `wait_for_start()` before entering its normal routine. The function locks `start_mutex` and waits while `start_ready` is false.

After the coder and monitor threads have been created, `threads_start()` sets `start_ready` and broadcasts on the condition variable:

```text
pthread_mutex_lock(&sim->start_mutex);
sim->start_ready = 1;
pthread_cond_broadcast(&sim->start_cond);
pthread_mutex_unlock(&sim->start_mutex);
```

This allows the waiting threads to start together after thread creation has completed.

If thread creation fails, the same condition variable is used to release threads that are waiting during startup.

### Custom event implementation

No separate custom event abstraction or event object is present in the supplied source code.

The synchronization mechanisms visible in the implementation are:

* `pthread_mutex_t`
* `pthread_cond_t`
* Shared state protected by mutexes
* The request heap used for dongle arbitration

Therefore, this README documents the synchronization mechanisms that actually appear in the supplied implementation rather than describing an event system that is not present in the code.

---

## Race-condition prevention

### Dongle acquisition

The request is inserted into both dongles' heaps while both dongle mutexes are held.

```text
lock both dongles
    |
    +-- add request to left heap
    |
    +-- add request to right heap
unlock both dongles
```

Later, acquisition checks both dongles while both mutexes are held. If both are ready, both requests are removed and both dongles are marked as taken before releasing the mutexes.

This prevents another thread from changing either dongle's state between the readiness check and the acquisition.

### Compile count

The coder's `compile_count` is modified while `stop_mutex` is held:

```text
pthread_mutex_lock(&sim->stop_mutex);
coder->compile_count++;
pthread_mutex_unlock(&sim->stop_mutex);
```

The monitor uses the same mutex when reading compile counts.

### Burnout deadline

`last_compile_start` is written under `stop_mutex`, and the monitor reads it under the same mutex. This prevents concurrent access to the timestamp from occurring without synchronization.

### Communication between coders and monitor

Coders do not directly communicate with the monitor through a custom messaging system.

Instead, they communicate through shared simulation state protected by mutexes:

```text
Coder threads
    |
    +-- last_compile_start
    +-- compile_count
    +-- stop
    |
    v
stop_mutex
    |
    v
Monitor thread
```

The monitor observes the protected state and can call `sim_set_stop()` when a burnout or successful completion condition is detected. Coder threads repeatedly call `sim_stopped()` and terminate when the monitor has stopped the simulation.

---

## Scheduling

The implementation supports two scheduling policies.

### FIFO

FIFO means that requests are ordered according to the sequence in which they arrive.

The simulation maintains `fifo_seq` and increments it when a FIFO request is created. The resulting sequence number becomes the request key.

### EDF

EDF means Earliest Deadline First.

The request key is calculated from:

```text
last_compile_start + time_to_burnout
```

The request with the earliest key has priority in the heap.

### Request ordering

The heap comparison is:

```text
1. key
2. compile_count
3. coder_id
```

The comparison is implemented by `req_less()`.

---

## Time management

The project uses `gettimeofday()` to obtain millisecond timestamps.

`time_now_ms()` converts the returned seconds and microseconds into milliseconds. `sim_elapsed_ms()` calculates elapsed simulation time relative to `start_time`.

The project also implements `smart_sleep()`, which repeatedly checks whether the simulation has stopped while waiting for the requested duration:

```text
while (!sim_stopped(sim) && sim_elapsed_ms(sim) < target)
    usleep(1000);
```

This allows sleeping operations to terminate early when the simulation is stopped.

---

## Memory management

The simulation dynamically allocates arrays for:

* Coders
* Dongles

These are allocated during initialization and released during cleanup.

Cleanup destroys the dongle mutexes and simulation synchronization primitives before freeing the allocated arrays.

Initialization also contains failure cleanup paths that destroy already-created synchronization objects and free allocated memory if initialization fails.

---

## Project Structure

The project is divided into several source files:

| File            | Responsibility                                                  |
| --------------- | --------------------------------------------------------------- |
| `codexion.c`    | Program entry point and simulation execution                    |
| `codexion.h`    | Structures, constants, and function declarations                |
| `coder.c`       | Coder thread routine and coder activity cycle                   |
| `dongle.c`      | Dongle acquisition and request management                       |
| `dongle_wait.c` | EDF request deadline calculation                                |
| `heap.c`        | Priority queue operations                                       |
| `sift.c`        | Heap ordering and sift operations                               |
| `init.c`        | Simulation, coder, dongle, and mutex initialization             |
| `monitor.c`     | Burnout and completion monitoring                               |
| `threads.c`     | Thread creation, startup, joining, and startup failure handling |
| `state.c`       | Thread-safe simulation stop state                               |
| `log.c`         | Thread-safe logging                                             |
| `time.c`        | Time measurement and interruptible sleeping                     |
| `utils.c`       | Numeric parsing, dongle release, and dongle readiness           |
| `parser.c`      | Command-line argument parsing                                   |
| `cleanup.c`     | Simulation resource cleanup                                     |

The Makefile compiles these source files into the `codexion` executable.

---

## Output

The program logs coder state changes using the following general format:

```text
timestamp coder_id message
```

Examples produced by the implementation include:

```text
0 1 has taken a dongle
1 1 is compiling
201 1 is debugging
401 1 is refactoring
```

A burnout is reported as:

```text
timestamp coder_id burned out
```

Logging is protected by `log_mutex` so that concurrent logging operations are serialized.

---

## Resources

The following resources are relevant to the concepts used in this project:

* **POSIX Threads Documentation** — Reference for threads, mutexes, condition variables, thread creation, and thread synchronization.
* **C Documentation** — Reference for C language features, memory management, and standard functions used by the project.
* **`gettimeofday()` Documentation** — Reference for obtaining timestamps and measuring elapsed time.
* **`usleep()` Documentation** — Reference for short-duration thread sleeping.
* **Binary Heap / Priority Queue concepts** — Background on the heap structure used to manage dongle requests.
* **FIFO and EDF Scheduling concepts** — Background on the scheduling policies implemented for dongle requests.
* **Deadlock and Thread Synchronization concepts** — Background on mutex locking, race conditions, and deadlock prevention.

### AI usage

AI was used as a support tool for documentation and understanding of the project.

Specifically, AI assistance was used to:

* Help structure and write this README.
* reviewing code for potential bugs
* improving code organization

---
