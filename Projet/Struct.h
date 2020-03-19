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
}poisson_t;

typedef struct plateau_t
{
    int* etang;
    int longueur;
    int largeur;
    poisson_t p [MAX_POISSON];
}plateau_t;


#endif