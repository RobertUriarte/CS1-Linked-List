#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "leak_detector_c.h"

//Credit Professor Gerber
void remove_crlf(char *s)
{
    char *t = s + strlen(s);

    t--;

    while((t >= s) && (*t == '\n' || *t == '\r'))
    {
        *t = '\0';
        t--;
    }
}

// Credit Professor Gerber
int get_next_nonblank_line(FILE *ifp, char *buf, int max_length)
{
    buf[0] = '\0';

    while(!feof(ifp) && (buf[0] == '\0'))
    {
        fgets(buf, max_length, ifp);
        remove_crlf(buf);
    }

    if(buf[0] != '\0') {
        return 1;
    } else {
        return 0;
    }
}

typedef struct failfish{
    int fishnumber;
    struct failfish *prev;
    struct failfish *next;
}failfish;

typedef struct failpond{
    int pondnumber;
    char* pondname;
    int n;
    int e;
    int th;
    struct failfish *head;
    struct failfish *tail;
}failpond;

//Creates a new failgroup
failpond new_failgroup(int pondnumber, int n, int e ,int th, char* pondname)
{
    failpond p;
    p.pondnumber = pondnumber;
    p.pondname = strdup(pondname);
    p.n = n;
    p.e = e;
    p.th = th;
    p.head = NULL;
    p.tail = NULL;

    return p;
}

//Adds a fish to an empty fish list
failfish* add_to_empty_fish_list(int fishnumber)
{
    failfish* fish = malloc(sizeof(failfish));
    fish->fishnumber = fishnumber;
    fish->prev = fish;
    fish->next = fish;
    return fish;
}

//Frees fish
void dispose_fish_list(failfish* fish)
{
    free(fish);
}

//Adds to the tail of a fish list
failfish* add_to_end(failpond failponds[10], int fishnumber, int i)
{
    failfish* newfish = add_to_empty_fish_list(fishnumber);
    failfish* temp = failponds[i].tail->next;
    newfish->next = temp;
    newfish->prev = failponds[i].tail;
    failponds[i].tail->next = newfish;
    temp->prev = newfish;
    failponds[i].tail = newfish;
    return failponds[i].tail;
}

//Swaps two pointers
void swap(failpond* failponds, failpond* failponds2)
{
    failpond temp = *failponds;
    *failponds = *failponds2;
    *failponds2 = temp;
}

//Bubble Sorts Ponds
void bubbleSort(failpond failponds[10], int n_of_ponds)
{
    for (int i = 0; i < n_of_ponds-1; i++){
        for (int j = 0; j < (n_of_ponds-i-1); j++){
            if (failponds[j].pondnumber > failponds[j+1].pondnumber){
                swap(&failponds[j],&failponds[j+1]);
            }
        }
    }
}

//Prints the ponds and its data
void print_ponds(int n_of_ponds, failpond failponds[10]){
    for(int i = 0; i<n_of_ponds; i++){
        printf("%d %s %d %d %d\n",failponds[i].pondnumber,failponds[i].pondname,failponds[i].n,failponds[i].e,failponds[i].th);
    }
}

//Removes a fish from a pond , Credit Professor Gerber for function idea
void fish_list_remove(failpond failponds[10], failfish *f, int i)
{
    //In case of empty list
    if(failponds[i].head == NULL) {
        return;
    }

    //Emptying list
    if((failponds[i].head == failponds[i].tail) && (failponds[i]. head == f)) {
        failponds[i].head = NULL;
        failponds[i].tail = NULL;
        f->prev = NULL;
        f->next = NULL;
        return;
    }

    //Removes Head
    if(f == failponds[i].head) {
        failponds[i].head = failponds[i].head->next;
        failponds[i].head->prev = failponds[i].tail;
        failponds[i].tail->next = failponds[i].head;

        f->prev = NULL;
        f->next = NULL;
        return;
    }

    //Removes Tail
    if(f == failponds[i].tail) {
        failponds[i].tail = failponds[i].tail->prev;
        failponds[i].tail->next = failponds[i].head;
        failponds[i].head->prev = failponds[i].tail;

        f->prev = NULL;
        f->next = NULL;
        return;
    }

    //Removes middle node
    f->prev->next = f->next;
    f->next->prev = f->prev;

    f->prev = NULL;
    f->next = NULL;
    return;
}

