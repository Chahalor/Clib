// Header
/**
 * @file args.h
 * @brief API publique du module interface/args (parser CLI).
 *
 * Ce module permet de declarer un parser (params, options, sous-commandes),
 * puis de parser un argv et d'extraire le resultat.
 *
 * Fonctionnalites principales:
 * - Options courtes (-v) et longues (--verbose).
 * - Sous-commandes (sub-parsers).
 * - Parametres positionnels et parametres d'options.
 * - Parametres "nargs" (multi-valeurs).
 * - Delimiteur "--" pour desactiver la detection des options.
 *
 * @defgroup args Args parser
 * @brief Interface publique de parsing d'arguments.
 * @{
 */

#ifndef INTERFACE_ARGS_H
# define INTERFACE_ARGS_H

# pragma once

/* ************************************************************************** */
/*                                 Headers                                    */
/* ************************************************************************** */

/* -----| Systems   |----- */
	//...

/* -----| Globals   |----- */
# include "../standards/lib_config.h"
# include "../standards/lib_standards.h"

/* -----| Internals |----- */
# include "args/types.h"
// #ifdef DEBUG
// # include "args/_internal_/_args.h"
// #endif

/* -----| Modules   |----- */
# include "../processor/processor.h"

/* ************************************************************************** */
/*                                 Macros                                     */
/* ************************************************************************** */

// #define FOR_EACH_OPTIONS(var, out) _FOR_EACH_OPTIONS(var, out)
// #define FOR_EACH_PARAMS(var, out)  _FOR_EACH_PARAMS(var, out)

/* ************************************************************************** */
/*                                 Prototypes                                 */
/* ************************************************************************** */

#pragma region Memory

/**
 * @brief Creer un parser racine.
 *
 * Fonctionnalites:
 * - Cree un parser vide (sans params, options, sous-commandes).
 * - Ajoute automatiquement l'option "help" / 'h' au parser racine.
 *
 * @return Pointeur vers le parser ou NULL en cas d'echec d'allocation.
 */
t_args_parser	*args_parser_new(void);

/**
 * @brief Liberer un parser et toutes ses dependances.
 *
 * Fonctionnalites:
 * - Libere les params, options et sous-commandes lies.
 * - Apres appel, tous les pointeurs vers le parser deviennent invalides.
 *
 * @param _parser Parser a liberer (NULL autorise).
 *
 * @return void
 */
void			args_parser_free(
	t_args_parser *const _parser
);

/**
 * @brief Liberer un resultat de parsing.
 *
 * Fonctionnalites:
 * - Libere l'arbre de sortie (options/params/subs) produit par args_parse().
 * - Les valeurs pointees dans la sortie deviennent invalides.
 *
 * @param _out Sortie a liberer (NULL autorise).
 *
 * @return void
 */
void			args_output_free(
	t_args_output *const _out
);

#pragma region Parsing

/**
 * @brief Parser un argv selon un parser.
 *
 * Fonctionnalites:
 * - Ignore argv[0] (le nom du programme).
 * - Supporte options courtes (-x) et longues (--name).
 * - Supporte les sous-commandes (switch de contexte).
 * - Supporte les params positionnels et "nargs".
 * - Le token "--" desactive la detection des options pour la suite.
 *
 * Limites:
 * - Pas de groupes d'options courtes (-abc) ni de formes --opt=val.
 * - Les options longues commencent par un alphanumerique (--name).
 *
 * @param _parser Parser racine.
 * @param _argc   Nombre d'arguments.
 * @param _argv   Tableau argv (argv[0] = nom du binaire).
 *
 * @return Pointeur vers une sortie (meme en cas d'erreur) ou NULL si
 *         echec d'allocation. Utiliser args_error() pour connaitre l'etat.
 */
t_args_output	*args_parse(
	const t_args_parser *const _parser,
	const int _argc,
	const char *const _argv[]
);

/**
 * TODO: doc of this function
*/
void	args_show_help(
	const t_args_parser *const _parser,
	const int _exit
);

#pragma region Sub-parsers

