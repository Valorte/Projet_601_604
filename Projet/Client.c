#include "Struct.h"
#include "Fonction.h"

int hauteur = 0, largeur = 0;
mutex_fd *fd;
joueur_t j;
envoie_t env;
case_t *etang;
canne_t c;
requete_t req;
WINDOW *fenetre_joueur;
WINDOW *fenetre_simulation;
WINDOW *fenetre_score;
mutex_f *f;
int x, y;
void *lire_message(void *args)
{
    int retval, t, att = 0;
    fd_set fds;
    envoie_t test;
    struct timeval tv;

    while (1)
    {
        tv.tv_sec = 2;
        tv.tv_usec = 0;
        FD_ZERO(&fds);
        FD_SET(fd->fd, &fds);

        if ((retval = select(fd->fd + 1, &fds, NULL, NULL, &tv)) == -1)
        {
            if (errno != EINTR)
            {
                perror("Serveur : erreur select");
                exit(EXIT_FAILURE);
            }
        }
        if (FD_ISSET(fd->fd, &fds))
        {
            if ((t = read(fd->fd, &test, sizeof(envoie_t))) == -1)
            {
                perror("Reception message");
                exit(EXIT_FAILURE);
            }

            if (test.type_message == TYPE_ETANG && t != 0)
            {
                ncurses_initialisation();
                ncurses_souris();
                ncurses_couleurs();
                wbkgd(stdscr, COLOR_PAIR(5));
                wrefresh(stdscr);
                hauteur = test.objet.taille[0];
                largeur = test.objet.taille[1];

                etang = malloc(sizeof(case_t) * hauteur * largeur);

                fenetre_joueur = creer_fenetre(5, COLS, 0, 0, 1);
                wbkgd(fenetre_joueur, COLOR_PAIR(1));
                mvwprintw(fenetre_joueur, 0, 1, "Infos");

                f->sous_joueur = creer_sous_fenetre(fenetre_joueur, 4 - 1, COLS - 2, 1, 1);
                scrollok(f->sous_joueur, TRUE);

                fenetre_simulation = creer_fenetre(largeur + 2, hauteur + 3, 5, 0, 1);
                wbkgd(fenetre_simulation, COLOR_PAIR(1));
                mvwprintw(fenetre_simulation, 0, 1, "Peche");
                f->sous_simulation = creer_sous_fenetre(fenetre_simulation, largeur, hauteur, 1, 1);

                fenetre_score = creer_fenetre(largeur + 2, hauteur + 8, 5, largeur + 3, 1);
                wbkgd(fenetre_score, COLOR_PAIR(1));
                mvwprintw(fenetre_score, 0, 1, "Score joueur %d", j.num);
                f->sous_score = creer_sous_fenetre(fenetre_score, 10, 10, 1, 1);

                mvwprintw(f->sous_score, 1, 1, "point : %d", j.point);
                wrefresh(f->sous_joueur);
                wrefresh(fenetre_joueur);
                wrefresh(fenetre_simulation);
                wrefresh(f->sous_simulation);
                wrefresh(fenetre_score);
                wrefresh(f->sous_score);
                pthread_mutex_lock(&fd->mutex_descripteur);
                while (att == 0)
                {
                    if ((att = read(fd->fd, etang, sizeof(case_t) * hauteur * largeur)) == -1)
                    {
                        perror("Erreur lors de la receptionde l'etang ");
                        exit(EXIT_FAILURE);
                    }
                }
                pthread_mutex_unlock(&fd->mutex_descripteur);
                pthread_cond_signal(&f->attente);
                t = 0;
                att = 0;
            }
            if (test.type_message == TYPE_MODIF && t != 0)
            {
                while (att == 0)
                {
                    if ((att = read(fd->fd, etang, sizeof(case_t) * hauteur * largeur)) == -1)
                    {
                        perror("Erreur lors de la receptionde l'etang ");
                        exit(EXIT_FAILURE);
                    }
                }

                att = 0;
                t = 0;
            }
            if (test.type_message == TYPE_POISSON && t != 0)
            {
                c.pos = -1;
                j.point += test.objet.p.valeur;
                pthread_mutex_lock(&f->mutex_fenetre);
                mvwprintw(f->sous_score, 1, 1, "point : %d", j.point);
                wrefresh(f->sous_score);
                pthread_mutex_unlock(&f->mutex_fenetre);
            }
        }

        pthread_mutex_lock(&f->mutex_fenetre);
        afficher_etang(etang, largeur, hauteur, f->sous_simulation);
        wrefresh(fenetre_simulation);
        wrefresh(f->sous_simulation);
        if (c.pos >= 0)
        {
            wattron(f->sous_simulation, COLOR_PAIR(5));
            mvwprintw(f->sous_simulation, x, y, " ");
            wrefresh(f->sous_simulation);
        }
        pthread_mutex_unlock(&f->mutex_fenetre);
    }

    return NULL;
}
int main(int argc, char *argv[])
{
    reponse_t reponse;
    pthread_t lire;
    struct sockaddr_in adresseServeur;
    int sockfd, sortie, bouton, position;
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
    c.joueur = j.num;
    c.pos = -1;

    pthread_mutex_init(&fd->mutex_descripteur, NULL);
    pthread_mutex_init(&f->mutex_fenetre, NULL);
    pthread_cond_init(&f->attente, NULL);
    pthread_create(&lire, NULL, lire_message, NULL);

    pthread_mutex_lock(&f->mutex_fenetre);
    pthread_cond_wait(&f->attente, &f->mutex_fenetre);
    pthread_mutex_unlock(&f->mutex_fenetre);
    position = -1;
    while (1)
    {
        pthread_mutex_lock(&f->mutex_fenetre);
        wrefresh(f->sous_joueur);
        pthread_mutex_unlock(&f->mutex_fenetre);
        sortie = getch();

        switch (sortie)
        {
        case KEY_MOUSE:
            if (souris_getpos(&y, &x, &bouton) == OK)
            {
                if (bouton & BUTTON1_CLICKED)
                {
                    x -= 6;
                    y--;

                    if ((y < 0 || x < 0) || (y > largeur || x > hauteur))
                    {
                        pthread_mutex_lock(&f->mutex_fenetre);
                        wprintw(f->sous_joueur, "Cliquez dans la fenetre simulation\n");
                        wrefresh(f->sous_joueur);
                        pthread_mutex_unlock(&f->mutex_fenetre);
                        position = -1;
                    }

                    else
                    {
                        position = x * largeur + y;
                    }
                }
            }
            break;
        }
        if (position >= 0 && position != c.pos)
        {
            pthread_mutex_lock(&f->mutex_fenetre);
            wprintw(f->sous_joueur, "canne posé a  la position %d\n", position);
            wrefresh(f->sous_joueur);
            pthread_mutex_unlock(&f->mutex_fenetre);
            c.pos = position;
            env.objet.c = c;
            env.type_message = TYPE_CANNE;
            pthread_mutex_lock(&fd->mutex_descripteur);
            if (write(fd->fd, &env, sizeof(envoie_t)) == -1)
            {
                perror("Erreur lors de l'envoie de la canne ");
                exit(EXIT_FAILURE);
            }
            pthread_mutex_unlock(&fd->mutex_descripteur);
        }
        else
        {
            if (position >= 0 && position == c.pos)
            {
                c.pos = -1;
                env.objet.c = c;
                env.type_message = TYPE_CANNE_RELEVE;

                pthread_mutex_lock(&f->mutex_fenetre);
                wprintw(f->sous_joueur, "canne relevé");
                wrefresh(f->sous_joueur);
                pthread_mutex_unlock(&f->mutex_fenetre);

                pthread_mutex_lock(&fd->mutex_descripteur);
                if (write(fd->fd, &env, sizeof(envoie_t)) == -1)
                {
                    perror("Erreur lors de l'envoie de la canne ");
                    exit(EXIT_FAILURE);
                }
                pthread_mutex_unlock(&fd->mutex_descripteur);
            }
        }
        sleep(3);
    }

    pthread_join(lire, NULL);

    /* Fermeture de la socket */
    if (close(fd->fd) == -1)
    {
        perror("Erreur lors de la fermeture de la socket ");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}