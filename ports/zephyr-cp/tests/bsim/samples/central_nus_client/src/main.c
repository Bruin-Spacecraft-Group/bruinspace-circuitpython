/*
 * SPDX-FileCopyrightText: 2026 Scott Shawcroft for Adafruit Industries
 * SPDX-License-Identifier: MIT
 *
 * Zephyr BLE central that connects to a device named "CPNUS",
 * discovers Nordic UART Service (NUS), writes to the RX characteristic,
 * subscribes to TX notifications, receives data, and disconnects.
 *
 * NUS UUIDs:
 *   Service: 6E400001-B5A3-F393-E0A9-E50E24DCCA9E
 *   TX (peripheral→central, notify): 6E400003-B5A3-F393-E0A9-E50E24DCCA9E
 *   RX (central→peripheral, write):  6E400002-B5A3-F393-E0A9-E50E24DCCA9E
 */

#include <stddef.h>
#include <errno.h>

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/uuid.h>

static struct bt_conn *default_conn;
static struct bt_gatt_discover_params discover_params;

/* Real NUS 128-bit UUIDs in little-endian byte order.
 * These match the standard Nordic UART Service UUIDs:
 *   Service: 6E400001-B5A3-F393-E0A9-E50E24DCCA9E
 *   TX:      6E400003-B5A3-F393-E0A9-E50E24DCCA9E
 *   RX:      6E400002-B5A3-F393-E0A9-E50E24DCCA9E
 */
static struct bt_uuid_128 nus_service_uuid = BT_UUID_INIT_128(
    0x9E, 0xCA, 0xDC, 0x24, 0x0E, 0xE5, 0xA9, 0xE0,
    0x93, 0xF3, 0xA3, 0xB5, 0x01, 0x00, 0x40, 0x6E);
static struct bt_uuid_128 nus_tx_uuid = BT_UUID_INIT_128(
    0x9E, 0xCA, 0xDC, 0x24, 0x0E, 0xE5, 0xA9, 0xE0,
    0x93, 0xF3, 0xA3, 0xB5, 0x03, 0x00, 0x40, 0x6E);
static struct bt_uuid_128 nus_rx_uuid = BT_UUID_INIT_128(
    0x9E, 0xCA, 0xDC, 0x24, 0x0E, 0xE5, 0xA9, 0xE0,
    0x93, 0xF3, 0xA3, 0xB5, 0x02, 0x00, 0x40, 0x6E);

static uint16_t tx_handle;
static uint16_t rx_handle;
static bool tx_notify_enabled;
static bool rx_written;
static bool received;
static uint8_t received_data[32];
static uint16_t received_len;

static void start_scan(void);

/* Check if advertisement data contains a name */
static bool ad_has_name(struct net_buf_simple *ad, const char *name) {
    size_t name_len = strlen(name);

    while (ad->len > 1) {
        uint8_t field_len = net_buf_simple_pull_u8(ad);
        if (field_len == 0 || field_len > ad->len) {
            break;
        }
        uint8_t type = net_buf_simple_pull_u8(ad);
        field_len--;

        if ((type == BT_DATA_NAME_COMPLETE || type == BT_DATA_NAME_SHORTENED) &&
            field_len == name_len &&
            memcmp(ad->data, name, name_len) == 0) {
            return true;
        }
        net_buf_simple_pull(ad, field_len);
    }
    return false;
}

static void device_found(const bt_addr_le_t *addr, int8_t rssi, uint8_t type,
    struct net_buf_simple *ad) {
    if (default_conn) {
        return;
    }

    /* Only interested in connectable devices */
    if (type != BT_GAP_ADV_TYPE_ADV_IND &&
        type != BT_GAP_ADV_TYPE_ADV_DIRECT_IND) {
        return;
    }

    /* Clone ad data so we can parse it */
    struct net_buf_simple ad_copy;
    uint8_t ad_buf[64];
    size_t copy_len = ad->len < sizeof(ad_buf) ? ad->len : sizeof(ad_buf);
    memcpy(ad_buf, ad->data, copy_len);
    net_buf_simple_init_with_data(&ad_copy, ad_buf, copy_len);

    if (!ad_has_name(&ad_copy, "CPNUS")) {
        return;
    }

    char addr_str[BT_ADDR_LE_STR_LEN];
    bt_addr_le_to_str(addr, addr_str, sizeof(addr_str));
    printk("Found CPNUS device: %s (RSSI %d)\n", addr_str, rssi);

    if (bt_le_scan_stop()) {
        return;
    }

    int err = bt_conn_le_create(addr, BT_CONN_LE_CREATE_CONN,
        BT_LE_CONN_PARAM_DEFAULT, &default_conn);
    if (err) {
        printk("Create conn failed (%d)\n", err);
        start_scan();
    }
}

static void start_scan(void) {
    int err = bt_le_scan_start(BT_LE_SCAN_PASSIVE, device_found);
    if (err) {
        printk("Scanning failed to start (err %d)\n", err);
        return;
    }
    printk("Scanning started\n");
}

/* TX notification callback */
static uint8_t on_tx_notify(struct bt_conn *conn,
    struct bt_gatt_subscribe_params *params,
    const void *data, uint16_t length) {
    if (data) {
        memcpy(received_data, data, length < sizeof(received_data) ?
            length : sizeof(received_data));
        received_len = length;
        received = true;
        printk("NUS: received '");
        for (uint16_t i = 0; i < received_len; i++) {
            printk("%c", received_data[i]);
        }
        printk("'\n");

        /* Disconnect after receiving response */
        bt_conn_disconnect(conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN);
    }
    return BT_GATT_ITER_CONTINUE;
}

