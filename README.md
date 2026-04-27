*This project has been created as part of the 42 curriculum by ndi-tull.*

# Codexion

## Description

Codexion is a concurrency simulation that models multiple "coders" working in a shared
environment, competing for limited USB dongles to compile their code. The project
explores the classical resource-sharing problems of multi-threaded programming
(inspired by the Dining Philosophers problem) using POSIX threads, mutexes, and
condition variables.

Each coder is represented by a thread that cycles through three phases:
**compiling**, **debugging**, and **refactoring**. Compiling requires holding two
USB dongles simultaneously (one to the left and one to the right of the coder).
After releasing the dongles, a configurable cooldown must elapse before another
coder can pick them up.

The simulation stops in one of two ways:

- **Success**: every coder has compiled at least `number_of_compiles_required` times.
- **Failure**: a coder burns out (fails to start a new compile within
  `time_to_burnout` milliseconds since the start of their previous compile or
  the start of the simulation).

Two arbitration policies are supported when multiple coders request the same dongle:

- **FIFO** (First In, First Out): the coder whose request arrived first wins.
- **EDF** (Earliest Deadline First): the coder whose burnout deadline is the closest wins.

A separate **monitor thread** continuously watches the state of every coder
to detect burnouts and the global completion condition with millisecond precision.

## Instructions

### Compilation

The project is written in C and uses a Makefile with the following targets:

- `make` or `make all` — builds the `codexion` binary
- `make clean` — removes intermediate object files
- `make fclean` — removes object files and the binary
- `make re` — full rebuild

The code compiles with `-Wall -Wextra -Werror -pthread` and produces no warnings.

### Execution

```sh
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug \
           time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

All eight arguments are mandatory:

| Argument                       | Type    | Meaning                                                       |
|--------------------------------|---------|---------------------------------------------------------------|
| `number_of_coders`             | int > 0 | number of coders and dongles                                  |
| `time_to_burnout`              | ms      | maximum time without compiling before a coder burns out       |
| `time_to_compile`              | ms      | duration of the compile phase                                 |
| `time_to_debug`                | ms      | duration of the debug phase                                   |
| `time_to_refactor`             | ms      | duration of the refactor phase                                |
| `number_of_compiles_required`  | int     | required compiles per coder for the simulation to succeed     |
| `dongle_cooldown`              | ms      | mandatory delay before a released dongle can be picked again  |
| `scheduler`                    | string  | `fifo` or `edf`                                               |

Invalid inputs (negative numbers, non-integers, unknown scheduler) are rejected
with an explicit error message and a non-zero exit code.

### Examples

```sh
./codexion 5 800 200 200 200 5 30 fifo   # 5 coders, FIFO scheduling
./codexion 8 2000 50 50 50 10 20 edf     # 8 coders, EDF scheduling
./codexion 1 300 100 100 100 5 10 fifo   # single coder will burn out (only 1 dongle)
```

### Output

Each state change is logged to stdout in the following format:

<timestamp_ms> <coder_id> <message>

Possible messages: `has taken a dongle`, `is compiling`, `is debugging`,
`is refactoring`, `burned out`. All log lines are serialized through a dedicated
mutex so that no two messages ever interleave.

## Blocking cases handled

The simulation must avoid all four classical concurrency hazards. Each one is
addressed explicitly:

### Deadlock prevention (Coffman's conditions)

Coffman identified four necessary conditions for deadlock: mutual exclusion,
hold-and-wait, no preemption, and circular wait. The first three are inherent to
the problem (dongles cannot be shared, coders need both before compiling, no
external entity can revoke a dongle). **Circular wait is the one we break**.

If every coder picked up their left dongle first and then their right, a circular
chain could form where each coder holds one dongle and waits for the next.
To avoid this, coders use an **asymmetric pickup order based on parity**:

- **Even-numbered coders** pick up the **right** dongle first, then the left.
- **Odd-numbered coders** pick up the **left** dongle first, then the right.

This breaks the symmetry of the request graph and mathematically eliminates
circular waiting.

### Starvation prevention

Without arbitration, a fast or lucky coder could repeatedly grab the same dongle
and starve a slower neighbor. To prevent this, every dongle maintains its own
**priority queue (min-heap)** that orders pending requests:

- Under **FIFO**, the queue is ordered by arrival timestamp.
- Under **EDF**, the queue is ordered by `last_compile_start + time_to_burnout`,
  meaning the coder who is most at risk of burning out is served first.

When timestamps or deadlines are equal, a **monotonically increasing sequence
counter** breaks ties deterministically based on the actual order of arrival
into the queue. This guarantees liveness whenever the simulation parameters
are feasible, as required by the subject.

### Cooldown handling

After a dongle is released, it cannot be reacquired until `dongle_cooldown` ms
have elapsed. This is enforced inside the dongle acquisition loop: a coder who
holds the head of the queue still has to wait if `now - release_time < cooldown`.
The monitor thread broadcasts on every dongle's condition variable periodically,
which wakes up sleeping coders so they can re-evaluate the cooldown without
busy-waiting.

### Precise burnout detection

A dedicated **monitor thread** scans every coder's state every millisecond.
A coder is declared burned out when:
(nb_compile < required) AND (now - last_compile_start > time_to_burnout)

The first condition is essential: without it, a coder who has already finished
its quota would still be flagged after sitting idle, producing false-positive
burnouts. The actual burnout log is printed within 1–2 ms of the deadline,
well within the 10 ms tolerance required by the subject.

### Log serialization

All log messages share a single `log_mutex`. Any thread that writes to stdout
must hold this mutex first. This guarantees that no two messages ever
interleave on the same line, even under heavy concurrency.

## Thread synchronization mechanisms

The implementation uses three POSIX primitives, each protecting a specific
class of shared state.

### Mutexes (`pthread_mutex_t`)

| Mutex                            | Protects                                        |
|----------------------------------|-------------------------------------------------|
| `dongle[i].mutex`                | `free`, `release_time`, `seq_counter`, `queue`  |
| `coders[i].mutex`                | `timestamp`, `nb_compile` for that coder        |
| `shared_data->simulation_mutex`  | the `simulation_over` flag                      |
| `shared_data->log_mutex`         | every printf to stdout                          |

Each mutex is initialized with `pthread_mutex_init` (with return-value checks
and rollback on failure) and destroyed at the end of the simulation.

### Condition variables (`pthread_cond_t`)

Each dongle has its own condition variable. Coders waiting for a dongle call
`pthread_cond_wait`, which atomically releases the dongle's mutex and suspends
the thread. The thread is woken up either by:

- a `pthread_cond_broadcast` from `release_dongles` (a coder is releasing this dongle),
- a `pthread_cond_broadcast` from the monitor (periodic re-check, useful for
  cooldown expirations),
- a `pthread_cond_broadcast` from `stop_simulation` (graceful shutdown).

The standard `while`-based wait pattern is used:

```c
while (!is_simulation_over(sd) && !dongle_ready(coder, idx))
    pthread_cond_wait(&d->condition, &d->mutex);
