# ck_testing

A collection of C/CMake examples that demonstrate the [Concurrency Kit](https://concurrencykit.org/) ring buffer API.

## Overview

This project contains self-contained example applications that showcase different ring buffer variants:

- **[ck_testing_app](example/ck_testing_app/)** — SPSC (Single-Producer/Single-Consumer) ring buffer
- **[ck_mpsc_app](example/ck_mpsc_app/)** — MPSC (Many-Producer/Single-Consumer) ring buffer with threads

Each example is built into its own executable and has its own source folder with a dedicated README.

## Build configuration

The top-level [CMakeLists.txt](CMakeLists.txt) configures the project using CMake 3.15+. It uses CPM to automatically download Concurrency Kit as a dependency.

### Dependency management

- CPM downloads are cached in the repository-local `.packages` directory set by `CPM_SOURCE_CACHE`
- CK is declared and configured in [packages/ck/CMakeLists.txt](packages/ck/CMakeLists.txt)
- CK headers are taken from the CPM package include directory
- Local CK machine-definition settings are in [include/ck_md.h](include/ck_md.h)

### Local CK configuration

The [include/ck_md.h](include/ck_md.h) header provides the minimum CK machine-definition settings:

- `CK_MD_CACHELINE` defaults to 64 bytes
- `CK_MD_PAGESIZE` defaults to 4096 bytes  
- `CK_MD_TSO` is defined for x86_64 (Total Store Order memory model)

Different architectures or memory models require appropriate CK configuration.

## Building and running

### Build all examples

```sh
cmake -S . -B build
cmake --build build
```

### Run the examples

```sh
./build/example/ck_testing_app/ck_testing_app
./build/example/ck_mpsc_app/ck_mpsc_app
```

For details on what each example does, see:
- [example/ck_testing_app/README.md](example/ck_testing_app/README.md)
- [example/ck_mpsc_app/README.md](example/ck_mpsc_app/README.md)

## How the CK ring buffer works

CK's ring buffer is a bounded FIFO queue designed for concurrent access. It is implemented as a circular buffer with separate producer and consumer counters.

### Core concepts

- Fixed-size ring, size must be a power of two
- `ck_ring_capacity()` reports the configured slot count
- `ck_ring_size()` reports how many entries are currently queued
- Bit masking is used for index wrapping (no modulo arithmetic)
- One slot is intentionally left unused, so maximum occupancy is `capacity - 1`

Internal ring state tracks:

- `c_head` — consumer position
- `p_head` — producer reservation position  
- `p_tail` — producer publish position
- `size` and `mask` — ring size and wrap mask

### Single-producer/single-consumer (SPSC)

When there is at most one producer and one consumer, CK provides the lightweight SPSC API:

**Enqueue:**
- Producer checks if ring is full
- If space available, pointer is copied into next slot
- Store fence published data before tail counter is advanced

**Dequeue:**
- Consumer checks if ring is empty
- If entry available, pointer is read from current slot
- Store fence ensures read completes before consumer counter is advanced

This keeps the fast path minimal while remaining safe for the intended concurrency pattern.

### Multiple producers (MPSC / MPMC)

For many producers, CK uses a ticketed reserve+commit pattern:

1. **Reserve** — Producer atomically claims a ticket (via CAS on `p_head`) mapping to a unique slot; returns NULL if full
2. **Write** — Producer stores pointer into reserved slot
3. **Commit** — Producer waits for `p_tail == ticket`, issues store fence, advances `p_tail`

This ensures:
- Each producer gets a unique slot (no conflicts)
- Entries visible to consumers in production order

For convenience, `ck_ring_enqueue_mpsc()` / `ck_ring_enqueue_mpmc()` do reserve→copy→commit atomically.

## Ring buffer API reference

### Initialization and state inspection

- `ck_ring_init(ring, size)` — initialize ring (size must be power of 2 ≥ 4)
- `ck_ring_capacity(ring)` — returns max slot count
- `ck_ring_size(ring)` — returns current occupancy
- `ck_ring_valid(ring)` — checks if ring state is consistent
- `ck_ring_repair(ring)` — repairs persistent ring state (only when no concurrent operations)

### SPSC (Single Producer / Single Consumer)

- `ck_ring_enqueue_spsc(ring, buffer, entry)` — enqueue pointer; returns false if full
- `ck_ring_dequeue_spsc(ring, buffer, &data)` — dequeue pointer; returns false if empty
- `ck_ring_enqueue_reserve_spsc(ring, buffer)` — reserve slot without copying
- `ck_ring_enqueue_commit_spsc(ring)` — publish reserved slot

### SPMC (Single Producer / Many Consumers)

- `ck_ring_enqueue_spmc()`, `ck_ring_enqueue_spmc_size()`
- `ck_ring_trydequeue_spmc()`, `ck_ring_dequeue_spmc()`
- Reserve and commit variants

### MPSC (Many Producers / Single Consumer)

- `ck_ring_enqueue_mpsc()`, `ck_ring_enqueue_mpsc_size()`
- `ck_ring_enqueue_reserve_mpsc()` — returns pointer, outputs ticket
- `ck_ring_enqueue_commit_mpsc(ring, ticket)` — publish by ticket
- `ck_ring_dequeue_mpsc()` — dequeue (single consumer)

### MPMC (Many Producers / Many Consumers)

- `ck_ring_enqueue_mpmc()`, `ck_ring_enqueue_mpmc_size()`
- `ck_ring_enqueue_reserve_mpmc()` — returns pointer, outputs ticket
- `ck_ring_enqueue_commit_mpmc(ring, ticket)` — publish by ticket
- `ck_ring_trydequeue_mpmc()`, `ck_ring_dequeue_mpmc()` — consumer operations

## Repository structure

- [example/ck_testing_app/](example/ck_testing_app/) — SPSC demo
- [example/ck_mpsc_app/](example/ck_mpsc_app/) — MPSC multi-threaded demo
- [include/ck_md.h](include/ck_md.h) — local CK machine-definition config
- [packages/](packages/) — CPM package wiring for CK
- [cmake/](cmake/) — vendored CPM support

## License

This repository contains example code and pulls Concurrency Kit as an external dependency. Refer to the upstream Concurrency Kit project for its license and documentation.