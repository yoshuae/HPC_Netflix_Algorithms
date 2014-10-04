#ifndef dlist_h_
#define dlist_h_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct movie* MOVIEPTR;
typedef struct user* USERPTR;
/**************************************************
** MOVIE
** int id: The movie's ID number.
**
** int numRatings: Number of users that rated this movie.
**
** USERPTR* users: Array of users that rated this movie.
** These are pointers and changing these values will change the
** users themselves.
**
** double* ratings: Array of ratings received from the corresponding
** users array. These are copies and will not effect the ratings that
** is within the USER structure.
**
** double* q: Array of values that approximate the attributes given
** to each movie and is a size d vector.
**
** double a: This value represents the bias for this movie.
** A positive value means this movie is, on average, rated higher.
**
****************************************************/
typedef struct movie
{
   int id;
   int numRatings;
   USERPTR *users;
   double* ratings;
   double* q;
   double a;
   double aold;
}MOVIE;
/**************************************************
** USER
** int id: The user's ID number.
**
** int numMovies: Number of movies this user has rated.
**
** MOVIEPTR* movies: Array of movies that this user has rated.
** These are pointers and changing these values will change the
** movies themselves.
**
** double* ratings: Array of ratings given to the corresponding
** movies array. These are copies and will not effect the ratings that
** is within the MOVIE structure.
**
** double* p: Array of values that approximate the attributes given
** to each user and is a size d vector.
**
** double b: This value represents the bias from this user.
** A positive value means this user is, on average, 
** more generous with his or her rating.
**
****************************************************/
typedef struct user
{
   int id;
   int numMovies;
   MOVIEPTR *movies;
   double *ratings;
   double *p;
   double b;
   double bold;
}USER;
typedef char* STRING;
typedef struct node* NODEPTR;
/**************************************************
** NODE
** NODEPTR next: Previous element in a list.
**
** NODEPTR tail: Next element in a list.
**
** int index: The index of this node in a list.
**
** void* data: A pointer to the data being held.
** Must be typecasted and maintained by the programmer.
**
****************************************************/
typedef struct node
{
   NODEPTR next;
   NODEPTR prev;
   int index;
   void* data;
}NODE;
/**************************************************
** DLIST
** NODEPTR head: Beginning of list. 
** Index = -1 and data = NULL
**
** NODEPTR tail: End of list.
** Index = -1 and data = NULL
**
** int length: Length of list.
**
****************************************************/
typedef struct dlist
{
   NODEPTR head;
   NODEPTR tail;
   int length;
}DLIST;


/***************************************************
** NODEPTR createNode(void* data)
**
** Creates a node for use in a linked list.
** Input(s): A pointer to the datum that will
** be held in the node.
** Output(s): A pointer to the newly created node.
**
*****************************************************/
NODEPTR createNode (void* data);

/***************************************************
** DLIST* dlistInit()
**
** Initializes a doubly linked list using calloc().
** Input(s): None
** Output(s): A pointer to the newly initialized
**    DLIST. The head's next is the tail and
**    the length is 0.
**
*****************************************************/
DLIST* dlistInit();

/***************************************************
** int dlistAdd(DLIST* l, void* e)
**
** Adds element e to DLIST l.
** Input(s): A pointer to the list that element e 
** will be added to and the address of e.
** Output(s): The index of datum e.
**
*****************************************************/
int dlistAdd(DLIST* l, void* e);

/***************************************************
** void* dlistGet(DLIST* l, int index)
**
** Gets the pointer to the element held at an index
** Input(s): A pointer to the list and the
** index to get the element.
** Output(s): A pointer to the element.
**
*****************************************************/
void* dlistGet(DLIST* l, int index);

/***************************************************
** void dlistPrint(DLIST* l)
**
** Prints the head and tail addresses and length of 
** list l to the standard output.
** Input(s): A pointer to the list.
** Output(s): None.
**
*****************************************************/
void dlistPrint(DLIST* l);

/***************************************************
** void* dlistRemove(DLIST* l, int index);
**
** Removes an element from the list l at index.
** Input(s): A pointer to the list and the 
**   index to remove.
** Output(s): A pointer to the removed element.
**
*****************************************************/
void* dlistRemove(DLIST* l, int index);

/***************************************************
** int dlistDelete(DLIST* l)
**
** Deletes a DLIST and frees its nodes' data.
** Input(s): A pointer to the list.
** Output(s): 1 if error. 0 if successful.
**
*****************************************************/
int dlistDelete(DLIST* l);


/***************************************************
** int ulistDelete(DLIST* l)
**
** Deletes a DLIST of USER's and frees its 
** nodes' data.
** Input(s): A pointer to the list.
** Output(s): 1 if error. 0 if successful.
***************************************************
** NOTE: This is specific to this implementation.
*****************************************************/
int ulistDelete(DLIST* l);

/***************************************************
** int mlistDelete(DLIST* l)
**
** Deletes a DLIST of MOVIE's and frees its 
** nodes' data.
** Input(s): A pointer to the list.
** Output(s): 1 if error. 0 if successful.
***************************************************
** NOTE: This is specific to this implementation.
*****************************************************/
int mlistDelete(DLIST* l);

/***************************************************
** int dlistContains(DLIST* l, void* e)
**
** Checks a list l and gives the index of the 
** element e if it is found.
** Input(s): A pointer to the list and to the 
** element that will be checked.
** Output(s): The index of e if successful or -1 if
** not found.
***************************************************
** NOTE: This is specific to this implementation.
*****************************************************/
int dlistContains(DLIST* l, void* e);

#endif
