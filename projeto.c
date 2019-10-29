#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>

int pratos = 0;

sem_t fogao, balcao, forno;

void *cozinha(void *args){
    int id = *((int *) args);
    srand(time(0));
    int cardapio;

    while(pratos > 0){
        cardapio = rand() % 2;
        switch(cardapio){
            case 0:
                printf("O chef %d preparará uma lasanha.\n", id);
                sleep(1);
                break;
            case 1:
                printf("O chef %d preparará um cannoli.\n", id);
                sleep(1);
                break;
        }
        pratos--;
    }
}

int main(){
    int n, *id;
    printf("Quantos chefs serão contratados? ");
    scanf("%d", &n);
    pthread_t chef[n];
    sem_init(&fogao, 0, 8);
    sem_init(&balcao,0,6);
    sem_init(&forno,0,2);

    printf("Quantos pratos serão feitos nesse serviço? ");
    scanf("%d", &pratos);

    for(int i = 0; i < n; i++){
        id = (int *) malloc(sizeof(int));
        *id = i;
        pthread_create(&chef[i], NULL, cozinha, (void *)id);
    }

    for(int i = 0; i < n; i++){
        pthread_join(chef[i], NULL);
    }

    sem_destroy(&fogao);
    sem_destroy(&balcao);
    sem_destroy(&forno);

    return 0;
}