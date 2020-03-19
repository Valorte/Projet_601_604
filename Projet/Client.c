#include "Struct.h"
#include "Fonction.h"
int main(int argc, char *argv[])
{
    joueur_t j;
    int sockfd,i=0;
    struct sockaddr_in adresseServeur;
    char msg[25];
    /* Vérification des arguments */
    if (argc != 4)
    {
        fprintf(stderr, "Usage : %s adresseServeur portServeur message\n", argv[0]);
        fprintf(stderr, "Où :\n");
        fprintf(stderr, "  adresseServeur : adresse IPv4 du serveur\n");
        fprintf(stderr, "  portServeur    : numéro de port du serveur\n");
        fprintf(stderr, "  message        : le message à envoyer\n");
        exit(EXIT_FAILURE);
    }

    /* Vérification de la taille du message */
    if (strlen(argv[3]) >= 25)
    {
        fprintf(stderr, "La taille du message est trop grande. Elle doit fait maximum 25 caractères.");
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

    /* Préparation du message */
    memset(msg, 0, 25);
    strcpy(msg, argv[3]);

    envoie_message(sockfd,msg,adresseServeur);


    if (recvfrom(sockfd, &i, sizeof(i), 0,NULL,NULL) == -1)
    {
        perror("Erreur lors de la réception du message ");
        exit(EXIT_FAILURE);
    }

    printf("Joueur connecté\n");
    
    init_joueur(i,&j);
    

    printf("tu es le joueur %d\nTu commences avec %d point et %d argent",j.num,j.point,j.argent);

    /* Fermeture de la socket */
    if (close(sockfd) == -1)
    {
        perror("Erreur lors de la fermeture de la socket ");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}