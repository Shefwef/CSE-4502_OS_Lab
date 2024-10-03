#include <lib/gcc.h>
#include <lib/x86.h>
#include <lib/debug.h>

#include "import.h"

#define PT_PERM_UP  0
#define PT_PERM_PTU (PTE_P | PTE_W | PTE_U)

/**
 * Page directory pool for NUM_IDS processes.
 * mCertiKOS maintains one page structure for each process.
 * Each PDirPool[index] represents the page directory of the page structure
 * for the process # [index].
 * In mCertiKOS, we statically allocate page directories, and maintain the second
 * level page tables dynamically.
 * The unsigned int * type is meant to suggest that the contents of the array
 * are pointers to page tables. In reality they are actually page directory
 * entries, which are essentially pointers plus permission bits. The functions
 * in this layer will require casting between integers and pointers anyway and
 * in fact any 32-bit type is fine, so feel free to change it if it makes more
 * sense to you with a different type.
 */

// PDirPool is a 2D array that stores pointers to page tables for each process.
// Each entry in PDirPool corresponds to a page directory entry.
unsigned int *PDirPool[NUM_IDS][1024] gcc_aligned(PAGESIZE);

/**
 * In mCertiKOS, we use identity page table mappings for the kernel memory.
 * IDPTbl is an array of statically allocated identity page tables that will be
 * reused for all the kernel memory.
 * That is, in every page directory, the entries that fall into the range of
 * addresses reserved for the kernel will point to an entry in IDPTbl.
 */
unsigned int IDPTbl[1024][1024] gcc_aligned(PAGESIZE);

// Sets the CR3 register with the start address of the page structure for process # [index].
void set_pdir_base(unsigned int index)
{
    // TODO
    set_cr3(PDirPool[index]);
}

// Returns the page directory entry # [pde_index] of the process # [proc_index].
// This can be used to test whether the page directory entry is mapped.
unsigned int get_pdir_entry(unsigned int proc_index, unsigned int pde_index)
{
    // TODO
    return (unsigned int)PDirPool[proc_index][pde_index];
}

// Sets the specified page directory entry with the start address of physical
// page # [page_index].
// You should also set the permissions PTE_P, PTE_W, and PTE_U.
void set_pdir_entry(unsigned int proc_index, unsigned int pde_index,
                    unsigned int page_index)
{
    // TODO
    // calculates the base address of page table index
    // also attaches the permission
    unsigned int value = (page_index << 12) | PT_PERM_PTU;

    // then stores the value at PDirPool
    PDirPool[proc_index][pde_index] = (char *)value;
}

// Sets the page directory entry # [pde_index] for the process # [proc_index]
// with the initial address of page directory # [pde_index] in IDPTbl.
// You should also set the permissions PTE_P, PTE_W, and PTE_U.
// This will be used to map a page directory entry to an identity page table.
void set_pdir_entry_identity(unsigned int proc_index, unsigned int pde_index)
{
    // TODO
    unsigned int value = (unsigned int)IDPTbl[pde_index];
    
    //uses the bitwise OR (|) operator to add permission bits
    value |= PT_PERM_PTU;
    
    // sets the pde_index entry of the page directory for process proc_index in the PDirPool array
    PDirPool[proc_index][pde_index] = (char *)value;
}

// Removes the specified page directory entry (sets the page directory entry to 0).
// Don't forget to cast the value to (unsigned int *).
void rmv_pdir_entry(unsigned int proc_index, unsigned int pde_index)
{
    // TODO
    // removes a specific page directory entry by setting it to 0
    PDirPool[proc_index][pde_index] = (char *)0x00000000;
}

// Returns the specified page table entry.
// Do not forget that the permission info is also stored in the page directory entries.
unsigned int get_ptbl_entry(unsigned int proc_index, unsigned int pde_index,
                            unsigned int pte_index)
{
    // TODO
    // get the page table entry address for the given process and index
    unsigned int pte_addr = (unsigned int )PDirPool[proc_index][pde_index];
    pte_addr &= 0xfffff000;         // remove perm bits
    pte_addr += pte_index << 2;     // page_index is shifted left by 12 bit to convert to base address
    return *(unsigned int *)pte_addr;
}

// Sets the specified page table entry with the start address of physical page # [page_index]
// You should also set the given permission.
void set_ptbl_entry(unsigned int proc_index, unsigned int pde_index,
                    unsigned int pte_index, unsigned int page_index,
                    unsigned int perm)
{
    // TODO
    // get the page table entry address for the given process and index
    unsigned int pte_addr =  (unsigned int )PDirPool[proc_index][pde_index];

    // performs a bitwise AND operation between pte_addr and 0xfffff000.
    // The purpose is to clear the lower 12 bits of pte_addr (permission bits)
    pte_addr &= 0xfffff000;

    // pte_index << 2 shifts pte_index left by 2 bits,
    // which is equivalent to multiplying pte_index by 4 (size of each entry)
    pte_addr += pte_index << 2;   // offset to the desired page table entry is added to the base address of the page table

    // creates a pointer pte that points to the page table entry address we just calculated
    unsigned int* pte = (unsigned int *)pte_addr;
    // clears the current value of the page table entry
    *pte &= 0x00000000;

    // The page_index is shifted left by 12 bits (page_index << 12),
    // which converts the page index to its base address
    // resulting value is assigned to *pte
    *pte = page_index << 12;

    // adds permission bits to the page table entry
    *pte |= (perm & 0x00000fff);
}

// Sets up the specified page table entry in IDPTbl as the identity map.
// You should also set the given permission.
void set_ptbl_entry_identity(unsigned int pde_index, unsigned int pte_index,
                             unsigned int perm)
{
    // TODO
    // calculates this physical address
    // pde_index is first shifted by 10 to make 16-bit
    // then pte_index is added to it
    // then shifted by 12 to 
    IDPTbl[pde_index][pte_index] = ((pde_index << 10) + pte_index) << 12;
    IDPTbl[pde_index][pte_index] |= perm;
}

// Sets the specified page table entry to 0.
void rmv_ptbl_entry(unsigned int proc_index, unsigned int pde_index,
                    unsigned int pte_index)
{
    // TODO
}
