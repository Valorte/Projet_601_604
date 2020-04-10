#ifndef STRUCT
#define STRUCT
#include <ncurses.h>
#include "ncurses.h"
#include <pthread.h>
#define MAX_POISSON 10
#define MAX_REQUETE 100

#define TYPE_TAILLE 1
#define TYPE_ETANG 2 
#define TYPE_CANNE 3
#define TYPE_BONUS 4
#define TYPE_MODIF 5
#define TYPE_CONNEXION 6
#define TYPE_REPONSE 7
#define TYPE_POISSON 8
#define TYPE_CANNE_RELEVE 9



typedef struct poisson_t
{
    pthread_mutex_t mutex_poisson;
    pthread_cond_t condi_poisson;
    pthread_mutex_t mutex_poisson2;
    pthread_cond_t condi_poisson2;
    int valeur;
    int pos;
    int etat;
} poisson_t;

typedef struct canne_t
{
    int pos;
    int etat;
    int joueur;
} canne_t;

typedef struct bonus_t
{
    int valeur;
    int pos;
    int type_bonus;
} bonus_t;


typedef struct case_t
{
    int valeur;
    int type_case;
    union {
        poisson_t p;
        bonus_t b;
    } objet;
} case_t;


typedef struct etang_t
{
    case_t* e;
    pthread_mutex_t mutex_etang;

} etang_t;

typedef struct envoie_t
{
    int type_message;
    union {
        poisson_t p;
        canne_t c;
        bonus_t b;
        int taille[2];
    } objet;
} envoie_t;

typedef struct joueur_t
{
    int num;
    int point;
    int argent;
    int etat;
} joueur_t;

typedef struct reponse_t
{
    long type;
    char port[25];
    int id;
} reponse_t;

typedef struct mutex_f
{
    WINDOW *sous_simulation;
    WINDOW *sous_joueur;
    WINDOW *sous_score;
    pthread_mutex_t mutex_fenetre;
    pthread_cond_t attente;
    pthread_cond_t attente2;
} mutex_f;

typedef struct mutex_fd
{
    int fd;
    pthread_mutex_t mutex_descripteur;
    pthread_cond_t condi_fd;
} mutex_fd;

typedef struct requete_t
{
    long type_requete;
    union
    {
        canne_t c;
        poisson_t p;
        bonus_t b;
        int taille[2];
    }objet;
    
} requete_t;


#endif