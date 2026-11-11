// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <string.h>

#include "py/runtime.h"
#include "shared-bindings/_bleio/__init__.h"
#include "shared-bindings/_bleio/Descriptor.h"
#include "shared-bindings/_bleio/Characteristic.h"
#include "common-hal/_bleio/__init__.h"
#include "common-hal/_bleio/Characteristic.h"
#include "common-hal/_bleio/Connection.h"
#include "supervisor/port_heap.h"

void common_hal_bleio_descriptor_construct(bleio_descriptor_obj_t *self,
    bleio_characteristic_obj_t *characteristic, bleio_uuid_obj_t *uuid,
    bleio_attribute_security_mode_t read_perm,
    bleio_attribute_security_mode_t write_perm,
    mp_int_t max_length, bool fixed_length,
    mp_buffer_info_t *initial_value_bufinfo) {

    self->characteristic = characteristic;
    self->uuid = uuid;
    self->read_perm = read_perm;
    self->write_perm = write_perm;
    self->max_length = max_length;
    self->fixed_length = fixed_length;

    // Allocate value buffer
    self->value = m_malloc(max_length);
    memset(self->value, 0, max_length);
    self->value_length = 0;

    // Copy initial value if provided
    if (initial_value_bufinfo != NULL && initial_value_bufinfo->len > 0) {
        size_t len = initial_value_bufinfo->len;
        if (len > (size_t)max_length) {
            len = max_length;
        }
        memcpy(self->value, initial_value_bufinfo->buf, len);
        self->value_length = len;
    }

    // Convert UUID to Zephyr format
    bleio_uuid_to_zephyr(uuid, &self->zephyr_uuid);
}

bleio_uuid_obj_t *common_hal_bleio_descriptor_get_uuid(bleio_descriptor_obj_t *self) {
    return self->uuid;
}

bleio_characteristic_obj_t *common_hal_bleio_descriptor_get_characteristic(bleio_descriptor_obj_t *self) {
    return (bleio_characteristic_obj_t *)self->characteristic;
}

static bool descriptor_is_remote(bleio_descriptor_obj_t *self) {
    return self->characteristic != NULL &&
           self->characteristic->service != NULL &&
           self->characteristic->service->is_remote;
}

size_t common_hal_bleio_descriptor_get_value(bleio_descriptor_obj_t *self, uint8_t *buf, size_t len) {
    if (descriptor_is_remote(self)) {
        bleio_connection_obj_t *connection =
            MP_OBJ_TO_PTR(self->characteristic->service->connection);
        if (connection == NULL || connection->connection == NULL ||
            connection->connection->conn == NULL) {
            mp_raise_bleio_BluetoothError(MP_ERROR_TEXT("Not connected"));
        }

        return bleio_gattc_read_sync(connection->connection->conn,
            self->handle, buf, len);
    }

    // Local descriptor
    size_t copy_len = self->value_length;
    if (copy_len > len) {
        copy_len = len;
    }
    memcpy(buf, self->value, copy_len);
    return copy_len;
}

void common_hal_bleio_descriptor_set_value(bleio_descriptor_obj_t *self, mp_buffer_info_t *bufinfo) {
    if (descriptor_is_remote(self)) {
        bleio_connection_obj_t *connection =
            MP_OBJ_TO_PTR(self->characteristic->service->connection);
        if (connection == NULL || connection->connection == NULL ||
            connection->connection->conn == NULL) {
            mp_raise_bleio_BluetoothError(MP_ERROR_TEXT("Not connected"));
        }

        bleio_gattc_write_sync(connection->connection->conn,
            self->handle, bufinfo->buf, bufinfo->len);
        return;
    }

    // Local descriptor
    size_t len = bufinfo->len;
    if (len > self->max_length) {
        len = self->max_length;
    }
    memcpy(self->value, bufinfo->buf, len);
    self->value_length = len;
}
