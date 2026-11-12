// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "supervisor/port.h"

#include <string.h>

#include "py/mphal.h"
#include "py/runtime.h"
#include "py/gc.h"

#include "lib/tlsf/tlsf.h"

static tlsf_t heap;

MP_WEAK void reset_port_early(void) {
}

MP_WEAK void port_wake_main_task(void) {
}

MP_WEAK void port_wake_main_task_from_isr(void) {
}

MP_WEAK void port_task_yield(void) {
}

MP_WEAK void port_task_sleep_ms(uint32_t msecs) {
    mp_hal_delay_ms(msecs);
}

MP_WEAK void port_boot_info(void) {
}

MP_WEAK void port_heap_init(void) {
    uint32_t *heap_bottom = port_heap_get_bottom();
    uint32_t *heap_top = port_heap_get_top();
    size_t size = (heap_top - heap_bottom) * sizeof(uint32_t);
    heap = tlsf_create_with_pool(heap_bottom, size, size);
}

MP_WEAK void *port_malloc(size_t size, bool dma_capable) {
    void *block = tlsf_malloc(heap, size);
    return block;
}

MP_WEAK void port_free(void *ptr) {
    tlsf_free(heap, ptr);
}

// Safely free an object that may have been allocated from either the GC heap or port heap.
// If the pointer is on the GC heap, it will be freed by the GC automatically, so we do nothing.
// If the pointer is not on the GC heap (i.e., allocated with port_malloc), we free it with port_free.
// This is safe to call during shutdown when GC may not be available.
void circuitpy_free_obj(mp_obj_t obj) {
    if (obj == mp_const_none) {
        return;
    }
    void *ptr = MP_OBJ_TO_PTR(obj);
    if (gc_alloc_possible() && gc_ptr_on_heap(ptr)) {
        gc_free(ptr);
    } else {
        port_free(ptr);
    }
}

MP_WEAK void *port_realloc(void *ptr, size_t size, bool dma_capable) {
    return tlsf_realloc(heap, ptr, size);
}

static bool max_size_walker(void *ptr, size_t size, int used, void *user) {
    size_t *max_size = (size_t *)user;
    if (!used && *max_size < size) {
        *max_size = size;
    }
    return true;
}

MP_WEAK size_t port_heap_get_largest_free_size(void) {
    size_t max_size = 0;
    tlsf_walk_pool(tlsf_get_pool(heap), max_size_walker, &max_size);
    // IDF does this. Not sure why.
    return tlsf_fit_size(heap, max_size);
}
