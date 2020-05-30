#ifndef CACHE_BASICCACHE_H_
#define CACHE_BASICCACHE_H_

#include <commons/temporal.h>


typedef void* t_basicCache;
t_basicCache mainCache;
t_basicCache alternativeCache;

void initializeCache (int size);

void setValue (void* value, int size, int position);

void* getValue (int size, int position);

void moveBlock(int size, int oldPosition, int newPosition);

#endif