/**
 * @brief Ajouter une sous-commande (sub-parser) a un parser.
 *
 * Fonctionnalites:
 * - Cree un nouveau parser enfant associe a un nom de sous-commande.
 * - Le contexte de parsing bascule des qu'un token egal a ce nom est rencontre.
 *
 * @param _parent Parser parent.
 * @param _name   Nom de la sous-commande (sans '-').
 * @param _desc   Description (optionnelle).
 *
 * @return Pointeur vers le sous-parser cree ou NULL en cas d'echec.
 */
t_args_parser	*args_parser_add_sub(
	t_args_parser *const _parent,
	const char *const _name,
	const char *const _desc
);

#pragma region Options

/**
 * @brief Ajouter une option a un parser.
 *
 * Fonctionnalites:
 * - Supporte option longue et/ou courte.
 * - Le nom long peut etre donne avec ou sans "-" / "--" (il sera normalise).
 *
 * @param _parser     Parser cible.
 * @param _long_name  Nom long (ex: "verbose" ou "--verbose", optionnel).
 * @param _short_name Nom court (ex: 'v', optionnel).
 * @param _desc       Description (optionnelle).
 *
 * @return Pointeur vers l'option creee, ou NULL si noms invalides ou echec.
 */
t_args_option	*args_parser_add_option(
	t_args_parser *const _parser,
	const char *const _long_name,
	const char _short_name,
	const char *const _desc
);

#pragma region Parameters

/**
 * @brief Ajouter un parametre positionnel a un parser.
 *
 * Fonctionnalites:
 * - Definit l'ordre des params positionnels.
 * - Si args_param_specs_nargs est active, le parametre peut recevoir
 *   plusieurs valeurs (jusqu'a la prochaine option, ou "--", ou fin).
 *
 * @param _parser Parser cible.
 * @param _name   Nom du parametre (ne doit pas commencer par '-').
 * @param _desc   Description (optionnelle).
 * @param _spec   Specs (args_param_specs_*).
 * @param _type   Type logique (t_param_type).
 *
 * @return Pointeur vers le parametre cree ou NULL en cas d'echec.
 */
t_args_param	*args_parser_add_param(
	t_args_parser *const _parser,
	const char *const _name,
	const char *const _desc,
	const int _spec,
	const int _type
);

/**
 * @brief Ajouter un parametre a une option.
 *
 * Fonctionnalites:
 * - Lie des valeurs a une option (ex: "--count 3").
 * - Si args_param_specs_nargs est active, les valeurs s'arretent
 *   avant la prochaine option (sauf si options desactivees par "--").
 *
 * @param _parent Option cible.
 * @param _name   Nom logique du parametre.
 * @param _desc   Description (optionnelle).
 * @param _spec   Specs (args_param_specs_*).
 * @param _type   Type logique (t_param_type).
 *
 * @return Pointeur vers le parametre cree ou NULL en cas d'echec.
 */
t_args_param	*args_option_add_param(
	t_args_option *const _parent,
	const char *const _name,
	const char *const _desc,
	const int _spec,
	const int _type
);

#pragma region Setter

/**
 * @brief	set the `var` node description to `description`
 * 
 * @param	var			the node where the description will be set
 * ca be a `t_args_parser*`, a `t_args_param*` or a `t_args_option*`
 * @param	description	The description to be set
 * 
 * @return	error_none or error_invalid_arg;
*/
# define	args_set_description(var, description) (__builtin_choose_expr( \
	IS_TYPE(var, t_args_parser *), \
	args_parser_set_desc, \
	__builtin_choose_expr( \
		IS_TYPE(var, t_args_param *), \
		args_param_set_desc, \
		__builtin_choose_expr( \
			IS_TYPE(var, t_args_option *), \
			args_option_set_desc, \
			(void *)0 \
		) \
	) \
)(var, description))

/**
 * @brief Definir la description d'un parser.
 *
 * Fonctionnalites:
 * - Remplace la description courante.
 *
 * @param parser Parser cible.
 * @param desc   Description (NULL pour supprimer).
 *
 * @return error_none ou code d'erreur.
 */
int args_parser_set_desc(
	t_args_parser *const parser,
	const char *const desc
);

