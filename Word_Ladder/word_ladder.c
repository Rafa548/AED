//
// AED, November 2022 (Tomás Oliveira e Silva)
//
// Second practical assignement (speed run)
//
// Place your student numbers and names here
//   N.Mec. XXXXXX  Name: XXXXXXX
//
// Do as much as you can
//   1) MANDATORY: complete the hash table code
//      *) hash_table_create
//      *) hash_table_grow
//      *) hash_table_free
//      *) find_word
//      +) add code to get some statistical data about the hash table
//   2) HIGHLY RECOMMENDED: build the graph (including union-find data) -- use the similar_words function...
//      *) find_representative
//      *) add_edge
//   3) RECOMMENDED: implement breadth-first search in the graph
//      *) breadh_first_search
//   4) RECOMMENDED: list all words belonginh to a connected component
//      *) breadh_first_search
//      *) list_connected_component
//   5) RECOMMENDED: find the shortest path between to words
//      *) breadh_first_search
//      *) path_finder
//      *) test the smallest path from bem to mal
//         [ 0] bem
//         [ 1] tem
//         [ 2] teu
//         [ 3] meu
//         [ 4] mau
//         [ 5] mal
//      *) find other interesting word ladders
//   6) OPTIONAL: compute the diameter of a connected component and list the longest word chain
//      *) breadh_first_search
//      *) connected_component_diameter
//   7) OPTIONAL: print some statistics about the graph
//      *) graph_info
//   8) OPTIONAL: test for memory leaks
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//
// static configuration
//

#define _max_word_size_ 32

//
// data structures (SUGGESTION --- you may do it in a different way)
//

typedef struct adjacency_node_s adjacency_node_t;
typedef struct hash_table_node_s hash_table_node_t;
typedef struct hash_table_s hash_table_t;

typedef struct queue_s queue_t;
typedef struct queue_node_s queue_node_t;

struct queue_s
{
  queue_t *next; // link to the next queue node
  queue_t *prev; // link to the previous queue node
  queue_t *head; // head of the queue
  queue_t *tail; // tail of the queue
  hash_table_node_t *vertex; // the vertex
};

struct queue_node_s
{
  queue_node_t *next; // link to the next queue node
  queue_node_t *prev; // link to the previous queue node
  queue_node_t *head; // head of the queue
  queue_node_t *tail; // tail of the queue
  queue_node_t *node; // the queue node
  hash_table_node_t *vertex; // the vertex
};

struct adjacency_node_s
{
  adjacency_node_t *next;    // link to the next adjacency list node
  hash_table_node_t *vertex; // the other vertex
};

struct hash_table_node_s
{
  // the hash table data
  char word[_max_word_size_]; // the word
  hash_table_node_t *next;    // next hash table linked list node
  // the vertex data
  adjacency_node_t *head;      // head of the linked list of adjancency edges
  int visited;                 // visited status (while not in use, keep it at 0)
  hash_table_node_t *previous; // breadth-first search parent
  // the union find data
  hash_table_node_t *representative; // the representative of the connected component this vertex belongs to
  int number_of_vertices;            // number of vertices of the conected component (only correct for the representative of each connected component)
  int number_of_edges;               // number of edges of the conected component (only correct for the representative of each connected component)
};

struct hash_table_s
{
  unsigned int hash_table_size;   // the size of the hash table array
  unsigned int number_of_entries; // the number of entries in the hash table
  unsigned int number_of_edges;   // number of edges (for information purposes only)
  unsigned int num_components;   // number of connected components (for information purposes only)
  unsigned int max_component_size_v;   // number of connected components (for information purposes only)
  hash_table_node_t **heads;      // the heads of the linked lists
};

//
// allocation and deallocation of linked list nodes (done)
//

static adjacency_node_t *allocate_adjacency_node(void)
{
  adjacency_node_t *node;

  node = (adjacency_node_t *)malloc(sizeof(adjacency_node_t));
  if (node == NULL)
  {
    fprintf(stderr, "allocate_adjacency_node: out of memory\n");
    exit(1);
  }
  return node;
}

static void free_adjacency_node(adjacency_node_t *node)
{
  free(node);
}

static hash_table_node_t *allocate_hash_table_node(void)
{
  hash_table_node_t *node;

  node = (hash_table_node_t *)malloc(sizeof(hash_table_node_t));
  if (node == NULL)
  {
    fprintf(stderr, "allocate_hash_table_node: out of memory\n");
    exit(1);
  }
  return node;
}

