/**
 *
 * $Id: drvwrapper.c 37433 2017-01-29 10:52:05Z monich $
 *
 * VGA2USB driver
 *
 * Copyright (C) 2003-2017 Epiphan Systems, Inc. All rights reserved.
 *
 * USB+V4L driver framework
 *
 */

#include <linux/version.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/usb.h>
#include <linux/vmalloc.h>
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0))
#include <linux/wrapper.h>
#endif
#include "v2u_id.h"
#include "drvwrapper.h"

/**
 * VGA2USB usb device codes
 */
#define VGA2USB_EVAL_VENDORID		0x04b4
#ifdef VGA2USB_EVAL
#define VGA2USB_EVAL_PRODID		0x8613
#endif
#ifdef VGA2USB_EVAL3
#define VGA2USB_EVAL3_PRODID1		0x00F3
#define VGA2USB_EVAL3_PRODID2		0x00F0
#define VGA2USB_PRODID_DVI2USB3_UINIT   0x3550
#define VGA2USB_PRODID_SDI2USB3_UINIT   0x3551
/* second stage bootloader from UVC product when there is no product id in
	 EEPROM */
#define VGA2USB_PRODID_BOOT2    0x3600
#endif

/**
 * 
 */
int wrp_usb_control_msg(struct usb_device * dev, 
			int reqtype, int req, 
			int val, int idx, 
			unsigned char * buf, int buflen, 
			int hztimeout) 
{
	int result = 0;
	char * kbuf = NULL;

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,15)
	int timeout = hztimeout;
#else
	int timeout = hztimeout*1000/HZ;
#endif

	if(!dev)
		return -ENODEV;

	if(buflen>0) {
		kbuf = kmalloc(buflen,GFP_KERNEL);
		if(!kbuf) 
			return -ENOMEM;
		
		if( !(reqtype & USB_DIR_IN) )
			memcpy(kbuf,buf,buflen);
	}

	result = usb_control_msg(dev,
				 (reqtype & USB_DIR_IN ? 
				  usb_rcvctrlpipe(dev,0) : 
				  usb_sndctrlpipe(dev,0)), 
				 req,reqtype,
				 val,idx,kbuf,buflen,
				 timeout);
	
	if( (result > 0) && (reqtype & USB_DIR_IN) && kbuf )
		memcpy(buf,kbuf,result);

	if(kbuf)
		kfree(kbuf);

	return result;
}

int wrp_reset_pipe(struct usb_device *dev, int ep)
{
	int result = 0;
	
	if(dev)
		result = usb_clear_halt(dev, usb_rcvbulkpipe(dev,ep));
	else
		result = -ENODEV;

	return result;
}

void * wrp_urb_get_context(const struct urb * urb) {return urb->context;}
int wrp_urb_get_status(const struct urb * urb) {return urb->status;}
int wrp_urb_get_actual_length(const struct urb * urb) {return urb->actual_length;}
void wrp_urb_add_transfer_flag(struct urb * urb, int flg) {urb->transfer_flags |= flg;}
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0))
struct urb *wrp_usb_alloc_urb(int iso_pkt) {return usb_alloc_urb(iso_pkt);}
int wrp_usb_submit_urb(struct urb *urb) {return usb_submit_urb(urb);}
#else
struct urb *wrp_usb_alloc_urb(int iso_pkt) {return usb_alloc_urb(iso_pkt,GFP_KERNEL);}
int wrp_usb_submit_urb(struct urb *urb) {return usb_submit_urb(urb,GFP_KERNEL);}
#endif
int wrp_usb_unlink_urb(struct urb *urb) {return usb_unlink_urb(urb);}
void wrp_usb_free_urb(struct urb *urb) {usb_free_urb(urb);}
void wrp_usb_fill_bulk_urb_ep(struct urb *urb, struct usb_device *dev, 
			      int ep, void * buf, int size, 
			      usb_complete_t complete, void * context)
{
	usb_fill_bulk_urb(urb, dev, usb_rcvbulkpipe(dev,ep), buf, size, complete, context);
}

int wrp_usb_register(struct usb_driver *d) {return usb_register(d);}
void wrp_usb_deregister(struct usb_driver *d) {usb_deregister(d);}
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0))
int wrp_usb_set_configuration(struct usb_device *dev, int configuration) {return usb_set_configuration(dev, configuration);}
#endif

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0))
static int wrp_ioctl(struct usb_device * usbdev, unsigned int code, void * userbuf) {return -ENODEV;}
static void * wrp_probe(struct usb_device * dev, unsigned int ifnum, const struct usb_device_id * id) {return vga2usb_probe(dev, ifnum, id);}
static void wrp_disconnect(struct usb_device * dev, void * ptr) {return vga2usb_disconnect(dev, ptr);}
#else /* LINUX < 2.6 */
static int wrp_ioctl(struct usb_interface *intf, unsigned int code, void *buf) {return -ENODEV;}
static int wrp_probe(struct usb_interface *interface, const struct usb_device_id *id) {return vga2usb_probe(interface, id);}
static void wrp_disconnect(struct usb_interface * interface) {vga2usb_disconnect(interface);}
#endif /* LINUX < 2.6 */