/**
 * @brief Definir la description d'un parametre.
 *
 * @param _param Parametre cible.
 * @param _desc  Description (NULL pour supprimer).
 *
 * @return error_none ou code d'erreur.
 */
int	args_param_set_desc(
	t_args_param *const _param,
	const char *const _desc
);

/**
 * @brief Definir la description d'une option.
 *
 * @param _option Option cible.
 * @param _desc   Description (NULL pour supprimer).
 *
 * @return error_none ou code d'erreur.
 */
int	args_option_set_desc(
	t_args_option *const _option,
	const char *const _desc
);

#pragma region Extraction

/**
 * @brief Tester la presence d'un parametre positionnel dans la sortie.
 *
 * Fonctionnalites:
 * - Recherche un parametre par nom dans la sortie racine.
 *
 * @param _parser Sortie de parsing (t_args_output).
 * @param _name   Nom du parametre.
 *
 * @return 1 si trouve, 0 si absent, ou error_invalid_arg si arguments invalides.
 */
char	args_has_param(
	t_args_output *const _parser,
	const char *const _name
);

/**
 * @brief Tester la presence d'une option dans la sortie.
 *
 * Fonctionnalites:
 * - Accepte "-x", "--name", "x" ou "name".
 *
 * @param _parser Sortie de parsing (t_args_output).
 * @param _name   Nom de l'option.
 *
 * @return 1 si trouve, 0 si absent, ou error_invalid_arg si arguments invalides.
 */
char	args_has_option(
	t_args_output *const _parser,
	const char *const _name
);

/**
 * @brief Tester la presence d'une sous-commande dans la sortie.
 *
 * @param _parser Sortie de parsing (t_args_output).
 * @param _name   Nom de la sous-commande.
 *
 * @return 1 si trouve, 0 si absent, ou error_invalid_arg si arguments invalides.
 */
char	args_has_sub(
	t_args_output *const _parser,
	const char *const _name
);

/**
 * @brief Tester si une option de sortie contient un parametre nomme.
 *
 * @param _option Option de sortie.
 * @param _name   Nom du parametre a tester.
 *
 * @return 1 si trouve, 0 si absent, ou error_invalid_arg si arguments invalides.
 */
char	args_option_has_param(
	t_args_output_option *const _option,
	const char *const _name
);

/**
 * @brief	get a param value form var
 * 
 * var can be a `t_args_output*` or a `t_args_output_parser*`
 * 
 * @param	var		the struct where to extract the param
 * @param	name	the name of the param
 * @param	n		a pointer to count the number of value in the param
 * 
 * @return	All value(s) strored inside of the param
 * @retval	NULL in case of syscall fail or if `n == 0`
 * @retval	`char *` if `n == 1`
 * @retval	`char**` if `n > 1`
 * 
 * @note	in any case the return value should be free using `mem_free`
 * 
 * @version	2.0.0
*/
# define	args_get_param(var, name, n) (__builtin_choose_expr( \
		IS_TYPE(var, t_args_output *), \
		args_parser_get_param, \
		__builtin_choose_expr( \
			IS_TYPE(var, t_args_output_parser *), \
			args_output_parser_get_param, \
			(void)0 \
		) \
	)(var, name, n))

/**
 * @brief	get a option struct for a output
 * 
 * var can be a `t_args_output*` or a `t_args_output_parser*`
 * 
 * @param	var		the struct where to extract the param
 * @param	name	the name of the param
 * @param	n		a pointer to count the number of value in the param
 * 
 * @return	The option or NULL if not found
 * 
 * @version	2.0.0
*/
# define	args_get_option(var, name) (__builtin_choose_expr( \
		IS_TYPE(var, t_args_output *), \
		args_parser_get_option, \
		__builtin_choose_expr( \
			IS_TYPE(var, t_args_output_parser *), \
			args_output_parser_get_option, \
			(void)0 \
		) \
	)(var, name))

