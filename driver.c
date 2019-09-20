
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL v2");

static const char driver_name[] = "nan";
static const char device_name[] = "nan64";
static struct class *nan_class = NULL;
static dev_t nan_device = 0;
static struct cdev nan_cdev;

// the page of NaNs
static double *nan_mem = NULL;

int nan_fault(struct vm_fault *vmf) {
  struct page *page;
  page = virt_to_page(nan_mem);
  get_page(page);
  vmf->page = page;
  return 0;
}

struct vm_operations_struct mmap_vm_ops = {
    .fault = nan_fault,
};

static int nan_mmap(struct file *filp, struct vm_area_struct *vma) {
  vma->vm_ops = &mmap_vm_ops;
  vma->vm_flags |= VM_IO;
  return 0;
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
  int res = 0, i = 0;
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

  nan_mem = (double *)get_zeroed_page(GFP_KERNEL);
  SetPageReserved(virt_to_page((unsigned long)nan_mem));

  for (i = 0; i < PAGE_SIZE / sizeof(double); ++i)
    nan_mem[i] = __builtin_nanf("");

  return res;

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
    free_page((unsigned long)nan_mem);
    nan_mem = NULL;
  }

  cdev_del(&nan_cdev);
  device_destroy(nan_class, nan_device);
  if (nan_class)
    class_destroy(nan_class);
  unregister_chrdev_region(nan_device, 1);
}

module_init(register_device);
module_exit(unregister_device);