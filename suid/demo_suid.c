/*
 * demo_suid.c
 *
 * Démonstration pédagogique du bit SUID.
 *
 * Idée : un même binaire a un comportement différent selon que son
 * UID effectif (geteuid) est root ou non, alors que son UID réel
 * (getuid) reste celui de l'utilisateur qui l'a lancé.
 *
 * Scénario :
 *   - Le binaire appartient à root et a le bit SUID positionné
 *     (chmod u+s). Un utilisateur classique qui l'exécute obtient
 *     alors un EUID root pendant l'exécution, sans jamais avoir eu
 *     le mot de passe root : c'est exactement le mécanisme utilisé
 *     par /usr/bin/passwd pour écrire dans /etc/shadow.
 *   - On réalise une action qui nécessite root (écrire dans un
 *     fichier de log appartenant à root, non accessible en écriture
 *     aux autres utilisateurs).
 *   - On montre ensuite qu'on peut abandonner définitivement les
 *     privilèges (setuid(getuid())) et que l'action redevient alors
 *     impossible, même si le binaire est toujours SUID root.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#define LOG_PATH "/var/log/suid_demo.log"

/* Affiche real uid/gid et effective uid/gid, avec les noms associés. */
static void afficher_identites(const char *etape) {
    uid_t ruid = getuid();
    uid_t euid = geteuid();
    gid_t rgid = getgid();
    gid_t egid = getegid();

    /*
     * getpwuid() renvoie un pointeur vers un buffer statique interne,
     * reutilise a chaque appel : on copie donc le nom tout de suite,
     * sinon le deuxieme appel (euid) ecraserait le resultat du premier.
     */
    char nom_reel[64] = "?";
    char nom_effectif[64] = "?";

    struct passwd *pw_r = getpwuid(ruid);
    if (pw_r != NULL) {
        strncpy(nom_reel, pw_r->pw_name, sizeof(nom_reel) - 1);
    }

    struct passwd *pw_e = getpwuid(euid);
    if (pw_e != NULL) {
        strncpy(nom_effectif, pw_e->pw_name, sizeof(nom_effectif) - 1);
    }

    printf("--- %s ---\n", etape);
    printf("  UID reel      : %d (%s)\n", ruid, nom_reel);
    printf("  UID effectif  : %d (%s)\n", euid, nom_effectif);
    printf("  GID reel/eff. : %d / %d\n", rgid, egid);
    printf("\n");
}

/* Action necessitant les droits root : ajouter une ligne a un log root-only. */
static int action_privilegiee(const char *qui) {
    FILE *f = fopen(LOG_PATH, "a");
    if (f == NULL) {
        fprintf(stderr, "  [ECHEC] Impossible d'ecrire dans %s : %s\n",
                LOG_PATH, strerror(errno));
        return -1;
    }

    time_t now = time(NULL);
    char horodatage[64];
    strftime(horodatage, sizeof(horodatage), "%Y-%m-%d %H:%M:%S", localtime(&now));

    fprintf(f, "[%s] action privilegiee executee pour l'utilisateur reel : %s\n",
            horodatage, qui);
    fclose(f);

    printf("  [OK] Ligne ajoutee dans %s\n", LOG_PATH);
    return 0;
}

int main(void) {
    uid_t ruid = getuid();

    /* Copie immediate du nom (cf. remarque dans afficher_identites) : le
     * buffer statique de getpwuid() serait sinon ecrase par les appels
     * suivants, faussant le nom ecrit dans le fichier de log. */
    char nom_utilisateur_reel[64] = "inconnu";
    struct passwd *pw_r = getpwuid(ruid);
    if (pw_r != NULL) {
        strncpy(nom_utilisateur_reel, pw_r->pw_name, sizeof(nom_utilisateur_reel) - 1);
    }

    afficher_identites("Au demarrage du programme");

    if (geteuid() == 0) {
        /*
         * EUID root : soit le programme est lance directement par
         * root, soit (cas interessant) il est SUID root et a ete
         * lance par un utilisateur classique. Dans les deux cas,
         * on peut faire l'action privilegiee.
         */
        printf("EUID = 0 : droits root actifs (via SUID ou lancement direct par root).\n");
        printf("-> Execution de l'action privilegiee...\n");
        action_privilegiee(nom_utilisateur_reel);

        /*
         * Abandon definitif et irreversible des privileges : on aligne
         * l'EUID sur l'UID reel. Comme getuid() == root ici est
         * improbable pour un utilisateur classique, cela retombe
         * sur l'UID de l'utilisateur qui a lance le binaire SUID.
         */
        printf("\nAbandon volontaire des privileges (setuid(getuid()))...\n");
        if (setuid(ruid) != 0) {
            perror("setuid");
            return EXIT_FAILURE;
        }
        afficher_identites("Apres abandon des privileges");

        printf("Nouvelle tentative de l'action privilegiee (doit echouer desormais) :\n");
        action_privilegiee(nom_utilisateur_reel);
    } else {
        /*
         * EUID non-root : soit le binaire n'a pas le bit SUID, soit
         * il appartient a un utilisateur non-root. L'action
         * privilegiee doit echouer naturellement (permission refusee
         * par le systeme de fichiers), ce qui illustre le comportement
         * "utilisateur classique sans SUID actif".
         */
        printf("EUID != 0 : pas de droits root (bit SUID absent ou binaire non root).\n");
        printf("-> Tentative de l'action privilegiee (echec attendu) :\n");
        action_privilegiee(nom_utilisateur_reel);
    }

    return EXIT_SUCCESS;
}
