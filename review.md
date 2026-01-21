# codex
**Findings**
- [ ] Critique: `t_args_parser` est typedef vers `_s_args_parser_root`, mais `args_new_parser()` retourne un `_t_args_parser*`; l'API publique mélange root/parser (types incompatibles, architecture incohérente) dans `interface/args/types.h:34`, `interface/args/args.c:27`, `interface/args/_internal_/_types.h:107`.
- [X] Critique: `_root_get_next()` compare `index` à lui-même et retourne toujours `NULL`, donc aucune consommation d'argv et boucles infinies possibles dans le parseur `interface/args/_internal_/_parser.c:106`.
- [X] Critique: `_parse_args()` utilise `result` non initialisé et ne retourne aucune valeur, UB + logique de boucle cassée `interface/args/_internal_/_parser.c:231`.
- [X] Critique: `_fill_param()` lit `argv[_root->argc]` (out-of-bounds) et ne traite jamais les params non-`nargs`, donc valeurs perdues `interface/args/_internal_/_parser.c:171`.
- Critique: `_parse_opt()` ne consomme pas l'option (pas d'incrément d'index), donc la boucle `while` peut tourner à l'infini `interface/args/_internal_/_parser.c:207`.
- [X] Haute: `_parse_sub_parser()` inverse le test `strcmp` et déclenche sur le mauvais sous-parser; en plus `_parse_args()` ignore le `_parser` passé et utilise toujours `_root->parser->options` `interface/args/_internal_/_parser.c:240` et `interface/args/_internal_/_parser.c:263`.
- [ ] Haute: `_args_parse()` appelle `_check_parser()` et `_check_output()` avec un type incompatible (root vs parser), ce qui masque des erreurs de validation et provoquera du UB quand implémenté `interface/args/_internal_/_parser.c:295`.
- [X] Haute: `_add_param()` réalloue sans jamais mettre à jour `max_values`, ce qui conduit à des réallocations infinies et risque d'écriture hors bornes `interface/args/_internal_/_parser.c:125`.
- [X] Haute: `_get_sub_parser_of()` parcourt `_parser->next` au lieu de `sub_parsers`, donc la résolution de sous-commandes échoue systématiquement `interface/args/_internal_/_parser.c:32`.
- [X] Moyenne: `_get_param_of()` initialise un pointeur à `false` (type `bool`), ce qui masque des erreurs de compilation/lecture `interface/args/_internal_/_parser.c:71`.
- [ ] Moyenne: `args_add_subparser()` appelle `_args_add_param()` avec une signature incorrecte, ce qui casse la création de sous-parseurs `interface/args/args.c:77`.
- [X] Moyenne: Les fonctions `_args_mem_new_*()` allouent un buffer de nom mais ne copient jamais `_name`, laissant des noms vides; `_args_mem_new_root()` ne remet pas à zéro `argc/index/argv/parser`, ce qui conduit à des lectures indéfinies `interface/args/_internal_/_args_memory.c:21`.
- [ ] Moyenne: `t_args_param`/`t_args_option` ne sont pas définis dans l'API publique alors qu'ils sont utilisés dans `args.h`, ce qui rend l'interface instable/inutilisable `interface/args/types.h:34`, `interface/args/args.h:97`.

