.. HiveEntropyAPI documentation master file, created by
   sphinx-quickstart on Sat Jun 19 21:05:33 2021.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

HiveEntropyAPI : Bibliothèque de distribution de calculs sur un réseau Hive Entropy
=====================================================================================

.. toctree::
   :maxdepth: 2
   :caption: Contenu :

   installation
   usage
   references/api_reference

Introduction
------------

Ce projet est réalisé dans le cadre d'un projet universitaire à `CY Tech <https://cytech.cyu.fr/>`_, dans le cadre de notre `Master 2 IISC <https://depinfo.u-cergy.fr/master>`_. L'objectif de ce projet est de couvrir un large éventail de connaissances et technologies, pour nous permettre de mettre en pratique nos apprentissages acquis au cours de notre formation.

Dans cet esprit, notre sujet concerne la distribution de puissance de calcul au sein d'un réseau d'appareils connectés. Ce programme correspond plus précisément à une bibliothèque transversale à notre projet, nous permettant de fournir les outils nécessaires la bonne exploitation d'un réseau de distribution de calcul.

Notre équipe est constituée des membres suivants:

* `OTTAVIANO Aurélien <https://www.github.com/SevenMoor/>`_ (Chef de projet)
* `LE MOULT Alban <https://www.github.com/AlbanLM/>`_
* `CHEVALLIER Gabriel <https://www.github.com/GabrielChev/>`_
* `PAUQUET Jean-Philippe <https://www.github.com/pauquetj/>`_

Comprendre Hive-Entropy
'''''''''''''''''''''''

**TODO** Décrire l'architecture fonctionnelle du projet

Fonctionnalités couvertes
'''''''''''''''''''''''''

Cette bibliothèque étant le coeur de notre projet, elle permet de couvrir à différents niveaux d'abstraction les fonctionnalités suivantes:

* Définition d'un callback pour la réception de tâches
* Définir les critères d'acceptation ou de refus pour fournir de la puissance de calcul
* Interroger les caractéristiques matérielles du nœud (batterie, taux d'occupation du processeur, modèles des composants, etc.)
* Distribuer les types de calculs supportés
* Définir les contraintes de temps à la réalisation d'un calcul
* Interroger le réseau pour connaître les nœuds disponibles
* Définir un callback pour la reconstruction du résultat
* Création de breakpoints dans le programme pour attendre la reconstruction complète du résultat là où sa valeur est requise

Tous les aspects de programmation concurrente, réseau/protocoles, découpage des tâches, décision des nœuds à utiliser et de l'ordre des calculs sont pris en charge par l'implémentation de la bibliothèque.