/**
 * Desribes to the kernel devices supported by this modules
 */
static struct usb_device_id vga2usb_table [] = {
	{ 
		idVendor: VGA2USB_VENDORID,
		idProduct: VGA2USB_PRODID_VGA2USB,
		match_flags: USB_DEVICE_ID_MATCH_VENDOR | USB_DEVICE_ID_MATCH_PRODUCT,
	},
	{ 
		idVendor: VGA2USB_VENDORID,
		idProduct: VGA2USB_PRODID_KVM2USB,
		match_flags: USB_DEVICE_ID_MATCH_VENDOR | USB_DEVICE_ID_MATCH_PRODUCT,
	},
	{ 
		idVendor: VGA2USB_VENDORID,
		idProduct: VGA2USB_PRODID_DVI2USB,
		match_flags: USB_DEVICE_ID_MATCH_VENDOR | USB_DEVICE_ID_MATCH_PRODUCT,
	},
	{ 
		idVendor: VGA2USB_VENDORID,
		idProduct: VGA2USB_PRODID_VGA2USB_PRO,
		match_flags: USB_DEVICE_ID_MATCH_VENDOR | USB_DEVICE_ID_MATCH_PRODUCT,
	},
	{ 
		idVendor: VGA2USB_VENDORID,
		idProduct: VGA2USB_PRODID_VGA2USB_HR,
		match_flags: USB_DEVICE_ID_MATCH_VENDOR | USB_DEVICE_ID_MATCH_PRODUCT,
	},
	{ 
		idVendor: VGA2USB_VENDORID,
		idProduct: VGA2USB_PRODID_VGA2USB_LR,
		match_flags: USB_DEVICE_ID_MATCH_VENDOR | USB_DEVICE_ID_MATCH_PRODUCT,
	},
	{ 
		idVendor: VGA2USB_VENDORID,
		idProduct: VGA2USB_PRODID_KVM2USB_PRO,
		match_flags: USB_DEVICE_ID_MATCH_VENDOR | USB_DEVICE_ID_MATCH_PRODUCT,
	},
	{ 
		idVendor: VGA2USB_VENDORID,
		idProduct: VGA2USB_PRODID_KVM2USB_LR,
		match_flags: USB_DEVICE_ID_MATCH_VENDOR | USB_DEVICE_ID_MATCH_PRODUCT,
	},
	{ 
		idVendor: VGA2USB_VENDORID,
		idProduct: VGA2USB_PRODID_DVI2USB_SOLO,
		match_flags: USB_DEVICE_ID_MATCH_VENDOR | USB_DEVICE_ID_MATCH_PRODUCT,
	},
	{ 
		idVendor: VGA2USB_VENDORID,
		idProduct: VGA2USB_PRODID_DVI2USB_DUO,
		match_flags: USB_DEVICE_ID_MATCH_VENDOR | USB_DEVICE_ID_MATCH_PRODUCT,
	},
	{ 
		idVendor: VGA2USB_VENDORID,
		idProduct: VGA2USB_PRODID_DVI2USB_RESPIN,
		match_flags: USB_DEVICE_ID_MATCH_VENDOR | USB_DEVICE_ID_MATCH_PRODUCT,
	},
	{ 
		idVendor: VGA2USB_VENDORID,
		idProduct: VGA2USB_PRODID_VGA2USB_LR_RESPIN,
		match_flags: USB_DEVICE_ID_MATCH_VENDOR | USB_DEVICE_ID_MATCH_PRODUCT,
	},
	{ 
		idVendor: VGA2USB_VENDORID,
		idProduct: VGA2USB_PRODID_VGA2USB_HR_RESPIN,
		match_flags: USB_DEVICE_ID_MATCH_VENDOR | USB_DEVICE_ID_MATCH_PRODUCT,
	},
	{ 
		idVendor: VGA2USB_VENDORID,
		idProduct: VGA2USB_PRODID_VGA2USB_PRO_RESPIN,
		match_flags: USB_DEVICE_ID_MATCH_VENDOR | USB_DEVICE_ID_MATCH_PRODUCT,
	},
	{ 
		idVendor: VGA2USB_VENDORID,
		idProduct: VGA2USB_PRODID_KVM2USB_LR_RESPIN,
		match_flags: USB_DEVICE_ID_MATCH_VENDOR | USB_DEVICE_ID_MATCH_PRODUCT,
	},
	{ 
		idVendor: VGA2USB_VENDORID,
		idProduct: VGA2USB_PRODID_DVI2USB3,
		bInterfaceClass: USB_CLASS_VENDOR_SPEC,
		match_flags: USB_DEVICE_ID_MATCH_VENDOR | USB_DEVICE_ID_MATCH_PRODUCT | USB_DEVICE_ID_MATCH_INT_CLASS,
	},
	{ 
		idVendor: VGA2USB_VENDORID,
		idProduct: VGA2USB_PRODID_DVI2USB3_ET,
		bInterfaceClass: USB_CLASS_VENDOR_SPEC,
		match_flags: USB_DEVICE_ID_MATCH_VENDOR | USB_DEVICE_ID_MATCH_PRODUCT | USB_DEVICE_ID_MATCH_INT_CLASS,
	},
	{ 
		idVendor: VGA2USB_VENDORID,
		idProduct: VGA2USB_PRODID_SDI2USB3,
		bInterfaceClass: USB_CLASS_VENDOR_SPEC,
		match_flags: USB_DEVICE_ID_MATCH_VENDOR | USB_DEVICE_ID_MATCH_PRODUCT | USB_DEVICE_ID_MATCH_INT_CLASS,
	},
	{ 
		idVendor: VGA2USB_VENDORID,
		idProduct: VGA2USB_PRODID_SDI2USB3_GEN,
		match_flags: USB_DEVICE_ID_MATCH_VENDOR | USB_DEVICE_ID_MATCH_PRODUCT,
	},
	{ 
		idVendor: VGA2USB_VENDORID,
		idProduct: VGA2USB_PRODID_DVI2USB3_R3,
		bInterfaceClass: USB_CLASS_VENDOR_SPEC,
		match_flags: USB_DEVICE_ID_MATCH_VENDOR | USB_DEVICE_ID_MATCH_PRODUCT | USB_DEVICE_ID_MATCH_INT_CLASS,
	},
	{ 
		idVendor: VGA2USB_VENDORID,
		idProduct: VGA2USB_PRODID_DVI2USB3_ET_R3,
		bInterfaceClass: USB_CLASS_VENDOR_SPEC,
		match_flags: USB_DEVICE_ID_MATCH_VENDOR | USB_DEVICE_ID_MATCH_PRODUCT | USB_DEVICE_ID_MATCH_INT_CLASS,
	},
	{ 
		idVendor: VGA2USB_VENDORID,
		idProduct: VGA2USB_PRODID_SDI2USB3_R3,
		bInterfaceClass: USB_CLASS_VENDOR_SPEC,
		match_flags: USB_DEVICE_ID_MATCH_VENDOR | USB_DEVICE_ID_MATCH_PRODUCT | USB_DEVICE_ID_MATCH_INT_CLASS,
	},
	{ 
		idVendor: VGA2USB_VENDORID,
		idProduct: VGA2USB_PRODID_SDI2USB3_GEN_R3,
		match_flags: USB_DEVICE_ID_MATCH_VENDOR | USB_DEVICE_ID_MATCH_PRODUCT,
	},
	{ 
		idVendor: VGA2USB_VENDORID,
		idProduct: VGA2USB_PRODID_DVI2USB3_4K,
		match_flags: USB_DEVICE_ID_MATCH_VENDOR | USB_DEVICE_ID_MATCH_PRODUCT,
	},
#ifdef VGA2USB_EVAL
	{ 
		idVendor: VGA2USB_EVAL_VENDORID,
		idProduct: VGA2USB_EVAL_PRODID,
		match_flags: USB_DEVICE_ID_MATCH_VENDOR | USB_DEVICE_ID_MATCH_PRODUCT,
	},
#endif
#ifdef VGA2USB_EVAL3
	{ /* Epiphan bootloader, device id */
		idVendor: VGA2USB_VENDORID,
		idProduct: VGA2USB_EVAL3_PRODID1,
		match_flags: USB_DEVICE_ID_MATCH_VENDOR | USB_DEVICE_ID_MATCH_PRODUCT,
	},
	{ 
		idVendor: VGA2USB_EVAL_VENDORID,
		idProduct: VGA2USB_EVAL3_PRODID1,
		match_flags: USB_DEVICE_ID_MATCH_VENDOR | USB_DEVICE_ID_MATCH_PRODUCT,
	},
	{ 
		idVendor: VGA2USB_EVAL_VENDORID,
		idProduct: VGA2USB_EVAL3_PRODID2,
		match_flags: USB_DEVICE_ID_MATCH_VENDOR | USB_DEVICE_ID_MATCH_PRODUCT,
	},
	{ 
		idVendor: VGA2USB_VENDORID,
		idProduct: VGA2USB_PRODID_DVI2USB3_UINIT,
		match_flags: USB_DEVICE_ID_MATCH_VENDOR | USB_DEVICE_ID_MATCH_PRODUCT,
	},
	{ 
		idVendor: VGA2USB_VENDORID,
		idProduct: VGA2USB_PRODID_SDI2USB3_UINIT,
		match_flags: USB_DEVICE_ID_MATCH_VENDOR | USB_DEVICE_ID_MATCH_PRODUCT,
	},
	{ 
		idVendor: EPIPHAN_VENDORID2,
		idProduct: VGA2USB_PRODID_BOOT2,
		match_flags: USB_DEVICE_ID_MATCH_VENDOR | USB_DEVICE_ID_MATCH_PRODUCT,
	},
#endif
	{},
};

