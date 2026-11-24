# Démonstration du bit SUID

Programme C pédagogique illustrant la différence entre UID réel
(`getuid`) et UID effectif (`geteuid`), et montrant comment le bit
SUID permet à un utilisateur classique d'obtenir temporairement les
droits root pour une action précise.

Le programme tente d'ajouter une ligne dans `/var/log/suid_demo.log`
(fichier appartenant à root) :

- si l'**EUID** vaut 0 (root direct, ou SUID root actif), l'action
  réussit, puis le programme abandonne volontairement ses privilèges
  (`setuid(getuid())`) et retente l'action, qui échoue alors ;
- si l'**EUID** n'est pas 0 (binaire lancé par un utilisateur
  classique sans bit SUID), l'action échoue directement
  (`Permission denied`).

## Compilation

```bash
gcc -Wall -o demo_suid demo_suid.c
```

## Test SANS le bit SUID (comportement "utilisateur classique")

```bash
./demo_suid
```

→ L'EUID est celui de l'utilisateur courant, l'écriture dans
`/var/log/suid_demo.log` échoue (permission refusée).

## Test AVEC le bit SUID (comportement "root via SUID")

```bash
# en root ou avec sudo :
sudo chown root:root demo_suid
sudo chmod u+s demo_suid

# vérification du bit SUID (un 's' à la place du 'x' du propriétaire) :
ls -l demo_suid
# -rwsr-xr-x 1 root root ...

# préparer le fichier de log (créé par root, pas écrit par les autres) :
sudo touch /var/log/suid_demo.log
sudo chmod 600 /var/log/suid_demo.log
```

Puis, en tant qu'utilisateur classique (pas root) :

```bash
./demo_suid
```

→ `getuid()` reste celui de l'utilisateur classique, mais
`geteuid()` vaut 0 : l'écriture dans le log réussit grâce au SUID.
Le programme abandonne ensuite ses privilèges et montre que la
même action échoue de nouveau une fois l'EUID ramené à l'UID réel.

## Nettoyage

```bash
sudo rm -f /var/log/suid_demo.log
```
