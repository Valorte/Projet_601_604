#ifndef STRUCT
#define STRUCT
#include <ncurses.h>
#include "ncurses.h"
#include <pthread.h>
#define MAX_POISSON 10

#define TYPE_BONUS 3
#define TYPE_CANNE 4
typedef struct action_t
{
    long type;
    int id_joueur;
    int position;
    int nouvelle_position;
} action_t;

typedef struct poisson_t
{
    int valeur;
    int pos;
    int etat;
    int id;
} poisson_t;


typedef struct case_t
{

    int valeur;
    int joueur;
    int type_case;
    union {
        poisson_t *p;
        action_t *a;

    } objet;

} case_t;

typedef struct joueur_t
{
    int num;
    int point;
    int argent;
    int etat;
} joueur_t;

typedef struct plateau_t
{
    int *etang;
    int longueur;
    int largeur;
    poisson_t p[MAX_POISSON];
} plateau_t;

#define TYPE_CONNEXION 1
typedef struct requete_t
{
    long type;

} requete_t;

#define TYPE_REPONSE 2
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
    pthread_mutex_t mutex_fenetre;
    pthread_cond_t attente;
} mutex_f;

typedef struct mutex_e
{
    case_t* etang;
    pthread_mutex_t mutex_etang;
} mutex_e;

#define TYPE_VICTOIRE 5
typedef struct victoire_t
{
    long type;
    joueur_t j;
} victoire_t;

typedef struct file_requete_t
{
    requete_t r[50];

    int indice_tete;
    int indice_queue;
} file_requete_t;

typedef struct file_action_t
{
    action_t a[50];

    int indice_tete;
    int indice_queue;
} file_action_t;

#endif