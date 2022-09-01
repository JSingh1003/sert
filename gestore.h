#ifndef GESTORE
#define GESTORE

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>

#define SEQ_NESS    0
#define SEQ_A       1
#define SEQ_B       2
#define SEQ_C       3
#define SEQ_D       4
#define SEQ_E       5
#define SEQ_D_o_E   6


struct gestore_t{
    sem_t mutex;

    sem_t sem_a_o_c,sem_b,sem_d_o_e;
    int nba, nbb, nbc, nbd, nbe;

    int nb;
    int stato;
};

//init.
void init_gestore(struct gestore_t *g);

#endif
