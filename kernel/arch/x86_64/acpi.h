#include <stdint.h>

typedef struct rsdp_desc
{
    char Signature[8];
    uint8_t Checksum;
    char OEMID[6];
    uint8_t Revision;
    uint32_t RsdtAddress;
} __attribute__ ((packed)) rsdp_desc;

struct ACPISDTHeader
{
    char Signature[4];
    uint32_t Length;
    uint8_t Revision;
    uint8_t Checksum;
    char OEMID[6];
    char OEMTableID[8];
    uint32_t OEMRevision;
    uint32_t CreatorID;
    uint32_t CreatorRevision;
};

typedef struct rsdt_t
{
    struct ACPISDTHeader h;
    uint64_t PointerToOtherSDT[];
} rsdt_t;

extern uint64_t sdt_base, sdt_top;

typedef struct
{
    struct ACPISDTHeader **tables;
    uint64_t len;
} acpi_list;

extern acpi_list acpi_info;
