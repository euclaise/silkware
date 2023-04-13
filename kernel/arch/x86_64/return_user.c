#include <paging.h>
#include <mp.h>
void return_user_(void);

void return_user(void)
{
    refresh_pages(get_cpu_data()->proc_current->pt);
    return_user_();
}
