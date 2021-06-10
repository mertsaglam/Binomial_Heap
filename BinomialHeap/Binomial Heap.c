#include <stdio.h>
#include <stdlib.h>
#include <stdio.h> 
#include <string.h>
#include <dirent.h> 
#include <ctype.h>

#define MAXCHAR 16000

int exactMatchCount(FILE *fptr, const char *search);

struct heap_node {
    int occurence;
    int degree;
    char* filename;
    struct heap_node* parent;
    struct heap_node* child;
    struct heap_node* sibling;
};

struct extracted_max_heap {
    struct heap_node* output_heap;
    struct heap_node* max_node;
};

struct heap_node* create_bin_heap();
void heap_link(struct heap_node*, struct heap_node*);
struct heap_node* create_heap_node(int, char*);
struct heap_node* heap_union(struct heap_node*, struct heap_node*);
struct heap_node* heap_merge(struct heap_node*, struct heap_node*);
struct extracted_max_heap* heap_extract_max(struct heap_node*);
void reverse_list(struct heap_node*);
void print_heap(struct heap_node*);
 
struct heap_node *global_heap = NULL;

int exactMatchCount(FILE *fptr, const char *word) {
    char str[MAXCHAR];
    char *pos;
    int index, count;
    char next;
	char prev;

    count = 0;

    // Read line from file till end of file.
    while ((fgets(str, MAXCHAR, fptr)) != NULL)
    {
        index = 0;

        // Occurrence of word in str
        while ((pos = strstr(str + index, word)) != NULL)
        {
			// update location of current index
            index = (pos - str) + 1;

			// exact next character and previous character
			// this will be used in order to find an exact match
			next = str[index + strlen(word) - 1];
			prev = str[index - 2];

			// prev char must be a space
			if (isspace(prev) != 0) {
				// next char can either be a space or puncutation for exact match
				if (ispunct(next) || isspace(next)) {
					count++;
				}
			}
        }
    }

    return count;
}

struct heap_node* create_bin_heap() {
    struct heap_node* emp = NULL;
    return emp;
}
 
void heap_link(struct heap_node* heap1, struct heap_node* heap2) {
    heap1->parent = heap2;
    heap1->sibling = heap2->child;
    heap2->child = heap1;
    heap2->degree = heap2->degree + 1;
}
 
struct heap_node* create_heap_node(int occurenceCount, char* filename) {
    struct heap_node* new_node = (struct heap_node*) malloc(sizeof(struct heap_node));
    new_node->occurence = occurenceCount;
    new_node->filename = filename;
    return new_node;
}
 
struct heap_node* heap_union(struct heap_node* H1, struct heap_node* H2) {
    struct heap_node* H = create_bin_heap();
    struct heap_node* prev_x;
    struct heap_node* next_x;
    struct heap_node* x;
    H = heap_merge(H1, H2);
    if (H == NULL)
        return H;
    prev_x = NULL;
    x = H;
    next_x = x->sibling;
    while (next_x != NULL) {
        if ((x->degree != next_x->degree) || ((next_x->sibling != NULL)
                && (next_x->sibling)->degree == x->degree)) {
            prev_x = x;
            x = next_x;
        } else {
            if (x->occurence >= next_x->occurence) {
                x->sibling = next_x->sibling;
                heap_link(next_x, x);
            } else {
                if (prev_x == NULL)
                    H = next_x;
                else
                    prev_x->sibling = next_x;
                heap_link(x, next_x);
                x = next_x;
            }
        }
        next_x = x->sibling;
    }
    return H;
}

struct heap_node* heap_merge(struct heap_node* H1, struct heap_node* H2) {
    struct heap_node* H = create_bin_heap();
    struct heap_node* y;
    struct heap_node* z;
    struct heap_node* a;
    struct heap_node* b;
    y = H1;
    z = H2;
    if (y != NULL) {
        if (z != NULL && y->degree <= z->degree)
            H = y;
        else if (z != NULL && y->degree > z->degree)
            H = z;
        else
            H = y;
    } else
        H = z;
    while (y != NULL && z != NULL) {
        if (y->degree < z->degree) {
            y = y->sibling;
        } else if (y->degree == z->degree) {
            a = y->sibling;
            y->sibling = z;
            y = a;
        } else {
            b = z->sibling;
            z->sibling = y;
            z = b;
        }
    }
    return H;
}
 
void print_heap(struct heap_node* H) {
    while (H) { 
        printf("%d ", H->occurence);
        print_heap(H->child); 
        H = H->sibling; 
    } 
}

