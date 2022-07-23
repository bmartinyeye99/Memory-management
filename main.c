#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#define FREE 1
#define USED 0

void* head = NULL;

//štruktúra hlavièky
typedef struct header
{
    char flag;
    unsigned int blocksize;
    struct header *next;
    struct header *prev;
} HEADER, *HEADERPTR;

#define HSIZE (sizeof(HEADER))

/**
funkica nastaví zaèiatoèné hodnoty
globálnu premennú smerujem na zaèiatok bloku memórie
**/
void memory_init(void* valid_ptr, unsigned int size)
{
    head = valid_ptr;

    HEADERPTR ptr = (HEADERPTR)valid_ptr;
    ptr->flag = FREE;
    ptr->blocksize = size-HSIZE;
    ptr->next = NULL;
    ptr->prev = NULL;
}


/**
funkcia slúži na allokovanie memórie,
pomocou metódy bestfit nájde blok s najlepšou velkosou
ktorý je ešte volny a dostatoène velky
Ked nájde blok, tak kontroluje ci sa dá blok rozdeli
na 2 èasti, podmienkou je že 1. èas bude ma požadovaný velkost
a 2. èast má minimálnu velkos, co sa rovná velkost hlavièky + velkost char
**/

void* memory_alloc(unsigned int size)
{
    if (size == 0)
        return NULL;
    long int bestsize = 999999;
    HEADERPTR temp = NULL;
    HEADERPTR best = NULL;
    temp = (HEADERPTR)head;
    int i;

    /**
    prejdem celý blok a hladám najmänši blok
    ktory je dostatoène velká pre zadanú velkost
    PHEADER best ukazuje na hlavicku najmänsieho bloku
    Keï blok na ktorý ukazuje pheader je mänsi ako best,
    tak presmerujem best na pheader
    **/
    while (temp != NULL )
    {
        if (temp->flag == FREE && temp->blocksize >= size)
        {
            if (temp->blocksize == size + HSIZE)
            {
                best = temp;
                printf("THERE IS A PERFECT FIT\n");
                break;
            }
            else if (temp->blocksize < bestsize)
            {
                best = temp;
                bestsize = best->blocksize;
            }
        }
        temp = temp->next;
    }

    if (best == NULL)
        return NULL;



    /**z velkosti najdeneho bloku odcitam hladany velkost, vysledok bude velkostou noveho bloku,
    ked sa da rozdelit
    blok = 50 // hlavicka = 16 // požadovana velkost = 5
    1.blok = 50 - 16 = 34
    left = 34 - 5 = 29
    **/

    int left = best->blocksize - size;

    /**kontrolujem èi sa dá rozdelit nájdený blok, ak nie tak nastavim najdeny blok ako USED(alokovany)*/
    if (left < (int)(HSIZE + sizeof(char)) || best->blocksize - size < size)
    {
        /**blok nastavim ako allokovany a vratim pointer na, ktory ukazuje na zacitok memorie*/
        best->flag = USED;
        return ((char*)best + HSIZE);
    }

    /**
    po rozdelení, pozíciu nového bloku zistím
    tak, že pripoèítam velkos hlavièky a hladanú ve¾kos k ukazovateli
    ktorý ukazuje na 1. byte hlavièky najdeneho bloku
    **/
    else
    {
        HEADERPTR newheader = (HEADERPTR)((char*)best + HSIZE + size);
        newheader->flag = FREE;
        newheader->prev = best;
        newheader->next = best->next;

        best->next = newheader;
        best->flag = USED;
        // z velkosti noveho bloku este treba odratat velkost hlavièky, teda 29-16
        newheader->blocksize = left - HSIZE;
        best->blocksize = size  ;
        return ((char*)best + HSIZE);
    }
}



int memory_check(void* ptr)
{
    if (head == NULL)
        return 0;

    char* tempptr = (char*)head;
    HEADERPTR tmpheader = (HEADERPTR)tempptr;
    while(tempptr != NULL)
    {
        if(tempptr + HSIZE== ptr)
            return 1;
        tempptr = tmpheader->next;
        tmpheader = (HEADERPTR)tempptr;
    }

    return 0;
}



