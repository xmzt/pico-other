#ifndef MICO_USB_H
#define MICO_USB_H

// Copyright (c) 2024 xmzt
// extended from pico-examples/usb/device/dev_lowlevel/usb_common.h
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "mico.h"

#include "hardware/regs/usb.h"
#include "hardware/structs/usb.h"
#include "pico/types.h"

#include <string.h> //memcpy

// bmRequestType bit definitions
#define MICO_USB_REQ_TYPE_STANDARD 0x00u
#define MICO_USB_REQ_TYPE_TYPE_MASK 0x60u
#define MICO_USB_REQ_TYPE_TYPE_CLASS 0x20u
#define MICO_USB_REQ_TYPE_TYPE_VENDOR 0x40u

#define MICO_USB_REQ_TYPE_RECIPIENT_MASK 0x1fu
#define MICO_USB_REQ_TYPE_RECIPIENT_DEVICE 0x00u
#define MICO_USB_REQ_TYPE_RECIPIENT_INTERFACE 0x01u
#define MICO_USB_REQ_TYPE_RECIPIENT_ENDPOINT 0x02u

#define MICO_USB_DIR_MASK 0x80u
#define MICO_USB_DIR_OUT 0x00u
#define MICO_USB_DIR_IN 0x80u

#define MICO_USB_TRANSFER_TYPE_CONTROL 0x0
#define MICO_USB_TRANSFER_TYPE_ISOCHRONOUS 0x1
#define MICO_USB_TRANSFER_TYPE_BULK 0x2
#define MICO_USB_TRANSFER_TYPE_INTERRUPT 0x3
#define MICO_USB_TRANSFER_TYPE_BITS 0x3

// Descriptor types
#define MICO_USB_DT_DEVICE 0x01
#define MICO_USB_DT_CONFIG 0x02
#define MICO_USB_DT_STRING 0x03
#define MICO_USB_DT_INTERFACE 0x04
#define MICO_USB_DT_ENDPOINT 0x05

#define MICO_USB_REQUEST_GET_STATUS 0x0
#define MICO_USB_REQUEST_CLEAR_FEATURE 0x01
#define MICO_USB_REQUEST_SET_FEATURE 0x03
#define MICO_USB_REQUEST_SET_ADDRESS 0x05
#define MICO_USB_REQUEST_GET_DESCRIPTOR 0x06
#define MICO_USB_REQUEST_SET_DESCRIPTOR 0x07
#define MICO_USB_REQUEST_GET_CONFIGURATION 0x08
#define MICO_USB_REQUEST_SET_CONFIGURATION 0x09
#define MICO_USB_REQUEST_GET_INTERFACE 0x0a
#define MICO_USB_REQUEST_SET_INTERFACE 0x0b
#define MICO_USB_REQUEST_SYNC_FRAME 0x0c

#define MICO_USB_REQUEST_MSC_GET_MAX_LUN 0xfe
#define MICO_USB_REQUEST_MSC_RESET 0xff

#define MICO_USB_FEAT_ENDPOINT_HALT            0x00
#define MICO_USB_FEAT_DEVICE_REMOTE_WAKEUP   0x01
#define MICO_USB_FEAT_TEST_MODE                0x02

#define MICO_USB_DESCRIPTOR_TYPE_ENDPOINT 0x05

//-----------------------------------------------------------------------------------------------------------------------
// mico_usb_setup

typedef struct {
    uint8_t bmRequestType;
    uint8_t bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
} __packed mico_usb_setup_packet_t;

