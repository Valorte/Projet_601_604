#include "Fonction.h"
mutex_fd *sockclient, *sockclient2;
etang_t *etang;
int hauteur, largeur;
canne_t canne[2];
int retval;
fd_set fds;
struct timeval tv;
poisson_t *p;
pthread_t poisson[MAX_POISSON], gene;
envoie_t message;

void *routine_poisson(void *args)
{

    envoie_t message_perso;
    poisson_t *tmp = (void *)args;
    int vie = -1;

    message_perso.type_message = TYPE_MODIF;
    message_perso.objet.taille[0] = hauteur;
    message_perso.objet.taille[1] = largeur;
    while (vie < 0)
    {
        pthread_mutex_lock(&etang->mutex_etang);
        deplacement_poisson(etang->e, tmp, largeur, hauteur);
        vie = attrape_poisson(tmp, largeur, canne); 
        pthread_mutex_unlock(&etang->mutex_etang);
        pthread_mutex_lock(&sockclient->mutex_descripteur);
        envoie_info(sockclient->fd, message_perso, etang->e);
        pthread_mutex_unlock(&sockclient->mutex_descripteur);
        pthread_mutex_lock(&sockclient2->mutex_descripteur);
        envoie_info(sockclient2->fd, message_perso, etang->e);
        pthread_mutex_unlock(&sockclient2->mutex_descripteur);
        sleep(1);
    }

    printf("fini\n");
    vider_case(&etang->e[tmp->pos]);
    message_perso.objet.p = *tmp;
    message_perso.type_message = TYPE_POISSON;
    if (canne[0].pos == vie)
    {
        pthread_mutex_lock(&sockclient->mutex_descripteur);
        canne[0].pos = -99;
        if (write(sockclient->fd, &message_perso, sizeof(envoie_t)) == -1)
        {
            perror("Erreur leurs du point\n");
            exit(EXIT_FAILURE);
        }
        pthread_mutex_unlock(&sockclient->mutex_descripteur);
    }
    if (canne[1].pos == vie)
    {
        pthread_mutex_lock(&sockclient2->mutex_descripteur);
        canne[1].pos = -99;
        if (write(sockclient2->fd, &message_perso, sizeof(envoie_t)) == -1)
        {
            perror("Erreur leurs du point\n");
            exit(EXIT_FAILURE);
        }
        pthread_mutex_unlock(&sockclient2->mutex_descripteur);
    }

    return NULL;
}
void *generer_poisson(void *args)
{
    int i;
    
    p = malloc(sizeof(poisson_t) * MAX_POISSON);
    for (i = 0; i < MAX_POISSON; i++)
    {
        pthread_mutex_lock(&etang->mutex_etang);
        generer_poison(etang->e, largeur, hauteur, &p[i], i);
        pthread_mutex_unlock(&etang->mutex_etang);
        usleep(100);
        pthread_create(&poisson[i], NULL, routine_poisson, &p[i]);
    }
    for (i = 0; i < MAX_POISSON; i++)
    {
        pthread_join(poisson[i], NULL);
    }
    free(p);
    return NULL;
}

