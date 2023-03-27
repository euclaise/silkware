#include <stdint.h>

typedef struct rsdp_desc
{
    char signature[8];
    uint8_t checksum;
    char oem_id[6];
    uint8_t revision;
    uint32_t addr;
} __attribute__ ((packed)) rsdp_desc;

extern rsdp_desc rsdp;

typedef struct sdt_header
{
    char signature[4];
    uint32_t length;
    uint8_t revision;
    uint8_t checksum;
    char oem_id[6];
    char oem_table_id[8];
    uint32_t oem_revision;
    uint32_t creator_id;
    uint32_t creator_revision;
} sdt_header;

typedef struct rsdt_t
{
    struct sdt_header h;
    uint32_t p[];
} rsdt_t;
