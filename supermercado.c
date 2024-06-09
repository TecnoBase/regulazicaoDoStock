#include <stdio.h>
#include <string.h>
#include <pthread.h>

#define NUM_SUPERMERCADOS 6
#define NUM_FRUTAS 5
#define MAX_NOME 30
#define MAX_ESTADO 10
#define MAX_CONSERVACAO 10
#define MAX_ESTACAO 10

typedef struct {
    char nome[MAX_NOME];
    int estoque;
    char estado[MAX_ESTADO]; 
    char conservacao[MAX_CONSERVACAO];
    char estacao[MAX_ESTACAO]; 
} Fruta;

typedef struct {
    char nome[MAX_NOME];
    Fruta frutas[NUM_FRUTAS];
    int num_frutas;
} Supermercado;

typedef struct {
    Supermercado* supermercado;
    int quantidades[NUM_FRUTAS];
} EncomendaArgs;

// Array para rastrear supermercados banidos
int supermercadosBanidos[NUM_SUPERMERCADOS] = {0}; 

void inicializaSupermercados(Supermercado supermercados[NUM_SUPERMERCADOS]) {
    for (int i = 0; i < NUM_SUPERMERCADOS; i++) {
        sprintf(supermercados[i].nome, "Supermercado %d", i + 1);
        supermercados[i].num_frutas = 0; 
    }
}

void exibeEstoque(Supermercado supermercado) {
    printf("Estoque do %s:\n", supermercado.nome);
    for (int j = 0; j < supermercado.num_frutas; j++) {
        printf("%s: %d (%s, %s, %s)\n", supermercado.frutas[j].nome, supermercado.frutas[j].estoque, supermercado.frutas[j].estado, supermercado.frutas[j].conservacao, supermercado.frutas[j].estacao);
    }
    printf("\n");
}

void exibeEstoqueVerde(Supermercado supermercado) {
    printf("Estoque de frutas verdes do %s:\n", supermercado.nome);
    for (int j = 0; j < supermercado.num_frutas; j++) {
        if (strcmp(supermercado.frutas[j].estado, "verde") == 0) {
            printf("%s: %d (%s, %s, %s)\n", supermercado.frutas[j].nome, supermercado.frutas[j].estoque, supermercado.frutas[j].estado, supermercado.frutas[j].conservacao, supermercado.frutas[j].estacao);
        }
    }
    printf("\n");
}

void exibeEstoqueMaduro(Supermercado supermercado) {
    printf("Estoque de frutas maduras do %s:\n", supermercado.nome);
    for (int j = 0; j < supermercado.num_frutas; j++) {
        if (strcmp(supermercado.frutas[j].estado, "madura") == 0) {
            printf("%s: %d (%s, %s, %s)\n", supermercado.frutas[j].nome, supermercado.frutas[j].estoque, supermercado.frutas[j].estado, supermercado.frutas[j].conservacao, supermercado.frutas[j].estacao);
        }
    }
    printf("\n");
}

void exibeEstoquePorEstacao(Supermercado supermercado, const char* estacao) {
    printf("Estoque do %s para a estação %s:\n", supermercado.nome, estacao);
    for (int j = 0; j < supermercado.num_frutas; j++) {
        if (strcmp(supermercado.frutas[j].estacao, estacao) == 0) {
            printf("%s: %d (%s, %s, %s)\n", supermercado.frutas[j].nome, supermercado.frutas[j].estoque, supermercado.frutas[j].estado, supermercado.frutas[j].conservacao, supermercado.frutas[j].estacao);
        }
    }
    printf("\n");
}

void insereProduto(Supermercado supermercados[NUM_SUPERMERCADOS]) {
    int supermercado;
    printf("Digite o número do supermercado para adicionar a fruta (1-%d): ", NUM_SUPERMERCADOS);
    scanf("%d", &supermercado);
    supermercado--;

    if (supermercado < 0 || supermercado >= NUM_SUPERMERCADOS) {
        printf("Supermercado inválido!\n");
        return;
    }

    if (supermercados[supermercado].num_frutas >= NUM_FRUTAS) {
        printf("Número máximo de frutas alcançado para este supermercado!\n");
        return;
    }

    Fruta nova_fruta;
    printf("Digite o nome da fruta: ");
    scanf("%s", nova_fruta.nome);
    printf("Digite a quantidade em estoque: ");
    scanf("%d", &nova_fruta.estoque);
    printf("Digite o estado da fruta (verde ou madura): ");
    scanf("%s", nova_fruta.estado);
    printf("Digite o modo de conservação da fruta (bom ou ótimo): ");
    scanf("%s", nova_fruta.conservacao);
    printf("Digite a estação de colheita da fruta (verão ou cachimbo): ");
    scanf("%s", nova_fruta.estacao);

    supermercados[supermercado].frutas[supermercados[supermercado].num_frutas] = nova_fruta;
    supermercados[supermercado].num_frutas++;

    printf("Fruta adicionada com sucesso!\n");
}

