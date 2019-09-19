#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>

MODULE_LICENSE ("GPL v2");

static const char driver_name[] = "nan";
static const char device_name[] = "nan64";
static struct class* nan_class = NULL;
static dev_t nan_device = 0;
static struct cdev nan_cdev;

static struct file_operations nan_fops = 
{
  .owner = THIS_MODULE,  
};

static char *nan_devnode(struct device *dev, umode_t *mode)
{
  if (mode)
    *mode = 0666;
  return NULL;
}

static int __init register_device(void)
{
  int res = 0;
  void* res_ptr = NULL;

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
  
static void __exit unregister_device(void)
{
  cdev_del(&nan_cdev);
  device_destroy(nan_class, nan_device);
  if (nan_class)
    class_destroy(nan_class);
  unregister_chrdev_region(nan_device, 1);
}


   
module_init(register_device);
module_exit(unregister_device);