/**
 *
 * $Id: drvwrapper.h 24695 2013-11-10 16:36:47Z monich $
 *
 * VGA2USB driver
 *
 * Copyright (C) 2003-2013 Epiphan Systems Inc. All rights reserved.
 *
 * USB+V4L driver framework
 *
 */

#ifndef __DRVWRAPPER_H
#define __DRVWRAPPER_H

#include "drvwrapper_lib.h"

// Driver description structures
extern struct video_device vga2usb_v4l;
extern struct usb_driver vga2usb_driver;

// Module load/unload
int vga2usb_init(void);
void vga2usb_exit(void);

// USB driver API
#ifdef __LINUX_USB_H
int vga2usb_probe(struct usb_interface *interface, const struct usb_device_id *id);
void vga2usb_disconnect(struct usb_interface * interface);

// Kernel USB API
int wrp_usb_register(struct usb_driver *);
void wrp_usb_deregister(struct usb_driver *);
int wrp_usb_control_msg(struct usb_device * dev, 
			int reqtype, int req, 
			int val, int idx, 
			unsigned char * buf, int buflen, 
			int hztimeout);
int wrp_reset_pipe(struct usb_device *dev, int ep);
void * wrp_urb_get_context(const struct urb * urb);
int wrp_urb_get_status(const struct urb * urb);
int wrp_urb_get_actual_length(const struct urb * urb);
void wrp_urb_add_transfer_flag(struct urb * urb, int flg);
struct urb *wrp_usb_alloc_urb(int iso_pkt);
int wrp_usb_submit_urb(struct urb *urb);
int wrp_usb_unlink_urb(struct urb *urb);
void wrp_usb_free_urb(struct urb *urb);
void wrp_usb_fill_bulk_urb_ep(struct urb *urb, struct usb_device *dev, 
			      int ep, void * buf, int size, 
			      usb_complete_t complete, void * context);
int wrp_usb_set_configuration(struct usb_device *dev, int configuration);

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,5))
struct kref {
	atomic_t refcount;
};
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0))
void wrp_usb_put_dev(struct usb_device *dev);
struct usb_interface *wrp_usb_find_interface(struct usb_driver *drv, int minor);
int wrp_usb_register_dev(struct usb_interface *intf,struct usb_class_driver *class_driver);
void wrp_usb_deregister_dev(struct usb_interface *intf, struct usb_class_driver *class_driver);
void wrp_usb_set_intfdata(struct usb_interface * interface, void *data);
void * wrp_usb_get_intfdata(struct usb_interface * interface);
#endif /* LINUX >= 2.6 */
#endif // __LINUX_USB_H

#endif // __DRVWRAPPER_H
