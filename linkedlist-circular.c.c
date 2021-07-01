#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Here we have a monster structure for a circular doubly-linked list of monsters.  We have
   to use the "struct" form of the pointer because typedefs refuse to resolve from within
   themselves - this is a weakness of C.  This is the only place we'll have to use the
   struct form.

   next is null if and only if the monster has not been added to a list.
   prev is null if and only if the monster has not been added to a list. */

struct monster_struct {
    char *name;
    char *type;
    int commonality;
    struct monster_struct *next;
    struct monster_struct *prev;
};

typedef struct monster_struct monster;

/* Doubly linked list of monsters with head and tail pointer.

   head is NULL if and only if the list is empty.
   tail is NULL if and only if the list is empty. */

struct monster_list_struct {
    monster *head;
    monster *tail;
};

typedef struct monster_list_struct monster_list;

/* MONSTER FUNCTIONS */

/* Allocate, initialize, and return a new monster. */

monster *new_monster(char *name, char *type, int commonality)
{
    monster *m = malloc(sizeof(monster));

    m->name = strdup(name);
    m->type = strdup(type);
    m->commonality = commonality;
    m->next = NULL;
    m->prev = NULL;

    return m;
}

/* Destructor for monster. */

void dispose_monster(monster *m)
{
    free(m->name);
    free(m->type);
    free(m);
}

/* Print out a monster's info. */

void print_monster(monster *m)
{
    printf("Monster %s of type %s with commonality %d\n",
           m->name,
           m->type,
           m->commonality);
    if(m->prev == NULL) {
        printf("  No link backward\n");
    } else {
        printf("  Link backward to %s\n", m->prev->name);
    }
    if(m->next == NULL) {
        printf("  No link forward\n");
    } else {
        printf("  Link forward to %s\n", m->next->name);
    }
}

/* MONSTER LIST FUNCTIONS */

/* Construct a new, empty list of monsters. */

monster_list *new_monster_list(void)
{
    monster_list *ml = malloc(sizeof(monster_list));
    ml->head = NULL;
    ml->tail = NULL;

    return ml;
}

/* Dispose of a list of monsters.  The list should be empty. */

void dispose_monster_list(monster_list *ml)
{
    free(ml);
}

/* Dispose of a list of monsters and all the monsters in it.

   This is an O(n) operation that can't be improved (we have to visit each monster
   to dispose it). */

void dispose_monster_list_all(monster_list *ml)
{
    monster *m = ml->head;
    monster *next;

    /* First, check if the list is empty. */

    if(m == NULL)
    {
        dispose_monster_list(ml);
        return;
    }

    /* Okay, now we know the list isn't empty, so we can free at least one element. */

    do {
        /* We can't use m->next after disposing of m, so we save a copy of m->next and
           then copy it back after disposing m. */

        next = m->next;
        dispose_monster(m);
        m = next;
    } while(m != ml->head);

    /* ml->head - and therefore m - are dangling pointers here!  But that's fine as long
       as we don't dereference them. */

    dispose_monster_list(ml);
}

/* Add new_monster to the head of ml.  new_monster must already be an allocated monster.
   new_monster must NOT be a member of any monster list.

   This is an O(1) operation - it always works in constant time.  (Notice it doesn't even
   have any loops!) */

void monster_list_add(monster_list *ml, monster *new_monster)
{
    if(ml->head == NULL)
    {
        // It's empty.

        ml->head = new_monster;
        ml->tail = new_monster;
        new_monster->prev = new_monster;
        new_monster->next = new_monster;
    } else {
        // It's not empty.

        new_monster->next = ml->head;
        new_monster->prev = ml->tail;

        ml->head->prev = new_monster;
        ml->tail->next = new_monster;

        ml->head = new_monster;
    }
}

/* Add monster new_monster to the tail of list ml.  new_monster must already be an
   allocated monster.  new_monster must NOT be a member of any monster list.

   This function is O(1) constant time. */

void monster_list_add_tail(monster_list *ml, monster *new_monster)
{
    /* The list is empty or it's not. */

    if(ml->tail == NULL)
    {
        // It's empty.

        ml->head = new_monster;
        ml->tail = new_monster;
        new_monster->prev = new_monster;
        new_monster->next = new_monster;
    } else {
        // It's not empty.

        new_monster->next = ml->head;
        new_monster->prev = ml->tail;

        ml->head->prev = new_monster;
        ml->tail->next = new_monster;

        ml->tail = new_monster;
    }
}

/* Remove m from ml.  Does not dispose m.

   Don't try to remove an invalid pointer, including null.  Bad things will happen.

   This is O(1) because we don't need to walk the list as long as we have the address
   of m.

   Don't pass an m that's not an element of ml.  The function will catch some cases
   of this but not all. */

void monster_list_remove(monster_list *ml, monster *m)
{
    /* We now have four cases to deal with. */

    /* Case 0: Don't delete from an empty list. */

    if(ml->head == NULL) {
        return;
    }

    /* Case 1: Emptying the list. */

    if((ml->head == ml->tail) && (ml->head == m)) {
        ml->head = NULL;
        ml->tail = NULL;
        m->prev = NULL;
        m->next = NULL;
        return;
    }

    /* Case 2: Removing the head. */

    if(m == ml->head) {
        ml->head = ml->head->next;
        ml->head->prev = ml->tail;
        ml->tail->next = ml->head;

        m->prev = NULL;
        m->next = NULL;
        return;
    }

    /* Case 3: Removing the tail. */

    if(m == ml->tail) {
        ml->tail = ml->tail->prev;
        ml->tail->next = ml->head;
        ml->head->prev = ml->tail;

        m->prev = NULL;
        m->next = NULL;
        return;
    }

    /* Case 4: Removing an interior element. */

    /* We are deleting neither the head nor the tail, so we have elements to our left
       and our right - which means we can just hook them together. */

    m->prev->next = m->next;
    m->next->prev = m->prev;

    m->prev = NULL;
    m->next = NULL;
}

/* Remove m from ml if m exists in ml, and dispose it.

   This is O(n) for the remove and O(1) for the dispose, so O(n). */

void monster_list_delete(monster_list *ml, monster *m)
{
    /* This is easier than it seems.  m is lost to the *list* after we remove it
       from ml, but is *not* lost to us, the caller.  We can simply dispose it
       normally. */

    monster_list_remove(ml, m);
    dispose_monster(m);
}


/* Traverse ml and print every monster in it.  This is inherently O(n). */

void print_monster_list(monster_list *ml)
{
    monster *m = ml->head;

    if(m == NULL)
    {
        return;
    }

    do {
        print_monster(m);
        m = m->next;
    } while(m != ml->head);
}

int main()
{
    monster_list *ml = new_monster_list();

    monster_list_add_tail(ml, new_monster("WholeWheat", "Bread", 20));
    monster_list_add_tail(ml, new_monster("Rye", "Bread", 8));
    monster_list_add_tail(ml, new_monster("Croissant", "Bread", 4));

    print_monster_list(ml);

    dispose_monster_list_all(ml);

    return 0;
}
