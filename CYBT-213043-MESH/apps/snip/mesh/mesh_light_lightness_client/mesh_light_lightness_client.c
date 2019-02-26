/*
* Copyright 2019, Cypress Semiconductor Corporation or a subsidiary of
* Cypress Semiconductor Corporation. All Rights Reserved.
*
* This software, including source code, documentation and related
* materials ("Software"), is owned by Cypress Semiconductor Corporation
* or one of its subsidiaries ("Cypress") and is protected by and subject to
* worldwide patent protection (United States and foreign),
* United States copyright laws and international treaty provisions.
* Therefore, you may use this Software only as provided in the license
* agreement accompanying the software package from which you
* obtained this Software ("EULA").
* If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
* non-transferable license to copy, modify, and compile the Software
* source code solely for use in connection with Cypress's
* integrated circuit products. Any reproduction, modification, translation,
* compilation, or representation of this Software except as specified
* above is prohibited without the express written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
* reserves the right to make changes to the Software without notice. Cypress
* does not assume any liability arising out of the application or use of the
* Software or any product or circuit described in the Software. Cypress does
* not authorize its products for use in any products where a malfunction or
* failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death ("High Risk Product"). By
* including Cypress's product in a High Risk Product, the manufacturer
* of such system or application assumes all risk of such use and in doing
* so agrees to indemnify Cypress against all liability.
*/

/** @file
 *
 *
 * This file shows how to create a device which implements mesh on/off client model.
 */
#include "wiced_bt_ble.h"
#include "wiced_bt_gatt.h"
#include "wiced_bt_mesh_models.h"
#include "wiced_bt_trace.h"
#include "wiced_bt_mesh_app.h"

#ifdef HCI_CONTROL
#include "wiced_transport.h"
#include "hci_control_api.h"
#endif

/******************************************************
 *          Constants
 ******************************************************/
#define MESH_PID                0x300E
#define MESH_VID                0x0001
#define MESH_FWID               0x300E000101010001
#define MESH_CACHE_REPLAY_SIZE  0x0008

/******************************************************
 *          Structures
 ******************************************************/

/******************************************************
 *          Function Prototypes
 ******************************************************/
static void mesh_app_init(wiced_bool_t is_provisioned);
static uint32_t mesh_app_proc_rx_cmd(uint16_t opcode, uint8_t *p_data, uint32_t length);
static void mesh_light_lightness_client_message_handler(uint16_t event, wiced_bt_mesh_event_t *p_event, void *p_data);
static void mesh_light_lightness_client_get(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint32_t length);
static void mesh_light_lightness_client_set(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint32_t length);
static void mesh_light_lightness_client_linear_get(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint32_t length);
static void mesh_light_lightness_client_linear_set(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint32_t length);
static void mesh_light_lightness_client_last_get(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint32_t length);
static void mesh_light_lightness_client_default_get(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint32_t length);
static void mesh_light_lightness_client_default_set(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint32_t length);
static void mesh_light_lightness_client_range_get(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint32_t length);
static void mesh_light_lightness_client_range_set(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint32_t length);
static void mesh_light_lightness_hci_event_send(wiced_bt_mesh_hci_event_t *p_hci_event, wiced_bt_mesh_light_lightness_status_data_t *p_data);
static void mesh_light_lightness_linear_hci_event_send(wiced_bt_mesh_hci_event_t *p_hci_event, wiced_bt_mesh_light_lightness_status_data_t *p_data);
static void mesh_light_lightness_last_hci_event_send(wiced_bt_mesh_hci_event_t *p_hci_event, wiced_bt_mesh_light_lightness_last_data_t *p_data);
static void mesh_light_lightness_default_hci_event_send(wiced_bt_mesh_hci_event_t *p_hci_event, wiced_bt_mesh_light_lightness_default_data_t *p_data);
static void mesh_light_lightness_range_hci_event_status_send(wiced_bt_mesh_hci_event_t *p_hci_event, wiced_bt_mesh_light_lightness_range_status_data_t *p_data);

/******************************************************
 *          Variables Definitions
 ******************************************************/

