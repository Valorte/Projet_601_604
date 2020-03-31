#include "Fonction.h"

pthread_mutex_t mutex_etang = PTHREAD_MUTEX_INITIALIZER;
pthread_t p_poisson;
int vict = 0,sockclient, sockclient2;
int *etang;
int longueur, largeur, nbpoisson;


void *routine_poisson(void *args)
{
    int i = 0, p;
    poisson_t *tmp = (void *)args;
    while (i != 15)
    {
        p = tmp->pos;
        deplacement_poisson(etang, tmp, largeur, longueur);
        pthread_mutex_lock(&mutex_etang);
        modif_etang(etang, p, tmp->pos, tmp->valeur);
        pthread_mutex_unlock(&mutex_etang);
        envoie_info(sockclient,etang,longueur,largeur,0);
        envoie_info(sockclient2,etang,longueur,largeur,0);
        sleep(1);
        i++;
    }

    return NULL;
}

int main(int argc, char *argv[])
{

    reponse_t reponse[2];
    requete_t requete;
    action_t action;
    victoire_t victoire;
    long type;
    poisson_t poisson;
    file_t file_requete;
    int sockfd, i = 0, fd, j, k = 0, taille;
    struct sockaddr_in adresseServeur;
    struct sockaddr_in adresseClient[2];
    socklen_t longueurAdresse;

    /* Récupération des arguments */
    if (argc != 4)
    {
        fprintf(stderr, "Usage : %s port longueur largeur cle\n", argv[0]);
        fprintf(stderr, "Où :\n");
        fprintf(stderr, "  port           : port d'écoute du serveur\n");
        fprintf(stderr, "  longueur       : longueur de l'etang\n");
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

    /* Fermeture de la socket */
    if (close(sockfd) == -1)
    {
        perror("Erreur lors de la fermeture de la socket ");
        exit(EXIT_FAILURE);
    }
    longueur = atoi(argv[2]);
    largeur = atoi(argv[3]);
    taille = longueur * largeur;
    etang = malloc(sizeof(int) * taille);
    for (i = 0; i < longueur; i++)
    {
        for (j = 0; j < largeur; j++)
        {
            etang[k] = 0;
            k++;
        }
    }

    generer_poison(etang,largeur,longueur,&poisson);
    pthread_create(&p_poisson,NULL,routine_poisson,&poisson);

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
    
    envoie_info(sockclient,etang,longueur,largeur,1);
    envoie_info(sockclient2,etang,longueur,largeur,1);

    if (read(sockclient, &type, sizeof(long)) == -1)
    {
        perror("Erreur lors de la lecture de la taille du message ");
        exit(EXIT_FAILURE);
    }

    switch (type)
    {
    case 4:
        if (read(sockclient, &action, sizeof(action_t)) == -1)
        {
            perror("Erreur lors de la lecture de la taille du message ");
            exit(EXIT_FAILURE);
        }
        printf("On vient de poser une canne a la position : %d\n", action.position);
        break;
    case 3:
        if (read(sockclient, &action, sizeof(action_t)) == -1)
        {
            perror("Erreur lors de la lecture de la taille du message ");
            exit(EXIT_FAILURE);
        }
        printf("On vient de poser un bonus a la position : %d\n", action.position);

        modif_etang(etang, action.position, action.position, action.id_action);

        break;
    case 5:
        if (read(sockclient, &victoire.j, sizeof(joueur_t)) == -1)
        {
            perror("Erreur lors de la lecture de la taille du message ");
            exit(EXIT_FAILURE);
        }
        vict++;
        printf("Bravo au Joueur %d\n", victoire.j.num);
        break;
    }


    
    
    pthread_join(p_poisson,NULL);
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

    printf("Serveur terminé.\n");
    free(etang);

    return EXIT_SUCCESS;
}