**Questions**
- Quel est le type public attendu pour le parser: `_t_args_parser` (définition fonctionnelle) ou `_t_args_parser_root` (wrapper runtime) ?
- Le design prévoit-il un parseur immuable (décrit par l'utilisateur) et un root séparé pour l'exécution, ou un seul type ?

**Tests/validation**
- Aucun test n'est présent dans ce module; difficile de valider la résolution d'options/sous-commandes et la gestion des index.

# GPT-5.2

## Findings (compléments)
### Critique

* [X] Critique: `_is_opt()` accepte des formes non standards comme `"---foo"` comme option longue valide et ne traite pas explicitement le token `"--"` comme fin des options; cela casse la norme POSIX implicite et rend ambigu le parsing des valeurs commençant par `-` `interface/args/_internal_/_parser.c`.
* [ ] Critique: absence totale de gestion de l’état *end-of-options* [ ] (`--`), ce qui empêche de passer des valeurs positionnelles ou des arguments d’option commençant par `-` de manière standard; impacte toutes les fonctions de parsing des tokens.
* Critique: `_parse_args()` mélange la responsabilité de détection (option / subparser / param) avec la consommation effective des arguments, ce qui conduit à des appels multiples à `_parse_params()` et à une assignation instable des paramètres positionnels.
* [ ] Critique: `_parse_params()` et `_fill_param()` implémentent une double itération incohérente sur la liste des paramètres (`for` imbriqués), provoquant soit une consommation multiple des mêmes paramètres, soit une perte de valeurs selon les chemins d’exécution.
* [ ] Critique: `_parse_opt()` incrémente `nb_calls` mais ne vérifie jamais les règles de répétition (option unique vs répétable), rendant inopérants les futurs checks de type `args_error_duplicate`.
* [ ] Critique: `_parse_args()` ne retourne aucune valeur (`missing return`), ce qui est un UB direct et rend le flux de contrôle imprévisible même si le compilateur ne se plaint pas.
* [ ] Critique: `_parse_sub_parser()` ne consomme pas explicitement le token déclencheur du sous-parser avant de déléguer, ce qui peut provoquer un décalage d’index ou une relecture du même token dans `_parse_args()`.

---

### Haute

* [ ] Haute: confusion systémique entre *définition du parser* (grammar immuable) et *état runtime* (index, argv, nb_calls, valeurs), malgré l’intention affichée de les séparer; plusieurs fonctions internes continuent d’utiliser `_root->parser` au lieu du parser courant passé en argument.
* [ ] Haute: absence d’un invariant clair sur `_root->index` (qui est responsable de son incrémentation et à quel moment), ce qui rend le raisonnement sur la progression du parsing extrêmement fragile.
* [ ] Haute: `_add_param()` copie les anciennes valeurs dans un nouveau buffer mais ne met jamais à jour `max_values`, ce qui invalide toute la stratégie d’allocation par blocs et conduit à des reallocations répétées ou à des écritures hors limites.
* [ ] Haute: aucune distinction claire entre “paramètre positionnel” et “paramètre d’option” dans le flux de parsing; la logique repose implicitement sur le contexte, ce qui rend le code difficile à maintenir et à auditer.
* [ ] Haute: la gestion des erreurs (`_args_config_set_errnum`) est disséminée sans garde-fou contre l’écrasement d’une erreur déjà posée, ce qui risque de masquer l’erreur racine par une erreur secondaire plus générique.

---

### Moyenne

* [ ] Moyenne: `_get_opt_of()` est non implémentée mais appelée comme si elle était fiable; or toute la logique de parsing des options repose dessus (résolution short/long, ambiguïtés, contexte).
* [ ] Moyenne: `_get_param_of()` et `_get_sub_parser_of()` effectuent des recherches linéaires à chaque token sans cache ni indexation; acceptable au début, mais le design ne prévoit aucune évolution vers une résolution plus efficace.
* [ ] Moyenne: les fonctions internes supposent que `_root->argv[_root->index]` est toujours non-NULL sans vérification systématique, ce qui rend le code vulnérable aux appels avec des argv mal formés ou des erreurs d’index.
* [ ] Moyenne: `_args_parse()` mélange initialisation, parsing, validation et destruction dans une seule fonction, ce qui rend les chemins d’erreur difficiles à raisonner et à tester isolément.

---

### Basse (mais à noter)

* [ ] Basse: usage incohérent de `restrict` dans des fonctions où les pointeurs peuvent aliaser indirectement via des structures partagées; bénéfice nul, risque de confusion.
* [ ] Basse: nommage parfois trompeur (`_parse_params` vs `_fill_param`) qui ne reflète pas clairement le niveau de granularité réel de la fonction.
* [ ] Basse: absence de commentaires sur les invariants attendus (ex: “à l’entrée de cette fonction, index pointe sur le premier token non consommé”).

---

## Questions (compléments)

* [ ] Le parsing est-il pensé comme **single-pass strict** (chaque token consommé exactement une fois), ou certaines fonctions sont-elles autorisées à “regarder en avance” (`peek`) sans consommer ?
* [ ] Les paramètres positionnels sont-ils censés être consommés **uniquement après** les options, ou bien intercalés librement (comme discuté précédemment) ? Le code actuel n’est pas cohérent sur ce point.
* [ ] Quelle est la politique exacte sur les options répétables vs non répétables, et où cette règle doit-elle être appliquée (au moment du parsing ou en post-validation) ?
* [ ] Le module est-il censé être *thread-safe* ou au moins *reentrant* ? L’usage d’un errnum global et d’un root mutable pose question.

---

## Recommandations structurantes (nouvelle section suggérée)

* [ ] Introduire une **boucle de parsing unique** qui est la seule autorité à incrémenter `_root->index`.
* [ ] Séparer strictement:

  * [ ] *détection* (`is option / is subparser / is positional`)
  * [ ] *consommation* (avance de l’index)
* [ ] Redéfinir `_fill_param()` pour qu’elle ne traite **qu’un seul paramètre**, sans boucle interne.
* [ ] Introduire un flag explicite `options_disabled` déclenché par `"--"`.
* [ ] Clarifier et figer les invariants du type “à l’entrée de `_parse_opt`, index pointe sur le token option”.

---

## Conclusion globale

Le module montre une **intention d’architecture solide** (séparation définition/runtime, parsing incrémental, gestion d’erreurs centralisée), mais l’implémentation actuelle souffre de bugs critiques, d’incohérences de flux et d’un manque d’invariants clairement définis.
Avant toute optimisation ou ajout de fonctionnalités, un refactor ciblé du cœur du parsing (gestion de l’index, boucle principale, consommation des tokens) est indispensable pour garantir la correction fonctionnelle.
