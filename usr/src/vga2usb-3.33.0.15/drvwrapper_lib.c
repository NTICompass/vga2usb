/**
 *
 * $Id: drvwrapper_lib.c 23414 2013-08-09 17:08:26Z monich $
 *
 * VGA2USB driver
 *
 * Copyright (C) 2003-2013 Epiphan Systems, Inc. All rights reserved.
 *
 * USB+V4L driver framework
 *
 */

//#include <linux/config.h>
#include <linux/version.h>
#include <linux/module.h>
#ifdef ENABLE_V4L
#include <linux/videodev.h>
#endif
#include <linux/sched.h>
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,9,0))
#include <linux/sched/rt.h>
#endif
#include <linux/slab.h>
#include <linux/usb.h>
#include <linux/vmalloc.h>
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0))
#include <linux/wrapper.h>
#endif
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,18))
#include <media/v4l2-common.h>
#endif
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27))
#include <linux/mm.h>
#endif
#include "drvwrapper_lib.h"



void wrp_wmb() {wmb();}
void wrp_rmb() {rmb();}
void wrp_wake_up(void *l) {wake_up(l);}
void wrp_init_waitqueue_head(void *x) {init_waitqueue_head(x);}
void wrp_add_wait_queue(void *x, void *y) {add_wait_queue(x,y);}
void wrp_remove_wait_queue(void *x, void *y) {remove_wait_queue(x,y);}
void wrp_set_current_state(long x) {set_current_state(x);}
signed long wrp_schedule_timeout(signed long timeout) {return schedule_timeout(timeout);}
unsigned long wrp_copy_to_user(void *d, const void *s, unsigned long l) {return copy_to_user(d,s,l);}
unsigned long wrp_copy_from_user(void *d, const void *s, unsigned long l) {return copy_from_user(d,s,l);}


#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0))
int wrp_set_user_nice(int n) {
#ifdef FC_KERNEL
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,4,22)
	int p = current->static_prio - MAX_RT_PRIO - 20;
#else
	int p = task_nice(current);
#endif
	set_user_nice(current, n);
#else
	int p = current->nice;
	current->nice = n;
#endif
	return p;
}
#else /* LINUX < 2.6 */
int wrp_set_user_nice(int n) {
	int p = current->static_prio - MAX_RT_PRIO - 20;
	set_user_nice(current, n);
	return p;
}
#endif

#ifdef ENABLE_V4L
int wrp_video_register_device(struct video_device *d, int t, int nr) {return video_register_device(d, t, nr);}
void wrp_video_unregister_device(struct video_device *d) {video_unregister_device(d);}
#endif


void *wrp_rvmalloc(unsigned long size)
{
         void *mem;
         unsigned long adr;
         size = PAGE_ALIGN(size);
         mem = vmalloc_32(size);
         if (!mem)
                 return NULL;
 
         memset(mem, 0, size); /* Clear the ram out, no junk to the user */
         adr = (unsigned long) mem;
         while (size > 0) {
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0))
		 mem_map_reserve(vmalloc_to_page((void *)adr));
#else
                 SetPageReserved(vmalloc_to_page((void *)adr));
#endif
                 adr += PAGE_SIZE;
                 size -= PAGE_SIZE;
         }
 
         return mem;
}
 
void wrp_rvfree(void *mem, unsigned long size)
{
         unsigned long adr;
 
         if (!mem)
                 return;
 
         adr = (unsigned long) mem;
         while ((long) size > 0) {
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0))
		 mem_map_unreserve(vmalloc_to_page((void *)adr));
#else
                 ClearPageReserved(vmalloc_to_page((void *)adr));
#endif
                 adr += PAGE_SIZE;
                 size -= PAGE_SIZE;
         }
         vfree(mem);
}

int wrp_remap_page_range(struct vm_area_struct *vma, unsigned long from,
						unsigned long pfn_to, unsigned long size, pgprot_t prot)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,10))
	return remap_pfn_range(vma,from,pfn_to,size,prot);
#else
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,4,21) || LINUX_VERSION_CODE > KERNEL_VERSION(2,6,0)
	return remap_page_range(vma,from,pfn_to,size,prot);
#else
	return remap_page_range(from,pfn_to,size,prot);
