#ifndef STRUCT
#define STRUCT
#define MAX_POISSON 10


typedef struct joueur_t
{
    int num;
    int point;
    int argent;
    int etat;
}joueur_t;


typedef struct poisson_t
{
    int valeur;
    int pos;
    int etat; 
    int id;
}poisson_t;

typedef struct plateau_t
{
    int* etang;
    int longueur;
    int largeur;
    poisson_t p [MAX_POISSON];
}plateau_t;

#define TYPE_CONNEXION 1
typedef struct requete_t
{
    long type;

}requete_t;

#define TYPE_REPONSE 2
typedef struct reponse_t
{
    long type;
    char port[25];
    int id;
}reponse_t;

#define TYPE_BONUS 3
#define TYPE_CANNE 4
typedef struct action_t
{
    long type;
    int id_action;
    int position;
}action_t;

#define TYPE_VICTOIRE 5
typedef struct victoire_t
{
    long type;
    joueur_t j;
}victoire_t;


typedef struct file_t
{
    requete_t file[50];
    int indice_tete;
    int indice_queue;
}file_t;


#endif