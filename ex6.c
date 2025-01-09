#include "ex6.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

# define INT_BUFFER 128

// ================================================
// Basic struct definitions from ex6.h assumed:
//   PokemonData { int id; char *name; PokemonType TYPE; int hp; int attack; EvolutionStatus CAN_EVOLVE; }
//   PokemonNode { PokemonData* data; PokemonNode* left, *right; }
//   OwnerNode   { char* ownerName; PokemonNode* pokedexRoot; OwnerNode *next, *prev; }
//   OwnerNode* ownerHead;
//   const PokemonData pokedex[];
// ================================================

// For alphabetical display, we collect all nodes in an array, then qsort by name


// --------------------------------------------------------------
// 1) Safe integer reading
// --------------------------------------------------------------

void trimWhitespace(char *str)
{
    // Remove leading spaces/tabs/\r
    int start = 0;
    while (str[start] == ' ' || str[start] == '\t' || str[start] == '\r')
        start++;

    if (start > 0)
    {
        int idx = 0;
        while (str[start])
            str[idx++] = str[start++];
        str[idx] = '\0';
    }

    // Remove trailing spaces/tabs/\r
    int len = (int)strlen(str);
    while (len > 0 && (str[len - 1] == ' ' || str[len - 1] == '\t' || str[len - 1] == '\r'))
    {
        str[--len] = '\0';
    }
}

char *myStrdup(const char *src)
{
    if (!src)
        return NULL;
    size_t len = strlen(src);
    char *dest = (char *)malloc(len + 1);
    if (!dest)
    {
        printf("Memory allocation failed in myStrdup.\n");
        return NULL;
    }
    strcpy(dest, src);
    return dest;
}

