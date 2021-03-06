#pragma once
#ifndef _INC_ARC
#define _INC_ARC

#define VERSION "0.9.24"

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <math.h>
#include <time.h>
#include <setjmp.h>

#ifdef READLINE
#include <readline/readline.h>
#include <readline/history.h>
#endif

#ifdef _MSC_VER
#define strdup _strdup
#endif

enum type {
	T_NIL, 
	T_CONS,
	T_SYM,
	T_NUM,
	T_BUILTIN,
	T_CLOSURE,
	T_MACRO,
	T_STRING,
	T_INPUT,
	T_OUTPUT,
	T_TABLE,
	T_CHAR,
	T_CONTINUATION
};

typedef enum {
  ERROR_OK = 0, ERROR_SYNTAX, ERROR_UNBOUND, ERROR_ARGS, ERROR_TYPE, ERROR_FILE, ERROR_USER
} error;

typedef struct atom atom;
typedef error(*builtin)(atom args, atom *result);

struct atom {
	enum type type;

	union {
		double number;
		struct pair *pair;
		char *symbol;
		struct str *str;
		builtin builtin;
		FILE *fp;
		struct table *table;
		char ch;
		jmp_buf *jb;
	} value;
};

struct pair {
	struct atom car, cdr;
	char mark;
	struct pair *next;
};

struct str {
	char *value;
	char mark;
	struct str *next;
};

struct table {
	int capacity;
	int size;
	atom *data;
	char mark;
	struct table *next;
};

/* forward declarations */
error apply(atom fn, atom args, atom *result);
int listp(atom expr);
char *slurp_fp(FILE *fp);
char *slurp(const char *path);
error eval_expr(atom expr, atom env, atom *result);
void gc_mark(atom root);
void gc();
void stack_add(atom a);
void stack_restore(int saved_size);
error macex(atom expr, atom *result);
char *to_string(atom a, int write);
char *strcat_alloc(char **dst, char *src);
char *str_new();
error macex_eval(atom expr, atom *result);
error arc_load_file(const char *path);
char *get_dir_path(char *file_path);
void arc_init(char *file_path);
#ifndef READLINE
char *readline(char *prompt);
#endif
char *readline_fp(char *prompt, FILE *fp);
error read_expr(const char *input, const char **end, atom *result);
void print_expr(atom a);
void print_error(error e);
int is(atom a, atom b);
unsigned int hash_code(atom a);
atom make_table(int capacity);
void table_add(struct table *tbl, atom k, atom v);
struct pair *table_get(struct table *tbl, atom k);
struct pair *table_get_sym(struct table *tbl, char *k);
int table_set(struct table *tbl, atom k, atom v);
int table_set_sym(struct table *tbl, char *k, atom v);
void consider_gc();
/* end forward */

#define car(p) ((p).value.pair->car)
#define cdr(p) ((p).value.pair->cdr)
#define no(atom) ((atom).type == T_NIL)

/* symbols for faster execution */
extern atom sym_t, sym_quote, sym_assign, sym_fn, sym_if, sym_mac, sym_apply, sym_while, sym_cons, sym_sym, sym_string, sym_num, sym_table;
extern int arc_reader_unclosed;
extern int stack_size;

#endif
