# MPSC (Many-Producer/Single-Consumer) Ring Buffer Demo

This example demonstrates the Concurrency Kit's many-producer/single-consumer ring buffer API with a small multi-threaded scenario.

## What this example does

1. Initializes a CK ring with capacity 16
2. Spawns 4 producer threads, each enqueueing 5 messages concurrently
3. Runs a consumer loop in the main thread to dequeue and print messages as they arrive
4. Waits for all producer threads to complete

The key difference from SPSC is that **multiple threads can safely call the enqueue API simultaneously** without explicit locking.

## The MPSC ring buffer

When there are multiple producers but only one consumer, CK provides the MPSC API:

### Ticketed reserve+commit pattern

Producers use a ticket-based reservation system to ensure each gets a unique slot:

1. **Reserve**: Call `ck_ring_enqueue_reserve_mpsc(&ring, &buffer, &ticket)`
   - Uses atomic CAS on `p_head` to claim a unique ticket
   - Returns a pointer to the reserved slot or NULL if ring is full
   - Retries only if contended; fails permanently if full

2. **Write**: Store your pointer into the slot returned by reserve

3. **Commit**: Call `ck_ring_enqueue_commit_mpsc(&ring, ticket)`
   - Waits until `p_tail == ticket` (previous producers have published)
   - Issues a store fence
   - Advances `p_tail`, making the entry visible to consumers

This ensures:
- Each producer writes to a distinct slot (no conflicts)
- Entries are visible to consumers in production order

### All-in-one convenience API

For simple cases, use `ck_ring_enqueue_mpsc(ring, &buffer, entry)` which does reserve→copy→commit atomically and returns false if full.

### Consumer side

- `ck_ring_dequeue_mpsc(ring, &buffer, &data)` dequeues an entry
- Works the same as SPSC; the single consumer reads entries in order

## Build and run

```sh
cd /workspaces/ck_testing
cmake -S . -B build
cmake --build build --target ck_mpsc_app
./build/example/ck_mpsc_app/ck_mpsc_app
```

## Key API functions

- `ck_ring_enqueue_reserve_mpsc(ring, buffer, &ticket)` — reserves a slot, returns pointer or NULL
- `ck_ring_enqueue_commit_mpsc(ring, ticket)` — publishes a reserved slot in order
- `ck_ring_enqueue_mpsc(ring, buffer, entry)` — reserve + copy + commit in one call
- `ck_ring_dequeue_mpsc(ring, buffer, &data)` — dequeues an entry; returns false if empty

## Example output

The output shows messages from different producers being consumed in order of production:

```
consumed: P0-msg0
consumed: P0-msg1
...
consumed: P1-msg0
...
```

This interleaving depends on producer and consumer thread scheduling.
