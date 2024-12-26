#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/* ------------ Start of memory management using buddy algorithm -------------*/
struct buddy
{
    size_t size;
    size_t longest[];
};
#define max(a, b) ((a) > (b) ? (a) : (b))


struct buddy *buddy_new(unsigned num_of_fragments);
void buddy_destory(struct buddy *self);
int buddy_alloc(struct buddy *self, size_t size);
void buddy_free(struct buddy *self, int offset);
int buddy_size(struct buddy *self, int offset);
void buddy_dump(struct buddy *self);
/* ------------ End of memory management using buddy algorithm -------------*/
