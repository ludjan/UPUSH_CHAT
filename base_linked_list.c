#include "base_linked_list.h"

/* functions are documented in header file */

base_linked_list_t * ll_create_base_linked_list() {

    base_linked_list_t * list = malloc(sizeof(base_linked_list_t));
    if (check_null_error_stderr(list, "Malloc of base_linked_list_t failed")) return NULL;

    list->head = malloc(sizeof(node_t));
    if (check_null_error_stderr(list->head, "Malloc of base_linked_list_t head failed")) {
        free(list);
        return NULL;
    }
    list->tail = malloc(sizeof(node_t));
    if (check_null_error_stderr(list->tail, "Malloc of base_linked_list_t tail failed")) {
        free(list->head);
        free(list);
        return NULL;
    }
    list->head->next = list->tail;
    list->head->prev = NULL;
    list->head->data = NULL;

    list->tail->prev = list->head;
    list->tail->next = NULL;
    list->tail->data = NULL;

    return list;
}

void ll_insert_node_between(node_t * left, node_t * middle, node_t * right) {
    middle->next = right;
    middle->prev = left;

    left->next = middle;
    right->prev = middle;
}

int ll_add_node(base_linked_list_t * list, void * data) {

    node_t * new_node = malloc(sizeof(node_t));
    if (check_null_error_stderr(new_node, "Malloc of new node failed")) return -1;
    
    new_node->data = data;
    ll_insert_node_between(list->head, new_node, list->head->next);
    
    return 1;
}

void ll_do_on_each(base_linked_list_t * list, void (*fptr)(void *)) {

    node_t * tmp = list->head->next;

    while (tmp != list->tail) {
        (*fptr)(tmp->data);
        tmp = tmp->next;
    }
}

int ll_base_list_size(base_linked_list_t * list) {
    
    int node_count = 0;
    node_t * tmp = list->head->next;

    while (tmp != list->tail) {
        node_count++;
        tmp = tmp->next;
    }

    return node_count;
}


int ll_contains_matching_data(base_linked_list_t * list, int (*cmp)(void *, void*), void * otherData) {

    node_t * tmp = list->head->next;

    while (tmp != list->tail) {
        if ( (*cmp)(tmp->data, otherData) == 1 ) return 1;
        tmp = tmp->next;
    }
    return 0;

}

// returnerer ikke data, men 1 hvis slett og 0 hvis ikke
int ll_remove_on_match(base_linked_list_t * list, int (*cmp)(void *, void*), void * otherData, void (*free)(void *)) {

    node_t * tmp = list->head->next;

    while (tmp != list->tail) {
        if ( (*cmp)(tmp->data, otherData) == 1 ) {
            
            ll_dettach_node(tmp);
            (*free)(tmp->data);
            free(tmp);
            
            return 1;
        }
        tmp = tmp->next;
    }
    return 0;
    

}

void ll_dettach_node(node_t * node) {

    node->next->prev = node->prev;
    node->prev->next = node->next;

    node->next = NULL;
    node->prev = NULL;
}

void ll_free_linked_list(base_linked_list_t * list, void (*free_func)(void *)) {

    if (ll_base_list_size(list) > 0) {

        node_t * node = list->head->next;

        while (node != list->tail) {

            node_t * tmp = node;
            node = node->next;

            ll_dettach_node(tmp);
            (*free_func)(tmp->data);
            free(tmp);
        }
    }

    free(list->head);
    free(list->tail);
    free(list);

    // printf("The whole list was freed\n");
}

// returner data, men fri node
void * ll_pop_on_match(base_linked_list_t * list, int (*cmp)(void *, void*), void * otherData) {

    node_t * tmp = list->head->next;

    while (tmp != list->tail) {
        if ( (*cmp)(tmp->data, otherData) == 1 ) {
            
            ll_dettach_node(tmp);
            void * data = tmp->data;
            free(tmp);
            
            return data;
        }
        tmp = tmp->next;
    }
    return NULL;

}

// fjerner node lengst bak, returnerer data
void * ll_pop_node(base_linked_list_t * list) {

    node_t * remove_node = list->tail->prev;
    ll_dettach_node(remove_node);
    
    void * data = remove_node->data;

    free(remove_node);

    return data;
}

void * ll_find_by_matching_data(base_linked_list_t * list, int (*cmp)(void *, void*), void * otherData) {

    node_t * tmp = list->head->next;

    while (tmp != list->tail) {
        if ( (*cmp)(tmp->data, otherData) == 1 ) {
            
            void * data = tmp->data;
            
            return data;
        }
        tmp = tmp->next;
    }
    return NULL;

}

void * ll_peek(base_linked_list_t * list) {
    if (ll_base_list_size(list) > 0) return list->tail->prev->data;
    return NULL;
}


base_iterator_t * get_base_iterator(base_linked_list_t * list) {

    base_iterator_t * new_iterator = malloc(sizeof(base_iterator_t));
    if (check_null_error_stderr(new_iterator, "Malloc of base_iterator failed")) return NULL;

    new_iterator->current = list->head->next;
    new_iterator->tail = list->tail;

    return new_iterator;
} 

int base_iterator_has_next (base_iterator_t * iterator) {
    if (iterator->current == iterator->tail) return 0;
    return 1; 
}

void * base_iterator_next(base_iterator_t * iterator) {
    void * tmp = iterator->current->data;
    iterator->current = iterator->current->next;
    return tmp;
}

void base_iterator_free(base_iterator_t * iterator) {
    // don't free the tail or current, that would destroy the list
    free(iterator);
}