char   *mesh_dev_name                                                      = "Dimmer";
uint8_t mesh_appearance[WICED_BT_MESH_PROPERTY_LEN_DEVICE_APPEARANCE]      = { BIT16_TO_8(APPEARANCE_CONTROL_DEVICE_SLIDER) };
uint8_t mesh_mfr_name[WICED_BT_MESH_PROPERTY_LEN_DEVICE_MANUFACTURER_NAME] = { 'C', 'y', 'p', 'r', 'e', 's', 's', 0 };
uint8_t mesh_model_num[WICED_BT_MESH_PROPERTY_LEN_DEVICE_MODEL_NUMBER]     = { '1', '2', '3', '4', 0, 0, 0, 0 };
uint8_t mesh_system_id[8]                                                  = { 0xbb, 0xb8, 0xa1, 0x80, 0x5f, 0x9f, 0x91, 0x71 };

wiced_bt_mesh_core_config_model_t   mesh_element1_models[] =
{
    WICED_BT_MESH_DEVICE,
    WICED_BT_MESH_MODEL_LIGHT_LIGHTNESS_CLIENT,
};
#define MESH_APP_NUM_MODELS  (sizeof(mesh_element1_models) / sizeof(wiced_bt_mesh_core_config_model_t))

#define MESH_LIGHT_LIGHTNESS_CLIENT_ELEMENT_INDEX   0

wiced_bt_mesh_core_config_element_t mesh_elements[] =
{
    {
        .location = MESH_ELEM_LOC_MAIN,                                 // location description as defined in the GATT Bluetooth Namespace Descriptors section of the Bluetooth SIG Assigned Numbers
        .default_transition_time = MESH_DEFAULT_TRANSITION_TIME_IN_MS,  // Default transition time for models of the element in milliseconds
        .onpowerup_state = WICED_BT_MESH_ON_POWER_UP_STATE_RESTORE,     // Default element behavior on power up
        .default_level = 0,                                             // Default value of the variable controlled on this element (for example power, lightness, temperature, hue...)
        .range_min = 1,                                                 // Minimum value of the variable controlled on this element (for example power, lightness, temperature, hue...)
        .range_max = 0xffff,                                            // Maximum value of the variable controlled on this element (for example power, lightness, temperature, hue...)
        .move_rollover = 0,                                             // If true when level gets to range_max during move operation, it switches to min, otherwise move stops.
        .properties_num = 0,                                            // Number of properties in the array models
        .properties = NULL,                                             // Array of properties in the element.
        .sensors_num = 0,                                               // Number of sensors in the sensor array
        .sensors = NULL,                                                // Array of sensors of that element
        .models_num = MESH_APP_NUM_MODELS,                              // Number of models in the array models
        .models = mesh_element1_models,                                 // Array of models located in that element. Model data is defined by structure wiced_bt_mesh_core_config_model_t
    },
};

