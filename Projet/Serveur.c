#include "Fonction.h"

pthread_mutex_t mutex_etang = PTHREAD_MUTEX_INITIALIZER;
int vict = 0, sockclient, sockclient2;
case_t *etang;
int hauteur, largeur, nbpoisson;

void *routine_requete(void *args)
{   
    
    return NULL;
}

void *routine_poisson(void *args)
{
    int i = 0;
    poisson_t *tmp = (void *)args;
    while (vict == 0)
    {

        pthread_mutex_lock(&mutex_etang);
        deplacement_poisson(etang, tmp, largeur, hauteur);
        pthread_mutex_unlock(&mutex_etang);
        envoie_info(sockclient, etang, hauteur, largeur, 0);
        envoie_info(sockclient2, etang, hauteur, largeur, 0);
        sleep(3);
        i++;
    }

    return NULL;
}

int main(int argc, char *argv[])
{
    reponse_t reponse[2];
    requete_t requete;
    file_requete_t file_requete;
    file_action_t file_action;
    poisson_t p[2];
    pthread_t poisson[2], r;
    int sockfd, i = 0, fd, j, k = 0, taille;
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
    file_requete.indice_queue = 0;
    file_requete.indice_tete = 0;
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
        ajouter_requete(&file_requete, &requete);
        if (requete.type == 1)
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
    etang = malloc(sizeof(case_t) * taille);
    for (i = 0; i < hauteur; i++)
    {
        for (j = 0; j < largeur; j++)
        {
            etang[k].nb = 0;
            etang[k].joueur = 0;
            etang[k].type_case = 0;
            k++;
        }
    }

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

    envoie_info(sockclient, etang, hauteur, largeur, 1);
    envoie_info(sockclient2, etang, hauteur, largeur, 1);

    pthread_create(&r, NULL, routine_requete, &file_action);

    for (i = 0; i < 2; i++)
    {
        generer_poison(etang, largeur, hauteur, &p[i], i + 2);
        pthread_create(&poisson[i], NULL, routine_poisson, &p[i]);
    }    

    for (i = 0; i < 2; i++)
    {
        pthread_join(poisson[i], NULL);
    }
    pthread_join(r, NULL);

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
    free(etang);

    return EXIT_SUCCESS;
}
