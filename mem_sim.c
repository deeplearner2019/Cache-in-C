/***************************************************************************
 * *    Inf2C-CS Coursework 2: Cache Simulation
 * *
 * *    Instructor: Boris Grot
 * *
 * *    TA: Siavash Katebzadeh
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <inttypes.h>
#include <math.h>
#include <time.h>


//REMOVE REMOVE REMOVE-------------
char* strsep(char** stringp, const char* delim)
{
  char* start = *stringp;
  char* p;

  p = (start != NULL) ? strpbrk(start, delim) : NULL;

  if (p == NULL)
  {
    *stringp = NULL;
  }
  else
  {
    *p = '\0';
    *stringp = p + 1;
  }

  return start;
}
//REMOVE REMOVE REMOVE-------------




/* Do not add any more header files */

/*
 * Various structures
 */
typedef enum
{
    FIFO,
    LRU,
    Random
} replacement_p;
//type replacement_p with 3 attributes/policies

const char *get_replacement_policy(uint32_t p)
{
    switch (p)
    {
    case FIFO:
        return "FIFO";
    case LRU:
        return "LRU";
    case Random:
        return "Random";
    default:
        assert(0);
        return "";
    };
    return "";
}
//get_rep_policy(fifo, lru...)

typedef struct
{
    uint32_t address;
} mem_access_t;
//mem_access_t type with attribute address

// These are statistics for the cache and should be maintained by you.
typedef struct
{
    int cache_hits;
    int cache_misses;
} result_t;

/////typdedef for cache block
typedef struct
{
    int valid_bit; // 0 or 1
    uint32_t tag;  //whole tag
    int tracker;

} cache_block;

/*
 * Parameters for the cache that will be populated by the provided code skeleton.
 */

replacement_p replacement_policy = FIFO;
uint32_t associativity = 0;
uint32_t number_of_cache_blocks = 0;
int cache_block_size = 0;

/*
 * Each of the variables below must be populated by you.
 */
uint32_t g_num_cache_tag_bits = 0;
uint32_t g_cache_offset_bits = 0;
result_t g_result;

/* Reads a memory access from the trace file and returns
 * 32-bit physical memory address
 */
mem_access_t read_transaction(FILE *ptr_file)
{
    char buf[1002];
    char *token = NULL;
    char *string = buf;
    mem_access_t access;

    if (fgets(buf, 1000, ptr_file) != NULL)
    {
        /* Get the address */
        token = strsep(&string, " \n"); //extract token from string
        access.address = (uint32_t)strtoul(token, NULL, 16);
        return access;
    }

    /* If there are no more entries in the file return an address 0 */
    access.address = 0;
    return access;
}

void print_statistics(uint32_t num_cache_tag_bits, uint32_t cache_offset_bits, result_t *r)
{
    /* Do Not Modify This Function */

    uint32_t cache_total_hits = r->cache_hits;
    uint32_t cache_total_misses = r->cache_misses;
    printf("CacheTagBits:%u\n", num_cache_tag_bits);
    printf("CacheOffsetBits:%u\n", cache_offset_bits);
    printf("Cache:hits:%u\n", r->cache_hits);
    printf("Cache:misses:%u\n", r->cache_misses);
    printf("Cache:hit-rate:%2.1f%%\n", cache_total_hits / (float)(cache_total_hits + cache_total_misses) * 100.0);
}

/*
 *
 * Add any global variables and/or functions here as needed.
 *
 */

///Variables related to CACHE
//uint32_t no_cache_blocks=0;

uint32_t no_index_bits = 0;
cache_block **cache;

//////declarations
uint32_t g_cache_offset_bits;
uint32_t number_of_sets;
uint32_t no_index_bits;
uint32_t g_num_cache_tag_bits;

//////cache_creator- adjustable according to set associativity
void cache_creator(number_of_cache_blocks, cache_block_size, associativity)
{
    g_cache_offset_bits = log2(cache_block_size);
    uint32_t number_of_sets = number_of_cache_blocks / associativity;
    uint32_t no_index_bits = log2(number_of_sets);

    uint32_t g_num_cache_tag_bits = 32 - no_index_bits - g_cache_offset_bits;
    cache = (cache_block **)calloc(number_of_sets, sizeof(cache_block *));
    //malloc vs calloc

    //cache= malloc(number_of_cache_blocks *sizeof(cache_block));
    //initialising valid bits of each block to 0
    for (int i = 0; i < number_of_sets; i++)
    {
        cache[i] = calloc(associativity, sizeof(cache_block));
        //malloc vs calloc
    }
    // for (int i=0; i<number_of_cache_blocks;i++)
    // {
    //     cache[i]=cache_block;
    //     cache_block.valid_bit=0;

    // }
}

void fifo(cache_block **cache, uint32_t tag, uint32_t index)
{
    int hit = 0; //this is bool

    for (int i = 0; i < associativity; i++)
    {
        if ((cache[index][i].tag == tag) && (cache[index][i].valid_bit == 1))
        {
            hit = 1;
        }
    }

    if (hit == 1)
    {
        g_result.cache_hits++;
    }
    else
    {
        g_result.cache_misses++;
        cache_block new; //variable new
        new.tag = tag;
        new.valid_bit = 1;
        for (int i=associativity-1; i>=1;i--)
        {
            cache[index][i]=cache[index][i-1];
        }
        cache[index][0]=new;
    }
} 

