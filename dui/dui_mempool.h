#ifndef __DUI_MEMPOOL_H__
#define __DUI_MEMPOOL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

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

#ifdef _DEBUG
size_t mempool_size(MemoryContext cxt);
#endif

void* palloc(MemoryContext cxt, size_t size);

void* palloc0(MemoryContext cxt, size_t size);

void pfree(void* pointer);

#ifdef __cplusplus
}
#endif

#endif /* __DUI_MEMPOOL_H__ */