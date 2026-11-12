// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "bindings/hostnetwork/HostNetwork.h"

hostnetwork_hostnetwork_obj_t common_hal_hostnetwork_obj = {
    .base = { &hostnetwork_hostnetwork_type },
};

void common_hal_hostnetwork_hostnetwork_construct(hostnetwork_hostnetwork_obj_t *self) {
    (void)self;
}