#define MICO_USB_SETUP_PACKET_REQUEST_TYPE (*(uint8_t*)(USBCTRL_DPRAM_BASE + 0))
#define MICO_USB_SETUP_PACKET_REQUEST (*(uint8_t*)(USBCTRL_DPRAM_BASE + 1))
#define MICO_USB_SETUP_PACKET_VALUE (*(uint16_t*)(USBCTRL_DPRAM_BASE + 2))
#define MICO_USB_SETUP_PACKET_VALUE_ADDRESS (*(uint8_t*)(USBCTRL_DPRAM_BASE + 2))
#define MICO_USB_SETUP_PACKET_VALUE_DESCRIPTOR_TYPE (*(uint8_t*)(USBCTRL_DPRAM_BASE + 3))
#define MICO_USB_SETUP_PACKET_VALUE_STRING_INDEX (*(uint8_t*)(USBCTRL_DPRAM_BASE + 2))
#define MICO_USB_SETUP_PACKET_INDEX (*(uint16_t*)(USBCTRL_DPRAM_BASE + 4))
#define MICO_USB_SETUP_PACKET_LENGTH (*(uint16_t*)(USBCTRL_DPRAM_BASE + 6))

//-----------------------------------------------------------------------------------------------------------------------
// mico_usb_*_descriptor

typedef struct {
    uint8_t bLength;
    uint8_t bDescriptorType;
} __packed mico_usb_descriptor_t;

typedef struct {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t bcdUSB;
    uint8_t bDeviceClass;
    uint8_t bDeviceSubClass;
    uint8_t bDeviceProtocol;
    uint8_t bMaxPacketSize0;
    uint16_t idVendor;
    uint16_t idProduct;
    uint16_t bcdDevice;
    uint8_t iManufacturer;
    uint8_t iProduct;
    uint8_t iSerialNumber;
    uint8_t bNumConfigurations;
} __packed mico_usb_device_descriptor_t;

typedef struct {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t wTotalLength;
    uint8_t bNumInterfaces;
    uint8_t bConfigurationValue;
    uint8_t iConfiguration;
    uint8_t bmAttributes;
    uint8_t bMaxPower;
} __packed mico_usb_configuration_descriptor_t;

typedef struct {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bInterfaceNumber;
    uint8_t bAlternateSetting;
    uint8_t bNumEndpoints;
    uint8_t bInterfaceClass;
    uint8_t bInterfaceSubClass;
    uint8_t bInterfaceProtocol;
    uint8_t iInterface;
} __packed mico_usb_interface_descriptor_t;

// for each endpoint define the following replacing pre with the prefix you want
// pre_ADDR: endpoint address
// pre_DIR: MICO_USB_DIR_IN or MICO_USB_DIR_OUT
// pre_TRANSFER_TYPE: one of MICO_USB_TRANSFER_TYPE_*
// pre_MAX_PACKET_SIZE: probably 1<<6
// pre_INTERVAL
// pre_BUF_OFF: offset to first byte of buffer for this endpoint in usb_dpram

typedef struct {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bEndpointAddress;
    uint8_t bmAttributes;
    uint16_t wMaxPacketSize;
    uint8_t bInterval;
} __packed mico_usb_endpoint_descriptor_t;

#define MICO_USB_ENDPOINT_DESCRIPTOR_INITIALIZER(pre) {		\
		.bLength = sizeof(mico_usb_endpoint_descriptor_t),	\
			.bDescriptorType  = MICO_USB_DT_ENDPOINT,		\
			.bEndpointAddress = pre##_ADDR | pre##_DIR,		\
			.bmAttributes     = pre##_TRANSFER_TYPE,		\
			.wMaxPacketSize   = pre##_MAX_PACKET_SIZE,		\
			.bInterval        = pre##_INTERVAL				\
			}

typedef struct {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bEndpointAddress;
    uint8_t bmAttributes;
    uint16_t wMaxPacketSize;
    uint8_t bInterval;
    uint8_t bRefresh;
    uint8_t bSyncAddr;
} __packed mico_usb_endpoint_descriptor_long_t; /* why __attribute__((packed)) in usb_common.h */ 

// ASSERT: compiler produces little-endian code for u"" string literals (rp2040 is little endian)
typedef struct {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint16_t content[];
} __packed mico_usb_string_descriptor_t;