MODULE_DEVICE_TABLE(usb, vga2usb_table);

/**
 * Describes the driver to the usb subsystem of the kernel
 */ 
struct usb_driver vga2usb_driver = {
	name: "vga2usb",
	id_table: vga2usb_table,
	probe: wrp_probe,
	disconnect: wrp_disconnect,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
	unlocked_ioctl: wrp_ioctl,
#else
  ioctl: wrp_ioctl,
#endif
};

/**
 * Initializes the module
 */
int wrp_v2u_init(void)
{
#ifdef EXPORT_NO_SYMBOLS
	EXPORT_NO_SYMBOLS;
#endif
	return vga2usb_init();
}

/**
 * Cleanup module's resources
 */
void wrp_v2u_exit(void)
{
	vga2usb_exit();
}

/**
 * Claim our initalization and cleanup routines
 */
module_init(wrp_v2u_init);
module_exit(wrp_v2u_exit);

/**
 * Module info
 */
MODULE_AUTHOR ("Epiphan Systems");
MODULE_DESCRIPTION ("USB+V4L Extension Driver");
/* GPL license is granted for USB+V4L extension driver code: drvwrapper.h and drvwrapper.c 
 *  To support particular USB device, additional device specific library is required.
 *  Epiphan Systems allows to link this GPL code with proprietary device specific
 *  library vga2usb_bins.o which provides support for Epiphan's VGA/DVI frame grabber devices
 */
