#ifndef __DRVWRAPPER_FIX_H
#define __DRVWRAPPER_FIX_H

#if (LINUX_VERSION_CODE > KERNEL_VERSION(5,5,19))
void __put_devmap_managed_page(struct page *page);
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5,10,0))
int dma_direct_map_sg(struct device *dev, struct scatterlist *sgl, int nents, enum dma_data_direction dir, unsigned long attrs);
void dma_direct_unmap_sg(struct device *dev, struct scatterlist *sgl, int nents, enum dma_data_direction dir, unsigned long attrs);

dma_addr_t dma_direct_map_page(struct device *dev, struct page *page, unsigned long offset, size_t size, enum dma_data_direction dir, unsigned long attrs);
void dma_direct_unmap_page(struct device *dev, dma_addr_t addr, size_t size, enum dma_data_direction dir, unsigned long attrs);
#endif

#if (LINUX_VERSION_CODE > KERNEL_VERSION(5,4,160))
bool ex_handler_refcount(const struct exception_table_entry *fixup, struct pt_regs *regs, int trapnr, unsigned long error_code, unsigned long fault_addr);
#endif

#endif
