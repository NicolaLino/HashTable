#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct Dictionary // struct to save the data (word, meaning)
{
    char Word[20];
    char Meaning[50];
} Dict;

typedef struct Hashtable
{                     // struct for the hashtable
    int status;       // status = 0: data does not exist status  = 1 data exists
    int hashKey;      // to store the hash number of the word
    char Word[20];    // store the word
    char Meaning[50]; // store the meaning
} hashtable;

hashtable *table; // initialize the hash table
int SIZE = 2;     // start size of the hashtable
int COUNTER = 0;  // counter for the entries of the table
int Choice;       // user choice for hashing technique

void insert(hashtable *h_table, char word[], char meaning[]); // insert to hash table function

int hash(char key[])
{ // function to return the word hash key = (first char * 29^0 + second char * 29^1 + third char * 29^2)
    int value = 0;
    int i = 0;
    int key_len = strlen(key);

    // do several rounds of multiplication
    for (; i < 3; ++i)
    { // loop through the word and calculate the hash key
        value = value + key[i] * pow(29, i);
    }
    return value;
}

int LinearHash(int key, int i)
{ // liner probing technique
    return abs((key + i) % SIZE);
}

int QuadraticHash(int key, int i)
{ // quadratic probing technique
    return abs((key + (i * i)) % SIZE);
}

int is_prime(int num)
{ // return 0 if the number is prime and 1 if it is not
    int i, flag = 0;
    for (i = 2; i <= num / 2; ++i)
    {
        // condition for non-prime
        if (num % i == 0)
        {
            flag = 1;
            break;
        }
    }
    // number is prime
    return flag;
}

int newSize()
{ // find the new size of the table after rehashing
    int size2 = SIZE;
    size2 *= 2;
    while (is_prime(size2) != 0)
    {
        size2++; // loop until it finds the nearest prime number after doubling the size
    }
    return size2;
}

void create_HashTable(hashtable h_table[], int size)
{ // initializing the hash table
    int i;
    for (i = 0; i < size; i++)
    {
        h_table[i].status = 0;          // set the status to 0 (empty)
        h_table[i].hashKey = 0;         // set hash ke to zero
        strcpy(h_table[i].Word, "");    // empty the word spot
        strcpy(h_table[i].Meaning, ""); // empty the meaning spot
    }
}
/*
 * function for rehashing the table after the load factor is > 0.65
 */
hashtable *reHash(hashtable h_table[])
{
    int size = newSize();                                                              // find the new suitable size of the hash table
    hashtable *table_new = (struct Hashtable *)calloc(size, sizeof(struct Hashtable)); // allocate new temp hash table
    if (table_new == NULL)
    {
        perror("failed to allocate new table");
        exit(1);
    }
    create_HashTable(table_new, size); // initialize the new hash table
    int O_size = SIZE;
    SIZE = size;
    for (int i = 0; i < O_size; i++)
    { // loop through the old hash table to insert the old hash table element in the new one
        if (h_table[i].status == 1)
        {
            insert(table_new, h_table[i].Word, h_table[i].Meaning); // insert the element
            strcpy(h_table[i].Meaning, "");
            strcpy(h_table[i].Word, "");
            h_table[i].status = 0;
            h_table[i].hashKey = 0;
        }
    }

    free(h_table);    // free the old hash table
    return table_new; // return the new one
}
/*
 * function to insert the new element from the dictionary file
 */
void insert(hashtable h_table[], char word[], char meaning[])
{
    int key = hash(word);   // find the key of the word
    int index = key % SIZE; // calculate the position of the word in the hash table
    int k;
    for (int i = 0; i < SIZE; i++)
    { // loop through the table
        if (Choice == 1)
            k = i; // if the choice is linear probing
        else
            k = i * i; // uf the choice in quadratic probing
        index = (index + k) % SIZE;
        if (h_table[index].status == 0)
        { // if the status in equal to zero inert the content to the array
            h_table[index].hashKey = key;
            strcpy(h_table[index].Word, word);
            strcpy(h_table[index].Meaning, meaning);
            h_table[index].status = 1;
            break;
        }
    }
}

void PrintHashTable(hashtable h_table[])
{ // function to print the hashtable into the output with empty slots
    int c = 0;
    for (int i = 0; i < SIZE; ++i)
    {        // loop through the table
        c++; // counter for the element
        if (h_table[i].status == 0)
        {
            c--;
            printf("Empty\n");
        }
        else
        {
            printf("slot [%4d] : ", c);
            printf("%s = %s \n", h_table[i].Word, h_table[i].Meaning);
        }
    }
}

