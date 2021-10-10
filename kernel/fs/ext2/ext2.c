#include "ext2.h"
#include <kernel/device.h>
#include <kernel/kheap.h>
#include <kernel/printm.h>

int ext2_probe(struct device *dev) {
  if (!dev) {
    return -1;
  }

  printm("EXT2-fs: Probing device %d\n", dev->id);

  if (!dev->read) {
    printm("EXT2-fs: error: Cannot read device %d\n", dev->id);
    return -1;
  }

  /* Check if the device has a valid EXT2 signature */
  uint8_t *buf = kmalloc(1024);
  dev->read(buf, 2, 2, dev);

  ext2_superblock_t *sb = (ext2_superblock_t *)buf;

  if (sb->ext2_signature != EXT2_SIGNATURE) {
    printm("EXT2-fs: error: Device %d has a invalid ext2 signature!\n",
           dev->id);
    return -1;
  }

  kfree(buf);
}
