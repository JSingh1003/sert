#include "gestore.h"

//init.
void init_gestore(struct gestore_t *g){
    sem_init(&g->mutex, 0, 1);

    sem_init(&g->sem_a_o_c, 0, 0);
    sem_init(&g->sem_b, 0, 0);
    sem_init(&g->sem_d_o_e, 0, 0);

    g->nba = g->nbb = g->nbc = g->nbd = g->nbe = 0;
    g->nb = 0;
    g->stato = SEQ_NESS;
}
