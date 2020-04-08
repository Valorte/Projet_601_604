#ifndef FONCTION
#define FONCTION
#define _XOPEN_SOURCE 500
#include <stdlib.h>     /* Pour exit, EXIT_FAILURE, EXIT_SUCCESS */
#include <stdio.h>      /* Pour printf, fprintf, perror */
#include <sys/socket.h> /* Pour socket, bind */
#include <arpa/inet.h>  /* Pour sockaddr_in */
#include <string.h>     /* Pour memset */
#include <unistd.h>     /* Pour close */
#include <errno.h>      /* Pour errno */
#include <sys/shm.h>    /* Pour shmget, shmat, shmdt */
#include <sys/stat.h>   /* Pour S_IRUSR, S_IWUSR */
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include "Struct.h"

void init_joueur(int i, joueur_t *j);
void afficher_etang(case_t *etang, int largeur, int longueur, WINDOW *fenetre);
void vider_case(case_t *etang);
void changer_case_poisson(case_t *etang, poisson_t *p);
void generer_poison(case_t *etang, int largeur, int longueur, poisson_t *p, int id);
void init_poisson(poisson_t *p, int id);
void deplacement_poisson(case_t *etang, poisson_t *p, int largeur, int longueur);
void envoie_info(int sockclient, case_t *etang, int largeur, int longueur, int choix);
void recevoir_info(int fd, case_t *etang, int longueur, int largeur);
int attrape_poisson(case_t *etang, poisson_t *p, int largeur);
/*void ajouter_action(file_action_t* f,action_t* r);
void supprimer_action(file_action_t* f,action_t* r);
action_t lire_action(file_action_t* f);
int file_vide(file_action_t* a);*/

#endif