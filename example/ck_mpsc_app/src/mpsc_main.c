#include <ck_ring.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sched.h>

struct producer_arg {
    unsigned int id;
    struct ck_ring *ring;
    struct ck_ring_buffer *buffer;
    unsigned int count;
};

static void *
producer_thread(void *arg)
{
    struct producer_arg *a = arg;
    for (unsigned int i = 0; i < a->count; ++i) {
        char *msg;
        int n = snprintf(NULL, 0, "P%u-msg%u", a->id, i) + 1;
        msg = malloc((size_t)n);
        if (msg == NULL) {
            perror("malloc");
            continue;
        }
        snprintf(msg, (size_t)n, "P%u-msg%u", a->id, i);

        unsigned int ticket;
        void *slot = NULL;

        /* Reserve a slot; retry on contention/full */
        while ((slot = ck_ring_enqueue_reserve_mpsc(a->ring, a->buffer, &ticket)) == NULL) {
            sched_yield();
        }

        /* store the pointer into the reserved slot and commit */
        *(void **)slot = msg;
        ck_ring_enqueue_commit_mpsc(a->ring, ticket);
    }

    return NULL;
}

int
main(void)
{
    enum { RING_CAPACITY = 16 };
    const unsigned int PRODUCERS = 4;
    const unsigned int PER_PRODUCER = 5;
    struct ck_ring ring;
    struct ck_ring_buffer ring_buffer[RING_CAPACITY];
    pthread_t producers[PRODUCERS];
    unsigned int total = PRODUCERS * PER_PRODUCER;

    ck_ring_init(&ring, RING_CAPACITY);

    /* Start producers */
    struct producer_arg args[PRODUCERS];
    for (unsigned int p = 0; p < PRODUCERS; ++p) {
        args[p].id = p;
        args[p].ring = &ring;
        args[p].buffer = ring_buffer;
        args[p].count = PER_PRODUCER;
        if (pthread_create(&producers[p], NULL, producer_thread, &args[p]) != 0) {
            perror("pthread_create");
            return 1;
        }
    }

    /* Consumer: simple inline loop in main thread to collect messages */
    unsigned int consumed = 0;
    void *entry = NULL;

    while (consumed < total) {
        if (ck_ring_dequeue_mpsc(&ring, ring_buffer, &entry)) {
            printf("consumed: %s\n", (char *)entry);
            free(entry);
            consumed++;
        } else {
            /* no entry available, yield briefly */
            sched_yield();
        }
    }

    /* Wait for producers to finish */
    for (unsigned int p = 0; p < PRODUCERS; ++p) {
        pthread_join(producers[p], NULL);
    }

    return 0;
}