int main(int argc, char *argv[])
{
    reponse_t reponse[2];
    requete_t requete;
    envoie_t canne_recu;
    int sockfd, i = 0, fd, j, k = 0, taille, max, c;
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
    etang = malloc(sizeof(etang_t));
    etang->e = malloc(sizeof(case_t) * taille);
    for (i = 0; i < hauteur; i++)
    {
        for (j = 0; j < largeur; j++)
        {
            etang->e[k].valeur = 0;
            etang->e[k].type_case = 0;
            k++;
        }
    }

    pthread_mutex_init(&etang->mutex_etang, NULL);

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
    sockclient = malloc(sizeof(mutex_fd));
    sockclient2 = malloc(sizeof(mutex_fd));

    /* Attente d'une connexion */
    printf("Serveur : attente de connexion...\n");
    if ((sockclient->fd = accept(fd, NULL, NULL)) == -1)
    {
        perror("Erreur lors de la demande de connexion ");
        exit(EXIT_FAILURE);
    }
    printf("1 connecté \n");

    if ((sockclient2->fd = accept(fd, NULL, NULL)) == -1)
    {
        perror("Erreur lors de la demande de connexion ");
        exit(EXIT_FAILURE);
    }
    printf("Les deux sockets sont connectées\n");

    pthread_mutex_init(&sockclient->mutex_descripteur, NULL);
    pthread_mutex_init(&sockclient2->mutex_descripteur, NULL);

    message.type_message = TYPE_ETANG;
    message.objet.taille[0] = hauteur;
    message.objet.taille[1] = largeur;
    pthread_mutex_lock(&sockclient->mutex_descripteur);
    envoie_info(sockclient->fd, message, etang->e);
    pthread_mutex_unlock(&sockclient->mutex_descripteur);
    pthread_mutex_lock(&sockclient2->mutex_descripteur);
    envoie_info(sockclient2->fd, message, etang->e);
    pthread_mutex_unlock(&sockclient2->mutex_descripteur);

    pthread_create(&gene, NULL, generer_poisson, NULL);
    canne[0].pos = -99;
    canne[1].pos = -99;
    while (1)
    {
        c = 0;

        max = sockclient2->fd + sockclient->fd;
        tv.tv_sec = 0;
        tv.tv_usec = 100;
        FD_ZERO(&fds);
        FD_SET(sockclient->fd, &fds);
        FD_SET(sockclient2->fd, &fds);

        if ((retval = select(max + 1, &fds, NULL, NULL, &tv)) == -1)
        {
            if (errno != EINTR)
            {
                perror("Serveur : erreur select");
                exit(EXIT_FAILURE);
            }
        }
        if (FD_ISSET(sockclient->fd, &fds))
        {

            if ((c = read(sockclient->fd, &canne_recu, sizeof(envoie_t))) == -1)
            {
                perror("Erreur lors de la recepetion de la canne de 1");
                exit(EXIT_FAILURE);
            }
            if (c != 0 && canne_recu.type_message == TYPE_CANNE)
            {
                 canne[0] = canne_recu.objet.c;
                /*for ( i = 0; i < MAX_POISSON; i++)
                {
                    pthread_mutex_lock(&p[i].mutex_poisson);
                    fuite_poisson(etang->e,canne[0].pos,taille);
                    pthread_mutex_unlock(&p[i].mutex_poisson);
                    
                } */
            }
            if (c != 0 && canne_recu.type_message == TYPE_CANNE_RELEVE)
            {
                printf("canne du relvé\n");
            }
        }
        if (FD_ISSET(sockclient2->fd, &fds))
        {

            if ((c = read(sockclient2->fd, &canne_recu, sizeof(envoie_t))) == -1)
            {
                perror("Erreur lors de la recepetion de la canne de 2");
                exit(EXIT_FAILURE);
            }
            if (c != 0 && canne_recu.type_message == TYPE_CANNE)
            {
                canne[1] = canne_recu.objet.c;
                /* for ( i = 0; i < MAX_POISSON; i++)
                {
                    pthread_mutex_lock(&p[i].mutex_poisson);
                    fuite_poisson(etang->e,canne[1].pos,taille);
                    pthread_mutex_unlock(&p[i].mutex_poisson);
                    
                } */
                
                printf("canne du joueur : %d , a la position %d\n", canne[1].joueur, canne[1].pos);
            }
            if (c != 0 && canne_recu.type_message == TYPE_CANNE_RELEVE)
            {
                printf("canne du relvé\n");
            }
        }
    }

    pthread_join(gene, NULL);

    /* Fermeture des sockets */
    if (close(sockclient->fd) == -1)
    {
        perror("Erreur lors de la fermeture de la socket de communication ");
        exit(EXIT_FAILURE);
    }
    if (close(sockclient2->fd) == -1)
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
    free(etang->e);
    free(etang);
    free(sockclient);
    free(sockclient2);

    return EXIT_SUCCESS;
}
