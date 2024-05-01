/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef SELFPROG_H
#define SELFPROG_H

#include "mico_usb.h"

// Descriptors
static const mico_usb_device_descriptor_t device_descriptor = {
	.bLength         = sizeof(mico_usb_device_descriptor_t),
	.bDescriptorType = MICO_USB_DT_DEVICE,
	.bcdUSB          = 0x0110, // USB 1.1 device
	.bDeviceClass    = 0,      // Specified in interface descriptor
	.bDeviceSubClass = 0,      // No subclass
	.bDeviceProtocol = 0,      // No protocol
	.bMaxPacketSize0 = 0x40,   // Max packet size for ep0
	.idVendor        = 0x0000, // Your vendor id
	.idProduct       = 0x0001, // Your product ID
	.bcdDevice       = 0,      // No device revision number
	.iManufacturer   = 1,      // Manufacturer string index
	.iProduct        = 2,      // Product string index
	.iSerialNumber = 0,        // No serial number
	.bNumConfigurations = 1    // One configuration
};

#define ep0_out_ADDR 0
#define ep0_out_DIR MICO_USB_DIR_OUT
#define ep0_out_TRANSFER_TYPE MICO_USB_TRANSFER_TYPE_CONTROL
#define ep0_out_MAX_PACKET_SIZE 0x40
#define ep0_out_INTERVAL 0
#define ep0_out_BUF_OFF 0x100

#define ep0_in_ADDR 0
#define ep0_in_DIR MICO_USB_DIR_IN
#define ep0_in_TRANSFER_TYPE MICO_USB_TRANSFER_TYPE_CONTROL
#define ep0_in_MAX_PACKET_SIZE 0x40
#define ep0_in_INTERVAL 0
#define ep0_in_BUF_OFF 0x100

#define ep1_out_ADDR 1
#define ep1_out_INOUT out
#define ep1_out_DIR MICO_USB_DIR_OUT
#define ep1_out_TRANSFER_TYPE MICO_USB_TRANSFER_TYPE_BULK
#define ep1_out_MAX_PACKET_SIZE 0x40
#define ep1_out_INTERVAL 0
#define ep1_out_BUF_OFF 0x180

#define ep2_in_ADDR 2
#define ep2_in_INOUT in
#define ep2_in_DIR MICO_USB_DIR_IN
#define ep2_in_TRANSFER_TYPE MICO_USB_TRANSFER_TYPE_BULK
#define ep2_in_MAX_PACKET_SIZE 0x40
#define ep2_in_INTERVAL 0
#define ep2_in_BUF_OFF 0x1c0

struct {
    mico_usb_configuration_descriptor_t configuration;
    mico_usb_interface_descriptor_t interface;
	mico_usb_endpoint_descriptor_t endpointV[2];
} __packed usb_configuration = {
	.configuration = {
        .bLength         = sizeof(mico_usb_configuration_descriptor_t),
        .bDescriptorType = MICO_USB_DT_CONFIG,
        .wTotalLength    = sizeof(usb_configuration),
        .bNumInterfaces  = 1,
        .bConfigurationValue = 1, // Configuration 1
        .iConfiguration = 0,      // No string
        .bmAttributes = 0xc0,     // attributes: self powered, no remote wakeup
        .bMaxPower = 0x32         // 100ma
	},
	.interface = {
        .bLength            = sizeof(mico_usb_interface_descriptor_t),
        .bDescriptorType    = MICO_USB_DT_INTERFACE,
        .bInterfaceNumber   = 0,
        .bAlternateSetting  = 0,
        .bNumEndpoints      = 2,    // Interface has 2 endpoints
        .bInterfaceClass    = 0xff, // Vendor specific endpoint
        .bInterfaceSubClass = 0,
        .bInterfaceProtocol = 0,
        .iInterface         = 0
	},
	.endpointV = {
		MICO_USB_ENDPOINT_DESCRIPTOR_INITIALIZER(ep1_out),
		MICO_USB_ENDPOINT_DESCRIPTOR_INITIALIZER(ep2_in)
	}
};

static const mico_usb_string_descriptor_t usb_lang = MICO_USB_STRING_LANG(0x0409);
static const mico_usb_string_descriptor_t usb_vendor = MICO_USB_STRING(u"Shittyvendor");
static const mico_usb_string_descriptor_t usb_product = MICO_USB_STRING(u"Shittierproduct");

static const mico_usb_string_descriptor_t *usb_stringV[] = {
	/* 0: lang */ &usb_lang,
	/* 1: vendor */ &usb_vendor,
	/* 2: product */ &usb_product
};

//-----------------------------------------------------------------------------------------------------------------------
//

#define LOG_ITEM_HEAD							\
	uint8_t len;								\
	uint8_t type;								\
	uint16_t ser

typedef struct {
	LOG_ITEM_HEAD;
} __packed log_item_t;

typedef struct {
	LOG_ITEM_HEAD;
	uint8_t addr;
} __packed log_item_set_address_t;

#define LOG_BUS_RESET()
#define LOG_SETUP_GET_DESCRIPTOR_CONFIG()
#define LOG_SETUP_GET_DESCRIPTOR_DEVICE()
#define LOG_SETUP_GET_DESCRIPTOR_STRING(i)
#define LOG_SETUP_SET_ADDRESS(dev_addr) 
#define LOG_SETUP_SET_CONFIGURATION()
#define LOG_SETUP_UNK()




#endif
