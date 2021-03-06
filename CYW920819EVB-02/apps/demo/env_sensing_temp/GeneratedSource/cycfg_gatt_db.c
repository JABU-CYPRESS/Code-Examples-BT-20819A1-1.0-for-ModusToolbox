/***************************************************************************//**
* File Name: cycfg_gatt_db.c
* Version: 1.1
*
* Description:
* BLE device's GATT database and device configuration.
* This file should not be modified. It was automatically generated by
* Bluetooth Configurator 1.1.0 build 216
*
********************************************************************************
* Copyright 2019 Cypress Semiconductor Corporation
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/

#include "cycfg_gatt_db.h"
#include "wiced_bt_uuid.h"
#include "wiced_bt_gatt.h"

/*************************************************************************************
* GATT server definitions
*************************************************************************************/

const uint8_t gatt_database[] =
{
    /* Primary Service: Generic Access */
    PRIMARY_SERVICE_UUID16 (HDLS_GAP, __UUID_SERVICE_GENERIC_ACCESS),
        /* Characteristic: Device Name */
        CHARACTERISTIC_UUID16 (HDLC_GAP_DEVICE_NAME, HDLC_GAP_DEVICE_NAME_VALUE, __UUID_CHARACTERISTIC_DEVICE_NAME, LEGATTDB_CHAR_PROP_READ, LEGATTDB_PERM_READABLE),
        /* Characteristic: Appearance */
        CHARACTERISTIC_UUID16 (HDLC_GAP_APPEARANCE, HDLC_GAP_APPEARANCE_VALUE, __UUID_CHARACTERISTIC_APPEARANCE, LEGATTDB_CHAR_PROP_READ, LEGATTDB_PERM_READABLE),

    /* Primary Service: Generic Attribute */
    PRIMARY_SERVICE_UUID16 (HDLS_GATT, __UUID_SERVICE_GENERIC_ATTRIBUTE),

    /* Primary Service: Environmental Sensing */
    PRIMARY_SERVICE_UUID16 (HDLS_ESS, __UUID_SERVICE_ENVIRONMENTAL_SENSING),
        /* Characteristic: Temperature */
        CHARACTERISTIC_UUID16 (HDLC_ESS_TEMPERATURE, HDLC_ESS_TEMPERATURE_VALUE, __UUID_CHARACTERISTIC_TEMPERATURE, LEGATTDB_CHAR_PROP_READ | LEGATTDB_CHAR_PROP_NOTIFY, LEGATTDB_PERM_READABLE),
            /* Descriptor: Client Characteristic Configuration */
            CHAR_DESCRIPTOR_UUID16_WRITABLE (HDLD_ESS_TEMPERATURE_CLIENT_CHAR_CONFIG, __UUID_DESCRIPTOR_CLIENT_CHARACTERISTIC_CONFIGURATION, LEGATTDB_PERM_READABLE | LEGATTDB_PERM_WRITE_REQ),
            /* Descriptor: Environmental Sensing Measurement */
            CHAR_DESCRIPTOR_UUID16 (HDLD_ESS_TEMPERATURE_ENVIRONMENTAL_SENSING_MEASUREMENT, __UUID_DESCRIPTOR_ENVIRONMENTAL_SENSING_MEASUREMENT, LEGATTDB_PERM_READABLE),
            /* Descriptor: Valid Range */
            CHAR_DESCRIPTOR_UUID16 (HDLD_ESS_TEMPERATURE_VALID_RANGE, __UUID_DESCRIPTOR_VALID_RANGE, LEGATTDB_PERM_READABLE),

#ifdef OTA_SECURE_FIRMWARE_UPGRADE
    PRIMARY_SERVICE_UUID128(HDLS_FWUPGRADESERVICE, UUID_OTA_SEC_FW_UPGRADE_SERVICE),
#else
    /* Primary Service: FWUpgradeService */
    PRIMARY_SERVICE_UUID128 (HDLS_FWUPGRADESERVICE, __UUID_SERVICE_FWUPGRADESERVICE),
#endif
        /* Characteristic: FWUpgradeContolPoint */
        CHARACTERISTIC_UUID128_WRITABLE (HDLC_FWUPGRADESERVICE_FWUPGRADECONTOLPOINT, HDLC_FWUPGRADESERVICE_FWUPGRADECONTOLPOINT_VALUE, __UUID_CHARACTERISTIC_FWUPGRADESERVICE_FWUPGRADECONTOLPOINT, LEGATTDB_CHAR_PROP_WRITE | LEGATTDB_CHAR_PROP_NOTIFY | LEGATTDB_CHAR_PROP_INDICATE, LEGATTDB_PERM_VARIABLE_LENGTH | LEGATTDB_PERM_WRITE_REQ),
            /* Descriptor: Client Characteristic Configuration */
            CHAR_DESCRIPTOR_UUID16_WRITABLE (HDLD_FWUPGRADESERVICE_FWUPGRADECONTOLPOINT_CLIENT_CHAR_CONFIG, __UUID_DESCRIPTOR_CLIENT_CHARACTERISTIC_CONFIGURATION, LEGATTDB_PERM_READABLE | LEGATTDB_PERM_WRITE_REQ),
        /* Characteristic: FWUpgradeData */
        CHARACTERISTIC_UUID128_WRITABLE (HDLC_FWUPGRADESERVICE_FWUPGRADEDATA, HDLC_FWUPGRADESERVICE_FWUPGRADEDATA_VALUE, __UUID_CHARACTERISTIC_FWUPGRADESERVICE_FWUPGRADEDATA, LEGATTDB_CHAR_PROP_WRITE, LEGATTDB_PERM_VARIABLE_LENGTH | LEGATTDB_PERM_WRITE_REQ | LEGATTDB_PERM_RELIABLE_WRITE),
};

