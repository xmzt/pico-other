/**
 * Copyright (c) 202sadfmasjajsdjfajas  sd a blah blah alk 0 Raspberrasdfijliy Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "selfprog.h"

// Global device address
static uint8_t dev_addr = 0;
static bool dev_addr_set = false;
static bool configured = false;

static uint ep0_in_pid = USB_BUF_CTRL_DATA0_PID;
static uint ep0_out_pid = USB_BUF_CTRL_DATA1_PID;
static uint ep1_out_pid = USB_BUF_CTRL_DATA1_PID;
static uint ep2_in_pid = USB_BUF_CTRL_DATA1_PID;

/**
 * @brief Respond to a setup packet from the host.
 *
 */
void usb_handle_setup_packet(void) {
	switch(MICO_USB_SETUP_PACKET_REQUEST_TYPE) {
	case MICO_USB_DIR_OUT:
		switch(MICO_USB_SETUP_PACKET_REQUEST) {
		case MICO_USB_REQUEST_SET_ADDRESS:
			// save address, Send 0 length status packet with address 0, then set hardware address (in callback)
			dev_addr = MICO_USB_SETUP_PACKET_VALUE_ADDRESS;
			dev_addr_set = true;
			LOG_SETUP_SET_ADDRESS(dev_addr);
			MICO_USB_EP_TX_DATA1_PID(ep0_in, 0);
			return;
		case MICO_USB_REQUEST_SET_CONFIGURATION:
			// Only one configuration so just acknowledge the request, then initiate first rx
			LOG_SETUP_SET_CONFIGURATION();
			MICO_USB_EP_TX_DATA1_PID(ep0_in, 0);
			if(! configured) {
				configured = true; 
				MICO_USB_EP_RX(ep1_out, ep1_out_MAX_PACKET_SIZE);
			}
			return;
		}
		LOG_SETUP_UNK();
		MICO_USB_EP_TX_DATA1_PID(ep0_in, 0);
		return;
	case MICO_USB_DIR_IN:
        switch(MICO_USB_SETUP_PACKET_REQUEST) {
		case MICO_USB_REQUEST_GET_DESCRIPTOR:
            switch(MICO_USB_SETUP_PACKET_VALUE_DESCRIPTOR_TYPE) {
			case MICO_USB_DT_DEVICE:
				LOG_SETUP_GET_DESCRIPTOR_DEVICE();
				MICO_USB_EP_TX_MEMCPY_DATA1_PID(ep0_in,
												MIN(sizeof(device_descriptor), MICO_USB_SETUP_PACKET_LENGTH),
												&device_descriptor);
				return;
			case MICO_USB_DT_CONFIG: 
				LOG_SETUP_GET_DESCRIPTOR_CONFIG();
				MICO_USB_EP_TX_MEMCPY_DATA1_PID(ep0_in,
												MIN(sizeof(usb_configuration), MICO_USB_SETUP_PACKET_LENGTH),
												&usb_configuration);
				return;
			case MICO_USB_DT_STRING:
				uint8_t i = MICO_USB_SETUP_PACKET_VALUE_STRING_INDEX;
				if(i < (sizeof(usb_stringV) / sizeof(*usb_stringV))) {
					LOG_SETUP_GET_DESCRIPTOR_STRING(i);
					MICO_USB_EP_TX_MEMCPY_DATA1_PID(ep0_in,
													MIN(usb_stringV[i]->bLength, MICO_USB_SETUP_PACKET_LENGTH),
													usb_stringV[i]);
					return;
				}
				break;
            }
			break;
        }
		break;
	}
	LOG_SETUP_UNK();
}

/**
 * @brief USB interrupt handler
 *
 */
/// \tag::isr_setup_packet[]
void isr_usbctrl(void) {
    // USB interrupt handler
    uint32_t status = usb_hw->ints;
    uint32_t handled = 0;

    // Setup packet received
    if (status & USB_INTS_SETUP_REQ_BITS) {
        handled |= USB_INTS_SETUP_REQ_BITS;
        MICO_ALIAS_CLR(usb_hw->sie_status) = USB_SIE_STATUS_SETUP_REC_BITS;
        usb_handle_setup_packet();
    }
/// \end::isr_setup_packet[]

    // Buffer status, one or more buffers have completed
    if (status & USB_INTS_BUFF_STATUS_BITS) {
        handled |= USB_INTS_BUFF_STATUS_BITS;

		uint bit;
		if((bit = /*EP0_IN*/1<<0) & usb_hw->buf_status) {
			MICO_ALIAS_CLR(usb_hw->buf_status) = bit;
			if (dev_addr_set) {
				// Set actual device address in hardware
				usb_hw->dev_addr_ctrl = dev_addr;
				dev_addr_set = false;
			} else {
				// Receive a zero length status packet from the host on EP0 OUT
				MICO_USB_EP_RX(ep0_out, 0);
			}
		}
		if((bit = /*EP0_OUT*/1<<1) & usb_hw->buf_status) {
			MICO_ALIAS_CLR(usb_hw->buf_status) = bit;
			// nop
		}
		if((bit = /*EP1_OUT*/1<<3) & usb_hw->buf_status) {
			MICO_ALIAS_CLR(usb_hw->buf_status) = bit;
			// Send data back to host
			MICO_USB_EP_TX_MEMCPY(ep2_in, MICO_USB_EP_BUF_CTRL_OUT_LEN_0(ep1_out), MICO_USB_EP_BUF(ep1_out));
		}
		if((bit = /*EP2_IN*/1<<4) & usb_hw->buf_status) {
			MICO_ALIAS_CLR(usb_hw->buf_status) = bit;
			// Get ready to rx again from host
			MICO_USB_EP_RX(ep1_out, ep1_out_MAX_PACKET_SIZE);
		}
		// todo deal with remaining usb_hw->buf_status
	}

    // Bus is reset
    if (status & USB_INTS_BUS_RESET_BITS) {
		LOG_BUS_RESET();
        handled |= USB_INTS_BUS_RESET_BITS;
        MICO_ALIAS_CLR(usb_hw->sie_status) = USB_SIE_STATUS_BUS_RESET_BITS;
		// Set address back to 0
		usb_hw->dev_addr_ctrl = dev_addr = 0;
		dev_addr_set = false;
		configured = false;
    }

    if (status ^ handled) {
        panic("Unhandled IRQ 0x%x\n", (uint) (status ^ handled));
    }
}


/**
 * @brief main
 *
 */

int main(void) {
	mico_usb_device_init0();
	MICO_USB_EP_CTRL_OUT(ep1_out) = MICO_USB_EP_CTRL_INITIALIZER(ep1_out);
	MICO_USB_EP_CTRL_IN(ep2_in) = MICO_USB_EP_CTRL_INITIALIZER(ep2_in);
	mico_usb_device_init1();

    for(;;) tight_loop_contents();
}