static void free_hash_table_node(hash_table_node_t *node)
{
  free(node);
}

//
// hash table stuff (mostly to be done)
//

unsigned int crc32(const char *str)
{
  static unsigned int table[256];
  unsigned int crc;

  if (table[1] == 0u) // do we need to initialize the table[] array?
  {
    unsigned int i, j;

    for (i = 0u; i < 256u; i++)
      for (table[i] = i, j = 0u; j < 8u; j++)
        if (table[i] & 1u)
          table[i] = (table[i] >> 1) ^ 0xAED00022u; // "magic" constant
        else
          table[i] >>= 1;
  }
  crc = 0xAED02022u; // initial value (chosen arbitrarily)
  while (*str != '\0')
    crc = (crc >> 8) ^ table[crc & 0xFFu] ^ ((unsigned int)*str++ << 24);
  return crc;
}

static hash_table_t *hash_table_create(void)
{
  hash_table_t *hash_table;
  unsigned int i;

  hash_table = (hash_table_t *)malloc(sizeof(hash_table_t));

  if (hash_table == NULL)
  {
    fprintf(stderr, "create_hash_table: out of memory\n");
    exit(1);
  }
  hash_table->hash_table_size = 100;
  hash_table->number_of_entries = 0;
  hash_table->number_of_edges = 0;
  hash_table->num_components = 0;
  hash_table->heads = (hash_table_node_t **)malloc(hash_table->hash_table_size * sizeof(hash_table_node_t *));
  for (i = 0; i < hash_table->hash_table_size; i++)
    hash_table->heads[i] = NULL;
  if (hash_table->heads == NULL)
  {
    fprintf(stderr, "create_hash_table: out of memory\n");
    exit(1);
  }
  printf("hash table size = %u\n", hash_table->hash_table_size);
  return hash_table;
}

static void hash_table_grow(hash_table_t *hash_table)
{

  hash_table_node_t **old_heads = hash_table->heads;
  hash_table_node_t *n2;
  hash_table_node_t *n1;
  int old_size = hash_table->hash_table_size;

  hash_table->hash_table_size = old_size + old_size / 2;
  hash_table->heads = (hash_table_node_t **)malloc(hash_table->hash_table_size * sizeof(hash_table_node_t *));
  for (int i = 0; i < hash_table->hash_table_size; i++)
  {
    hash_table->heads[i] = NULL;
  }
  for (int i = 0; i < old_size; i++)
  {
    for (n1 = old_heads[i]; n1 != NULL; n1 = n2)
    {
      n2 = n1->next; // importante nao atribuir em cima porque o valor é modificado abaixo
      int j = crc32(n1->word) % hash_table->hash_table_size;
      n1->next = hash_table->heads[j];
      hash_table->heads[j] = n1;
    }
  }
  free(old_heads);
}

static void hash_table_free(hash_table_t *hash_table)
{
  hash_table_node_t *n1;
  hash_table_node_t *n2;
  adjacency_node_t *an1;
  adjacency_node_t *an2;

  for (int i = 0; i < hash_table->hash_table_size; i++)
  {
    n1 = hash_table->heads[i];
    for (n1 = hash_table->heads[i]; n1 != NULL; n1 = n2)
    {
      n2 = n1->next;
      for (an1 = n1->head; an1 != NULL; an1 = an2)
      {
        an2 = an1->next;
        free_adjacency_node(an1);
      }
      free_hash_table_node(n1);
    }
  }
  free(hash_table->heads);
  free(hash_table);
}

static hash_table_node_t *find_word(hash_table_t *hash_table, const char *word, int insert_if_not_found)
{
  hash_table_node_t *node;
  hash_table_node_t *node2;
  unsigned int i;
  // testar
  i = crc32(word) % hash_table->hash_table_size;
  for (node = hash_table->heads[i]; node != NULL; node = node2)
  {
    if (strcmp(node->word, word) == 0)
    {
      break;
    }
    node2 = node->next;
  }
  if (insert_if_not_found)
  {
    hash_table->number_of_entries++;
    hash_table->num_components++;

    node = allocate_hash_table_node();
    
    node->head = NULL;
    node->previous = NULL;
    node->number_of_edges = 0;
    node->number_of_vertices = 1;
    node->representative = node;
    node->visited = 0;

    node->next = hash_table->heads[i];
    hash_table->heads[i] = node;
    strcpy(node->word, word);
    

    if (hash_table->number_of_entries > 2 * (hash_table->hash_table_size))
    {
      hash_table_grow(hash_table);
      printf("hash table size = %u entries = %u\n", hash_table->hash_table_size, hash_table->number_of_entries);
    }

  }
  return node;
}