/**
 * @brief Extraire les valeurs d'un parametre positionnel.
 *
 * Fonctionnalites:
 * - Alloue un tableau de pointeurs vers les valeurs.
 * - Les chaines pointeent appartiennent a la sortie; ne pas les liberer.
 *
 * @param	output	Sortie de parsing.
 * @param	name	Nom du parametre.
 * @param	n		a pointer to a variable that wwill return the number of param returned
 * 
 * @return the param value
 * @retval if `*n == 0`, the return value is `NULL`
 * @retval if `*n` == 1, the return value is a `char *`
 * @retval if `*n` > 1,  the return value is a NULL terminated `char **`
 *
 * @note Le tableau retourne doit etre libere avec mem_free().
 * 
 * @old
char	*args_parser_get_param(
	t_args_output *const _output,
	const char *const _name,
	char *const * *const _values,
	unsigned int *const _count
);
 * 
 */
void	*args_parser_get_param(
	t_args_output *const output,
	const char *const name,
	size_t *const n
);

/**
 * @brief Extraire les valeurs d'un parametre depuis une sous-commande.
 *
 * @param	output	Sortie de parsing.
 * @param	name	Nom du parametre.
 * @param	n		a pointer to a variable that wwill return the number of param returned
 * 
 * @return the param value
 * @retval if `*n == 0`, the return value is `NULL`
 * @retval if `*n` == 1, the return value is a `char *`
 * @retval if `*n` > 1,  the return value is a NULL terminated `char **`
 *
 * @note Le tableau retourne doit etre libere avec mem_free().
 * 
 * @old
char	*args_output_parser_get_param(
	t_args_output_parser *const _output,
	const char *const _name,
	char *const * *const _values,
	unsigned int *const _count
);
 * 
 */
void	*args_output_parser_get_param(
	t_args_output_parser *const parser,
	const char *const name,
	size_t *const n
);

/**
 * @brief Extraire les valeurs d'une option.
 *
 * Fonctionnalites:
 * - Accepte "-x", "--name", "x" ou "name".
 * - Regroupe toutes les valeurs de ses params dans un seul tableau.
 * - Si l'option n'a pas de params, *_values est mis a (char **)0x1
 *   et *_count a 0.
 *
 * @param _output Sortie de parsing.
 * @param _name   Nom de l'option.
 *
 * @return the option search or `NULL` if the option is not found in the parsing
 * 
 * @old
 char	args_parser_get_option(
 	t_args_output *const _output,
 	const char *const _name,
 	char *const * *const _values,
 	unsigned int *const _count
 );
 * 
 */
t_args_output_option	*args_parser_get_option(
	t_args_output *const output,
	const char *const name
);

/**
 * @brief Extraire les valeurs d'une option depuis une sous-commande.
 *
 * @param _output Sortie de parsing.
 * @param _name   Nom de l'option.
 *
 * @return the option search or `NULL` if the option is not found in the parsing
 * 
 * @old
char	args_output_parser_get_option(
	 t_args_output_parser *const _output,
	 const char *const _name,
	 char *const * *const _values,
	 unsigned int *const _count
 );
 * 
 */
t_args_output_option	*args_output_parser_get_option(
	t_args_output_parser *const parser,
	const char *const name
);

/**
 * @brief Recuperer le nom de la sous-commande active.
 *
 * Fonctionnalites:
 * - Retourne le nom de la premiere sous-commande rencontree.
 *
 * @param out Sortie de parsing.
 *
 * @return Nom de la sous-commande active ou NULL.
 */
const char	*args_active_subcommand(
	const t_args_output *out
);

/**
 * @brief Recuperer la sortie de la premiere sous-commande.
 *
 * @param out Sortie de parsing.
 *
 * @return Pointeur vers t_args_output_parser ou NULL si aucune sous-commande.
 */
t_args_output_parser	*args_get_sub_output(
	const t_args_output *out
);

#pragma region Configuration

/**
 * @brief Recuperer le code d'erreur de parsing.
 *
 * @param out Sortie de parsing.
 *
 * @return Code d'erreur (enum e_args_error).
 */
int	args_error(
	const t_args_output *out
);

/**
 * @brief Convertir un code d'erreur en chaine lisible.
 *
 * @param err Code d'erreur (enum e_args_error).
 *
 * @return Chaine constante descriptive.
 */
const char	*args_error_str(
	int err
);

#endif	// INTERFACE_ARGS_H
/** @} */
