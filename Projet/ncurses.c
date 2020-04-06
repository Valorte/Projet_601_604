#include "ncurses.h"

/**
 * Initialisation de ncurses.
 */
void ncurses_initialisation()
{
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    refresh();
    curs_set(FALSE);
}

/**
 * Fin de ncurses.
 */
void ncurses_stopper()
{
    endwin();
}

/**
 * Initialisation des couleurs.
 */
void ncurses_couleurs()
{
    if(has_colors() == FALSE)
    {
        ncurses_stopper();
        fprintf(stderr, "Le terminal ne supporte pas les couleurs.\n");
        exit(EXIT_FAILURE);
    }

    start_color();
 
    init_pair(1, COLOR_BLUE, COLOR_WHITE);
    init_pair(2, COLOR_GREEN, COLOR_GREEN);
    init_pair(3, COLOR_BLUE, COLOR_WHITE);
    init_pair(4, COLOR_WHITE, COLOR_WHITE);
}

/**
 * Initialisation de la souris.
 */
void ncurses_souris() 
{
    if(!mousemask(ALL_MOUSE_EVENTS, NULL)) {
        ncurses_stopper();
        fprintf(stderr, "Erreur lors de l'initialisation de la souris.\n");
        exit(EXIT_FAILURE);
    }

    if(has_mouse() != TRUE) {
        ncurses_stopper();
        fprintf(stderr, "Aucune souris n'est détectée.\n");
        exit(EXIT_FAILURE);
    }
}

/**
 * Position de la souris
 */
int souris_getpos(int *x, int *y, int *bouton) 
{
    MEVENT event;
    int resultat = getmouse(&event);
    
    if(resultat == OK) {
        *x = event.x;
        *y = event.y;
        if(bouton != NULL) *bouton = event.bstate;
    }

    return resultat;
}

/**
 * Création d'une fenêtre
 */
WINDOW* creer_fenetre(int hauteur, int largeur, int y, int x, int boole)
{
    WINDOW* fenetre = newwin(hauteur, largeur, y, x);

    if (boole == 1)
    {
        box(fenetre, 0, 0);
    }

    return fenetre;
}

/**
 * Création d'une sous-fenêtre
 */
WINDOW* creer_sous_fenetre(WINDOW* fenetre, int hauteur, int largeur, int y, int x)
{
  WINDOW* sous_fenetre = derwin(fenetre, hauteur, largeur, y, x);
 
  return sous_fenetre;
}

int obstacle(unsigned char tab[15][30], int i , int j)
{
  int repo = 0; /*ok*/

  if(tab[i + 1][j] != '0')
  {
    if((tab[i][j - 1] != '0' && tab[i][j + 1] != '0') || (j > 30 || j < 0))
    {
      repo = -2; /*bloqué*/
    }
    
    if(tab[i][j - 1] == '0' && tab[i + 1][j - 1] != '0')
    {
      repo = -2; /*bloqué*/
    }

    if(tab[i][j + 1] == '0' && tab[i + 1][j + 1] != '0')
    {
      repo = -2; /*bloqué*/
    }

    if(tab[i][j - 1] == '0' && tab[i + 1][j - 1] == '0')
    {
      repo = -1; /*gauche*/
    }

    if(tab[i][j + 1] == '0' && tab[i + 1][j + 1] == '0')
    {
      repo = 1; /*droite*/
    }  
  }

  return repo;
}