#ifndef UTIL_H
#define UTIL_H

extern void *kern_load;
void map_kern_pages(void);
void freeze(void);
void main(void);

#endif