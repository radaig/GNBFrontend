#include "stdlib.h"
#include "string.h"

#include "gnb_alloc.h"

#ifndef offsetof
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif

#ifndef container_of

#define container_of(ptr, type, member) ({                                    \
                      const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
                       (type *)( (char *)__mptr - offsetof(type,member) );})
#endif


typedef struct _gnb_heap_fragment_t{
    uint32_t size;
    uint32_t idx;
    unsigned char block[0];
}gnb_heap_fragment_t;


gnb_heap_t* gnb_heap_create(uint32_t size){
    
    gnb_heap_t *gnb_heap = (gnb_heap_t *)malloc( sizeof(gnb_heap_t) + sizeof(gnb_heap_fragment_t) * size );

    if (NULL==gnb_heap) {
        return NULL;
    }

    memset(gnb_heap, 0, sizeof(gnb_heap_t));

    gnb_heap->size = size;
    gnb_heap->num  = 0;

    return gnb_heap;
    
}


void* gnb_heap_alloc(gnb_heap_t *gnb_heap, uint32_t size){

    if ( gnb_heap->size == gnb_heap->num) {
        return NULL;
    }

    if ( 0 == size ) {
        return NULL;
    }
    
    if ( size > (uint32_t)(1024l * 1024l * 1024l) - 1) {
        return NULL;
    }
    
    gnb_heap_fragment_t *fragment = malloc( sizeof(gnb_heap_fragment_t) + sizeof(unsigned char) * size);

    if ( NULL == fragment ) {
        return NULL;
    }

    fragment->idx = gnb_heap->num;
    gnb_heap->fragment_list[ gnb_heap->num ] = fragment;
    gnb_heap->num++;

    gnb_heap->alloc_byte  += size;
    gnb_heap->ralloc_byte += sizeof(gnb_heap_fragment_t)+size;

    return (void *)fragment->block;

}


void gnb_heap_free(gnb_heap_t *gnb_heap, void *p){

    gnb_heap_fragment_t *fragment;
    
    if ( 0 == gnb_heap->num ) {
        //发生错误了
        return;
    }

    if ( NULL == p ) {
        return;
    }
    
    fragment = container_of(p, struct _gnb_heap_fragment_t, block);

    gnb_heap_fragment_t *last_fragment = gnb_heap->fragment_list[gnb_heap->num - 1];

    if ( fragment->idx > (gnb_heap->size-1)  ) {
        //发生错误了
        return;
    }
    
    if ( last_fragment->idx > (gnb_heap->size-1) ) {
        //发生错误了
        return;
    }
    
    if ( 1 == gnb_heap->num ) {
        if ( fragment->idx != last_fragment->idx ){
            //发生错误了
            return;
        }
        goto finish;
    }

    if( last_fragment->idx == fragment->idx ){
        goto finish;
    }

    gnb_heap->alloc_byte -= fragment->size;
    gnb_heap->ralloc_byte -= (sizeof(gnb_heap_fragment_t)+fragment->size);

    last_fragment->idx = fragment->idx;

    gnb_heap->fragment_list[last_fragment->idx] = last_fragment;

finish:

    gnb_heap->num--;

    free(fragment);

}


void gnb_heap_clean(gnb_heap_t *gnb_heap){
    
    int i;
    
    if( 0 == gnb_heap->num ){
        return;
    }
    
    for ( i=0; i < gnb_heap->num; i++ ) {
        free(gnb_heap->fragment_list[i]);
    }
    
    gnb_heap->num = 0;
    gnb_heap->alloc_byte  = 0;
    gnb_heap->ralloc_byte = 0;

}


void gnb_heap_release(gnb_heap_t *gnb_heap){
    
    gnb_heap_clean(gnb_heap);

    free(gnb_heap);
    
}
