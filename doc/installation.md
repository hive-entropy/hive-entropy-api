# Installation

## Dépendances
### OpenBLAS
```
git clone --depth 1 --branch v0.3.14 git@github.com:xianyi/OpenBLAS.git
cd OpenBLAS/
cmake -Bbuild/ -S.
sudo cmake --build build/ --target install 
```

À noter que l'option `-DBUILD_WITHOUT_LAPACK=ON` peut être passée lors de la première commande CMake pour éviter la compilation de LAPACK. Cependant cette option crée une erreur si aucun compilateur fortran n'est présent sur le système.

Sur Raspberry Pi 3, les options définies dans le CMakeList.txt du projet OpenBLAS ne sont pas forcément adaptées.
L'option `-DCCOMMON_OPT="-mfpu=vfpv3 -march=armv7-a -mfpu=neon -lpthread"` doit être ajoutée pour pouvoir compiler correctement.

### Libcoap
```
git clone --depth 1 --branch v4.3.0-rc3 git@github.com:obgm/libcoap.git
cd libcoap/
cmake -Bbuild/ -S. -DENABLE_DTLS=OFF -DENABLE_TCP=OFF
sudo cmake --build build/ --target install
```

## Bibliothèque

### Initialiser le projet CMake
```
cmake -Bbuild/ -S.
```

### Lancer la compilation/construction
```
cmake --build build/
```

### Lancer les tests
```
cmake --build build/ --target HiveEntropyTest
./build/tests/HiveEntropyTest
```

### Installer la bibliothèque
```
[sudo] cmake --build build/ --target install
```

### Générer la documentation
Dépendances nécessaires :
* doxygen
* python-sphinx
* python-sphinx_rtd_theme
* python-breathe
* python-myst-parser

Exécuter les cibles doxygen, puis la cible sphinx.
```bash
cmake --build build/ --target Doxygen
cmake --build build/ --target Sphinx
```

### Accéder à la documentation

Chemin pour la documentation Doxygen : `build/doc/doxygen/html/index.html`

Chemin pour la documentation Sphinx : `build/doc/sphinx/index.html`