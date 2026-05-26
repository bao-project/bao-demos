#include <nuttx/config.h>
#include <nuttx/arch.h>

extern uint8_t _core0_heap_start[];
extern uint8_t _core0_heap_end[];
extern uint8_t _core1_heap_start[];
extern uint8_t _core1_heap_end[];
extern uint8_t _core2_heap_start[];
extern uint8_t _core2_heap_end[];
extern uint8_t _core3_heap_start[];
extern uint8_t _core3_heap_end[];

/****************************************************************************
 * Name: up_allocate_heap
 *
 * Description:
 *   This function will be called to dynamically set aside the heap region.
 *
 *   For the kernel build (CONFIG_BUILD_KERNEL=y) with both kernel- and
 *   user-space heaps (CONFIG_MM_KERNEL_HEAP=y), this function provides the
 *   size of the unprotected, user-space heap.
 *
 *   If a protected kernel-space heap is provided, the kernel heap must be
 *   allocated (and protected) by an analogous up_allocate_kheap().
 *
 ****************************************************************************/


void up_allocate_heap(void **heap_start, size_t *heap_size)
{
  int cpu = up_cpu_index();

  switch (cpu)
  {
    case 0:
      *heap_start = (void *)_core0_heap_start;
      *heap_size = (size_t)_core0_heap_end - (size_t)_core0_heap_start;
      break;

    case 1:
      *heap_start = (void *)_core1_heap_start;
      *heap_size = (size_t)_core1_heap_end - (size_t)_core1_heap_start;
      break;

    case 2:
      *heap_start = (void *)_core2_heap_start;
      *heap_size = (size_t)_core2_heap_end - (size_t)_core2_heap_start;
      break;

    case 3:
      *heap_start = (void *)_core3_heap_start;
      *heap_size = (size_t)_core3_heap_end - (size_t)_core3_heap_start;
      break;

    default:
      /* Defensive fallback for unexpected CPU IDs. */

      *heap_start = (void *)_core0_heap_start;
      *heap_size = (size_t)_core0_heap_end - (size_t)_core0_heap_start;
      break;
  }
}
