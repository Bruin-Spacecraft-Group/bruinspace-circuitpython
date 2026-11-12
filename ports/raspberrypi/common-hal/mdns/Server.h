// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

#include "lwip/apps/mdns_opts.h"

#define MDNS_MAX_TXT_RECORDS 32

typedef struct {
    mp_obj_base_t base;
    // Store copies rather than the caller's pointers. The setters are handed
    // GC-heap strings, which are recycled when the VM resets while this object
    // (and the web workflow's static one) lives on.
    char hostname[64]; // RFC 6762 Appendix A - 63 bytes for label + 1 for NUL
    char instance_name[64]; // RFC 6763 Section 7.2 - 63 bytes + 1 for NUL
    const char *service_type[MDNS_MAX_SERVICES];
    size_t num_txt_records;
    // Owned copies of the TXT records, packed NUL-separated into txt_storage,
    // which lives on the GC heap. See the warning on assign_txt_records().
    char *txt_storage;
    const char *txt_records[MDNS_MAX_TXT_RECORDS];
    // Track if this object owns access to the underlying MDNS service.
    bool inited;
} mdns_server_obj_t;
