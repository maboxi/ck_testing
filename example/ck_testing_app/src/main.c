#include <ck_ring.h>
#include <stdio.h>

int
main(void)
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

    printf("ring capacity: %u, current size: %u\n",
        ck_ring_capacity(&ring), ck_ring_size(&ring));

    for (unsigned int i = 0; i < (unsigned int)(sizeof(messages) / sizeof(messages[0])); ++i) {
        if (!ck_ring_enqueue_spsc(&ring, ring_buffer, messages[i])) {
            fprintf(stderr, "failed to enqueue message %u\n", i);
            return 1;
        }
    }

    printf("ring capacity: %u, current size: %u\n",
        ck_ring_capacity(&ring), ck_ring_size(&ring));
    puts("dequeued messages:");

    while (ck_ring_dequeue_spsc(&ring, ring_buffer, &entry)) {
        printf("- %s\n", (const char *)entry);
    }

    printf("ring capacity: %u, current size: %u\n",
        ck_ring_capacity(&ring), ck_ring_size(&ring));
    return 0;
}
