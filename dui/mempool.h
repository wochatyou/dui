#ifndef __WOCHAT_MEMPOOL_H__
#define __WOCHAT_MEMPOOL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

typedef size_t Size;

#define MemoryContext		WoChatMemoryContext
#define MemoryContextData	WoChatMemoryContextData
#define	mempool_create		WoChat_mempool_create
#define	mempool_destroy		WoChat_mempool_destroy
#define mempool_reset		WoChat_mempool_reset
#define mempool_size		WoChat_mempool_size
#define	palloc				WoChat_palloc
#define	palloc0				WoChat_palloc0
#define pfree				WoChat_pfree

struct	MemoryContextData;
typedef struct MemoryContextData* MemoryContext;

MemoryContext mempool_create(unsigned int initBlockSize, unsigned int maxBlockSize);

void mempool_destroy(MemoryContext cxt);

void mempool_reset(MemoryContext cxt);

Size mempool_size(MemoryContext cxt);

void* palloc(MemoryContext cxt, Size size);

void* palloc0(MemoryContext cxt, Size size);

void pfree(void* pointer);

#ifdef __cplusplus
}
#endif

#endif /* __WOCHAT_MEMPOOL_H__ */