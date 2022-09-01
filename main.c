#include "gestore.h"

void sveglia_A_o_C(struct gestore_t *g){
    if(g->nb == 0){
        if(g->nba){
            g->nba--;
            g->stato = SEQ_A;
            sem_post(&g->sem_a_o_c);
        }
        else if(g->nbc){
            g->nbc--;
            g->stato = SEQ_C;
            sem_post(&g->sem_a_o_c);
        }
        else{
            g->stato = SEQ_NESS;
        }
    }
}

void StartA(struct gestore_t *g)
{
    sem_wait(&g->mutex);
    if(g->stato == SEQ_NESS){
        g->stato = SEQ_A;
        sem_post(&g->sem_a_o_c);
    }
    else{
        g->nba++;
    }
    sem_post(&g->mutex);
    sem_wait(&g->sem_a_o_c);
}

void EndA(struct gestore_t *g)
{
    sem_wait(&g->mutex);
    g->stato = SEQ_B;
    while(g->nbb){
        g->nbb--;
        g->nb++;
        sem_post(&g->sem_b);
    }
    sem_post(&g->mutex);
}

void StartB(struct gestore_t *g)
{
    sem_wait(&g->mutex);
    if(g->stato == SEQ_B){
        g->nb++;
        sem_post(&g->sem_b);
    }
    else{
        g->nbb++;
    }
    sem_post(&g->mutex);
    sem_wait(&g->sem_b);
}

void EndB(struct gestore_t *g)
{
    sem_wait(&g->mutex);
    g->nb--;
    sveglia_A_o_C(g);
    sem_post(&g->mutex);
}

void StartC(struct gestore_t *g)
{
    sem_wait(&g->mutex);
    if(g->stato == SEQ_NESS){
        g->stato = SEQ_C;
        sem_post(&g->sem_a_o_c);
    }
    else{
        g->nbc++;
    }
    sem_post(&g->mutex);
    sem_wait(&g->sem_a_o_c);
}

void EndC(struct gestore_t *g)
{
    sem_wait(&g->mutex);
    if(g->nbd){
        g->nbd--;
        g->stato = SEQ_D;
        sem_post(&g->sem_d_o_e);
    }
    else if(g->nbe){
        g->nbe--;
        g->stato = SEQ_E;
        sem_post(&g->sem_d_o_e);
    }
    else{
        g->stato = SEQ_D_o_E;
    }
    sem_post(&g->mutex);
}

void StartD(struct gestore_t *g)
{
    sem_wait(&g->mutex);
    if(g->stato == SEQ_D_o_E){
        g->stato = SEQ_D;
        sem_post(&g->sem_d_o_e);
    }
    else{
        g->nbd++;
    }
    sem_post(&g->mutex);
    sem_wait(&g->sem_d_o_e);
}

void EndD(struct gestore_t *g)
{
    sem_wait(&g->mutex);
    sveglia_A_o_C(g);
    sem_post(&g->mutex);
}

void StartE(struct gestore_t *g)
{
    sem_wait(&g->mutex);
    if(g->stato == SEQ_D_o_E){
        g->stato = SEQ_E;
        sem_post(&g->sem_d_o_e);
    }
    else{
        g->nbe++;
    }
    sem_post(&g->mutex);
    sem_wait(&g->sem_d_o_e);
}

void EndE(struct gestore_t *g)
{
    sem_wait(&g->mutex);
    sveglia_A_o_C(g);
    sem_post(&g->mutex);
}


void pausetta(void)
{
    struct timespec t;
    t.tv_sec = 0;
    t.tv_nsec = (rand()%10+1)*100000;
    nanosleep(&t,NULL);
}

struct gestore_t gestore;

void *A (void *arg)
{
    for(;;){
        StartA(&gestore);
        putchar(*(char *)arg);
        EndA(&gestore);
        pausetta();
    }
    return 0;
}

void *B (void *arg)
{
    for(;;){
        StartB(&gestore);
        putchar(*(char *)arg);
        EndB(&gestore);
        pausetta();
    }
    return 0;
}

void *C (void *arg)
{
    for(;;){
        StartC(&gestore);
        putchar(*(char *)arg);
        EndC(&gestore);
        pausetta();
    }
    return 0;
}

void *D (void *arg)
{
    for(;;){
        StartD(&gestore);
        putchar(*(char *)arg);
        EndD(&gestore);
        pausetta();
    }
    return 0;
}

void *E (void *arg)
{
    for(;;){
        StartE(&gestore);
        putchar(*(char *)arg);
        EndE(&gestore);
        pausetta();
    }
    return 0;
}

int main ()
{
    pthread_t thread;
    pthread_attr_t attr;

    init_gestore(&gestore);
    srand(555);

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    sem_wait(&gestore.mutex);
    printf("\n\nA: %d,B: %d,C: %d,D: %d,E: %d\n\n",gestore.nba,gestore.nbb,gestore.nbc,gestore.nbd,gestore.nbe);
    sem_post(&gestore.mutex);
    
    pthread_create(&thread, &attr, A, (void *)"a");
    pthread_create(&thread, &attr, A, (void *)"A");

    pthread_create(&thread, &attr, B, (void *)"B");
    pthread_create(&thread, &attr, B, (void *)"b");

    pthread_create(&thread, &attr, C, (void *)"C");
    pthread_create(&thread, &attr, C, (void *)"c");

    pthread_create(&thread, &attr, D, (void *)"D");
    pthread_create(&thread, &attr, D, (void *)"d");

    pthread_create(&thread, &attr, E, (void *)"E");
    pthread_create(&thread, &attr, E, (void *)"e");

    pthread_attr_destroy(&attr);

    for(int i = 0;i < 50; i++){
        sleep(1);
        sem_wait(&gestore.mutex);
        printf("\n\n Iterazione %d) A: %d,B: %d,C: %d,D: %d,E: %d\n\n",i, gestore.nba,gestore.nbb,gestore.nbc,gestore.nbd,gestore.nbe);
        if(gestore.nba == gestore.nbb && gestore.nbb == gestore.nbc 
        && gestore.nbc == gestore.nbd && gestore.nbd == gestore.nbe && gestore.nbe == 0) {
            sem_post(&gestore.mutex);
            break;
        }
        sem_post(&gestore.mutex);
    }
    return 0;
}