//Frees and removes a fish from a pond
void fish_list_delete(failpond failponds[10], failfish* f, int i)
{
    fish_list_remove(failponds,f,i);
    dispose_fish_list(f);
}

//Creates a pond and fills it with fishes
failpond new_pond(FILE* ifp,char buf[256], int n_of_ponds, failpond* failponds,int i)
{
    int pondnumber, n , e , th;
    char* pondname = malloc(sizeof(char)*20);

    //Gets pond data
    (get_next_nonblank_line(ifp,buf,255));
    sscanf(buf,"%d %s %d %d %d", &pondnumber,pondname,&n,&e,&th);

    failponds[i] = new_failgroup(pondnumber,n,e,th,pondname); // Creates new pond

    //Fills pond with fishes
    failfish* fish = add_to_empty_fish_list(1);
    failponds[i].head = fish;
    failponds[i].tail = failponds[i].head;
    for(int j = 2; j<=n; j++){
        failponds[i].tail = add_to_end(failponds,j,i);
    }

    free(pondname);
    return failponds[i];
}

//File prints pond data
void file_print_pond(failpond failponds[10], FILE* ofp, int n_of_ponds){

    fprintf(ofp,"Initial Pond Status");

    for(int i = 0; i < n_of_ponds; i++){
        fprintf(ofp,"\n%d %s", failponds[i].pondnumber,failponds[i].pondname);
        for(int j = 1; j < (failponds[i].n + 1); j++){
            fprintf(ofp," %d",j);
        }
    }
}

//Runs first course of eating
void first_course(failpond failponds[10], int n_of_ponds, FILE* ofp){

    fprintf(ofp,"\n\nFirst Course\n\n");

    failfish* f;
    failfish* temp;

    for (int i = 0; i < n_of_ponds; i++){ //For loop based on number of ponds

            f = failponds[i].head;
            printf("\nPond %d: %s\n",failponds[i].pondnumber, failponds[i].pondname);
            fprintf(ofp,"Pond %d: %s\n",failponds[i].pondnumber, failponds[i].pondname);


            for(int j = failponds[i].n; j > failponds[i].th; j--){ //Removes fish in the pond until threshold is reached
                for (int k = 0; k < (failponds[i].e - 1); k++){ //Removes fish based on eth index
                    f = f->next;
                }

                temp = f->next;

                printf("\nFailfish %d is eaten\n", f->fishnumber);
                fprintf(ofp,"Failfish %d is eaten\n", f->fishnumber);

                fish_list_delete(failponds,f,i);
                f = temp;
            }
            printf("\n");
            fprintf(ofp,"\n");
        }

    printf("End of course pond status\n");
    fprintf(ofp,"End of Course Pond Status\n");

    //Prints end of course pond status
    for(int i = 0; i < n_of_ponds; i++){
        printf("%d %s %d",failponds[i].pondnumber,failponds[i].pondname, failponds[i].head->fishnumber);
        fprintf(ofp,"%d %s %d",failponds[i].pondnumber,failponds[i].pondname, failponds[i].head->fishnumber);

        temp = failponds[i].head->next;

        for(int j=1; j < failponds[i].th; j++){ //Prints all the fish that are left in each pond
                printf(" %d", temp->fishnumber);
                fprintf(ofp," %d", temp->fishnumber);

                temp = temp->next;
        }

        printf("\n");
        fprintf(ofp,"\n");
        }

}