//
// add edges to the word ladder graph (mostly do be done)
//

static hash_table_node_t *find_representative(hash_table_node_t *node)
{
  hash_table_node_t *n1,*n2, *n3;

  for(n1 = node;n1->representative != n1; n1 = n1->representative ) //se o representante for ele proprio, entao ele é o representante
    ;
  for(n2 = node; n2 != n1; n2 = n3) //enquanto o n2 for diferente do n1, o n3 vai ser o n2
  {
    n3 = n2->representative;
    n2->representative = n1;
  }

  return n1;
}

static void add_edge(hash_table_t *hash_table, hash_table_node_t *from, const char *word)
{
  // from é a palavra que ja esta no hash table
  // word é a palavra que esta a ser lida

  hash_table_node_t *to, *from_representative, *to_representative;
  adjacency_node_t *link;

  to = find_word(hash_table, word, 0);

  if (to == NULL)
    return;
  
  for (link = from->head; link != NULL && link->vertex != to; link = link->next)
    ;
    
  if (link != NULL)
    return;

  link = allocate_adjacency_node();
  link->vertex = to->word;
  link->next = from->head;
  from->head = link;
  

  link = allocate_adjacency_node();
  link->vertex = from->word;
  link->next = to->head;
  to->head = link;
  

  hash_table->number_of_edges++;

  from_representative = find_representative(from);
  to_representative = find_representative(to);
  
  if (from_representative != to_representative)
  {
    if (from_representative->number_of_vertices < to_representative->number_of_vertices)
    {
      to_representative->number_of_vertices += from_representative->number_of_vertices;
      to_representative->number_of_edges += from_representative->number_of_edges + 1 ;    
      from_representative->number_of_vertices = 0;
      from_representative->number_of_edges = 0;
      hash_table->num_components--;
      from_representative->representative = to_representative;
    }
    else
    {
      from_representative->number_of_vertices += to_representative->number_of_vertices;
      from_representative->number_of_edges += to_representative->number_of_edges + 1 ;
      to_representative->number_of_vertices = 0;
      to_representative->number_of_edges = 0;
      hash_table->num_components--;
      to_representative->representative = from_representative;
    }
  }else{
    from_representative->number_of_edges++;
  }
}

//
// generates a list of similar words and calls the function add_edge for each one (done)
//
// man utf8 for details on the uft8 encoding
//

static void break_utf8_string(const char *word, int *individual_characters)
{
  int byte0, byte1;

  while (*word != '\0')
  {
    byte0 = (int)(*(word++)) & 0xFF;
    if (byte0 < 0x80)
      *(individual_characters++) = byte0; // plain ASCII character
    else
    {
      byte1 = (int)(*(word++)) & 0xFF;
      if ((byte0 & 0b11100000) != 0b11000000 || (byte1 & 0b11000000) != 0b10000000)
      {
        fprintf(stderr, "break_utf8_string: unexpected UFT-8 character\n");
        exit(1);
      }
      *(individual_characters++) = ((byte0 & 0b00011111) << 6) | (byte1 & 0b00111111); // utf8 -> unicode
    }
  }
  *individual_characters = 0; // mark the end!
}

static void make_utf8_string(const int *individual_characters, char word[_max_word_size_])
{
  int code;

  while (*individual_characters != 0)
  {
    code = *(individual_characters++);
    if (code < 0x80)
      *(word++) = (char)code;
    else if (code < (1 << 11))
    { // unicode -> utf8
      *(word++) = 0b11000000 | (code >> 6);
      *(word++) = 0b10000000 | (code & 0b00111111);
    }
    else
    {
      fprintf(stderr, "make_utf8_string: unexpected UFT-8 character\n");
      exit(1);
    }
  }
  *word = '\0'; // mark the end
}

