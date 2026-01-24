# codex
## Revue interface/args (2026-01-24)
**Findings**
- [ ] Critique: `args_parse` a une signature/retour incohérents entre header et implémentation (header: `t_args_output *args_parse(const t_args_parser*)`, impl: `args_parse(const t_args_parser*, int, char**)` et retourne un `int`). Cela casse l’ABI et retourne un pointeur invalide. `interface/args/args_parsing.c:22-35`, `interface/args.h:48-50`.
- [ ] Critique: `t_args_output` est typedef sur `_s_args_output_parser` au lieu de `_s_args_output`, donc l’API publique ne correspond pas à la structure réellement produite (champ `root/error`). Les fonctions d’extraction accèdent alors à des champs inexistants (`_output->params/options/sub`). `interface/args/types.h:41-42`, `interface/args/_internal_/_types.h:206-220`, `interface/args/_internal_/_args_extract.c:69-188`.
- [ ] Critique: `_args_parse_loop_option` ne met jamais à jour l’état du parseur (affectations sans `*` sur `context_type/opt/out_opt/opt_param`) et passe un mauvais pointeur à `_out_add_param` (liste de params vs option). Résultat: parsing d’options cassé + crash potentiel. `interface/args/_internal_/_args_parser.c:345-410`.
- [ ] Haute: `_args_mem_new_*` alloue des tailles incorrectes (pas de `+1` pour le NUL, `sizeof(_t_args_parser)` utilisé pour option/param, ternaires sans parenthèses qui évaluent `strlen(NULL)`), provoquant overflow/segfault. `interface/args/_internal_/_args_memory.c:30-210`, `interface/args/_internal_/_args_memory.c:215-266`.
- [ ] Haute: `_args_config_set_errnum` écrit `config->errnum` alors que `_s_args_config` ne contient pas ce champ. Cela ne compile pas ou écrase la mémoire. `interface/args/_internal_/_args_config.c:46-58`, `interface/args/_internal_/_types.h:91-117`.
- [ ] Haute: `_args_check_output` est appelé mais jamais défini, ce qui casse le link. `interface/args/_internal_/_args_parser.c:625-631`.
- [ ] Moyenne: appels à des symboles inexistants (`_args_check_parser_has_*`, `_args_parser_add_*`, `_args_option_add_param`, `_args_mem_free_*`) rendent le module non-liable. `interface/args/args_extract.c:21-108`, `interface/args/args_memory.c:31-120`.
- [ ] Moyenne: `_args_get_param` utilise `value` au lieu de `values`, masque `_this`, et retourne un `char` au lieu d’un `char *`, ce qui casse la récupération des paramètres. `interface/args/_internal_/_args_extract.c:92-132`.
- [ ] Moyenne: `_args_get_option` utilise `_lname/_key` non initialisés selon le chemin, ne renseigne pas toujours `_count` et retourne toujours `0`. `interface/args/_internal_/_args_extract.c:134-188`.

**Questions**
- Le type public `t_args_output` doit-il exposer la structure complète (avec `root/error`) ou un simple nœud parser ? La typedef actuelle contredit l’implémentation.
- L’API publique doit-elle prendre `argc/argv` (comme l’implémentation) ou seulement un parser (comme le header) ?

**Tests/validation**
- Aucun test dédié pour valider le parsing, l’extraction ou les erreurs; difficile de vérifier les changements structurels.
