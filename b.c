/*
   b.c
   Dwayne Phillips

   This program illustrates the breadth first 
   search algorithm.

   April 1999

*/


#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

#define BLOCKED 1
#define CLEAR   0
#define H       4
#define V       4

/************************************************/


int matrix[H][V] =
   { {1, 0, 0, 0},
     {0, 0, 1, 0},
     {0, 1, 0, 0},
     {1, 0, 0, 0} };

struct node {
   int    number;
   int    x;
   int    y;
   struct node *parent;
   struct node *next;
};

/************************************************/

struct node * remove_from_list(struct node **);
struct node * new_down_node(struct node **, int);
struct node * new_left_node(struct node **, int);
struct node * new_right_node(struct node **, int);
struct node * create_new_node(int, int, int);
void add_to_list(struct node **, struct node **);
void print_node(struct node *);
void free_nodes_in_list(struct node *);
void traverse_list(struct node *);
int is_empty(struct node *);
int is_a_goal_node(struct node *);
int is_clear(struct node *);
int no_solution_exists(int *);
int can_go_downwards(struct node *);
int can_go_left(struct node *);
int can_go_right(struct node *);
int solution_found(struct node *, int *);


/************************************************/

main(int argc, char *argv[])
{
char response[80];
int stupid = 99;
   int node_number = 0,
       searching   = 1;
   struct node *CLOSED,
               *down,
               *left,
               *n,
               *OPEN,
               *right;

      /******************************************
      *
      *   Initialize this problem.
      *
      ******************************************/

   OPEN   = NULL;
   CLOSED = NULL;


   n = create_new_node(node_number, 1, 0);
   OPEN = n;
   node_number++;

   n = create_new_node(node_number, 2, 0);
   add_to_list(&OPEN, &n);
   node_number++;

   n = create_new_node(node_number, 3, 0);
   add_to_list(&OPEN, &n);
   node_number++;

      /******************************************
      *
      *   Search through the state space for 
      *   a solution.
      *
      ******************************************/

   while(searching){

      if(is_empty(OPEN))
         no_solution_exists(&searching);

      else{  /* OPEN list is not empty */

            /************************************
            *
            *   Take the first node on OPEN list
            *   and put it on the CLOSED list.
            *
            ************************************/

         n = remove_from_list(&OPEN);
         add_to_list(&CLOSED, &n);

            /************************************
            *
            *   First try to go downwards.  If 
            *   that is possible, don't expand 
            *   anymore.
            *
            ************************************/

         if(can_go_downwards(n)){
            down = new_down_node(&n, node_number);
            node_number++;
            if(is_a_goal_node(down)) 
               solution_found(down, &searching);
            else
               add_to_list(&OPEN, &down);
         }  /* ends if can_go_downwards */

            /************************************
            *
            *   If cannot go downwards, expand 
            *   left and right.
            *
            ************************************/

         else{  /* cannot go downwards */
            if(can_go_left(n)){
               left = new_left_node(&n, node_number);
               node_number++;
               if(is_clear(left))
                  add_to_list(&OPEN, &left);
            }  /* ends if can_go_left */

            if(can_go_right(n)){
               right = new_right_node(&n, node_number);
               node_number++;
               if(is_clear(right))
                  add_to_list(&OPEN, &right);
            }  /* ends if can_go_right */

         }  /* ends else cannot go downwards */

      }  /* ends else OPEN is not empty */

   }  /* ends while searching */


   free_nodes_in_list(OPEN);
   free_nodes_in_list(CLOSED);

}  /* ends main */

/************************************************/
/*
   Add a node to the end of the linked list 
   pointed to be the head pointer.
*/

void add_to_list(struct node **head, 
                 struct node **new)
{
   struct node *temp;

   if((*head) == NULL)
      (*head) = (*new);
   else{
      temp = (*head);
      while(temp->next != NULL)
         temp = temp->next;
      temp->next = (*new);
   }  /* ends else */

   (*new)->next = NULL;

}  /* ends add_to_list */

