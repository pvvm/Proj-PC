#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>

int pratos = 0, est_forno;
int num_pratos[5] = {0,0,0,0,0};
int* sorteio = 0;

sem_t sem_fogao, sem_balcao, sem_forno;
pthread_mutex_t mutex, mutex_forno;

void balcao(int id){
    sem_wait(&sem_balcao);
    printf("Chef %d sovando a massa.\n", id);
    sleep(2);
    sem_post(&sem_balcao);
}

void fogao(int id){
    sem_wait(&sem_fogao);
    printf("Chef %d cozinhando a massa.\n", id);
    sleep(3);
    sem_post(&sem_fogao);
}

void forno(int id, int salgadoce){
    if(salgadoce == 1 && est_forno == 2)
        pthread_mutex_lock(&mutex_forno);
    else if(salgadoce == 2 && est_forno == 1)
        pthread_mutex_lock(&mutex_forno);

    sem_wait(&sem_forno);
    est_forno = salgadoce;

    sem_post(&sem_fogao);
}

void *cozinha(void *args){
    int id = *((int *) args);
    int cardapio;

    while(pratos > 0){
        pthread_mutex_lock(&mutex);
        if(pratos == 0)
            break;
        pratos--;
        pthread_mutex_unlock(&mutex);
        switch(sorteio[pratos]){
            case 0:
                printf("O chef %d preparará uma lasanha.\n", id);
                balcao(id);
                fogao(id);
                //forno(id, 1);
                printf("Pedido de lasanha saindo!\n");
                num_pratos[0]++;
                break;
            case 1:
                printf("O chef %d preparará um cannoli.\n", id);
                fogao(id);
                //forno(id, 2);
                printf("Pedido de cannoli saindo!\n");
                num_pratos[1]++;
                break;
        }
    }
}

int main(){
    int n, *id;
    printf("Quantos chefs serão contratados? ");
    scanf("%d", &n);
    pthread_t chef[n];

    pthread_mutex_init(&mutex,0);
    pthread_mutex_init(&mutex_forno,0);
    sem_init(&sem_fogao, 0, 8);
    sem_init(&sem_balcao,0,6);
    sem_init(&sem_forno,0,2);

    printf("Quantos pratos serão feitos nesse serviço? ");
    scanf("%d", &pratos);

    sorteio = (int*) malloc(pratos * sizeof(int));

    srand(time(0));
    for(int i = 0; i < pratos; i++){
        sorteio[i] = rand() % 2;
    }

    for(int i = 0; i < n; i++){
        id = (int *) malloc(sizeof(int));
        *id = i;
        pthread_create(&chef[i], NULL, cozinha, (void *)id);
    }

    for(int i = 0; i < n; i++){
        pthread_join(chef[i], NULL);
    }

    printf("\nAo todo, foram servidos:\n");
    printf("%d Lasanhas\n%d Espaguetes\n%d Risotos\n%d Cannolis\n%d Zeppoles\n", num_pratos[0], num_pratos[1], num_pratos[2], num_pratos[3], num_pratos[4]);

    sem_destroy(&sem_fogao);
    sem_destroy(&sem_balcao);
    sem_destroy(&sem_forno);
    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&mutex_forno);

    free(sorteio);

    return 0;
}