/* Length of the GATT database */
const uint16_t gatt_database_len = sizeof(gatt_database);

/*************************************************************************************
 * GATT Initial Value Arrays
 ************************************************************************************/

uint8_t app_gap_device_name[]                                   = {'T', 'h', 'e', 'r', 'm', 'i', 's', 't', 'o', 'r', };
uint8_t app_gap_appearance[]                                    = {0x00u, 0x03u, };
uint8_t app_ess_temperature[]                                   = {0x00u, 0x00u, };
uint8_t app_ess_temperature_client_char_config[]                = {0x00u, 0x00u, };
uint8_t app_ess_temperature_environmental_sensing_measurement[] = {0x00u, 0x00u, 0x01u, 0x00u, 0x00u, 0x00u, 0x05u, 0x00u, 0x00u, 0x00u, 0x0Au, };
uint8_t app_ess_temperature_valid_range[]                       = {0x60u, 0xF0u, 0xD4u, 0x30u, };
uint8_t app_fwupgradeservice_fwupgradecontolpoint[]                    = {};
uint8_t app_fwupgradeservice_fwupgradecontolpoint_client_char_config[] = {0x00u, 0x00u, };
uint8_t app_fwupgradeservice_fwupgradedata[]                           = {};

 /************************************************************************************
 * GATT Lookup Table
 ************************************************************************************/

gatt_db_lookup_table_t app_gatt_db_ext_attr_tbl[] =
{
    /* { attribute handle,                                    maxlen, curlen, attribute data } */
    { HDLC_GAP_DEVICE_NAME_VALUE,                             10,     10,     app_gap_device_name },
    { HDLC_GAP_APPEARANCE_VALUE,                              2,      2,      app_gap_appearance },
    { HDLC_ESS_TEMPERATURE_VALUE,                             2,      2,      app_ess_temperature },
    { HDLD_ESS_TEMPERATURE_CLIENT_CHAR_CONFIG,                2,      2,      app_ess_temperature_client_char_config },
    { HDLD_ESS_TEMPERATURE_ENVIRONMENTAL_SENSING_MEASUREMENT, 11,     11,     app_ess_temperature_environmental_sensing_measurement },
    { HDLD_ESS_TEMPERATURE_VALID_RANGE,                       4,      4,      app_ess_temperature_valid_range },
    { HDLC_FWUPGRADESERVICE_FWUPGRADECONTOLPOINT_VALUE,              0,      0,      app_fwupgradeservice_fwupgradecontolpoint },
    { HDLD_FWUPGRADESERVICE_FWUPGRADECONTOLPOINT_CLIENT_CHAR_CONFIG, 2,      2,      app_fwupgradeservice_fwupgradecontolpoint_client_char_config },
    { HDLC_FWUPGRADESERVICE_FWUPGRADEDATA_VALUE,                     0,      0,      app_fwupgradeservice_fwupgradedata },

};

/* Number of Lookup Table entries */
const uint16_t app_gatt_db_ext_attr_tbl_size = (sizeof(app_gatt_db_ext_attr_tbl) / sizeof(gatt_db_lookup_table_t));

/* Number of GATT initial value arrays entries */
const uint16_t app_gap_device_name_len = (sizeof(app_gap_device_name));
const uint16_t app_gap_appearance_len = (sizeof(app_gap_appearance));
const uint16_t app_ess_temperature_len = (sizeof(app_ess_temperature));
const uint16_t app_ess_temperature_client_char_config_len = (sizeof(app_ess_temperature_client_char_config));
const uint16_t app_ess_temperature_environmental_sensing_measurement_len = (sizeof(app_ess_temperature_environmental_sensing_measurement));
const uint16_t app_ess_temperature_valid_range_len = (sizeof(app_ess_temperature_valid_range));
const uint16_t app_fwupgradeservice_fwupgradecontolpoint_len = (sizeof(app_fwupgradeservice_fwupgradecontolpoint));
const uint16_t app_fwupgradeservice_fwupgradecontolpoint_client_char_config_len = (sizeof(app_fwupgradeservice_fwupgradecontolpoint_client_char_config));
const uint16_t app_fwupgradeservice_fwupgradedata_len = (sizeof(app_fwupgradeservice_fwupgradedata));

