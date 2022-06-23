#ifndef BASE_LINKED_LIST
#define BASE_LINKED_LIST

#include "common.h"

typedef struct node {
    struct node * next;
    struct node * prev;
    void * data;
} node_t;

typedef struct base_linked_list {
    node_t * head;
    node_t * tail;
} base_linked_list_t;

// mallocs a base_linked_list with head and tail, and returns the list
base_linked_list_t * ll_create_base_linked_list();

// adds a node to the list, and gives that node a pointer to input data
int ll_add_node(base_linked_list_t * list, void * data);

// applies a function (like printing) to data of each node
void ll_do_on_each(base_linked_list_t * list, void (*fptr)(void *));

// returns the size of data nodes (excluding head and tail node)
int ll_base_list_size(base_linked_list_t * list);

// iterates through list, applying a match function to each element,
// if a match to other_data is found, returns 1
// if there are no matches, 0 is returned
int ll_contains_matching_data(base_linked_list_t * list, int (*cmp)(void *, void*), void * other_data);

// function for moving pointers and dettaching a node from the list
// while keeping node allocated
void ll_dettach_node(node_t * node);

// iterates through list, applying a free function to each element
// frees head and tail and list before returning
void ll_free_linked_list(base_linked_list_t * list, void (*free)(void *));

// iterates through list, applying a match function to each element,
// and returns pointer to data of first match
void * ll_find_by_matching_data(base_linked_list_t * list, int (*cmp)(void *, void*), void * otherData);

// pops node (removes from list and frees) closest to tail
// returns a pointer to the popped nodes data
void * ll_pop_node(base_linked_list_t * list);

// iterates through list, applying a match function to each element, 
// on first match, the node is removed, it's data is freed (with free function) and 1 is returned.
// if there are no matches, 0 is returned
int ll_remove_on_match(base_linked_list_t * list, int (*cmp)(void *, void*), void * otherData, void (*free)(void *));

// iterates though list, applying a match function to each eleemnt,
// on first match, the node is removed, and a pointer to it's data returned
// if there are no matches, NULL is returned
void * ll_pop_on_match(base_linked_list_t * list, int (*cmp)(void *, void*), void * otherData);

// returns a pointer to data of node closest to tail, without removing node.
void * ll_peek(base_linked_list_t * list);


/* ------ iterator -------- */

typedef struct base_iterator {
    node_t * tail;
    node_t * current; // starts as head
} base_iterator_t;

// get a new iterator over the base list
base_iterator_t * get_base_iterator(base_linked_list_t * list);

// returns 1 if there are more nodes to iterate through
// returns 0 if next node is tail
int base_iterator_has_next (base_iterator_t * iterator);

// returns a pointer to the data of the current nodes
// and moves the current pointer to next node
void * base_iterator_next(base_iterator_t * iterator);

// frees the iterator
void base_iterator_free(base_iterator_t * iterator);

#endif