void random(cache_block **cache, uint32_t tag, uint32_t index)
{
    int hit = 0; //this is bool

    for (int i = 0; i < associativity; i++)
    {
        if ((cache[index][i].tag == tag) && (cache[index][i].valid_bit == 1))
        {
            hit = 1;
        }
    }

    if (hit == 1)
    {
        g_result.cache_hits++;
    }
    else
    {
        g_result.cache_misses++;
        cache_block new; //variable new
        new.tag = tag;
        new.valid_bit = 1;
        //g_result.cache_misses++;
        uint32_t y=rand() % associativity;
        // cache_block rand;
        // cache[index][y]=rand;

        cache[index][y]=new;
    

    }

}


/*void lru(cache_block **cache, uint32_t tag, uint32_t index)
{
    int hit = 0; //this is bool

     for (int i = 0; i < associativity; i++)
     {
         cache[index][i].tracker=1;

     }
    for (int i = 0; i < associativity; i++)
    {
        if ((cache[index][i].tag == tag) && (cache[index][i].valid_bit == 1))
        {
            hit = 1;
            cache[index][i].tracker=0;
            for(int j=0;j<associativity ;j++)
            {
                if( j!=i)
                {
                    cache[index][j].tracker++;

                }
            }


            
        }
        else
        {
            cache[index][i].tracker++;


        }
    }
    
        

    if (hit == 1)
    {
        g_result.cache_hits++;

    }
    else
    {
        g_result.cache_misses++;
        // cache_block new; //variable new
        // new.tag = tag;
        // new.valid_bit = 1;
        
     
        // for (int i=1;i<=associativity;i++)
        // {
        //     cache[index][i]=cache[index][i-1];
        // }
        // cache[index][0]=new;
    }
} 
*/



////helper function
uint32_t srl(uint32_t x, uint32_t n)
{
    return (uint32_t)((uint32_t)x >> n);
}

int main(int argc, char **argv)
{
    time_t t;
    /* Intializes random number generator */
    /* Important: *DO NOT* call this function anywhere else. */
    srand((unsigned)time(&t));
    /* ----------------------------------------------------- */
    /* ----------------------------------------------------- */

    /*
     *
     * Read command-line parameters and initialize configuration variables.
     *
     */
    int improper_args = 0;
    char file[10000];
    if (argc < 6)
    {
        improper_args = 1;
        printf("Usage: ./mem_sim [replacement_policy: FIFO LRU Random] [associativity: 1 2 4 8 ...] [number_of_cache_blocks: 16 64 256 1024] [cache_block_size: 32 64] mem_trace.txt\n");
    }
    else
    {
        /* argv[0] is program name, parameters start with argv[1] */
        if (strcmp(argv[1], "FIFO") == 0)
        {
            replacement_policy = FIFO;
        }
        else if (strcmp(argv[1], "LRU") == 0)
        {
            replacement_policy = LRU;
        }
        else if (strcmp(argv[1], "Random") == 0)
        {
            replacement_policy = Random;
        }
        else
        {
            improper_args = 1;
            printf("Usage: ./mem_sim [replacement_policy: FIFO LRU Random] [associativity: 1 2 4 8 ...] [number_of_cache_blocks: 16 64 256 1024] [cache_block_size: 32 64] mem_trace.txt\n");
        }
        associativity = atoi(argv[2]);
        number_of_cache_blocks = atoi(argv[3]);
        cache_block_size = atoi(argv[4]);
        strcpy(file, argv[5]);
    }
    if (improper_args)
    {
        exit(-1);
    }
    assert(number_of_cache_blocks == 16 || number_of_cache_blocks == 64 || number_of_cache_blocks == 256 || number_of_cache_blocks == 1024);
    assert(cache_block_size == 32 || cache_block_size == 64);
    assert(number_of_cache_blocks >= associativity);
    assert(associativity >= 1);

    printf("input:trace_file: %s\n", file);
    printf("input:replacement_policy: %s\n", get_replacement_policy(replacement_policy));
    printf("input:associativity: %u\n", associativity);
    printf("input:number_of_cache_blocks: %u\n", number_of_cache_blocks);
    printf("input:cache_block_size: %u\n", cache_block_size);
    printf("\n");

    /* Open the file mem_trace.txt to read memory accesses */
    FILE *ptr_file;
    ptr_file = fopen(file, "r");
    if (!ptr_file)
    {
        printf("Unable to open the trace file: %s\n", file);
        exit(-1);
    }

    /* result structure is initialized for you. */
    memset(&g_result, 0, sizeof(result_t));

    /* Do not delete any of the lines below.
     * Use the following snippet and add your code to finish the task. */

    /* You may want to setup your Cache structure here. */

    mem_access_t access;
    /* Loop until the whole trace file has been read. */
    while (1)
    {
        access = read_transaction(ptr_file);
        // If no transactions left, break out of loop.
        if (access.address == 0)
            break;

        //cache_simulator(access);
        /* Add your code here */
        uint32_t tag = srl(access.address, no_index_bits + g_cache_offset_bits);
        uint32_t filter = pow(2, no_index_bits) - 1;
        uint32_t index = srl(access.address, g_cache_offset_bits) & filter;

        if (replacement_policy == FIFO)
        {

            fifo(cache, tag, index);
        }
        // else if (replacement_policy == LRU)
        // {
        //     lru(cache, tag, index);
        // }
        else
        {
            random(cache, tag, index);
        }
    }

    /* Do not modify code below. */
    /* Make sure that all the parameters are appropriately populated. */
    free(cache);
    print_statistics(g_num_cache_tag_bits, g_cache_offset_bits, &g_result);

    /* Close the trace file. */
    fclose(ptr_file);
    return 0;
}
