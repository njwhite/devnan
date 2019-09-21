TARGET_MODULE:=devnan

# If we are running by kernel building system
ifneq ($(KERNELRELEASE),)
	$(TARGET_MODULE)-objs := driver.o
	obj-m := $(TARGET_MODULE).o
# If we running without kernel build system
else
	BUILDSYSTEM_DIR:=/lib/modules/$(shell uname -r)/build
	PWD:=$(shell pwd)

all: 
	$(MAKE) -C $(BUILDSYSTEM_DIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(BUILDSYSTEM_DIR) M=$(PWD) clean

install: 
	$(MAKE) -C $(BUILDSYSTEM_DIR) M=$(PWD) modules_install

load:
	insmod ./$(TARGET_MODULE).ko
unload:
	rmmod ./$(TARGET_MODULE).ko

endif

format:
	clang-format -i $(PWD)/driver.c
	yapf -i $(PWD)/test_driver.py