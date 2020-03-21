#include "Fonction.h"

int vict=0;
int *etang;
int longueur, largeur ,nbpoisson;

void* create_poisson(void* args){
    poisson_t* tableau=(poisson_t*)args;
    while (nbpoisson < MAX_POISSON)
    {
        init_poisson(&tableau[nbpoisson],nbpoisson);
        nbpoisson++;
    }
    
    return NULL;
}

void* routine_poisson(void* args){
    int i=0,p;
    poisson_t* tmp = (void*)args;
    while (i!=5)
    {   
        
        modif_etang(etang,p,tmp->pos,tmp->valeur);
        afficher_etang(etang,largeur,longueur);
        printf("\n");
        i++;
        sleep(1);
    }
    
    return NULL;
}


int main(int argc, char *argv[])
{
    pthread_t p_poisson;
    poisson_t p;
    reponse_t reponse[2];
    requete_t requete;
    action_t action;
    victoire_t victoire;
    file_t file;
    int sockfd, i = 0, fd, sockclient, shmid, j, k = 0;
    long type;
    struct sockaddr_in adresseServeur;
    struct sockaddr_in adresseClient[2];
    socklen_t longueurAdresse;

    /* Récupération des arguments */
    if (argc != 5)
    {
        fprintf(stderr, "Usage : %s port longueur largeur cle\n", argv[0]);
        fprintf(stderr, "Où :\n");
        fprintf(stderr, "  port           : port d'écoute du serveur\n");
        fprintf(stderr, "  longueur       : longueur de l'etang\n");
        fprintf(stderr, "  largeur        : largeur de l'etang\n");
        fprintf(stderr, "  cle            : segment memoire\n");
        exit(EXIT_FAILURE);
    }
    file.indice_queue = 0;
    file.indice_tete = 0;
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
        ajouter_requete(&file, &requete);
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

    /* Fermeture de la socket */
    if (close(sockfd) == -1)
    {
        perror("Erreur lors de la fermeture de la socket ");
        exit(EXIT_FAILURE);
    }
    longueur = atoi(argv[2]);
    largeur = atoi(argv[3]);
    /*Segment memoire partagé*/
    if ((shmid = shmget((key_t)atoi(argv[4]), sizeof(int) * (longueur * largeur), S_IRUSR | S_IWUSR | IPC_CREAT | IPC_EXCL)) == -1)
    {
        if (errno == EEXIST)
            fprintf(stderr, "Le segment de mémoire partagée (cle=%d) existe deja\n", atoi(argv[4]));
        else
            perror("Erreur lors de la création du segment de mémoire ");
        exit(EXIT_FAILURE);
    };

    /* Attachement du segment de mémoire partagée */
    if ((etang = shmat(shmid, NULL, 0)) == (void *)-1)
    {
        perror("Erreur lors de l'attachement du segment de mémoire partagée ");
        exit(EXIT_FAILURE);
    }
    for (i = 0; i < longueur; i++)
    {
        for (j = 0; j < largeur; j++)
        {
            etang[k] = 0;
            k++;
        }
    }
    afficher_etang(etang, largeur, longueur);

    /* Création de la socket */
    if ((fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
    {
        perror("Erreur lors de la création de la socket ");
        exit(EXIT_FAILURE);
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
    if (listen(fd, 1) == -1)
    {
        perror("Erreur lors de la mise en mode passif ");
        exit(EXIT_FAILURE);
    }
    init_poisson(&p,0);

    pthread_create(&p_poisson,NULL,routine_poisson,&p);

    /* Attente d'une connexion */
    printf("Serveur : attente de connexion...\n");
    if ((sockclient = accept(fd, NULL, NULL)) == -1)
    {
        perror("Erreur lors de la demande de connexion ");
        exit(EXIT_FAILURE);
    }

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
        
        modif_etang(etang,action.position,action.position,action.id_action);
        
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

    /* Suppression du segment de memoire partagee */
    if (shmctl(shmid, IPC_RMID, 0) == -1)
    {
        perror("Erreur lors de la suppression du segment de memoire partagee ");
        exit(EXIT_FAILURE);
    }

    printf("Serveur terminé.\n");

    return EXIT_SUCCESS;
}
