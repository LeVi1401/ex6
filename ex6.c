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
    if (owner->pokedexRoot == NULL)
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
        BFSGeneric(owner->pokedexRoot, displayBFS);
        break;
    case 2:
        preOrderGeneric(owner->pokedexRoot, preOrderTraversal);
        break;
    case 3:
        inOrderGeneric(owner->pokedexRoot, inOrderTraversal);
        break;
    case 4:
        postOrderGeneric(owner->pokedexRoot, postOrderTraversal);
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
    if(ownerHead == NULL)
    {
        printf("No existing Pokedexes.\n");
        return;
    }
    int index = 1;
    OwnerNode* current = ownerHead;
    // list owners
    printf("\nExisting Pokedexes:\n");
    if (current == NULL)
        return;
    printf("%d. %s\n", index, current->ownerName);
    current = current->next;
    while (current != ownerHead)
    {
        index++;
        printf("%d. %s\n", index, current->ownerName);
        current = current->next;
    }

    int owner = readIntSafe("Choose a Pokedex by number: ");
    current = ownerHead;
    for (int i = 1 ; i < owner; i++)
        current = current->next;

    printf("\nEntering %s's Pokedex...\n", current->ownerName);
    int subChoice;
    do
    {
        printf("\n-- %s's Pokedex Menu --\n", current->ownerName);
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
            addPokemon(current);
            break;
        case 2:
            displayMenu(current);
            break;
        case 3:
            freePokemon(current);
            break;
        case 4:
            pokemonFight(current);
            break;
        case 5:
            evolvePokemon(current);
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
        choice = readIntSafe("Your choice: ");

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
    printf("Your name: ");
    name = getDynamicInput();
    if(findOwnerByName(name) != NULL)
    {
        printf("Owner '%s' already exists. Not creating a new Pokedex.\n", name);
        free(name);
        return;
    }
    printf("Choose Starter:\n"
                "1. Bulbasaur\n"
                "2. Charmander\n"
                "3. Squirtle\n");
    starter = readIntSafe("Your choice: ");
    PokemonNode* pokemon = createPokemonNode(&pokedex[(starter-1)*3]);
    OwnerNode* newOwner = createOwner(name, pokemon);
    linkOwnerInCircularList(newOwner);
    printf("New Pokedex created for %s with starter %s.", newOwner->ownerName, newOwner->pokedexRoot->data->name);

}

