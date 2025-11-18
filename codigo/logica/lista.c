#include "lista.h"

void inserir_bala(Lista *lista, Bala bala) {
    No * novo = (No *)malloc(sizeof(No));
    novo->dado = bala;
    novo->prox = NULL;
    if(lista->inicio == NULL) {
        lista->inicio = novo;
    }else {
        No * temp;
        for(temp = lista->inicio; temp->prox != NULL; temp = temp->prox);
        temp->prox = novo;
    }
}
void remover_balas_mortas(Lista *lista) {
   if(lista->inicio == NULL) return;

    No * temp = lista->inicio;
    No * anterior = NULL;

   for(;temp != NULL;) {
        if(!temp->dado.ativa) {
            No * remover = temp;

            if(anterior == NULL) {
                lista->inicio = temp->prox;
                temp = lista->inicio;
            }else {
                anterior->prox = temp->prox;
                temp = anterior->prox;
            }
            free(remover);
        }else {
            anterior = temp;
            temp = temp->prox;
        }
   }
}

void liberar_lista(Lista *lista) {
    if(lista->inicio == NULL) return;
    No * temp = lista->inicio;
    No * auxi;
    while(temp != NULL) {
        auxi = temp;
        temp->prox = auxi->prox;
        free(auxi);
    }
    
}