wiced_bt_mesh_core_config_t  mesh_config =
{
    .company_id         = MESH_COMPANY_ID_CYPRESS,                  // Company identifier assigned by the Bluetooth SIG
    .product_id         = MESH_PID,                                 // Vendor-assigned product identifier
    .vendor_id          = MESH_VID,                                 // Vendor-assigned product version identifier
    .firmware_id        = MESH_FWID,                                // Vendor-assigned firmware version identifier
    .replay_cache_size  = MESH_CACHE_REPLAY_SIZE,                   // Number of replay protection entries, i.e. maximum number of mesh devices that can send application messages to this device.
#if defined(LOW_POWER_NODE) && (LOW_POWER_NODE == 1)
    .features           = WICED_BT_MESH_CORE_FEATURE_BIT_LOW_POWER, // A bit field indicating the device features. In Low Power mode no Relay, no Proxy and no Friend
    .friend_cfg         =                                           // Empty Configuration of the Friend Feature
    {
        .receive_window = 0,                                        // Receive Window value in milliseconds supported by the Friend node.
        .cache_buf_len  = 0                                         // Length of the buffer for the cache
    },
    .low_power          =                                           // Configuration of the Low Power Feature
    {
        .rssi_factor           = 2,                                 // contribution of the RSSI measured by the Friend node used in Friend Offer Delay calculations.
        .receive_window_factor = 2,                                 // contribution of the supported Receive Window used in Friend Offer Delay calculations.
        .min_cache_size_log    = 3,                                 // minimum number of messages that the Friend node can store in its Friend Cache.
        .receive_delay         = 100,                               // Receive delay in 1 ms units to be requested by the Low Power node.
        .poll_timeout          = 36000                              // Poll timeout in 100ms units to be requested by the Low Power node.
    },
#else
    .features = WICED_BT_MESH_CORE_FEATURE_BIT_FRIEND | WICED_BT_MESH_CORE_FEATURE_BIT_RELAY | WICED_BT_MESH_CORE_FEATURE_BIT_GATT_PROXY_SERVER,   // Supports Friend, Relay and GATT Proxy
    .friend_cfg         =                                           // Configuration of the Friend Feature(Receive Window in Ms, messages cache)
    {
        .receive_window        = 200,
        .cache_buf_len         = 300                                // Length of the buffer for the cache
    },
    .low_power          =                                           // Configuration of the Low Power Feature
    {
        .rssi_factor           = 0,                                 // contribution of the RSSI measured by the Friend node used in Friend Offer Delay calculations.
        .receive_window_factor = 0,                                 // contribution of the supported Receive Window used in Friend Offer Delay calculations.
        .min_cache_size_log    = 0,                                 // minimum number of messages that the Friend node can store in its Friend Cache.
        .receive_delay         = 0,                                 // Receive delay in 1 ms units to be requested by the Low Power node.
        .poll_timeout          = 0                                  // Poll timeout in 100ms units to be requested by the Low Power node.
    },
#endif
    .gatt_client_only          = WICED_FALSE,                       // Can connect to mesh over GATT or ADV
    .elements_num  = (uint8_t)(sizeof(mesh_elements) / sizeof(mesh_elements[0])),   // number of elements on this device
    .elements      = mesh_elements                                  // Array of elements for this device
};

/*
 * Mesh application library will call into application functions if provided by the application.
 */
wiced_bt_mesh_app_func_table_t wiced_bt_mesh_app_func_table =
{
    mesh_app_init,          // application initialization
    NULL,                   // Default SDK platform button processing
    NULL,                   // GATT connection status
    NULL,                   // attention processing
    NULL,                   // notify period set
    mesh_app_proc_rx_cmd,   // WICED HCI command
    NULL,                   // LPN sleep
    NULL                    // factory reset
};

/******************************************************
 *               Function Definitions
 ******************************************************/
void mesh_app_init(wiced_bool_t is_provisioned)
{
#if 0
    extern uint8_t wiced_bt_mesh_model_trace_enabled;
    wiced_bt_mesh_model_trace_enabled = WICED_TRUE;
#endif
    wiced_bt_mesh_model_light_lightness_client_init(MESH_LIGHT_LIGHTNESS_CLIENT_ELEMENT_INDEX, mesh_light_lightness_client_message_handler, is_provisioned);
}

/*
 * Process event received from the Light Lightness Server.
 */
