# SPSC (Single-Producer/Single-Consumer) Ring Buffer Demo

This example demonstrates the Concurrency Kit's simple and efficient single-producer/single-consumer ring buffer API.

## What this example does

1. Initializes a CK ring with capacity 8
2. Prints the ring capacity and current occupancy
3. Enqueues three string pointers using `ck_ring_enqueue_spsc()`
4. Dequeues and prints the messages using `ck_ring_dequeue_spsc()`
5. Prints the final ring state

## The SPSC ring buffer

When there is at most one producer and one consumer, CK provides the optimized SPSC API:

### Enqueue (producer side)

- `ck_ring_enqueue_spsc()` checks if the ring is full
- If space is available, the pointer is copied into the next slot
- A store fence publishes the data before the tail counter is advanced

### Dequeue (consumer side)

- `ck_ring_dequeue_spsc()` checks if the ring is empty
- If an entry is available, the pointer is read from the current slot
- A store fence ensures the read completes before the consumer counter is advanced

## Build and run

```sh
cd /workspaces/ck_testing
cmake -S . -B build
cmake --build build --target ck_testing_app
./build/example/ck_testing_app/ck_testing_app
```

## Key API functions

- `ck_ring_init(ring, size)` — initializes the ring (size must be power of two ≥ 4)
- `ck_ring_capacity(ring)` — returns configured slot count
- `ck_ring_size(ring)` — returns current occupancy
- `ck_ring_enqueue_spsc(ring, buffer, entry)` — enqueues a pointer; returns false if full
- `ck_ring_dequeue_spsc(ring, buffer, &data)` — dequeues a pointer; returns false if empty
- `ck_ring_enqueue_reserve_spsc(ring, buffer)` — reserves a slot without copying
- `ck_ring_enqueue_commit_spsc(ring)` — publishes a reserved slot
