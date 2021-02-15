#ifndef USB_SERIAL_H_
#define USB_SERIAL_H_
#include "stdint.h"

/**
 * \brief Initialise the Serial-over-USB driver
 */
void usb_serial_init(void);

/**
 * \brief Write a byte over USB
 * \param b The byte
 * \sa usb_serial_flush()
 *
 * USB-Serial output is buffered. The buffer is actually submitted to the USB
 * controller for transmission when:
 *  - It is full or
 *  - The code explicitly calls usb_serial_flush()
 */
void usb_serial_writeb(uint8_t b);

/**
 * \brief Set an input hook for bytes received over USB
 * \param input A pointer to a function to be called when a byte is received
 */
void usb_serial_set_input(int (*input)(unsigned char c));

/**
 * \brief Immediately transmit the content of Serial-over-USB TX buffers
 * \sa usb_serial_writeb()
 */
void usb_serial_flush(void);

#endif /* USB_SERIAL_H_ */

/** @} */