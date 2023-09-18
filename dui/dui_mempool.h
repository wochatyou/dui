#ifndef __DUI_MEMPOOL_H__
#define __DUI_MEMPOOL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

typedef size_t Size;

#define MemoryContext		DUIMemoryContext
#define MemoryContextData	DUIMemoryContextData
#define	mempool_create		DUI_mempool_create
#define	mempool_destroy		DUI_mempool_destroy
#define mempool_reset		DUI_mempool_reset
#define mempool_size		DUI_mempool_size
#define	palloc				DUI_palloc
#define	palloc0				DUI_palloc0
#define pfree				DUI_pfree

typedef struct MemoryContextData* MemoryContext;

MemoryContext mempool_create(unsigned int minContextSize, unsigned int initBlockSize, unsigned int maxBlockSize);

void mempool_destroy(MemoryContext cxt);

void mempool_reset(MemoryContext cxt);

Size mempool_size(MemoryContext cxt);

void* palloc(MemoryContext cxt, Size size);

void* palloc0(MemoryContext cxt, Size size);

void pfree(void* pointer);

#ifdef __cplusplus
}
#endif

#endif /* __DUI_MEMPOOL_H__ */