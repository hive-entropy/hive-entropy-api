# Hive-Entropy API : Bibliothèque de distribution de calculs sur un réseau Hive-Entropy

Ce projet est réalisé dans le cadre d'un projet universitaire à [CY Tech](https://cytech.cyu.fr/), dans le cadre de notre [Master 2 IISC](https://depinfo.u-cergy.fr/master). L'objectif de ce projet est de couvrir un large éventail de connaissances et technologies, pour nous permettre de mettre en pratique nos apprentissages acquis au cours de notre formation.

Dans cet esprit, notre sujet concerne la distribution de puissance de calcul au sein d'un réseau d'appareils connectés. Ce programme correspond plus précisément à une bibliothèque transversale à notre projet, nous permettant de fournir les outils nécessaires la bonne exploitation d'un réseau de distribution de calcul.

Notre équipe est consituée des membres suivants:

- [OTTAVIANO Aurélien](https://www.github.com/SevenMoor/) (Chef de projet)
- [LE MOULT Alban](https://www.github.com/AlbanLM/)
- [CHEVALLIER Gabriel](https://www.github.com/GabrielChev/)
- [PAUQUET Jean-Philippe](https://www.github.com/pauquetj/)

## Comprendre Hive-Entropy

**TODO** Décrire l'architecture fonctionnelle du projet

## Fonctionnalités couvertes

Cette bibliothèque étant le coeur de notre projet, elle permet de couvrir à différents niveaux d'abstraction les fonctionnalités suivantes:

- Définition d'un callback pour la réception de tâches
- Définir les critères d'acceptation ou de refus pour fournir de la puissance de calcul
- Interroger les caractéristiques matérielles du noeud (batterie, taux d'occupation du processeur, modèles des composants, etc.)
- Distribuer les types de calculs supportés
- Définir les contraintes de temps à la réalisation d'un calcul
- Interroger le réseau pour connaître les noeuds disponibles
- Définir un callback pour la reconstruction du résultat
- Création de breakpoints dans le programme pour attendre la reconstruction complète du résultat là où sa valeur est requise

Tous les aspects de programmation concurrente, réseau/protocoles, découpage des tâches, décision des noeuds à utiliser et de l'ordre des calculs sont pris en charge par l'implémentation de la bibliothèque.

## Installation

### Dépendances
#### OpenBLAS
```
git clone --depth 1 --branch v0.3.14 git@github.com:xianyi/OpenBLAS.git
cd OpenBLAS/
cmake -Bbuild/ -S.
[sudo] cmake --build build/ --target install 
```

#### Libcoap
```
git clone --depth 1 --branch v4.3.0-rc3 git@github.com:obgm/libcoap.git
cd libcoap/
cmake -Bbuild/ -S. -DENABLE_DTLS=OFF -DENABLE_TCP=OFF -DUSE_VENDORED_TINYDTLS=OFF
[sudo] cmake --build build/ --target install
```

## Utilisation

**TODO** Décrire la compilation, l'installation, et les étapes à réaliser pour pouvoir utiliser la bibliothèque

#### Initialiser le projet CMake
```
cmake -Bbuild/ -S.
```

#### Lancer la compilation/construction
```
cmake --build build/
```

#### Lancer les tests
```
cmake --build build/ --target HiveEntropyTest
./build/tests/HiveEntropyTest
```

#### Installer la bibliothèque
```
[sudo] cmake --build build/ --target install
```

## Exemple

**TODO** Définir un petit programme d'exemple
