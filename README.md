# /dev/nan

A device driver to let you memory-map arbitary numbers of double `NaN` values.


## Useful links

- [Device Drivers, Part 5: Character Device Files — Creation & Operations](https://opensourceforu.com/2011/04/character-device-files-creation-operations)
- [VIRTual BLocK IO SIMulating](https://github.com/rgolubtsov/virtblkiosim)
- [Chapter 4. Character Device Files](https://www.tldp.org/LDP/lkmpg/2.4/html/c577.htm)
- [mem.c](https://github.com/torvalds/linux/blob/master/drivers/char/mem.c)
- [Memory-Mapping](https://linux-kernel-labs.github.io/master/labs/memory_mapping.html)
- [MMap Driver Implementation](https://jlmedina123.wordpress.com/2015/04/14/mmap-driver-implementation)

## Vagrant

- `VAGRANT_DISABLE_STRICT_DEPENDENCY_ENFORCEMENT=1 vagrant plugin install vagrant-reload`
- `vagrant up`