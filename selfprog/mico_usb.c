#include "mico_usb.h"

#include "hardware/resets.h"

#include <string.h> // memset

void mico_usb_device_init0() {
    // Reset usb controller
	MICO_ALIAS_SET(resets_hw->reset) = RESETS_RESET_USBCTRL_BITS;
	MICO_ALIAS_CLR(resets_hw->reset) = RESETS_RESET_USBCTRL_BITS;
	while(! (resets_hw->reset_done & RESETS_RESET_USBCTRL_BITS));

    // Clear any previous state in dpram just in case
    memset(usb_dpram, 0, sizeof(*usb_dpram)); // <1>

    // Enable USB interrupt at processor
	mico_irq_clrpend_enable(1<<USBCTRL_IRQ);

    // Mux the controller to the onboard usb phy
    usb_hw->muxing = USB_USB_MUXING_TO_PHY_BITS | USB_USB_MUXING_SOFTCON_BITS;

    // Force VBUS detect so the device thinks it is plugged into a host
    usb_hw->pwr = USB_USB_PWR_VBUS_DETECT_BITS | USB_USB_PWR_VBUS_DETECT_OVERRIDE_EN_BITS;

    // Enable the USB controller in device mode.
    usb_hw->main_ctrl = USB_MAIN_CTRL_CONTROLLER_EN_BITS;

    // Enable an interrupt per EP0 transaction
    usb_hw->sie_ctrl = USB_SIE_CTRL_EP0_INT_1BUF_BITS; // <2>

    // Enable interrupts for when a buffer is done, when the bus is reset,
    // and when a setup packet is received
    usb_hw->inte = USB_INTS_BUFF_STATUS_BITS |
                   USB_INTS_BUS_RESET_BITS |
                   USB_INTS_SETUP_REQ_BITS;
}
