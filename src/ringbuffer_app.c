#include "ck_testing/ringbuffer_app.h"

#include <ck_ring.h>

#include <stdio.h>

static void
print_ring_state(const struct ck_ring *ring)
{
    printf("ring capacity: %u, current size: %u\n",
        ck_ring_capacity(ring), ck_ring_size(ring));
}

int
ck_testing_run_ringbuffer_demo(void)
{
    enum { CK_TESTING_RING_CAPACITY = 8 };
    struct ck_ring ring;
    struct ck_ring_buffer ring_buffer[CK_TESTING_RING_CAPACITY];
    const char *messages[] = {
        "first message",
        "second message",
        "third message"
    };
    void *entry = NULL;

    ck_ring_init(&ring, CK_TESTING_RING_CAPACITY);

    print_ring_state(&ring);

    for (unsigned int i = 0; i < (unsigned int)(sizeof(messages) / sizeof(messages[0])); ++i) {
        if (!ck_ring_enqueue_spsc(&ring, ring_buffer, messages[i])) {
            fprintf(stderr, "failed to enqueue message %u\n", i);
            return 1;
        }
    }

    print_ring_state(&ring);
    puts("dequeued messages:");

    while (ck_ring_dequeue_spsc(&ring, ring_buffer, &entry)) {
        printf("- %s\n", (const char *)entry);
    }

    print_ring_state(&ring);
    return 0;
}
