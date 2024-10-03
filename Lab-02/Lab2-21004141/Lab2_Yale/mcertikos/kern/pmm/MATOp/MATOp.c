#include <lib/debug.h>
#include "import.h"

#define PAGESIZE 4096
#define VM_USERLO 0x40000000
#define VM_USERHI 0xF0000000
#define VM_USERLO_PI (VM_USERLO / PAGESIZE)
#define VM_USERHI_PI (VM_USERHI / PAGESIZE)

// set a next pointer to remember the last allocated page index
static unsigned int pointer = VM_USERLO_PI;

/**
 * Allocate a physical page.
 *
 * 1. First, implement a naive page allocator that scans the allocation table (AT)
 *    using the functions defined in import.h to find the first unallocated page
 *    with normal permissions.
 *    (Q: Do you have to scan the allocation table from index 0? Recall how you have
 *    initialized the table in pmem_init.)
 *    Then mark the page as allocated in the allocation table and return the page
 *    index of the page found. In the case when there is no available page found,
 *    return 0.
 * 2. Optimize the code using memoization so that you do not have to
 *    scan the allocation table from scratch every time.
 */
unsigned int palloc()
{
    // no available physical pages
    if (get_nps() == 0)
    {
        return 0;
    }

    // first record the current value of next
    unsigned int i = pointer;
    while (1)
    {
        if (at_is_norm(i) == 1 && at_is_allocated(i) == 0)
        {
            // mark the page as allocated
            at_set_allocated(i, 1);
            pointer = i + 1;
            return i;
        }
        i++;
        
        if (i == VM_USERHI_PI)
        {
            i = VM_USERLO_PI;
        }

        if (i == pointer)
        {
            return 0;
        }
    }
    // all pages are allocated
    return 0;
}

/**
 * Free a physical page.
 *
 * This function marks the page with given index as unallocated
 * in the allocation table.
 *
 * Hint: Simple.
 */
void pfree(unsigned int pfree_index)
{
    // TODO
    at_set_allocated(pfree_index, 0);
}