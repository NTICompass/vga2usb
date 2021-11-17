/**
 *
 * $Id: drvwrapper_lib.h 23414 2013-08-09 17:08:26Z monich $
 *
 * VGA2USB driver
 *
 * Copyright (C) 2003-2013 Epiphan Systems, Inc. All rights reserved.
 *
 * USB+V4L driver framework
 *
 */

#ifndef __DRVWRAPPER_LIB_H
#define __DRVWRAPPER_LIB_H

// Module Parameters
extern int firmware_autoload;
extern int grab_og;
extern int no_fine_tune;
extern int scalemode;

#ifdef ENABLE_V4L

// V4L driver API
struct video_device * wrp_video_device_alloc(void);
int wrp_video_register_device(struct video_device *, int type, int nr);
void wrp_video_unregister_device(struct video_device *);

void * wrp_video_get_drvdata(struct video_device *dev);
void wrp_video_set_drvdata(struct video_device *dev, void * data);
#endif

int wrp_remap_page_range(struct vm_area_struct *vma, unsigned long from, unsigned long to, unsigned long size, pgprot_t prot);
unsigned long wrp_vmalloc_to_page(void *addr);
void *wrp_rvmalloc(unsigned long size);
void wrp_rvfree(void *mem, unsigned long size);

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0))
void wrp_kref_init(struct kref *kref, void (*delete)(struct kref *kref));
void wrp_kref_get(struct kref *kref);
void wrp_kref_put(struct kref *kref, void (*delete)(struct kref *kref));
#endif /* LINUX >= 2.6 */

void wrp_wmb(void);
void wrp_rmb(void);
void wrp_wake_up(void *x);
void wrp_init_waitqueue_head(void *x);
void wrp_add_wait_queue(void *x, void *y);
void wrp_remove_wait_queue(void *x, void *y);
void wrp_set_current_state(long x);
signed long wrp_schedule_timeout(signed long timeout);
int wrp_set_user_nice(int n);
unsigned long wrp_copy_to_user(void *, const void *, unsigned long);
unsigned long wrp_copy_from_user(void *, const void *, unsigned long);

unsigned long wrp__get_free_pages(int priority, unsigned long gfporder);
void wrp_free_pages(unsigned long addr, unsigned long order);
void * wrp_vmalloc(unsigned long size);
void wrp_vfree(const void *addr);
void *wrp_kmalloc (size_t size, int priority);
void wrp_kfree(const void *obj);
unsigned long get_jiffies(void);
unsigned long wrp_wait_for_completion_timeout(struct completion *x, unsigned long timeout);

int wrp_printk(const char *fmt, ...);

#if (LINUX_VERSION_CODE > KERNEL_VERSION(5,5,19))
void __put_devmap_managed_page(struct page *page);
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5,10,0))
int dma_direct_map_sg(struct device *dev, struct scatterlist *sgl, int nents, enum dma_data_direction dir, unsigned long attrs);
void dma_direct_unmap_sg(struct device *dev, struct scatterlist *sgl, int nents, enum dma_data_direction dir, unsigned long attrs);

dma_addr_t dma_direct_map_page(struct device *dev, struct page *page, unsigned long offset, size_t size, enum dma_data_direction dir, unsigned long attrs);
void dma_direct_unmap_page(struct device *dev, dma_addr_t addr, size_t size, enum dma_data_direction dir, unsigned long attrs);
#endif

#endif // __DRVWRAPPER_LIB_H
