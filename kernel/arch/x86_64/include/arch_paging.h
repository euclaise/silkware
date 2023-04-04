typedef struct
{
    void *kaddr_pml4;
    void *kaddr_pdpt;
    void *kaddr_pd;
    void *kaddr_pt;

    void *pml4;
} page_tab;
