# Linux

```sh
ssh srvadmin@localhost
```

## Prise en main
Debian trixie (13)
```sh
uname -a   # version du noyau

# debian
cat /etc/debian_version
# alma
cat /etc/almalinux-release 
# rhel/rocky
cat /etc/redhat-release
# all
cat os-release
```

Commandes de base
```sh
cd /etc
pwd    # cuurent directory
ls
ls -l
ls | wc -l # compter
echo $PATH
echo $PWD
```

TODO: declinaison ls

### Gestion des users
Trios:  `useradd`, `usermod`, `userdel` et `groupadd`, `groupmod`, `groupdel`
```sh
sudo useradd matth
sudo userdel -f matth # delete with associated resources
sudo useradd -m -s /usr/bin/bash  matth
sudo passwd matth
    # type 2x password
sudo usermod -s /usr/bin/bash  matth  # modify
sudo usermod -aG sudo matth  # add matth in group sudo
sudo su - matth
```

Infos users
```sh
id
id matth
groups
groups matth
whoami
echo $USER
```

```sh
cat /etc/passwd # users
cat /etc/group  # groupes et membres
cat /etc/shadow # passwords
cat /etc/sudoers # gestion des sudoers
```


Variables d'environnement:
```sh
env
env | sort
env | grep US
```

## Gestion disques
```sh
ls /dev
ls -l
ls -lF # mise en evidence des répertoires
ls -al
ls -ln # uid/gid plutot que username/groupname
ls -dl /usr/bin # que ce répertoire
ls -lh  # taille human readable
ls -lhS # tri taille
ls -lht # tri derniere modif
df
df -h
df -H
lsblk
lsblk -f
du -sh
du -h
tree
```

## Visualiser un fichier
```sh
cat apt.log
more cat apt.log
head -50 apt.log
tail -50 apt.log
less apt.log
```

## Processus / Service
```sh
ps
ps -eaf
ps -aux

pstree

systemctl status ssh
systemctl status nginx
sudo systemctl start|stop|restart|reload nginx  # cycle de vie
sudo systemctl enable|disable nginx # démarrage auto
```

## Editeur de texte
Mode commande
- :q!     : quitter sans sauver
- :w      : write
- :wq     : write and quit
- iIaAoO  : insertion
- G       : Ground (fond du fichier)
- :12     : aller à la ligne 12
- :set nu : afficher les numéros de ligne
- :set nonu : enlever les numéros de ligne
- u         : undo

- d?        : couper
    dd      : 1 ligne
    2dd     : 2 lignes
    d$      : du curseur jusqu'à fin de ligne
    dw      : 1 mot
- y?        : copier (yank)
    yy      : 1 ligne
    2yy     : 2 lignes
    y$      : du curseur jusqu'à fin de ligne
    yw      : 1 mot
- p         : coller 
- `/pattern`  : chercher, puis `n` (next)

Mode insertion : éditer, déplacer (vim)


## Droits classiques
3 fois 3 flags pour 3 catégories
rwxrwxrwx : resp user owner, group owner, others

Détail pour une catégorie

```
---   0 000 (aucun droit)
--x   1 001
-w-   2 010 (mask : retirer write)
-wx   3 011
r--   4 100
r-x   5 101  (exe + directory)
rw-   6 110  (fichier regulier en lecture/ecriture)
rwx   7 111  (tous les droits)
```

Exemples de doits positionnés
755 : rwxr-xr-x  : dossier ou executable
644 : rw-r--r--  : fichier régulier

Exemples de masques : droits retirés à la création
002 : retirer write à la catégorie other
022 : retirer write aux catégorie group owner et other
077 : retirer tous les droits aux catégorie group owner et other

Droit départ avant masque:
- 777 : dossier
- 666 : fichier

Droit final : doit départ - masque

Positionner les droits directements
chmod 755 install.sh


