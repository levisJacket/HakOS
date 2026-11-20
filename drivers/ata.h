#ifndef ATA_H
#define ATA_H

#include <stdint.h>
extern void ata_read_sector(uint32_t lba, void* buffer);
extern void ata_write_sector(uint32_t lba, void* buffer);

#endif
