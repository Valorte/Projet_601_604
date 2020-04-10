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

            if (etang[k].type_case==TYPE_POISSON)
            {
                wattron(fenetre,COLOR_PAIR(6));
                mvwprintw(fenetre, i, j, "%d", etang[k].valeur);
                wattroff(fenetre,COLOR_PAIR(6));
            }else
            {
                wattron(fenetre,COLOR_PAIR(4));
                mvwprintw(fenetre, i, j, "%d", etang[k].valeur);
                wattroff(fenetre,COLOR_PAIR(4));
            }
            
            
            k++;
        }
        printf("\n");
    }
}
void init_poisson(poisson_t *p, int id)
{
    int v;
    srand(time(NULL));
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
    pthread_mutex_init(&p->mutex_poisson,NULL);
    pthread_cond_init(&p->condi_poisson,NULL);
    p->pos = i;
    etang[i].objet.p = *p;
    etang[i].type_case = TYPE_POISSON;
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
            if (etang[p->pos - 1].valeur == 0)
            {
                p->pos--;
            }
        }
        break;
    case 1:
        if (p->pos < (largeur * longueur) - 1)
        {
            if (etang[p->pos + 1].valeur == 0)
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
    changer_case_poisson(etang, p);
}

void envoie_info(int sockclient, envoie_t e, case_t *etang)
{

    if (e.type_message == TYPE_MODIF || e.type_message == TYPE_ETANG)
    {
        /* code */

        if (write(sockclient, &e, sizeof(envoie_t)) == -1)
        {
            perror("Erreur lors de l'envoie de l'info");
            exit(EXIT_FAILURE);
        }
        if (write(sockclient, etang, sizeof(case_t) * e.objet.taille[0] * e.objet.taille[1]) == -1)
        {
            perror("Erreur lors de la reception de l'etang ");
            exit(EXIT_FAILURE);
        }
    }
    else{
        if (write(sockclient, &e, sizeof(envoie_t)) == -1)
        {
            perror("Erreur lors de l'envoie de l'info");
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

void vider_case(case_t *etang)
{
    etang->valeur = 0;
    etang->type_case = 0;
}

void changer_case_poisson(case_t *etang, poisson_t *p)
{
    etang[p->pos].objet.p = *p;
    etang[p->pos].type_case = TYPE_POISSON;
    etang[p->pos].valeur = p->valeur;
}

int attrape_poisson(poisson_t *p, int largeur, canne_t c[2])
{
    int joueur = -1, i;

    for (i = 0; i < 2; i++)
    {
        if (c[i].pos == p->pos - 1 || c[i].pos == p->pos + 1 || c[i].pos == p->pos - largeur || c[i].pos == p->pos + largeur)
        {
            joueur = c[i].pos;
        }
    }

    return joueur;
}

void fuite_poisson(case_t * etang,int pos_canne,int taille){
    int i;
    for ( i = 1; i < 4; i++)
    {
        if (etang[pos_canne +i].type_case==TYPE_POISSON && pos_canne +i < taille-(4-i))
        {
            etang[pos_canne+i].objet.p.pos+=3-i;
        }

        if (etang[pos_canne -i].type_case==TYPE_POISSON && pos_canne -i > 4-i)
        {
            etang[pos_canne-i].objet.p.pos-=3-i;
        }
        
    }
    
    
}