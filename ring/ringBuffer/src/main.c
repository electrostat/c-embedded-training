#include <stdio.h>
#include "ring_buffer.h"

CIRC_BBUF_DEF(my_circ_buf, 32);

int main(void) {
    uint8_t in_data = 0x55;
    uint8_t out_data = 0;

    if (circ_bbuf_push(&my_circ_buf, in_data) != 0) {
        printf("Out of space in CB\n");
        return -1;
    }

    if (circ_bbuf_pop(&my_circ_buf, &out_data) != 0) {
        printf("CB is empty\n");
        return -1;
    }

    printf("Push: 0x%x\n", in_data);
    printf("Pop:  0x%x\n", out_data);

    return 0;
}
