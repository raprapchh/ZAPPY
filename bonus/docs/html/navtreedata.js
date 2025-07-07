/*
 @licstart  The following is the entire license notice for the JavaScript code in this file.

 The MIT License (MIT)

 Copyright (C) 1997-2020 by Dimitri van Heesch

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 and associated documentation files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge, publish, distribute,
 sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies or
 substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 @licend  The above is the entire license notice for the JavaScript code in this file
*/
var NAVTREE =
[
  [ "Zappy Project", "index.html", [
    [ "🌟 Zappy - Documentation Développeur", "index.html", "index" ],
    [ "Architecture du Serveur Zappy", "md_doc_2zappy-server.html", [
      [ "Vue d'ensemble", "md_doc_2zappy-server.html#autotoc_md1", null ],
      [ "Architecture modulaire", "md_doc_2zappy-server.html#autotoc_md2", null ],
      [ "Structures de données principales", "md_doc_2zappy-server.html#autotoc_md3", [
        [ "Structure Server", "md_doc_2zappy-server.html#autotoc_md4", null ],
        [ "Structure Client", "md_doc_2zappy-server.html#autotoc_md5", null ]
      ] ],
      [ "Gestion du temps et des événements", "md_doc_2zappy-server.html#autotoc_md6", [
        [ "Concept temporel", "md_doc_2zappy-server.html#autotoc_md7", null ],
        [ "Event Queue", "md_doc_2zappy-server.html#autotoc_md8", null ],
        [ "Flow d'exécution", "md_doc_2zappy-server.html#autotoc_md9", null ]
      ] ],
      [ "Module Network", "md_doc_2zappy-server.html#autotoc_md10", [
        [ "Buffer circulaire", "md_doc_2zappy-server.html#autotoc_md11", null ],
        [ "Gestion des commandes", "md_doc_2zappy-server.html#autotoc_md12", null ]
      ] ],
      [ "Module World", "md_doc_2zappy-server.html#autotoc_md13", [
        [ "Map toroïdale", "md_doc_2zappy-server.html#autotoc_md14", null ],
        [ "Tile structure", "md_doc_2zappy-server.html#autotoc_md15", null ],
        [ "Spawn des ressources", "md_doc_2zappy-server.html#autotoc_md16", null ]
      ] ],
      [ "Module Game", "md_doc_2zappy-server.html#autotoc_md17", [
        [ "Système d'élévation", "md_doc_2zappy-server.html#autotoc_md18", null ],
        [ "Gestion des œufs", "md_doc_2zappy-server.html#autotoc_md19", null ]
      ] ],
      [ "Optimisations importantes", "md_doc_2zappy-server.html#autotoc_md20", [
        [ "Spatial indexing", "md_doc_2zappy-server.html#autotoc_md21", null ],
        [ "Memory pools", "md_doc_2zappy-server.html#autotoc_md22", null ],
        [ "Zero-copy networking", "md_doc_2zappy-server.html#autotoc_md23", null ],
        [ "Time management", "md_doc_2zappy-server.html#autotoc_md24", null ]
      ] ],
      [ "Gestion des erreurs", "md_doc_2zappy-server.html#autotoc_md25", [
        [ "Hiérarchie d'erreurs", "md_doc_2zappy-server.html#autotoc_md26", null ],
        [ "Recovery patterns", "md_doc_2zappy-server.html#autotoc_md27", null ]
      ] ],
      [ "Points d'attention critiques", "md_doc_2zappy-server.html#autotoc_md28", [
        [ "Thread safety", "md_doc_2zappy-server.html#autotoc_md29", null ],
        [ "Integer overflow", "md_doc_2zappy-server.html#autotoc_md30", null ],
        [ "Resource leaks", "md_doc_2zappy-server.html#autotoc_md31", null ],
        [ "Network edge cases", "md_doc_2zappy-server.html#autotoc_md32", null ]
      ] ],
      [ "Tests spécifiques serveur", "md_doc_2zappy-server.html#autotoc_md33", [
        [ "Tests unitaires", "md_doc_2zappy-server.html#autotoc_md34", null ],
        [ "Tests d'intégration", "md_doc_2zappy-server.html#autotoc_md35", null ],
        [ "Tests de stress", "md_doc_2zappy-server.html#autotoc_md36", null ],
        [ "Tests de robustesse", "md_doc_2zappy-server.html#autotoc_md37", null ]
      ] ],
      [ "Debugging et monitoring", "md_doc_2zappy-server.html#autotoc_md38", [
        [ "Logs structurés", "md_doc_2zappy-server.html#autotoc_md39", null ],
        [ "Métriques importantes", "md_doc_2zappy-server.html#autotoc_md40", null ],
        [ "Debug mode", "md_doc_2zappy-server.html#autotoc_md41", null ]
      ] ],
      [ "Checklist de développement", "md_doc_2zappy-server.html#autotoc_md42", null ]
    ] ],
    [ "Architecture du Client GUI Zappy", "md_doc_2zappy-gui.html", [
      [ "Vue d'ensemble", "md_doc_2zappy-gui.html#autotoc_md44", null ],
      [ "Architecture MVC", "md_doc_2zappy-gui.html#autotoc_md45", null ],
      [ "Structure des modules", "md_doc_2zappy-gui.html#autotoc_md46", null ],
      [ "Classes principales", "md_doc_2zappy-gui.html#autotoc_md47", [
        [ "Application", "md_doc_2zappy-gui.html#autotoc_md48", null ],
        [ "World (Model)", "md_doc_2zappy-gui.html#autotoc_md49", null ],
        [ "Renderer", "md_doc_2zappy-gui.html#autotoc_md50", null ]
      ] ],
      [ "Système de rendu", "md_doc_2zappy-gui.html#autotoc_md51", [
        [ "Architecture en couches", "md_doc_2zappy-gui.html#autotoc_md52", null ],
        [ "Optimisations de rendu", "md_doc_2zappy-gui.html#autotoc_md53", [
          [ "Culling", "md_doc_2zappy-gui.html#autotoc_md54", null ],
          [ "Batching", "md_doc_2zappy-gui.html#autotoc_md55", null ],
          [ "LOD (Level of Detail)", "md_doc_2zappy-gui.html#autotoc_md56", null ]
        ] ]
      ] ],
      [ "Système de caméra", "md_doc_2zappy-gui.html#autotoc_md57", [
        [ "Modes de caméra", "md_doc_2zappy-gui.html#autotoc_md58", null ],
        [ "Contrôles", "md_doc_2zappy-gui.html#autotoc_md59", null ]
      ] ],
      [ "Système de réseau", "md_doc_2zappy-gui.html#autotoc_md60", [
        [ "Thread de réseau séparé", "md_doc_2zappy-gui.html#autotoc_md61", null ],
        [ "Parsing des messages", "md_doc_2zappy-gui.html#autotoc_md62", null ]
      ] ],
      [ "Interface utilisateur", "md_doc_2zappy-gui.html#autotoc_md63", [
        [ "Composants UI", "md_doc_2zappy-gui.html#autotoc_md64", null ],
        [ "Panels détaillés", "md_doc_2zappy-gui.html#autotoc_md65", [
          [ "Team Panel", "md_doc_2zappy-gui.html#autotoc_md66", null ],
          [ "Resource Panel", "md_doc_2zappy-gui.html#autotoc_md67", null ],
          [ "Debug Panel (F3)", "md_doc_2zappy-gui.html#autotoc_md68", null ]
        ] ]
      ] ],
      [ "Système d'animation et effets", "md_doc_2zappy-gui.html#autotoc_md69", [
        [ "Animations de base", "md_doc_2zappy-gui.html#autotoc_md70", null ],
        [ "Système de particules", "md_doc_2zappy-gui.html#autotoc_md71", null ]
      ] ],
      [ "Optimisations importantes", "md_doc_2zappy-gui.html#autotoc_md72", [
        [ "Interpolation des mouvements", "md_doc_2zappy-gui.html#autotoc_md73", null ],
        [ "Cache de visibilité", "md_doc_2zappy-gui.html#autotoc_md74", null ],
        [ "Texture Atlas", "md_doc_2zappy-gui.html#autotoc_md75", null ]
      ] ],
      [ "Configuration et options", "md_doc_2zappy-gui.html#autotoc_md76", [
        [ "Fichier de configuration", "md_doc_2zappy-gui.html#autotoc_md77", null ],
        [ "Options graphiques", "md_doc_2zappy-gui.html#autotoc_md78", null ]
      ] ],
      [ "Gestion des assets", "md_doc_2zappy-gui.html#autotoc_md79", [
        [ "Structure des assets", "md_doc_2zappy-gui.html#autotoc_md80", null ],
        [ "Resource Manager", "md_doc_2zappy-gui.html#autotoc_md81", null ]
      ] ],
      [ "Tests spécifiques GUI", "md_doc_2zappy-gui.html#autotoc_md82", [
        [ "Tests visuels", "md_doc_2zappy-gui.html#autotoc_md83", null ],
        [ "Tests de performance", "md_doc_2zappy-gui.html#autotoc_md84", null ],
        [ "Tests d'interaction", "md_doc_2zappy-gui.html#autotoc_md85", null ]
      ] ],
      [ "Points d'attention", "md_doc_2zappy-gui.html#autotoc_md86", [
        [ "Thread safety", "md_doc_2zappy-gui.html#autotoc_md87", null ],
        [ "Memory management", "md_doc_2zappy-gui.html#autotoc_md88", null ],
        [ "Scalabilité", "md_doc_2zappy-gui.html#autotoc_md89", null ]
      ] ],
      [ "Debug et profiling", "md_doc_2zappy-gui.html#autotoc_md90", [
        [ "Mode debug (F3)", "md_doc_2zappy-gui.html#autotoc_md91", null ],
        [ "Profiling tools", "md_doc_2zappy-gui.html#autotoc_md92", null ]
      ] ],
      [ "Checklist de développement", "md_doc_2zappy-gui.html#autotoc_md93", null ]
    ] ],
    [ "Architecture du Client AI Zappy", "md_doc_2zappy-ai.html", [
      [ "Vue d'ensemble", "md_doc_2zappy-ai.html#autotoc_md95", null ],
      [ "Architecture comportementale", "md_doc_2zappy-ai.html#autotoc_md96", null ],
      [ "Structure recommandée (Python)", "md_doc_2zappy-ai.html#autotoc_md97", null ],
      [ "Machine à états", "md_doc_2zappy-ai.html#autotoc_md98", [
        [ "États principaux", "md_doc_2zappy-ai.html#autotoc_md99", null ],
        [ "Transitions d'états", "md_doc_2zappy-ai.html#autotoc_md100", null ]
      ] ],
      [ "Stratégies détaillées", "md_doc_2zappy-ai.html#autotoc_md101", [
        [ "Stratégie de survie", "md_doc_2zappy-ai.html#autotoc_md102", null ],
        [ "Stratégie de collecte", "md_doc_2zappy-ai.html#autotoc_md103", null ],
        [ "Stratégie d'élévation", "md_doc_2zappy-ai.html#autotoc_md104", null ]
      ] ],
      [ "Système de mémoire", "md_doc_2zappy-ai.html#autotoc_md105", [
        [ "World Model", "md_doc_2zappy-ai.html#autotoc_md106", null ],
        [ "Team Coordination", "md_doc_2zappy-ai.html#autotoc_md107", null ]
      ] ],
      [ "Algorithmes importants", "md_doc_2zappy-ai.html#autotoc_md108", [
        [ "Pathfinding toroïdal", "md_doc_2zappy-ai.html#autotoc_md109", null ],
        [ "Interprétation de la vision", "md_doc_2zappy-ai.html#autotoc_md110", null ],
        [ "Décision multi-critères", "md_doc_2zappy-ai.html#autotoc_md111", null ]
      ] ],
      [ "Communication d'équipe", "md_doc_2zappy-ai.html#autotoc_md112", [
        [ "Protocole de messages", "md_doc_2zappy-ai.html#autotoc_md113", null ],
        [ "Stratégies de communication", "md_doc_2zappy-ai.html#autotoc_md114", null ]
      ] ],
      [ "Optimisations et heuristiques", "md_doc_2zappy-ai.html#autotoc_md115", [
        [ "Exploration efficace", "md_doc_2zappy-ai.html#autotoc_md116", null ],
        [ "Cache de décisions", "md_doc_2zappy-ai.html#autotoc_md117", null ],
        [ "Anticipation", "md_doc_2zappy-ai.html#autotoc_md118", null ]
      ] ],
      [ "Gestion des erreurs et robustesse", "md_doc_2zappy-ai.html#autotoc_md119", [
        [ "Reconnexion automatique", "md_doc_2zappy-ai.html#autotoc_md120", null ],
        [ "Recovery après erreur", "md_doc_2zappy-ai.html#autotoc_md121", null ]
      ] ],
      [ "Configuration et tuning", "md_doc_2zappy-ai.html#autotoc_md122", [
        [ "Fichier de configuration", "md_doc_2zappy-ai.html#autotoc_md123", null ],
        [ "Paramètres ajustables", "md_doc_2zappy-ai.html#autotoc_md124", null ]
      ] ],
      [ "Tests et debugging", "md_doc_2zappy-ai.html#autotoc_md125", [
        [ "Mode debug", "md_doc_2zappy-ai.html#autotoc_md126", null ],
        [ "Visualisation", "md_doc_2zappy-ai.html#autotoc_md127", null ],
        [ "Métriques", "md_doc_2zappy-ai.html#autotoc_md128", null ]
      ] ],
      [ "Stratégies avancées", "md_doc_2zappy-ai.html#autotoc_md129", [
        [ "Formation d'alliances", "md_doc_2zappy-ai.html#autotoc_md130", null ],
        [ "Économie de ressources", "md_doc_2zappy-ai.html#autotoc_md131", null ],
        [ "Défense territoriale", "md_doc_2zappy-ai.html#autotoc_md132", null ]
      ] ],
      [ "Checklist de développement", "md_doc_2zappy-ai.html#autotoc_md133", null ]
    ] ],
    [ "Guide de Développement Zappy", "md_doc_2zappy-development.html", [
      [ "Setup de l'environnement", "md_doc_2zappy-development.html#autotoc_md135", [
        [ "Prérequis", "md_doc_2zappy-development.html#autotoc_md136", null ],
        [ "Structure initiale", "md_doc_2zappy-development.html#autotoc_md137", null ]
      ] ],
      [ "Standards de code", "md_doc_2zappy-development.html#autotoc_md138", [
        [ "C (Serveur)", "md_doc_2zappy-development.html#autotoc_md139", [
          [ "Style de code", "md_doc_2zappy-development.html#autotoc_md140", null ],
          [ "Conventions de nommage", "md_doc_2zappy-development.html#autotoc_md141", null ]
        ] ],
        [ "C++ (GUI)", "md_doc_2zappy-development.html#autotoc_md142", [
          [ "Style de code", "md_doc_2zappy-development.html#autotoc_md143", null ]
        ] ],
        [ "Python (AI)", "md_doc_2zappy-development.html#autotoc_md144", [
          [ "Style de code", "md_doc_2zappy-development.html#autotoc_md145", null ]
        ] ]
      ] ],
      [ "Organisation Git", "md_doc_2zappy-development.html#autotoc_md146", [
        [ "Structure des branches", "md_doc_2zappy-development.html#autotoc_md147", null ],
        [ "Workflow Git", "md_doc_2zappy-development.html#autotoc_md148", [
          [ "Créer une feature", "md_doc_2zappy-development.html#autotoc_md149", null ],
          [ "Convention de commits", "md_doc_2zappy-development.html#autotoc_md150", null ],
          [ "Pull Request", "md_doc_2zappy-development.html#autotoc_md151", null ]
        ] ],
        [ "Hooks Git", "md_doc_2zappy-development.html#autotoc_md152", [
          [ "Pre-commit hook", "md_doc_2zappy-development.html#autotoc_md153", null ]
        ] ]
      ] ],
      [ "Tests", "md_doc_2zappy-development.html#autotoc_md154", [
        [ "Structure des tests", "md_doc_2zappy-development.html#autotoc_md155", null ],
        [ "Tests unitaires (C)", "md_doc_2zappy-development.html#autotoc_md156", null ],
        [ "Tests d'intégration", "md_doc_2zappy-development.html#autotoc_md157", null ],
        [ "Coverage", "md_doc_2zappy-development.html#autotoc_md158", null ]
      ] ],
      [ "CI/CD", "md_doc_2zappy-development.html#autotoc_md159", [
        [ "GitHub Actions", "md_doc_2zappy-development.html#autotoc_md160", null ]
      ] ],
      [ "Debugging", "md_doc_2zappy-development.html#autotoc_md161", [
        [ "GDB pour le serveur", "md_doc_2zappy-development.html#autotoc_md162", null ],
        [ "Valgrind pour les fuites", "md_doc_2zappy-development.html#autotoc_md163", null ],
        [ "Debug réseau", "md_doc_2zappy-development.html#autotoc_md164", null ]
      ] ],
      [ "Performance", "md_doc_2zappy-development.html#autotoc_md165", [
        [ "Profiling CPU", "md_doc_2zappy-development.html#autotoc_md166", null ],
        [ "Optimisations communes", "md_doc_2zappy-development.html#autotoc_md167", null ]
      ] ],
      [ "Documentation", "md_doc_2zappy-development.html#autotoc_md168", [
        [ "Doxygen", "md_doc_2zappy-development.html#autotoc_md169", null ],
        [ "README par module", "md_doc_2zappy-development.html#autotoc_md170", null ]
      ] ],
      [ "Sécurité", "md_doc_2zappy-development.html#autotoc_md171", [
        [ "Validation des entrées", "md_doc_2zappy-development.html#autotoc_md172", null ],
        [ "Limites de ressources", "md_doc_2zappy-development.html#autotoc_md173", null ]
      ] ],
      [ "Checklist finale", "md_doc_2zappy-development.html#autotoc_md174", [
        [ "Avant de push", "md_doc_2zappy-development.html#autotoc_md175", null ],
        [ "Avant une release", "md_doc_2zappy-development.html#autotoc_md176", null ],
        [ "Review checklist", "md_doc_2zappy-development.html#autotoc_md177", null ]
      ] ]
    ] ],
    [ "Zappy - Documentation Utilisateur", "md_doc_2doc__user.html", [
      [ "Table des matières", "md_doc_2doc__user.html#autotoc_md236", null ],
      [ "Introduction", "md_doc_2doc__user.html#autotoc_md238", [
        [ "1.1 Vue d’ensemble", "md_doc_2doc__user.html#autotoc_md239", null ],
        [ "1.2 Public visé", "md_doc_2doc__user.html#autotoc_md240", null ],
        [ "1.3 Prérequis", "md_doc_2doc__user.html#autotoc_md241", null ]
      ] ],
      [ "Installation", "md_doc_2doc__user.html#autotoc_md243", [
        [ "2.1 Compilation du projet", "md_doc_2doc__user.html#autotoc_md244", null ],
        [ "2.2 Vérification de l’installation", "md_doc_2doc__user.html#autotoc_md245", null ]
      ] ],
      [ "Architecture du système", "md_doc_2doc__user.html#autotoc_md247", [
        [ "3.1 Serveur de jeu", "md_doc_2doc__user.html#autotoc_md248", null ],
        [ "3.2 Interface graphique (GUI)", "md_doc_2doc__user.html#autotoc_md249", null ],
        [ "3.3 Intelligence artificielle (AI)", "md_doc_2doc__user.html#autotoc_md250", null ]
      ] ],
      [ "Utilisation", "md_doc_2doc__user.html#autotoc_md252", [
        [ "4.1 Démarrage du serveur", "md_doc_2doc__user.html#autotoc_md253", null ],
        [ "4.2 Lancement de l’interface graphique", "md_doc_2doc__user.html#autotoc_md254", null ],
        [ "4.3 Lancement de l’intelligence artificielle", "md_doc_2doc__user.html#autotoc_md255", null ],
        [ "4.4 Configuration des équipes", "md_doc_2doc__user.html#autotoc_md256", null ]
      ] ],
      [ "Règles du jeu", "md_doc_2doc__user.html#autotoc_md258", [
        [ "5.1 Objectif", "md_doc_2doc__user.html#autotoc_md259", null ],
        [ "5.2 Monde de jeu", "md_doc_2doc__user.html#autotoc_md260", null ],
        [ "5.3 Ressources", "md_doc_2doc__user.html#autotoc_md261", null ],
        [ "5.4 Élévation des joueurs", "md_doc_2doc__user.html#autotoc_md262", null ]
      ] ],
      [ "Interface graphique", "md_doc_2doc__user.html#autotoc_md264", [
        [ "6.1 Mode 2D", "md_doc_2doc__user.html#autotoc_md265", null ],
        [ "6.2 Mode 3D", "md_doc_2doc__user.html#autotoc_md266", null ],
        [ "6.3 Contrôles et navigation", "md_doc_2doc__user.html#autotoc_md267", null ]
      ] ],
      [ "Protocole de communication", "md_doc_2doc__user.html#autotoc_md269", [
        [ "7.1 Commandes AI", "md_doc_2doc__user.html#autotoc_md270", null ],
        [ "7.2 Commandes GUI", "md_doc_2doc__user.html#autotoc_md271", null ]
      ] ],
      [ "Dépannage et FAQ", "md_doc_2doc__user.html#autotoc_md273", [
        [ "8.1 Problèmes courants", "md_doc_2doc__user.html#autotoc_md274", null ],
        [ "8.2 Questions fréquentes", "md_doc_2doc__user.html#autotoc_md275", null ]
      ] ],
      [ "Sécurité et bonnes pratiques", "md_doc_2doc__user.html#autotoc_md277", null ],
      [ "Support et ressources", "md_doc_2doc__user.html#autotoc_md279", null ],
      [ "Glossaire", "md_doc_2doc__user.html#autotoc_md281", null ],
      [ "Auteurs", "md_doc_2doc__user.html#autotoc_md283", null ]
    ] ],
    [ "Topics", "topics.html", "topics" ],
    [ "Namespaces", "namespaces.html", [
      [ "Namespace List", "namespaces.html", "namespaces_dup" ],
      [ "Namespace Members", "namespacemembers.html", [
        [ "All", "namespacemembers.html", null ],
        [ "Functions", "namespacemembers_func.html", null ],
        [ "Variables", "namespacemembers_vars.html", null ],
        [ "Enumerations", "namespacemembers_enum.html", null ]
      ] ]
    ] ],
    [ "Data Structures", "annotated.html", [
      [ "Data Structures", "annotated.html", "annotated_dup" ],
      [ "Data Structure Index", "classes.html", null ],
      [ "Class Hierarchy", "hierarchy.html", "hierarchy" ],
      [ "Data Fields", "functions.html", [
        [ "All", "functions.html", "functions_dup" ],
        [ "Functions", "functions_func.html", "functions_func" ],
        [ "Variables", "functions_vars.html", "functions_vars" ],
        [ "Typedefs", "functions_type.html", null ],
        [ "Enumerations", "functions_enum.html", null ]
      ] ]
    ] ],
    [ "Files", "files.html", [
      [ "File List", "files.html", "files_dup" ],
      [ "Globals", "globals.html", [
        [ "All", "globals.html", "globals_dup" ],
        [ "Functions", "globals_func.html", "globals_func" ],
        [ "Variables", "globals_vars.html", null ],
        [ "Typedefs", "globals_type.html", null ],
        [ "Enumerations", "globals_enum.html", null ],
        [ "Enumerator", "globals_eval.html", null ],
        [ "Macros", "globals_defs.html", null ]
      ] ]
    ] ]
  ] ]
];

