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

void afficher_etang(case_t *etang, int largeur, int longueur)
{
    int i, j, k = 0;

    for (i = 0; i < longueur; i++)
    {
        for (j = 0; j < largeur; j++)
        {
            printf("%d", etang[k].nb);
            k++;
        }
        printf("\n");
    }
}
void init_poisson(poisson_t *p, int id)
{
    int v;
    p->id = id;
    p->etat = 0;
    p->pos = 0;

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
void generer_poison(case_t *etang, int largeur, int longueur, poisson_t *p, int id)
{
    int i;
    srand(time(NULL));
    init_poisson(p, id);
    i = rand() % longueur * largeur;

    while (etang[i].nb != 0)
    {
        i = rand() % longueur * largeur;
    }
    p->pos = i;
    etang[i].objet.p = p;
}

void ajouter_requete(file_t *f, requete_t *r)
{
    f->type.file_q[f->indice_tete] = *r;
    f->indice_tete++;
    if (f->indice_tete == 50)
    {
        f->indice_tete = 0;
    }
}

void supprimer_requete(file_t *f, requete_t *r)
{
    f->type.file_q[f->indice_tete] = *r;
    f->indice_queue++;
    if (f->indice_queue == 50)
    {
        f->indice_queue = 0;
    }
}

void ajouter_action(file_t *f, action_t *r)
{
    f->type.file_a[f->indice_tete] = *r;
    f->indice_tete++;
    if (f->indice_tete == 50)
    {
        f->indice_tete = 0;
    }
}

void supprimer_action(file_t *f, action_t *r)
{
    f->type.file_a[f->indice_tete] = *r;
    f->indice_queue++;
    if (f->indice_queue == 50)
    {
        f->indice_queue = 0;
    }
}
void deplacement_poisson(case_t *etang, poisson_t *p, int largeur, int longueur)
{
    int r;
    vider_case(&etang[p->pos]);

    r = rand() % 4;

    printf("\nrand : %d\n", r);
    switch (r)
    {
    case 0:
        if (p->pos > 0 && &etang[(p->pos)-1]==0)
        {
            p->pos--;
        }
        break;
    case 1:
        if (p->pos < (largeur * longueur) - 1 && &etang[(p->pos)+1])
        {
            p->pos++;
        }
        break;
    case 2:
        if (p->pos < (largeur * longueur) - largeur && &etang[(p->pos)+largeur])
        {
            p->pos += largeur;
        }
        break;
    case 3:
        if (p->pos >= largeur && &etang[(p->pos)-largeur])
        {
            p->pos -= largeur;
        }
        break;
    }
    changer_case(&etang[p->pos],1,p->id,0);
    etang[p->pos].objet.p=p;
    
}
/**
 * @param envoie 1 pour envoyé la carte avec la longueur et la largeur 
 **/
void envoie_info(int sockclient, case_t *etang, int longueur, int largeur, int envoie)
{
    if (envoie == 1)
    {
        if (write(sockclient, &longueur, sizeof(longueur)) == -1)
        {
            perror("Erreur lors de l'envoie de la longueur  ");
            exit(EXIT_FAILURE);
        }

        if (write(sockclient, &largeur, sizeof(largeur)) == -1)
        {
            perror("Erreur lors de l'envoie de la largeur 1 ");
            exit(EXIT_FAILURE);
        }

        if (write(sockclient, etang, sizeof(case_t) * longueur * largeur) == -1)
        {
            perror("Erreur lors de l'envoie de l'etang 1 ");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        if (write(sockclient, etang, sizeof(case_t) * longueur * largeur) == -1)
        {
            perror("Erreur lors de l'envoie de l'etang  2 ");
            exit(EXIT_FAILURE);
        }
    }
}

void recevoir_info(int fd, case_t *etang, int longueur, int largeur)
{

    printf("Recevoir");
    while (longueur == 0)
    {
        if (read(fd, &longueur, sizeof(int)) == -1)
        {
            perror("Erreur lors de la recpetion de la longueur ");
            exit(EXIT_FAILURE);
        }
    }

    while (largeur == 0)
    {
        if (read(fd, &largeur, sizeof(int)) == -1)
        {
            perror("Erreur lors de la reception de la largeur ");
            exit(EXIT_FAILURE);
        }
    }
    printf("largeur : %d", largeur);
    etang = malloc(sizeof(case_t) * (longueur * largeur));

    if (read(fd, &etang, sizeof(case_t) * longueur * largeur) == -1)
    {
        perror("Erreur lors de la reception de l'etang ");
        exit(EXIT_FAILURE);
    }
}

action_t lire_action(file_t* f){

    action_t tmp;

    tmp=f->type.file_a[f->indice_queue];
    f->indice_queue++;

    return tmp;
}

void vider_case(case_t* etang){
    etang->joueur=0;
    etang->nb=0;
    etang->type_case=0;
    etang->objet.p=NULL;
}

void changer_case(case_t* etang , int type , int nb , int joueur){
    etang->joueur=joueur;
    etang->type_case=type;
    etang->nb=nb;
}

void ajouter_canne(case_t* etang ,action_t* canne , int position){
    etang[position].objet.a=canne;
    etang[position].joueur=canne->id_action;
    etang[position].type_case=TYPE_CANNE;
    etang[position].nb=3;
}