void cosolidate(HEADERPTR block)
{
    /**kontrolujem, èi susedy blok na pravej strane je volny*/
    HEADERPTR right = block->next;
    HEADERPTR left = block->prev;
    if (right != NULL && right->flag == FREE)
    {
        /** treba kosolidovat, blok spojim so susednym blokom na pravej strane
         a blok spojim s blokom na ktory ukazuje ted blok ktorym som spojil
         */
        block->next = right->next;
        block->blocksize += HSIZE + right->blocksize;
    }

    /**to iste len s lavou stranou*/
    if (left != NULL && left->flag == FREE)
    {
        if (block->next != NULL)
            block->next->prev = left;

        left->blocksize += block->blocksize + HSIZE;
        left->next = block->next;

        /** ked aj lava aj prava strana je volna, tak pravy blok a blok ktory chceme uvolnit spajat dokopy s lavym blokom*/

    }
    return ;

}



int memory_free(void* valid_ptr)
{

    char* ptr;
    HEADERPTR block;

    if (memory_check(valid_ptr) == 0)
        return 1;

    /** ptr ukazuje na zaèiatok payload, nie na hlavièku, preto treba odèíta velkost hlavièky*/
    ptr = (char*)valid_ptr - HSIZE;
    block = (HEADERPTR)ptr;
    block->flag = FREE;
    cosolidate(block);
    return 0;

}



int position(char *end)
{
    return (end - (char*)head);
}

void prinindexes(void *payload, int i, int *space_left, int bytes)
{
    if (payload == NULL)
    {
        printf("UZ SA NEDALO ALOKOVAT. Volne miesto : %d  A potrebujem :%d\n",*space_left, bytes + HSIZE);
        return;
    }
    printf(" pointer %d \n",i);
    HEADERPTR blok = (HEADERPTR)((char*)payload - HSIZE);
    *space_left -= blok->blocksize + HSIZE;
    printf("STATE: %X\n",blok->flag);
    printf("BLOCKSIZE WITHOUT HEADER = %d\n",blok->blocksize);
    printf("BLOCKSIZE WITH HEADER = %d\n",blok->blocksize + HSIZE);
    printf("Position (POS OF HEADERS 1.BYTE) = %d\n", position((char*)blok));
    printf("NEXT (POS OF NEXT BLOCK's HEADER'S 1. BYTE) = %d\n", position(blok->next));
    printf("PREV (POS OF PREV BLOCK's HEADER'S 1. BYTE) = %d\n\n", position(blok->prev));

}

