-Simulation peche deux joueurs
-Etang :
    Taille variable ,un fond bleu
-Poissons :
    Carré jaune , deplacement case en case , attraper si proximité canne , si deux cannes choix aléatoire , generation aléatoire
-Canne a peche :
    Click gauche du joueur , possible a la meme case , vitesse de releve de la canne , poser que toutes les 3secondes

1 point par poisson + gagne de l'argent en fonction du poisson :
    - Valeur des poissons : 1,2,3 , plus chere plus rare
    - Argent : 100 200 500 
    - Type requin : bonus

Avec l'argent , bonus possible : 
    -Pneu (150) : carré Noir visible que par le poseur
        Si un joueur pose sa canne à pêche à côté d’un pneu, au moment où il tente de larelever, 
        il est bloqué pendant 3 secondes (le poisson éventuel s’échappe, le pneu disparaît et lejoueur qui a posé le pneu gagne 1 point sauf si c’est son propre pneu).
    -Dynamite (200)  : carré Rouge invisible pour l'adversaire 
        le joueur pose une dynamite dans l’étang (un carré rouge invisiblepour son adversaire). 
        En explosant, tous les poissons situés dans un carré de 5 par 5 (centrésur la dynamite) meurent et le joueur gagne les points et l’argent associés. 
        Si la ligne d’unjoueur est posée dans le rayon d’action de la dynamite, il est bloqué pendant 3 secondes aprèsl’explosion
    -Requin (300) : carré Vert mais Jaune pour l'adversaire
        Si un joueur peche le requin ,perte de 1 point et 100 argent
    -Furtif (500) : Ne fais plus fuir les poissons tant qu'il n'a pris de piege

Mode fuite , etat d'un poisson: 
    -Evenement sur case : pose/releve de la canne(sauf furtif), pose d'un bonus , poisson fuit de 3 case et ne peux plus etre péché pendant 5 tours ( sauf dynamite)



Debut de reflexion

Etang : 
    -int* Taille


Joueurs :
    -int numero(1/2);
    -int point
    -int argent
    -int etat (peche/attente)

Faire type requete

//Utilisation de union pour le type de message

Mettre en fonction au fur et a mesure
Que faire : 
    -Connexion entre de deux client a un serveur
    -Creation de l'étang
    -Generation des poissons
    -deplacement des poissons
    -Fuite des poissons
    -Peche des poissons
    -Creation des bonus
    -Ajout des bonus sur l'étang




Le main recupere les requetes et reveil dans l'ordre les thread conecrné

il faut que je fasse plusieur varibale condi , une pour chaque thread a reveillé