void mesh_light_lightness_client_message_handler(uint16_t event, wiced_bt_mesh_event_t *p_event, void *p_data)
{
    wiced_bt_mesh_light_lightness_status_data_t *p_status_data;
    wiced_bt_mesh_light_lightness_last_data_t *p_last_status_data;
    wiced_bt_mesh_light_lightness_default_data_t *p_default_status_data;
    wiced_bt_mesh_light_lightness_range_status_data_t *p_range_status_data;
#if defined HCI_CONTROL
    wiced_bt_mesh_hci_event_t *p_hci_event;
#endif

    WICED_BT_TRACE("lightness clt msg:%d\n", event);

    switch (event)
    {
    case WICED_BT_MESH_TX_COMPLETE:
        WICED_BT_TRACE("tx complete status:%d\n", p_event->tx_status);
#if defined HCI_CONTROL
        if ((p_hci_event = wiced_bt_mesh_create_hci_event(p_event)) != NULL)
            wiced_bt_mesh_send_hci_tx_complete(p_hci_event, p_event);
#endif
        break;

    case WICED_BT_MESH_LIGHT_LIGHTNESS_STATUS:
        p_status_data = (wiced_bt_mesh_light_lightness_status_data_t *)p_data;
        WICED_BT_TRACE("light:%d target:%d remaining time:%d\n",
                p_status_data->present, p_status_data->target, p_status_data->remaining_time);

#if defined HCI_CONTROL
        if ((p_hci_event = wiced_bt_mesh_create_hci_event(p_event)) != NULL)
            mesh_light_lightness_hci_event_send(p_hci_event, p_status_data);
#endif
        break;

    case WICED_BT_MESH_LIGHT_LIGHTNESS_LINEAR_STATUS:
        p_status_data = (wiced_bt_mesh_light_lightness_status_data_t *)p_data;
        WICED_BT_TRACE("linear light:%d target:%d remaining time:%d\n",
                p_status_data->present, p_status_data->target, p_status_data->remaining_time);

#if defined HCI_CONTROL
        if ((p_hci_event = wiced_bt_mesh_create_hci_event(p_event)) != NULL)
            mesh_light_lightness_linear_hci_event_send(p_hci_event, p_status_data);
#endif
        break;

    case WICED_BT_MESH_LIGHT_LIGHTNESS_LAST_STATUS:
        p_last_status_data = (wiced_bt_mesh_light_lightness_last_data_t *)p_data;
        WICED_BT_TRACE("last light:%d\n", p_last_status_data->last_level);

#if defined HCI_CONTROL
        if ((p_hci_event = wiced_bt_mesh_create_hci_event(p_event)) != NULL)
            mesh_light_lightness_last_hci_event_send(p_hci_event, p_last_status_data);
#endif
        break;

    case WICED_BT_MESH_LIGHT_LIGHTNESS_DEFAULT_STATUS:
        p_default_status_data = (wiced_bt_mesh_light_lightness_default_data_t *)p_data;
        WICED_BT_TRACE("default light:%d\n", p_default_status_data->default_level);

#if defined HCI_CONTROL
        if ((p_hci_event = wiced_bt_mesh_create_hci_event(p_event)) != NULL)
            mesh_light_lightness_default_hci_event_send(p_hci_event, p_default_status_data);
#endif
        break;

    case WICED_BT_MESH_LIGHT_LIGHTNESS_RANGE_STATUS:
        p_range_status_data = (wiced_bt_mesh_light_lightness_range_status_data_t *)p_data;
        WICED_BT_TRACE("status:%d min light:%d max:%d\n", p_range_status_data->status, p_range_status_data->min_level, p_range_status_data->max_level);

#if defined HCI_CONTROL
        if ((p_hci_event = wiced_bt_mesh_create_hci_event(p_event)) != NULL)
            mesh_light_lightness_range_hci_event_status_send(p_hci_event, p_range_status_data);
#endif
        break;

    }
    wiced_bt_mesh_release_event(p_event);
}

/*
 * In 2 chip solutions MCU can send commands to change onoff state.
 */