```

This guards against spurious wake-ups: even if a thread is woken without the
condition being satisfied, it loops back and re-checks.

### Race condition prevention — examples

**Example 1: protecting `simulation_over`.**
The monitor sets `simulation_over = 1` when the simulation must stop, while
all coders read it concurrently. Without protection this is a data race.
The flag is always read and written under `simulation_mutex`. A helper
`is_simulation_over(sd)` encapsulates the locked read.

**Example 2: dongle ownership.**
The transition from "free" to "owned" must be atomic with respect to all other
coders contending for the same dongle. The whole sequence — checking `free`,
checking the cooldown, checking the queue head, popping from the queue, and
setting `free = 0` — is performed inside one critical section guarded by
`dongle[i].mutex`. No coder can interrupt another mid-sequence.

**Example 3: deterministic queue ordering.**
When several coders push into the same queue in the same millisecond, comparing
timestamps alone is not enough — the heap could break ties non-deterministically.
A `seq_counter` is incremented under the dongle mutex on every push; the heap's
comparator falls back to the sequence number when timestamps are equal,
producing a strictly deterministic order.

### Thread-safe communication coder ↔ monitor

The monitor never modifies coder fields; it only reads `timestamp` and
`nb_compile` under each coder's mutex. The coders never read the monitor's
internal state. The only piece of state shared in both directions is
`simulation_over`, which the monitor sets and the coders read, always under
`simulation_mutex`. This unidirectional flow eliminates an entire class of
potential races.

When the monitor decides to stop the simulation, it sets the flag and then
broadcasts on every dongle's condition variable. Any coder currently sleeping
in `pthread_cond_wait` immediately wakes up, sees `is_simulation_over() == 1`,
and exits its acquisition loop without taking the dongle.

### Robust initialization with rollback

Every `pthread_mutex_init`, `pthread_cond_init`, and `pthread_create` call has
its return value checked. On failure, partial initialization is rolled back:

- Helpers `destroy_dongles_until(n)` and `destroy_coders_until(n)` destroy
  only the first `n` already-initialized resources.
- `stop_and_join_coders(sd, threads, n)` signals the `n` already-running
  coder threads to exit (via `simulation_over` and broadcasts), then joins
  them cleanly before any memory is freed.

This guarantees that no resource leak or use-after-destroy occurs, regardless
of where in the initialization sequence a system call may fail.

## Resources

### Documentation and references

- **POSIX Threads Programming** — Lawrence Livermore National Laboratory:
  https://hpc-tutorials.llnl.gov/posix/
- **The Linux Programming Interface** by Michael Kerrisk — Chapters 29–30 on
  threads, mutexes, and condition variables.
- **Dining Philosophers Problem** — Edsger W. Dijkstra's classical formulation,
  which directly inspired this assignment's structure.
- **Earliest Deadline First scheduling** — Liu & Layland, "Scheduling Algorithms
  for Multiprogramming in a Hard-Real-Time Environment" (1973).
- **Coffman conditions for deadlock** — Coffman, Elphick & Shoshani,
  "System Deadlocks" (1971).
- **`man` pages**: `pthread_create(3)`, `pthread_mutex_init(3)`,
  `pthread_cond_wait(3)`, `pthread_cond_timedwait(3)`, `gettimeofday(2)`,
  `usleep(3)`.

### Use of AI

AI was used as a brainstorming and reviewing tool during the development of
this project, not as a code generator. Specifically:

- **Concept clarification**: discussing the precise semantics of EDF, the role
  of the tie-breaker, and the interaction between cooldown and condition variables.
- **Stress-testing strategy**: suggesting LD_PRELOAD-based fault injection to
  verify that `pthread_create`, `pthread_mutex_init`, and `malloc` failures
  are handled without leaks.

Every line of code in the project was written, reviewed, tested, and is
fully understood by the author. AI suggestions were always cross-checked
against the official POSIX documentation and verified empirically with
Valgrind, Helgrind, and ThreadSanitizer.