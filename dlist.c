#include "dlist.h"
NODEPTR createNode(void* data)
{
    NODEPTR newNode;

    newNode = (NODEPTR)malloc(sizeof(NODE));
    if (newNode == NULL)
    {
        fprintf(stderr, "Not enough memory to create node.\n");
        exit(-3);
    }
    newNode-> data = data;
    newNode-> next = NULL;
    newNode-> prev = NULL;
    newNode-> index = -1;

    return newNode;
}


DLIST* dlistInit()
{
    NODEPTR h;
    NODEPTR t;
    DLIST* out = (DLIST*)calloc(sizeof(DLIST), 1);
    if (out == NULL)
        return NULL;
    h = (NODEPTR)calloc(sizeof(NODE), 1);
    t = (NODEPTR)calloc(sizeof(NODE), 1);
    h->next = t;
    h->prev = NULL;
    t->prev = h;
    t->next = NULL;
    t->index = -1;
    h->index = -1;
    out->head = h;
    out->tail = t;
    out->length = 0;
    return out;
}

int dlistAdd(DLIST* l, void* e)
{
    NODEPTR h = l->head;
    NODEPTR t = l->tail;
    NODEPTR element = createNode(e);
    if (h->next == t) // List is empty
    {
        h->next = element;
        t->prev = element;

        h->next->index = 0;

        element->next = t;
        element->prev = h;

        l->length++;
        return 1;
    }
    else
    {
        element->prev = t->prev;
        element->next = t;
        t->prev = element;
        element->prev->next = element;
        element->index = element->prev->index + 1;
        l->length++;
        return 1;
    }
    return 0;
}

void* dlistGet(DLIST* l, int index)
{
    NODEPTR h = l->head;
    NODEPTR t = l->tail;
    void* output = NULL;

    if (h->next == t) //empty, throw error
    {
        fprintf(stderr, "Cannot get from empty list.\n");
        return output;
    }

    if (l->length < index)
    {
        fprintf(stderr, "List is smaller than index.\n");
        return output;
    }

    if (index < l->length / 2)
    {
        while (h != NULL)
        {
            if (h->index != index)
            {
                h = h->next;
            }
            else
            {
                output = (h->data);
                return output;
            }
        }
    }
    else
    {
        while (t != NULL)
        {
            if (t->index != index)
            {
                t = t->prev;
            }
            else
            {
                output = (t->data);
                return output;
            }
        }
    }
    return output;
}

void dlistPrint(DLIST* l)
{
    fprintf(stdout, "Length = %d, head = %p, tail = %p\n",
            l->length, l->head, l->tail);
}


void* dlistRemove(DLIST* l, int index)
{

    NODEPTR h = l->head;
    NODEPTR t = l->tail;
    void* output = NULL;
    if (h->next == t) //empty, throw error
    {
        fprintf(stderr, "Cannot remove from empty list.\n");
        return output;
    }

    if (l->length < index)
    {
        fprintf(stderr, "List is smaller than index.\n");
        return output;
    }

    while (t != NULL)
    {
        if (t->index != index)
        {
            t = t->prev;
            t->next->index--;
        }
        else
        {
            output = t->data;
            t->prev->next = t->next;
            t->next->prev = t->prev;
            free(t);
            return output;
        }
    }
    return output;
}

int dlistDelete(DLIST* l)
{
    NODEPTR h = l->head;
    NODEPTR t = l->tail;
    void* d = NULL;
    if (h->next == t)   // list is empty
    {
        free(h);
        free(t);
        free(l);
        return 1;
    }
    else
    {
        if (h->data != NULL) d = h->data;
        h = h->next;
        while (h != NULL)
        {
            if (d != NULL) free(d);
            free(h->prev);
            if (h->next == t)
            {
                if (h->data != NULL) d = h->data;
                if (d != NULL)  free(d);
                free(h);
                free(t);
                break;
            }
            if (h->data != NULL) d = h->data;
            h = h->next;
        }
    }
    free(l);
    return 0;
}

int ulistDelete(DLIST* l)
{
    NODEPTR h = l->head;
    NODEPTR t = l->tail;
    USER* d = NULL;
    if (h->next == t)   // list is empty
    {
        free(h);
        free(t);
        free(l);
        return 1;
    }
    else
    {
        if (h->data != NULL) d = h->data;
        h = h->next;
        while (h != NULL)
        {
            if (d != NULL)
            {
                free(d->movies);
                free(d->ratings);
		free(d->p);
            }
            free(d);
            free(h->prev);
            if (h->next == t)
            {
                if (h->data != NULL) d = (USER*) h->data;
                if (d != NULL)
                {
                    free(d->movies);
                    free(d->ratings);
		    free(d->p);
                }
                free(d);
                free(h);
                free(t);
                break;
            }
            if (h->data != NULL) d = (USER*) h->data;
            h = h->next;
        }
    }
    free(l);
    return 0;
}
int mlistDelete(DLIST* l)
{
    NODEPTR h = l->head;
    NODEPTR t = l->tail;
    MOVIE* d = NULL;
    if (h->next == t)   // list is empty
    {
        free(h);
        free(t);
        free(l);
        return 1;
    }
    else
    {
        if (h->data != NULL) d = h->data;
        h = h->next;
        while (h != NULL)
        {
            if (d != NULL)
            {
                free(d->users);
                free(d->ratings);
		free(d->q);
            }
            free(d);
            free(h->prev);
            if (h->next == t)
            {
                if (h->data != NULL) d = (MOVIE*) h->data;
                if (d != NULL)
                {
                    free(d->users);
                    free(d->ratings);
		    free(d->q);
                }
                free(d);
                free(h);
                free(t);
                break;
            }
            if (h->data != NULL) d = (MOVIE*) h->data;
            h = h->next;
        }
    }
    free(l);
    return 0;
}



int dlistContains(DLIST* l, void* e)
{
    NODEPTR h = l->head;
    NODEPTR t = l->tail;
    if (h->next == t)   // list is empty
    {
        return -1;
    }
    while (h->next != t)
    {
        h = h->next;
        USER *u, *arg;
        u = (USER*) h->data;
        arg = (USER*) e;
        if (u->id == arg->id)
            return h->index;
    }
    return -1;
}


// kate: indent-mode cstyle; space-indent on; indent-width 0;  replace-tabs on;