static struct bt_gatt_subscribe_params tx_subscribe_params;

/* Step 5: Subscribe to TX notifications */
static void subscribe_tx(struct bt_conn *conn) {
    tx_subscribe_params.notify = on_tx_notify;
    tx_subscribe_params.value = BT_GATT_CCC_NOTIFY;
    tx_subscribe_params.value_handle = tx_handle;
    tx_subscribe_params.ccc_handle = tx_handle + 1;

    int err = bt_gatt_subscribe(conn, &tx_subscribe_params);
    if (err) {
        printk("TX subscribe failed (err %d)\n", err);
        bt_conn_disconnect(conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN);
    } else {
        tx_notify_enabled = true;
        printk("TX notify enabled\n");
    }
}

/* Step 4: Write to RX characteristic (central→peripheral) using write-without-response */
static void write_rx(struct bt_conn *conn) {
    static uint8_t data[] = "Hello";

    int err = bt_gatt_write_without_response(conn, rx_handle,
        data, 5, false);
    if (err) {
        printk("RX write request failed (err %d)\n", err);
        bt_conn_disconnect(conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN);
        return;
    }
    rx_written = true;
    printk("RX write complete\n");

    /* Now subscribe to TX to receive the response */
    subscribe_tx(conn);
}

/* Step 3: Discover characteristics to find TX and RX handles */
static uint8_t discover_char_cb(struct bt_conn *conn,
    const struct bt_gatt_attr *attr,
    struct bt_gatt_discover_params *params) {
    if (!attr) {
        printk("Characteristic discovery complete\n");
        if (rx_handle == 0 || tx_handle == 0) {
            printk("NUS characteristics not found\n");
            bt_conn_disconnect(conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN);
            return BT_GATT_ITER_STOP;
        }
        if (rx_handle) {
            printk("Writing to RX...\n");
            write_rx(conn);
        }
        return BT_GATT_ITER_STOP;
    }

    struct bt_gatt_chrc *chrc = (struct bt_gatt_chrc *)attr->user_data;

    if (bt_uuid_cmp(chrc->uuid, &nus_rx_uuid.uuid) == 0) {
        rx_handle = chrc->value_handle;
        printk("Found NUS RX, handle: %u\n", rx_handle);
    } else if (bt_uuid_cmp(chrc->uuid, &nus_tx_uuid.uuid) == 0) {
        tx_handle = chrc->value_handle;
        printk("Found NUS TX, handle: %u\n", tx_handle);
    }

    return BT_GATT_ITER_CONTINUE;
}

/* Step 2: Discover characteristics within the NUS service */
static uint8_t discover_service_cb(struct bt_conn *conn,
    const struct bt_gatt_attr *attr,
    struct bt_gatt_discover_params *params) {
    if (!attr) {
        printk("Service discovery complete, NUS not found\n");
        bt_conn_disconnect(conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN);
        return BT_GATT_ITER_STOP;
    }

    printk("Found NUS, handle: %u\n", attr->handle);

    /* Discover characteristics within this service */
    discover_params.uuid = NULL;
    discover_params.start_handle = attr->handle + 1;
    discover_params.end_handle = 0xFFFF;
    discover_params.type = BT_GATT_DISCOVER_CHARACTERISTIC;
    discover_params.func = discover_char_cb;

    int err = bt_gatt_discover(conn, &discover_params);
    if (err) {
        printk("Char discovery failed (err %d)\n", err);
    }

    return BT_GATT_ITER_STOP;
}

/* Step 1: Connected - discover NUS */
static void connected(struct bt_conn *conn, uint8_t err) {
    char addr[BT_ADDR_LE_STR_LEN];
    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    if (err) {
        printk("Failed to connect to %s (%u)\n", addr, err);
        bt_conn_unref(default_conn);
        default_conn = NULL;
        start_scan();
        return;
    }

    if (conn != default_conn) {
        return;
    }

    printk("Connected: %s\n", addr);

    /* Discover NUS */
    discover_params.uuid = &nus_service_uuid.uuid;
    discover_params.start_handle = BT_ATT_FIRST_ATTRIBUTE_HANDLE;
    discover_params.end_handle = BT_ATT_LAST_ATTRIBUTE_HANDLE;
    discover_params.type = BT_GATT_DISCOVER_PRIMARY;
    discover_params.func = discover_service_cb;

    err = bt_gatt_discover(conn, &discover_params);
    if (err) {
        printk("Service discovery failed (err %d)\n", err);
    }
}

static void disconnected(struct bt_conn *conn, uint8_t reason) {
    char addr[BT_ADDR_LE_STR_LEN];

    if (conn != default_conn) {
        return;
    }

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    printk("Disconnected: %s (reason 0x%02x)\n", addr, reason);

    bt_conn_unref(default_conn);
    default_conn = NULL;
}

BT_CONN_CB_DEFINE(conn_callbacks) = {
    .connected = connected,
    .disconnected = disconnected,
};

int main(void) {
    int err = bt_enable(NULL);
    if (err) {
        printk("Bluetooth init failed (err %d)\n", err);
        return 0;
    }

    printk("Bluetooth initialized\n");
    start_scan();
    return 0;
}