int main()
{

    /** SCENAR 1 */
    /** 8 BYTE in 50 */

    /*
        char region[50];
        memory_init(region,50);
        int space_left = 50 - HSIZE;
        char *p = (char*)memory_alloc(8);
            prinindexes(p,0,&space_left,8);
        char *p1 = (char*)memory_alloc(8);
        prinindexes(p1,1,&space_left,8);
            char *p2 = (char*)memory_alloc(8);
        prinindexes(p2,2,&space_left,8);
        */



    /** 8 BYTE in 100 */

    /*
            char region[100];
            memory_init(region,100);
            int space_left = 100 - HSIZE;
            char *p = (char*)memory_alloc(8);
                prinindexes(p,0,&space_left,8);
            char *p1 = (char*)memory_alloc(8);
            prinindexes(p1,1,&space_left,8);
            char *p2 = (char*)memory_alloc(8);
            prinindexes(p2,2,&space_left,8);
            char *p3 = (char*)memory_alloc(8);
            prinindexes(p3,3,&space_left,8);
            char *p4 = (char*)memory_alloc(8);
            prinindexes(p4,4,&space_left,8);

    */


    /** 8 BYTE in 200 */

    /*
            char region[200];
            memory_init(region,200);
            int space_left = 200 - HSIZE;
            char *p = (char*)memory_alloc(8);
                prinindexes(p,0,&space_left,8);
            char *p1 = (char*)memory_alloc(8);
            prinindexes(p1,1,&space_left,8);
            char *p2 = (char*)memory_alloc(8);
            prinindexes(p2,2,&space_left,8);
            char *p3 = (char*)memory_alloc(8);
            prinindexes(p3,3,&space_left,8);
            char *p4 = (char*)memory_alloc(8);
            prinindexes(p4,4,&space_left,8);
            char *p5 = (char*)memory_alloc(8);
            prinindexes(p5,5,&space_left,8);
            char *p6 = (char*)memory_alloc(8);
            prinindexes(p6,6,&space_left,8);
            char *p7 = (char*)memory_alloc(8);
            prinindexes(p7,7,&space_left,8);
            char *p8 = (char*)memory_alloc(8);
            prinindexes(p8,8,&space_left,8);
        */


    /** ******* */



    /** 15 BYTE 50 Bytes */

    /*
             char region[50];
             memory_init(region,50);
            int space_left = 50 - HSIZE;
             char *p = (char*)memory_alloc(15);
                prinindexes(p,0,&space_left,15);
            char *p1 = (char*)memory_alloc(15);
                prinindexes(p1,1,&space_left,15);
        */



    /** 15 in 100 Bytes*/
    /*
            char region[100];
            memory_init(region,100);
            int space_left = 100 - HSIZE;
            char *p = (char*)memory_alloc(15);
                prinindexes(p,0,&space_left,15);
            char *p1 = (char*)memory_alloc(15);
                prinindexes(p1,1,&space_left,15);
                 char *p2 = (char*)memory_alloc(15);
                prinindexes(p2,2,&space_left,15);
            char *p3 = (char*)memory_alloc(15);
                prinindexes(p3,3,&space_left,15);

    */


    /** 15 in 200 Bytes*/
    /*
            char region[200];
            memory_init(region,200);
            int space_left = 200 - HSIZE;
            char *p = (char*)memory_alloc(15);
            prinindexes(p,0,&space_left,15);
            char *p1 = (char*)memory_alloc(15);
            prinindexes(p1,1,&space_left,15);
            char *p2 = (char*)memory_alloc(15);
            prinindexes(p2,2,&space_left,15);
            char *p3 = (char*)memory_alloc(15);
            prinindexes(p3,3,&space_left,15);
            char *p4 = (char*)memory_alloc(15);
            prinindexes(p4,4,&space_left,15);
            char *p5 = (char*)memory_alloc(15);
            prinindexes(p5,5,&space_left,15);
            char *p6 = (char*)memory_alloc(15);
            prinindexes(p6,6,&space_left,15);
    */
    /** ******* */




    /** 24 in 50 Bytes */
    /*
     char region[50];
     memory_init(region,50);
    int space_left = 50 - HSIZE;
     char *p = (char*)memory_alloc(24);
         prinindexes(p,0,&space_left,24);
         char *p1 = (char*)memory_alloc(24);
         prinindexes(p1,1,&space_left,24);
    */



    /** 24 in 100 Bytes */
    /*
            char region[100];
            memory_init(region,100);
            int space_left = 100 - HSIZE;
           char *p = (char*)memory_alloc(24);
                 prinindexes(p,0,&space_left,24);
                 char *p1 = (char*)memory_alloc(24);
                 prinindexes(p1,1,&space_left,24);
                 char *p2 = (char*)memory_alloc(24);
                 prinindexes(p2,2,&space_left,24);
        */



    /** 24 in 200 Bytes */
    /*
            char region[200];
            memory_init(region,200);
            int space_left = 200 - HSIZE;
               char *p = (char*)memory_alloc(24);
                 prinindexes(p,0,&space_left,24);
                 char *p1 = (char*)memory_alloc(24);
                 prinindexes(p1,1,&space_left,24);
                 char *p2 = (char*)memory_alloc(24);
                 prinindexes(p2,2,&space_left,24);
                  char *p3 = (char*)memory_alloc(24);
                 prinindexes(p3,3,&space_left,24);
                 char *p4 = (char*)memory_alloc(24);
                 prinindexes(p4,4,&space_left,24);
                 char *p5 = (char*)memory_alloc(24);
                 prinindexes(p5,5,&space_left,24);
        */
    /** ******* */


    /** SCENAR 2 */

    /** random 50 Bytes */
    /*

     srand(time(0));
         char region[50];
         memory_init(region,50);
        int space_left = 50 - HSIZE;
         while (1){
            int num = (rand() % (24 - 8 + 1)) + 8;
            char *p1 = memory_alloc(num);
            prinindexes(p1,1,&space_left,num);
            if (p1 == NULL)
                break;
         }
    */



    /**  random 100 bytes */
    /*
    srand(time(0));
     char region[100];
     memory_init(region,100);
    int space_left = 100 - HSIZE;
     int i = 0;
     while (1){
        int num = (rand() % (24 - 8 + 1)) + 8;
        char *p1 = memory_alloc(num);
        prinindexes(p1,i,&space_left,num);
        if (p1 == NULL)
            break;
        i++;
     }
    */


    /**  random 200 bytes */
    /*
         srand(time(0));
         char region[200];
         memory_init(region,200);
        int space_left = 200 - HSIZE;
         int i = 0;
         while (1){
            int num = (rand() % (24 - 8 + 1)) + 8;
            char *p1 = memory_alloc(num);
            prinindexes(p1,i,&space_left,num);
            if (p1 == NULL)
                break;
            i++;
         }
    */

    /** SCENAR 3 */
    /*
        srand(time(0));
        char region[10000];
        memory_init(region,10000);
        int space_left = 10000 - HSIZE;
        int i = 0;
        while (i < 5)
        {
            int num = (rand() % (5000 - 500 + 1)) + 500;
            printf(" RANDOM NUMBER > %d\n",num);
            char *p1 = memory_alloc(num);
            prinindexes(p1,i,&space_left,num);
            if (p1 == NULL)
                break;
            i++;
        }
    */

    /** SCENAR 4 */

    /*
             srand(time(0));
             char region[100000];
             memory_init(region,100000);
             int space_left = 100000 - HSIZE;
             int i = 0;
             while (i < 5){
                int num = (rand() % (50000 - 80 + 1)) + 80;
                printf(" RANDOM NUMBER > %d\n",num);
                char *p1 = memory_alloc(num);
                prinindexes(p1,i,&space_left-HSIZE,num);
                if (p1 == NULL)
                    break;
                i++;
             }
    */

    /** vlastny test */

    char region[100];
    printf("struct size: %d\n\n ",HSIZE);
    memory_init(region, 100);
    unsigned space_left = 100 - HSIZE;

    char* p0 = (char*) memory_alloc(5);
    if (p0 != NULL)
    {
        memset(p0, 0, 5);
    };

    prinindexes(p0,0,&space_left,5);

    char* p1 = (char*) memory_alloc(3);
    if (p1 != NULL )
    {
        memset(p1, 0, 3);
    };

    prinindexes(p1,1,&space_left,3);


    char* p2 = (char*) memory_alloc(6);
    if (p2 != NULL )
    {
        memset(p2, 0, 6);
    };

    prinindexes(p2,2,&space_left,6);


    if (p0 != NULL )
    {
        memory_free(p0);

    };
    prinindexes(p0,0,&space_left,6);


    char* p3 = (char*) memory_alloc(10);
    if (p3 != NULL )
    {
        memset(p3, 0, 10);
    };

    prinindexes(p3,3,&space_left,10);
    printf("Tento blok sa uz neda rozdelit, lebo v bloku\n"
           "mame len 22 volnych byteov, z tychto sa uz neda vytvorit\n"
           " hlavicku a 1 Byte. 22 -16 = 6 < 16 + 1\n\n\n");

    if (p2 != NULL )
    {
        memory_free(p2);
    };
    prinindexes(p2,2,&space_left,6);


    /*
        char* p4 = (char*) memory_alloc(5);
        if (p4 != NULL )
        {
            memset(p4, 0, 5);
        };
        prinindexes(p4,4,&space_left,100);
        prinindexes(p0,0,&space_left,100);

    */

    if (p1 != NULL)
    {
        memory_free(p1);
    };
    prinindexes(p1,1,&space_left,3);
    printf("*** Stala sa kosolidacia P1 + P2 3+16+6 = 25(velkost bloku p1 + HSIZE + velkost p2) *** \n\n ");
    prinindexes(p0,0,&space_left,5);
    printf("*** Stala sa kosolidacia P0 +(P1 + P2) = 5 + 16 +25 = 46(velkost bloku p0 + HSIZE + velkost p1) *** \n\n ");


    if (p3 != NULL )
    {
        memory_free(p3);
    };

    return 0;
}