uint32_t mesh_app_proc_rx_cmd(uint16_t opcode, uint8_t *p_data, uint32_t length)
{
#ifdef HCI_CONTROL
    wiced_bt_mesh_event_t *p_event;

    switch (opcode)
    {
    case HCI_CONTROL_MESH_COMMAND_LIGHT_LIGHTNESS_GET:
    case HCI_CONTROL_MESH_COMMAND_LIGHT_LIGHTNESS_SET:
    case HCI_CONTROL_MESH_COMMAND_LIGHT_LIGHTNESS_LINEAR_GET:
    case HCI_CONTROL_MESH_COMMAND_LIGHT_LIGHTNESS_LINEAR_SET:
    case HCI_CONTROL_MESH_COMMAND_LIGHT_LIGHTNESS_LAST_GET:
    case HCI_CONTROL_MESH_COMMAND_LIGHT_LIGHTNESS_DEFAULT_GET:
    case HCI_CONTROL_MESH_COMMAND_LIGHT_LIGHTNESS_DEFAULT_SET:
    case HCI_CONTROL_MESH_COMMAND_LIGHT_LIGHTNESS_RANGE_GET:
    case HCI_CONTROL_MESH_COMMAND_LIGHT_LIGHTNESS_RANGE_SET:
        break;

    default:
        return WICED_FALSE;
    }
    p_event = wiced_bt_mesh_create_event_from_wiced_hci(opcode, MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_LIGHT_LIGHTNESS_CLNT, &p_data, &length);
    if (p_event == NULL)
    {
        WICED_BT_TRACE("lightness bad hdr\n");
        return WICED_TRUE;
    }
    switch (opcode)
    {
    case HCI_CONTROL_MESH_COMMAND_LIGHT_LIGHTNESS_GET:
        mesh_light_lightness_client_get(p_event, p_data, length);
        break;

    case HCI_CONTROL_MESH_COMMAND_LIGHT_LIGHTNESS_SET:
        mesh_light_lightness_client_set(p_event, p_data, length);
        break;

    case HCI_CONTROL_MESH_COMMAND_LIGHT_LIGHTNESS_LINEAR_GET:
        mesh_light_lightness_client_linear_get(p_event, p_data, length);
        break;

    case HCI_CONTROL_MESH_COMMAND_LIGHT_LIGHTNESS_LINEAR_SET:
        mesh_light_lightness_client_linear_set(p_event, p_data, length);
        break;

    case HCI_CONTROL_MESH_COMMAND_LIGHT_LIGHTNESS_LAST_GET:
        mesh_light_lightness_client_last_get(p_event, p_data, length);
        break;

    case HCI_CONTROL_MESH_COMMAND_LIGHT_LIGHTNESS_DEFAULT_GET:
        mesh_light_lightness_client_default_get(p_event, p_data, length);
        break;

    case HCI_CONTROL_MESH_COMMAND_LIGHT_LIGHTNESS_DEFAULT_SET:
        mesh_light_lightness_client_default_set(p_event, p_data, length);
        break;

    case HCI_CONTROL_MESH_COMMAND_LIGHT_LIGHTNESS_RANGE_GET:
        mesh_light_lightness_client_range_get(p_event, p_data, length);
        break;

    case HCI_CONTROL_MESH_COMMAND_LIGHT_LIGHTNESS_RANGE_SET:
        mesh_light_lightness_client_range_set(p_event, p_data, length);
        break;
    }
#endif
    return WICED_TRUE;
}

/*
 * Send Light Lightness Get command
 */
void mesh_light_lightness_client_get(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint32_t length)
{
    wiced_bt_mesh_model_light_lightness_client_send_get(p_event);
}

/*
 * Send Light Lightness Set command
 */
void mesh_light_lightness_client_set(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint32_t length)
{
    wiced_bt_mesh_light_lightness_actual_set_t set_data;

    STREAM_TO_UINT16(set_data.lightness_actual, p_data);
    STREAM_TO_UINT32(set_data.transition_time, p_data);
    STREAM_TO_UINT16(set_data.delay, p_data);

    wiced_bt_mesh_model_light_lightness_client_send_set(p_event, &set_data);
}

/*
 * Send Light Lightness Linear Get command
 */
void mesh_light_lightness_client_linear_get(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint32_t length)
{
    wiced_bt_mesh_model_light_lightness_client_send_linear_get(p_event);
}

/*
 * Send Light Lightness Linear Set command
 */
void mesh_light_lightness_client_linear_set(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint32_t length)
{
    wiced_bt_mesh_light_lightness_linear_set_t set_data;

    STREAM_TO_UINT16(set_data.lightness_linear, p_data);
    STREAM_TO_UINT32(set_data.transition_time, p_data);
    STREAM_TO_UINT16(set_data.delay, p_data);

    wiced_bt_mesh_model_light_lightness_client_send_linear_set(p_event, &set_data);
}

/*
 * Send Light Lightness Last Get command
 */
void mesh_light_lightness_client_last_get(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint32_t length)
{
    wiced_bt_mesh_model_light_lightness_client_send_last_get(p_event);
}

/*
 * Send Light Lightness Default Get command
 */
void mesh_light_lightness_client_default_get(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint32_t length)
{
    wiced_bt_mesh_model_light_lightness_client_send_default_get(p_event);
}

/*
 * Send Light Lightness Default Set command
 */
void mesh_light_lightness_client_default_set(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint32_t length)
{
    wiced_bt_mesh_light_lightness_default_data_t set_data;

    STREAM_TO_UINT16(set_data.default_level, p_data);

    wiced_bt_mesh_model_light_lightness_client_send_default_set(p_event, &set_data);
}

