#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <time.h>
#include <limits.h>
#include <errno.h>
#define TRUE 1
#define FALSE !TRUE
#define MAX_FUNCTION_SIZE 128
#define CKH_MAX_DECIMAL_FORMAT 64
#define MALLOC(type, n) (type *)malloc((n) * sizeof(type))
#define CALLOC(type, n) (type *)calloc((n), sizeof(type))
#define REALLOC(data, type, n) (type *)realloc(data, n * sizeof(type))
#define KEY_CMP(s1, s2) strcmp((const char *)s1, (const char *)s2)
#define EXIT_IF(expr) \
	do { \
		if (expr) { \
			fprintf(stderr, "\n%s:%d: In function `%s': %s\n\n", \
				__FILE__, __LINE__, __FUNCTION__, \
				strerror(errno)); \
			fflush(stderr); \
			exit(EXIT_FAILURE); \
		} \
	} while (0)
#define EXIT_HOW_IF(expr, where, fmt, ...) \
	do { \
		if (expr) { \
			if (where) \
				fprintf(stderr, "\n%s:%d: In function `%s': ", \
					__FILE__, __LINE__, __FUNCTION__); \
			else \
				fprintf(stderr, "\n"); \
			fprintf(stderr, fmt, ##__VA_ARGS__); \
			fprintf(stderr, "\n\n"); \
			fflush(stderr); \
			exit(EXIT_FAILURE); \
		} \
	} while (0)

typedef struct {			/* Hash table cell type */
    unsigned char *key;		/* Hash key */
    int value;			/* Hash value */
} CKHash_Cell;

typedef struct {
    unsigned int size;		/*  Current size */
    unsigned int table_size;	/*  Size of hash tables */
    int shift;			/*  Value used for hash function */
    unsigned int min_size;		/*  Rehash trigger size */
    unsigned int mean_size;		/*  Rehash trigger size */
    unsigned int max_chain;		/*  Max iterations in insertion */
    CKHash_Cell *T1;		/*  Pointer to hash table 1 */
    CKHash_Cell *T2;		/*  Pointer to hash table 2 */
    int function_size;		/*  Size of hash function */
    int *a1;			/*  Hash function 1 */
    int *a2;			/*  Hash function 2 */
} CKHash_Table;

CKHash_Table *ckh_alloc_table(int table_size);

CKHash_Table *ckh_construct_table(int min_size);

int ckh_insert(CKHash_Table *D, const unsigned char *key, int value);

int ckh_lookup(CKHash_Table *D, const unsigned char *key);

int ckh_delete(CKHash_Table *D, const unsigned char *key);

int ckh_get(CKHash_Table *D, const unsigned char *key, int *ret_value);

int ckh_increase_value(CKHash_Table *D, const unsigned char *key);

int ckh_increase_value2(CKHash_Table *D, const unsigned char *key, int value);

int ckh_decrease_value(CKHash_Table *D, const unsigned char *key);

CKHash_Table *ckh_destruct_table(CKHash_Table *D);

void ckh_print(CKHash_Table *D);

int ckh_export_table(CKHash_Table *D, const char *file_name, const char *delimiter);

int ckh_export_key(CKHash_Table *D, const char *file_name);


typedef struct Dictionary // struct to save the data (word, meaning)
{
    char Word[20];
    char Meaning[50];
} Dict;

typedef struct Hashtable { //struct for the hashtable
    int status; // status = 0: data does not exist status  = 1 data exists
    int hashKey; // to store the hash number of the word
    char Word[20];// store the word
    char Meaning[50];// store the meaning
} hashtable;

hashtable *table; //initialize the hash table
int SIZE = 2;// start size of the hashtable
int COUNTER = 0; // counter for the entries of the table
int Choice;// user choice for hashing technique

void insert(hashtable *h_table, char word[], char meaning[]); // insert to hash table function

int hash(char key[]) {// function to return the word hash key = (first char * 29^0 + second char * 29^1 + third char * 29^2)
    int value = 0;
    int i = 0;
    int key_len = strlen(key);

    // do several rounds of multiplication
    for (; i < 3; ++i) { // loop through the word and calculate the hash key
        value = value + key[i] * pow(29, i);
    }
    return value;
}

int LinearHash(int key, int i) { // liner probing technique
    return abs((key + i) % SIZE);
}

int QuadraticHash(int key, int i) {// quadratic probing technique
    return abs((key + (i * i)) % SIZE);
}

int is_prime(int num) {// return 0 if the number is prime and 1 if it is not
    int i, flag = 0;
    for (i = 2; i <= num / 2; ++i) {
        // condition for non-prime
        if (num % i == 0) {
            flag = 1;
            break;
        }
    }
    // number is prime
    return flag;
}

int newSize() {// find the new size of the table after rehashing
    int size2 = SIZE;
    size2 *= 2;
    while (is_prime(size2) != 0) {
        size2++;// loop until it finds the nearest prime number after doubling the size
    }
    return size2;
}

void create_HashTable(hashtable h_table[], int size) { // initializing the hash table
    int i;
    for (i = 0; i < size; i++) {
        h_table[i].status = 0;// set the status to 0 (empty)
        h_table[i].hashKey = 0;// set hash ke to zero
        strcpy(h_table[i].Word, "");// empty the word spot
        strcpy(h_table[i].Meaning, "");// empty the meaning spot
    }
}
/*
 * function for rehashing the table after the load factor is > 0.65
 */
hashtable *reHash(hashtable h_table[]) {
    int size = newSize();// find the new suitable size of the hash table
    hashtable *table_new = (struct Hashtable *) calloc(size, sizeof(struct Hashtable)); // allocate new temp hash table
    if (table_new == NULL){
        perror("failed to allocate new table");
        exit(1);
    }
    create_HashTable(table_new, size);// initialize the new hash table
    int O_size = SIZE;
    SIZE = size;
    for (int i = 0; i < O_size; i++) {// loop through the old hash table to insert the old hash table element in the new one
        if (h_table[i].status == 1) {
            insert(table_new, h_table[i].Word, h_table[i].Meaning);// insert the element
            strcpy(h_table[i].Meaning, "");
            strcpy(h_table[i].Word, "");
            h_table[i].status = 0;
            h_table[i].hashKey = 0;
        }
    }

    free(h_table);// free the old hash table
    return table_new;// return the new one
}
/*
 * function to insert the new element from the dictionary file
 */
void insert(hashtable h_table[], char word[], char meaning[]) {
    int key = hash(word);// find the key of the word
    int index = key % SIZE;// calculate the position of the word in the hash table
    int k;
    for (int i = 0; i < SIZE; i++) {// loop through the table
        if (Choice == 1)
            k = i; // if the choice is linear probing
        else
            k = i * i; // uf the choice in quadratic probing
        index = (index + k) % SIZE;
        if (h_table[index].status == 0) {// if the status in equal to zero inert the content to the array
            h_table[index].hashKey = key;
            strcpy(h_table[index].Word, word);
            strcpy(h_table[index].Meaning, meaning);
            h_table[index].status = 1;
            break;
        }
    }
}

void PrintHashTable(hashtable h_table[]) {// function to print the hashtable into the output with empty slots
    int c = 0;
    for (int i = 0; i < SIZE; ++i) {// loop through the table
        c++;// counter for the element
        if (h_table[i].status == 0) {
            c--;
            printf("Empty\n");
        } else {
            printf("slot [%4d] : ", c);
            printf("%s = %s \n", h_table[i].Word, h_table[i].Meaning);
        }
    }
}

void PrintDict(hashtable h_table[]) {// function to print all hash table element into the output
    int c = 0;
    for (int i = 0; i < SIZE; ++i) {
        if (h_table[i].status == 0) {
            continue;
        } else {
            c++;
            printf("[%4d] | ", c);
            printf("%s : %s \n", h_table[i].Word, h_table[i].Meaning);
        }
    }
}
/*
 * this function read form the dictionary file and then parse the file to word and meaning
 * also check for the load factor after every insertion
 * if the load factor is more than 0.65 do the rehash method to the hashtable
 */
void ReadFromFile() {
    FILE *dictionary = fopen("dictionary.txt", "r");
    if (dictionary == NULL) // file error handling
    {
        printf("\n The file could not be opened\n\n");
        exit(1);
    }
    Dict data;
    char line[128];
    while (fgets(line, sizeof(line), dictionary)) // while loop to loop line by line
    {
        while (sscanf(line, "%[^:]: %[^\n]", data.Word, data.Meaning) == 2) // sscanf to parse the line into pieces
        {
            int new_line =
                    strlen(data.Meaning) - 1; // this will remove the spaces form the last character of the meaning
            if (data.Meaning[new_line] == '\n') {
                data.Meaning[new_line] = '\0';
            }
            if (((double) (double) COUNTER / (double) SIZE) > 0.65) {
                table = reHash(table);
            }
            insert(table, data.Word, data.Meaning);
            printf(">| %13s | %s \n", data.Word, data.Meaning);
            COUNTER++;
            break;
        }
    }
    fclose(dictionary);
}

void Heading(char heading[]) // function to print the heading
{
    printf("\n___________________________________________________________________________________________________\n \t%s \n___________________________________________________________________________________________________\n",
           heading);
}

void find_word() {// this function finds the element on the hash table
    Dict data;
    printf("\nEnter A Word :");
    scanf("%s", data.Word);
    char trash;
    scanf("%c", &trash);
    int key = hash(data.Word);
    int index;
    for (int i = 0; i < SIZE; ++i) {// loop until it finds the wonted element
        if (Choice == 1)
            index = LinearHash(key, i);
        else
            index = QuadraticHash(key, i);
        if (strcmp(data.Word, table[index].Word) == 0) {
            printf("The Meaning Of (%s) Is : %s \n", data.Word, table[index].Meaning);
            break;
        }
    }
}

void Update_Meaning() {// function to update the meaning of existing word in the hash table
    Dict data;
    printf("\nEnter A Word :");
    scanf("%s", data.Word);
    char trash;
    scanf("%c", &trash);
    int key = hash(data.Word);
    int index;
    for (int i = 0; i < SIZE; ++i) {// loop until it finds the wonted element
        if (Choice == 1)
            index = LinearHash(key, i);
        else
            index = QuadraticHash(key, i);
        if (strcmp(data.Word, table[index].Word) == 0) {
            printf("Enter A New Meaning For the Word:");
            scanf("%[^\n]", data.Meaning);
            printf("\nThe Previous Meaning : %s \n", table[index].Meaning);
            strcpy(table[index].Meaning, data.Meaning);
            printf("The New Meaning : %s \n\n", table[index].Meaning);
            break;
        }
    }
}

void delete_word() {// function to delete an existing word in the hashtable
    Dict data;
    printf("\nEnter A Word :");
    scanf("%s", data.Word);
    char trash;
    scanf("%c", &trash);
    int key = hash(data.Word);
    int index;
    for (int i = 0; i < SIZE; ++i) {
        if (Choice == 1)
            index = LinearHash(key, i);
        else
            index = QuadraticHash(key, i);
        if (strcmp(data.Word, table[index].Word) == 0) {
            strcpy(table[index].Word, "");
            strcpy(table[index].Meaning, "");
            table[index].hashKey = 0;
            table[index].status = 0;
            break;
        }
    }

}

void Insert_new_word() // function to insert new word in the tree (case 3)
{
    Dict data;
    printf("\nEnter A Word :");
    scanf("%s", data.Word);
    char trash;
    scanf("%c", &trash);
    printf("Enter A Meaning :");
    scanf("%[^\n]", data.Meaning);
    insert(table, data.Word, data.Meaning);
    printf("\n\nThe Word Was Inserted Successfully\n");
}

void PrintHashTable_OnFile() {// this function for printing the hashtable into new file
    FILE *out = fopen("dictionary_modified.txt", "w");// open "dictionary_modified.txt" and write to it
    int n = 0;
    // loop through the hash table
    for (int i = 0; i < SIZE; i++) {
        // if the slot does not have any element
        if (table[i].status == 0) {
            fprintf(out, "[Empty]\n");
        } else {// if the slot is not empty
            n++;
            fprintf(out, "[%5d] %s : %s  -> key : %d\n", n, table[i].Word, table[i].Meaning, table[i].hashKey);
        }
    }
    // close the file
    fclose(out);
}

void show_menu() //function for the command window
{
    while (1) {

        printf("___________________________________________________________________________________________________");
        printf("\n___________________________________________________________________________________________________\n");

        printf("\nCommands:\n\n");
        // print the command
        printf("1 > Search for a word and display its meaning.\n");
        printf("2 > Update the meaning of an existing word.\n");
        printf("3 > Insert a new word from the user with its meaning. \n");
        printf("4 > Print all words in the dictionary with their associated meanings.\n");
        printf("5 > Print the entire hash table including the empty slots.\n");
        printf("6 > Delete a word from the dictionary.\n");
        printf("7 > Print the number of occupied slots, hash table size and the load factor.\n");
        printf("8 > Save all words back in file dictionary_modified.txt.\n");
        printf("9 > Quit\n");

        printf("\n >Enter command number to execute :");

        int command;
        char str[3];
        scanf(" %s", str);
        command = (int) strtol(str, NULL, 10);
        while (command < 1 || command > 9) {// loop until the user enter valid number of the command
            puts("please enter a valid option:");
            scanf("%s", str);
            command = (int) strtol(str, NULL, 10);
        }

        switch (command) //switch statement to execute the entered number
        {
            case 1:
                Heading("Search for a word");
                find_word();
                break;

            case 2:
                Heading("Update The Meaning");
                Update_Meaning();
                break;

            case 3:
                Heading("Insert New Word");
                Insert_new_word();
                break;

            case 4:
                Heading("Print All Words In The Dictionary With Their Associated Meanings");
                PrintDict(table);
                break;
            case 5:
                Heading("Print The Entire Hash Table Including The Empty Slots");
                PrintHashTable(table);
                break;
            case 6:
                Heading("Delete A Word");
                delete_word();
                break;

            case 7:
                Heading("Print the number of occupied slots, hash table size and the load factor.");
                printf("Occupied Slots : %d\n"
                       "Hashtable Size : %d\n"
                       "Load Factor = (%d/%d) * 100%% = %2.1lf%%\n", COUNTER, SIZE, COUNTER, SIZE,
                       (double) (((double) COUNTER / (double) SIZE) * 100));
                break;
            case 8:
                Heading("Save all words back in file dictionary_modified.txt.");
                PrintHashTable_OnFile();
                printf("\n\t\tDONE!\n\n");
                break;
            case 9:
                printf("Quiting...");
                exit(1);
            default:
                break;
        }
    }
}

int main() {
    printf(("[1] Linear Probing\n[2] Quadratic Probing\n")); // ask the user to choose between linear probing and quadratic probing
    char c[3];
    scanf("%s", c);
    Choice = (int) strtol(c, NULL, 10);// to handle the conversation from character to integer
    table = (struct Hashtable *) calloc(SIZE, sizeof(struct Hashtable)); // allocate the hash table
    create_HashTable(table, SIZE);// initialize the hash table
    ReadFromFile();// read from the file
    show_menu();// display the menu of the command center
    return 0;// the end
}

inline void ckh_init(int a[], int function_size)
{
    int i;

    for (i = 0; i < function_size; i++)
        a[i] = ((int)rand() << 1) + 1;
}

inline void ckh_hash(unsigned long *h, int a[], int function_size,
                     int table_size, int shift, const unsigned char *key)
{
    int i;

    *h = 0;
    for (i = 0; key[i]; i++)
        *h ^= (unsigned int)(a[(i % function_size)] * key[i]);
    *h = ((unsigned int)*h >> shift) % table_size;
}

CKHash_Table *ckh_alloc_table(int table_size)
{
    CKHash_Table *D = CALLOC(CKHash_Table, 1);
    EXIT_IF(D == NULL);
    D->size = 0;
    D->table_size = table_size;
    D->mean_size = 5 * (2 * table_size) / 12;
    D->min_size = (2 * table_size) / 5;
    D->shift = 32 - (int)(log(table_size) / log(2) + 0.5);
    D->max_chain = 4 + (int)(4 * log(table_size) / log(2) + 0.5);
    D->T1 = CALLOC(CKHash_Cell, D->table_size);
    EXIT_IF(D->T1 == NULL);
    D->T2 = CALLOC(CKHash_Cell, D->table_size);
    EXIT_IF(D->T2 == NULL);
    D->function_size = MAX_FUNCTION_SIZE;
    D->a1 = MALLOC(int, D->function_size);
    EXIT_IF(D->a1 == NULL);
    D->a2 = MALLOC(int, D->function_size);
    EXIT_IF(D->a2 == NULL);
    ckh_init(D->a1, D->function_size);
    ckh_init(D->a2, D->function_size);

    return D;
}

CKHash_Table *ckh_construct_table(int min_size)
{
    srand(time(NULL));

    return ckh_alloc_table(min_size);
}

int ckh_rehash_insert(CKHash_Table *D, unsigned char *key, int value)
{
    unsigned long hkey;
    unsigned int j;
    CKHash_Cell x, temp;

    x.key = key;
    x.value = value;

    for (j = 0; j < D->max_chain; j++) {
        ckh_hash(&hkey, D->a1, D->function_size, D->table_size,
                 D->shift, x.key);
        temp = D->T1[hkey];
        D->T1[hkey] = x;
        if (temp.key == NULL)
            return TRUE;

        x = temp;
        ckh_hash(&hkey, D->a2, D->function_size, D->table_size,
                 D->shift, x.key);
        temp = D->T2[hkey];
        D->T2[hkey] = x;
        if (temp.key == NULL)
            return TRUE;

        x = temp;
    }

    for (j = 0; j < D->table_size; j++) {
        D->T1[j].key = D->T2[j].key = NULL;
        D->T1[j].value = D->T2[j].value = 0;
    }

    ckh_init(D->a1, D->function_size);
    ckh_init(D->a2, D->function_size);

    return FALSE;
}

void ckh_rehash(CKHash_Table *D, int new_size)
{
    CKHash_Table *D_new;
    unsigned int k;

    D_new = ckh_alloc_table(new_size);

    for (k = 0; k < D->table_size; k++) {
        if ((D->T1[k].key != NULL) &&
            (!ckh_rehash_insert(D_new, D->T1[k].key, D->T1[k].value))) {
            k = -1;
            continue;
        }
        if ((D->T2[k].key != NULL) &&
            (!ckh_rehash_insert(D_new, D->T2[k].key, D->T2[k].value)))
            k = -1;
    }

    free(D->T1);
    free(D->T2);
    free(D->a1);
    free(D->a2);

    D_new->size = D->size;
    *D = *D_new;
    free(D_new);
}

int ckh_insert(CKHash_Table *D, const unsigned char *key, int value)
{
    unsigned long h1, h2;
    unsigned int j;
    CKHash_Cell x, temp;

    /*
     * If the element is already in D, then overwrite and return.
     */
    ckh_hash(&h1, D->a1, D->function_size, D->table_size, D->shift, key);
    if (D->T1[h1].key != NULL && KEY_CMP(D->T1[h1].key, key) == 0) {
        D->T1[h1].value = value;
        return FALSE;
    }

    ckh_hash(&h2, D->a2, D->function_size, D->table_size, D->shift, key);
    if (D->T2[h2].key != NULL && KEY_CMP(D->T2[h2].key, key) == 0) {
        D->T2[h2].value = value;
        return FALSE;
    }

    /*
     * If not, the insert the new element in D.
     */
    int key_len = strlen((const char *)key) + 1;
    x.key = MALLOC(unsigned char, key_len);
    EXIT_IF(x.key == NULL);

    strncpy((char *)x.key, (const char *)key, key_len);
    x.value = value;

    for (j = 0; j < D->max_chain; j++) {
        temp = D->T1[h1];
        D->T1[h1] = x;
        if (temp.key == NULL) {
            D->size++;
            if (D->table_size < D->size)
                ckh_rehash(D, 2 * D->table_size);
            return TRUE;
        }

        x = temp;
        ckh_hash(&h2, D->a2, D->function_size, D->table_size, D->shift,
                 x.key);
        temp = D->T2[h2];
        D->T2[h2] = x;
        if (temp.key == NULL) {
            D->size++;
            if (D->table_size < D->size)
                ckh_rehash(D, 2 * D->table_size);
            return TRUE;
        }

        x = temp;
        ckh_hash(&h1, D->a1, D->function_size, D->table_size, D->shift,
                 x.key);
    }

    /*
     * Forced rehash.
     */
    if (D->size < D->mean_size)
        ckh_rehash(D, D->table_size);
    else
        ckh_rehash(D, 2 * D->table_size);

    ckh_insert(D, x.key, x.value);
    free(x.key); /* Free x.key, because it is already copied. */

    return TRUE;
}

int ckh_lookup(CKHash_Table *D, const unsigned char *key)
{
    unsigned long hkey;

    ckh_hash(&hkey, D->a1, D->function_size, D->table_size, D->shift, key);
    if (D->T1[hkey].key != NULL && KEY_CMP(D->T1[hkey].key, key) == 0)
        return TRUE;

    ckh_hash(&hkey, D->a2, D->function_size, D->table_size, D->shift, key);
    if (D->T2[hkey].key != NULL && KEY_CMP(D->T2[hkey].key, key) == 0)
        return TRUE;

    return FALSE;
}

int ckh_delete(CKHash_Table *D, const unsigned char *key)
{
    unsigned long hkey;

    ckh_hash(&hkey, D->a1, D->function_size, D->table_size, D->shift, key);
    if (D->T1[hkey].key != NULL) {
        if (KEY_CMP(D->T1[hkey].key, key) == 0) {
            free(D->T1[hkey].key);
            D->T1[hkey].key = NULL;
            D->size--;
            if (D->size < D->min_size)
                ckh_rehash(D, D->table_size / 2);
            return TRUE;
        }
    }

    ckh_hash(&hkey, D->a2, D->function_size, D->table_size, D->shift, key);
    if (D->T2[hkey].key != NULL) {
        if (KEY_CMP(D->T2[hkey].key, key) == 0) {
            free(D->T2[hkey].key);
            D->T2[hkey].key = NULL;
            D->size--;
            if (D->size < D->min_size)
                ckh_rehash(D, D->table_size / 2);
            return TRUE;
        }
    }

    return FALSE;
}

int ckh_get(CKHash_Table *D, const unsigned char *key, int *ret_value)
{
    unsigned long hkey;
    int found = FALSE;

    ckh_hash(&hkey, D->a1, D->function_size, D->table_size, D->shift, key);
    if (D->T1[hkey].key != NULL && KEY_CMP(D->T1[hkey].key, key) == 0) {
        *ret_value = D->T1[hkey].value;
        found = TRUE;
    }

    ckh_hash(&hkey, D->a2, D->function_size, D->table_size, D->shift, key);
    if (D->T2[hkey].key != NULL && KEY_CMP(D->T2[hkey].key, key) == 0) {
        *ret_value = D->T2[hkey].value;
        found = TRUE;
    }

    return found;
}

int ckh_increase_value(CKHash_Table *D, const unsigned char *key)
{
    unsigned long hkey;

    ckh_hash(&hkey, D->a1, D->function_size, D->table_size, D->shift, key);
    if (D->T1[hkey].key != NULL) {
        if (KEY_CMP(D->T1[hkey].key, key) == 0) {
            D->T1[hkey].value++;
            return TRUE;
        }
    }

    ckh_hash(&hkey, D->a2, D->function_size, D->table_size, D->shift, key);
    if (D->T2[hkey].key != NULL) {
        if (KEY_CMP(D->T2[hkey].key, key) == 0) {
            D->T2[hkey].value++;
            return TRUE;
        }
    }

    return FALSE;
}

int ckh_increase_value2(CKHash_Table *D, const unsigned char *key, int value)
{
    unsigned long hkey;

    ckh_hash(&hkey, D->a1, D->function_size, D->table_size, D->shift, key);
    if (D->T1[hkey].key != NULL) {
        if (KEY_CMP(D->T1[hkey].key, key) == 0) {
            D->T1[hkey].value += value;
            return TRUE;
        }
    }

    ckh_hash(&hkey, D->a2, D->function_size, D->table_size, D->shift, key);
    if (D->T2[hkey].key != NULL) {
        if (KEY_CMP(D->T2[hkey].key, key) == 0) {
            D->T2[hkey].value += value;
            return TRUE;
        }
    }

    return FALSE;
}

int ckh_decrease_value(CKHash_Table *D, const unsigned char *key)
{
    unsigned long hkey;

    ckh_hash(&hkey, D->a1, D->function_size, D->table_size, D->shift, key);
    if (D->T1[hkey].key != NULL) {
        if (KEY_CMP(D->T1[hkey].key, key) == 0) {
            D->T1[hkey].value--;
            return TRUE;
        }
    }

    ckh_hash(&hkey, D->a2, D->function_size, D->table_size, D->shift, key);
    if (D->T2[hkey].key != NULL) {
        if (KEY_CMP(D->T2[hkey].key, key) == 0) {
            D->T2[hkey].value--;
            return TRUE;
        }
    }

    return FALSE;
}

CKHash_Table *ckh_destruct_table(CKHash_Table *D)
{
    if (D == NULL)
        return NULL;

    unsigned int j;

    for (j = 0; j < D->table_size; j++) {
        if (D->T1[j].key != NULL)
            free(D->T1[j].key);
        if (D->T2[j].key != NULL)
            free(D->T2[j].key);
    }

    free(D->T1);
    free(D->T2);
    free(D->a1);
    free(D->a2);
    free(D);

    return NULL;
}

void ckh_print(CKHash_Table *D)
{
    unsigned int i;

    for (i = 0; i < D->table_size; i++) {
        if (D->T1[i].key != NULL)
            printf("%s\t%d\n", D->T1[i].key, D->T1[i].value);

        if (D->T2[i].key != NULL)
            printf("%s\t%d\n", D->T2[i].key, D->T2[i].value);
    }
}

static int key_cmp(const void *a_, const void *b_)
{
    const char **a = (const char **)a_;
    const char **b = (const char **)b_;

    return strcmp(*a, *b);
}

int ckh_export_table(CKHash_Table *D, const char *file_name, const char *delimiter)
{
    unsigned int i, j;

    char **key = CALLOC(char *, D->size);
    j = 0;

    for (i = 0; i < D->table_size; i++) {
        if (D->T1[i].key != NULL)
            key[j++] =  (char *)D->T1[i].key;
        if (D->T2[i].key != NULL)
            key[j++] =  (char *)D->T2[i].key;
    }

    EXIT_HOW_IF(j != D->size, 1, "Element size differs: read=%d expected=%d",
                (int)j, (int)D->size);

    qsort(key, j, sizeof(char *), key_cmp);
    FILE *file_ptr = fopen(file_name, "w");

    for (i = 0; i < j; i++) {
        int value;
        EXIT_HOW_IF(!ckh_get(D, (const unsigned char *)key[i], &value), 1,
                    "Cannot find key %s", key[i]);
        fprintf(file_ptr, "%s%s%d\n", key[i], delimiter, value);
    }

    fclose(file_ptr);
    free(key);

    return TRUE;
}

int ckh_export_key(CKHash_Table *D, const char *file_name)
{
    unsigned int i, j;

    char **key = CALLOC(char *, D->size);
    j = 0;

    for (i = 0; i < D->table_size; i++) {
        if (D->T1[i].key != NULL)
            key[j++] =  (char *)D->T1[i].key;
        if (D->T2[i].key != NULL)
            key[j++] =  (char *)D->T2[i].key;
    }

    EXIT_HOW_IF(j != D->size, 1, "Element size differs: read=%d expected=%d",
                (int)j, (int)D->size);

    qsort(key, j, sizeof(char *), key_cmp);
    FILE *file_ptr = fopen(file_name, "w");

    for (i = 0; i < j; i++)
        fprintf(file_ptr, "%s\n", key[i]);

    fclose(file_ptr);
    free(key);

    return TRUE;
}