struct extracted_max_heap* heap_extract_max(struct heap_node* heap) {
    struct extracted_max_heap* result;
    
    if (heap == NULL) 
       return NULL; 
  
    struct heap_node* max_prev = NULL; 
    struct heap_node* max_heap_node = heap; 
  
    int max = heap->occurence; 
    struct heap_node* curr = heap; 
    while (curr->sibling != NULL) 
    { 
        if ((curr->sibling)->occurence > max) 
        { 
            max = (curr->sibling)->occurence; 
            max_prev = curr; 
            max_heap_node = curr->sibling; 
        } 
        curr = curr->sibling; 
    } 

    // break code if no occurences
    if(max_heap_node->occurence == 0) {
        exit(0);
    }
    
    if (max_prev == NULL && max_heap_node->sibling == NULL){
        heap = NULL; 
    }
  
    else if (max_prev == NULL) {
        heap = max_heap_node->sibling; 
    }
  
    else {
        max_prev->sibling = max_heap_node->sibling; 
    }
  
    if (max_heap_node->child != NULL) { 
        reverse_list(max_heap_node->child); 
        (max_heap_node->child)->sibling = NULL; 
    } 

    result->output_heap = heap_union(heap, global_heap);
    result->max_node = max_heap_node;
    return result;
}
 
void reverse_list(struct heap_node* y) {
    if (y->sibling != NULL) { 
        reverse_list(y->sibling); 
        (y->sibling)->sibling = y; 
    } 
    else
        global_heap = y; 
}

void print_relevance_output(struct heap_node* max_node) {
    printf("%s(%d): ", max_node->filename, max_node->occurence);

    struct dirent *de; // Pointer for directory entry 
	DIR *dr = opendir("files/"); 
  	char name[50], path[50];
	  
    if (dr == NULL)  // opendir returns NULL if couldn't open directory 
    { 
        printf("Could not open current directory" ); 
    } 

	while ((de = readdir(dr)) != NULL) {
        if(strcmp(de->d_name, max_node->filename)==0){
            FILE *openedFile;
            strcpy(path, "files/");		//to get in to the directory properly
            strcpy(name, de->d_name);
            strcat(path, name);
            
            
            openedFile = fopen(path, "r");
            if (openedFile == NULL){
                printf("Could not open file %s",de->d_name);
            }

            char str[MAXCHAR];

            // Read line from file till end of file.
            while ((fgets(str, MAXCHAR, openedFile)) != NULL)
            {
                printf("%s", str);
            }

            printf("\n\n");
        }
	}
}

int main(void) {
	struct dirent *de; // Pointer for directory entry 
	DIR *dr = opendir("files/"); 
  	char name[50], path[50];
	char search[100];
	int wordCount;

    struct heap_node* H1 = create_bin_heap();
    struct heap_node* H2 = create_bin_heap();

	/* Input word to search in file */
    printf("\nEnter word to search in file: ");
    scanf("%s", search);
	  
    if (dr == NULL)  // opendir returns NULL if couldn't open directory 
    { 
        printf("Could not open current directory" ); 
        return 0; 
    } 

	while ((de = readdir(dr)) != NULL) {
        if(strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") != 0){
            // printf("%s\n", de->d_name); 
            FILE *openedFile;
            strcpy(path, "files/");		//to get in to the directory properly
            strcpy(name, de->d_name);
            strcat(path, name);
            
            
            openedFile = fopen(path, "r");
            if (openedFile == NULL){
                printf("Could not open file %s",de->d_name);
            }

            wordCount = exactMatchCount(openedFile, search);
            // printf("%s   ", de->d_name);
            // printf("%d\n", wordCount);
            H1 = heap_union(H1, create_heap_node(wordCount, de->d_name));
            H2 = heap_union(H2, create_heap_node(wordCount, de->d_name));
        }
	}
  
    struct extracted_max_heap* eH;
    struct extracted_max_heap* eH2;
    printf("\nThe relevance order is: ");
    int i;
    for(i = 0; i < 5; i++) {
        eH = heap_extract_max(H1);
        printf("%s(%d) ", eH->max_node->filename, eH->max_node->occurence);
        H1 = eH->output_heap;
    }

    printf("\n\n");

    for(i = 0; i < 5; i++) {
        eH2 = heap_extract_max(H2);
        print_relevance_output(eH2->max_node);
        H2 = eH2->output_heap;
    }

	closedir(dr);     
    return 0; 
}
