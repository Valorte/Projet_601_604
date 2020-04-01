#include "Struct.h"
#include "Fonction.h"
int main(int argc, char *argv[])
{
    requete_t connex;
    reponse_t reponse;
    action_t bonus;
    action_t canne;
    joueur_t j;
    case_t *etang;
    int sockfd, fd, longueur = 0, largeur = 0, choix;
    struct sockaddr_in adresseServeur;
    /* Vérification des arguments */
    if (argc != 3)
    {
        fprintf(stderr, "Usage : %s adresseServeur portServeur message\n", argv[0]);
        fprintf(stderr, "Où :\n");
        fprintf(stderr, "  adresseServeur : adresse IPv4 du serveur\n");
        fprintf(stderr, "  portServeur    : numéro de port du serveur\n");
        exit(EXIT_FAILURE);
    }

    /* Création de la socket */
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        perror("Erreur lors de la création de la socket ");
        exit(EXIT_FAILURE);
    }

    /* Création de l'adresse du serveur */
    memset(&adresseServeur, 0, sizeof(struct sockaddr_in));
    adresseServeur.sin_family = AF_INET;
    adresseServeur.sin_port = htons(atoi(argv[2]));
    if (inet_pton(AF_INET, argv[1], &adresseServeur.sin_addr) != 1)
    {
        perror("Erreur lors de la conversion de l'adresse ");
        exit(EXIT_FAILURE);
    }

    connex.type = TYPE_CONNEXION;

    if (sendto(sockfd, &connex, sizeof(requete_t), 0, (struct sockaddr *)&adresseServeur, sizeof(struct sockaddr_in)) == -1)
    {
        perror("Erreur lors de l'envoi du message ");
        exit(EXIT_FAILURE);
    }

    if (recvfrom(sockfd, &reponse, sizeof(reponse_t), 0, NULL, NULL) == -1)
    {
        perror("Erreur lors de la réception du message ");
        exit(EXIT_FAILURE);
    }

    init_joueur(reponse.id, &j);

    printf("Je suis le joueur : %d\n", j.num);

    /* Fermeture de la socket */
    if (close(sockfd) == -1)
    {
        perror("Erreur lors de la fermeture de la socket ");
        exit(EXIT_FAILURE);
    }

    /* Création de la socket */
    if ((fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
    {
        perror("Erreur lors de la création de la socket ");
        exit(EXIT_FAILURE);
    }

    /* Remplissage de la structure */
    memset(&adresseServeur, 0, sizeof(struct sockaddr_in));
    adresseServeur.sin_family = AF_INET;
    adresseServeur.sin_port = htons(atoi(reponse.port));
    if (inet_pton(AF_INET, argv[1], &adresseServeur.sin_addr.s_addr) != 1)
    {
        perror("Erreur lors de la conversion de l'adresse ");
        exit(EXIT_FAILURE);
    }

    /* Connexion au serveur */
    if (connect(fd, (struct sockaddr *)&adresseServeur, sizeof(adresseServeur)) == -1)
    {
        perror("Erreur lors de la connexion ");
        exit(EXIT_FAILURE);
    }
    /*recevoir_info(fd,etang,longueur,largeur);*/

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
    printf("\nLongueur : %d , largeur : %d\n", longueur, largeur);

    etang = malloc(sizeof(case_t) * (longueur * largeur));

    if (read(fd, etang, sizeof(case_t) * longueur * largeur) == -1)
    {
        perror("Erreur lors de la reception de la largeur ");
        exit(EXIT_FAILURE);
    }

    afficher_etang(etang, largeur, longueur);

    printf("On mets un bonus a la case 6");

    bonus.type = TYPE_BONUS;
    bonus.position = 6;
    bonus.id_action = 8;

    if (write(fd, &bonus.type, sizeof(long)) == -1)
    {
        perror("Erreur lors de l'envoie de la largeur 1 ");
        exit(EXIT_FAILURE);
    }
    if (write(fd, &bonus, sizeof(action_t)) == -1)
    {
        perror("Erreur lors de l'envoie de la largeur 1 ");
        exit(EXIT_FAILURE);
    }

    while (1)
    {

        printf("Où placer votre ameçon ?\n");
        if(scanf("%d", &choix)==-1){
            exit(EXIT_FAILURE);
        }

        canne.type = TYPE_CANNE;
        canne.id_action = j.num;
        canne.position = choix;

        if (write(fd, &canne.type, sizeof(long)) == -1)
        {
            perror("Erreur lors de l'envoie de l'etang 1 ");
            exit(EXIT_FAILURE);
        }

        if (write(fd, &canne, sizeof(action_t)) == -1)
        {
            perror("Erreur lors de l'envoie de l'etang 1 ");
            exit(EXIT_FAILURE);
        }
        printf("\n");

        while (read(fd, etang, sizeof(case_t) * longueur * largeur) == 0)
        {
            if (read(fd, etang, sizeof(case_t) * longueur * largeur) == -1)
            {
                perror("Erreur lors de la reception de la largeur ");
                exit(EXIT_FAILURE);
            }
        }
        afficher_etang(etang, largeur, longueur);
    }

    /* Fermeture de la socket */
    if (close(fd) == -1)
    {
        perror("Erreur lors de la fermeture de la socket ");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}