static void similar_words(hash_table_t *hash_table, hash_table_node_t *from)
{
  static const int valid_characters[] =
      {                                                                                          // unicode!
       0x2D,                                                                                     // -
       0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D,             // A B C D E F G H I J K L M
       0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A,             // N O P Q R S T U V W X Y Z
       0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D,             // a b c d e f g h i j k l m
       0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A,             // n o p q r s t u v w x y z
       0xC1, 0xC2, 0xC9, 0xCD, 0xD3, 0xDA,                                                       // Á Â É Í Ó Ú
       0xE0, 0xE1, 0xE2, 0xE3, 0xE7, 0xE8, 0xE9, 0xEA, 0xED, 0xEE, 0xF3, 0xF4, 0xF5, 0xFA, 0xFC, // à á â ã ç è é ê í î ó ô õ ú ü
       0};
  static int counter = 0;
  int i, j, k, individual_characters[_max_word_size_];
  char new_word[2 * _max_word_size_];

  break_utf8_string(from->word, individual_characters);
  for (i = 0; individual_characters[i] != 0; i++)
  {
    k = individual_characters[i];
    for (j = 0; valid_characters[j] != 0; j++)
    {
      individual_characters[i] = valid_characters[j];
      make_utf8_string(individual_characters, new_word);
      // avoid duplicate cases
      if (strcmp(new_word, from->word) > 0)
        add_edge(hash_table, from, new_word);
    }
    individual_characters[i] = k;
  }
}

//
// breadth-first search (to be done)
//
// returns the number of vertices visited; if the last one is goal, following the previous links gives the shortest path between goal and origin
//

//enqueue
static void enqueue(queue_t *queue, hash_table_node_t *node)
  {
    queue_node_t *queue_node = (queue_node_t *)malloc(sizeof(queue_node_t));
    queue_node->node = node;
    queue_node->next = NULL;
    if (queue->head == NULL)
      queue->head = queue_node;
    else
      queue->tail->next = queue_node;
    queue->tail = queue_node;
  }

  //dequeue
  hash_table_node_t *dequeue(queue_t *queue)
  {
    hash_table_node_t *node;
    queue_node_t *queue_node;

    if (queue->head == NULL)
      return NULL;
    queue_node = queue->head;
    queue->head = queue_node->next;
    if (queue->head == NULL)
      queue->tail = NULL;
    node = queue_node->node;
    free(queue_node);
    return node;
  }

  //free_queue
  static void free_queue(queue_t *queue)
  {
    queue_node_t *queue_node;

    while (queue->head != NULL)
    {
      queue_node = queue->head;
      queue->head = queue_node->next;
      free(queue_node);
    }
    free(queue);
  }


static int breadth_first_search(int maximum_number_of_vertices, hash_table_node_t **list_of_vertices, hash_table_node_t *origin, hash_table_node_t *goal)
{
  int i, j, k, n, number_of_vertices_visited = 0;
  hash_table_node_t *node, *node2;
  adjacency_node_t *adjacency_node;

  // initialize queue
  queue_t *queue = (queue_t *)malloc(sizeof(queue_t));
  queue->head = NULL;
  queue->tail = NULL;

  // enqueue origin
  enqueue(queue, origin);
  // while queue is not empty
  while (queue->head != NULL)
  {
    // dequeue node if not null
    if((node = dequeue(queue)) == NULL)
      break;

    // if node is goal
    if (node == goal)
    {
      // return number of vertices visited
      return number_of_vertices_visited;
    }

    // if node is not visited
    if (node->visited == 0)
    {
      // mark node as visited
      node->visited = 1;

      // enqueue all adjacent nodes
      adjacency_node = node->head;
      while (adjacency_node != NULL)
      { 
        if(adjacency_node->vertex->visited == 0){
          adjacency_node->vertex->previous = node;
          enqueue(queue, adjacency_node->vertex);
          adjacency_node = adjacency_node->next;
        }
      }

      // increment number of vertices visited
      number_of_vertices_visited++;
    }
    free_queue(queue);
  }
  //free queue
  return -1;
}

//
// list all vertices belonging to a connected component (complete this)
//

static void list_connected_component(hash_table_t *hash_table, const char *word)
{
  hash_table_node_t *node;
  hash_table_node_t *n1;
  hash_table_node_t *n2;
  hash_table_node_t *rep;
  hash_table_node_t *rep_n;
  int count = 0;

  node = find_word(hash_table, word, 0);

  if (node == NULL)
  {
    printf("list_connected_component: word not found\n");
    return;
  }

  rep_n = find_representative(node);

  for (int i = 0; i < hash_table->hash_table_size; i++)
  {
    n1 = hash_table->heads[i];
    for (n1 = hash_table->heads[i]; n1 != NULL; n1 = n2)
    {
      n2 = n1->next;
      rep = find_representative(n1);
      if (rep == rep_n)
      {
        count++;
        printf("%s \n", n1->word);
      }
    }
  }
  printf("Number of vertices: %d \n", count);
}

