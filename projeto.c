#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

int pratos = 0;

void *cozinha(void *args){
    
}

int main(){
    int n, *id;
    printf("Quantos chefes serão contratados? ");
    scanf("%d", &n);
    pthread_t chef[n];

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

    return 0;
}