#define MICO_USB_STRING(s) { sizeof(s), 0x03, s }
#define MICO_USB_STRING_LANG(code) { 4, 0x03, { code } }
#define MICO_USB_STRING_LANG_ENGLISH_US() MICO_USB_STRING_LANG(0x0409)

//-----------------------------------------------------------------------------------------------------------------------
// mico_usb_device

void mico_usb_device_init0();
	
inline static void mico_usb_device_init1() {
    // Present full speed device by enabling pull up on DP
	MICO_ALIAS_SET(usb_hw->sie_ctrl) = USB_SIE_CTRL_PULLUP_EN_BITS;
}

//-----------------------------------------------------------------------------------------------------------------------
// mico_usb_ep

#define MICO_USB_EP_CTRL_IN(pre) (((io_rw_32*)USBCTRL_DPRAM_BASE)[2 * pre##_ADDR + 0])
#define MICO_USB_EP_CTRL_OUT(pre) (((io_rw_32*)USBCTRL_DPRAM_BASE)[2 * pre##_ADDR + 1])

#define MICO_USB_EP_CTRL_INITIALIZER(pre)				\
	(EP_CTRL_ENABLE_BITS								\
	 | EP_CTRL_INTERRUPT_PER_BUFFER						\
	 | (pre##_TRANSFER_TYPE << EP_CTRL_BUFFER_TYPE_LSB)	\
	 | pre##_BUF_OFF)

#define MICO_USB_EP_BUF_CTRL_IN(pre) (((io_rw_32*)(USBCTRL_DPRAM_BASE + 0x80))[2 * pre##_ADDR + 0])
#define MICO_USB_EP_BUF_CTRL_OUT(pre) (((io_rw_32*)(USBCTRL_DPRAM_BASE + 0x80))[2 * pre##_ADDR + 1])

#define MICO_USB_EP_BUF_CTRL_IN_LEN_0(pre) (MICO_USB_EP_BUF_CTRL_IN(pre) & USB_BUF_CTRL_LEN_MASK)
#define MICO_USB_EP_BUF_CTRL_OUT_LEN_0(pre) (MICO_USB_EP_BUF_CTRL_OUT(pre) & USB_BUF_CTRL_LEN_MASK)

#define MICO_USB_EP_BUF(pre) ((uint8_t*)(USBCTRL_DPRAM_BASE + pre##_BUF_OFF))

#define MICO_USB_EP_RX(pre, len) do {					\
		MICO_USB_EP_BUF_CTRL_OUT(pre)					\
			= len										\
			| USB_BUF_CTRL_AVAIL						\
			| (pre##_pid ^= USB_BUF_CTRL_DATA1_PID);	\
	} while(0)

#define MICO_USB_EP_TX(pre, len) do {					\
		MICO_USB_EP_BUF_CTRL_IN(pre)					\
			= len										\
			| USB_BUF_CTRL_AVAIL						\
			| USB_BUF_CTRL_FULL							\
			| (pre##_pid ^= USB_BUF_CTRL_DATA1_PID);	\
	} while(0)

#define MICO_USB_EP_TX_DATA1_PID(pre, len) do {		\
		MICO_USB_EP_BUF_CTRL_IN(pre)				\
			= len									\
			| USB_BUF_CTRL_AVAIL					\
			| USB_BUF_CTRL_FULL						\
			| (pre##_pid = USB_BUF_CTRL_DATA1_PID);	\
	} while(0)

#define MICO_USB_EP_TX_MEMCPY(pre, len, buf) do {	\
		io_rw_32 _len = (len);						\
		memcpy(MICO_USB_EP_BUF(pre), (buf), _len);	\
		MICO_USB_EP_TX(pre, _len);					\
	} while(0)

#define MICO_USB_EP_TX_MEMCPY_DATA1_PID(pre, len, buf) do {	\
		io_rw_32 _len = (len);								\
		memcpy(MICO_USB_EP_BUF(pre), (buf), _len);			\
		MICO_USB_EP_TX_DATA1_PID(pre, _len);				\
	} while(0)

#endif