//
// compute the diameter of a connected component (optional)
//

static int largest_diameter;
static hash_table_node_t **largest_diameter_example;

static int connected_component_diameter(hash_table_node_t *node)
{
  int diameter;

  //
  // complete this
  //


  return diameter;
}

//
// find the shortest path from a given word to another given word (to be done)
//

static void path_finder(hash_table_t *hash_table, const char *from_word, const char *to_word)
{
  //
  // complete this
  //
  // find the shortest path from a given word to another given word
  // use breadth-first search
  hash_table_node_t *from = find_word(hash_table, from_word, 0);
  if (from == NULL)
  {
    printf("path_finder: from_word not found\n");
    return;
  }

  hash_table_node_t *to = find_word(hash_table, to_word, 0);
  if (to == NULL)
  {
    printf("path_finder: to_word not found\n");
    return;
  }
  // list of vertices of hash table
  hash_table_node_t **list_of_vertices = hash_table->heads;

  // breadth-first search
  int number_of_vertices_visited = breadth_first_search(hash_table->number_of_entries, list_of_vertices, from, to);

  // print result
  printf("path_finder: number of vertices visited = %d  \n", number_of_vertices_visited);

  // print path
  hash_table_node_t *node = to;
  while (node != NULL)
  {
    printf("path_finder: %s \n", node->word);
    node = node->previous;
  }
}

//
// some graph information (optional)
//

static void graph_info(hash_table_t *hash_table)
{
  printf("graph_info:number of edges = %d \n", hash_table->number_of_edges); // resultado do stor 9265
  printf("graph_info:number of vertices = %d \n", hash_table->number_of_entries); // resultado do stor 9917
  printf("graph_info:number of connected components = %d \n", hash_table->num_components); // resultado do stor 187
  hash_table_node_t *node;
  hash_table_node_t *n1;
  hash_table_node_t *n2;
  hash_table_node_t *rep;
  hash_table_node_t *rep_n;
  int countedge = 0;
  int count = 0;
  for (int i = 0; i < hash_table->hash_table_size; i++)
  {
    for (n1 = hash_table->heads[i]; n1 != NULL; n1 = n2)
    {
      n2 = n1->next;
      rep = find_representative(n1);
      if(rep->number_of_vertices>count){
        count = rep->number_of_vertices;
      }
      if(rep->number_of_edges>countedge){
        countedge = rep->number_of_edges;
      }
    }
  }
  printf("Max number of vertices of connected component: %d \n", count);
  printf("Max number of edges of connected component: %d \n", countedge);

}

//
// main program
//

int main(int argc, char **argv)
{
  char word[100], from[100], to[100];
  hash_table_t *hash_table;
  hash_table_node_t *node;
  unsigned int i;
  int command;
  FILE *fp;

  // initialize hash table
  hash_table = hash_table_create();
  // read words
  fp = fopen((argc < 2) ? "wordlist-big-latest.txt" : argv[1], "rb");
  if (fp == NULL)
  {
    fprintf(stderr, "main: unable to open the words file\n");
    exit(1);
  }
  while (fscanf(fp, "%99s", word) == 1)
    (void)find_word(hash_table, word, 1);
  fclose(fp);
  // find all similar words
  for (i = 0u; i < hash_table->hash_table_size; i++)
    for (node = hash_table->heads[i]; node != NULL; node = node->next)
      similar_words(hash_table, node);
  graph_info(hash_table);
  // ask what to do
  for (;;)
  {
    fprintf(stderr, "Your wish is my command:\n");
    fprintf(stderr, "  1 WORD       (list the connected component WORD belongs to)\n");
    fprintf(stderr, "  2 FROM TO    (list the shortest path from FROM to TO)\n");
    fprintf(stderr, "  3            (terminate)\n");
    fprintf(stderr, "> ");
    if (scanf("%99s", word) != 1)
      break;
    command = atoi(word);
    if (command == 1)
    {
      
      if (scanf("%99s", word) != 1)
        break;
      list_connected_component(hash_table, word);
    }
    else if (command == 2)
    {
      if (scanf("%99s", from) != 1)
        break;
      if (scanf("%99s", to) != 1)
        break;
      path_finder(hash_table, from, to);
    }
    else if (command == 3)
      break;
  }
  // clean up
  hash_table_free(hash_table);
  return 0;
}
