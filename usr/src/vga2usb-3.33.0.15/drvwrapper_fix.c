#include <linux/version.h>

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5,10,0) || LINUX_VERSION_CODE <= KERNEL_VERSION(5,8,18))
#include <linux/dma-mapping.h>
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5,9,0) && LINUX_VERSION_CODE <= KERNEL_VERSION(5,9,16))
#include <linux/dma-direct.h>
#endif

#include "drvwrapper_fix.h"

#if (LINUX_VERSION_CODE > KERNEL_VERSION(5,5,19))
void __put_devmap_managed_page(struct page *page)
{
    put_devmap_managed_page(page);
}
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5,10,0))
int dma_direct_map_sg(struct device *dev, struct scatterlist *sgl, int nents, enum dma_data_direction dir, unsigned long attrs)
{
    return dma_map_sg_attrs(dev, sgl, nents, dir, attrs);
}

void dma_direct_unmap_sg(struct device *dev, struct scatterlist *sgl, int nents, enum dma_data_direction dir, unsigned long attrs)
{
    dma_unmap_sg_attrs(dev, sgl, nents, dir, attrs);
}

dma_addr_t dma_direct_map_page(struct device *dev, struct page *page, unsigned long offset, size_t size, enum dma_data_direction dir, unsigned long attrs)
{
    return dma_map_page_attrs(dev, page, offset, size, dir, attrs);
}

void dma_direct_unmap_page(struct device *dev, dma_addr_t addr, size_t size, enum dma_data_direction dir, unsigned long attrs)
{
    dma_unmap_page_attrs(dev, addr, size, dir, attrs);
}
#endif

#if (LINUX_VERSION_CODE > KERNEL_VERSION(5,4,160))
bool ex_handler_refcount(const struct exception_table_entry *fixup, struct pt_regs *regs, int trapnr, unsigned long error_code, unsigned long fault_addr)
{
    return true;
    // TODO
    //return ex_handler_default(fixup, regs, trapnr, error_code, fault_addr);
}
#endif
