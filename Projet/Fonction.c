#include "Fonction.h"

void init_joueur(int i, joueur_t *j)
{

    j->num = i;
    j->etat = 0;
    j->argent = 0;
    j->point = 0;
}
void envoie_message(int sockfd, void *msg, struct sockaddr_in adresseServeur)
{
    /* Envoi du message */
    if (sendto(sockfd, &msg, sizeof(msg), 0, (struct sockaddr *)&adresseServeur, sizeof(struct sockaddr_in)) == -1)
    {
        perror("Erreur lors de l'envoi du message ");
        exit(EXIT_FAILURE);
    }
}

void recevoir_message(int sockfd, int *msg, void *sa, void *sl)
{
    if (recvfrom(sockfd, &msg, sizeof(msg), 0, sa, sl) == -1)
    {
        perror("Erreur lors de la réception du message ");
        exit(EXIT_FAILURE);
    }
    printf("%d", *msg);
}

void afficher_etang(int *etang, int largeur, int longueur)
{
    int i, j, k = 0;

    for (i = 0; i < longueur; i++)
    {
        for (j = 0; j < largeur; j++)
        {
            printf("%d", etang[k]);
        }
        printf("\n");
        k++;
    }
}
void init_poisson(poisson_t *p)
{
    int v;
    p->etat = 0;
    v = (rand() % 99);
    if (v < 65)
    {
        p->valeur = 1;
    }
    else if (v > 90)
    {
        p->valeur = 3;
    }
    else
    {
        p->valeur = 2;
    }
}
void generer_poison(int *etang, int largeur, int longueur, poisson_t *p)
{
    int i;
    srand(time(NULL));
    init_poisson(p);
    i = rand() % longueur * largeur;

    while (etang[i] != 0)
    {
        i = rand() % longueur * largeur;
    }
    p->pos = i;
    etang[i] = p->valeur;
    printf("\nNouvelle valeur dasn l'etang : %d\n",etang[i]);
}

void modif_etang(int *etang, int p, int s, int val)
{
    etang[p] = 0;
    etang[s] = val;
}