int readIntSafe(const char *prompt)
{
    char buffer[INT_BUFFER];
    int value;
    int success = 0;

    while (!success)
    {
        printf("%s", prompt);

        // If we fail to read, treat it as invalid
        if (!fgets(buffer, sizeof(buffer), stdin))
        {
            printf("Invalid input.\n");
            clearerr(stdin);
            continue;
        }

        // 1) Strip any trailing \r or \n
        //    so "123\r\n" becomes "123"
        size_t len = strlen(buffer);
        if (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r'))
            buffer[--len] = '\0';
        if (len > 0 && (buffer[len - 1] == '\r' || buffer[len - 1] == '\n'))
            buffer[--len] = '\0';

        // 2) Check if empty after stripping
        if (len == 0)
        {
            printf("Invalid input.\n");
            continue;
        }

        // 3) Attempt to parse integer with strtol
        char *endptr;
        value = (int)strtol(buffer, &endptr, 10);

        // If endptr didn't point to the end => leftover chars => invalid
        // or if buffer was something non-numeric
        if (*endptr != '\0')
        {
            printf("Invalid input.\n");
        }
        else
        {
            // We got a valid integer
            success = 1;
        }
    }
    return value;
}

// --------------------------------------------------------------
// 2) Utility: Get type name from enum
// --------------------------------------------------------------
const char *getTypeName(PokemonType type)
{
    switch (type)
    {
    case GRASS:
        return "GRASS";
    case FIRE:
        return "FIRE";
    case WATER:
        return "WATER";
    case BUG:
        return "BUG";
    case NORMAL:
        return "NORMAL";
    case POISON:
        return "POISON";
    case ELECTRIC:
        return "ELECTRIC";
    case GROUND:
        return "GROUND";
    case FAIRY:
        return "FAIRY";
    case FIGHTING:
        return "FIGHTING";
    case PSYCHIC:
        return "PSYCHIC";
    case ROCK:
        return "ROCK";
    case GHOST:
        return "GHOST";
    case DRAGON:
        return "DRAGON";
    case ICE:
        return "ICE";
    default:
        return "UNKNOWN";
    }
}

// --------------------------------------------------------------
// Utility: getDynamicInput (for reading a line into malloc'd memory)
// --------------------------------------------------------------
char *getDynamicInput()
{
    char *input = NULL;
    size_t size = 0, capacity = 1;
    input = (char *)malloc(capacity);
    if (!input)
    {
        printf("Memory allocation failed.\n");
        return NULL;
    }

    int c;
    while ((c = getchar()) != '\n' && c != EOF)
    {
        if (size + 1 >= capacity)
        {
            capacity *= 2;
            char *temp = (char *)realloc(input, capacity);
            if (!temp)
            {
                printf("Memory reallocation failed.\n");
                free(input);
                return NULL;
            }
            input = temp;
        }
        input[size++] = (char)c;
    }
    input[size] = '\0';

    // Trim any leading/trailing whitespace or carriage returns
    trimWhitespace(input);

    return input;
}

// Function to print a single Pokemon node
void printPokemonNode(PokemonNode *node)
{
    if (!node)
        return;
    printf("ID: %d, Name: %s, Type: %s, HP: %d, Attack: %d, Can Evolve: %s\n",
           node->data->id,
           node->data->name,
           getTypeName(node->data->TYPE),
           node->data->hp,
           node->data->attack,
           (node->data->CAN_EVOLVE == CAN_EVOLVE) ? "Yes" : "No");
}

// --------------------------------------------------------------
// Display Menu
// --------------------------------------------------------------
void displayMenu(OwnerNode *owner)
{
    if (!owner->pokedexRoot)
    {
        printf("Pokedex is empty.\n");
        return;
    }

    printf("Display:\n");
    printf("1. BFS (Level-Order)\n");
    printf("2. Pre-Order\n");
    printf("3. In-Order\n");
    printf("4. Post-Order\n");
    printf("5. Alphabetical (by name)\n");

    int choice = readIntSafe("Your choice: ");

    switch (choice)
    {
    case 1:
        displayBFS(owner->pokedexRoot);
        break;
    case 2:
        preOrderTraversal(owner->pokedexRoot);
        break;
    case 3:
        inOrderTraversal(owner->pokedexRoot);
        break;
    case 4:
        postOrderTraversal(owner->pokedexRoot);
        break;
    case 5:
        displayAlphabetical(owner->pokedexRoot);
        break;
    default:
        printf("Invalid choice.\n");
    }
}

// --------------------------------------------------------------
// Sub-menu for existing Pokedex
// --------------------------------------------------------------
void enterExistingPokedexMenu()
{
    OwnerNode* cur = NULL;
    // list owners
    printf("\nExisting Pokedexes:\n");
    // you need to implement a few things here :)

    printf("\nEntering %s's Pokedex...\n", cur->ownerName);

    int subChoice;
    do
    {
        printf("\n-- %s's Pokedex Menu --\n", cur->ownerName);
        printf("1. Add Pokemon\n");
        printf("2. Display Pokedex\n");
        printf("3. Release Pokemon (by ID)\n");
        printf("4. Pokemon Fight!\n");
        printf("5. Evolve Pokemon\n");
        printf("6. Back to Main\n");

        subChoice = readIntSafe("Your choice: ");

        switch (subChoice)
        {
        case 1:
            addPokemon(cur);
            break;
        case 2:
            displayMenu(cur);
            break;
        case 3:
            freePokemon(cur);
            break;
        case 4:
            pokemonFight(cur);
            break;
        case 5:
            evolvePokemon(cur);
            break;
        case 6:
            printf("Back to Main Menu.\n");
            break;
        default:
            printf("Invalid choice.\n");
        }
    } while (subChoice != 6);
}

// --------------------------------------------------------------
// Main Menu
// --------------------------------------------------------------
void mainMenu()
{
    int choice;
    do
    {
        printf("\n=== Main Menu ===\n");
        printf("1. New Pokedex\n");
        printf("2. Existing Pokedex\n");
        printf("3. Delete a Pokedex\n");
        printf("4. Merge Pokedexes\n");
        printf("5. Sort Owners by Name\n");
        printf("6. Print Owners in a direction X times\n");
        printf("7. Exit\n");
        choice = readIntSafe("Your choice:");

        switch (choice)
        {
        case 1:
            openPokedexMenu();
            break;
        case 2:
            enterExistingPokedexMenu();
            break;
        case 3:
            deletePokedex();
            break;
        case 4:
            mergePokedexMenu();
            break;
        case 5:
            sortOwners();
            break;
        case 6:
            printOwnersCircular();
            break;
        case 7:
            printf("Goodbye!\n");
            break;
        default:
            printf("Invalid.\n");
        }
    } while (choice != 7);
}

int main()
{
    mainMenu();
    freeAllOwners();
    return 0;
}

void openPokedexMenu()
{
    int starter = 0;
    char* name = NULL;
    printf("Your name:\n");
    name = getDynamicInput();
    if(findOwnerByName(name) != NULL)
    {
        printf("Name already in use\n");
        free(name);
        return;
    }
    printf("Choose Starter:\n"
                "1. Bulbasaur\n"
                "2. Charmander\n"
                "3. Squirtle\n");
    starter = readIntSafe("Your choice:");
    PokemonNode* pokemon = createPokemonNode(&pokedex[(starter-1)*3]);
    OwnerNode* newOwner = createOwner(name, pokemon);
    linkOwnerInCircularList(newOwner);
    printf("New Pokedex created for %s with starter %s.\n", newOwner->ownerName, newOwner->pokedexRoot->data->name);

}

PokemonNode *createPokemonNode(const PokemonData *data)
{
    PokemonNode *pokeNode = (PokemonNode *)malloc(sizeof(PokemonNode));
    if (pokeNode == NULL)
    {
        printf("Memory allocation failed.\n");
        return NULL;
    }
    pokeNode->data = (PokemonData *)malloc(sizeof(PokemonData));
    if (pokeNode->data == NULL)
    {
        printf("Memory allocation failed.\n");
        free(pokeNode);
        return NULL;
    }
    *pokeNode->data = *data;
    pokeNode->left = NULL;
    pokeNode->right = NULL;
    return pokeNode;
}

/**
 * @brief Create an OwnerNode for the circular owners list.
 * @param ownerName the dynamically allocated name
 * @param starter BST root for the starter Pokemon
 * @return newly allocated OwnerNode*
 * Why we made it: Each user is represented as an OwnerNode.
 */
OwnerNode *createOwner(char *ownerName, PokemonNode *starter)
{
    OwnerNode *ownerNode = (OwnerNode *)malloc(sizeof(OwnerNode));
    if (ownerName == NULL)
    {
        printf("Memory allocation failed.\n");
        return NULL;
    }
    ownerNode->ownerName = ownerName;
    PokemonNode *pokeNode = createPokemonNode(starter->data);
    ownerNode->pokedexRoot = pokeNode;
    ownerNode->next = NULL;
    ownerNode->prev = NULL;
    return ownerNode;
}

/**
 * @brief Free one PokemonNode (including name).
 * @param node pointer to node
 * Why we made it: Avoid memory leaks for single nodes.
 */
void freePokemonNode(PokemonNode *node)
{
    free(node);
}

/**
 * @brief Recursively free a BST of PokemonNodes.
 * @param root BST root
 * Why we made it: Clearing a user’s entire Pokedex means freeing a tree.
 */
void freePokemonTree(PokemonNode *root)
{
    if (root == NULL)
        return;
    freePokemonTree(root->left);
    freePokemonTree(root->right);
    freePokemonNode(root);
}

/**
 * @brief Free an OwnerNode (including name and entire Pokedex BST).
 * @param owner pointer to the owner
 * Why we made it: Deleting an owner also frees their Pokedex & name.
 */
void freeOwnerNode(OwnerNode *owner)
{
    free(owner->ownerName);
    freePokemonTree(owner->pokedexRoot);
    free(owner);
}

/**
 * @brief Insert a PokemonNode into BST by ID; duplicates freed.
 * @param root pointer to BST root
 * @param newNode node to insert
 * @return updated BST root
 * Why we made it: Standard BST insertion ignoring duplicates.
 */
PokemonNode *insertPokemonNode(PokemonNode *root, PokemonNode *newNode)
{
    if (root == NULL)
        return NULL;
    if(root->data->id > newNode->data->id)
    {
        if (root->left == NULL)
        {
            root->left = newNode;
            return root;
        }
        root->left = insertPokemonNode(root->left, newNode);
        return root;
    }
    if(root->data->id < newNode->data->id)
    {
        if (root->right == NULL)
        {
            root->right = newNode;
            return root;
        }
        root->right = insertPokemonNode(root->right, newNode);
        return root;
    }
    return root;
}

/**
 * @brief BFS search for a Pokemon by ID in the BST.
 * @param root BST root
 * @param id ID to find
 * @return pointer to found node or NULL
 * Why we made it: BFS ensures we find nodes even in an unbalanced tree.
 */
PokemonNode *searchPokemonBFS(PokemonNode *root, int id){}

/**
 * @brief Remove node from BST by ID if found (BST removal logic).
 * @param root BST root
 * @param id ID to remove
 * @return updated BST root
 * Why we made it: We handle special cases of a BST remove (0,1,2 children).
 */
PokemonNode *removeNodeBST(PokemonNode *root, int id){}

/**
 * @brief Combine BFS search + BST removal to remove Pokemon by ID.
 * @param root BST root
 * @param id the ID to remove
 * @return updated BST root
 * Why we made it: BFS confirms existence, then removeNodeBST does the removal.
 */
PokemonNode *removePokemonByID(PokemonNode *root, int id){}

/* ------------------------------------------------------------
   4) Generic BST Traversals (Function Pointers)
   ------------------------------------------------------------ */

// Please notice, it's not really generic, it's just a demonstration of function pointers.
// so don't be confused by the name, but please remember that you must use it.

typedef void (*VisitNodeFunc)(PokemonNode *);//////////////////////////////////////////////////////////////////////////////

/**
 * @brief Generic BFS traversal: call visit() on each node (level-order).
 * @param root BST root
 * @param visit function pointer for what to do with each node
 * Why we made it: BFS plus function pointers => flexible traversal.
 */
void BFSGeneric(PokemonNode *root, VisitNodeFunc visit){}

/**
 * @brief A generic pre-order traversal (Root-Left-Right).
 * @param root BST root
 * @param visit function pointer
 * Why we made it: Another demonstration of function-pointer-based traversal.
 */
void preOrderGeneric(PokemonNode *root, VisitNodeFunc visit){}

/**
 * @brief A generic in-order traversal (Left-Root-Right).
 * @param root BST root
 * @param visit function pointer
 * Why we made it: Great for seeing sorted order if BST is sorted by ID.
 */
void inOrderGeneric(PokemonNode *root, VisitNodeFunc visit){}

/**
 * @brief A generic post-order traversal (Left-Right-Root).
 * @param root BST root
 * @param visit function pointer
 * Why we made it: Another standard traversal pattern.
 */
void postOrderGeneric(PokemonNode *root, VisitNodeFunc visit){}

/**
 * @brief Print one PokemonNode’s data: ID, Name, Type, HP, Attack, Evolve?
 * @param node pointer to the node
 * Why we made it: We can pass this to BFSGeneric or others to quickly print.
 */

/* ------------------------------------------------------------
   5) Display Methods (BFS, Pre, In, Post, Alphabetical)
   ------------------------------------------------------------ */

/**
 * @brief Initialize a NodeArray with given capacity.
 * @param na pointer to NodeArray
 * @param cap initial capacity
 * Why we made it: We store pointers to PokemonNodes for alphabetical sorting.
 */
void initNodeArray(NodeArray *na, int cap){}

/**
 * @brief Add a PokemonNode pointer to NodeArray, realloc if needed.
 * @param na pointer to NodeArray
 * @param node pointer to the node
 * Why we made it: We want a dynamic list of BST nodes for sorting.
 */
void addNode(NodeArray *na, PokemonNode *node){}

/**
 * @brief Recursively collect all nodes from the BST into a NodeArray.
 * @param root BST root
 * @param na pointer to NodeArray
 * Why we made it: We gather everything for qsort.
 */
void collectAll(PokemonNode *root, NodeArray *na){}

/**
 * @brief Compare function for qsort (alphabetical by node->data->name).
 * @param a pointer to a pointer to PokemonNode
 * @param b pointer to a pointer to PokemonNode
 * @return -1, 0, or +1
 * Why we made it: Sorting by name for alphabetical display.
 */
int compareByNameNode(const void *a, const void *b){}

/**
 * @brief BFS is nice, but alphabetical means we gather all nodes, sort by name, then print.
 * @param root BST root
 * Why we made it: Provide user the option to see Pokemon sorted by name.
 */
void displayAlphabetical(PokemonNode *root){}

/**
 * @brief BFS user-friendly display (level-order).
 * @param root BST root
 * Why we made it: Quick listing in BFS order.
 */
void displayBFS(PokemonNode *root){}

/**
 * @brief Pre-order user-friendly display (Root->Left->Right).
 * @param root BST root
 * Why we made it: Another standard traversal for demonstration.
 */
void preOrderTraversal(PokemonNode *root){}

/**
 * @brief In-order user-friendly display (Left->Root->Right).
 * @param root BST root
 * Why we made it: Good for sorted output by ID if the tree is a BST.
 */
void inOrderTraversal(PokemonNode *root){}

/**
 * @brief Post-order user-friendly display (Left->Right->Root).
 * @param root BST root
 * Why we made it: Another standard traversal pattern.
 */
void postOrderTraversal(PokemonNode *root){}

/* ------------------------------------------------------------
   6) Pokemon-Specific
   ------------------------------------------------------------ */

/**
 * @brief Let user pick two Pokemon by ID in the same Pokedex to fight.
 * @param owner pointer to the Owner
 * Why we made it: Fun demonstration of BFS and custom formula for battles.
 */
void pokemonFight(OwnerNode *owner){}

/**
 * @brief Evolve a Pokemon (ID -> ID+1) if allowed.
 * @param owner pointer to the Owner
 * Why we made it: Demonstrates removing an old ID, inserting the next ID.
 */
void evolvePokemon(OwnerNode *owner){}

/**
 * @brief Prompt for an ID, BFS-check duplicates, then insert into BST.
 * @param owner pointer to the Owner
 * Why we made it: Primary user function for adding new Pokemon to an owner’s Pokedex.
 */
void addPokemon(OwnerNode *owner){}

/**
 * @brief Prompt for ID, remove that Pokemon from BST by ID.
 * @param owner pointer to the Owner
 * Why we made it: Another user function for releasing a Pokemon.
 */
void freePokemon(OwnerNode *owner){}


/* ------------------------------------------------------------
   8) Sorting Owners (Bubble Sort on Circular List)
   ------------------------------------------------------------ */

/**
 * @brief Sort the circular owners list by name.
 * Why we made it: Another demonstration of pointer manipulation + sorting logic.
 */
void sortOwners(void){}

/**
 * @brief Helper to swap name & pokedexRoot in two OwnerNode.
 * @param a pointer to first owner
 * @param b pointer to second owner
 * Why we made it: Used internally by bubble sort to swap data.
 */
void swapOwnerData(OwnerNode *a, OwnerNode *b){}

/* ------------------------------------------------------------
   9) Circular List Linking & Searching
   ------------------------------------------------------------ */

/**
 * @brief Insert a new owner into the circular list. If none exist, it's alone.
 * @param newOwner pointer to newly created OwnerNode
 * Why we made it: We need a standard approach to keep the list circular.
 */
void linkOwnerInCircularList(OwnerNode *newOwner)
{
    if (newOwner == NULL)
        return;
    if (ownerHead == NULL)
    {
        ownerHead = newOwner;
        ownerHead->next = ownerHead;
        ownerHead->prev = ownerHead;
    }
    else if (ownerHead->next == ownerHead)
    {
        ownerHead->next = newOwner;
        ownerHead->prev = newOwner;
        newOwner->next = ownerHead;
        newOwner->prev = ownerHead;
    }
    else
    {
        ownerHead->prev->next = newOwner;
        newOwner->prev = ownerHead->prev;
        ownerHead->prev = newOwner;
        newOwner->next = ownerHead;
    }
}

/**
 * @brief Remove a specific OwnerNode from the circular list, possibly updating head.
 * @param target pointer to the OwnerNode
 * Why we made it: Deleting or merging owners requires removing them from the ring.
 */
void removeOwnerFromCircularList(OwnerNode *target){}

/**
 * @brief Find an owner by name in the circular list.
 * @param name string to match
 * @return pointer to the matching OwnerNode or NULL
 * Why we made it: We often need to locate an owner quickly.
 */
OwnerNode *findOwnerByName(const char *name)
{
    if (name == NULL || ownerHead == NULL)
        return NULL;
    if(strcmp(ownerHead->ownerName, name) == 0)
        return ownerHead;
    OwnerNode *temp = ownerHead->next;
    while(strcmp(temp->ownerName, name) != 0 && temp != ownerHead)
    {
        temp = temp->next;
    }
    if (temp == ownerHead)
        return NULL;
    return temp;
}

/* ------------------------------------------------------------
   10) Owner Menus
   ------------------------------------------------------------ */

/**
 * @brief Let user pick an existing Pokedex (owner) by number, then sub-menu.
 * Why we made it: This is the main interface for adding/fighting/evolving, etc.
 */
//void enterExistingPokedexMenu(void){}

/**
 * @brief Creates a new Pokedex (prompt for name, check uniqueness, choose starter).
 * Why we made it: The main entry for building a brand-new Pokedex.
 */
//void openPokedexMenu(void){}

/**
 * @brief Delete an entire Pokedex (owner) from the list.
 * Why we made it: Let user pick which Pokedex to remove and free everything.
 */
void deletePokedex(void){}

/**
 * @brief Merge the second owner's Pokedex into the first, then remove the second owner.
 * Why we made it: BFS copy demonstration plus removing an owner.
 */
void mergePokedexMenu(void){}

/* ------------------------------------------------------------
   11) Printing Owners in a Circle
   ------------------------------------------------------------ */

/**
 * @brief Print owners left or right from head, repeating as many times as user wants.
 * Why we made it: Demonstrates stepping through a circular list in a chosen direction.
 */
void printOwnersCircular(void){}

/* ------------------------------------------------------------
   12) Cleanup All Owners at Program End
   ------------------------------------------------------------ */

/**
 * @brief Frees every remaining owner in the circular list, setting ownerHead = NULL.
 * Why we made it: Ensures a squeaky-clean exit with no leftover memory.
 */
void freeAllOwners(void){}