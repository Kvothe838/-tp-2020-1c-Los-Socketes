#ifndef CACHE_BASICCACHE_H_
#define CACHE_BASICCACHE_H_

typedef void* t_basicCache;
t_basicCache mainCache;


void initializeCache (int size);

void setValue (void* value, int size, int position);

void* getValue (int size, int position);


#endif