MODULE_LICENSE("GPL");

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0))
void wrp_usb_put_dev(struct usb_device *dev){usb_put_dev(dev);}
struct usb_interface *wrp_usb_find_interface(struct usb_driver *drv, int minor){return usb_find_interface(drv,minor);}
int wrp_usb_register_dev(struct usb_interface *intf,struct usb_class_driver *class_driver){return usb_register_dev(intf,class_driver);}
void wrp_usb_deregister_dev(struct usb_interface *intf, struct usb_class_driver *class_driver){usb_deregister_dev(intf,class_driver);}
/* kernels around 2.6.8 and before used "delete" parameter in initialization, but didn't use it in "put" */
void wrp_usb_set_intfdata(struct usb_interface * interface, void *data){usb_set_intfdata(interface,data);}
void * wrp_usb_get_intfdata(struct usb_interface * interface){return usb_get_intfdata(interface);}

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,11))
unsigned long wrp_wait_for_completion_timeout(struct completion *x, unsigned long timeout) {return wait_for_completion_timeout(x, timeout);}
#else
unsigned long wrp_wait_for_completion_timeout(struct completion *x, unsigned long timeout)
{
        might_sleep();

        spin_lock_irq(&x->wait.lock);
        if (!x->done) {
                DECLARE_WAITQUEUE(wait, current);

                wait.flags |= WQ_FLAG_EXCLUSIVE;
                __add_wait_queue_tail(&x->wait, &wait);
                do {
                        __set_current_state(TASK_UNINTERRUPTIBLE);
                        spin_unlock_irq(&x->wait.lock);
                        timeout = schedule_timeout(timeout);
                        spin_lock_irq(&x->wait.lock);
                        if (!timeout) {
                                __remove_wait_queue(&x->wait, &wait);
                                goto out;
                        }
                } while (!x->done);
                __remove_wait_queue(&x->wait, &wait);
        }
        x->done--;
out:
        spin_unlock_irq(&x->wait.lock);
        return timeout;
}
#endif

#endif