PokemonNode *createPokemonNode(const PokemonData *data)
{
    PokemonNode *pokeNode = (PokemonNode *)malloc(sizeof(PokemonNode));
    if (pokeNode == NULL)
    {
        printf("Memory allocation failed.\n");
        free(pokeNode);
        return NULL;
    }
    pokeNode->data = data;
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
    //owner = NULL;
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
    if (newNode == NULL)
        return root;
    if (root == NULL)
    {
        PokemonNode* pokemon = createPokemonNode(newNode->data);
        root = pokemon;
        return root;
    }
    if(root->data->id > newNode->data->id)
    {
        if (root->left == NULL)
        {
            PokemonNode* pokemon = createPokemonNode(newNode->data);
            root->left = pokemon;
            return root;
        }
        root->left = insertPokemonNode(root->left, newNode);
        return root;
    }
    if(root->data->id < newNode->data->id)
    {
        if (root->right == NULL)
        {
            PokemonNode* pokemon = createPokemonNode(newNode->data);
            root->right = pokemon;
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
PokemonNode *searchPokemonBFS(PokemonNode *root, int id)
{
    if (root == NULL)
        return NULL;
    int Height = pokedexHeight(root);
    for(int i = 1 ; i <= Height ; i++)
    {
        PokemonNode *temp = searchPokemonByLevel(id, root, i);
        if(temp != NULL)
            return temp;
    }
    return NULL;
}

PokemonNode *searchPokemonByLevel(int id, PokemonNode *root, int level)
{
    if(root == NULL)
        return NULL;
    if(level == 1)
    {
        if(root->data->id == id)
            return root;
        return NULL;
    }
    PokemonNode *left = searchPokemonByLevel(id, root->left, level - 1);
    PokemonNode *right = searchPokemonByLevel(id, root->right, level - 1);
    if(left != NULL)
        return left;
    if(right != NULL)
        return right;
    return NULL;
}

int pokedexHeight(PokemonNode *root)
{
    if (root == NULL)
        return 0;
    int left = pokedexHeight(root->left);
    int right = pokedexHeight(root->right);
    return (left > right ? left : right) + 1;
}

/**
 * @brief Remove node from BST by ID if found (BST removal logic).
 * @param root BST root
 * @param id ID to remove
 * @return updated BST root
 * Why we made it: We handle special cases of a BST remove (0,1,2 children).
 */
PokemonNode *removeNodeBST(PokemonNode *root, int id)
{
    if(root == NULL || id <= 0 || id > 151)
        return NULL;
    int currentId = root->data->id;
    if(currentId > id)
    {
        root->left = removeNodeBST(root->left, id);
        return root;
    }
    if(currentId < id)
    {
        root->right = removeNodeBST(root->right, id);
        return root;
    }
    if(root->left == NULL && root->right == NULL)
    {
        freePokemonNode(root);
        root = NULL;
        return NULL;
    }
    if(root->left == NULL)
    {
        PokemonNode *temp = root;
        root = root->right;
        freePokemonNode(temp);
        return root;
    }
    if(root->right == NULL)
    {
        PokemonNode *temp = root;
        root = root->left;
        freePokemonNode(temp);
        return root;
    }
    PokemonNode *successor = findMin(root->right);
    root->data = successor->data;
    root->right = removeNodeBST(root->right, successor->data->id);
    return root;
}

PokemonNode *findMin(PokemonNode *root)
{
    if(root == NULL)
        return NULL;
    if(root->left == NULL)
        return root;
    return findMin(root->left);
}

/**
 * @brief Combine BFS search + BST removal to remove Pokemon by ID.
 * @param root BST root
 * @param id the ID to remove
 * @return updated BST root
 * Why we made it: BFS confirms existence, then removeNodeBST does the removal.
 */
PokemonNode *removePokemonByID(PokemonNode *root, int id)
{
    if (root == NULL)
        return NULL;
    if(searchPokemonBFS(root, id) == NULL)
        return root;
    return removeNodeBST(root, id);
}

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
void BFSGeneric(PokemonNode *root, VisitNodeFunc visit)
{
    visit(root);
}

/**
 * @brief A generic pre-order traversal (Root-Left-Right).
 * @param root BST root
 * @param visit function pointer
 * Why we made it: Another demonstration of function-pointer-based traversal.
 */
void preOrderGeneric(PokemonNode *root, VisitNodeFunc visit)
{
    visit(root);
}

/**
 * @brief A generic in-order traversal (Left-Root-Right).
 * @param root BST root
 * @param visit function pointer
 * Why we made it: Great for seeing sorted order if BST is sorted by ID.
 */
void inOrderGeneric(PokemonNode *root, VisitNodeFunc visit)
{
    visit(root);
}

/**
 * @brief A generic post-order traversal (Left-Right-Root).
 * @param root BST root
 * @param visit function pointer
 * Why we made it: Another standard traversal pattern.
 */
void postOrderGeneric(PokemonNode *root, VisitNodeFunc visit)
{
    visit(root);
}

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
void initNodeArray(NodeArray *na, int cap)
{
    na->capacity = cap;
    na->nodes = (PokemonNode **)malloc(cap * sizeof(PokemonNode *));
    if(na->nodes == NULL)
    {
        printf("Memory allocation failed.\n");
        return;
    }
    na->size = 0;
}

/**
 * @brief Add a PokemonNode pointer to NodeArray, realloc if needed.
 * @param na pointer to NodeArray
 * @param node pointer to the node
 * Why we made it: We want a dynamic list of BST nodes for sorting.
 */
void addNode(NodeArray *na, PokemonNode *node)
{
    if(na->size == na->capacity)
    {
        na->nodes = (PokemonNode**)realloc(na->nodes, (na->capacity * 2) * sizeof(PokemonNode*));
        if(na->nodes == NULL)
        {
            printf("Memory reallocation failed.\n");
            free(na->nodes);
            return;
        }
    }
    na->nodes[na->size] = node;
    na->size++;
}

/**
 * @brief Recursively collect all nodes from the BST into a NodeArray.
 * @param root BST root
 * @param na pointer to NodeArray
 * Why we made it: We gather everything for qsort.
 */
void collectAll(PokemonNode *root, NodeArray *na)
{
    if(root == NULL)
        return;
    addNode(na, root);
    collectAll(root->left, na);
    collectAll(root->right, na);
}

/**
 * @brief Compare function for qsort (alphabetical by node->data->name).
 * @param a pointer to a pointer to PokemonNode
 * @param b pointer to a pointer to PokemonNode
 * @return -1, 0, or +1
 * Why we made it: Sorting by name for alphabetical display.
 */
int compareByNameNode(const void *a, const void *b)
{
    PokemonNode **nodeA = (PokemonNode **)a;
    PokemonNode **nodeB = (PokemonNode **)b;
    int cmp = strcmp((*nodeA)->data->name, (*nodeB)->data->name);
    if(cmp == 0)
        return 0;
    return cmp > 0 ? 1 : -1;
}

/**
 * @brief BFS is nice, but alphabetical means we gather all nodes, sort by name, then print.
 * @param root BST root
 * Why we made it: Provide user the option to see Pokemon sorted by name.
 */
void displayAlphabetical(PokemonNode *root)
{
    if(root == NULL)
        return;
    NodeArray *array = (NodeArray*)malloc(sizeof(NodeArray));
    initNodeArray(array, pokedexHeight(root));
    collectAll(root, array);

    for(int i = 0; i < array->size - 1; i++)
    {
        for(int j = 0; j < array->size - i - 1; j++)
        {
            if(compareByNameNode(&array->nodes[j], &array->nodes[j + 1]) == 1)
            {
                PokemonNode *temp = array->nodes[j];
                array->nodes[j] = array->nodes[j + 1];
                array->nodes[j + 1] = temp;
            }
        }
    }
    for(int i = 0; i < array->size; i++)
    {
        printPokemonNode(array->nodes[i]);
    }
    free(array->nodes);
    free(array);
}

/**
 * @brief BFS user-friendly display (level-order).
 * @param root BST root
 * Why we made it: Quick listing in BFS order.
 */
void displayBFS(PokemonNode *root)
{
    if(root == NULL)
        return;
    int height = pokedexHeight(root);
    for(int i = 1 ; i <= height ; i++)
        printPokemonByLevel(root, i);
}

void printPokemonByLevel(PokemonNode *root, int level)
{
    if(root == NULL)
        return;
    if(level == 1)
        printPokemonNode(root);
    else
    {
        printPokemonByLevel(root->left, level - 1);
        printPokemonByLevel(root->right, level - 1);
    }
}


/**
 * @brief Pre-order user-friendly display (Root->Left->Right).
 * @param root BST root
 * Why we made it: Another standard traversal for demonstration.
 */
void preOrderTraversal(PokemonNode *root)
{
    if(root == NULL)
        return;
    printPokemonNode(root);
    preOrderTraversal(root->left);
    preOrderTraversal(root->right);
}

/**
 * @brief In-order user-friendly display (Left->Root->Right).
 * @param root BST root
 * Why we made it: Good for sorted output by ID if the tree is a BST.
 */
void inOrderTraversal(PokemonNode *root)
{
    if(root == NULL)
        return;
    inOrderTraversal(root->left);
    printPokemonNode(root);
    inOrderTraversal(root->right);
}

/**
 * @brief Post-order user-friendly display (Left->Right->Root).
 * @param root BST root
 * Why we made it: Another standard traversal pattern.
 */
void postOrderTraversal(PokemonNode *root)
{
    if(root == NULL)
        return;
    postOrderTraversal(root->left);
    postOrderTraversal(root->right);
    printPokemonNode(root);
}

/* ------------------------------------------------------------
   6) Pokemon-Specific
   ------------------------------------------------------------ */

/**
 * @brief Let user pick two Pokemon by ID in the same Pokedex to fight.
 * @param owner pointer to the Owner
 * Why we made it: Fun demonstration of BFS and custom formula for battles.
 */
void pokemonFight(OwnerNode *owner)
{
    if(owner == NULL)
        return;
    if(owner->pokedexRoot == NULL)
    {
        printf("Pokedex is empty.\n");
        return;
    }
    int firstId = readIntSafe("Enter ID of the first Pokemon: ");
    int secondId = readIntSafe("Enter ID of the second Pokemon: ");
    if(searchPokemonBFS(owner->pokedexRoot, firstId) == NULL ||
        searchPokemonBFS(owner->pokedexRoot, secondId) == NULL)
    {
        printf("One or both Pokemon IDs not found.\n");
        return;
    }
    double firstPokemon = pokedex[firstId - 1].attack * 1.5 + pokedex[firstId - 1].hp * 1.2;
    double secondPokemon = pokedex[secondId - 1].attack * 1.5 + pokedex[secondId - 1].hp * 1.2;
    printf("Pokemon 1: %s (Score = %.2f)\n", pokedex[firstId - 1].name, firstPokemon);
    printf("Pokemon 2: %s (Score = %.2f)\n", pokedex[secondId - 1].name, secondPokemon);
    if(firstPokemon > secondPokemon)
        printf("%s wins!\n", pokedex[firstId - 1].name);
    else
        printf("%s wins!\n", pokedex[secondId - 1].name);
}

/**
 * @brief Evolve a Pokemon (ID -> ID+1) if allowed.
 * @param owner pointer to the Owner
 * Why we made it: Demonstrates removing an old ID, inserting the next ID.
 */
void evolvePokemon(OwnerNode *owner)
{
    if(owner == NULL)
        return;
    if(owner->pokedexRoot == NULL)
    {
        printf("Cannot evolve. Pokedex empty.\n");
        return;
    }
    int id = readIntSafe("Enter ID of Pokemon to evolve: ");
    if(searchPokemonBFS(owner->pokedexRoot, id) == NULL)
    {
        printf("No Pokemon with ID %d found.\n", id);
        return;
    }
    if(pokedex[id - 1].CAN_EVOLVE == CANNOT_EVOLVE)
    {
        printf("Cannot evolve.\n");
        return;
    }
    if(searchPokemonBFS(owner->pokedexRoot, id + 1) != NULL)
    {
        PokemonNode *pokemon = searchPokemonBFS(owner->pokedexRoot, id);
        pokemon->data = &pokedex[pokemon->data->id];
        freeDuplicate(owner->pokedexRoot, id + 1, pokemon);
    }
    else
    {
        PokemonNode *pokemon = searchPokemonBFS(owner->pokedexRoot, id);
        pokemon->data = &pokedex[pokemon->data->id];
    }
    printf("Pokemon evolved from %s (ID %d) to %s (ID %d).\n", pokedex[id - 1].name, id, pokedex[id + 1 - 1].name, id + 1);
}

PokemonNode* freeDuplicate(PokemonNode* root, int id, PokemonNode *node)
{
    int currentId = root->data->id;
    if(currentId > id)
    {
        root->left = freeDuplicate(root->left, id, node);
        return root;
    }
    if(currentId < id)
    {
        root->right = freeDuplicate(root->right, id, node);
        return root;
    }
    if(root == node)
    {
        if(root->left == NULL && root->right == NULL)
        {
            freePokemonNode(root);
            root = NULL;
            return NULL;
        }
        if(root->left == NULL)
        {
            PokemonNode *temp = root;
            root = root->right;
            freePokemonNode(temp);
            return root;
        }
        if(root->right == NULL)
        {
            PokemonNode *temp = root;
            root = root->left;
            freePokemonNode(temp);
            return root;
        }
        PokemonNode *successor = findMin(root->right);
        root->data = successor->data;
        root->right = freeDuplicate(root->right, successor->data->id, successor);
    }
    return root;
}

/**
 * @brief Prompt for an ID, BFS-check duplicates, then insert into BST.
 * @param owner pointer to the Owner
 * Why we made it: Primary user function for adding new Pokemon to an owner’s Pokedex.
 */
void addPokemon(OwnerNode *owner)
{
    int pokemonId = readIntSafe("Enter ID to add: ");
    if(pokemonId <= 0 || pokemonId > 151)
    {
        printf("Invalid ID.\n");
        return;
    }
    PokemonNode *temp = searchPokemonBFS(owner->pokedexRoot, pokemonId);
    if(temp != NULL)
    {
        printf("Pokemon with ID %d is already in the Pokedex. No changes made.\n", pokemonId);
        return;
    }
    const PokemonData *pokemon = &pokedex[pokemonId - 1];
    PokemonNode *newPokemon = createPokemonNode(pokemon);
    owner->pokedexRoot = insertPokemonNode(owner->pokedexRoot, newPokemon);
    freePokemonNode(newPokemon);
    printf("Pokemon %s (ID %d) added.\n", newPokemon->data->name, pokemonId);
}

/**
 * @brief Prompt for ID, remove that Pokemon from BST by ID.
 * @param owner pointer to the Owner
 * Why we made it: Another user function for releasing a Pokemon.
 */
void freePokemon(OwnerNode *owner)
{
    if(owner->pokedexRoot == NULL)
    {
        printf("No Pokemon to release.\n");
        return;
    }
    int pokemonId = readIntSafe("Enter Pokemon ID to release: ");
    if(searchPokemonBFS(owner->pokedexRoot, pokemonId) != NULL)
    {
        owner->pokedexRoot = removePokemonByID(owner->pokedexRoot, pokemonId);
        printf("Removing Pokemon %s (ID %d).\n", pokedex[pokemonId - 1].name, pokemonId);
    }
    else
        printf("Pokemon with ID %d not found.\n", pokemonId);
}

/* ------------------------------------------------------------
   8) Sorting Owners (Bubble Sort on Circular List)
   ------------------------------------------------------------ */

/**
 * @brief Sort the circular owners list by name.
 * Why we made it: Another demonstration of pointer manipulation + sorting logic.
 */
void sortOwners()
{
    int amount = ownersAmount();
    if(amount == 0 || amount == 1)
    {
        printf("0 or 1 owners only => no need to sort.\n");
        return;
    }
    for(int i = 0 ; i < amount ; i++)
    {
        OwnerNode* temp = ownerHead;
        for(int j = 0 ; j < amount - i; j++, temp = temp->next)
        {
            if(strcmp(temp->ownerName, temp->next->ownerName) > 0)
            {
                swapOwnerData(temp, temp->next);
            }
        }
    }
    printf("Owners sorted by name.\n");
}

/**
 * @brief Helper to swap name & pokedexRoot in two OwnerNode.
 * @param a pointer to first owner
 * @param b pointer to second owner
 * Why we made it: Used internally by bubble sort to swap data.
 */
void swapOwnerData(OwnerNode *a, OwnerNode *b)
{
    PokemonNode* temp = a->pokedexRoot;
    a->pokedexRoot = b->pokedexRoot;
    b->pokedexRoot = temp;
    char* tempName = a->ownerName;
    a->ownerName = b->ownerName;
    b->ownerName = tempName;
}

int ownersAmount()
{
    if(ownerHead == NULL)
        return 0;
    int count = 1;
    OwnerNode* owner = ownerHead->next;
    while(owner != ownerHead)
    {
        count++;
        owner = owner->next;
    }
    return count;
}

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
void removeOwnerFromCircularList(OwnerNode *target)
{
    if (target == NULL || ownerHead == NULL)
        return;
    if(target == ownerHead)
    {
        if(ownerHead->next == ownerHead)
        {
            freeOwnerNode(ownerHead);
            ownerHead = NULL;
            return;
        }
        OwnerNode *temp = ownerHead;
        ownerHead->prev->next = ownerHead->next;
        ownerHead->next->prev = ownerHead->prev;
        ownerHead = ownerHead->next;
        freeOwnerNode(temp);
        return;
    }
    OwnerNode *temp = ownerHead->next;
    while(temp != ownerHead)
    {
        if(target == temp)
        {
            temp->prev->next = temp->next;
            temp->next->prev = temp->prev;
            freeOwnerNode(temp);
            return;
        }
        temp = temp->next;
    }
    printf("owner do not exist ERROR\n");
}

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
void deletePokedex()
{
    int index = 1;
    OwnerNode* current = ownerHead;
    if (current == NULL)
    {
        printf("No existing Pokedexes to delete.\n");
        return;
    }
    printf("\n=== Delete a Pokedex ===\n");
    printf("%d. %s\n", index, current->ownerName);
    current = current->next;
    while (current != ownerHead)
    {
        index++;
        printf("%d. %s\n", index, current->ownerName);
        current = current->next;
    }

    int owner = readIntSafe("Choose a Pokedex to delete by number: ");
    current = ownerHead;
    for (int i = 1 ; i < owner; i++)
        current = current->next;

    printf("Deleting %s's entire Pokedex...\n", current->ownerName);
    removeOwnerFromCircularList(current);
    printf("Pokedex deleted.\n");
}

/**
 * @brief Merge the second owner's Pokedex into the first, then remove the second owner.
 * Why we made it: BFS copy demonstration plus removing an owner.
 */
void mergePokedexMenu()
{
    if (ownerHead == NULL)
    {
        printf("Not enough owners to merge.\n");
        return;
    }
    if (ownerHead->next == ownerHead)
    {
        printf("Not enough owners to merge.\n");
        return;
    }
    printf("\n=== Merge Pokedexes ===\n");
    printf("Enter name of first owner: ");
    char* name1 = getDynamicInput();
    printf("Enter name of second owner: ");
    char* name2 = getDynamicInput();
    printf("Merging %s and %s...\n", name1, name2);
    OwnerNode* firstOwner = findOwnerByName(name1);
    OwnerNode* secondOwner = findOwnerByName(name2);
    if (firstOwner == NULL || secondOwner == NULL)
        return;

    int height = pokedexHeight(secondOwner->pokedexRoot);
    for (int i = 1; i <= height; i++)
    {
        firstOwner->pokedexRoot = insertPokemonByLevel(secondOwner->pokedexRoot, i, firstOwner->pokedexRoot);
    }
    printf("Merge completed.\n");
    removeOwnerFromCircularList(secondOwner);
    printf("Owner '%s' has been removed after merging.\n", name2);
    free(name1);
    free(name2);
}

PokemonNode* insertPokemonByLevel(PokemonNode* root, int level, PokemonNode* ownerRoot)
{
    if(root == NULL)
        return NULL;
    if (level == 1)
    {
        ownerRoot = insertPokemonNode(ownerRoot, root);
    }
    else
    {
        insertPokemonByLevel(root->left, level - 1, ownerRoot);
        insertPokemonByLevel(root->right, level - 1, ownerRoot);
    }
    return ownerRoot;
}

/* ------------------------------------------------------------
   11) Printing Owners in a Circle
   ------------------------------------------------------------ */

/**
 * @brief Print owners left or right from head, repeating as many times as user wants.
 * Why we made it: Demonstrates stepping through a circular list in a chosen direction.
 */
void printOwnersCircular()
{
    if(ownerHead == NULL)
    {
        printf("No owners.\n");
        return;
    }
    char direction;
    printf("Enter direction (F or B): ");
    scanf(" %c", &direction);
    scanf("%*c");
    if (direction == 'F' || direction == 'f')
        direction = 'F';
    else if (direction == 'B' || direction == 'b')
        direction = 'B';
    int steps = readIntSafe("How many prints? ");
    OwnerNode* current = ownerHead;
    for(int i = 0 ; i < steps ; i++)
    {
        printf("[%d] %s\n", i + 1, current->ownerName);
        if(direction == 'F')
            current = current->next;
        else
            current = current->prev;
    }

}

/* ------------------------------------------------------------
   12) Cleanup All Owners at Program End
   ------------------------------------------------------------ */

/**
 * @brief Frees every remaining owner in the circular list, setting ownerHead = NULL.
 * Why we made it: Ensures a squeaky-clean exit with no leftover memory.
 */
void freeAllOwners()
{
    if (ownerHead == NULL)
        return;
    OwnerNode* current = ownerHead->next;
    OwnerNode* temp;
    while(current != ownerHead)
    {
        temp = current;
        current = current->next;
        freeOwnerNode(temp);
    }
    freeOwnerNode(current);
    ownerHead = NULL;
}