/************************************************/

void free_nodes_in_list(struct node *head)
{
   struct node *temp;

   printf("\n\nFreeing nodes in a list");
   while(head != NULL){
      temp = head;
      print_node(temp);
      head = temp->next;
      free(temp);
   }  /* ends while */
}  /* ends free_nodes_in_list */

/************************************************/

int is_empty(struct node *head)
{
   int result = 0;
   if(head == NULL)
      result = 1;
   return(result);
}  /* ends is_empty */

/************************************************/

int no_solution_exists(int *searching)
{
   *searching = 0;
   printf("\nThe OPEN list is empty, "
          "no solution exists");
   return(1);
}  /* ends no_solution_exists */

/************************************************/

void print_node(struct node *a)
{
   printf("\nNode number=%d x=%d y=%d", 
   a->number, a->x, a->y);
}  /* ends print_node */

/************************************************/

struct node * remove_from_list(struct node **head)
{
   struct node *result;

   if(*head == NULL){
      printf("\nEMPTY LIST CANNOT REMOVE ");
      exit(1);
   }  /* ends if NULL */

   result = *head;
   *head  = result->next;
   return(result);

}  /* ends remove_from_list */

/************************************************/

int can_go_downwards(struct node *n)
{
   int result = 0;

   if(matrix[n->x][n->y+1] == CLEAR)
      result = 1;
   return(result);

}  /* ends can_go_downwards */

/************************************************/

struct node * new_down_node(struct node **n, 
                            int node_number)
{
   struct node *result;

   result = create_new_node(node_number, 
                            (*n)->x, (*n)->y+1);
   result->parent = (*n);
   return(result);
}  /* ends new_down_node */

/************************************************/

int is_a_goal_node(struct node *n)
{
   int result = 0;
   if(n->y >= V-1)
      result = 1;
   return(result);
}  /* ends is_a_goal_node */


/************************************************/

int solution_found(struct node *end, 
                   int *searching)
{
   struct node *temp;

   temp       = end;
   *searching = 0;

   printf("\n\n\nHere is the solution");
   while(temp != NULL){
      print_node(temp);
      temp = temp->parent;
   }  /* ends while */

   return(1);
}  /* ends solution_found */

/************************************************/

int can_go_left(struct node *n)
{
   int result = 0;
   if(n->x > 0)
      result = 1;
   return(result);
}  /* ends can_go_left */

/************************************************/

int can_go_right(struct node *n)
{
   int result = 0;
   if(n->x < H-1)
      result = 1;
   return(result);
}  /* ends can_go_right */

/************************************************/

int is_clear(struct node *n)
{
   int result = 0;
   if(matrix[n->x][n->y] == CLEAR)
      result = 1;
   return(result);
}  /* ends is_clear */

/************************************************/

struct node * new_left_node(struct node **n, 
                            int node_number)
{
   struct node *result;

   result = create_new_node(node_number, 
                            (*n)->x-1, (*n)->y);
   result->parent = (*n);
   return(result);
}  /* ends new_left_node */

/************************************************/

struct node * new_right_node(struct node **n, 
                             int node_number)
{
   struct node *result;

   result = create_new_node(node_number, 
                            (*n)->x+1, (*n)->y);
   result->parent = (*n);
   return(result);
}  /* ends new_right_node */


/************************************************/

struct node * create_new_node(int number, 
                              int x, int y)
{
   struct node *new;
   new = (struct node *)
         calloc(1, sizeof(struct node));
   new->number = number;
   new->x      = x;
   new->y      = y;
   new->next   = NULL;
   new->parent = NULL;
   return(new);
}  /* ends create_new_node */

/************************************************/


/************************************************/


/************************************************/

void traverse_list(struct node *head)
{
   struct node *temp;

   printf("\n\nTraversing list");
   while(head != NULL){
      temp = head;
      print_node(temp);
      head = temp->next;
   }  /* ends while */
   printf("\n\n");
}  /* ends free_nodes_in_list */
