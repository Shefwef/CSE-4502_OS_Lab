#include <lib/debug.h>
#include "import.h"

#define PAGESIZE 4096
#define VM_USERLO 0x40000000
#define VM_USERHI 0xF0000000
#define VM_USERLO_PI (VM_USERLO / PAGESIZE)   // Address/PageSize = PageIndex
#define VM_USERHI_PI (VM_USERHI / PAGESIZE)   // Address/PageSize = PageIndex

/**
 * The initialization function for the allocation table AT.
 * It contains two major parts:
 * 1. Calculate the actual physical memory of the machine, and sets the number
 *    of physical pages (NUM_PAGES).
 * 2. Initializes the physical allocation table (AT) implemented in the MATIntro layer
 *    based on the information available in the physical memory map table.
 *    Review import.h in the current directory for the list of available
 *    getter and setter functions.
 */
void pmem_init(unsigned int mbi_addr)
{
    // TODO: Define your local variables here.
    unsigned int nps;
    unsigned int total_table_row;
    unsigned int start_address;
    unsigned int length;
    unsigned int last_address;
    unsigned int page_index;
    unsigned int perm;
    unsigned int i;

    // Calls the lower layer initialization primitive.
    // The parameter mbi_addr should not be used in the further code.
    devinit(mbi_addr);

    /**
     * Calculate the total number of physical pages provided by the hardware and
     * store it into the local variable nps.
     * Hint: Think of it as the highest address in the ranges of the memory map table,
     *       divided by the page size.
     */

    // TODO

    /*We will be given some table row informations. To calculate the number of available pages,
    we need to work with the last row to find out the highest address.*/

    total_table_row = get_size();
    // If table has zero rows => no page
    if (total_table_row == 0)
    {
        nps = 0;
    }
    else
    {
        // start addr of the last row
        start_address = get_mms(total_table_row - 1);
        // length of last row
        length = get_mml(total_table_row - 1);
        // calc last address of the last row 
        last_address = start_address + length -1;
        //
        nps = (last_address + 1) / PAGESIZE;
        /*

        start_address / PAGESIZE: This gives the number of complete pages before the start_address.
        We divide the starting address by the size of one page to figure out where the memory region starts in terms of pages.

        length / PAGESIZE: This computes how many whole pages fit in the length of the memory region.
        For example, if the length of the memory region is 16KB and the page size is 4KB, this term will return 4 pages.


        (start_address % PAGESIZE + length % PAGESIZE) / PAGESIZE: This part handles partial pages.
        The modulo (%) operation gives the remainder when dividing start_address and length by the page size.

        start_address % PAGESIZE gives the bytes that fall after the last full page at start_address.
        length % PAGESIZE gives the bytes that fall after the last full page in the length of the region.

        The sum of these two values is divided by PAGESIZE to see if they together form another full page.
        If they do, you get an additional page count.

        */
    }

    set_nps(nps); // Setting the value computed above to NUM_PAGES.

    /**
     * Initialization of the physical allocation table (AT).
     *
     * In CertiKOS, all addresses < VM_USERLO or >= VM_USERHI are reserved by the kernel.
     * That corresponds to the physical pages from 0 to VM_USERLO_PI - 1,
     * and from VM_USERHI_PI to NUM_PAGES - 1.
     * The rest of the pages that correspond to addresses [VM_USERLO, VM_USERHI)
     * can be used freely ONLY IF the entire page falls into one of the ranges in
     * the memory map table with the permission marked as usable.
     *
     * Hint:
     * 1. You have to initialize AT for all the page indices from 0 to NPS - 1.
     * 2. For the pages that are reserved by the kernel, simply set its permission to 1.
     *    Recall that the setter at_set_perm also marks the page as unallocated.
     *    Thus, you don't have to call another function to set the allocation flag.
     * 3. For the rest of the pages, explore the memory map table to set its permission
     *    accordingly. The permission should be set to 2 only if there is a range
     *    containing the entire page that is marked as available in the memory map table.
     *    Otherwise, it should be set to 0. Note that the ranges in the memory map are
     *    not aligned by pages, so it may be possible that for some pages, only some of
     *    the addresses are in a usable range. Currently, we do not utilize partial pages,
     *    so in that case, you should consider those pages as unavailable.
     */
    // TODO

    /*
    Pages that are reserved by the kernel. at_set_perm is a setter function completed in MATIntro.c.
     * The permission of the page.
     * 0: Reserved by the BIOS.
     * 1: Kernel only.
     * >1: Normal (available).
     */

     // Perm for Kernel...
    for (i = 0; i < VM_USERLO_PI; i++)
    {
        at_set_perm(i, 1);
    }
    for (i = VM_USERHI_PI; i < nps; i++)
    {
        at_set_perm(i, 1);
    }

    // Perm for bios and normal
    // Initially all for BIOS
    for(int i = VM_USERLO_PI; i < VM_USERHI_PI; ++i){
        at_set_perm(i, 0);
    }
    // Checking for Normal pages
    for(int i=0; i < total_table_row; i++){
        start_address = get_mms(i);     // current row's start address
        length = get_mml(i);            // current row's length

        if(is_usable(i)==1){
            perm = 2;   // Normal
        }else{
            perm = 0;   // BIOS
        }

        page_index = start_address/PAGESIZE;

        // page_index*PAGESIZE = current page er starting index
        // Eta current row er starting index er cheye choto hoile that's a partial page
        if(page_index*PAGESIZE < start_address) {page_index++;}

        // 
        while((page_index+1) * PAGESIZE <= start_address + length){
            if(page_index >= VM_USERLO_PI && page_index < VM_USERHI_PI){
                at_set_perm(page_index, perm);
            }
            page_index++;
            if(page_index>= VM_USERHI_PI) break;
        }
    }
}
