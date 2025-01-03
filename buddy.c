/* ------------ Start of memory management using buddy algorithm -------------*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "buddy.h"



static inline int left_child(int index)
{
    return ((index << 1) + 1);
}

static inline int right_child(int index)
{
    return ((index << 1) + 2);
}

static inline int parent(int index)
{
    return (((index + 1) >> 1) - 1);
}

static inline bool is_power_of_2(int index)
{
    return !(index & (index - 1));
}

static void *b_malloc(size_t size)
{
    void *tmp = NULL;

    tmp = malloc(size);
    if (tmp == NULL)
    {
        fprintf(stderr, "my_malloc: not enough memory, quit\n");
        exit(EXIT_FAILURE);
    }

    return tmp;
}

static void b_free(void *addr)
{
    free(addr);
}

static inline unsigned next_power_of_2(unsigned size)
{
    size -= 1;
    size |= (size >> 1);
    size |= (size >> 2);
    size |= (size >> 4);
    size |= (size >> 8);
    size |= (size >> 16);
    return size + 1;
}

struct buddy *buddy_new(unsigned num_of_fragments)
{
    struct buddy *self = NULL;
    size_t node_size;

    int i;

    if (num_of_fragments < 1 || !is_power_of_2(num_of_fragments))
    {
        return NULL;
    }

    self = (struct buddy *)b_malloc(sizeof(struct buddy) + 2 * num_of_fragments * sizeof(size_t));
    self->size = num_of_fragments;
    node_size = num_of_fragments * 2;

    int iter_end = num_of_fragments * 2 - 1;
    for (i = 0; i < iter_end; i++)
    {
        if (is_power_of_2(i + 1))
        {
            node_size >>= 1;
        }
        self->longest[i] = node_size;
    }

    return self;
}

void buddy_destory(struct buddy *self)
{
    b_free(self);
}

unsigned choose_better_child(struct buddy *self, unsigned index, size_t size)
{
    struct compound
    {
        size_t size;
        unsigned index;
    } children[2];
    children[0].index = left_child(index);
    children[0].size = self->longest[children[0].index];
    children[1].index = right_child(index);
    children[1].size = self->longest[children[1].index];

    int min_idx = (children[0].size <= children[1].size) ? 0 : 1;

    if (size > children[min_idx].size)
    {
        min_idx = 1 - min_idx;
    }

    return children[min_idx].index;
}

int buddy_alloc(struct buddy *self, size_t size)
{
    if (self == NULL || self->size < size)
    {
        return -1;
    }
    size = next_power_of_2(size);

    unsigned index = 0;
    if (self->longest[index] < size)
    {
        return -1;
    }

    unsigned node_size = 0;
    for (node_size = self->size; node_size != size; node_size >>= 1)
    {
        index = choose_better_child(self, index, size);
    }

    self->longest[index] = 0;
    int offset = (index + 1) * node_size - self->size;

    while (index)
    {
        index = parent(index);
        self->longest[index] =
            max(self->longest[left_child(index)],
                self->longest[right_child(index)]);
    }

    return offset;
}

void buddy_free(struct buddy *self, int offset)
{
    if (self == NULL || offset < 0 || offset > self->size)
    {
        return;
    }

    size_t node_size;
    unsigned index;

    node_size = 1;
    index = offset + self->size - 1;

    for (; self->longest[index] != 0; index = parent(index))
    {
        node_size <<= 1;

        if (index == 0)
        {
            break;
        }
    }

    self->longest[index] = node_size;

    while (index)
    {
        index = parent(index);
        node_size <<= 1;

        size_t left_longest = self->longest[left_child(index)];
        size_t right_longest = self->longest[right_child(index)];

        if (left_longest + right_longest == node_size)
        {
            self->longest[index] = node_size;
        }
        else
        {
            self->longest[index] = max(left_longest, right_longest);
        }
    }
}

int buddy_size(struct buddy *self, int offset)
{
    unsigned node_size = 1;
    unsigned index = offset + self->size - 1;

    for (; self->longest[index]; index = parent(index))
    {
        node_size <<= 1;
    }

    return node_size;
}

void buddy_dump(struct buddy *self)
{
    int len = self->size << 1;
    int max_col = self->size << 1;
    int level = 0;
    int i, j;

    char cs[] = {'/', '\\'};
    int idx = 0;
    char c;

    for (i = 0, max_col = len, level = 0; i < len - 1; i++)
    {
        if (is_power_of_2(i + 1))
        {
            max_col >>= 1;
            level++;
            idx = 0;
            printf("\n%d(%.2d): ", level, max_col);
        }

        printf("%*ld", max_col, self->longest[i]);
    }

    for (i = 0, max_col = len, level = 0; i < len - 1; i++)
    {
        if (is_power_of_2(i + 1))
        {
            max_col >>= 1;
            level++;
            idx = 0;
            printf("\n%d(%.2d): ", level, max_col);
        }

        if (self->longest[i] > 0)
        {
            c = '-';
        }
        else
        {
            c = cs[idx];
            idx ^= 0x1;
        }

        for (j = 0; j < max_col; j++)
        {
            printf("%c", c);
        }
    }
    printf("\n");
}
/* ------------ End of memory management using buddy algorithm -------------*/
