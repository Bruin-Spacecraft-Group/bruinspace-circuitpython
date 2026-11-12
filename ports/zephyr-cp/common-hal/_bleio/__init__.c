// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/runtime.h"
#include "shared-bindings/_bleio/__init__.h"
#include "shared-bindings/_bleio/Adapter.h"
#include "common-hal/_bleio/Adapter.h"
#include "common-hal/_bleio/__init__.h"
#include "bindings/zephyr_kernel/__init__.h"
#include "common-hal/_bleio/Connection.h"
#include "supervisor/shared/bluetooth/bluetooth.h"
#include "supervisor/shared/tick.h"

// The singleton _bleio.Adapter object
bleio_adapter_obj_t common_hal_bleio_adapter_obj;

void common_hal_bleio_init(void) {
    common_hal_bleio_adapter_obj.base.type = &bleio_adapter_type;
    bleio_adapter_reset(&common_hal_bleio_adapter_obj);
    bleio_connection_register_auth_callbacks();
}

void bleio_user_reset(void) {
    if (common_hal_bleio_adapter_get_enabled(&common_hal_bleio_adapter_obj)) {
        // Stop any user scanning or advertising.
        common_hal_bleio_adapter_stop_scan(&common_hal_bleio_adapter_obj);
        common_hal_bleio_adapter_stop_advertising(&common_hal_bleio_adapter_obj);
    }

    // Maybe start advertising the BLE workflow.
    supervisor_bluetooth_background();
}

void bleio_reset(void) {
    common_hal_bleio_adapter_obj.base.type = &bleio_adapter_type;
    if (!common_hal_bleio_adapter_get_enabled(&common_hal_bleio_adapter_obj)) {
        return;
    }

    supervisor_stop_bluetooth();
    bleio_adapter_reset(&common_hal_bleio_adapter_obj);
    common_hal_bleio_adapter_set_enabled(&common_hal_bleio_adapter_obj, false);
    supervisor_start_bluetooth();
}

void common_hal_bleio_gc_collect(void) {
    bleio_adapter_gc_collect(&common_hal_bleio_adapter_obj);
}

// =======================================================================
// Shared synchronous GATT helpers
// =======================================================================

typedef struct {
    uint8_t *buf;
    size_t buf_len;
    size_t read_len;
    volatile bool done;
    volatile int err;
} gattc_read_ctx_t;

typedef struct {
    volatile bool done;
    volatile int err;
} gattc_write_ctx_t;

static gattc_read_ctx_t *active_read_ctx;
static gattc_write_ctx_t *active_write_ctx;
static struct bt_gatt_read_params read_params;
static struct bt_gatt_write_params write_params;

static uint8_t on_gattc_read(struct bt_conn *conn, uint8_t err,
    struct bt_gatt_read_params *params,
    const void *data, uint16_t length) {
    gattc_read_ctx_t *ctx = active_read_ctx;
    if (ctx == NULL) {
        return BT_GATT_ITER_STOP;
    }

    if (err) {
        ctx->err = err;
        ctx->done = true;
        return BT_GATT_ITER_STOP;
    }

    if (data == NULL || length == 0) {
        ctx->done = true;
        return BT_GATT_ITER_STOP;
    }

    size_t copy_len = length;
    if (ctx->read_len + copy_len > ctx->buf_len) {
        copy_len = ctx->buf_len - ctx->read_len;
    }
    if (copy_len > 0) {
        memcpy(ctx->buf + ctx->read_len, data, copy_len);
        ctx->read_len += copy_len;
    }

    ctx->done = true;
    return BT_GATT_ITER_STOP;
}

static void on_gattc_write(struct bt_conn *conn, uint8_t err,
    struct bt_gatt_write_params *params) {
    gattc_write_ctx_t *ctx = active_write_ctx;
    if (ctx == NULL) {
        return;
    }
    ctx->err = err;
    ctx->done = true;
}

size_t bleio_gattc_read_sync(struct bt_conn *conn, uint16_t handle,
    uint8_t *buf, size_t len) {
    gattc_read_ctx_t ctx = {
        .buf = buf,
        .buf_len = len,
        .read_len = 0,
        .done = false,
        .err = 0,
    };
    active_read_ctx = &ctx;

    memset(&read_params, 0, sizeof(read_params));
    read_params.func = on_gattc_read;
    read_params.handle_count = 1;
    read_params.single.handle = handle;
    read_params.single.offset = 0;

    int err = bt_gatt_read(conn, &read_params);
    if (err != 0) {
        active_read_ctx = NULL;
        raise_zephyr_error(err);
    }

    while (!ctx.done) {
        RUN_BACKGROUND_TASKS;
    }
    active_read_ctx = NULL;

    if (ctx.err != 0) {
        raise_zephyr_error(ctx.err);
    }

    return ctx.read_len;
}

void bleio_gattc_write_sync(struct bt_conn *conn, uint16_t handle,
    const uint8_t *data, size_t len) {
    gattc_write_ctx_t ctx = {
        .done = false,
        .err = 0,
    };
    active_write_ctx = &ctx;

    memset(&write_params, 0, sizeof(write_params));
    write_params.func = on_gattc_write;
    write_params.handle = handle;
    write_params.offset = 0;
    write_params.data = data;
    write_params.length = len;

    int err = bt_gatt_write(conn, &write_params);
    if (err != 0) {
        active_write_ctx = NULL;
        raise_zephyr_error(err);
    }

    while (!ctx.done) {
        RUN_BACKGROUND_TASKS;
    }
    active_write_ctx = NULL;

    if (ctx.err != 0) {
        raise_zephyr_error(ctx.err);
    }
}