#endif
#endif
}
unsigned long wrp_vmalloc_to_page(void *addr){ 
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,12))
	return vmalloc_to_pfn(addr); 
#else
        unsigned long kva, ret;
        kva = (unsigned long) page_address(vmalloc_to_page(addr));
        kva |= ((long)addr) & (PAGE_SIZE-1); /* restore the offset */
        ret = __pa(kva);
        return ret;
#endif
}

unsigned long wrp__get_free_pages(int priority, unsigned long gfporder) 
{
	unsigned long result = __get_free_pages(priority, gfporder);
	return result;
}
void wrp_free_pages(unsigned long addr, unsigned long order) 
{
	return free_pages(addr, order);
}
void * wrp_vmalloc(unsigned long size) {return vmalloc(size);}
void wrp_vfree(const void *addr) {vfree(addr);}
void *wrp_kmalloc (size_t size, int priority) {return kmalloc(size,priority);}
void wrp_kfree(const void *obj) {kfree(obj);}
unsigned long get_jiffies() {return jiffies;}

int wrp_printk(const char *fmt, ...) 
{
	va_list args;
	static char printk_buf[1024];
	int printed_len;

	va_start(args, fmt);
	printed_len = vsnprintf(printk_buf, sizeof(printk_buf), fmt, args);
	va_end(args);
	if(printed_len>0) 
		printed_len = printk(printk_buf);
	return printed_len;
}

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0))

/**
 * Wrappers and video_device structure template describing video device for V4L.
 *   The only parameter that needs to be changed after copying the template
 *   is the priv.
 */
static int wrp_v4l_ioctl(struct video_device *dev, unsigned int ioctlcode , void *arg) {return vga2usb_v4l_ioctl(dev->priv, ioctlcode, arg);}
static int wrp_v4l_open(struct video_device *dev, int flags) {return vga2usb_v4l_open(dev->priv, flags);}
static void wrp_v4l_close(struct video_device *dev) {return vga2usb_v4l_close(dev->priv);}
static long wrp_v4l_read(struct video_device *dev, char *buf, unsigned long count, int noblock) {return -EINVAL;}
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,4,20)
static int wrp_v4l_mmap(struct vm_area_struct *vma, struct video_device *dev, const char *adr, unsigned long size) {return vga2usb_v4l_mmap(vma, dev->priv, adr, size);}
#else
static int wrp_v4l_mmap(struct video_device *dev, const char *adr, unsigned long size) {return vga2usb_v4l_mmap(NULL, dev->priv, adr, size);}
#endif
struct video_device vga2usb_v4l = {
	owner:		THIS_MODULE,
	name:		"Epiphan Frame Grabber",
	type:		VID_TYPE_CAPTURE,
	hardware:	VID_HARDWARE_PSEUDO,
	ioctl:		wrp_v4l_ioctl,
	read:		wrp_v4l_read,
	open:		wrp_v4l_open,
	close:		wrp_v4l_close,
	mmap:		wrp_v4l_mmap,
	minor:		-1,
};

#endif /* LINUX < 2.6 */

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,5))
void wrp_kref_init(struct kref *kref, void (*delete)(struct kref *kref)){kref_init(kref);}
void wrp_kref_get(struct kref *kref){kref_get(kref);}
void wrp_kref_put(struct kref *kref, void (*delete)(struct kref *kref)){kref_put(kref,delete);}
#else
void wrp_kref_init(struct kref *kref, void (*delete)(struct kref *kref)){atomic_set(&kref->refcount,1);}
void wrp_kref_get(struct kref *kref){ atomic_inc(&kref->refcount); }
int wrp_kref_put(struct kref *kref, void (*delete)(struct kref *kref)){ if( atomic_dec_and_test(&kref->refcount) ) {delete(kref); return 1;} else { return 0;} }
#endif
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0))
#ifdef ENABLE_V4L
struct video_device* wrp_video_devdata(struct file* f){return video_devdata(f);}
struct video_device * wrp_video_device_alloc(void) {return video_device_alloc();}
void wrp_video_device_release(struct video_device *vfd){video_device_release(vfd);}
void * wrp_video_get_drvdata(struct video_device *dev){return video_get_drvdata(dev);}
void wrp_video_set_drvdata(struct video_device *dev, void * data) {video_set_drvdata(dev,data);}
#endif

#endif
