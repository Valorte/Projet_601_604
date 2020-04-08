#include "Struct.h"
#include "Fonction.h"

int hauteur = 0, largeur = 0;
mutex_fd *fd;
case_t *etang;
canne_t c;
requete_t req;
WINDOW *fenetre_joueur;
WINDOW *fenetre_simulation;
mutex_f *f;

void *affichage(void *args)
{
    int att = 0;
    ncurses_initialisation();
    ncurses_souris();
    ncurses_couleurs();
    wbkgd(stdscr, COLOR_PAIR(5));
    wrefresh(stdscr);

    while (hauteur == 0)
    {
        if (read(fd->fd, &hauteur, sizeof(int)) == -1)
        {
            perror("Erreur lors de la recpetion de la longueur ");
            exit(EXIT_FAILURE);
        }
    }
    while (largeur == 0)
    {
        if (read(fd->fd, &largeur, sizeof(int)) == -1)
        {
            perror("Erreur lors de la reception de la largeur ");
            exit(EXIT_FAILURE);
        }
    }
    etang = malloc(sizeof(case_t) * hauteur * largeur);

    fenetre_joueur = creer_fenetre(5, COLS, 0, 0, 1);
    wbkgd(fenetre_joueur, COLOR_PAIR(1));
    mvwprintw(fenetre_joueur, 0, 1, "Infos");
    scrollok(f->sous_joueur, TRUE);

    f->sous_joueur = creer_sous_fenetre(fenetre_joueur, 4 - 1, COLS - 2, 1, 1);

    fenetre_simulation = creer_fenetre(largeur + 2, hauteur + 3, 5, 0, 1);
    wbkgd(fenetre_simulation, COLOR_PAIR(1));
    mvwprintw(fenetre_simulation, 0, 1, "Peche");
    f->sous_simulation = creer_sous_fenetre(fenetre_simulation, largeur, hauteur, 1, 1);
    wrefresh(f->sous_joueur);
    wrefresh(fenetre_joueur);
    wrefresh(fenetre_simulation);
    wrefresh(f->sous_simulation);

    while (1)
    {
        while (att == 0)
        {
            if ((att = read(fd->fd, etang, sizeof(case_t) * hauteur * largeur)) == -1)
            {
                perror("Erreur lors de la receptionde l'etang ");
                exit(EXIT_FAILURE);
            }
        }
        afficher_etang(etang, largeur, hauteur, f->sous_simulation);
        wrefresh(f->sous_joueur);
        wrefresh(fenetre_joueur);
        wrefresh(fenetre_simulation);
        wrefresh(f->sous_simulation);
        att = 0;
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    reponse_t reponse;
    pthread_t aff;
    joueur_t j;
    struct sockaddr_in adresseServeur;
    int sockfd,i;
    /* Vérification des arguments */
    if (argc != 3)
    {
        fprintf(stderr, "Usage : %s adresseServeur portServeur message\n", argv[0]);
        fprintf(stderr, "Où :\n");
        fprintf(stderr, "  adresseServeur : adresse IPv4 du serveur\n");
        fprintf(stderr, "  portServeur    : numéro de port du serveur\n");
        exit(EXIT_FAILURE);
    }
    fd = malloc(sizeof(mutex_fd));

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

    req.type_requete = TYPE_CONNEXION;

    if (sendto(sockfd, &req, sizeof(requete_t), 0, (struct sockaddr *)&adresseServeur, sizeof(struct sockaddr_in)) == -1)
    { /* code */
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
    if ((fd->fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
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
    if (connect(fd->fd, (struct sockaddr *)&adresseServeur, sizeof(adresseServeur)) == -1)
    {
        perror("Erreur lors de la connexion ");
        exit(EXIT_FAILURE);
    }
    f = malloc(sizeof(mutex_f));

    pthread_mutex_init(&fd->mutex_descripteur, NULL);
    pthread_mutex_init(&f->mutex_fenetre, NULL);
    pthread_create(&aff, NULL, affichage, NULL);
    c.valeur = j.num;
    i=0;
    while (1)
    {
        c.pos = i;
        if (write(fd->fd, &c, sizeof(canne_t)) == -1)
        {
            perror("Erreur lors de l'envoie de la largeur 1 ");
            exit(EXIT_FAILURE);
        }
        sleep(3);
        i++;
    }

    pthread_join(aff, NULL);

    /* Fermeture de la socket */
    if (close(fd->fd) == -1)
    {
        perror("Erreur lors de la fermeture de la socket ");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}