#include <stdio.h>
#include <stdlib.h>

typedef struct heap {
    int* arr;
    int elements;
    int capacity;
} Heap;

Heap heap_create(int inicial_capacity);
void heap_destroy(Heap *heap);
void heap_insert(Heap *heap);
void heap_remove(Heap *heap);

Heap
heap_create(int inicial_capacity) {
    Heap new_heap;
    new_heap.elements = 0;

    new_heap.arr = (int*) malloc( sizeof(int) * inicial_capacity );

    new_heap.capacity = inicial_capacity;
}

void
heap_destroy(Heap *heap) {
    free(heap->arr);
    heap->capacity = 0;
    heap->elements = 0;
}

/*heap_getmin()*/
heap_heapify(Heap* heap, int i)

int main() {
    return 0;
}
