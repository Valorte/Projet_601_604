#ifndef FONCTION
#define FONCTION
#include <pthread.h>
#include <stdlib.h>     /* Pour exit, EXIT_FAILURE, EXIT_SUCCESS */
#include <stdio.h>      /* Pour printf, fprintf, perror */
#include <sys/socket.h> /* Pour socket, bind */
#include <arpa/inet.h>  /* Pour sockaddr_in */
#include <string.h>     /* Pour memset */
#include <unistd.h>     /* Pour close */
#include <errno.h>      /* Pour errno */
#include <sys/shm.h>  /* Pour shmget, shmat, shmdt */
#include <sys/stat.h> /* Pour S_IRUSR, S_IWUSR */
#include <time.h> 
#include "Struct.h"

void init_joueur(int i , joueur_t* j);
void envoie_message(int sockfd, void* msg ,struct sockaddr_in adresseServeur);
void recevoir_message(int sockfd , int* msg, void *sa, void* sl);
void afficher_etang(int* etang , int largeur ,int longueur );
void generer_poison(int* etang , int largeur , int longueur , poisson_t* p);
void init_poisson(poisson_t* p,int val);
void modif_etang(int* etang , int p , int s , int val);
void deplacement_poisson(int* etang,poisson_t* p,int largeur ,int longueur);
void ajouter_requete(file_t* f,requete_t* r);
void supprimer_requete(file_t* f,requete_t* r);


#endif