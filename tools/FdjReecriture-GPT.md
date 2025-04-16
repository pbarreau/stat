
# Configuration d'expertise - Projet FdjReecriture (GPT)

## ✨ Contexte
Le projet **FdjReecriture (GPT)** est une configuration spécifique de l'assistant ChatGPT visant à :

- Améliorer un projet existant (ex: `fdj-initial`) vers une version modernisée et modulaire (`fdj-gpt`).
- Appliquer les principes d'ingénierie logicielle moderne : SRP, testabilité, clarté de l'architecture.
- Intégrer des fonctions avancées de traitement FDJ : parsing CSV, config dynamique, gestion des historiques.

## 🧠 Expertises activées

| Domaine                         | Capacités activées |
|---------------------------------|------------------------|
| **C++ / Qt 5 / QtCreator**      | Refactoring .h/.cpp, SRP, `QObject`, `QWidget`, `QTest` |
| **Architecture logicielle**    | Modules découplés, namespaces, logique vs interface |
| **Traitement FDJ (CSV)**        | Parsing, tri, structure dynamique des tirages |
| **Configuration dynamique**     | Lecture + transformation `fdj_config.txt` → objets C++ |
| **Base SQLite**                 | Importation automatique des CSV extraits |
| **Tests unitaires**             | Modules testables avec `QTest` |
| **Téléchargement & décompression** | Automatisation à la volée + affichage GUI |
| **Modularité GUI / Core**       | `core/`, `gui/`, `utils/` clairement séparés |

## 🗂️ Structure attendue du projet `fdj-gpt`

```
fdj-gpt/
├── src/
│   ├── core/        # Modules métier (combinatoires, config, parseur...)
│   ├── gui/         # Interface Qt, widgets et fenêtres
│   └── utils/       # Outils annexes (logs, conversions, helpers)
├── tests/           # Tests unitaires (QTest)
├── data/            # ZIPs sources et fichiers CSV extraits
├── output/          # Résultats traités
├── fdj_config.txt   # Configuration hiérarchique FDJ (jeu_x.soustype_y...)
```

## 📊 Règles et conventions

- **Pas d'accents** dans les noms d'identifiants (Qt, console).
- **Un .cpp par .h**, même pour les petites classes.
- **Espaces de nommage clairs** : `Fdj::Core`, `Fdj::Gui`, `Fdj::Utils`...
- **Debug log en anglais** pour cohérence internationale.
- **Réutilisation progressive** du code `fdj-initial` pendant la transition vers `fdj-gpt`.

## 📅 Version active
Cette configuration est active pour toutes les interactions relatives au projet **FdjReecriture (GPT)**. Elle peut être mise à jour à tout moment sur demande.


---


# Consignes de configuration - Projet FdjReecriture (GPT)

## ✨ Objectif
Le projet **FdjReecriture (GPT)** consiste à réécrire le projet initial situé dans l'archive `CodageInitialPourChatGpt.zip`, pour en faire une version moderne, modulaire et maintenable nommée **`fdj-gpt`**.

## 🔹 Sources et contexte
- **Projet initial** : contenu dans `fdj-initial.zip`.
- **Instructions techniques** : présentes dans un fichier `.md` inclus dans `CodageInitialPourChatGpt.zip`.
- **Projet réécrit** : contenu évolutif dans `fdj-gpt.zip`.

## 🔧 Objectifs techniques

1. Modulariser le code
2. Architecture C++/Qt avec SRP
3. Pas d’accents dans les noms
4. Espaces de nommage (ex: `Fdj::Core`)
5. Tests unitaires Qt
6. Parsing dynamique des fichiers `.txt`
7. Téléchargement + décompression automatique
8. Intégration CSV vers SQLite
9. Résumé des opérations en GUI

## 📁 Organisation attendue du projet

```
fdj-gpt/
├── src/
│   ├── core/
│   ├── gui/
│   └── utils/
├── tests/
├── data/
├── output/
├── fdj_config.txt
```

## 🧠 Fonctionnalités à implémenter

- [x] Téléchargement automatique
- [x] Décompression avec création du dossier
- [x] GUI avec résumé de fichier
- [x] Importation SQLite
- [x] Parsing configuré via `fdj_config.txt`
- [x] Tests unitaires Qt

## 🧭 Méthodologie

- Refactorisation progressive
- Fonctionnalités testables individuellement
- Projet final stable, modulaire, maintenable


---


# INSTRUCTIONS TECHNIQUES - Projet FdjReecriture (GPT)

## ✨ Objectif du projet
Le projet **FdjReecriture (GPT)** vise à transformer un code existant (situé dans l'archive `CodageInitialPourChatGpt.zip`) en une version moderne et modulaire, nommée `fdj-gpt`, reposant sur les meilleures pratiques de conception logicielle en C++/Qt.

## 🤖 Rôle attendu de l'assistant
- Assister à la lecture, réorganisation et amélioration du code.
- Produire une architecture claire et testable.
- Répondre aux besoins de l'utilisateur dans une logique de progrès incrémental.
- S’adapter aux objectifs exprimés au fil des itérations.

## 🔧 Consignes techniques principales

1. .h/.cpp pour chaque classe
2. Pas d'accents dans le code
3. Namespaces (`Fdj::Core`, etc.)
4. Structure modulaire (core/gui/utils)
5. Tests QTest
6. Parsing config hiérarchique
7. Décompression auto
8. Importation CSV → SQLite
9. Affichage GUI des opérations
10. SRP obligatoire

## 🗂️ Structure projet recommandée

```
fdj-gpt/
├── src/
│   ├── core/
│   ├── gui/
│   └── utils/
├── tests/
├── data/
├── output/
├── fdj_config.txt
```

## 📅 Processus de transition

- Phase 1 : Analyse + compatibilité ancienne version
- Phase 2 : Introduction de nouveaux modules
- Phase 3 : Remplacement progressif
- Phase 4 : Nettoyage final et documentation

## 📌 Finalité
Ce document remplace le `.md` initialement prévu dans `CodageInitialPourChatGpt.zip` comme base des instructions techniques.