var NAVTREEINDEX =
[
"AnimationSystem_8cpp.html",
"PlayerInfoPanel_8cpp_source.html",
"classmodels_1_1Player.html#a50ecebecd3eeef49daa9b62248413a43",
"classzappy_1_1Application.html#aef9df42d41c6307df09ef19bedcfa1fc",
"classzappy_1_1DebugPanel.html#ae2c398c25986e3dfbcef8752cf3a22fb",
"classzappy_1_1EventLog.html#afe002bcd0dcba9236f61025a6cf7fc41",
"classzappy_1_1MessageParser.html#ae8805f0f50243e3939015645fc918e27",
"classzappy_1_1PlayerInfoPanel.html",
"classzappy_1_1Renderer3D.html#ab10ffe75afa589d83326da6f62a177d8",
"classzappy_1_1TeamPanel.html#a39f2694687f7c185ba76680acbd42012",
"classzappy_1_1TileInfoPanel.html#a8953a025da2ada8ef0f7fa3e29a5fcef",
"client__utils_8c.html#a7f77697c324f1496d12e7b9ac27ffd02",
"group__config.html#gaa35730e2ee087e2e891f4e2805f69c58",
"group__player.html#gae77941d41d73cccad875a8434501649f",
"md_doc_2zappy-development.html#autotoc_md167",
"namespacezappy_1_1math.html#a9b620084bb578e2bfa58d066a287623e",
"structzappy_1_1Message.html#a90d43ff00d89e12a771aba6a6075c6d8"
];

var SYNCONMSG = 'click to disable panel synchronisation';
var SYNCOFFMSG = 'click to enable panel synchronisation';