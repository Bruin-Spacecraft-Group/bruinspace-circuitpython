// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/mdns/Server.h"

#include <string.h>

#include "py/gc.h"
#include "py/runtime.h"
#include "shared-bindings/mdns/RemoteService.h"
#include "shared-bindings/wifi/__init__.h"

#include "mdns.h"

// Track whether the underlying IDF mdns has been started so that we only
// create a single inited MDNS object to CircuitPython. (After deinit, another
// could be created.)
static mdns_server_obj_t *_active_object = NULL;

// strlcpy(), but a NULL src yields an empty string instead of undefined
// behavior. The IDF leaves mdns_result_t fields NULL when a query didn't
// resolve them.
static void strlcpy_or_empty(char *dest, const char *src, size_t dest_len) {
    strlcpy(dest, src == NULL ? "" : src, dest_len);
}

void mdns_server_construct(mdns_server_obj_t *self, bool workflow) {
    if (_active_object != NULL) {
        if (self == _active_object) {
            return;
        }
        // Mark this object as deinited because another is already using MDNS.
        self->inited = false;
        return;
    }
    esp_err_t ret = mdns_init();
    if (ret != ESP_OK) {
        return;
    }
    _active_object = self;

    self->instance_name[0] = '\0';

    // Match the netif hostname set when `import wifi` was called.
    const char *netif_hostname;
    esp_netif_get_hostname(common_hal_wifi_radio_obj.netif, &netif_hostname);
    common_hal_mdns_server_set_hostname(self, netif_hostname);

    self->inited = true;

    if (workflow) {
        // Set a delegated entry to ourselves. This allows us to respond to "circuitpython.local"
        // queries as well.
        mdns_ip_addr_t our_ip;
        esp_netif_get_ip_info(common_hal_wifi_radio_obj.netif, &common_hal_wifi_radio_obj.ip_info);
        our_ip.next = NULL;
        our_ip.addr.type = ESP_IPADDR_TYPE_V4;
        our_ip.addr.u_addr.ip4 = common_hal_wifi_radio_obj.ip_info.ip;
        our_ip.addr.u_addr.ip6.addr[1] = 0;
        our_ip.addr.u_addr.ip6.addr[2] = 0;
        our_ip.addr.u_addr.ip6.addr[3] = 0;
        our_ip.addr.u_addr.ip6.zone = 0;
        mdns_delegate_hostname_add("circuitpython", &our_ip);
    }
}

void common_hal_mdns_server_construct(mdns_server_obj_t *self, mp_obj_t network_interface) {
    if (network_interface != MP_OBJ_FROM_PTR(&common_hal_wifi_radio_obj)) {
        mp_raise_ValueError(MP_ERROR_TEXT("mDNS only works with built-in WiFi"));
        return;
    }
    mdns_server_construct(self, false);
    if (common_hal_mdns_server_deinited(self)) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("mDNS already initialized"));
    }
}

void common_hal_mdns_server_deinit(mdns_server_obj_t *self) {
    if (common_hal_mdns_server_deinited(self)) {
        return;
    }
    self->inited = false;
    _active_object = NULL;
    mdns_free();
}

void mdns_server_deinit_singleton(void) {
    if (_active_object != NULL) {
        common_hal_mdns_server_deinit(_active_object);
    }
}

bool common_hal_mdns_server_deinited(mdns_server_obj_t *self) {
    return !self->inited;
}

const char *common_hal_mdns_server_get_hostname(mdns_server_obj_t *self) {
    return self->hostname;
}

void common_hal_mdns_server_set_hostname(mdns_server_obj_t *self, const char *hostname) {
    mdns_hostname_set(hostname);
    // Wait for the mdns task to set the new hostname.
    while (!mdns_hostname_exists(hostname)) {
        RUN_BACKGROUND_TASKS;
    }
    strlcpy_or_empty(self->hostname, hostname, sizeof(self->hostname));
}

const char *common_hal_mdns_server_get_instance_name(mdns_server_obj_t *self) {
    if (self->instance_name[0] == '\0') {
        return self->hostname;
    }
    return self->instance_name;
}

void common_hal_mdns_server_set_instance_name(mdns_server_obj_t *self, const char *instance_name) {
    mdns_instance_name_set(instance_name);
    strlcpy_or_empty(self->instance_name, instance_name, sizeof(self->instance_name));
}