/*
 * Send Light Lightness Range Get command
 */
void mesh_light_lightness_client_range_get(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint32_t length)
{
    wiced_bt_mesh_model_light_lightness_client_send_range_get(p_event);
}

/*
 * Send Light Lightness Range Set command
 */
void mesh_light_lightness_client_range_set(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint32_t length)
{
    wiced_bt_mesh_light_lightness_range_set_data_t set_data;

    STREAM_TO_UINT16(set_data.min_level, p_data);
    STREAM_TO_UINT16(set_data.max_level, p_data);

    wiced_bt_mesh_model_light_lightness_client_send_range_set(p_event, &set_data);
}

#ifdef HCI_CONTROL
/*
 * Send Light Lightness Status event over transport
 */
void mesh_light_lightness_hci_event_send(wiced_bt_mesh_hci_event_t *p_hci_event, wiced_bt_mesh_light_lightness_status_data_t *p_data)
{
    uint8_t *p = p_hci_event->data;

    UINT16_TO_STREAM(p, p_data->present);
    UINT16_TO_STREAM(p, p_data->target);
    UINT32_TO_STREAM(p, p_data->remaining_time);

    mesh_transport_send_data(HCI_CONTROL_MESH_EVENT_LIGHT_LIGHTNESS_STATUS, (uint8_t *)p_hci_event, (uint16_t)(p - (uint8_t *)p_hci_event));
}

/*
 * Send Light Lightness Linear Status event over transport
 */
void mesh_light_lightness_linear_hci_event_send(wiced_bt_mesh_hci_event_t *p_hci_event, wiced_bt_mesh_light_lightness_status_data_t *p_data)
{
    uint8_t *p = p_hci_event->data;

    UINT16_TO_STREAM(p, p_data->present);
    UINT16_TO_STREAM(p, p_data->target);
    UINT32_TO_STREAM(p, p_data->remaining_time);

    mesh_transport_send_data(HCI_CONTROL_MESH_EVENT_LIGHT_LIGHTNESS_LINEAR_STATUS, (uint8_t *)p_hci_event, (uint16_t)(p - (uint8_t *)p_hci_event));
}

/*
 * Send Light Lightness Last Status event over transport
 */
void mesh_light_lightness_last_hci_event_send(wiced_bt_mesh_hci_event_t *p_hci_event, wiced_bt_mesh_light_lightness_last_data_t *p_data)
{
    uint8_t *p = p_hci_event->data;

    UINT16_TO_STREAM(p, p_data->last_level);

    mesh_transport_send_data(HCI_CONTROL_MESH_EVENT_LIGHT_LIGHTNESS_LAST_STATUS, (uint8_t *)p_hci_event, (uint16_t)(p - (uint8_t *)p_hci_event));
}

/*
 * Send Light Lightness Default Status event over transport
 */
void mesh_light_lightness_default_hci_event_send(wiced_bt_mesh_hci_event_t *p_hci_event, wiced_bt_mesh_light_lightness_default_data_t *p_data)
{
    uint8_t *p = p_hci_event->data;

    UINT16_TO_STREAM(p, p_data->default_level);

    mesh_transport_send_data(HCI_CONTROL_MESH_EVENT_LIGHT_LIGHTNESS_DEFAULT_STATUS, (uint8_t *)p_hci_event, (uint16_t)(p - (uint8_t *)p_hci_event));
}

/*
 * Send Light Lightness Range Status event over transport
 */
void mesh_light_lightness_range_hci_event_status_send(wiced_bt_mesh_hci_event_t *p_hci_event, wiced_bt_mesh_light_lightness_range_status_data_t *p_data)
{
    uint8_t *p = p_hci_event->data;

    UINT8_TO_STREAM(p, p_data->status);
    UINT16_TO_STREAM(p, p_data->min_level);
    UINT16_TO_STREAM(p, p_data->max_level);

    mesh_transport_send_data(HCI_CONTROL_MESH_EVENT_LIGHT_LIGHTNESS_RANGE_STATUS, (uint8_t *)p_hci_event, (uint16_t)(p - (uint8_t *)p_hci_event));
}

#endif