//Runs second course of eating
void second_course(int n_of_ponds, failpond failponds[10],FILE* ofp)
{
    printf("\nSecond Course\n");
    fprintf(ofp,"\nSecond Course\n");

    //Gets the total number of fish left
    int total_num_of_fish_left = 0;
    for(int i = 0; i < n_of_ponds; i++){
        total_num_of_fish_left = failponds[i].th + total_num_of_fish_left;
    }
    printf("\nTotal number of fish left is %d\n", total_num_of_fish_left);

    failfish* largest;

    for (int i = 1; i < total_num_of_fish_left; i++){ //Eats fish until there is only 1 left

        failfish* temp = NULL;
        int q = -1;

        //Find the first non-NULL head and makes largest equal to it
        while(temp == NULL){
            q += 1;
            largest = failponds[q].head;
            temp = failponds[q].head;
        }

        temp = largest->next;

        //Checks which head is the largest
        for (int j = 0; j < n_of_ponds; j++){
            if (failponds[j].head != NULL){
                if(failponds[j].head->fishnumber > largest->fishnumber){
                    largest = failponds[j].head;
                }
            }
        }

        //Eats largest fish
        for(int j = 0; j < n_of_ponds; j++){
            if(failponds[j].head == largest){ //Checks which head is being eaten to get pond number
                printf("\nEaten: Failfish %d from Pond %d", largest->fishnumber,failponds[j].pondnumber);
                fprintf(ofp,"\nEaten: Failfish %d from Pond %d", largest->fishnumber,failponds[j].pondnumber);
                if(largest == temp){ //If the last fish in a pond is being eaten temp is equal to the next available fish head
                    failfish* temp2 = NULL;
                    int k = -1;
                    while(temp2 == NULL){
                        k += 1;
                        temp = failponds[k].head;
                        temp2 = failponds[k].head;
                        if(temp == largest){
                            temp2 = NULL;
                        }
                   }
                }
                fish_list_delete(failponds,largest,j);
            }

        }

        largest = temp;
    }

    //Finds the last fish and then deletes it
    failfish* temp3 = NULL;
    int x = -1;
    while(temp3 == NULL){
        x += 1;
        temp3 = failponds[x].head;
    }
    printf("\n\nFailfish %d from pond %d remains", largest->fishnumber, failponds[x].pondnumber);
    fprintf(ofp,"\n\nFailfish %d from pond %d remains", largest->fishnumber, failponds[x].pondnumber);
    fish_list_delete(failponds,largest,x);
}


int main()
{
    //atexit(report_mem_leak);

    char buf[256];
    int n_of_ponds;
    failpond failponds[10];

    //Opens input file
    FILE* ifp = fopen("cop3502-as2-input.txt","r");
    if (ifp != NULL){
        printf("Input file successfully opened\n");
    }
    else{
        printf("Error input file unsuccessfully opened\n");
        return 0;
    }

    //Opens output file
    FILE* ofp = fopen ("cop3502-as2-output-uriarte-robert.txt","w");
    if (ofp != NULL){
        printf("Output file successfully opened\n");
    }
    else{
        printf("Error output file unsuccessfully opened\n");
        return 0;
    }

    //Gets number of ponds from input file
    (get_next_nonblank_line(ifp,buf,255));
    n_of_ponds = atoi(strtok(buf," "));
    printf("\nThere are %d ponds\n\n", n_of_ponds);
    printf("Initial Pond Status\n");

    //Creates ponds based on n_of_ponds
    for (int i = 0; i < n_of_ponds; i++){
        failponds[i] = new_pond(ifp,buf,n_of_ponds,failponds,i);
    }

    //bubbleSort(failponds,n_of_ponds);

    print_ponds(n_of_ponds,failponds);
    file_print_pond(failponds,ofp,n_of_ponds);

    //first_course(failponds,n_of_ponds,ofp);
    //second_course(n_of_ponds,failponds,ofp);

    //Closes out of input and output files
    fclose(ofp);
    fclose(ifp);

    return 0;
}
