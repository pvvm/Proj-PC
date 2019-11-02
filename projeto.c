// autor: Pedro Vitor Valença Mizuno
// arquivo: projeto.c
// atividade extraclasse

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>

int pratos = 0, est_forno = 0, tamanho, cont = 0;
int num_pratos[5] = {0,0,0,0,0};
int* sorteio = 0;
struct fila f;

sem_t sem_fogao, sem_balcao, sem_forno;
pthread_mutex_t mutex, mutex_forno, dentro_forno;

struct fila{        // Struct que compõe a fila
    int id_chef;
    int saloudoce;
    int inicio, fim;
};

void inserir(struct fila *f, int id, int salgadoce){        // Inclui na fila
    if((f->fim + 1) < tamanho){
        f->fim++;
        f->saloudoce = salgadoce;
        f->id_chef = id;
    }   
}

struct fila consumir(struct fila *f){       // Tira da fila
    struct fila aux;
    if(f->inicio < tamanho){
        aux.id_chef = f->id_chef;
        aux.saloudoce = f->saloudoce;
        f->inicio++;
    }
    return aux;
}

void balcao(int id){        // Função do balcão
    sem_wait(&sem_balcao);
    printf("\t\t\t\t |  Chef %d sovando a massa.           |\n", id);
    sleep(3);
    sem_post(&sem_balcao);
}

void fogao(int id){         // Função do fogão
    sem_wait(&sem_fogao);
    printf("\t\t\t\t |  Chef %d cozinhando o arroz/massa.  |\n", id);
    sleep(2);
    sem_post(&sem_fogao);
}

void forno(int id, int salgadoce){              // Função do forno
    pthread_mutex_lock(&mutex_forno);
    inserir(&f, id, salgadoce);                 // Empilha na fila
    pthread_mutex_unlock(&mutex_forno);

    struct fila proximo;

    sem_wait(&sem_forno);
    proximo = consumir(&f);                     // Desempilha
    if(((proximo.saloudoce == 1 && est_forno == 2) || (proximo.saloudoce == 2 && est_forno == 1)) && cont == 1){   // Se o próximo for diferente do tipo do atual no forno
        printf("\t\t\t\t |  Chef %d esperando acesso ao forno. |\n", id);
        pthread_mutex_lock(&dentro_forno);
        printf("\t\t\t\t |  Forno liberado para uso.          |\n");
    }
    cont = 1;

    est_forno = salgadoce;      // Define o tipo do prato atual no forno (salgado ou doce)

    printf("\t\t\t\t |  Chef %d assando a massa.           |\n", id);
    sleep(4);
    cont = 0;
    sem_post(&sem_forno);

    pthread_mutex_unlock(&dentro_forno);
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
        switch(sorteio[pratos]){            // Cada caso é um prato diferente
            case 0:
                printf("O chef %d preparará uma lasanha.  |\t\t\t\t      |\n", id);
                balcao(id);
                fogao(id);
                forno(id, 1);
                printf("\t\t\t\t | \t\t\t\t      | Pedido de lasanha do chef %d saindo!\n", id);
                num_pratos[0]++;
                break;

            case 1:
                printf("O chef %d preparará um espaguete. |\t\t\t\t      |\n", id);
                balcao(id);
                fogao(id);
                printf("\t\t\t\t | \t\t\t\t      | Pedido de espaguete do chef %d saindo!\n", id);
                num_pratos[1]++;
                break;

            case 2:
                printf("O chef %d preparará um risoto.    |\t\t\t\t      |\n", id);
                fogao(id);
                printf("\t\t\t\t | \t\t\t\t      | Pedido de risoto do chef %d saindo!\n", id);
                num_pratos[2]++;
                break;

            case 3:
                printf("O chef %d preparará um cannoli.   |\t\t\t\t      |\n", id);
                balcao(id);
                fogao(id);
                printf("\t\t\t\t | \t\t\t\t      | Pedido de cannoli do chef %d saindo!\n", id);
                num_pratos[3]++;
                break;

            case 4:
                printf("O chef %d preparará um zeppole.   |\t\t\t\t      |\n", id);
                balcao(id);
                fogao(id);
                forno(id, 2);
                printf("\t\t\t\t | \t\t\t\t      | Pedido de zeppole do chef %d saindo!\n", id);
                num_pratos[4]++;
                break;
        }
    }
}

int main(){
    printf(" ____  ______  ____  ____  ____  ____  ____  ____  ____    ___\n");
    printf("|  __||  __  ||_  _||_  _|| ___|| ___|| ___||  __||_  _|  / _ \\ \n");
    printf("|  \\  | |__| |  ||   _||_ |___ ||___ || _|_ |  \\   _||_  / |_| \\ \n");
    printf("|_\\_\\ |______|  ||  |____||____||____||____||_\\_\\ |____|/__| |__\\ \n\n");

    printf(" ____  ______  _  _  ____  ______  ____  ____  ____  _  _  ____  ____\n");
    printf("|  __||  __  || \\| ||  __||  __  ||  __||  __|| ___|| \\| ||_  _|| ___|\n");
    printf("| |__ | |__| || \\  || |__ | |__| ||  \\  |  \\  | _|_ | \\  |  ||  | _|_\n");
    printf("|____||______||_|\\_||____||______||_\\_\\ |_\\_\\ |____||_|\\_|  ||  |____|\n\n\n");

    printf("Bem-vindo à Rotisseria Concorrente!\nHoje você estará responsável pela contratação dos chefs e dizer quantos clientes serão servidos.\n");

    int n, *id;
    printf("Quantos chefs serão contratados? ");
    scanf("%d", &n);
    pthread_t chef[n];

    pthread_mutex_init(&mutex,0);           // Mutex para o controle de pratos já feitos 
    pthread_mutex_init(&mutex_forno,0);     // Mutex para o incremento da fila do forno
    pthread_mutex_init(&dentro_forno,0);    // Mutex para o controle dos pratos dentro do forno
    sem_init(&sem_fogao, 0, 8);             // Semáforo para o fogão, com máximo de 8
    sem_init(&sem_balcao,0,6);              // Semáforo para o balcão, com máximo de 6
    sem_init(&sem_forno,0,2);               // Semáforo para o forno, com máximo de 2

    printf("Quantos pratos serão feitos nesse serviço? ");
    scanf("%d", &pratos);
    tamanho = pratos;                       // Variável para o tamanho máximo da fila

    printf("\n____________________________________________________________________________________________________________\n");
    printf("                                 |                                    |                                     \n");

    sorteio = (int*) malloc(pratos * sizeof(int));

    srand(time(0));
    for(int i = 0; i < pratos; i++){
        sorteio[i] = rand() % 5;            // Sorteia os pratos que serão pedidos
    }

    for(int i = 0; i < n; i++){
        id = (int *) malloc(sizeof(int));
        *id = i;
        pthread_create(&chef[i], NULL, cozinha, (void *)id);        // Cria as threads chefs
        free(id);
    }

    for(int i = 0; i < n; i++){
        pthread_join(chef[i], NULL);
    }
    printf("_________________________________|____________________________________|_____________________________________\n\n");

    printf("\nAo todo, foram servidos:\n");
    printf("%d Lasanhas\n%d Espaguetes\n%d Risotos\n%d Cannolis\n%d Zeppoles\n", num_pratos[0], num_pratos[1], num_pratos[2], num_pratos[3], num_pratos[4]);

    sem_destroy(&sem_fogao);
    sem_destroy(&sem_balcao);
    sem_destroy(&sem_forno);
    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&mutex_forno);
    pthread_mutex_destroy(&dentro_forno);

    free(sorteio);

    return 0;
}