void* encomendaThread(void* args) {
    EncomendaArgs* encomendaArgs = (EncomendaArgs*) args;
    Supermercado* supermercado = encomendaArgs->supermercado;

    printf("Encomenda para %s:\n", supermercado->nome);
    for (int i = 0; i < supermercado->num_frutas; i++) {
        if (encomendaArgs->quantidades[i] > supermercado->frutas[i].estoque) {
            printf("Quantidade requisitada de %s excede o estoque disponível. Estoque disponível: %d, Quantidade requisitada: %d\n", supermercado->frutas[i].nome, supermercado->frutas[i].estoque, encomendaArgs->quantidades[i]);
            // Verifica se é a terceira encomenda e se o supermercado já foi banido
                      if (supermercado->num_frutas >= 3 && !supermercadosBanidos[i]) {
                printf("Supermercado %s banido por exceder o estoque disponível.\n", supermercado->nome);
                supermercadosBanidos[i] = 1;
            }
        } else {
            supermercado->frutas[i].estoque -= encomendaArgs->quantidades[i];
            printf("Fruta %s agora tem %d unidades em estoque.\n", supermercado->frutas[i].nome, supermercado->frutas[i].estoque);
        }
    }

    return NULL;
}

void encomendarProduto(Supermercado supermercados[NUM_SUPERMERCADOS]) {
    int supermercado;
    printf("Digite o número do supermercado para fazer a encomenda (1-%d): ", NUM_SUPERMERCADOS);
    scanf("%d", &supermercado);
    supermercado--;

    if (supermercado < 0 || supermercado >= NUM_SUPERMERCADOS) {
        printf("Supermercado inválido!\n");
        return;
    }

    printf("Encomenda para %s:\n", supermercados[supermercado].nome);
    for (int i = 0; i < supermercados[supermercado].num_frutas; i++) {
        printf("Digite a quantidade desejada de %s: ", supermercados[supermercado].frutas[i].nome);
        int quantidade;
        scanf("%d", &quantidade);

        if (quantidade > supermercados[supermercado].frutas[i].estoque) {
            printf("Quantidade requisitada de %s excede o estoque disponível. Estoque disponível: %d, Quantidade requisitada: %d\n", supermercados[supermercado].frutas[i].nome, supermercados[supermercado].frutas[i].estoque, quantidade);
        } else {
            supermercados[supermercado].frutas[i].estoque -= quantidade;
            printf("Fruta %s agora tem %d unidades em estoque.\n", supermercados[supermercado].frutas[i].nome, supermercados[supermercado].frutas[i].estoque);
        }
    }
    printf("Encomenda realizada com sucesso!\n");
}

void encomendarProdutoSimultaneo(Supermercado supermercados[NUM_SUPERMERCADOS]) {
    pthread_t threads[NUM_SUPERMERCADOS];
    EncomendaArgs args[NUM_SUPERMERCADOS];

    for (int i = 0; i < NUM_SUPERMERCADOS; i++) {
        args[i].supermercado = &supermercados[i];
        for (int j = 0; j < supermercados[i].num_frutas; j++) {
            printf("Digite a quantidade desejada de %s para o %s: ", supermercados[i].frutas[j].nome, supermercados[i].nome);
            scanf("%d", &args[i].quantidades[j]);
        }
    }

    for (int i = 0; i < NUM_SUPERMERCADOS; i++) {
        pthread_create(&threads[i], NULL, encomendaThread, (void*) &args[i]);
    }

    for (int i = 0; i < NUM_SUPERMERCADOS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Todas as encomendas foram realizadas com sucesso!\n");
}

void exibirEstoqueSupermercado(Supermercado supermercados[NUM_SUPERMERCADOS]) {
    int supermercado;
    printf("Digite o número do supermercado para exibir o estoque (1-%d): ", NUM_SUPERMERCADOS);
    scanf("%d", &supermercado);
    supermercado--;

    if (supermercado < 0 || supermercado >= NUM_SUPERMERCADOS) {
        printf("Supermercado inválido!\n");
        return;
    }

    exibeEstoque(supermercados[supermercado]);
}

void exibirEstoquePorEstacao(Supermercado supermercados[NUM_SUPERMERCADOS], const char* estacao) {
    for (int i = 0; i < NUM_SUPERMERCADOS; i++) {
        exibeEstoquePorEstacao(supermercados[i], estacao);
    }
}

int main() {
    Supermercado supermercados[NUM_SUPERMERCADOS];

    inicializaSupermercados(supermercados);

    int opcao;

    do {
        printf("Escolha uma opção:\n");
        printf("1. Exibir estoque de frutas verdes\n");
        printf("2. Exibir estoque de frutas maduras\n");
        printf("3. Exibir estoque por estação de colheita\n");
        printf("4. Inserir novo produto\n");
        printf("5. Fazer encomenda\n");
        printf("6. Fazer encomenda simultânea\n");
        printf("7. Exibir estoque de um supermercado\n");
        printf("0. Sair\n");
        printf("Opção: ");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1:
                printf("Exibindo estoque de frutas verdes:\n");
                for (int i = 0; i < NUM_SUPERMERCADOS; i++) {
                    exibeEstoqueVerde(supermercados[i]);
                }
                break;
            case 2:
                printf("Exibindo estoque de frutas maduras:\n");
                for (int i = 0; i < NUM_SUPERMERCADOS; i++) {
                    exibeEstoqueMaduro(supermercados[i]);
                }
                break;
            case 3: {
                char estacao[MAX_ESTACAO];
                printf("Digite a estação de colheita para exibir o estoque (verão ou cachimbo): ");
                scanf("%s", estacao);
                exibirEstoquePorEstacao(supermercados, estacao);
                break;
            }
            case 4:
                insereProduto(supermercados);
                break;
            case 5:
                encomendarProduto(supermercados);
                break;
            case 6:
                encomendarProdutoSimultaneo(supermercados);
                break;
            case 7:
                exibirEstoqueSupermercado(supermercados);
                break;
            case 0:
                printf("Saindo...\n");
                break;
            default:
                printf("Opção inválida!\n");
        }
    } while (opcao != 0);

    return 0;
}
