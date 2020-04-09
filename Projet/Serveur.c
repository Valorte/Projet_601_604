#include "Fonction.h"

int vict = 0, sockclient, sockclient2, tp;
case_t *e;
int hauteur, largeur, nbpoisson;
canne_t c;
int retval;
fd_set fds;
struct timeval tv;
pthread_t poisson[MAX_POISSON] , gene;

void *routine_poisson(void *args)
{

    poisson_t *tmp = (void *)args;
    int vie = -1;
    while (vie < 0)
    {
        pthread_mutex_lock(&e->mutex_case);
        deplacement_poisson(e, tmp, largeur, hauteur);
        envoie_info(sockclient, e, largeur, hauteur, 0);
        envoie_info(sockclient2, e, largeur, hauteur, 0);
        vie = attrape_poisson(e, tmp, largeur);
        pthread_mutex_unlock(&e->mutex_case);
        sleep(3);
    }
    
    if (e[vie].objet.c.joueur == 1)
    {
        printf("Le joueur %d a peché \n",e[vie].objet.c.joueur);
        if (write(sockclient, tmp, sizeof(poisson_t)) == -1)
        {
            perror("Erreur lors de l'envoie de la longueur  ");
            exit(EXIT_FAILURE);
        }
    }
    if (e[vie].objet.c.joueur == 2)
    {
        printf("Le joueur %d a peché \n",e[vie].objet.c.joueur);
        if (write(sockclient2, tmp, sizeof(poisson_t)) == -1)
        {
            perror("Erreur lors de l'envoie de la longueur  ");
            exit(EXIT_FAILURE);
        }
    }
    printf("fini\n");
    pthread_mutex_lock(&e->mutex_case);
    vider_case(&e[tmp->pos]);
    vider_case(&e[vie]);
    pthread_mutex_unlock(&e->mutex_case);

    return NULL;
}
void* generer_poisson(void *args){
    int i;
    poisson_t *p;
    p=malloc(sizeof(poisson_t)*MAX_POISSON);
    for (i = 0; i < MAX_POISSON; i++)
    {
        
        generer_poison(e, largeur, hauteur, &p[i], i);
        pthread_create(&poisson[i], NULL, routine_poisson, &p[i]);
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    reponse_t reponse[2];
    requete_t requete;
    int sockfd, i = 0, fd, j, k = 0, taille, max;
    struct sockaddr_in adresseServeur;
    struct sockaddr_in adresseClient[2];
    socklen_t longueurAdresse;

    /* Récupération des arguments */
    if (argc != 4)
    {
        fprintf(stderr, "Usage : %s port hauteur largeur cle\n", argv[0]);
        fprintf(stderr, "Où :\n");
        fprintf(stderr, "  port           : port d'écoute du serveur\n");
        fprintf(stderr, "  hauteur       : hauteur de l'etang\n");
        fprintf(stderr, "  largeur        : largeur de l'etang\n");
        exit(EXIT_FAILURE);
    }
    longueurAdresse = sizeof(struct sockaddr_in);
    /* Création de la socket */
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        perror("Erreur lors de la création de la socket ");
        exit(EXIT_FAILURE);
    }

    /* Création de l'adresse du serveur */
    memset(&adresseServeur, 0, sizeof(struct sockaddr_in));
    adresseServeur.sin_family = AF_INET;
    adresseServeur.sin_port = htons(atoi(argv[1]));
    adresseServeur.sin_addr.s_addr = htonl(INADDR_ANY);

    /* Nommage de la socket */
    if (bind(sockfd, (struct sockaddr *)&adresseServeur, sizeof(struct sockaddr_in)) == -1)
    {
        perror("Erreur lors du nommage de la socket ");
        exit(EXIT_FAILURE);
    }
    while (i != 2)
    {
        /* Attente de la requête du client */
        printf("Serveur en attente du message du client.\n");
        if (recvfrom(sockfd, &requete, sizeof(requete_t), 0, (struct sockaddr *)&adresseClient[i], &longueurAdresse) == -1)
        {
            perror("Erreur lors de la réception du message ");
            exit(EXIT_FAILURE);
        }
        if (requete.type_requete == TYPE_CONNEXION)
        {
            reponse[i].type = TYPE_REPONSE;
            reponse[i].id = i + 1;
            strcpy(reponse[i].port, "1024");
            i++;
            printf("Joueur %d connecté\n", i);
        }
    }

    for (i = 0; i < 2; i++)
    {
        if (sendto(sockfd, &reponse[i], sizeof(reponse_t), 0, (struct sockaddr *)&adresseClient[i], longueurAdresse) == -1)
        {
            perror("Erreur lors de l'envoi du message ");
            exit(EXIT_FAILURE);
        }
    }

    if ((fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
    {
        perror("Erreur lors de la création de la socket ");
        exit(EXIT_FAILURE);
    }

    hauteur = atoi(argv[2]);
    largeur = atoi(argv[3]);
    taille = hauteur * largeur;
    e = malloc(sizeof(case_t) * taille);
    for (i = 0; i < hauteur; i++)
    {
        for (j = 0; j < largeur; j++)
        {
            e[k].valeur = 0;
            e[k].type_case = 0;
            k++;
        }
    }
    pthread_mutex_init(&e->mutex_case, NULL);

    /* Création de l'adresse du serveur */
    memset(&adresseServeur, 0, sizeof(struct sockaddr_in));
    adresseServeur.sin_family = AF_INET;
    adresseServeur.sin_addr.s_addr = htonl(INADDR_ANY);
    adresseServeur.sin_port = htons(atoi("1024"));

    /* Nommage de la socket */
    if (bind(fd, (struct sockaddr *)&adresseServeur, sizeof(struct sockaddr_in)) == -1)
    {
        perror("Erreur lors du nommage de la socket ");
        exit(EXIT_FAILURE);
    }

    /* Mise en mode passif de la socket */
    if (listen(fd, 2) == -1)
    {
        perror("Erreur lors de la mise en mode passif ");
        exit(EXIT_FAILURE);
    }

    /* Attente d'une connexion */
    printf("Serveur : attente de connexion...\n");
    if ((sockclient = accept(fd, NULL, NULL)) == -1)
    {
        perror("Erreur lors de la demande de connexion ");
        exit(EXIT_FAILURE);
    }

    if ((sockclient2 = accept(fd, NULL, NULL)) == -1)
    {
        perror("Erreur lors de la demande de connexion ");
        exit(EXIT_FAILURE);
    }
    printf("Les deux sockets sont connectées\n");

    envoie_info(sockclient, e, largeur, hauteur, 1);
    envoie_info(sockclient2, e, largeur, hauteur, 1);

    /* pthread_create(&gene,NULL,generer_poisson,NULL); */
    while (1)
    {

        max = sockclient2 + sockclient;
        tv.tv_sec = 0;
        tv.tv_usec = 100;
        FD_ZERO(&fds);
        FD_SET(sockclient, &fds);
        FD_SET(sockclient2, &fds);

        if ((retval = select(max + 1, &fds, NULL, NULL, &tv)) == -1)
        {
            if (errno != EINTR)
            {
                perror("Serveur : erreur select");
                exit(EXIT_FAILURE);
            }
        }
        if (FD_ISSET(sockclient, &fds))
        {
            if (read(sockclient, &c, sizeof(canne_t)) == -1)
            {
                perror("Erreur lors de la recepetion de la canne de 1");
                exit(EXIT_FAILURE);
            }
            pthread_mutex_lock(&e->mutex_case);
            vider_case(&e[c.ancienne_pos]);
            e[c.pos].type_case = TYPE_CANNE;
            e[c.pos].valeur = c.joueur + 5;
            e[c.pos].objet.c = c;
            pthread_mutex_unlock(&e->mutex_case);
        }
        if (FD_ISSET(sockclient2, &fds))
        {

            if (read(sockclient2, &c, sizeof(canne_t)) == -1)
            {
                perror("Erreur lors de la recepetion de la canne de 2");
                exit(EXIT_FAILURE);
            }
            pthread_mutex_lock(&e->mutex_case);
            vider_case(&e[c.ancienne_pos]);
            e[c.pos].type_case = TYPE_CANNE;
            e[c.pos].valeur = c.joueur + 5;
            e[c.pos].objet.c = c;
            pthread_mutex_unlock(&e->mutex_case);
        }
        envoie_info(sockclient, e, largeur, hauteur, 0);
        envoie_info(sockclient2, e, largeur, hauteur, 0);
    }

    for (i = 0; i < MAX_POISSON; i++)
    {
        pthread_join(poisson[i], NULL);
    }
    pthread_join(gene, NULL);

    /* Fermeture des sockets */
    if (close(sockclient) == -1)
    {
        perror("Erreur lors de la fermeture de la socket de communication ");
        exit(EXIT_FAILURE);
    }
    if (close(sockclient2) == -1)
    {
        perror("Erreur lors de la fermeture de la socket de communication ");
        exit(EXIT_FAILURE);
    }
    if (close(fd) == -1)
    {
        perror("Erreur lors de la fermeture de la socket de connexion ");
        exit(EXIT_FAILURE);
    }
    /* Fermeture de la socket */
    if (close(sockfd) == -1)
    {
        perror("Erreur lors de la fermeture de la socket ");
        exit(EXIT_FAILURE);
    }

    printf("Serveur terminé.\n");
    free(e);

    return EXIT_SUCCESS;
}