// Copy everything we expose out of the IDF's result so that the RemoteService
// no longer references IDF-owned memory. The caller is responsible for freeing
// the result itself.
static void copy_data_into_remote_service(mdns_result_t *result, mdns_remoteservice_obj_t *out) {
    out->base.type = &mdns_remoteservice_type;
    out->port = result->port;
    out->ipv4_address = 0;
    if (result->ip_protocol == MDNS_IP_PROTOCOL_V4) {
        for (mdns_ip_addr_t *cur = result->addr; cur != NULL; cur = cur->next) {
            if (cur->addr.type == ESP_IPADDR_TYPE_V4) {
                out->ipv4_address = cur->addr.u_addr.ip4.addr;
                break;
            }
        }
    }
    strlcpy_or_empty(out->protocol, result->proto, sizeof(out->protocol));
    strlcpy_or_empty(out->service_name, result->service_type, sizeof(out->service_name));
    strlcpy_or_empty(out->instance_name, result->instance_name, sizeof(out->instance_name));
    strlcpy_or_empty(out->hostname, result->hostname, sizeof(out->hostname));
}

size_t mdns_server_find(mdns_server_obj_t *self, const char *service_type, const char *protocol,
    mp_float_t timeout, mdns_remoteservice_obj_t *out, size_t out_len) {
    mdns_search_once_t *search = mdns_query_async_new(NULL, service_type, protocol, MDNS_TYPE_PTR, timeout * 1000, 255, NULL);
    if (search == NULL) {
        return 0;
    }
    uint8_t num_results;
    mdns_result_t *results;
    while (!mdns_query_async_get_results(search, 1, &results, &num_results)) {
        RUN_BACKGROUND_TASKS;
    }
    mdns_query_async_delete(search);
    mdns_result_t *next = results;
    // Truncate if we don't have space for everything the IDF found.
    size_t added = 0;
    while (next != NULL && added < out_len) {
        copy_data_into_remote_service(next, &out[added]);
        next = next->next;
        added++;
    }
    // We've copied out everything we need, so release the IDF's copy.
    mdns_query_results_free(results);
    return num_results;
}

mp_obj_t common_hal_mdns_server_find(mdns_server_obj_t *self, const char *service_type, const char *protocol, mp_float_t timeout) {
    mdns_search_once_t *search = mdns_query_async_new(NULL, service_type, protocol, MDNS_TYPE_PTR, timeout * 1000, 255, NULL);
    if (search == NULL) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Unable to start mDNS query"));
    }
    uint8_t num_results;
    mdns_result_t *results;
    while (!mdns_query_async_get_results(search, 1, &results, &num_results)) {
        RUN_BACKGROUND_TASKS;
    }
    mdns_query_async_delete(search);
    mp_obj_tuple_t *tuple = MP_OBJ_TO_PTR(mp_obj_new_tuple(num_results, NULL));
    // The empty tuple object is shared and stored in flash so return early if
    // we got it. Without this we'll crash when trying to set len below.
    if (num_results == 0) {
        mdns_query_results_free(results);
        return MP_OBJ_FROM_PTR(tuple);
    }
    mdns_result_t *next = results;
    // Don't error if we're out of memory. Instead, truncate the tuple.
    uint8_t added = 0;
    while (next != NULL) {
        mdns_remoteservice_obj_t *service = m_malloc_maybe(sizeof(mdns_remoteservice_obj_t));
        if (service == NULL) {
            if (added == 0) {
                mdns_query_results_free(results);
                m_malloc_fail(sizeof(mdns_remoteservice_obj_t));
            }
            break;
        }
        copy_data_into_remote_service(next, service);
        next = next->next;
        tuple->items[added] = MP_OBJ_FROM_PTR(service);
        added++;
    }
    tuple->len = added;

    // We've copied out everything we need, so release the IDF's copy.
    mdns_query_results_free(results);

    return MP_OBJ_FROM_PTR(tuple);
}

void common_hal_mdns_server_advertise_service(mdns_server_obj_t *self, const char *service_type, const char *protocol, mp_int_t port, const char *txt_records[], size_t num_txt_records) {
    // Reject rather than silently drop them. See the TODO below.
    if (num_txt_records > 0) {
        mp_raise_NotImplementedError_varg(MP_ERROR_TEXT("%q"), MP_QSTR_txt_records);
    }

    if (mdns_service_exists(service_type, protocol, NULL)) {
        mdns_service_port_set(service_type, protocol, port);
    } else {
        // TODO: Add support for TXT record
        /* NOTE: The `mdns_txt_item_t *txt` argument of mdns_service_add uses a struct
         * that splits out the TXT record into keys and values, though it seems little
         * is done with those fields aside from concatenating them with an optional
         * equals sign and calculating the total length of the concatenated string.
         *
         * There should be little issue with the underlying implementation to populate
         * the mdns_txt_item_t struct with only a key containing exactly the desired TXT
         * record. As long as the underlying implementation calculates the length of the
         * key + NULL value correctly, it should work.
         *
         * Ref: RFC 6763, section 6.1:
         * > The format of each constituent string within the DNS TXT record is a single
         * > length byte, followed by 0-255 bytes of text data.
         */
        mdns_service_add(NULL, service_type, protocol, port, NULL, 0);
    }
}
