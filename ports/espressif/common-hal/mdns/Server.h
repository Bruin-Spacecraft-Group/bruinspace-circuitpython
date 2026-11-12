// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    // Store copies rather than the caller's pointers. The setters are handed
    // GC-heap strings, which are recycled when the VM resets while this object
    // (and the web workflow's static one) lives on.
    char hostname[64]; // RFC 6762 Appendix A - 63 bytes for label + 1 for NUL
    char instance_name[64]; // RFC 6763 Section 7.2 - 63 bytes + 1 for NUL
    // Track if this object owns access to the underlying MDNS service.
    bool inited;
} mdns_server_obj_t;

void mdns_server_deinit_singleton(void);
