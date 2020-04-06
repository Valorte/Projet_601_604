#include "Fonction.h"

void init_joueur(int i, joueur_t *j)
{

    j->num = i;
    j->etat = 0;
    j->argent = 0;
    j->point = 0;
}

void afficher_etang(case_t *etang, int largeur, int longueur, WINDOW *fenetre)
{
    int i, j, k = 0;
    for (i = 0; i < longueur; i++)
    {
        for (j = 0; j < largeur; j++)
        {
            mvwprintw(fenetre, i, j, "%d", etang[k].valeur);
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

    while (etang[i].valeur != 0)
    {
        i = rand() % longueur * largeur;
    }
    p->pos = i;
    etang[i].objet.p = p;
}

void ajouter_requete(file_requete_t *f, requete_t *r)
{
    f->r[f->indice_tete] = *r;
    f->indice_tete++;
    if (f->indice_tete == 50)
    {
        f->indice_tete = 0;
    }
}

void supprimer_requete(file_requete_t *f, requete_t *r)
{
    f->r[f->indice_tete] = *r;
    f->indice_queue++;
    if (f->indice_queue == 50)
    {
        f->indice_queue = 0;
    }
}

void ajouter_action(file_action_t *f, action_t *r)
{
    f->a[f->indice_tete] = *r;
    f->indice_tete++;
    if (f->indice_tete == 50)
    {
        f->indice_tete = 0;
    }
}

void supprimer_action(file_action_t *f, action_t *r)
{
    f->a[f->indice_tete] = *r;
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
    switch (r)
    {
    case 0:
        if (p->pos > 0)
        {
            if (etang[p->pos-1].valeur == 0)
            {
                p->pos--;
            }
        }
        break;
    case 1:
        if (p->pos < (largeur * longueur) - 1)
        {
            if (etang[p->pos+1].valeur == 0)
            {
                p->pos++;
            }
        }
        break;
    case 2:
        if (p->pos < (largeur * longueur) - largeur)
        {
            if (etang[p->pos + largeur].valeur == 0)
            {
                p->pos += largeur;
            }
        }
        break;
    case 3:
        if (p->pos >= largeur)
        {
            if (etang[p->pos - largeur].valeur == 0)
            {
                p->pos -= largeur;
            }
        }
        break;
    }
    changer_case(&etang[p->pos], 1, p->id, 0);
    etang[p->pos].objet.p = p;
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

action_t lire_action(file_action_t *f)
{

    action_t tmp;

    tmp = f->a[f->indice_queue];
    f->indice_queue++;

    return tmp;
}

void vider_case(case_t *etang)
{
    etang->joueur = 0;
    etang->valeur = 0;
    etang->type_case = 0;
    etang->objet.p = NULL;
}

void changer_case(case_t *etang, int type, int nb, int joueur)
{
    etang->joueur = joueur;
    etang->type_case = type;
    etang->valeur = nb;
}

void ajouter_canne(case_t *etang, action_t *canne)
{
    vider_case(&etang[canne->position]);
    etang[canne->nouvelle_position].objet.a = canne;
    etang[canne->nouvelle_position].joueur = canne->id_joueur;
    etang[canne->nouvelle_position].type_case = TYPE_CANNE;
    etang[canne->nouvelle_position].valeur = 9;
}

int file_vide(file_action_t *a)
{
    if (a->indice_queue == a->indice_tete)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

int attrape_poisson(case_t *etang, poisson_t *p, int largeur)
{
    case_t maybe[4];
    int nb_canne = 0, i, ok = -1, choix;

    srand(time(NULL));
    maybe[0] = etang[p->pos - 1];
    maybe[1] = etang[p->pos + 1];
    maybe[2] = etang[p->pos + largeur];
    maybe[3] = etang[p->pos - largeur];

    for (i = 0; i < 4; i++)
    {
        if (maybe[i].type_case == TYPE_CANNE)
        {
            nb_canne++;
            choix = i;
        }
    }
    if (nb_canne > 1)
    {
        choix = rand() % 99;

        while (maybe[choix % 4].type_case != TYPE_CANNE)
        {
            choix++;
        }
        choix = choix % 4;
    }

    if (choix == 0)
    {
        ok = p->pos - 1;
    }
    if (choix == 1)
    {
        ok = p->pos + 1;
    }
    if (choix == 2)
    {
        ok = p->pos + largeur;
    }
    if (choix == 3)
    {
        ok = p->pos - largeur;
    }

    return ok;
}