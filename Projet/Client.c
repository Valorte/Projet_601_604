#include "Struct.h"
#include "Fonction.h"

int fd, hauteur = 0, largeur = 0;
case_t *etang;
WINDOW *fenetre_joueur;
WINDOW *sous_fenetre_joueur;
WINDOW *fenetre_simulation;
WINDOW *sous_fenetre_simulation;
void *affichage(void *args)
{
    int test = 0;
    ncurses_initialisation();
    ncurses_souris();
    ncurses_couleurs();

    while (hauteur == 0)
    {

        if (read(fd, &hauteur, sizeof(int)) == -1)
        {
            perror("Erreur lors de la recpetion de la hauteur ");
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

    /**
     * Fenêtre information
     */
    fenetre_joueur = creer_fenetre(5, COLS, 0, 0, 1);
    wbkgd(fenetre_joueur, COLOR_PAIR(1));
    mvwprintw(fenetre_joueur, 0, 1, "Infos");

    sous_fenetre_joueur = creer_sous_fenetre(sous_fenetre_joueur, 4 - 1, COLS - 2, 1, 1);
    /**
     * Fenêtre simulation
     */
    fenetre_simulation = creer_fenetre(15 + 2, 30 + 3, 5, 0, 1);
    wbkgd(fenetre_simulation, COLOR_PAIR(1));
    mvwprintw(fenetre_simulation, 0, 1, "Peche");
    sous_fenetre_simulation = creer_sous_fenetre(fenetre_simulation, 15, 30, 1, 1);
    etang = malloc(sizeof(case_t) * (hauteur * largeur));
    while (1)
    {
        while (test == 0)
        {

            if ((test = read(fd, etang, sizeof(case_t) * hauteur * largeur)) == -1)
            {
                perror("Erreur lors de la reception de la largeur ");
                exit(EXIT_FAILURE);
            }
        }
        afficher_etang(etang, largeur, hauteur, sous_fenetre_simulation);
        wrefresh(fenetre_simulation);
        wrefresh(sous_fenetre_simulation);

        test = 0;
    }

    return NULL;
}

int main(int argc, char *argv[])
{
    requete_t connex;
    reponse_t reponse;
    pthread_t aff;
    joueur_t j;
    struct sockaddr_in adresseServeur;
    int sockfd;
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

    pthread_create(&aff, NULL, affichage, NULL);


    pthread_join(aff, NULL);

    /* Fermeture de la socket */
    if (close(fd) == -1)
    {
        perror("Erreur lors de la fermeture de la socket ");
        exit(EXIT_FAILURE);
    }

    delwin(fenetre_simulation);
    delwin(sous_fenetre_simulation);
    ncurses_stopper();

    return EXIT_SUCCESS;
}