void PrintDict(hashtable h_table[])
{ // function to print all hash table element into the output
    int c = 0;
    for (int i = 0; i < SIZE; ++i)
    {
        if (h_table[i].status == 0)
        {
            continue;
        }
        else
        {
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
void ReadFromFile()
{
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
            if (data.Meaning[new_line] == '\n')
            {
                data.Meaning[new_line] = '\0';
            }
            if (((double)(double)COUNTER / (double)SIZE) > 0.65)
            {
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

void find_word()
{ // this function finds the element on the hash table
    Dict data;
    printf("\nEnter A Word :");
    scanf("%s", data.Word);
    char trash;
    scanf("%c", &trash);
    int key = hash(data.Word);
    int index;
    for (int i = 0; i < SIZE; ++i)
    { // loop until it finds the wonted element
        if (Choice == 1)
            index = LinearHash(key, i);
        else
            index = QuadraticHash(key, i);
        if (strcmp(data.Word, table[index].Word) == 0)
        {
            printf("The Meaning Of (%s) Is : %s \n", data.Word, table[index].Meaning);
            break;
        }
    }
}

void Update_Meaning()
{ // function to update the meaning of existing word in the hash table
    Dict data;
    printf("\nEnter A Word :");
    scanf("%s", data.Word);
    char trash;
    scanf("%c", &trash);
    int key = hash(data.Word);
    int index;
    for (int i = 0; i < SIZE; ++i)
    { // loop until it finds the wonted element
        if (Choice == 1)
            index = LinearHash(key, i);
        else
            index = QuadraticHash(key, i);
        if (strcmp(data.Word, table[index].Word) == 0)
        {
            printf("Enter A New Meaning For the Word:");
            scanf("%[^\n]", data.Meaning);
            printf("\nThe Previous Meaning : %s \n", table[index].Meaning);
            strcpy(table[index].Meaning, data.Meaning);
            printf("The New Meaning : %s \n\n", table[index].Meaning);
            break;
        }
    }
}

void delete_word()
{ // function to delete an existing word in the hashtable
    Dict data;
    printf("\nEnter A Word :");
    scanf("%s", data.Word);
    char trash;
    scanf("%c", &trash);
    int key = hash(data.Word);
    int index;
    for (int i = 0; i < SIZE; ++i)
    {
        if (Choice == 1)
            index = LinearHash(key, i);
        else
            index = QuadraticHash(key, i);
        if (strcmp(data.Word, table[index].Word) == 0)
        {
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

void PrintHashTable_OnFile()
{                                                      // this function for printing the hashtable into new file
    FILE *out = fopen("dictionary_modified.txt", "w"); // open "dictionary_modified.txt" and write to it
    int n = 0;
    // loop through the hash table
    for (int i = 0; i < SIZE; i++)
    {
        // if the slot does not have any element
        if (table[i].status == 0)
        {
            fprintf(out, "[Empty]\n");
        }
        else
        { // if the slot is not empty
            n++;
            fprintf(out, "[%5d] %s : %s  -> key : %d\n", n, table[i].Word, table[i].Meaning, table[i].hashKey);
        }
    }
    // close the file
    fclose(out);
}

void show_menu() // function for the command window
{
    while (1)
    {

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
        command = (int)strtol(str, NULL, 10);
        while (command < 1 || command > 9)
        { // loop until the user enter valid number of the command
            puts("please enter a valid option:");
            scanf("%s", str);
            command = (int)strtol(str, NULL, 10);
        }

        switch (command) // switch statement to execute the entered number
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
                   "Load Factor = (%d/%d) * 100%% = %2.1lf%%\n",
                   COUNTER, SIZE, COUNTER, SIZE,
                   (double)(((double)COUNTER / (double)SIZE) * 100));
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

int main()
{
    printf(("[1] Linear Probing\n[2] Quadratic Probing\n")); // ask the user to choose between linear probing and quadratic probing
    char c[3];
    scanf("%s", c);
    Choice = (int)strtol(c, NULL, 10);                                  // to handle the conversation from character to integer
    table = (struct Hashtable *)calloc(SIZE, sizeof(struct Hashtable)); // allocate the hash table
    create_HashTable(table, SIZE);                                      // initialize the hash table
    ReadFromFile();                                                     // read from the file
    show_menu();                                                        // display the menu of the command center
    return 0;                                                           // the end
}
