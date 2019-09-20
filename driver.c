
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/mm.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL v2");

static const char driver_name[] = "nan";
static const char device_name[] = "nan64";
static struct class *nan_class = NULL;
static dev_t nan_device = 0;
static struct cdev nan_cdev;

struct kmem_cache * nan_mem_cache = NULL;

// the page of NaNs
static char *nan_mem = NULL;

static int nan_mmap(struct file *filp, struct vm_area_struct *vma) {
  int res = 0;
  unsigned long pfn = virt_to_phys((void *)nan_mem)>>PAGE_SHIFT;
  unsigned long base = 0;

  for (base = vma->vm_start; base < vma->vm_end; base += PAGE_SIZE) {
    res = remap_pfn_range(
      vma,
      base,
      pfn, 
      min(PAGE_SIZE, vma->vm_end - base), 
      vma->vm_page_prot);
    if (res < 0) {
      printk("can't alloc %d\n", res);
      return -EIO;
    }
  }

  return res;
}

static struct file_operations nan_fops = {
    .owner = THIS_MODULE,
    .mmap = nan_mmap,
};

static char *nan_devnode(struct device *dev, umode_t *mode) {
  if (mode)
    *mode = 0444;
  return NULL;
}

static int __init register_device(void) {
  int res = 0;
  void *res_ptr = NULL;

  res = alloc_chrdev_region(&nan_device, 0, 1, driver_name) < 0;
  if (res < 0)
    goto err1;

  nan_class = class_create(THIS_MODULE, driver_name);
  if (IS_ERR(nan_class)) {
    res = PTR_ERR(nan_class);
    goto err2;
  }
  nan_class->devnode = nan_devnode;

  res_ptr = device_create(nan_class, NULL, nan_device, NULL, device_name);
  if (IS_ERR(res_ptr))
    goto err3;

  cdev_init(&nan_cdev, &nan_fops);

  res = cdev_add(&nan_cdev, nan_device, 1);
  if (res < 0)
    goto err4;

  nan_mem_cache = kmem_cache_create(
    driver_name,
    PAGE_SIZE,
    PAGE_SIZE,
    SLAB_POISON | SLAB_RED_ZONE | SLAB_HWCACHE_ALIGN,
    NULL);
  if (!nan_mem_cache)
    goto err4;

  nan_mem = kmem_cache_alloc(nan_mem_cache, GFP_KERNEL);  
  if (!nan_mem)
    goto err5;
  SetPageReserved(virt_to_page((unsigned long)nan_mem));

  return res;

err5:
  kmem_cache_destroy(nan_mem_cache);
err4:
  device_destroy(nan_class, nan_device);
err3:
  class_destroy(nan_class);
err2:
  unregister_chrdev_region(nan_device, 1);
err1:
  return res;
}

static void __exit unregister_device(void) {
  if (nan_mem) {
    ClearPageReserved(virt_to_page((unsigned long)nan_mem));
    kmem_cache_free(nan_mem_cache, nan_mem);
  }

  if (nan_mem_cache) {
    kmem_cache_destroy(nan_mem_cache);
  }

  cdev_del(&nan_cdev);
  device_destroy(nan_class, nan_device);
  if (nan_class)
    class_destroy(nan_class);
  unregister_chrdev_region(nan_device, 1);
}

module_init(register_device);
module_exit(unregister_device);