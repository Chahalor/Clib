// Header
/**
 * @file types.h
 * @brief Types publics pour le module interface/args.
 *
 * @ingroup args
 */

# pragma once

/* ************************************************************************** */
/*                                 Headers                                    */
/* ************************************************************************** */

/* -----| Systems   |----- */
	//...

/* -----| Globals   |----- */
	//...

/* -----| Internals |----- */
	//...

/* -----| Modules   |----- */
	//...

/* ************************************************************************** */
/*                                 Typedefs                                   */
/* ************************************************************************** */

/* -----| Typename  |----- */
	//...

/* -----| Enums     |----- */
typedef enum e_param_type				t_param_type;
typedef enum e_args_param_specs			t_args_param_specs;

/* -----| Structs   |----- */
typedef struct _s_args_parser			t_args_parser;

typedef struct _s_args_param			t_args_param;
typedef struct _s_args_option			t_args_option;
typedef struct _s_args_parser			t_args_parser;

// typedef struct _s_args_output_value		t_args_output_value;
// typedef struct _s_args_output_param		t_args_output_param;
typedef struct _s_args_output_option	t_args_output_option;
typedef struct _s_args_output_parser	t_args_output_parser;
typedef struct _s_args_output			t_args_output;

/* ************************************************************************** */
/*                                 Enums                                      */
/* ************************************************************************** */

/**
 * @brief Type logique attendu pour un parametre.
 *
 * @note Ce champ est stocke dans le modele; aucune conversion automatique
 *       n'est effectuee par le parseur.
 */
enum e_param_type
{
	param_type_file,	/**< Chemin ou nom de fichier. */
	param_type_int,		/**< Entier signe. */
	param_type_uint,	/**< Entier non signe. */
	param_type_str,		/**< Chaine (defaut logique). */
};

/**
 * @brief Specifications de parametre.
 */
enum e_args_param_specs
{
	args_param_specs_require = 1 << 0,	/**< Parametre requis. */
	args_param_specs_nargs =   1 << 1,	/**< Parametre multi-valeurs. */
};

/**
 * @brief Codes d'erreur du parseur d'arguments.
 */
enum e_args_error
{
	args_error_none = 0,				/**< Pas d'erreur. */

	/* internal / programmer errors */
	args_error_internal,				/**< Erreur interne (NULL, alloc, modele). */

	/* option / command resolution */
	args_error_unknown_option,			/**< Option inconnue. */
	args_error_ambiguous_option,		/**< Option ambigue. */
	args_error_unknown_command,			/**< Sous-commande inconnue. */

	/* argument structure */
	args_error_missing_param,			/**< Parametre requis manquant. */
	args_error_extra_param,				/**< Trop de parametres. */
	args_error_missing_value,			/**< Valeur d'option manquante. */

	/* value validation */
	args_error_invalid_type,			/**< Type invalide. */
	args_error_out_of_range,			/**< Valeur hors bornes. */
	args_error_invalid_value,			/**< Valeur invalide. */

	/* usage / logic */
	args_error_conflict,				/**< Conflit entre options. */
	args_error_duplicate,				/**< Option dupliquee. */
};


/* ************************************************************************** */
/*                                 Unions                                     */
/* ************************************************************************** */
	//...

/* ************************************************************************** */
/*                                 Structs                                    */
/* ************************************************************************** */
	//...
