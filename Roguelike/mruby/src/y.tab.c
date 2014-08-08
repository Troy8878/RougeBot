/* A Bison parser, made by GNU Bison 2.7.1.  */

/* Bison implementation for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2013 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

#pragma warning (disable: 4267)
#pragma warning (disable: 4244)

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.7.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* Copy the first part of user declarations.  */
/* Line 371 of yacc.c  */
#line 7 "parse.y"

#undef PARSER_DEBUG

#define YYDEBUG 1
#define YYERROR_VERBOSE 1
/*
 * Force yacc to use our memory management.  This is a little evil because
 * the macros assume that "parser_state *p" is in scope
 */
#define YYMALLOC(n)    mrb_malloc(p->mrb, (n))
#define YYFREE(o)      mrb_free(p->mrb, (o))
#define YYSTACK_USE_ALLOCA 0

#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "mruby.h"
#include "mruby/compile.h"
#include "mruby/proc.h"
#include "mruby/error.h"
#include "node.h"
#include "mrb_throw.h"

#define YYLEX_PARAM p

typedef mrb_ast_node node;
typedef struct mrb_parser_state parser_state;
typedef struct mrb_parser_heredoc_info parser_heredoc_info;

static int yyparse(parser_state *p);
static int yylex(void *lval, parser_state *p);
static void yyerror(parser_state *p, const char *s);
static void yywarn(parser_state *p, const char *s);
static void yywarning(parser_state *p, const char *s);
static void backref_error(parser_state *p, node *n);
static void tokadd(parser_state *p, int32_t c);

#ifndef isascii
#define isascii(c) (((c) & ~0x7f) == 0)
#endif

#define identchar(c) (isalnum(c) || (c) == '_' || !isascii(c))

typedef unsigned int stack_type;

#define BITSTACK_PUSH(stack, n) ((stack) = ((stack)<<1)|((n)&1))
#define BITSTACK_POP(stack)     ((stack) = (stack) >> 1)
#define BITSTACK_LEXPOP(stack)  ((stack) = ((stack) >> 1) | ((stack) & 1))
#define BITSTACK_SET_P(stack)   ((stack)&1)

#define COND_PUSH(n)    BITSTACK_PUSH(p->cond_stack, (n))
#define COND_POP()      BITSTACK_POP(p->cond_stack)
#define COND_LEXPOP()   BITSTACK_LEXPOP(p->cond_stack)
#define COND_P()        BITSTACK_SET_P(p->cond_stack)

#define CMDARG_PUSH(n)  BITSTACK_PUSH(p->cmdarg_stack, (n))
#define CMDARG_POP()    BITSTACK_POP(p->cmdarg_stack)
#define CMDARG_LEXPOP() BITSTACK_LEXPOP(p->cmdarg_stack)
#define CMDARG_P()      BITSTACK_SET_P(p->cmdarg_stack)

#define SET_LINENO(c,n) ((c)->lineno = (n))

#define sym(x) ((mrb_sym)(intptr_t)(x))
#define nsym(x) ((node*)(intptr_t)(x))

static inline mrb_sym
intern_cstr_gen(parser_state *p, const char *s)
{
  return mrb_intern_cstr(p->mrb, s);
}
#define intern_cstr(s) intern_cstr_gen(p,(s))

static inline mrb_sym
intern_gen(parser_state *p, const char *s, size_t len)
{
  return mrb_intern(p->mrb, s, len);
}
#define intern(s,len) intern_gen(p,(s),(len))

static inline mrb_sym
intern_gen_c(parser_state *p, const char c)
{
  return mrb_intern(p->mrb, &c, 1);
}
#define intern_c(c) intern_gen_c(p,(c))

static void
cons_free_gen(parser_state *p, node *cons)
{
  cons->cdr = p->cells;
  p->cells = cons;
}
#define cons_free(c) cons_free_gen(p, (c))

static void*
parser_palloc(parser_state *p, size_t size)
{
  void *m = mrb_pool_alloc(p->pool, size);

  if (!m) {
    MRB_THROW(p->jmp);
  }
  return m;
}

static node*
cons_gen(parser_state *p, node *car, node *cdr)
{
  node *c;

  if (p->cells) {
    c = p->cells;
    p->cells = p->cells->cdr;
  }
  else {
    c = (node *)parser_palloc(p, sizeof(mrb_ast_node));
  }

  c->car = car;
  c->cdr = cdr;
  c->lineno = p->lineno;
  c->filename_index = p->current_filename_index;
  return c;
}
#define cons(a,b) cons_gen(p,(a),(b))

static node*
list1_gen(parser_state *p, node *a)
{
  return cons(a, 0);
}
#define list1(a) list1_gen(p, (a))

static node*
list2_gen(parser_state *p, node *a, node *b)
{
  return cons(a, cons(b,0));
}
#define list2(a,b) list2_gen(p, (a),(b))

static node*
list3_gen(parser_state *p, node *a, node *b, node *c)
{
  return cons(a, cons(b, cons(c,0)));
}
#define list3(a,b,c) list3_gen(p, (a),(b),(c))

static node*
list4_gen(parser_state *p, node *a, node *b, node *c, node *d)
{
  return cons(a, cons(b, cons(c, cons(d, 0))));
}
#define list4(a,b,c,d) list4_gen(p, (a),(b),(c),(d))

static node*
list5_gen(parser_state *p, node *a, node *b, node *c, node *d, node *e)
{
  return cons(a, cons(b, cons(c, cons(d, cons(e, 0)))));
}
#define list5(a,b,c,d,e) list5_gen(p, (a),(b),(c),(d),(e))

static node*
list6_gen(parser_state *p, node *a, node *b, node *c, node *d, node *e, node *f)
{
  return cons(a, cons(b, cons(c, cons(d, cons(e, cons(f, 0))))));
}
#define list6(a,b,c,d,e,f) list6_gen(p, (a),(b),(c),(d),(e),(f))

static node*
append_gen(parser_state *p, node *a, node *b)
{
  node *c = a;

  if (!a) return b;
  while (c->cdr) {
    c = c->cdr;
  }
  if (b) {
    c->cdr = b;
  }
  return a;
}
#define append(a,b) append_gen(p,(a),(b))
#define push(a,b) append_gen(p,(a),list1(b))

static char*
parser_strndup(parser_state *p, const char *s, size_t len)
{
  char *b = (char *)parser_palloc(p, len+1);

  memcpy(b, s, len);
  b[len] = '\0';
  return b;
}
#define strndup(s,len) parser_strndup(p, s, len)

static char*
parser_strdup(parser_state *p, const char *s)
{
  return parser_strndup(p, s, strlen(s));
}
#undef strdup
#define strdup(s) parser_strdup(p, s)

/* xxx ----------------------------- */

static node*
local_switch(parser_state *p)
{
  node *prev = p->locals;

  p->locals = cons(0, 0);
  return prev;
}

static void
local_resume(parser_state *p, node *prev)
{
  p->locals = prev;
}

static void
local_nest(parser_state *p)
{
  p->locals = cons(0, p->locals);
}

static void
local_unnest(parser_state *p)
{
  p->locals = p->locals->cdr;
}

static int
local_var_p(parser_state *p, mrb_sym sym)
{
  node *l = p->locals;

  while (l) {
    node *n = l->car;
    while (n) {
      if (sym(n->car) == sym) return 1;
      n = n->cdr;
    }
    l = l->cdr;
  }
  return 0;
}

static void
local_add_f(parser_state *p, mrb_sym sym)
{
  p->locals->car = push(p->locals->car, nsym(sym));
}

static void
local_add(parser_state *p, mrb_sym sym)
{
  if (!local_var_p(p, sym)) {
    local_add_f(p, sym);
  }
}

/* (:scope (vars..) (prog...)) */
static node*
new_scope(parser_state *p, node *body)
{
  return cons((node*)NODE_SCOPE, cons(p->locals->car, body));
}

/* (:begin prog...) */
static node*
new_begin(parser_state *p, node *body)
{
  if (body)
    return list2((node*)NODE_BEGIN, body);
  return cons((node*)NODE_BEGIN, 0);
}

#define newline_node(n) (n)

/* (:rescue body rescue else) */
static node*
new_rescue(parser_state *p, node *body, node *resq, node *els)
{
  return list4((node*)NODE_RESCUE, body, resq, els);
}

/* (:ensure body ensure) */
static node*
new_ensure(parser_state *p, node *a, node *b)
{
  return cons((node*)NODE_ENSURE, cons(a, cons(0, b)));
}

/* (:nil) */
static node*
new_nil(parser_state *p)
{
  return list1((node*)NODE_NIL);
}

/* (:true) */
static node*
new_true(parser_state *p)
{
  return list1((node*)NODE_TRUE);
}

/* (:false) */
static node*
new_false(parser_state *p)
{
  return list1((node*)NODE_FALSE);
}

/* (:alias new old) */
static node*
new_alias(parser_state *p, mrb_sym a, mrb_sym b)
{
  return cons((node*)NODE_ALIAS, cons(nsym(a), nsym(b)));
}

/* (:if cond then else) */
static node*
new_if(parser_state *p, node *a, node *b, node *c)
{
  return list4((node*)NODE_IF, a, b, c);
}

/* (:unless cond then else) */
static node*
new_unless(parser_state *p, node *a, node *b, node *c)
{
  return list4((node*)NODE_IF, a, c, b);
}

/* (:while cond body) */
static node*
new_while(parser_state *p, node *a, node *b)
{
  return cons((node*)NODE_WHILE, cons(a, b));
}

/* (:until cond body) */
static node*
new_until(parser_state *p, node *a, node *b)
{
  return cons((node*)NODE_UNTIL, cons(a, b));
}

/* (:for var obj body) */
static node*
new_for(parser_state *p, node *v, node *o, node *b)
{
  return list4((node*)NODE_FOR, v, o, b);
}

/* (:case a ((when ...) body) ((when...) body)) */
static node*
new_case(parser_state *p, node *a, node *b)
{
  node *n = list2((node*)NODE_CASE, a);
  node *n2 = n;

  while (n2->cdr) {
    n2 = n2->cdr;
  }
  n2->cdr = b;
  return n;
}

/* (:postexe a) */
static node*
new_postexe(parser_state *p, node *a)
{
  return cons((node*)NODE_POSTEXE, a);
}

/* (:self) */
static node*
new_self(parser_state *p)
{
  return list1((node*)NODE_SELF);
}

/* (:call a b c) */
static node*
new_call(parser_state *p, node *a, mrb_sym b, node *c)
{
  return list4((node*)NODE_CALL, a, nsym(b), c);
}

/* (:fcall self mid args) */
static node*
new_fcall(parser_state *p, mrb_sym b, node *c)
{
  return list4((node*)NODE_FCALL, new_self(p), nsym(b), c);
}

/* (:super . c) */
static node*
new_super(parser_state *p, node *c)
{
  return cons((node*)NODE_SUPER, c);
}

/* (:zsuper) */
static node*
new_zsuper(parser_state *p)
{
  return list1((node*)NODE_ZSUPER);
}

/* (:yield . c) */
static node*
new_yield(parser_state *p, node *c)
{
  if (c) {
    if (c->cdr) {
      yyerror(p, "both block arg and actual block given");
    }
    return cons((node*)NODE_YIELD, c->car);
  }
  return cons((node*)NODE_YIELD, 0);
}

/* (:return . c) */
static node*
new_return(parser_state *p, node *c)
{
  return cons((node*)NODE_RETURN, c);
}

/* (:break . c) */
static node*
new_break(parser_state *p, node *c)
{
  return cons((node*)NODE_BREAK, c);
}

/* (:next . c) */
static node*
new_next(parser_state *p, node *c)
{
  return cons((node*)NODE_NEXT, c);
}

/* (:redo) */
static node*
new_redo(parser_state *p)
{
  return list1((node*)NODE_REDO);
}

/* (:retry) */
static node*
new_retry(parser_state *p)
{
  return list1((node*)NODE_RETRY);
}

/* (:dot2 a b) */
static node*
new_dot2(parser_state *p, node *a, node *b)
{
  return cons((node*)NODE_DOT2, cons(a, b));
}

/* (:dot3 a b) */
static node*
new_dot3(parser_state *p, node *a, node *b)
{
  return cons((node*)NODE_DOT3, cons(a, b));
}

/* (:colon2 b c) */
static node*
new_colon2(parser_state *p, node *b, mrb_sym c)
{
  return cons((node*)NODE_COLON2, cons(b, nsym(c)));
}

/* (:colon3 . c) */
static node*
new_colon3(parser_state *p, mrb_sym c)
{
  return cons((node*)NODE_COLON3, nsym(c));
}

/* (:and a b) */
static node*
new_and(parser_state *p, node *a, node *b)
{
  return cons((node*)NODE_AND, cons(a, b));
}

/* (:or a b) */
static node*
new_or(parser_state *p, node *a, node *b)
{
  return cons((node*)NODE_OR, cons(a, b));
}

/* (:array a...) */
static node*
new_array(parser_state *p, node *a)
{
  return cons((node*)NODE_ARRAY, a);
}

/* (:splat . a) */
static node*
new_splat(parser_state *p, node *a)
{
  return cons((node*)NODE_SPLAT, a);
}

/* (:hash (k . v) (k . v)...) */
static node*
new_hash(parser_state *p, node *a)
{
  return cons((node*)NODE_HASH, a);
}

/* (:sym . a) */
static node*
new_sym(parser_state *p, mrb_sym sym)
{
  return cons((node*)NODE_SYM, nsym(sym));
}

static mrb_sym
new_strsym(parser_state *p, node* str)
{
  const char *s = (const char*)str->cdr->car;
  size_t len = (size_t)str->cdr->cdr;

  return mrb_intern(p->mrb, s, len);
}

/* (:lvar . a) */
static node*
new_lvar(parser_state *p, mrb_sym sym)
{
  return cons((node*)NODE_LVAR, nsym(sym));
}

/* (:gvar . a) */
static node*
new_gvar(parser_state *p, mrb_sym sym)
{
  return cons((node*)NODE_GVAR, nsym(sym));
}

/* (:ivar . a) */
static node*
new_ivar(parser_state *p, mrb_sym sym)
{
  return cons((node*)NODE_IVAR, nsym(sym));
}

/* (:cvar . a) */
static node*
new_cvar(parser_state *p, mrb_sym sym)
{
  return cons((node*)NODE_CVAR, nsym(sym));
}

/* (:const . a) */
static node*
new_const(parser_state *p, mrb_sym sym)
{
  return cons((node*)NODE_CONST, nsym(sym));
}

/* (:undef a...) */
static node*
new_undef(parser_state *p, mrb_sym sym)
{
  return list2((node*)NODE_UNDEF, nsym(sym));
}

/* (:class class super body) */
static node*
new_class(parser_state *p, node *c, node *s, node *b)
{
  return list4((node*)NODE_CLASS, c, s, cons(p->locals->car, b));
}

/* (:sclass obj body) */
static node*
new_sclass(parser_state *p, node *o, node *b)
{
  return list3((node*)NODE_SCLASS, o, cons(p->locals->car, b));
}

/* (:module module body) */
static node*
new_module(parser_state *p, node *m, node *b)
{
  return list3((node*)NODE_MODULE, m, cons(p->locals->car, b));
}

/* (:def m lv (arg . body)) */
static node*
new_def(parser_state *p, mrb_sym m, node *a, node *b)
{
  return list5((node*)NODE_DEF, nsym(m), p->locals->car, a, b);
}

/* (:sdef obj m lv (arg . body)) */
static node*
new_sdef(parser_state *p, node *o, mrb_sym m, node *a, node *b)
{
  return list6((node*)NODE_SDEF, o, nsym(m), p->locals->car, a, b);
}

/* (:arg . sym) */
static node*
new_arg(parser_state *p, mrb_sym sym)
{
  return cons((node*)NODE_ARG, nsym(sym));
}

/* (m o r m2 b) */
/* m: (a b c) */
/* o: ((a . e1) (b . e2)) */
/* r: a */
/* m2: (a b c) */
/* b: a */
static node*
new_args(parser_state *p, node *m, node *opt, mrb_sym rest, node *m2, mrb_sym blk)
{
  node *n;

  n = cons(m2, nsym(blk));
  n = cons(nsym(rest), n);
  n = cons(opt, n);
  return cons(m, n);
}

/* (:block_arg . a) */
static node*
new_block_arg(parser_state *p, node *a)
{
  return cons((node*)NODE_BLOCK_ARG, a);
}

/* (:block arg body) */
static node*
new_block(parser_state *p, node *a, node *b)
{
  return list4((node*)NODE_BLOCK, p->locals->car, a, b);
}

/* (:lambda arg body) */
static node*
new_lambda(parser_state *p, node *a, node *b)
{
  return list4((node*)NODE_LAMBDA, p->locals->car, a, b);
}

/* (:asgn lhs rhs) */
static node*
new_asgn(parser_state *p, node *a, node *b)
{
  return cons((node*)NODE_ASGN, cons(a, b));
}

/* (:masgn mlhs=(pre rest post)  mrhs) */
static node*
new_masgn(parser_state *p, node *a, node *b)
{
  return cons((node*)NODE_MASGN, cons(a, b));
}

/* (:asgn lhs rhs) */
static node*
new_op_asgn(parser_state *p, node *a, mrb_sym op, node *b)
{
  return list4((node*)NODE_OP_ASGN, a, nsym(op), b);
}

/* (:int . i) */
static node*
new_int(parser_state *p, const char *s, int base)
{
  return list3((node*)NODE_INT, (node*)strdup(s), (node*)(intptr_t)base);
}

/* (:float . i) */
static node*
new_float(parser_state *p, const char *s)
{
  return cons((node*)NODE_FLOAT, (node*)strdup(s));
}

/* (:str . (s . len)) */
static node*
new_str(parser_state *p, const char *s, int len)
{
  return cons((node*)NODE_STR, cons((node*)strndup(s, len), (node*)(intptr_t)len));
}

/* (:dstr . a) */
static node*
new_dstr(parser_state *p, node *a)
{
  return cons((node*)NODE_DSTR, a);
}

/* (:str . (s . len)) */
static node*
new_xstr(parser_state *p, const char *s, int len)
{
  return cons((node*)NODE_XSTR, cons((node*)strndup(s, len), (node*)(intptr_t)len));
}

/* (:xstr . a) */
static node*
new_dxstr(parser_state *p, node *a)
{
  return cons((node*)NODE_DXSTR, a);
}

/* (:dsym . a) */
static node*
new_dsym(parser_state *p, node *a)
{
  return cons((node*)NODE_DSYM, new_dstr(p, a));
}

/* (:str . (a . a)) */
static node*
new_regx(parser_state *p, const char *p1, const char* p2)
{
  return cons((node*)NODE_REGX, cons((node*)p1, (node*)p2));
}

/* (:dregx . a) */
static node*
new_dregx(parser_state *p, node *a, node *b)
{
  return cons((node*)NODE_DREGX, cons(a, b));
}

/* (:backref . n) */
static node*
new_back_ref(parser_state *p, int n)
{
  return cons((node*)NODE_BACK_REF, (node*)(intptr_t)n);
}

/* (:nthref . n) */
static node*
new_nth_ref(parser_state *p, int n)
{
  return cons((node*)NODE_NTH_REF, (node*)(intptr_t)n);
}

/* (:heredoc . a) */
static node*
new_heredoc(parser_state *p)
{
  parser_heredoc_info *inf = (parser_heredoc_info *)parser_palloc(p, sizeof(parser_heredoc_info));
  return cons((node*)NODE_HEREDOC, (node*)inf);
}

static void
new_bv(parser_state *p, mrb_sym id)
{
}

static node*
new_literal_delim(parser_state *p)
{
  return cons((node*)NODE_LITERAL_DELIM, 0);
}

/* (:words . a) */
static node*
new_words(parser_state *p, node *a)
{
  return cons((node*)NODE_WORDS, a);
}

/* (:symbols . a) */
static node*
new_symbols(parser_state *p, node *a)
{
  return cons((node*)NODE_SYMBOLS, a);
}

/* xxx ----------------------------- */

/* (:call a op) */
static node*
call_uni_op(parser_state *p, node *recv, const char *m)
{
  return new_call(p, recv, intern_cstr(m), 0);
}

/* (:call a op b) */
static node*
call_bin_op(parser_state *p, node *recv, const char *m, node *arg1)
{
  return new_call(p, recv, intern_cstr(m), list1(list1(arg1)));
}

static void
args_with_block(parser_state *p, node *a, node *b)
{
  if (b) {
    if (a->cdr) {
      yyerror(p, "both block arg and actual block given");
    }
    a->cdr = b;
  }
}

static void
call_with_block(parser_state *p, node *a, node *b)
{
  node *n;

  if (a->car == (node*)NODE_SUPER ||
      a->car == (node*)NODE_ZSUPER) {
    if (!a->cdr) a->cdr = cons(0, b);
    else {
      args_with_block(p, a->cdr, b);
    }
  }
  else {
    n = a->cdr->cdr->cdr;
    if (!n->car) n->car = cons(0, b);
    else {
      args_with_block(p, n->car, b);
    }
  }
}

static node*
negate_lit(parser_state *p, node *n)
{
  return cons((node*)NODE_NEGATE, n);
}

static node*
cond(node *n)
{
  return n;
}

static node*
ret_args(parser_state *p, node *n)
{
  if (n->cdr) {
    yyerror(p, "block argument should not be given");
    return NULL;
  }
  if (!n->car->cdr) return n->car->car;
  return new_array(p, n->car);
}

static void
assignable(parser_state *p, node *lhs)
{
  if ((int)(intptr_t)lhs->car == NODE_LVAR) {
    local_add(p, sym(lhs->cdr));
  }
}

static node*
var_reference(parser_state *p, node *lhs)
{
  node *n;

  if ((int)(intptr_t)lhs->car == NODE_LVAR) {
    if (!local_var_p(p, sym(lhs->cdr))) {
      n = new_fcall(p, sym(lhs->cdr), 0);
      cons_free(lhs);
      return n;
    }
  }

  return lhs;
}

typedef enum mrb_string_type  string_type;

static node*
new_strterm(parser_state *p, string_type type, int term, int paren)
{
  return cons((node*)(intptr_t)type, cons((node*)0, cons((node*)(intptr_t)paren, (node*)(intptr_t)term)));
}

static void
end_strterm(parser_state *p)
{
  cons_free(p->lex_strterm->cdr->cdr);
  cons_free(p->lex_strterm->cdr);
  cons_free(p->lex_strterm);
  p->lex_strterm = NULL;
}

static parser_heredoc_info *
parsing_heredoc_inf(parser_state *p)
{
  node *nd = p->parsing_heredoc;
  if (nd == NULL)
    return NULL;
  /* mrb_assert(nd->car->car == NODE_HEREDOC); */
  return (parser_heredoc_info*)nd->car->cdr;
}

static void
heredoc_treat_nextline(parser_state *p)
{
  if (p->heredocs_from_nextline == NULL)
    return;
  if (p->parsing_heredoc == NULL) {
    node *n;
    p->parsing_heredoc = p->heredocs_from_nextline;
    p->lex_strterm_before_heredoc = p->lex_strterm;
    p->lex_strterm = new_strterm(p, parsing_heredoc_inf(p)->type, 0, 0);
    n = p->all_heredocs;
    if (n) {
      while (n->cdr)
        n = n->cdr;
      n->cdr = p->parsing_heredoc;
    }
    else {
      p->all_heredocs = p->parsing_heredoc;
    }
  }
  else {
    node *n, *m;
    m = p->heredocs_from_nextline;
    while (m->cdr)
      m = m->cdr;
    n = p->all_heredocs;
    mrb_assert(n != NULL);
    if (n == p->parsing_heredoc) {
      m->cdr = n;
      p->all_heredocs = p->heredocs_from_nextline;
      p->parsing_heredoc = p->heredocs_from_nextline;
    }
    else {
      while (n->cdr != p->parsing_heredoc) {
        n = n->cdr;
        mrb_assert(n != NULL);
      }
      m->cdr = n->cdr;
      n->cdr = p->heredocs_from_nextline;
      p->parsing_heredoc = p->heredocs_from_nextline;
    }
  }
  p->heredocs_from_nextline = NULL;
}

static void
heredoc_end(parser_state *p)
{
  p->parsing_heredoc = p->parsing_heredoc->cdr;
  if (p->parsing_heredoc == NULL) {
    p->lstate = EXPR_BEG;
    p->cmd_start = TRUE;
    end_strterm(p);
    p->lex_strterm = p->lex_strterm_before_heredoc;
    p->lex_strterm_before_heredoc = NULL;
    p->heredoc_end_now = TRUE;
  }
  else {
    /* next heredoc */
    p->lex_strterm->car = (node*)(intptr_t)parsing_heredoc_inf(p)->type;
  }
}
#define is_strterm_type(p,str_func) ((int)(intptr_t)((p)->lex_strterm->car) & (str_func))

/* xxx ----------------------------- */


/* Line 371 of yacc.c  */
#line 1053 "y.tab.c"

# ifndef YY_NULL
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULL nullptr
#  else
#   define YY_NULL 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* In a future release of Bison, this section will be replaced
   by #include "y.tab.h".  */
#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     keyword_class = 258,
     keyword_module = 259,
     keyword_def = 260,
     keyword_undef = 261,
     keyword_begin = 262,
     keyword_rescue = 263,
     keyword_ensure = 264,
     keyword_end = 265,
     keyword_if = 266,
     keyword_unless = 267,
     keyword_then = 268,
     keyword_elsif = 269,
     keyword_else = 270,
     keyword_case = 271,
     keyword_when = 272,
     keyword_while = 273,
     keyword_until = 274,
     keyword_for = 275,
     keyword_break = 276,
     keyword_next = 277,
     keyword_redo = 278,
     keyword_retry = 279,
     keyword_in = 280,
     keyword_do = 281,
     keyword_do_cond = 282,
     keyword_do_block = 283,
     keyword_do_LAMBDA = 284,
     keyword_return = 285,
     keyword_yield = 286,
     keyword_super = 287,
     keyword_self = 288,
     keyword_nil = 289,
     keyword_true = 290,
     keyword_false = 291,
     keyword_and = 292,
     keyword_or = 293,
     keyword_not = 294,
     modifier_if = 295,
     modifier_unless = 296,
     modifier_while = 297,
     modifier_until = 298,
     modifier_rescue = 299,
     keyword_alias = 300,
     keyword_BEGIN = 301,
     keyword_END = 302,
     keyword__LINE__ = 303,
     keyword__FILE__ = 304,
     keyword__ENCODING__ = 305,
     tIDENTIFIER = 306,
     tFID = 307,
     tGVAR = 308,
     tIVAR = 309,
     tCONSTANT = 310,
     tCVAR = 311,
     tLABEL = 312,
     tINTEGER = 313,
     tFLOAT = 314,
     tCHAR = 315,
     tXSTRING = 316,
     tREGEXP = 317,
     tSTRING = 318,
     tSTRING_PART = 319,
     tSTRING_MID = 320,
     tNTH_REF = 321,
     tBACK_REF = 322,
     tREGEXP_END = 323,
     tUPLUS = 324,
     tUMINUS = 325,
     tPOW = 326,
     tCMP = 327,
     tEQ = 328,
     tEQQ = 329,
     tNEQ = 330,
     tGEQ = 331,
     tLEQ = 332,
     tANDOP = 333,
     tOROP = 334,
     tMATCH = 335,
     tNMATCH = 336,
     tDOT2 = 337,
     tDOT3 = 338,
     tAREF = 339,
     tASET = 340,
     tLSHFT = 341,
     tRSHFT = 342,
     tCOLON2 = 343,
     tCOLON3 = 344,
     tOP_ASGN = 345,
     tASSOC = 346,
     tLPAREN = 347,
     tLPAREN_ARG = 348,
     tRPAREN = 349,
     tLBRACK = 350,
     tLBRACE = 351,
     tLBRACE_ARG = 352,
     tSTAR = 353,
     tAMPER = 354,
     tLAMBDA = 355,
     tSYMBEG = 356,
     tREGEXP_BEG = 357,
     tWORDS_BEG = 358,
     tSYMBOLS_BEG = 359,
     tSTRING_BEG = 360,
     tXSTRING_BEG = 361,
     tSTRING_DVAR = 362,
     tLAMBEG = 363,
     tHEREDOC_BEG = 364,
     tHEREDOC_END = 365,
     tLITERAL_DELIM = 366,
     tHD_LITERAL_DELIM = 367,
     tHD_STRING_PART = 368,
     tHD_STRING_MID = 369,
     tLOWEST = 370,
     tUMINUS_NUM = 371,
     idNULL = 372,
     idRespond_to = 373,
     idIFUNC = 374,
     idCFUNC = 375,
     id_core_set_method_alias = 376,
     id_core_set_variable_alias = 377,
     id_core_undef_method = 378,
     id_core_define_method = 379,
     id_core_define_singleton_method = 380,
     id_core_set_postexe = 381,
     tLAST_TOKEN = 382
   };
#endif
/* Tokens.  */
#define keyword_class 258
#define keyword_module 259
#define keyword_def 260
#define keyword_undef 261
#define keyword_begin 262
#define keyword_rescue 263
#define keyword_ensure 264
#define keyword_end 265
#define keyword_if 266
#define keyword_unless 267
#define keyword_then 268
#define keyword_elsif 269
#define keyword_else 270
#define keyword_case 271
#define keyword_when 272
#define keyword_while 273
#define keyword_until 274
#define keyword_for 275
#define keyword_break 276
#define keyword_next 277
#define keyword_redo 278
#define keyword_retry 279
#define keyword_in 280
#define keyword_do 281
#define keyword_do_cond 282
#define keyword_do_block 283
#define keyword_do_LAMBDA 284
#define keyword_return 285
#define keyword_yield 286
#define keyword_super 287
#define keyword_self 288
#define keyword_nil 289
#define keyword_true 290
#define keyword_false 291
#define keyword_and 292
#define keyword_or 293
#define keyword_not 294
#define modifier_if 295
#define modifier_unless 296
#define modifier_while 297
#define modifier_until 298
#define modifier_rescue 299
#define keyword_alias 300
#define keyword_BEGIN 301
#define keyword_END 302
#define keyword__LINE__ 303
#define keyword__FILE__ 304
#define keyword__ENCODING__ 305
#define tIDENTIFIER 306
#define tFID 307
#define tGVAR 308
#define tIVAR 309
#define tCONSTANT 310
#define tCVAR 311
#define tLABEL 312
#define tINTEGER 313
#define tFLOAT 314
#define tCHAR 315
#define tXSTRING 316
#define tREGEXP 317
#define tSTRING 318
#define tSTRING_PART 319
#define tSTRING_MID 320
#define tNTH_REF 321
#define tBACK_REF 322
#define tREGEXP_END 323
#define tUPLUS 324
#define tUMINUS 325
#define tPOW 326
#define tCMP 327
#define tEQ 328
#define tEQQ 329
#define tNEQ 330
#define tGEQ 331
#define tLEQ 332
#define tANDOP 333
#define tOROP 334
#define tMATCH 335
#define tNMATCH 336
#define tDOT2 337
#define tDOT3 338
#define tAREF 339
#define tASET 340
#define tLSHFT 341
#define tRSHFT 342
#define tCOLON2 343
#define tCOLON3 344
#define tOP_ASGN 345
#define tASSOC 346
#define tLPAREN 347
#define tLPAREN_ARG 348
#define tRPAREN 349
#define tLBRACK 350
#define tLBRACE 351
#define tLBRACE_ARG 352
#define tSTAR 353
#define tAMPER 354
#define tLAMBDA 355
#define tSYMBEG 356
#define tREGEXP_BEG 357
#define tWORDS_BEG 358
#define tSYMBOLS_BEG 359
#define tSTRING_BEG 360
#define tXSTRING_BEG 361
#define tSTRING_DVAR 362
#define tLAMBEG 363
#define tHEREDOC_BEG 364
#define tHEREDOC_END 365
#define tLITERAL_DELIM 366
#define tHD_LITERAL_DELIM 367
#define tHD_STRING_PART 368
#define tHD_STRING_MID 369
#define tLOWEST 370
#define tUMINUS_NUM 371
#define idNULL 372
#define idRespond_to 373
#define idIFUNC 374
#define idCFUNC 375
#define id_core_set_method_alias 376
#define id_core_set_variable_alias 377
#define id_core_undef_method 378
#define id_core_define_method 379
#define id_core_define_singleton_method 380
#define id_core_set_postexe 381
#define tLAST_TOKEN 382



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{
/* Line 387 of yacc.c  */
#line 996 "parse.y"

    node *nd;
    mrb_sym id;
    int num;
    stack_type stack;
    const struct vtable *vars;


/* Line 387 of yacc.c  */
#line 1359 "y.tab.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (parser_state *p);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */

/* Copy the second part of user declarations.  */

/* Line 390 of yacc.c  */
#line 1386 "y.tab.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef __attribute__
/* This feature is available in gcc versions 2.5 and later.  */
# if (! defined __GNUC__ || __GNUC__ < 2 \
      || (__GNUC__ == 2 && __GNUC_MINOR__ < 5))
#  define __attribute__(Spec) /* empty */
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif


/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(N) (N)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
       && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
   || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
  YYSIZE_T yynewbytes;						\
  YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
  Stack = &yyptr->Stack_alloc;					\
  yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
  yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (YYID (0))
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   11296

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  154
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  162
/* YYNRULES -- Number of rules.  */
#define YYNRULES  557
/* YYNRULES -- Number of states.  */
#define YYNSTATES  972

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   382

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     153,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   130,     2,     2,     2,   128,   123,     2,
     149,   150,   126,   124,   147,   125,   146,   127,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   118,   152,
     120,   116,   119,   117,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   145,     2,   151,   122,     2,   148,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   143,   121,   144,   131,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   129,   132,   133,   134,   135,   136,   137,   138,   139,
     140,   141,   142
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     4,     7,    10,    12,    14,    18,    21,
      23,    24,    30,    35,    38,    40,    42,    46,    49,    50,
      55,    58,    62,    66,    70,    74,    78,    83,    85,    89,
      93,   100,   106,   112,   118,   124,   128,   132,   136,   140,
     142,   146,   150,   152,   156,   160,   164,   167,   169,   171,
     173,   175,   177,   182,   183,   189,   192,   196,   201,   207,
     212,   218,   221,   224,   227,   230,   233,   235,   239,   241,
     245,   247,   250,   254,   260,   263,   268,   271,   276,   278,
     282,   284,   288,   291,   295,   297,   300,   302,   307,   311,
     315,   319,   323,   326,   328,   330,   335,   339,   343,   347,
     351,   354,   356,   358,   360,   363,   365,   369,   371,   373,
     375,   377,   379,   381,   383,   385,   386,   391,   393,   395,
     397,   399,   401,   403,   405,   407,   409,   411,   413,   415,
     417,   419,   421,   423,   425,   427,   429,   431,   433,   435,
     437,   439,   441,   443,   445,   447,   449,   451,   453,   455,
     457,   459,   461,   463,   465,   467,   469,   471,   473,   475,
     477,   479,   481,   483,   485,   487,   489,   491,   493,   495,
     497,   499,   501,   503,   505,   507,   509,   511,   513,   515,
     517,   519,   521,   523,   525,   527,   529,   533,   539,   543,
     549,   556,   562,   568,   574,   580,   585,   589,   593,   597,
     601,   605,   609,   613,   617,   621,   626,   631,   634,   637,
     641,   645,   649,   653,   657,   661,   665,   669,   673,   677,
     681,   685,   689,   692,   695,   699,   703,   707,   711,   718,
     720,   722,   724,   727,   732,   735,   739,   741,   743,   745,
     747,   750,   755,   758,   760,   763,   766,   771,   773,   774,
     777,   780,   783,   785,   787,   790,   794,   799,   804,   810,
     814,   819,   822,   824,   826,   828,   830,   832,   834,   836,
     838,   839,   844,   845,   846,   852,   853,   857,   861,   865,
     868,   872,   876,   878,   883,   887,   889,   894,   898,   901,
     903,   906,   907,   912,   919,   926,   927,   928,   936,   937,
     938,   946,   952,   957,   958,   959,   969,   970,   971,   979,
     980,   981,   982,   992,   993,   994,  1001,  1002,  1009,  1010,
    1011,  1021,  1023,  1025,  1027,  1029,  1031,  1033,  1035,  1038,
    1040,  1042,  1044,  1050,  1052,  1055,  1057,  1059,  1061,  1065,
    1067,  1071,  1073,  1078,  1085,  1089,  1095,  1098,  1103,  1105,
    1109,  1116,  1125,  1130,  1137,  1142,  1145,  1152,  1155,  1160,
    1167,  1170,  1175,  1178,  1183,  1185,  1187,  1189,  1193,  1195,
    1200,  1202,  1207,  1209,  1213,  1215,  1217,  1222,  1224,  1228,
    1232,  1233,  1239,  1242,  1247,  1253,  1259,  1262,  1267,  1272,
    1276,  1280,  1284,  1287,  1289,  1294,  1295,  1301,  1302,  1308,
    1314,  1316,  1318,  1325,  1327,  1329,  1331,  1333,  1336,  1338,
    1341,  1343,  1345,  1347,  1349,  1351,  1353,  1355,  1358,  1362,
    1364,  1367,  1369,  1370,  1375,  1377,  1380,  1383,  1387,  1390,
    1394,  1396,  1397,  1399,  1401,  1404,  1406,  1409,  1411,  1414,
    1416,  1417,  1422,  1425,  1429,  1431,  1436,  1439,  1441,  1443,
    1445,  1447,  1449,  1452,  1455,  1459,  1461,  1463,  1466,  1469,
    1471,  1473,  1475,  1477,  1479,  1481,  1483,  1485,  1487,  1489,
    1491,  1493,  1495,  1497,  1499,  1501,  1502,  1507,  1510,  1514,
    1517,  1524,  1533,  1538,  1545,  1550,  1557,  1560,  1565,  1572,
    1575,  1580,  1583,  1588,  1590,  1591,  1593,  1595,  1597,  1599,
    1601,  1603,  1605,  1609,  1611,  1615,  1618,  1621,  1624,  1626,
    1630,  1632,  1636,  1638,  1640,  1643,  1645,  1647,  1649,  1652,
    1655,  1657,  1659,  1660,  1665,  1667,  1670,  1672,  1676,  1680,
    1683,  1685,  1687,  1689,  1691,  1693,  1695,  1697,  1699,  1701,
    1703,  1705,  1707,  1708,  1710,  1711,  1713,  1716,  1719,  1720,
    1722,  1724,  1726,  1728,  1729,  1733,  1735,  1738
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     155,     0,    -1,    -1,   156,   157,    -1,   158,   306,    -1,
     315,    -1,   159,    -1,   158,   314,   159,    -1,     1,   159,
      -1,   164,    -1,    -1,    46,   160,   143,   157,   144,    -1,
     162,   251,   229,   254,    -1,   163,   306,    -1,   315,    -1,
     164,    -1,   163,   314,   164,    -1,     1,   164,    -1,    -1,
      45,   185,   165,   185,    -1,     6,   186,    -1,   164,    40,
     168,    -1,   164,    41,   168,    -1,   164,    42,   168,    -1,
     164,    43,   168,    -1,   164,    44,   164,    -1,    47,   143,
     162,   144,    -1,   166,    -1,   174,   116,   169,    -1,   276,
      90,   169,    -1,   225,   145,   195,   309,    90,   169,    -1,
     225,   146,    51,    90,   169,    -1,   225,   146,    55,    90,
     169,    -1,   225,    88,    55,    90,   169,    -1,   225,    88,
      51,    90,   169,    -1,   278,    90,   169,    -1,   181,   116,
     202,    -1,   174,   116,   191,    -1,   174,   116,   202,    -1,
     167,    -1,   181,   116,   169,    -1,   181,   116,   166,    -1,
     169,    -1,   167,    37,   167,    -1,   167,    38,   167,    -1,
      39,   307,   167,    -1,   130,   169,    -1,   190,    -1,   167,
      -1,   173,    -1,   170,    -1,   244,    -1,   244,   305,   303,
     197,    -1,    -1,    97,   172,   235,   162,   144,    -1,   302,
     197,    -1,   302,   197,   171,    -1,   225,   146,   303,   197,
      -1,   225,   146,   303,   197,   171,    -1,   225,    88,   303,
     197,    -1,   225,    88,   303,   197,   171,    -1,    32,   197,
      -1,    31,   197,    -1,    30,   196,    -1,    21,   196,    -1,
      22,   196,    -1,   176,    -1,    92,   175,   308,    -1,   176,
      -1,    92,   175,   308,    -1,   178,    -1,   178,   177,    -1,
     178,    98,   180,    -1,   178,    98,   180,   147,   179,    -1,
     178,    98,    -1,   178,    98,   147,   179,    -1,    98,   180,
      -1,    98,   180,   147,   179,    -1,    98,    -1,    98,   147,
     179,    -1,   180,    -1,    92,   175,   308,    -1,   177,   147,
      -1,   178,   177,   147,    -1,   177,    -1,   178,   177,    -1,
     275,    -1,   225,   145,   195,   309,    -1,   225,   146,    51,
      -1,   225,    88,    51,    -1,   225,   146,    55,    -1,   225,
      88,    55,    -1,    89,    55,    -1,   278,    -1,   275,    -1,
     225,   145,   195,   309,    -1,   225,   146,    51,    -1,   225,
      88,    51,    -1,   225,   146,    55,    -1,   225,    88,    55,
      -1,    89,    55,    -1,   278,    -1,    51,    -1,    55,    -1,
      89,   182,    -1,   182,    -1,   225,    88,   182,    -1,    51,
      -1,    55,    -1,    52,    -1,   188,    -1,   189,    -1,   184,
      -1,   271,    -1,   185,    -1,    -1,   186,   147,   187,   185,
      -1,   121,    -1,   122,    -1,   123,    -1,    72,    -1,    73,
      -1,    74,    -1,    80,    -1,    81,    -1,   119,    -1,    76,
      -1,   120,    -1,    77,    -1,    75,    -1,    86,    -1,    87,
      -1,   124,    -1,   125,    -1,   126,    -1,    98,    -1,   127,
      -1,   128,    -1,    71,    -1,   130,    -1,   131,    -1,    69,
      -1,    70,    -1,    84,    -1,    85,    -1,   148,    -1,    48,
      -1,    49,    -1,    50,    -1,    46,    -1,    47,    -1,    45,
      -1,    37,    -1,     7,    -1,    21,    -1,    16,    -1,     3,
      -1,     5,    -1,    26,    -1,    15,    -1,    14,    -1,    10,
      -1,     9,    -1,    36,    -1,    20,    -1,    25,    -1,     4,
      -1,    22,    -1,    34,    -1,    39,    -1,    38,    -1,    23,
      -1,     8,    -1,    24,    -1,    30,    -1,    33,    -1,    32,
      -1,    13,    -1,    35,    -1,     6,    -1,    17,    -1,    31,
      -1,    11,    -1,    12,    -1,    18,    -1,    19,    -1,   181,
     116,   190,    -1,   181,   116,   190,    44,   190,    -1,   276,
      90,   190,    -1,   276,    90,   190,    44,   190,    -1,   225,
     145,   195,   309,    90,   190,    -1,   225,   146,    51,    90,
     190,    -1,   225,   146,    55,    90,   190,    -1,   225,    88,
      51,    90,   190,    -1,   225,    88,    55,    90,   190,    -1,
      89,    55,    90,   190,    -1,   278,    90,   190,    -1,   190,
      82,   190,    -1,   190,    83,   190,    -1,   190,   124,   190,
      -1,   190,   125,   190,    -1,   190,   126,   190,    -1,   190,
     127,   190,    -1,   190,   128,   190,    -1,   190,    71,   190,
      -1,   129,    58,    71,   190,    -1,   129,    59,    71,   190,
      -1,    69,   190,    -1,    70,   190,    -1,   190,   121,   190,
      -1,   190,   122,   190,    -1,   190,   123,   190,    -1,   190,
      72,   190,    -1,   190,   119,   190,    -1,   190,    76,   190,
      -1,   190,   120,   190,    -1,   190,    77,   190,    -1,   190,
      73,   190,    -1,   190,    74,   190,    -1,   190,    75,   190,
      -1,   190,    80,   190,    -1,   190,    81,   190,    -1,   130,
     190,    -1,   131,   190,    -1,   190,    86,   190,    -1,   190,
      87,   190,    -1,   190,    78,   190,    -1,   190,    79,   190,
      -1,   190,   117,   190,   307,   118,   190,    -1,   203,    -1,
     190,    -1,   315,    -1,   201,   310,    -1,   201,   147,   300,
     310,    -1,   300,   310,    -1,   149,   195,   308,    -1,   315,
      -1,   193,    -1,   315,    -1,   196,    -1,   201,   147,    -1,
     201,   147,   300,   147,    -1,   300,   147,    -1,   173,    -1,
     201,   200,    -1,   300,   200,    -1,   201,   147,   300,   200,
      -1,   199,    -1,    -1,   198,   196,    -1,    99,   191,    -1,
     147,   199,    -1,   315,    -1,   191,    -1,    98,   191,    -1,
     201,   147,   191,    -1,   201,   147,    98,   191,    -1,   201,
     147,   264,   191,    -1,   201,   147,   264,    98,   191,    -1,
     201,   147,   191,    -1,   201,   147,    98,   191,    -1,    98,
     191,    -1,   255,    -1,   256,    -1,   260,    -1,   261,    -1,
     262,    -1,   277,    -1,   278,    -1,    52,    -1,    -1,     7,
     204,   161,    10,    -1,    -1,    -1,    93,   205,   167,   206,
     308,    -1,    -1,    93,   207,   308,    -1,    92,   162,   150,
      -1,   225,    88,    55,    -1,    89,    55,    -1,    95,   192,
     151,    -1,    96,   299,   144,    -1,    30,    -1,    31,   149,
     196,   308,    -1,    31,   149,   308,    -1,    31,    -1,    39,
     149,   167,   308,    -1,    39,   149,   308,    -1,   302,   246,
      -1,   245,    -1,   245,   246,    -1,    -1,   100,   208,   240,
     241,    -1,    11,   168,   226,   162,   228,    10,    -1,    12,
     168,   226,   162,   229,    10,    -1,    -1,    -1,    18,   209,
     168,   227,   210,   162,    10,    -1,    -1,    -1,    19,   211,
     168,   227,   212,   162,    10,    -1,    16,   168,   306,   249,
      10,    -1,    16,   306,   249,    10,    -1,    -1,    -1,    20,
     230,    25,   213,   168,   227,   214,   162,    10,    -1,    -1,
      -1,     3,   215,   183,   279,   216,   161,    10,    -1,    -1,
      -1,    -1,     3,   217,    86,   167,   218,   311,   219,   161,
      10,    -1,    -1,    -1,     4,   220,   183,   221,   161,    10,
      -1,    -1,     5,   184,   222,   281,   161,    10,    -1,    -1,
      -1,     5,   297,   305,   223,   184,   224,   281,   161,    10,
      -1,    21,    -1,    22,    -1,    23,    -1,    24,    -1,   203,
      -1,   311,    -1,    13,    -1,   311,    13,    -1,   311,    -1,
      27,    -1,   229,    -1,    14,   168,   226,   162,   228,    -1,
     315,    -1,    15,   162,    -1,   181,    -1,   174,    -1,   284,
      -1,    92,   233,   308,    -1,   231,    -1,   232,   147,   231,
      -1,   232,    -1,   232,   147,    98,   284,    -1,   232,   147,
      98,   284,   147,   232,    -1,   232,   147,    98,    -1,   232,
     147,    98,   147,   232,    -1,    98,   284,    -1,    98,   284,
     147,   232,    -1,    98,    -1,    98,   147,   232,    -1,   286,
     147,   290,   147,   293,   296,    -1,   286,   147,   290,   147,
     293,   147,   286,   296,    -1,   286,   147,   290,   296,    -1,
     286,   147,   290,   147,   286,   296,    -1,   286,   147,   293,
     296,    -1,   286,   147,    -1,   286,   147,   293,   147,   286,
     296,    -1,   286,   296,    -1,   290,   147,   293,   296,    -1,
     290,   147,   293,   147,   286,   296,    -1,   290,   296,    -1,
     290,   147,   286,   296,    -1,   293,   296,    -1,   293,   147,
     286,   296,    -1,   295,    -1,   315,    -1,   236,    -1,   121,
     237,   121,    -1,    79,    -1,   121,   234,   237,   121,    -1,
     307,    -1,   307,   152,   238,   307,    -1,   239,    -1,   238,
     147,   239,    -1,    51,    -1,   283,    -1,   149,   282,   237,
     150,    -1,   282,    -1,   108,   162,   144,    -1,    29,   162,
      10,    -1,    -1,    28,   243,   235,   162,    10,    -1,   173,
     242,    -1,   244,   305,   303,   194,    -1,   244,   305,   303,
     194,   246,    -1,   244,   305,   303,   197,   242,    -1,   302,
     193,    -1,   225,   146,   303,   194,    -1,   225,    88,   303,
     193,    -1,   225,    88,   304,    -1,   225,   146,   193,    -1,
     225,    88,   193,    -1,    32,   193,    -1,    32,    -1,   225,
     145,   195,   309,    -1,    -1,   143,   247,   235,   162,   144,
      -1,    -1,    26,   248,   235,   162,    10,    -1,    17,   201,
     226,   162,   250,    -1,   229,    -1,   249,    -1,     8,   252,
     253,   226,   162,   251,    -1,   315,    -1,   191,    -1,   202,
      -1,   315,    -1,    91,   181,    -1,   315,    -1,     9,   162,
      -1,   315,    -1,   274,    -1,   270,    -1,   269,    -1,   273,
      -1,    60,    -1,    63,    -1,   105,    63,    -1,   105,   257,
      63,    -1,   258,    -1,   257,   258,    -1,    65,    -1,    -1,
      64,   259,   162,   144,    -1,   111,    -1,   112,   264,    -1,
     106,    61,    -1,   106,   257,    61,    -1,   102,    62,    -1,
     102,   257,    62,    -1,   109,    -1,    -1,   264,    -1,   265,
      -1,   264,   265,    -1,   110,    -1,   266,   110,    -1,   267,
      -1,   266,   267,    -1,   114,    -1,    -1,   113,   268,   162,
     144,    -1,   103,    63,    -1,   103,   257,    63,    -1,   271,
      -1,   101,   105,   258,    63,    -1,   101,   272,    -1,   184,
      -1,    54,    -1,    53,    -1,    56,    -1,    63,    -1,   105,
      63,    -1,   104,    63,    -1,   104,   257,    63,    -1,    58,
      -1,    59,    -1,   129,    58,    -1,   129,    59,    -1,    51,
      -1,    54,    -1,    53,    -1,    56,    -1,    55,    -1,   275,
      -1,   275,    -1,    34,    -1,    33,    -1,    35,    -1,    36,
      -1,    49,    -1,    48,    -1,    66,    -1,    67,    -1,   311,
      -1,    -1,   120,   280,   168,   311,    -1,     1,   311,    -1,
     149,   282,   308,    -1,   282,   311,    -1,   286,   147,   291,
     147,   293,   296,    -1,   286,   147,   291,   147,   293,   147,
     286,   296,    -1,   286,   147,   291,   296,    -1,   286,   147,
     291,   147,   286,   296,    -1,   286,   147,   293,   296,    -1,
     286,   147,   293,   147,   286,   296,    -1,   286,   296,    -1,
     291,   147,   293,   296,    -1,   291,   147,   293,   147,   286,
     296,    -1,   291,   296,    -1,   291,   147,   286,   296,    -1,
     293,   296,    -1,   293,   147,   286,   296,    -1,   295,    -1,
      -1,    55,    -1,    54,    -1,    53,    -1,    56,    -1,   283,
      -1,    51,    -1,   284,    -1,    92,   233,   308,    -1,   285,
      -1,   286,   147,   285,    -1,    51,   116,    -1,   287,   191,
      -1,   287,   225,    -1,   289,    -1,   290,   147,   289,    -1,
     288,    -1,   291,   147,   288,    -1,   126,    -1,    98,    -1,
     292,    51,    -1,   292,    -1,   123,    -1,    99,    -1,   294,
      51,    -1,   147,   295,    -1,   315,    -1,   277,    -1,    -1,
     149,   298,   167,   308,    -1,   315,    -1,   300,   310,    -1,
     301,    -1,   300,   147,   301,    -1,   191,    91,   191,    -1,
      57,   191,    -1,    51,    -1,    55,    -1,    52,    -1,    51,
      -1,    55,    -1,    52,    -1,   188,    -1,    51,    -1,    52,
      -1,   188,    -1,   146,    -1,    88,    -1,    -1,   314,    -1,
      -1,   312,    -1,   307,   150,    -1,   307,   151,    -1,    -1,
     312,    -1,   147,    -1,   152,    -1,   312,    -1,    -1,   153,
     313,   263,    -1,   311,    -1,   314,   152,    -1,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,  1155,  1155,  1155,  1165,  1171,  1175,  1179,  1183,  1189,
    1191,  1190,  1202,  1228,  1234,  1238,  1242,  1246,  1252,  1252,
    1256,  1260,  1264,  1268,  1272,  1276,  1280,  1285,  1286,  1290,
    1294,  1298,  1302,  1306,  1311,  1315,  1320,  1324,  1328,  1332,
    1335,  1339,  1346,  1347,  1351,  1355,  1359,  1363,  1366,  1373,
    1374,  1377,  1378,  1382,  1381,  1394,  1398,  1403,  1407,  1412,
    1416,  1421,  1425,  1429,  1433,  1437,  1443,  1447,  1453,  1454,
    1460,  1464,  1468,  1472,  1476,  1480,  1484,  1488,  1492,  1496,
    1502,  1503,  1509,  1513,  1519,  1523,  1529,  1533,  1537,  1541,
    1545,  1549,  1555,  1561,  1568,  1572,  1576,  1580,  1584,  1588,
    1594,  1600,  1607,  1611,  1614,  1618,  1622,  1628,  1629,  1630,
    1631,  1636,  1643,  1644,  1647,  1651,  1651,  1657,  1658,  1659,
    1660,  1661,  1662,  1663,  1664,  1665,  1666,  1667,  1668,  1669,
    1670,  1671,  1672,  1673,  1674,  1675,  1676,  1677,  1678,  1679,
    1680,  1681,  1682,  1683,  1684,  1685,  1688,  1688,  1688,  1689,
    1689,  1690,  1690,  1690,  1691,  1691,  1691,  1691,  1692,  1692,
    1692,  1693,  1693,  1693,  1694,  1694,  1694,  1694,  1695,  1695,
    1695,  1695,  1696,  1696,  1696,  1696,  1697,  1697,  1697,  1697,
    1698,  1698,  1698,  1698,  1699,  1699,  1702,  1706,  1710,  1714,
    1718,  1722,  1726,  1730,  1734,  1739,  1744,  1749,  1753,  1757,
    1761,  1765,  1769,  1773,  1777,  1781,  1785,  1789,  1793,  1797,
    1801,  1805,  1809,  1813,  1817,  1821,  1825,  1829,  1833,  1837,
    1841,  1845,  1849,  1853,  1857,  1861,  1865,  1869,  1873,  1877,
    1883,  1890,  1891,  1895,  1899,  1905,  1911,  1912,  1915,  1916,
    1917,  1921,  1925,  1931,  1935,  1939,  1943,  1947,  1953,  1953,
    1964,  1970,  1974,  1980,  1984,  1988,  1992,  1996,  2000,  2006,
    2010,  2014,  2020,  2021,  2022,  2023,  2024,  2025,  2026,  2027,
    2032,  2031,  2043,  2047,  2042,  2052,  2052,  2056,  2060,  2064,
    2068,  2072,  2076,  2080,  2084,  2088,  2092,  2096,  2100,  2104,
    2105,  2111,  2110,  2123,  2130,  2137,  2137,  2137,  2143,  2143,
    2143,  2149,  2155,  2160,  2162,  2159,  2169,  2173,  2168,  2186,
    2190,  2195,  2185,  2209,  2213,  2208,  2226,  2225,  2241,  2242,
    2241,  2258,  2262,  2266,  2270,  2276,  2283,  2284,  2285,  2288,
    2289,  2292,  2293,  2301,  2302,  2308,  2312,  2315,  2319,  2325,
    2329,  2335,  2339,  2343,  2347,  2351,  2355,  2359,  2363,  2367,
    2373,  2377,  2381,  2385,  2389,  2393,  2397,  2401,  2405,  2409,
    2413,  2417,  2421,  2425,  2429,  2435,  2436,  2443,  2448,  2453,
    2460,  2464,  2470,  2471,  2474,  2479,  2482,  2486,  2492,  2496,
    2503,  2502,  2515,  2525,  2529,  2534,  2541,  2545,  2549,  2553,
    2557,  2561,  2565,  2569,  2573,  2580,  2579,  2592,  2591,  2605,
    2613,  2622,  2625,  2632,  2635,  2639,  2640,  2643,  2647,  2650,
    2654,  2657,  2658,  2659,  2660,  2663,  2664,  2665,  2669,  2675,
    2676,  2682,  2687,  2686,  2697,  2701,  2707,  2711,  2717,  2721,
    2727,  2730,  2731,  2734,  2735,  2738,  2744,  2750,  2751,  2754,
    2761,  2760,  2774,  2778,  2785,  2789,  2796,  2803,  2804,  2805,
    2806,  2807,  2811,  2817,  2821,  2827,  2828,  2829,  2833,  2839,
    2843,  2847,  2851,  2855,  2861,  2867,  2871,  2875,  2879,  2883,
    2887,  2894,  2903,  2904,  2907,  2912,  2911,  2920,  2927,  2933,
    2939,  2943,  2947,  2951,  2955,  2959,  2963,  2967,  2971,  2975,
    2979,  2983,  2987,  2991,  2996,  3002,  3007,  3012,  3017,  3024,
    3028,  3035,  3039,  3045,  3049,  3055,  3062,  3068,  3074,  3078,
    3084,  3088,  3094,  3095,  3098,  3103,  3110,  3111,  3114,  3121,
    3125,  3132,  3137,  3137,  3162,  3163,  3169,  3173,  3179,  3183,
    3189,  3190,  3191,  3194,  3195,  3196,  3197,  3200,  3201,  3202,
    3205,  3206,  3209,  3210,  3213,  3214,  3217,  3220,  3223,  3224,
    3225,  3228,  3229,  3233,  3232,  3239,  3240,  3244
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "keyword_class", "keyword_module",
  "keyword_def", "keyword_undef", "keyword_begin", "keyword_rescue",
  "keyword_ensure", "keyword_end", "keyword_if", "keyword_unless",
  "keyword_then", "keyword_elsif", "keyword_else", "keyword_case",
  "keyword_when", "keyword_while", "keyword_until", "keyword_for",
  "keyword_break", "keyword_next", "keyword_redo", "keyword_retry",
  "keyword_in", "keyword_do", "keyword_do_cond", "keyword_do_block",
  "keyword_do_LAMBDA", "keyword_return", "keyword_yield", "keyword_super",
  "keyword_self", "keyword_nil", "keyword_true", "keyword_false",
  "keyword_and", "keyword_or", "keyword_not", "modifier_if",
  "modifier_unless", "modifier_while", "modifier_until", "modifier_rescue",
  "keyword_alias", "keyword_BEGIN", "keyword_END", "keyword__LINE__",
  "keyword__FILE__", "keyword__ENCODING__", "tIDENTIFIER", "tFID", "tGVAR",
  "tIVAR", "tCONSTANT", "tCVAR", "tLABEL", "tINTEGER", "tFLOAT", "tCHAR",
  "tXSTRING", "tREGEXP", "tSTRING", "tSTRING_PART", "tSTRING_MID",
  "tNTH_REF", "tBACK_REF", "tREGEXP_END", "tUPLUS", "tUMINUS", "tPOW",
  "tCMP", "tEQ", "tEQQ", "tNEQ", "tGEQ", "tLEQ", "tANDOP", "tOROP",
  "tMATCH", "tNMATCH", "tDOT2", "tDOT3", "tAREF", "tASET", "tLSHFT",
  "tRSHFT", "tCOLON2", "tCOLON3", "tOP_ASGN", "tASSOC", "tLPAREN",
  "tLPAREN_ARG", "tRPAREN", "tLBRACK", "tLBRACE", "tLBRACE_ARG", "tSTAR",
  "tAMPER", "tLAMBDA", "tSYMBEG", "tREGEXP_BEG", "tWORDS_BEG",
  "tSYMBOLS_BEG", "tSTRING_BEG", "tXSTRING_BEG", "tSTRING_DVAR", "tLAMBEG",
  "tHEREDOC_BEG", "tHEREDOC_END", "tLITERAL_DELIM", "tHD_LITERAL_DELIM",
  "tHD_STRING_PART", "tHD_STRING_MID", "tLOWEST", "'='", "'?'", "':'",
  "'>'", "'<'", "'|'", "'^'", "'&'", "'+'", "'-'", "'*'", "'/'", "'%'",
  "tUMINUS_NUM", "'!'", "'~'", "idNULL", "idRespond_to", "idIFUNC",
  "idCFUNC", "id_core_set_method_alias", "id_core_set_variable_alias",
  "id_core_undef_method", "id_core_define_method",
  "id_core_define_singleton_method", "id_core_set_postexe", "tLAST_TOKEN",
  "'{'", "'}'", "'['", "'.'", "','", "'`'", "'('", "')'", "']'", "';'",
  "'\\n'", "$accept", "program", "$@1", "top_compstmt", "top_stmts",
  "top_stmt", "@2", "bodystmt", "compstmt", "stmts", "stmt", "$@3",
  "command_asgn", "expr", "expr_value", "command_call", "block_command",
  "cmd_brace_block", "$@4", "command", "mlhs", "mlhs_inner", "mlhs_basic",
  "mlhs_item", "mlhs_list", "mlhs_post", "mlhs_node", "lhs", "cname",
  "cpath", "fname", "fsym", "undef_list", "$@5", "op", "reswords", "arg",
  "arg_value", "aref_args", "paren_args", "opt_paren_args",
  "opt_call_args", "call_args", "command_args", "@6", "block_arg",
  "opt_block_arg", "args", "mrhs", "primary", "$@7", "$@8", "$@9", "$@10",
  "@11", "$@12", "$@13", "$@14", "$@15", "$@16", "$@17", "@18", "@19",
  "@20", "@21", "@22", "@23", "@24", "@25", "$@26", "@27", "primary_value",
  "then", "do", "if_tail", "opt_else", "for_var", "f_marg", "f_marg_list",
  "f_margs", "block_param", "opt_block_param", "block_param_def",
  "opt_bv_decl", "bv_decls", "bvar", "f_larglist", "lambda_body",
  "do_block", "$@28", "block_call", "method_call", "brace_block", "@29",
  "@30", "case_body", "cases", "opt_rescue", "exc_list", "exc_var",
  "opt_ensure", "literal", "string", "string_rep", "string_interp", "@31",
  "xstring", "regexp", "heredoc", "opt_heredoc_bodies", "heredoc_bodies",
  "heredoc_body", "heredoc_string_rep", "heredoc_string_interp", "@32",
  "words", "symbol", "basic_symbol", "sym", "symbols", "numeric",
  "variable", "var_lhs", "var_ref", "backref", "superclass", "$@33",
  "f_arglist", "f_args", "f_bad_arg", "f_norm_arg", "f_arg_item", "f_arg",
  "f_opt_asgn", "f_opt", "f_block_opt", "f_block_optarg", "f_optarg",
  "restarg_mark", "f_rest_arg", "blkarg_mark", "f_block_arg",
  "opt_f_block_arg", "singleton", "$@34", "assoc_list", "assocs", "assoc",
  "operation", "operation2", "operation3", "dot_or_colon", "opt_terms",
  "opt_nl", "rparen", "rbracket", "trailer", "term", "nl", "$@35", "terms",
  "none", YY_NULL
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353,   354,
     355,   356,   357,   358,   359,   360,   361,   362,   363,   364,
     365,   366,   367,   368,   369,   370,    61,    63,    58,    62,
      60,   124,    94,    38,    43,    45,    42,    47,    37,   371,
      33,   126,   372,   373,   374,   375,   376,   377,   378,   379,
     380,   381,   382,   123,   125,    91,    46,    44,    96,    40,
      41,    93,    59,    10
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint16 yyr1[] =
{
       0,   154,   156,   155,   157,   158,   158,   158,   158,   159,
     160,   159,   161,   162,   163,   163,   163,   163,   165,   164,
     164,   164,   164,   164,   164,   164,   164,   164,   164,   164,
     164,   164,   164,   164,   164,   164,   164,   164,   164,   164,
     166,   166,   167,   167,   167,   167,   167,   167,   168,   169,
     169,   170,   170,   172,   171,   173,   173,   173,   173,   173,
     173,   173,   173,   173,   173,   173,   174,   174,   175,   175,
     176,   176,   176,   176,   176,   176,   176,   176,   176,   176,
     177,   177,   178,   178,   179,   179,   180,   180,   180,   180,
     180,   180,   180,   180,   181,   181,   181,   181,   181,   181,
     181,   181,   182,   182,   183,   183,   183,   184,   184,   184,
     184,   184,   185,   185,   186,   187,   186,   188,   188,   188,
     188,   188,   188,   188,   188,   188,   188,   188,   188,   188,
     188,   188,   188,   188,   188,   188,   188,   188,   188,   188,
     188,   188,   188,   188,   188,   188,   189,   189,   189,   189,
     189,   189,   189,   189,   189,   189,   189,   189,   189,   189,
     189,   189,   189,   189,   189,   189,   189,   189,   189,   189,
     189,   189,   189,   189,   189,   189,   189,   189,   189,   189,
     189,   189,   189,   189,   189,   189,   190,   190,   190,   190,
     190,   190,   190,   190,   190,   190,   190,   190,   190,   190,
     190,   190,   190,   190,   190,   190,   190,   190,   190,   190,
     190,   190,   190,   190,   190,   190,   190,   190,   190,   190,
     190,   190,   190,   190,   190,   190,   190,   190,   190,   190,
     191,   192,   192,   192,   192,   193,   194,   194,   195,   195,
     195,   195,   195,   196,   196,   196,   196,   196,   198,   197,
     199,   200,   200,   201,   201,   201,   201,   201,   201,   202,
     202,   202,   203,   203,   203,   203,   203,   203,   203,   203,
     204,   203,   205,   206,   203,   207,   203,   203,   203,   203,
     203,   203,   203,   203,   203,   203,   203,   203,   203,   203,
     203,   208,   203,   203,   203,   209,   210,   203,   211,   212,
     203,   203,   203,   213,   214,   203,   215,   216,   203,   217,
     218,   219,   203,   220,   221,   203,   222,   203,   223,   224,
     203,   203,   203,   203,   203,   225,   226,   226,   226,   227,
     227,   228,   228,   229,   229,   230,   230,   231,   231,   232,
     232,   233,   233,   233,   233,   233,   233,   233,   233,   233,
     234,   234,   234,   234,   234,   234,   234,   234,   234,   234,
     234,   234,   234,   234,   234,   235,   235,   236,   236,   236,
     237,   237,   238,   238,   239,   239,   240,   240,   241,   241,
     243,   242,   244,   244,   244,   244,   245,   245,   245,   245,
     245,   245,   245,   245,   245,   247,   246,   248,   246,   249,
     250,   250,   251,   251,   252,   252,   252,   253,   253,   254,
     254,   255,   255,   255,   255,   256,   256,   256,   256,   257,
     257,   258,   259,   258,   258,   258,   260,   260,   261,   261,
     262,   263,   263,   264,   264,   265,   265,   266,   266,   267,
     268,   267,   269,   269,   270,   270,   271,   272,   272,   272,
     272,   272,   272,   273,   273,   274,   274,   274,   274,   275,
     275,   275,   275,   275,   276,   277,   277,   277,   277,   277,
     277,   277,   278,   278,   279,   280,   279,   279,   281,   281,
     282,   282,   282,   282,   282,   282,   282,   282,   282,   282,
     282,   282,   282,   282,   282,   283,   283,   283,   283,   284,
     284,   285,   285,   286,   286,   287,   288,   289,   290,   290,
     291,   291,   292,   292,   293,   293,   294,   294,   295,   296,
     296,   297,   298,   297,   299,   299,   300,   300,   301,   301,
     302,   302,   302,   303,   303,   303,   303,   304,   304,   304,
     305,   305,   306,   306,   307,   307,   308,   309,   310,   310,
     310,   311,   311,   313,   312,   314,   314,   315
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     2,     2,     1,     1,     3,     2,     1,
       0,     5,     4,     2,     1,     1,     3,     2,     0,     4,
       2,     3,     3,     3,     3,     3,     4,     1,     3,     3,
       6,     5,     5,     5,     5,     3,     3,     3,     3,     1,
       3,     3,     1,     3,     3,     3,     2,     1,     1,     1,
       1,     1,     4,     0,     5,     2,     3,     4,     5,     4,
       5,     2,     2,     2,     2,     2,     1,     3,     1,     3,
       1,     2,     3,     5,     2,     4,     2,     4,     1,     3,
       1,     3,     2,     3,     1,     2,     1,     4,     3,     3,
       3,     3,     2,     1,     1,     4,     3,     3,     3,     3,
       2,     1,     1,     1,     2,     1,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     0,     4,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     3,     5,     3,     5,
       6,     5,     5,     5,     5,     4,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     4,     4,     2,     2,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     2,     2,     3,     3,     3,     3,     6,     1,
       1,     1,     2,     4,     2,     3,     1,     1,     1,     1,
       2,     4,     2,     1,     2,     2,     4,     1,     0,     2,
       2,     2,     1,     1,     2,     3,     4,     4,     5,     3,
       4,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       0,     4,     0,     0,     5,     0,     3,     3,     3,     2,
       3,     3,     1,     4,     3,     1,     4,     3,     2,     1,
       2,     0,     4,     6,     6,     0,     0,     7,     0,     0,
       7,     5,     4,     0,     0,     9,     0,     0,     7,     0,
       0,     0,     9,     0,     0,     6,     0,     6,     0,     0,
       9,     1,     1,     1,     1,     1,     1,     1,     2,     1,
       1,     1,     5,     1,     2,     1,     1,     1,     3,     1,
       3,     1,     4,     6,     3,     5,     2,     4,     1,     3,
       6,     8,     4,     6,     4,     2,     6,     2,     4,     6,
       2,     4,     2,     4,     1,     1,     1,     3,     1,     4,
       1,     4,     1,     3,     1,     1,     4,     1,     3,     3,
       0,     5,     2,     4,     5,     5,     2,     4,     4,     3,
       3,     3,     2,     1,     4,     0,     5,     0,     5,     5,
       1,     1,     6,     1,     1,     1,     1,     2,     1,     2,
       1,     1,     1,     1,     1,     1,     1,     2,     3,     1,
       2,     1,     0,     4,     1,     2,     2,     3,     2,     3,
       1,     0,     1,     1,     2,     1,     2,     1,     2,     1,
       0,     4,     2,     3,     1,     4,     2,     1,     1,     1,
       1,     1,     2,     2,     3,     1,     1,     2,     2,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     0,     4,     2,     3,     2,
       6,     8,     4,     6,     4,     6,     2,     4,     6,     2,
       4,     2,     4,     1,     0,     1,     1,     1,     1,     1,
       1,     1,     3,     1,     3,     2,     2,     2,     1,     3,
       1,     3,     1,     1,     2,     1,     1,     1,     2,     2,
       1,     1,     0,     4,     1,     2,     1,     3,     3,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     0,     1,     0,     1,     2,     2,     0,     1,
       1,     1,     1,     0,     3,     1,     2,     0
};

/* YYDEFACT[STATE-NAME] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       2,     0,     0,     1,     0,   306,   313,     0,     0,   270,
       0,     0,   542,   295,   298,     0,   321,   322,   323,   324,
     282,   248,   248,   467,   466,   468,   469,   544,     0,    10,
       0,   471,   470,   459,   532,   461,   460,   463,   462,   455,
     456,   415,   416,   472,   473,     0,     0,     0,     0,   272,
     557,   557,    78,   291,     0,     0,     0,     0,     0,     0,
     430,     0,     0,     0,     3,   542,     6,     9,    27,    39,
      42,    50,    49,     0,    66,     0,    70,    80,     0,    47,
     229,     0,    51,   289,   262,   263,   264,   265,   266,   413,
     412,   444,   414,   411,   465,     0,   267,   268,   248,     5,
       8,     0,     0,     0,   156,   166,   157,   179,   153,   172,
     162,   161,   182,   183,   177,   160,   159,   155,   180,   184,
     185,   164,   154,   167,   171,   173,   165,   158,   174,   181,
     176,   175,   168,   178,   163,   152,   170,   169,   151,   149,
     150,   146,   147,   148,   107,   109,   108,   141,   142,   138,
     120,   121,   122,   129,   126,   128,   123,   124,   143,   144,
     130,   131,   135,   125,   127,   117,   118,   119,   132,   133,
     134,   136,   137,   139,   140,   145,   522,   316,   110,   111,
     465,   521,     0,   175,   168,   178,   163,   146,   147,   107,
     108,     0,   112,   114,    20,   113,     0,     0,     0,    48,
       0,     0,     0,   465,     0,   268,     0,   551,   553,   542,
       0,   555,   552,   543,     0,     0,   321,   322,   282,   285,
     393,     0,     0,     0,   336,   335,   325,     0,     0,   465,
     268,     0,     0,     0,     0,     0,   243,   230,   253,    64,
     247,   557,   557,   526,    65,    63,   544,    62,     0,   557,
     392,    61,   544,     0,   545,    18,     0,     0,   207,     0,
     208,   279,     0,     0,     0,   542,    15,   544,    68,    14,
       0,   544,     0,   548,   548,   231,     0,     0,   548,   524,
       0,     0,    76,     0,    86,    93,   494,   449,   448,   450,
     451,     0,   447,   446,   428,   422,   421,   424,     0,     0,
     419,   442,     0,   453,     0,   417,     0,   426,     0,   457,
     458,    46,   222,   223,     4,   543,     0,     0,     0,     0,
       0,     0,     0,   380,   382,     0,    82,     0,    74,    71,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   557,     0,
     541,   540,     0,   397,   395,   290,     0,     0,   386,    55,
     288,   102,   103,     0,   105,     0,     0,   268,     0,   314,
       0,   494,   318,     0,   115,     0,   557,   279,   327,     0,
     326,     0,     0,   557,     0,     0,     0,     0,   431,     0,
       0,     0,   556,     0,     0,   279,   457,   458,     0,   557,
       0,   303,   529,   254,   250,     0,     0,   244,   252,     0,
     245,   544,     0,   284,   249,   544,   239,   557,   557,   238,
     544,   287,    45,     0,     0,     0,     0,     0,     0,    17,
     544,   277,    13,   543,    67,   273,   276,   280,   550,   232,
     549,   550,   234,   281,   525,    92,    84,     0,    79,     0,
       0,   557,     0,   500,   497,   496,   495,   498,     0,   513,
     517,   516,   512,   494,     0,   377,   499,   501,   503,   557,
       0,   510,   557,   515,   557,     0,   493,   452,     0,     0,
     435,   440,   439,   425,   433,     0,   437,   429,   420,   443,
     454,   418,   427,     0,     0,     7,    21,    22,    23,    24,
      25,    43,    44,   557,     0,    28,    37,     0,    38,   544,
       0,    72,    83,    41,    40,     0,   186,   253,    36,   204,
     212,   217,   218,   219,   214,   216,   226,   227,   220,   221,
     197,   198,   224,   225,   544,   213,   215,   209,   210,   211,
     199,   200,   201,   202,   203,   533,   538,   534,   539,   391,
     248,   389,   544,   533,   535,   534,   536,   390,   248,   533,
     534,   248,   557,   557,    29,   188,    35,   196,    53,    56,
     102,   103,   104,     0,   475,   307,   474,     0,   557,     0,
     310,     0,   544,   494,     0,     0,     0,     0,   271,   557,
     557,   403,   557,   328,   186,   537,   534,   544,   533,   534,
     557,   554,   432,     0,     0,   302,   330,   296,   329,   299,
     537,   278,     0,   544,   533,   534,   557,     0,   528,     0,
     255,   251,     0,   557,   527,   283,   546,   235,   240,   242,
     286,    19,     0,    26,   195,    69,    16,   544,   548,    85,
      77,    89,    91,   544,   533,   534,   505,   500,     0,   348,
     339,   341,   544,   337,   544,     0,     0,   292,     0,   486,
     520,   506,     0,   489,   514,     0,   491,   518,   445,     0,
       0,   434,   436,   438,   205,   206,   368,   544,     0,   366,
     365,   261,     0,    81,    75,     0,     0,     0,     0,     0,
       0,   388,    59,     0,   394,     0,     0,   237,   387,    57,
     236,   383,    52,     0,     0,     0,   557,   477,     0,     0,
     102,   103,   106,   544,     0,     0,   523,   544,     0,   479,
     319,   116,   404,   405,   557,   406,     0,   557,   333,     0,
       0,   331,     0,     0,   394,     0,     0,     0,   301,     0,
       0,     0,     0,   394,     0,   256,     0,   257,   246,   557,
      11,   274,   233,    87,   544,     0,   346,     0,   502,     0,
     370,     0,     0,   504,   557,   557,   519,   557,   511,   557,
     557,   423,     0,   544,     0,   557,     0,   508,   557,   557,
     364,     0,     0,   259,    73,   187,     0,    34,   193,    33,
     194,    60,   547,     0,    31,   191,    32,   192,    58,   384,
     385,     0,     0,   189,     0,     0,     0,   394,   311,   315,
     478,   317,   494,     0,     0,   408,   334,     0,    12,   410,
       0,   293,     0,   294,   255,   557,     0,     0,   304,   258,
     241,   338,   349,     0,   344,   340,   376,     0,   379,   378,
       0,   482,     0,   484,     0,   490,     0,   487,   492,   441,
       0,   367,   355,   357,     0,   507,     0,   360,     0,   362,
     381,   260,   228,    30,   190,   398,   396,     0,   476,   308,
       0,     0,     0,   407,     0,    94,   101,     0,   409,     0,
     400,   401,   399,   297,   300,     0,     0,   347,     0,   342,
     374,   544,   372,   375,   557,   557,   557,   557,   369,   557,
     557,   279,     0,   557,   509,   557,   557,    54,     0,     0,
     100,     0,   557,     0,   557,   557,     0,   345,     0,     0,
     371,   483,     0,   480,   485,   488,     0,   352,     0,   354,
     537,   278,   361,     0,   358,   363,   312,   320,   537,    99,
     544,   533,   534,   402,   332,   305,   343,   373,   557,   557,
     557,   557,   557,   394,   481,   353,     0,   350,   356,   359,
     557,   351
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,    64,    65,    66,   256,   385,   386,   265,
     266,   433,    68,    69,   200,    70,    71,   579,   716,    72,
      73,   267,    74,    75,    76,   458,    77,   201,   374,   375,
     192,   193,   194,   597,   178,   179,    79,   238,   272,   559,
     708,   425,   426,   247,   248,   240,   417,   427,   518,    80,
     196,   270,   647,   271,   286,   214,   751,   215,   752,   627,
     895,   101,   719,   102,   724,   880,   103,   591,   381,   596,
     822,   259,   389,   617,   740,   741,   228,   660,   661,   662,
     783,   688,   689,   769,   901,   902,   474,   667,   324,   513,
      82,    83,   370,   573,   572,   401,   892,   600,   734,   824,
     828,    84,    85,   299,   300,   489,    86,    87,    88,   611,
     632,   494,   495,   496,   680,    89,    90,    91,   293,    92,
      93,   203,   204,    96,   205,   585,   718,   594,   595,   476,
     477,   478,   479,   480,   481,   787,   788,   482,   483,   484,
     485,   776,   669,   182,   380,   277,   428,   243,   231,   622,
     561,   362,   210,   422,   423,   704,   449,   390,   254,   398,
     213,   269
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -768
static const yytype_int16 yypact[] =
{
    -768,   148,  2855,  -768,  7699,    53,  -768,  5985,  7068,  -768,
    9207,  9207,  5446,  -768,  -768,  9555,  8395,  8395,  -768,  -768,
    8395,  3554,  3110,  -768,  -768,  -768,  -768,     6,  7068,  -768,
      18,  -768,  -768,  6119,  3258,  -768,  -768,  6253,  -768,  -768,
    -768,  -768,  -768,  -768,  -768,  9323,  9323,   111,  4742,   251,
    8511,  8743,  7345,  -768,  6776,   780,   853,   886,   910,   659,
    -768,    74,  9439,  9323,  -768,   282,  -768,   772,  -768,   109,
    -768,  -768,   156,    80,  -768,   101,  9667,  -768,   143,  2646,
      26,    40,   188,    76,  -768,  -768,  -768,  -768,  -768,  -768,
    -768,  -768,  -768,  -768,    47,   201,  -768,   273,   131,  -768,
    -768,  9779,   235,  9779,  -768,  -768,  -768,  -768,  -768,  -768,
    -768,  -768,  -768,  -768,  -768,  -768,  -768,  -768,  -768,  -768,
    -768,  -768,  -768,  -768,  -768,  -768,  -768,  -768,  -768,  -768,
    -768,   198,   240,   294,   340,  -768,  -768,  -768,  -768,  -768,
    -768,   358,   366,  -768,   401,  -768,   402,  -768,  -768,  -768,
    -768,  -768,  -768,  -768,  -768,  -768,  -768,  -768,  -768,  -768,
    -768,  -768,  -768,  -768,  -768,  -768,  -768,  -768,  -768,  -768,
    -768,  -768,  -768,  -768,  -768,  -768,  -768,  -768,  -768,  -768,
    -768,  -768,   188,  -768,  -768,  -768,  -768,  -768,  -768,  -768,
    -768,  6922,  -768,  -768,   160,  -768,  3891,   257,  4895,   109,
     130,   207,    46,    29,   263,    48,   130,  -768,  -768,   282,
     355,  -768,  -768,   222,  9207,  9207,  -768,  -768,  -768,   264,
     268,   278,   406,   389,  -768,  -768,  -768,   212,   431,    50,
      65,   131,  9323,  9323,  9323,  9323,  -768,  2646,   388,  -768,
    -768,   368,   380,  -768,  -768,  -768,  5330,  -768,  8395,  8395,
    -768,  -768,  5584,  9207,  -768,  -768,   370,  5048,  -768,   221,
     459,   352,  7931,  4742,   394,   282,   772,   412,   433,  -768,
    9207,   412,   422,   265,   369,  -768,   388,   430,   369,  -768,
     522,  9891,   432,   232,   292,   342,   829,  -768,  -768,  -768,
    -768,   942,  -768,  -768,  -768,  -768,  -768,  -768,   227,   824,
    -768,  -768,  1073,  -768,  1101,  -768,  1107,  -768,   834,   510,
     516,  -768,  -768,  -768,  -768,  5700,  9207,  9207,  9207,  9207,
    7931,  9207,  9207,  -768,  -768,  8859,  -768,  4742,  7464,   447,
    8859,  9323,  9323,  9323,  9323,  9323,  9323,  9323,  9323,  9323,
    9323,  9323,  9323,  9323,  9323,  9323,  9323,  9323,  9323,  9323,
    9323,  9323,  9323,  9323,  9323,  9323,  9323,  2368,  8395, 10176,
    -768,  -768, 11148,  -768,  -768,  -768,  9439,  9439,  -768,   499,
    -768,   326,   357,   247,  -768,   102,   364,  -768,  9207,  -768,
    9207,   940,  -768,   539,  -768,   596,   603,    51,  -768,  4035,
     612,  9323, 10257,  8395, 10338,  9323,  9323,  4313,   227,   355,
    8975,   606,  -768,   172,   172,    66,  -768,  -768, 10419,  8395,
   10500,  -768,  -768,  -768,  -768,  9323,  7583,  -768,  -768,  8627,
    -768,   412,   478,  -768,  -768,   412,  -768,   494,   508,  -768,
      88,  -768,  -768,  7068,  4451,   502, 10257, 10338,  9323,   772,
     412,  -768,  -768,  5835,   517,   109,  -768,  -768,  7815,  -768,
    -768,  8743,  -768,  -768,  -768,   372,   101,  9891,  -768,  9891,
   10581,  8395, 10662,   534,  -768,  -768,  -768,  -768,  1152,  -768,
    -768,  -768,  -768,   932,    98,  -768,  -768,  -768,  -768,   524,
    9323,  -768,   525,   618,   527,   624,  -768,  -768,   617,  5048,
    -768,  -768,  -768,   227,  -768,   479,  -768,  -768,  -768,  -768,
    -768,  -768,  -768,  9323,  9323,  -768,  -768,  -768,  -768,  -768,
    -768,  -768,  -768,    41,  9323,  -768,   536,   544,  -768,   412,
    9891,   545,  -768,  -768,  -768,   570,  1005,  -768,  -768,   459,
    1963,  1963,  1963,  1963,  1032,  1032,  2537,  1225,  1963,  1963,
    2941,  2941,   583,   583,  2450,  1032,  1032,   974,   974,  1064,
     424,   424,   459,   459,   459,  3646,  6508,  3738,  6629,  -768,
     268,  -768,   412,   514,  -768,   531,  -768,  -768,  3406,  -768,
    -768,  1468,    41,    41,  -768,  1875,  -768,  2646,  -768,  -768,
    -768,   372,  -768,   282,  -768,  -768,  -768, 10743,  8395, 10824,
     109,  3891,    88,   932,  3891,   282,  7214,  7068,  -768,  9091,
     672,  -768,   540,  -768,  2305,  6387,  2962,   412,    54,   255,
     672,  -768,   227,   684,    79,  -768,  -768,  -768,  -768,  -768,
      61,   117,   268,   412,    82,    86,   268,  9207,  -768,  9323,
     388,  -768,  8047,   380,  -768,  -768,  -768,  -768,  7583,  8627,
    -768,  -768,   553,  -768,  2646,    13,   772,   412,   369,   447,
    -768,   396,   190,   412,   226,   323,  -768,  -768,  1152,   124,
    -768,   568,   412,  -768,   412,  5192,  5048,  -768,   932,  -768,
    -768,  -768,   932,  -768,  -768,   737,  -768,  -768,  -768,   572,
    5048,  -768,  -768,  -768,   459,   459,  -768,   559,  5192,  -768,
    -768,   574,  8163,  -768,  -768,  9891,  9439,  9323,   599,  9439,
    9439,  -768,   499,   577,   546,  9439,  9439,  -768,  -768,   499,
    -768,    76,   156,  5192,  5048,  9323,    41,  -768,  9207,  3891,
     396,   190,  -768,   412,   282,   715,  -768,   412,   730,  -768,
    -768,  -768,   536,  -768,   651,  -768,  4604,   744,  -768,  9207,
     736,  -768,  9323,  9323,   303,  9323,  9323,   745,  -768,  8279,
    4174,  5192,  5192,    91,   172,  -768,  9323,  -768,  -768,   609,
    -768,  -768,  -768,   362,   412,  1185,   613,  1173,  -768,   608,
     615,   754,   621,  -768,   622,   627,  -768,   629,  -768,   632,
     629,  -768,   640,   412,   664,   642, 10003,  -768,   648,   657,
    -768,   784,  9323,   658,  -768,  2646,  9323,  -768,  2646,  -768,
    2646,  -768,  -768,  9439,  -768,  2646,  -768,  2646,  -768,  -768,
    -768,   797,   674,  2646,  5048,   282,   811,  -768,  -768,  -768,
    -768,  -768,   940, 10115,   130,  -768,  -768,  5192,  -768,  -768,
     130,  -768,  9323,  -768,  -768,   139,   812,   813,  -768,  -768,
    8627,  -768,   677,  1185,   470,  -768,  -768,   647,  -768,  -768,
     932,  -768,   737,  -768,   737,  -768,   737,  -768,  -768,  -768,
     705,  -768,   932,  -768,   773,   386,   932,  -768,   737,  -768,
    -768,   683,  2646,  -768,  2646,  -768,  -768,   687,  -768,  -768,
    3891,  3891,   779,  -768,   390,   292,   342,  3891,  -768,  4035,
    -768,  -768,  -768,  -768,  -768,  5192,  1185,   677,  1185,   688,
    -768,   393,  -768,  -768,   629,   690,   629,   629,  -768,   691,
     693,  -768, 10905,   629,  -768,   699,   629,  -768,   844,   845,
     372, 10986,  8395, 11067,   603,   540,   846,   677,  1185,   647,
    -768,  -768,   737,  -768,  -768,  -768,   932,  -768,   737,  -768,
     709,   717,  -768,   737,  -768,  -768,  -768,  -768,    97,   190,
     412,   136,   140,  -768,  -768,  -768,   677,  -768,   629,   629,
     721,   629,   629,   152,  -768,  -768,   737,  -768,  -768,  -768,
     629,  -768
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -768,  -768,  -768,   439,  -768,    23,  -768,  -512,   -46,  -768,
      85,  -768,  -304,    43,     0,   -40,  -768,  -579,  -768,   -12,
     849,  -154,     2,   -63,  -214,  -408,   -14,   848,  -334,   776,
       7,   -22,  -768,  -768,  -286,  -768,  1424,   233,  -768,    -4,
     304,  -321,    84,    20,  -768,  -396,  -225,    24,  -302,    -6,
    -768,  -768,  -768,  -768,  -768,  -768,  -768,  -768,  -768,  -768,
    -768,  -768,  -768,  -768,  -768,  -768,  -768,  -768,  -768,  -768,
    -768,   481,  -185,  -379,   -49,  -541,  -768,  -703,  -708,   223,
    -768,  -488,  -768,  -594,  -768,   -42,  -768,  -768,   182,  -768,
    -768,  -768,   -73,  -768,  -768,  -380,  -768,   -27,  -768,  -768,
    -768,  -768,  -768,   503,   446,  -768,  -768,  -768,  -768,  -768,
    -217,  -430,  -768,   405,  -768,  -768,  -768,     8,  -768,  -768,
    -768,  1588,  1962,   894,  1869,  -768,  -768,    87,  -243,  -742,
    -434,  -610,     1,  -657,  -627,  -767,    44,   244,  -768,  -551,
    -768,  -285,   611,  -768,  -768,  -768,    15,  -404,  1000,  -279,
    -768,   747,    -9,   -24,   301,  -555,  -245,    12,    -5,  -768,
      -3,    -2
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -558
static const yytype_int16 yytable[] =
{
      99,   486,   264,   253,   236,   236,   255,   212,   236,   226,
     365,   206,   209,   329,   177,   634,   195,   420,   250,   613,
     631,   397,   311,   631,   211,   619,   523,   100,   528,   452,
     786,   242,   242,   454,   663,   242,   195,   562,   282,   582,
     241,   241,   251,   475,   241,   778,   226,   634,   275,   279,
     268,   650,   744,   199,   199,   199,   314,   842,   773,   737,
     212,   292,   315,   681,   845,   274,   278,   457,   753,   747,
     226,   558,   607,   566,   273,   -94,   566,   211,   560,   725,
     568,   493,   728,   571,   713,   714,   -97,    67,   623,    67,
    -101,  -100,   388,   784,   368,   226,   486,   226,   763,   914,
     239,   244,   363,   583,   245,   903,   558,   -96,   566,   440,
     -97,   -98,   694,   560,  -325,   568,   -95,   775,   369,  -464,
     686,   779,   558,   801,   566,   321,   322,   665,   357,   -67,
     808,   626,   309,   310,   392,   897,   789,  -464,   396,  -309,
     653,   438,   -99,   388,   745,   -94,   321,   322,     3,   -96,
     558,   566,   264,   -98,   736,   252,   400,   363,   626,   208,
     -81,   257,   687,   -94,  -101,   -95,   261,  -100,   817,   914,
     -96,  -325,  -325,   519,   558,   657,   566,   464,   465,   466,
     467,   612,   681,   626,   323,   358,   359,   903,   486,   860,
     927,   393,   394,   845,   -86,   212,   325,   -86,   292,   616,
     399,   212,   681,   420,   212,   786,   666,   816,   -89,   786,
    -533,   435,   -93,   -92,   403,   404,   250,   264,   456,   364,
     956,   211,   584,   778,   663,   766,   749,   368,   814,   -88,
     664,   207,   208,   -90,   236,   634,   236,   236,   -87,   418,
     418,   208,   631,   631,   773,   457,  -533,   429,   326,   -97,
     -97,   -88,   773,   722,   207,   208,   442,   199,   199,   330,
     212,   242,   443,   242,   -91,   268,  -534,   723,   450,   450,
     241,   765,   241,   450,   364,   226,   360,   211,  -278,   786,
     249,   264,   207,   208,   276,   515,  -467,   794,   -96,   -96,
     524,   366,   -98,   -98,   890,   430,   432,   733,   580,   905,
     408,   558,   581,   566,   -95,   -95,   457,   384,   486,   436,
     626,   910,   387,   445,   521,   915,   506,   507,   508,   509,
     460,   378,   226,   391,   207,   208,   574,   576,  -466,   268,
     421,   663,   424,   663,   361,  -278,  -278,   490,   505,  -534,
     491,   492,   -88,   602,  -467,   746,   236,   439,   -90,   517,
     727,   610,  -530,   395,   517,   567,   429,   409,   410,   199,
     199,   199,   199,   367,   511,   512,   393,   437,   918,   919,
     212,   -98,   400,   -88,   402,   838,   -88,   461,   462,   -88,
    -465,   236,  -468,  -531,   601,   960,  -466,   586,  -394,  -101,
     567,   429,   523,   832,   649,   963,   456,   236,   212,   212,
      67,  -275,   790,   762,  -275,   510,   567,   429,   758,   663,
     899,   641,   448,   246,  -459,   618,   618,   249,   208,   -95,
     -93,   590,  -537,   592,   614,   418,   418,   252,  -469,   750,
    -268,   633,    99,   567,   207,   208,   634,  -465,  -465,   -90,
    -468,   195,   438,   679,   631,  -463,  -471,   406,   407,   236,
    -394,   226,   587,   226,  -470,   891,   411,   456,   567,   429,
    -279,   405,   663,   648,   663,   412,   413,   414,  -100,  -530,
     -90,  -459,  -459,   -90,   912,  -530,   -90,   670,   921,   415,
     670,   457,   670,    81,  -537,    81,  -469,  -268,  -268,  -459,
    -463,   202,   202,   202,   663,   331,   227,   202,   202,   -92,
    -531,   202,  -463,  -463,  -471,  -394,  -531,  -394,  -394,   588,
     589,   690,  -470,   434,   226,   416,   451,  -279,  -279,    67,
     698,   657,   208,   464,   465,   466,   467,   419,   646,    81,
     331,   588,   589,   283,   758,   922,   923,   486,   703,  -537,
     929,  -537,  -537,   202,   441,  -533,   208,  -459,  -463,   -66,
     354,   355,   356,   431,   739,   736,   701,   283,   516,   302,
     304,   306,   308,   527,   707,   208,   710,   707,   444,   710,
     690,   690,   446,   447,   453,   731,   236,   455,   212,   459,
     702,   503,   376,   703,   376,   567,   429,   504,   709,   682,
     212,   712,   491,   492,   522,   717,   578,   735,   738,   703,
     738,   950,   487,   730,   705,   195,   598,   729,   738,   212,
     463,   599,   464,   465,   466,   467,   615,   898,   701,   771,
     772,   706,   707,   517,   710,   603,   558,   754,   636,   703,
     -96,   418,   456,   527,   782,   558,   803,   566,   809,   887,
     770,   638,   791,   450,   626,   889,   643,   -98,   628,   630,
     656,   468,   276,   759,   331,   639,   524,   469,   470,   797,
     799,   -88,   -95,   770,   -81,   804,   806,   811,   812,   674,
     199,   668,   672,   777,   675,   677,   780,    81,   -90,    81,
     678,   630,   471,  -253,   276,   472,   696,   736,   785,   226,
     826,   692,   695,   -87,   748,   202,   202,   760,   900,   703,
     464,   465,   466,   467,   835,   836,   837,   352,   353,   354,
     355,   356,   208,   671,   690,   767,   781,   796,   815,   212,
     307,  -254,   635,   295,   296,   819,   637,   202,   802,   202,
     202,   640,   825,   202,   202,   829,   818,   488,    81,   830,
     821,   645,   823,    81,    81,   498,   831,   691,   498,   212,
     498,   202,   498,   827,   498,   833,   840,   418,   846,   770,
     843,   199,   283,   873,   848,   849,   618,   847,   877,   850,
     297,   298,   670,   670,   852,   670,   854,   670,   670,   856,
     226,   888,   199,   670,   859,   861,   670,   670,   657,   862,
     464,   465,   466,   467,   870,   866,    81,   202,   202,   202,
     202,    81,   202,   202,   868,  -255,   202,   875,    81,   283,
     212,   202,   316,   317,   318,   319,   320,   226,   876,   212,
     693,   879,   893,   894,   896,   212,   908,   878,   911,   468,
    -256,   917,   732,   738,   920,   928,   470,   932,   936,   202,
     938,   924,   294,   925,   295,   296,   943,   202,   202,   926,
      78,   904,    78,   906,   946,   947,   955,   907,  -533,   202,
     471,   202,   755,   225,   224,   757,  -534,   913,   966,   916,
      81,   630,   276,   642,   202,   711,   954,   930,    81,   379,
     463,   764,   464,   465,   466,   467,   497,   957,   295,   296,
     202,   297,   298,   726,   810,   502,    78,   953,   295,   296,
     683,   181,   670,   670,   670,   670,   909,   670,   670,   881,
     236,   670,   774,   670,   670,    81,   301,   295,   296,   567,
     429,   468,   601,   738,    81,   793,   703,   469,   470,   382,
       0,     0,     0,   958,     0,   297,   298,   959,   283,   961,
     283,     0,   202,     0,   962,   297,   298,     0,   761,   303,
     295,   296,   471,     0,     0,   472,   670,   670,   670,   670,
     670,     0,     0,   768,   297,   298,     0,   970,   670,     0,
      81,     0,     0,   305,   295,   296,     0,     0,   473,     0,
       0,     0,   834,   463,     0,   464,   465,   466,   467,   839,
       0,   463,     0,   464,   465,   466,   467,   297,   298,     0,
       0,   283,    98,     0,    98,   487,   295,   296,     0,     0,
      98,    98,    98,     0,     0,     0,    98,    98,     0,     0,
      98,   297,   298,     0,   468,   871,     0,     0,   820,     0,
     469,   470,   468,     0,     0,     0,     0,     0,   469,   470,
       0,     0,     0,     0,    78,   331,    78,     0,    98,   697,
       0,     0,     0,   297,   298,   471,     0,     0,   472,     0,
     344,   345,    98,   471,     0,   841,   472,     0,     0,   202,
       0,     0,    81,   276,     0,    81,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   593,
       0,   344,   345,   673,     0,   676,     0,   351,   352,   353,
     354,   355,   356,   331,     0,    78,     0,     0,   202,     0,
      78,    78,     0,     0,     0,     0,     0,     0,   344,   345,
       0,     0,   346,     0,   347,   348,   349,   350,   351,   352,
     353,   354,   355,   356,     0,   331,   499,   295,   296,     0,
       0,     0,     0,     0,     0,     0,    81,    81,     0,     0,
     344,   345,  -230,   349,   350,   351,   352,   353,   354,   355,
     356,    81,     0,    78,   500,   295,   296,     0,    78,    81,
     501,   295,   296,     0,     0,    78,   283,   202,   525,     0,
     202,   202,     0,     0,   297,   298,   202,   202,   352,   353,
     354,   355,   356,     0,    81,    81,    98,     0,    98,   202,
      81,     0,     0,   657,     0,   464,   465,   466,   467,     0,
       0,     0,   297,   298,    98,    98,     0,    81,   297,   298,
     202,     0,     0,     0,   657,     0,   464,   465,   466,   467,
       0,    81,    81,    81,     0,     0,   657,    78,   464,   465,
     466,   467,     0,     0,   658,    78,    98,     0,    98,    98,
     659,     0,    98,    98,     0,     0,     0,    98,     0,     0,
       0,     0,    98,    98,     0,   658,     0,   865,     0,     0,
      98,   844,     0,     0,     0,     0,     0,   658,     0,     0,
       0,     0,    78,     0,   202,     0,     0,     0,     0,     0,
       0,    78,     0,     0,     0,    81,   331,   332,   333,   334,
     335,   336,   337,   338,   884,   340,   341,     0,    81,     0,
       0,   344,   345,     0,     0,    98,    98,    98,    98,    98,
      98,    98,    98,     0,     0,    98,     0,    98,     0,     0,
      98,     0,     0,     0,     0,     0,     0,    78,     0,     0,
       0,     0,     0,     0,   347,   348,   349,   350,   351,   352,
     353,   354,   355,   356,     0,     0,     0,     0,    98,     0,
       0,    81,    81,     0,     0,     0,    98,    98,    81,     0,
      81,     0,     0,     0,     0,     0,    81,     0,    98,     0,
      98,     0,     0,     0,     0,   851,   853,     0,   855,    98,
     857,   858,     0,    98,     0,     0,   863,    98,     0,   867,
     869,     0,     0,   202,     0,     0,     0,     0,     0,    98,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    98,     0,     0,     0,     0,    78,
     237,   237,    78,    98,   237,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    98,     0,     0,     0,     0,     0,     0,  -557,   258,
     260,     0,     0,     0,   237,   237,  -557,  -557,  -557,     0,
       0,  -557,  -557,  -557,     0,  -557,   312,   313,     0,    98,
       0,     0,     0,     0,  -557,  -557,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  -557,  -557,     0,  -557,  -557,
    -557,  -557,  -557,    78,    78,   931,   933,   934,   935,     0,
     937,   939,     0,     0,   942,     0,   944,   945,    78,     0,
       0,     0,     0,     0,     0,     0,    78,     0,     0,     0,
       0,     0,     0,     0,   525,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  -557,     0,     0,     0,
       0,    78,    78,     0,     0,     0,     0,    78,     0,   964,
     965,   967,   968,   969,     0,     0,     0,     0,     0,     0,
       0,   971,     0,     0,    78,     0,     0,     0,    98,     0,
      94,    98,    94,     0,    98,   180,     0,     0,    78,    78,
      78,     0,     0,   229,     0,     0,     0,     0,     0,     0,
       0,  -557,  -557,     0,  -557,     0,     0,   249,  -557,     0,
    -557,  -557,     0,     0,     0,     0,     0,    98,     0,     0,
       0,     0,     0,     0,     0,     0,    94,     0,     0,     0,
     284,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   237,   237,   237,   312,
       0,     0,    78,     0,   284,    98,    98,     0,     0,     0,
     237,   883,   237,   237,     0,    78,     0,     0,     0,     0,
      98,     0,     0,     0,     0,     0,     0,     0,    98,   180,
       0,   180,     0,     0,     0,     0,    98,     0,     0,    98,
      98,     0,     0,     0,     0,    98,    98,     0,     0,     0,
       0,     0,     0,    98,    98,     0,     0,     0,    98,    98,
       0,     0,     0,     0,     0,     0,     0,     0,    78,    78,
       0,     0,     0,     0,     0,    78,    98,    78,     0,    98,
       0,     0,     0,    78,     0,     0,     0,     0,     0,   237,
      98,    98,    98,     0,   526,   529,   530,   531,   532,   533,
     534,   535,   536,   537,   538,   539,   540,   541,   542,   543,
     544,   545,   546,   547,   548,   549,   550,   551,   552,   553,
     554,     0,   237,     0,    94,     0,    94,     0,     0,     0,
     575,   577,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    98,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    98,   604,     0,   237,     0,   575,
     577,     0,     0,     0,   237,     0,     0,    98,     0,     0,
       0,     0,     0,   237,     0,     0,     0,     0,     0,   237,
     237,     0,     0,   237,     0,    94,     0,     0,     0,     0,
      94,    94,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   644,     0,     0,     0,     0,     0,     0,   284,
       0,    97,   237,    97,     0,   237,     0,     0,     0,     0,
      98,    98,     0,     0,   230,   237,     0,    98,     0,    98,
       0,     0,     0,     0,     0,    98,     0,     0,     0,     0,
       0,     0,     0,    94,   237,     0,     0,     0,    94,     0,
       0,     0,     0,     0,     0,    94,   284,    97,     0,   715,
       0,   285,    98,     0,     0,     0,     0,   684,   685,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   237,     0,
       0,     0,     0,     0,     0,   285,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,     0,
       0,   344,   345,     0,    95,     0,    95,     0,     0,     0,
     377,     0,   377,     0,     0,     0,     0,    94,     0,     0,
       0,     0,     0,     0,     0,    94,     0,     0,     0,     0,
       0,     0,   346,     0,   347,   348,   349,   350,   351,   352,
     353,   354,   355,   356,     0,     0,     0,     0,     0,     0,
      95,     0,   237,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    94,   237,     0,     0,     0,     0,     0,     0,
       0,    94,     0,     0,   331,  -558,  -558,  -558,  -558,   336,
     337,     0,     0,  -558,  -558,   284,     0,   284,     0,   344,
     345,     0,     0,   237,     0,     0,   237,     0,     0,     0,
       0,     0,   237,   237,     0,    97,     0,    97,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    94,     0,     0,
       0,     0,   347,   348,   349,   350,   351,   352,   353,   354,
     355,   356,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   284,     0,
       0,     0,     0,     0,     0,     0,   237,     0,     0,     0,
     604,   795,     0,   798,   800,     0,    97,     0,     0,   805,
     807,    97,    97,     0,     0,     0,     0,     0,     0,   813,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     285,     0,     0,     0,     0,     0,     0,     0,    95,     0,
      95,     0,     0,     0,     0,     0,   798,   800,     0,   805,
     807,     0,     0,   237,     0,     0,     0,     0,     0,    94,
     237,     0,    94,     0,    97,     0,     0,     0,     0,    97,
       0,     0,     0,     0,     0,     0,    97,   285,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   237,     0,     0,    95,
     872,     0,     0,     0,    95,    95,     0,   874,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    94,    94,     0,   874,     0,    97,     0,
       0,     0,     0,     0,   237,     0,    97,     0,    94,     0,
       0,     0,     0,     0,     0,     0,    94,    95,     0,     0,
       0,     0,    95,   284,     0,     0,     0,     0,     0,    95,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    94,    94,    97,     0,     0,     0,    94,     0,     0,
       0,     0,    97,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    94,     0,   285,     0,   285,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    94,    94,
      94,     0,     0,     0,     0,     0,   237,     0,     0,   697,
       0,    95,     0,     0,     0,     0,     0,     0,    97,    95,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   180,     0,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   285,
       0,   344,   345,     0,     0,     0,    95,     0,     0,     0,
       0,     0,    94,     0,     0,    95,     0,     0,     0,     0,
       0,   885,     0,     0,     0,    94,     0,     0,     0,   555,
     556,     0,   346,   557,   347,   348,   349,   350,   351,   352,
     353,   354,   355,   356,     0,     0,     0,   147,   148,   149,
     150,   151,   152,   153,   154,   155,     0,     0,   156,   157,
       0,    95,   158,   159,   160,   161,     0,     0,     0,     0,
      97,     0,     0,    97,     0,     0,   162,     0,    94,    94,
       0,     0,     0,     0,     0,    94,     0,    94,     0,     0,
       0,     0,     0,    94,     0,     0,     0,   163,   164,   165,
     166,   167,   168,   169,   170,   171,   172,     0,   173,   174,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   175,   249,     0,     0,
       0,   331,   332,   333,   334,   335,   336,   337,   338,   339,
     340,   341,   342,   343,    97,    97,   344,   345,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    97,
       0,     0,     0,    95,     0,     0,    95,    97,     0,     0,
       0,     0,     0,     0,   285,     0,     0,   346,     0,   347,
     348,   349,   350,   351,   352,   353,   354,   355,   356,     0,
       0,     0,    97,    97,     0,     0,     0,     0,    97,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   208,     0,    97,     0,     0,   331,   332,
     333,   334,   335,   336,   337,     0,     0,   340,   341,    97,
      97,    97,     0,   344,   345,     0,     0,    95,    95,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    95,     0,     0,     0,     0,     0,     0,     0,
      95,     0,     0,     0,     0,   377,   347,   348,   349,   350,
     351,   352,   353,   354,   355,   356,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    95,    95,     0,     0,     0,
       0,    95,     0,    97,     0,     0,     0,     0,     0,     0,
       0,     0,   886,     0,     0,     0,    97,     0,    95,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    95,    95,    95,     0,     0,   331,   332,   333,
     334,   335,   336,   337,   338,   339,   340,   341,   342,   343,
       0,     0,   344,   345,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    97,
      97,     0,     0,     0,     0,     0,    97,     0,    97,     0,
       0,     0,     0,   346,    97,   347,   348,   349,   350,   351,
     352,   353,   354,   355,   356,     0,    95,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    95,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    95,    95,     0,     0,     0,     0,     0,    95,
       0,    95,     0,     0,     0,  -557,     4,    95,     5,     6,
       7,     8,     9,     0,     0,     0,    10,    11,     0,     0,
       0,    12,     0,    13,    14,    15,    16,    17,    18,    19,
       0,     0,     0,     0,     0,    20,    21,    22,    23,    24,
      25,    26,     0,     0,    27,     0,     0,     0,     0,     0,
      28,    29,    30,    31,    32,     0,    33,    34,    35,    36,
      37,    38,     0,    39,    40,    41,     0,     0,    42,     0,
       0,    43,    44,     0,    45,    46,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    47,     0,     0,    48,    49,     0,
      50,    51,     0,    52,     0,    53,    54,    55,    56,    57,
      58,    59,  -278,     0,    60,     0,     0,     0,     0,     0,
    -278,  -278,  -278,     0,     0,  -278,  -278,  -278,     0,  -278,
       0,     0,     0,     0,    61,    62,    63,     0,     0,  -278,
    -278,  -278,     0,     0,     0,     0,     0,     0,     0,  -278,
    -278,     0,  -278,  -278,  -278,  -278,  -278,  -557,  -557,     0,
       0,     0,   331,   332,   333,   334,   335,   336,   337,   338,
     339,   340,   341,  -558,  -558,     0,     0,   344,   345,     0,
       0,     0,     0,  -278,  -278,  -278,  -278,  -278,  -278,  -278,
    -278,  -278,  -278,  -278,  -278,  -278,     0,     0,  -278,  -278,
    -278,     0,   743,  -278,     0,     0,     0,     0,     0,  -278,
     347,   348,   349,   350,   351,   352,   353,   354,   355,   356,
    -278,     0,     0,     0,     0,     0,     0,     0,   -99,  -278,
    -278,  -278,  -278,  -278,  -278,  -278,  -278,  -278,  -278,  -278,
    -278,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  -278,  -278,  -278,  -278,
    -393,     0,  -278,  -278,  -278,  -278,     0,     0,  -393,  -393,
    -393,     0,     0,  -393,  -393,  -393,     0,  -393,     0,     0,
       0,     0,     0,     0,     0,     0,  -393,  -393,  -393,     0,
       0,     0,     0,     0,     0,     0,     0,  -393,  -393,     0,
    -393,  -393,  -393,  -393,  -393,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,
    -393,  -393,  -393,  -393,     0,     0,  -393,  -393,  -393,     0,
       0,  -393,     0,     0,     0,     0,     0,  -393,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  -393,     0,  -393,
    -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  -393,  -393,  -393,  -393,  -393,  -269,   249,
    -393,  -393,  -393,  -393,     0,     0,  -269,  -269,  -269,     0,
       0,  -269,  -269,  -269,     0,  -269,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  -269,  -269,  -269,     0,     0,
       0,     0,     0,     0,     0,  -269,  -269,     0,  -269,  -269,
    -269,  -269,  -269,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  -269,
    -269,  -269,  -269,  -269,  -269,  -269,  -269,  -269,  -269,  -269,
    -269,  -269,     0,     0,  -269,  -269,  -269,     0,     0,  -269,
       0,     0,     0,     0,     0,  -269,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  -269,     0,     0,     0,
       0,     0,     0,     0,     0,  -269,  -269,  -269,  -269,  -269,
    -269,  -269,  -269,  -269,  -269,  -269,  -269,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  -269,  -269,  -269,  -269,  -557,     0,  -269,  -269,
    -269,  -269,     0,     0,  -557,  -557,  -557,     0,     0,  -557,
    -557,  -557,     0,  -557,     0,     0,     0,     0,     0,     0,
       0,     0,  -557,  -557,  -557,     0,     0,     0,     0,     0,
       0,     0,     0,  -557,  -557,     0,  -557,  -557,  -557,  -557,
    -557,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  -557,  -557,  -557,
    -557,  -557,  -557,  -557,  -557,  -557,  -557,  -557,  -557,  -557,
       0,     0,  -557,  -557,  -557,     0,     0,  -557,     0,     0,
       0,     0,     0,  -557,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  -557,     0,  -557,  -557,  -557,  -557,  -557,
    -557,  -557,  -557,  -557,  -557,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  -557,
    -557,  -557,  -557,  -557,  -285,   249,  -557,  -557,  -557,  -557,
       0,     0,  -285,  -285,  -285,     0,     0,  -285,  -285,  -285,
       0,  -285,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  -285,  -285,     0,     0,     0,     0,     0,     0,     0,
       0,  -285,  -285,     0,  -285,  -285,  -285,  -285,  -285,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  -285,  -285,  -285,  -285,  -285,
    -285,  -285,  -285,  -285,  -285,  -285,  -285,  -285,     0,     0,
    -285,  -285,  -285,     0,     0,  -285,  -537,     0,     0,     0,
       0,  -285,     0,     0,  -537,  -537,  -537,     0,     0,     0,
    -537,  -537,     0,  -537,     0,     0,     0,     0,     0,     0,
       0,  -285,  -537,  -285,  -285,  -285,  -285,  -285,  -285,  -285,
    -285,  -285,  -285,  -537,  -537,     0,  -537,  -537,  -537,  -537,
    -537,     0,     0,     0,     0,     0,     0,     0,  -285,  -285,
    -285,  -285,     0,   246,  -285,  -285,  -285,  -285,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  -537,  -537,  -537,
    -537,  -537,  -537,  -537,  -537,  -537,  -537,  -537,  -537,  -537,
       0,     0,  -537,  -537,  -537,     0,   699,     0,  -278,     0,
       0,     0,     0,     0,     0,     0,  -278,  -278,  -278,     0,
       0,     0,  -278,  -278,     0,  -278,     0,     0,     0,     0,
       0,     0,   -97,  -537,     0,  -537,  -537,  -537,  -537,  -537,
    -537,  -537,  -537,  -537,  -537,  -278,  -278,     0,  -278,  -278,
    -278,  -278,  -278,     0,     0,     0,     0,     0,     0,  -537,
    -537,  -537,  -537,   -89,     0,     0,  -537,     0,  -537,  -537,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  -278,
    -278,  -278,  -278,  -278,  -278,  -278,  -278,  -278,  -278,  -278,
    -278,  -278,     0,     0,  -278,  -278,  -278,     0,   700,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   -99,  -278,     0,  -278,  -278,  -278,
    -278,  -278,  -278,  -278,  -278,  -278,  -278,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  -278,  -278,  -278,   -91,     0,     0,  -278,     0,
    -278,  -278,   262,     0,     5,     6,     7,     8,     9,  -557,
    -557,  -557,    10,    11,     0,     0,  -557,    12,     0,    13,
      14,    15,    16,    17,    18,    19,     0,     0,     0,     0,
       0,    20,    21,    22,    23,    24,    25,    26,     0,     0,
      27,     0,     0,     0,     0,     0,    28,     0,    30,    31,
      32,     0,    33,    34,    35,    36,    37,    38,     0,    39,
      40,    41,     0,     0,    42,     0,     0,    43,    44,     0,
      45,    46,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      47,     0,     0,    48,    49,     0,    50,    51,     0,    52,
       0,    53,    54,    55,    56,    57,    58,    59,     0,     0,
      60,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      61,    62,    63,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   262,     0,     5,     6,
       7,     8,     9,  -557,  -557,  -557,    10,    11,     0,  -557,
    -557,    12,     0,    13,    14,    15,    16,    17,    18,    19,
       0,     0,     0,     0,     0,    20,    21,    22,    23,    24,
      25,    26,     0,     0,    27,     0,     0,     0,     0,     0,
      28,     0,    30,    31,    32,     0,    33,    34,    35,    36,
      37,    38,     0,    39,    40,    41,     0,     0,    42,     0,
       0,    43,    44,     0,    45,    46,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    47,     0,     0,    48,    49,     0,
      50,    51,     0,    52,     0,    53,    54,    55,    56,    57,
      58,    59,     0,     0,    60,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    61,    62,    63,     0,     0,     0,
       0,     0,     0,     0,     0,   262,     0,     5,     6,     7,
       8,     9,     0,     0,  -557,    10,    11,  -557,  -557,  -557,
      12,  -557,    13,    14,    15,    16,    17,    18,    19,     0,
       0,     0,     0,     0,    20,    21,    22,    23,    24,    25,
      26,     0,     0,    27,     0,     0,     0,     0,     0,    28,
       0,    30,    31,    32,     0,    33,    34,    35,    36,    37,
      38,     0,    39,    40,    41,     0,     0,    42,     0,     0,
      43,    44,     0,    45,    46,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    47,     0,     0,    48,    49,     0,    50,
      51,     0,    52,     0,    53,    54,    55,    56,    57,    58,
      59,     0,     0,    60,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    61,    62,    63,     0,     0,     0,     0,
       0,     0,     0,     0,   262,     0,     5,     6,     7,     8,
       9,     0,     0,  -557,    10,    11,  -557,  -557,  -557,    12,
       0,    13,    14,    15,    16,    17,    18,    19,     0,     0,
       0,     0,     0,    20,    21,    22,    23,    24,    25,    26,
       0,     0,    27,     0,     0,     0,     0,     0,    28,     0,
      30,    31,    32,     0,    33,    34,    35,    36,    37,    38,
       0,    39,    40,    41,     0,     0,    42,     0,     0,    43,
      44,     0,    45,    46,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    47,     0,     0,    48,    49,     0,    50,    51,
       0,    52,     0,    53,    54,    55,    56,    57,    58,    59,
       0,     0,    60,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    61,    62,    63,     0,     0,     0,     0,     0,
       0,     0,     4,     0,     5,     6,     7,     8,     9,     0,
       0,     0,    10,    11,     0,  -557,  -557,    12,     0,    13,
      14,    15,    16,    17,    18,    19,     0,     0,     0,     0,
       0,    20,    21,    22,    23,    24,    25,    26,     0,     0,
      27,     0,     0,     0,     0,     0,    28,    29,    30,    31,
      32,     0,    33,    34,    35,    36,    37,    38,     0,    39,
      40,    41,     0,     0,    42,     0,     0,    43,    44,     0,
      45,    46,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      47,     0,     0,    48,    49,     0,    50,    51,     0,    52,
       0,    53,    54,    55,    56,    57,    58,    59,     0,     0,
      60,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      61,    62,    63,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  -557,     0,     0,     0,     0,
       0,     0,     0,  -557,  -557,   262,     0,     5,     6,     7,
       8,     9,     0,  -557,  -557,    10,    11,     0,     0,     0,
      12,     0,    13,    14,    15,    16,    17,    18,    19,     0,
       0,     0,     0,     0,    20,    21,    22,    23,    24,    25,
      26,     0,     0,    27,     0,     0,     0,     0,     0,    28,
       0,    30,    31,    32,     0,    33,    34,    35,    36,    37,
      38,     0,    39,    40,    41,     0,     0,    42,     0,     0,
      43,    44,     0,    45,    46,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    47,     0,     0,    48,    49,     0,    50,
      51,     0,    52,     0,    53,    54,    55,    56,    57,    58,
      59,     0,     0,    60,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    61,    62,    63,     0,     0,     0,     0,
       0,     0,     0,   262,     0,     5,     6,     7,     8,     9,
       0,     0,     0,    10,    11,     0,  -557,  -557,    12,     0,
      13,    14,    15,    16,    17,    18,    19,     0,     0,     0,
       0,     0,    20,    21,    22,    23,    24,    25,    26,     0,
       0,    27,     0,     0,     0,     0,     0,    28,     0,    30,
      31,    32,     0,    33,    34,    35,    36,    37,    38,     0,
      39,    40,    41,     0,     0,    42,     0,     0,    43,    44,
       0,    45,    46,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    47,     0,     0,   263,    49,     0,    50,    51,     0,
      52,     0,    53,    54,    55,    56,    57,    58,    59,     0,
       0,    60,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    61,    62,    63,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  -557,     0,  -557,  -557,   262,     0,     5,     6,
       7,     8,     9,     0,     0,     0,    10,    11,     0,     0,
       0,    12,     0,    13,    14,    15,    16,    17,    18,    19,
       0,     0,     0,     0,     0,    20,    21,    22,    23,    24,
      25,    26,     0,     0,    27,     0,     0,     0,     0,     0,
      28,     0,    30,    31,    32,     0,    33,    34,    35,    36,
      37,    38,     0,    39,    40,    41,     0,     0,    42,     0,
       0,    43,    44,     0,    45,    46,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    47,     0,     0,    48,    49,     0,
      50,    51,     0,    52,     0,    53,    54,    55,    56,    57,
      58,    59,     0,     0,    60,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    61,    62,    63,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  -557,     0,  -557,  -557,   262,
       0,     5,     6,     7,     8,     9,     0,     0,     0,    10,
      11,     0,     0,     0,    12,     0,    13,    14,    15,    16,
      17,    18,    19,     0,     0,     0,     0,     0,    20,    21,
      22,    23,    24,    25,    26,     0,     0,    27,     0,     0,
       0,     0,     0,    28,     0,    30,    31,    32,     0,    33,
      34,    35,    36,    37,    38,     0,    39,    40,    41,     0,
       0,    42,     0,     0,    43,    44,     0,    45,    46,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    47,     0,     0,
      48,    49,     0,    50,    51,     0,    52,     0,    53,    54,
      55,    56,    57,    58,    59,     0,     0,    60,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    61,    62,    63,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  -557,   262,     0,     5,     6,     7,     8,     9,
    -557,  -557,  -557,    10,    11,     0,     0,     0,    12,     0,
      13,    14,    15,    16,    17,    18,    19,     0,     0,     0,
       0,     0,    20,    21,    22,    23,    24,    25,    26,     0,
       0,    27,     0,     0,     0,     0,     0,    28,     0,    30,
      31,    32,     0,    33,    34,    35,    36,    37,    38,     0,
      39,    40,    41,     0,     0,    42,     0,     0,    43,    44,
       0,    45,    46,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    47,     0,     0,    48,    49,     0,    50,    51,     0,
      52,     0,    53,    54,    55,    56,    57,    58,    59,     0,
       0,    60,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    61,    62,    63,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     5,     6,     7,     0,     9,     0,     0,
       0,    10,    11,     0,  -557,  -557,    12,     0,    13,    14,
      15,    16,    17,    18,    19,     0,     0,     0,     0,     0,
      20,    21,    22,    23,    24,    25,    26,     0,     0,   221,
       0,     0,     0,     0,     0,     0,     0,     0,    31,    32,
       0,    33,    34,    35,    36,    37,    38,   232,    39,    40,
      41,     0,     0,    42,     0,     0,    43,    44,     0,    45,
      46,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   197,
       0,     0,   198,    49,     0,    50,    51,     0,   233,   234,
      53,    54,    55,    56,    57,    58,    59,     0,     0,    60,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     5,
       6,     7,     0,     9,     0,     0,     0,    10,    11,    61,
     235,    63,    12,     0,    13,    14,    15,    16,    17,    18,
      19,     0,     0,     0,     0,     0,    20,    21,    22,    23,
      24,    25,    26,   208,     0,    27,     0,     0,     0,     0,
       0,     0,     0,     0,    31,    32,     0,    33,    34,    35,
      36,    37,    38,     0,    39,    40,    41,     0,     0,    42,
       0,     0,    43,    44,     0,    45,    46,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   197,     0,     0,   198,    49,
       0,    50,    51,     0,     0,     0,    53,    54,    55,    56,
      57,    58,    59,     0,     0,    60,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    61,    62,    63,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     5,     6,     7,
       0,     9,     0,     0,     0,    10,    11,     0,   207,   208,
      12,     0,    13,    14,    15,    16,    17,    18,    19,     0,
       0,     0,     0,     0,    20,    21,    22,    23,    24,    25,
      26,     0,     0,    27,     0,     0,     0,     0,     0,     0,
       0,     0,    31,    32,     0,    33,    34,    35,    36,    37,
      38,     0,    39,    40,    41,     0,     0,    42,     0,     0,
      43,    44,     0,    45,    46,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   197,     0,     0,   198,    49,     0,    50,
      51,     0,     0,     0,    53,    54,    55,    56,    57,    58,
      59,     0,     0,    60,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     5,     6,     7,     8,     9,     0,     0,
       0,    10,    11,    61,    62,    63,    12,     0,    13,    14,
      15,    16,    17,    18,    19,     0,     0,     0,     0,     0,
      20,    21,    22,    23,    24,    25,    26,   208,     0,    27,
       0,     0,     0,     0,     0,    28,    29,    30,    31,    32,
       0,    33,    34,    35,    36,    37,    38,     0,    39,    40,
      41,     0,     0,    42,     0,     0,    43,    44,     0,    45,
      46,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    47,
       0,     0,    48,    49,     0,    50,    51,     0,    52,     0,
      53,    54,    55,    56,    57,    58,    59,     0,     0,    60,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    61,
      62,    63,     0,     0,     0,     0,     0,     0,     5,     6,
       7,     8,     9,     0,     0,     0,    10,    11,     0,     0,
       0,    12,   402,    13,    14,    15,    16,    17,    18,    19,
       0,     0,     0,     0,     0,    20,    21,    22,    23,    24,
      25,    26,     0,     0,    27,     0,     0,     0,     0,     0,
      28,     0,    30,    31,    32,     0,    33,    34,    35,    36,
      37,    38,     0,    39,    40,    41,     0,     0,    42,     0,
       0,    43,    44,     0,    45,    46,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    47,     0,     0,    48,    49,     0,
      50,    51,     0,    52,     0,    53,    54,    55,    56,    57,
      58,    59,     0,     0,    60,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    61,    62,    63,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   402,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,   127,     0,     0,     0,   128,   129,   130,   131,   132,
     133,   134,   135,   136,   137,     0,     0,     0,     0,     0,
     138,   139,   140,   141,   142,   143,   144,   145,    35,    36,
     146,    38,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   147,   148,   149,   150,   151,   152,
     153,   154,   155,     0,     0,   156,   157,     0,     0,   158,
     159,   160,   161,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   162,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   163,   164,   165,   166,   167,   168,
     169,   170,   171,   172,     0,   173,   174,     0,     0,     0,
       0,     0,  -530,  -530,  -530,     0,  -530,     0,     0,     0,
    -530,  -530,     0,   175,   176,  -530,     0,  -530,  -530,  -530,
    -530,  -530,  -530,  -530,     0,  -530,     0,     0,     0,  -530,
    -530,  -530,  -530,  -530,  -530,  -530,     0,     0,  -530,     0,
       0,     0,     0,     0,     0,     0,     0,  -530,  -530,     0,
    -530,  -530,  -530,  -530,  -530,  -530,  -530,  -530,  -530,  -530,
       0,     0,  -530,     0,     0,  -530,  -530,     0,  -530,  -530,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  -530,     0,
       0,  -530,  -530,     0,  -530,  -530,     0,  -530,  -530,  -530,
    -530,  -530,  -530,  -530,  -530,  -530,     0,     0,  -530,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  -530,  -530,
    -530,     0,     0,     0,     0,     0,  -531,  -531,  -531,     0,
    -531,     0,  -530,     0,  -531,  -531,     0,     0,  -530,  -531,
       0,  -531,  -531,  -531,  -531,  -531,  -531,  -531,     0,  -531,
       0,     0,     0,  -531,  -531,  -531,  -531,  -531,  -531,  -531,
       0,     0,  -531,     0,     0,     0,     0,     0,     0,     0,
       0,  -531,  -531,     0,  -531,  -531,  -531,  -531,  -531,  -531,
    -531,  -531,  -531,  -531,     0,     0,  -531,     0,     0,  -531,
    -531,     0,  -531,  -531,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  -531,     0,     0,  -531,  -531,     0,  -531,  -531,
       0,  -531,  -531,  -531,  -531,  -531,  -531,  -531,  -531,  -531,
       0,     0,  -531,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  -531,  -531,  -531,     0,     0,     0,     0,     0,
    -533,  -533,  -533,     0,  -533,     0,  -531,     0,  -533,  -533,
       0,     0,  -531,  -533,     0,  -533,  -533,  -533,  -533,  -533,
    -533,  -533,     0,     0,     0,     0,     0,  -533,  -533,  -533,
    -533,  -533,  -533,  -533,     0,     0,  -533,     0,     0,     0,
       0,     0,     0,     0,     0,  -533,  -533,     0,  -533,  -533,
    -533,  -533,  -533,  -533,  -533,  -533,  -533,  -533,     0,     0,
    -533,     0,     0,  -533,  -533,     0,  -533,  -533,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  -533,   742,     0,  -533,
    -533,     0,  -533,  -533,     0,  -533,  -533,  -533,  -533,  -533,
    -533,  -533,  -533,  -533,     0,     0,  -533,     0,     0,     0,
       0,     0,     0,   -97,     0,     0,     0,     0,     0,     0,
       0,  -535,  -535,  -535,     0,  -535,  -533,  -533,  -533,  -535,
    -535,     0,     0,     0,  -535,     0,  -535,  -535,  -535,  -535,
    -535,  -535,  -535,     0,     0,     0,  -533,     0,  -535,  -535,
    -535,  -535,  -535,  -535,  -535,     0,     0,  -535,     0,     0,
       0,     0,     0,     0,     0,     0,  -535,  -535,     0,  -535,
    -535,  -535,  -535,  -535,  -535,  -535,  -535,  -535,  -535,     0,
       0,  -535,     0,     0,  -535,  -535,     0,  -535,  -535,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  -535,     0,     0,
    -535,  -535,     0,  -535,  -535,     0,  -535,  -535,  -535,  -535,
    -535,  -535,  -535,  -535,  -535,     0,     0,  -535,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  -536,  -536,  -536,     0,  -536,  -535,  -535,  -535,
    -536,  -536,     0,     0,     0,  -536,     0,  -536,  -536,  -536,
    -536,  -536,  -536,  -536,     0,     0,     0,  -535,     0,  -536,
    -536,  -536,  -536,  -536,  -536,  -536,     0,     0,  -536,     0,
       0,     0,     0,     0,     0,     0,     0,  -536,  -536,     0,
    -536,  -536,  -536,  -536,  -536,  -536,  -536,  -536,  -536,  -536,
       0,     0,  -536,     0,     0,  -536,  -536,     0,  -536,  -536,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  -536,     0,
       0,  -536,  -536,     0,  -536,  -536,     0,  -536,  -536,  -536,
    -536,  -536,  -536,  -536,  -536,  -536,     0,     0,  -536,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  -536,  -536,
    -536,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  -536,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,     0,     0,     0,   128,   129,   130,   183,
     184,   185,   186,   135,   136,   137,     0,     0,     0,     0,
       0,   138,   139,   140,   187,   188,   143,   189,   145,   287,
     288,   190,   289,     0,     0,     0,     0,     0,     0,   290,
       0,     0,     0,     0,     0,   147,   148,   149,   150,   151,
     152,   153,   154,   155,     0,     0,   156,   157,     0,     0,
     158,   159,   160,   161,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   162,     0,     0,     0,     0,     0,
       0,   291,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   163,   164,   165,   166,   167,
     168,   169,   170,   171,   172,     0,   173,   174,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   175,   104,   105,   106,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   125,   126,   127,     0,
       0,     0,   128,   129,   130,   183,   184,   185,   186,   135,
     136,   137,     0,     0,     0,     0,     0,   138,   139,   140,
     187,   188,   143,   189,   145,   287,   288,   190,   289,     0,
       0,     0,     0,     0,     0,   290,     0,     0,     0,     0,
       0,   147,   148,   149,   150,   151,   152,   153,   154,   155,
       0,     0,   156,   157,     0,     0,   158,   159,   160,   161,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     162,     0,     0,     0,     0,     0,     0,   383,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   163,   164,   165,   166,   167,   168,   169,   170,   171,
     172,     0,   173,   174,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     175,   104,   105,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   122,
     123,   124,   125,   126,   127,     0,     0,     0,   128,   129,
     130,   183,   184,   185,   186,   135,   136,   137,     0,     0,
       0,     0,     0,   138,   139,   140,   187,   188,   143,   189,
     145,     0,     0,   190,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   147,   148,   149,
     150,   151,   152,   153,   154,   155,     0,     0,   156,   157,
       0,     0,   158,   159,   160,   161,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   162,     0,     0,   191,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   163,   164,   165,
     166,   167,   168,   169,   170,   171,   172,     0,   173,   174,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   175,   104,   105,   106,
     107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   122,   123,   124,   125,   126,
     127,     0,     0,     0,   128,   129,   130,   183,   184,   185,
     186,   135,   136,   137,     0,     0,     0,     0,     0,   138,
     139,   140,   187,   188,   143,   189,   145,     0,     0,   190,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   147,   148,   149,   150,   151,   152,   153,
     154,   155,     0,     0,   156,   157,     0,     0,   158,   159,
     160,   161,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   162,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   163,   164,   165,   166,   167,   168,   169,
     170,   171,   172,     0,   173,   174,     0,     0,     5,     6,
       7,     0,     9,     0,     0,     0,    10,    11,     0,     0,
       0,    12,   175,    13,    14,    15,   216,   217,    18,    19,
       0,     0,     0,     0,     0,   218,   219,   220,    23,    24,
      25,    26,     0,     0,   221,     0,     0,     0,     0,     0,
       0,     0,     0,    31,    32,     0,    33,    34,    35,    36,
      37,    38,     0,    39,    40,    41,     0,     0,    42,     0,
       0,    43,    44,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   280,     0,     0,   198,    49,     0,
      50,    51,     0,     0,     0,    53,    54,    55,    56,    57,
      58,    59,     0,     0,    60,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     5,     6,     7,
       0,     9,     0,     0,   223,    10,    11,     0,     0,     0,
      12,     0,    13,    14,    15,   216,   217,    18,    19,     0,
       0,     0,   281,     0,   218,   219,   220,    23,    24,    25,
      26,     0,     0,   221,     0,     0,     0,     0,     0,     0,
       0,     0,    31,    32,     0,    33,    34,    35,    36,    37,
      38,     0,    39,    40,    41,     0,     0,    42,     0,     0,
      43,    44,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   280,     0,     0,   198,    49,     0,    50,
      51,     0,     0,     0,    53,    54,    55,    56,    57,    58,
      59,     0,     0,    60,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     5,     6,     7,     0,
       9,     0,     0,   223,    10,    11,     0,     0,     0,    12,
       0,    13,    14,    15,   216,   217,    18,    19,     0,     0,
       0,   520,     0,   218,   219,   220,    23,    24,    25,    26,
       0,     0,   221,     0,     0,     0,     0,     0,     0,     0,
       0,    31,    32,     0,    33,    34,    35,    36,    37,    38,
     232,    39,    40,    41,     0,     0,    42,     0,     0,    43,
      44,     0,    45,    46,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   197,     0,     0,   198,    49,     0,    50,    51,
       0,   629,   234,    53,    54,    55,    56,    57,    58,    59,
       0,     0,    60,   490,     0,     0,   491,   492,     0,     0,
       0,     0,     5,     6,     7,     8,     9,     0,     0,     0,
      10,    11,    61,   235,    63,    12,     0,    13,    14,    15,
      16,    17,    18,    19,     0,     0,     0,     0,     0,    20,
      21,    22,    23,    24,    25,    26,     0,     0,    27,     0,
       0,     0,     0,     0,    28,    29,    30,    31,    32,     0,
      33,    34,    35,    36,    37,    38,     0,    39,    40,    41,
       0,     0,    42,     0,     0,    43,    44,     0,    45,    46,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    47,     0,
       0,    48,    49,     0,    50,    51,     0,    52,     0,    53,
      54,    55,    56,    57,    58,    59,     0,     0,    60,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     5,     6,
       7,     0,     9,     0,     0,     0,    10,    11,    61,    62,
      63,    12,     0,    13,    14,    15,   216,   217,    18,    19,
       0,     0,     0,     0,     0,   218,   219,   220,    23,    24,
      25,    26,     0,     0,   221,     0,     0,     0,     0,     0,
       0,     0,     0,    31,    32,     0,    33,    34,    35,    36,
      37,    38,   232,    39,    40,    41,     0,     0,    42,     0,
       0,    43,    44,     0,    45,    46,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   197,     0,     0,   198,    49,     0,
      50,    51,     0,   629,     0,    53,    54,    55,    56,    57,
      58,    59,     0,     0,    60,   490,     0,     0,   491,   492,
       0,     0,     0,     0,     5,     6,     7,     8,     9,     0,
       0,     0,    10,    11,    61,   235,    63,    12,     0,    13,
      14,    15,    16,    17,    18,    19,     0,     0,     0,     0,
       0,    20,    21,    22,    23,    24,    25,    26,     0,     0,
      27,     0,     0,     0,     0,     0,    28,     0,    30,    31,
      32,     0,    33,    34,    35,    36,    37,    38,     0,    39,
      40,    41,     0,     0,    42,     0,     0,    43,    44,     0,
      45,    46,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      47,     0,     0,    48,    49,     0,    50,    51,     0,    52,
       0,    53,    54,    55,    56,    57,    58,    59,     0,     0,
      60,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       5,     6,     7,     0,     9,     0,     0,     0,    10,    11,
      61,    62,    63,    12,     0,    13,    14,    15,   216,   217,
      18,    19,     0,     0,     0,     0,     0,   218,   219,   220,
      23,    24,    25,    26,     0,     0,   221,     0,     0,     0,
       0,     0,     0,     0,     0,    31,    32,     0,    33,    34,
      35,    36,    37,    38,     0,    39,    40,    41,     0,     0,
      42,     0,     0,    43,    44,     0,    45,    46,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   197,     0,     0,   198,
      49,     0,    50,    51,     0,   756,     0,    53,    54,    55,
      56,    57,    58,    59,     0,     0,    60,   490,     0,     0,
     491,   492,     0,     0,     0,     0,     5,     6,     7,     0,
       9,     0,     0,     0,    10,    11,    61,   235,    63,    12,
       0,    13,    14,    15,   216,   217,    18,    19,     0,     0,
       0,     0,     0,   218,   219,   220,    23,    24,    25,    26,
       0,     0,   221,     0,     0,     0,     0,     0,     0,     0,
       0,    31,    32,     0,    33,    34,    35,    36,    37,    38,
       0,    39,    40,    41,     0,     0,    42,     0,     0,    43,
      44,     0,    45,    46,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   197,     0,     0,   198,    49,     0,    50,    51,
       0,   792,     0,    53,    54,    55,    56,    57,    58,    59,
       0,     0,    60,   490,     0,     0,   491,   492,     0,     0,
       0,     0,     5,     6,     7,     0,     9,     0,     0,     0,
      10,    11,    61,   235,    63,    12,     0,    13,    14,    15,
     216,   217,    18,    19,     0,     0,     0,     0,     0,   218,
     219,   220,    23,    24,    25,    26,     0,     0,   221,     0,
       0,     0,     0,     0,     0,     0,     0,    31,    32,     0,
      33,    34,    35,    36,    37,    38,     0,    39,    40,    41,
       0,     0,    42,     0,     0,    43,    44,     0,    45,    46,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   197,     0,
       0,   198,    49,     0,    50,    51,     0,   629,     0,    53,
      54,    55,    56,    57,    58,    59,     0,     0,    60,   490,
       0,     0,   491,   492,     0,     0,     0,     0,     5,     6,
       7,     0,     9,     0,     0,     0,    10,    11,    61,   235,
      63,    12,     0,    13,    14,    15,    16,    17,    18,    19,
       0,     0,     0,     0,     0,    20,    21,    22,    23,    24,
      25,    26,     0,     0,   221,     0,     0,     0,     0,     0,
       0,     0,     0,    31,    32,     0,    33,    34,    35,    36,
      37,    38,   232,    39,    40,    41,     0,     0,    42,     0,
       0,    43,    44,     0,    45,    46,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   197,     0,     0,   198,    49,     0,
      50,    51,     0,   233,   234,    53,    54,    55,    56,    57,
      58,    59,     0,     0,    60,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     5,     6,     7,     0,     9,     0,
       0,     0,    10,    11,    61,   235,    63,    12,     0,    13,
      14,    15,   216,   217,    18,    19,     0,     0,     0,     0,
       0,   218,   219,   220,    23,    24,    25,    26,     0,     0,
     221,     0,     0,     0,     0,     0,     0,     0,     0,    31,
      32,     0,    33,    34,    35,    36,    37,    38,   232,    39,
      40,    41,     0,     0,    42,     0,     0,    43,    44,     0,
      45,    46,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     197,     0,     0,   198,    49,     0,    50,    51,     0,   233,
       0,    53,    54,    55,    56,    57,    58,    59,     0,     0,
      60,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       5,     6,     7,     0,     9,     0,     0,     0,    10,    11,
      61,   235,    63,    12,     0,    13,    14,    15,   216,   217,
      18,    19,     0,     0,     0,     0,     0,   218,   219,   220,
      23,    24,    25,    26,     0,     0,   221,     0,     0,     0,
       0,     0,     0,     0,     0,    31,    32,     0,    33,    34,
      35,    36,    37,    38,   232,    39,    40,    41,     0,     0,
      42,     0,     0,    43,    44,     0,    45,    46,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   197,     0,     0,   198,
      49,     0,    50,    51,     0,     0,   234,    53,    54,    55,
      56,    57,    58,    59,     0,     0,    60,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     5,     6,     7,     0,
       9,     0,     0,     0,    10,    11,    61,   235,    63,    12,
       0,    13,    14,    15,   216,   217,    18,    19,     0,     0,
       0,     0,     0,   218,   219,   220,    23,    24,    25,    26,
       0,     0,   221,     0,     0,     0,     0,     0,     0,     0,
       0,    31,    32,     0,    33,    34,    35,    36,    37,    38,
     232,    39,    40,    41,     0,     0,    42,     0,     0,    43,
      44,     0,    45,    46,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   197,     0,     0,   198,    49,     0,    50,    51,
       0,     0,     0,    53,    54,    55,    56,    57,    58,    59,
       0,     0,    60,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     5,     6,     7,     0,     9,     0,     0,     0,
      10,    11,    61,   235,    63,    12,     0,    13,    14,    15,
      16,    17,    18,    19,     0,     0,     0,     0,     0,    20,
      21,    22,    23,    24,    25,    26,     0,     0,   221,     0,
       0,     0,     0,     0,     0,     0,     0,    31,    32,     0,
      33,    34,    35,    36,    37,    38,     0,    39,    40,    41,
       0,     0,    42,     0,     0,    43,    44,     0,    45,    46,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   197,     0,
       0,   198,    49,     0,    50,    51,     0,   514,     0,    53,
      54,    55,    56,    57,    58,    59,     0,     0,    60,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     5,     6,
       7,     0,     9,     0,     0,     0,    10,    11,    61,   235,
      63,    12,     0,    13,    14,    15,   216,   217,    18,    19,
       0,     0,     0,     0,     0,   218,   219,   220,    23,    24,
      25,    26,     0,     0,   221,     0,     0,     0,     0,     0,
       0,     0,     0,    31,    32,     0,    33,    34,    35,    36,
      37,    38,     0,    39,    40,    41,     0,     0,    42,     0,
       0,    43,    44,     0,    45,    46,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   197,     0,     0,   198,    49,     0,
      50,    51,     0,   233,     0,    53,    54,    55,    56,    57,
      58,    59,     0,     0,    60,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     5,     6,     7,     0,     9,     0,
       0,     0,    10,    11,    61,   235,    63,    12,     0,    13,
      14,    15,   216,   217,    18,    19,     0,     0,     0,     0,
       0,   218,   219,   220,    23,    24,    25,    26,     0,     0,
     221,     0,     0,     0,     0,     0,     0,     0,     0,    31,
      32,     0,    33,    34,    35,    36,    37,    38,     0,    39,
      40,    41,     0,     0,    42,     0,     0,    43,    44,     0,
      45,    46,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     197,     0,     0,   198,    49,     0,    50,    51,     0,   514,
       0,    53,    54,    55,    56,    57,    58,    59,     0,     0,
      60,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       5,     6,     7,     0,     9,     0,     0,     0,    10,    11,
      61,   235,    63,    12,     0,    13,    14,    15,    16,    17,
      18,    19,     0,     0,     0,     0,     0,    20,    21,    22,
      23,    24,    25,    26,     0,     0,    27,     0,     0,     0,
       0,     0,     0,     0,     0,    31,    32,     0,    33,    34,
      35,    36,    37,    38,     0,    39,    40,    41,     0,     0,
      42,     0,     0,    43,    44,     0,    45,    46,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   197,     0,     0,   198,
      49,     0,    50,    51,     0,     0,     0,    53,    54,    55,
      56,    57,    58,    59,     0,     0,    60,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     5,     6,     7,     0,
       9,     0,     0,     0,    10,    11,    61,    62,    63,    12,
       0,    13,    14,    15,   216,   217,    18,    19,     0,     0,
       0,     0,     0,   218,   219,   220,    23,    24,    25,    26,
       0,     0,   221,     0,     0,     0,     0,     0,     0,     0,
       0,    31,    32,     0,    33,    34,    35,    36,    37,    38,
       0,    39,    40,    41,     0,     0,    42,     0,     0,    43,
      44,     0,    45,    46,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   197,     0,     0,   198,    49,     0,    50,    51,
       0,     0,     0,    53,    54,    55,    56,    57,    58,    59,
       0,     0,    60,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     5,     6,     7,     0,     9,     0,     0,     0,
      10,    11,    61,   235,    63,    12,     0,    13,    14,    15,
      16,    17,    18,    19,     0,     0,     0,     0,     0,    20,
      21,    22,    23,    24,    25,    26,     0,     0,   221,     0,
       0,     0,     0,     0,     0,     0,     0,    31,    32,     0,
      33,    34,    35,    36,    37,    38,     0,    39,    40,    41,
       0,     0,    42,     0,     0,    43,    44,     0,    45,    46,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   197,     0,
       0,   198,    49,     0,    50,    51,     0,     0,     0,    53,
      54,    55,    56,    57,    58,    59,     0,     0,    60,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     5,     6,
       7,     0,     9,     0,     0,     0,    10,    11,    61,   235,
      63,    12,     0,    13,    14,    15,   216,   217,    18,    19,
       0,     0,     0,     0,     0,   218,   219,   220,    23,    24,
      25,    26,     0,     0,   221,     0,     0,     0,     0,     0,
       0,     0,     0,    31,    32,     0,    33,    34,    35,    36,
      37,    38,     0,    39,    40,    41,     0,     0,    42,     0,
       0,    43,    44,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   222,     0,     0,    48,    49,     0,
      50,    51,     0,    52,     0,    53,    54,    55,    56,    57,
      58,    59,     0,     0,    60,     0,     0,     0,     0,     0,
       5,     6,     7,     0,     9,     0,     0,     0,    10,    11,
       0,     0,     0,    12,   223,    13,    14,    15,   216,   217,
      18,    19,     0,     0,     0,     0,     0,   218,   219,   220,
      23,    24,    25,    26,     0,     0,   221,     0,     0,     0,
       0,     0,     0,     0,     0,    31,    32,     0,    33,    34,
      35,    36,    37,    38,     0,    39,    40,    41,     0,     0,
      42,     0,     0,    43,    44,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   280,     0,     0,   327,
      49,     0,    50,    51,     0,   328,     0,    53,    54,    55,
      56,    57,    58,    59,     0,     0,    60,     0,     0,     0,
       0,     0,     5,     6,     7,     0,     9,     0,     0,     0,
      10,    11,     0,     0,     0,    12,   223,    13,    14,    15,
     216,   217,    18,    19,     0,     0,     0,     0,     0,   218,
     219,   220,    23,    24,    25,    26,     0,     0,   221,     0,
       0,     0,     0,     0,     0,     0,     0,    31,    32,     0,
     371,    34,    35,    36,   372,    38,     0,    39,    40,    41,
       0,     0,    42,     0,     0,    43,    44,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   373,     0,
       0,   198,    49,     0,    50,    51,     0,     0,     0,    53,
      54,    55,    56,    57,    58,    59,     0,     0,    60,     0,
       0,     0,     0,     0,     5,     6,     7,     0,     9,     0,
       0,     0,    10,    11,     0,     0,     0,    12,   223,    13,
      14,    15,   216,   217,    18,    19,     0,     0,     0,     0,
       0,   218,   219,   220,    23,    24,    25,    26,     0,     0,
     221,     0,     0,     0,     0,     0,     0,     0,     0,    31,
      32,     0,    33,    34,    35,    36,    37,    38,     0,    39,
      40,    41,     0,     0,    42,     0,     0,    43,    44,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     280,     0,     0,   327,    49,     0,    50,    51,     0,     0,
       0,    53,    54,    55,    56,    57,    58,    59,     0,     0,
      60,     0,     0,     0,     0,     0,     5,     6,     7,     0,
       9,     0,     0,     0,    10,    11,     0,     0,     0,    12,
     223,    13,    14,    15,   216,   217,    18,    19,     0,     0,
       0,     0,     0,   218,   219,   220,    23,    24,    25,    26,
       0,     0,   221,     0,     0,     0,     0,     0,     0,     0,
       0,    31,    32,     0,    33,    34,    35,    36,    37,    38,
       0,    39,    40,    41,     0,     0,    42,     0,     0,    43,
      44,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   864,     0,     0,   198,    49,     0,    50,    51,
       0,     0,     0,    53,    54,    55,    56,    57,    58,    59,
       0,     0,    60,     0,     0,     0,     0,     0,     5,     6,
       7,     0,     9,     0,     0,     0,    10,    11,     0,     0,
       0,    12,   223,    13,    14,    15,   216,   217,    18,    19,
       0,     0,     0,     0,     0,   218,   219,   220,    23,    24,
      25,    26,     0,     0,   221,     0,     0,     0,     0,     0,
       0,     0,     0,    31,    32,     0,    33,    34,    35,    36,
      37,    38,     0,    39,    40,    41,     0,     0,    42,     0,
       0,    43,    44,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   882,     0,     0,   198,    49,     0,
      50,    51,     0,     0,     0,    53,    54,    55,    56,    57,
      58,    59,     0,     0,    60,     0,     0,   563,   564,     0,
       0,   565,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   223,   147,   148,   149,   150,   151,
     152,   153,   154,   155,     0,     0,   156,   157,     0,     0,
     158,   159,   160,   161,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   162,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   163,   164,   165,   166,   167,
     168,   169,   170,   171,   172,     0,   173,   174,   605,   556,
       0,     0,   606,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   175,   249,   147,   148,   149,   150,
     151,   152,   153,   154,   155,     0,     0,   156,   157,     0,
       0,   158,   159,   160,   161,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   162,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   163,   164,   165,   166,
     167,   168,   169,   170,   171,   172,     0,   173,   174,   608,
     564,     0,     0,   609,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   175,   249,   147,   148,   149,
     150,   151,   152,   153,   154,   155,     0,     0,   156,   157,
       0,     0,   158,   159,   160,   161,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   162,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   163,   164,   165,
     166,   167,   168,   169,   170,   171,   172,     0,   173,   174,
     620,   556,     0,     0,   621,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   175,   249,   147,   148,
     149,   150,   151,   152,   153,   154,   155,     0,     0,   156,
     157,     0,     0,   158,   159,   160,   161,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   162,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   163,   164,
     165,   166,   167,   168,   169,   170,   171,   172,     0,   173,
     174,   624,   564,     0,     0,   625,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   175,   249,   147,
     148,   149,   150,   151,   152,   153,   154,   155,     0,     0,
     156,   157,     0,     0,   158,   159,   160,   161,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   162,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   163,
     164,   165,   166,   167,   168,   169,   170,   171,   172,     0,
     173,   174,   651,   556,     0,     0,   652,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   175,   249,
     147,   148,   149,   150,   151,   152,   153,   154,   155,     0,
       0,   156,   157,     0,     0,   158,   159,   160,   161,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   162,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     163,   164,   165,   166,   167,   168,   169,   170,   171,   172,
       0,   173,   174,   654,   564,     0,     0,   655,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   175,
     249,   147,   148,   149,   150,   151,   152,   153,   154,   155,
       0,     0,   156,   157,     0,     0,   158,   159,   160,   161,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     162,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   163,   164,   165,   166,   167,   168,   169,   170,   171,
     172,     0,   173,   174,   720,   556,     0,     0,   721,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     175,   249,   147,   148,   149,   150,   151,   152,   153,   154,
     155,     0,     0,   156,   157,     0,     0,   158,   159,   160,
     161,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   162,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   163,   164,   165,   166,   167,   168,   169,   170,
     171,   172,     0,   173,   174,   569,   564,     0,     0,   570,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   175,   249,   147,   148,   149,   150,   151,   152,   153,
     154,   155,     0,     0,   156,   157,     0,     0,   158,   159,
     160,   161,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   162,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   163,   164,   165,   166,   167,   168,   169,
     170,   171,   172,     0,   173,   174,   940,   556,     0,     0,
     941,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   175,   249,   147,   148,   149,   150,   151,   152,
     153,   154,   155,     0,     0,   156,   157,     0,     0,   158,
     159,   160,   161,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   162,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   163,   164,   165,   166,   167,   168,
     169,   170,   171,   172,     0,   173,   174,   948,   556,     0,
       0,   949,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   175,   249,   147,   148,   149,   150,   151,
     152,   153,   154,   155,     0,     0,   156,   157,     0,     0,
     158,   159,   160,   161,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   162,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   163,   164,   165,   166,   167,
     168,   169,   170,   171,   172,     0,   173,   174,   951,   564,
       0,     0,   952,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   175,   249,   147,   148,   149,   150,
     151,   152,   153,   154,   155,     0,     0,   156,   157,     0,
       0,   158,   159,   160,   161,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   162,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   163,   164,   165,   166,
     167,   168,   169,   170,   171,   172,     0,   173,   174,   569,
     564,     0,     0,   570,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   175,   249,   147,   148,   149,
     150,   151,   152,   153,   154,   155,     0,     0,   156,   157,
       0,     0,   158,   159,   160,   161,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   162,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   163,   164,   165,
     166,   167,   168,   169,   170,   171,   172,     0,   173,   174,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   175
};

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-768)))

#define yytable_value_is_error(Yytable_value) \
  (!!((Yytable_value) == (-558)))

static const yytype_int16 yycheck[] =
{
       2,   286,    48,    27,    16,    17,    28,    12,    20,    15,
      83,    11,    12,    76,     7,   419,     8,   242,    22,   399,
     416,   206,    62,   419,    12,   404,   330,     4,   330,   274,
     687,    16,    17,   278,   468,    20,    28,   358,    52,   373,
      16,    17,    22,   286,    20,   672,    52,   451,    50,    51,
      48,   459,   607,    10,    11,    12,    65,   765,   668,   600,
      65,    54,    65,   493,   767,    50,    51,   281,   623,   610,
      76,   357,   393,   359,    50,    25,   362,    65,   357,   591,
     359,   298,   594,   362,   572,   573,    25,     2,   409,     4,
      25,    25,    13,   687,    98,   101,   381,   103,   653,   866,
      16,    17,    26,     1,    20,   847,   392,    25,   394,   263,
      13,    25,   520,   392,    88,   394,    25,   668,    98,    90,
      79,   672,   408,   702,   410,    37,    38,    29,    88,   116,
     709,   410,    58,    59,    88,   843,   687,    90,    90,    86,
     461,    90,    25,    13,    90,   116,    37,    38,     0,    13,
     436,   437,   198,    13,    15,   149,    17,    26,   437,   153,
     147,   143,   121,   116,   116,    13,    55,   116,   723,   936,
     116,   145,   146,   327,   460,    51,   462,    53,    54,    55,
      56,   398,   612,   462,    28,   145,   146,   929,   473,   783,
     898,   145,   146,   896,   147,   200,   116,   147,   191,    27,
     209,   206,   632,   428,   209,   862,   108,   719,   147,   866,
     149,   257,   147,   147,   214,   215,   220,   263,   281,   143,
     928,   209,   120,   850,   658,   659,   147,   231,   716,   147,
     473,   152,   153,   147,   246,   639,   248,   249,   147,   241,
     242,   153,   638,   639,   854,   459,   149,   249,   147,   152,
     153,    25,   862,   587,   152,   153,   265,   214,   215,   116,
     265,   246,   265,   248,   147,   263,   149,   588,   273,   274,
     246,   147,   248,   278,   143,   281,    88,   265,    88,   936,
     149,   327,   152,   153,    51,   325,    88,   695,   152,   153,
     330,    90,   152,   153,   835,   252,   253,   599,    51,   850,
      88,   587,    55,   589,   152,   153,   520,   147,   593,    88,
     589,   862,    55,   270,   328,   866,   316,   317,   318,   319,
      88,    86,   328,   116,   152,   153,   366,   367,    88,   327,
     246,   765,   248,   767,   146,   145,   146,   110,   315,   149,
     113,   114,   116,   389,   146,    90,   358,   262,    25,   325,
     593,   397,    26,    90,   330,   359,   358,   145,   146,   316,
     317,   318,   319,    90,   321,   322,   145,   146,   880,   881,
     375,   116,    17,   147,   152,   754,   150,   145,   146,   153,
      88,   393,    88,    26,   386,   936,   146,   375,    26,   116,
     394,   393,   696,    90,   457,   950,   459,   409,   403,   404,
     315,   150,   687,   648,   153,   320,   410,   409,   633,   843,
     844,   433,   147,   149,    88,   403,   404,   149,   153,   116,
     147,   378,    26,   380,   400,   427,   428,   149,    88,   614,
      88,   416,   434,   437,   152,   153,   840,   145,   146,   116,
     146,   433,    90,   489,   840,    88,    88,    58,    59,   461,
      88,   457,    88,   459,    88,   835,    25,   520,   462,   461,
      88,    55,   896,   448,   898,   232,   233,   234,   116,   143,
     147,   145,   146,   150,    88,   149,   153,   479,    88,    91,
     482,   695,   484,     2,    88,     4,   146,   145,   146,    88,
      88,    10,    11,    12,   928,    71,    15,    16,    17,   147,
     143,    20,   145,   146,   146,   143,   149,   145,   146,   145,
     146,   513,   146,   143,   520,   147,   147,   145,   146,   434,
     544,    51,   153,    53,    54,    55,    56,   147,   443,    48,
      71,   145,   146,    52,   759,   145,   146,   822,   562,   143,
     147,   145,   146,    62,   150,   149,   153,   146,   146,   116,
     126,   127,   128,   252,    14,    15,   560,    76,   325,    56,
      57,    58,    59,   330,   568,   153,   568,   571,   267,   571,
     572,   573,   271,   151,   144,   597,   588,    55,   583,   147,
     560,    71,   101,   607,   103,   589,   588,    71,   568,   110,
     595,   571,   113,   114,   147,   583,    97,   599,   600,   623,
     602,   922,    63,   596,    90,   597,    10,   595,   610,   614,
      51,     8,    53,    54,    55,    56,    10,   147,   622,   665,
     666,    90,   626,   599,   626,    13,   912,   627,   150,   653,
     116,   633,   695,   400,   680,   921,    90,   923,   711,   824,
     664,   147,   688,   648,   923,   830,   144,   116,   415,   416,
     116,    92,   419,   638,    71,   147,   696,    98,    99,   699,
     700,   147,   116,   687,   147,   705,   706,   713,   714,    51,
     627,   147,   147,   672,   147,    51,   675,   196,   147,   198,
      63,   448,   123,   147,   451,   126,   116,    15,   687,   695,
     736,   147,   147,   147,    10,   214,   215,   144,    51,   723,
      53,    54,    55,    56,   750,   751,   752,   124,   125,   126,
     127,   128,   153,   480,   716,   147,   144,   118,   718,   724,
      61,   147,   421,    64,    65,    10,   425,   246,   151,   248,
     249,   430,   734,   252,   253,   737,   724,   291,   257,   739,
      10,   440,    91,   262,   263,   299,    10,   514,   302,   754,
     304,   270,   306,     9,   308,    10,   147,   759,   150,   783,
     147,   718,   281,   803,    10,   144,   754,   152,   814,   147,
     111,   112,   774,   775,   147,   777,   147,   779,   780,   147,
     786,   827,   739,   785,   144,   121,   788,   789,    51,   147,
      53,    54,    55,    56,    10,   147,   315,   316,   317,   318,
     319,   320,   321,   322,   147,   147,   325,    10,   327,   328,
     815,   330,    40,    41,    42,    43,    44,   823,   144,   824,
     519,    10,    10,    10,   147,   830,   121,   815,    55,    92,
     147,   144,   599,   835,    55,   147,    99,   147,   147,   358,
     147,   887,    62,   889,    64,    65,   147,   366,   367,   895,
       2,   850,     4,   852,    10,    10,    10,   856,   149,   378,
     123,   380,   629,    15,    15,   632,   149,   866,   147,   868,
     389,   638,   639,   434,   393,   571,   925,   901,   397,   103,
      51,   658,    53,    54,    55,    56,    62,   929,    64,    65,
     409,   111,   112,   592,   712,    61,    48,   924,    64,    65,
     495,     7,   904,   905,   906,   907,   862,   909,   910,   822,
     922,   913,   668,   915,   916,   434,    63,    64,    65,   923,
     922,    92,   924,   925,   443,   692,   950,    98,    99,   182,
      -1,    -1,    -1,   932,    -1,   111,   112,   936,   457,   938,
     459,    -1,   461,    -1,   943,   111,   112,    -1,   647,    63,
      64,    65,   123,    -1,    -1,   126,   958,   959,   960,   961,
     962,    -1,    -1,   662,   111,   112,    -1,   966,   970,    -1,
     489,    -1,    -1,    63,    64,    65,    -1,    -1,   149,    -1,
      -1,    -1,   749,    51,    -1,    53,    54,    55,    56,   756,
      -1,    51,    -1,    53,    54,    55,    56,   111,   112,    -1,
      -1,   520,     2,    -1,     4,    63,    64,    65,    -1,    -1,
      10,    11,    12,    -1,    -1,    -1,    16,    17,    -1,    -1,
      20,   111,   112,    -1,    92,   792,    -1,    -1,   727,    -1,
      98,    99,    92,    -1,    -1,    -1,    -1,    -1,    98,    99,
      -1,    -1,    -1,    -1,   196,    71,   198,    -1,    48,    44,
      -1,    -1,    -1,   111,   112,   123,    -1,    -1,   126,    -1,
      86,    87,    62,   123,    -1,   764,   126,    -1,    -1,   588,
      -1,    -1,   591,   840,    -1,   594,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,   149,
      -1,    86,    87,   482,    -1,   484,    -1,   123,   124,   125,
     126,   127,   128,    71,    -1,   257,    -1,    -1,   627,    -1,
     262,   263,    -1,    -1,    -1,    -1,    -1,    -1,    86,    87,
      -1,    -1,   117,    -1,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,    -1,    71,    63,    64,    65,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   665,   666,    -1,    -1,
      86,    87,   147,   121,   122,   123,   124,   125,   126,   127,
     128,   680,    -1,   315,    63,    64,    65,    -1,   320,   688,
      63,    64,    65,    -1,    -1,   327,   695,   696,   330,    -1,
     699,   700,    -1,    -1,   111,   112,   705,   706,   124,   125,
     126,   127,   128,    -1,   713,   714,   196,    -1,   198,   718,
     719,    -1,    -1,    51,    -1,    53,    54,    55,    56,    -1,
      -1,    -1,   111,   112,   214,   215,    -1,   736,   111,   112,
     739,    -1,    -1,    -1,    51,    -1,    53,    54,    55,    56,
      -1,   750,   751,   752,    -1,    -1,    51,   389,    53,    54,
      55,    56,    -1,    -1,    92,   397,   246,    -1,   248,   249,
      98,    -1,   252,   253,    -1,    -1,    -1,   257,    -1,    -1,
      -1,    -1,   262,   263,    -1,    92,    -1,   786,    -1,    -1,
     270,    98,    -1,    -1,    -1,    -1,    -1,    92,    -1,    -1,
      -1,    -1,   434,    -1,   803,    -1,    -1,    -1,    -1,    -1,
      -1,   443,    -1,    -1,    -1,   814,    71,    72,    73,    74,
      75,    76,    77,    78,   823,    80,    81,    -1,   827,    -1,
      -1,    86,    87,    -1,    -1,   315,   316,   317,   318,   319,
     320,   321,   322,    -1,    -1,   325,    -1,   327,    -1,    -1,
     330,    -1,    -1,    -1,    -1,    -1,    -1,   489,    -1,    -1,
      -1,    -1,    -1,    -1,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,    -1,    -1,    -1,    -1,   358,    -1,
      -1,   880,   881,    -1,    -1,    -1,   366,   367,   887,    -1,
     889,    -1,    -1,    -1,    -1,    -1,   895,    -1,   378,    -1,
     380,    -1,    -1,    -1,    -1,   774,   775,    -1,   777,   389,
     779,   780,    -1,   393,    -1,    -1,   785,   397,    -1,   788,
     789,    -1,    -1,   922,    -1,    -1,    -1,    -1,    -1,   409,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   434,    -1,    -1,    -1,    -1,   591,
      16,    17,   594,   443,    20,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   461,    -1,    -1,    -1,    -1,    -1,    -1,     0,    45,
      46,    -1,    -1,    -1,    50,    51,     8,     9,    10,    -1,
      -1,    13,    14,    15,    -1,    17,    62,    63,    -1,   489,
      -1,    -1,    -1,    -1,    26,    27,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    37,    38,    -1,    40,    41,
      42,    43,    44,   665,   666,   904,   905,   906,   907,    -1,
     909,   910,    -1,    -1,   913,    -1,   915,   916,   680,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   688,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   696,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    88,    -1,    -1,    -1,
      -1,   713,   714,    -1,    -1,    -1,    -1,   719,    -1,   958,
     959,   960,   961,   962,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   970,    -1,    -1,   736,    -1,    -1,    -1,   588,    -1,
       2,   591,     4,    -1,   594,     7,    -1,    -1,   750,   751,
     752,    -1,    -1,    15,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   143,   144,    -1,   146,    -1,    -1,   149,   150,    -1,
     152,   153,    -1,    -1,    -1,    -1,    -1,   627,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    48,    -1,    -1,    -1,
      52,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   232,   233,   234,   235,
      -1,    -1,   814,    -1,    76,   665,   666,    -1,    -1,    -1,
     246,   823,   248,   249,    -1,   827,    -1,    -1,    -1,    -1,
     680,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   688,   101,
      -1,   103,    -1,    -1,    -1,    -1,   696,    -1,    -1,   699,
     700,    -1,    -1,    -1,    -1,   705,   706,    -1,    -1,    -1,
      -1,    -1,    -1,   713,   714,    -1,    -1,    -1,   718,   719,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   880,   881,
      -1,    -1,    -1,    -1,    -1,   887,   736,   889,    -1,   739,
      -1,    -1,    -1,   895,    -1,    -1,    -1,    -1,    -1,   325,
     750,   751,   752,    -1,   330,   331,   332,   333,   334,   335,
     336,   337,   338,   339,   340,   341,   342,   343,   344,   345,
     346,   347,   348,   349,   350,   351,   352,   353,   354,   355,
     356,    -1,   358,    -1,   196,    -1,   198,    -1,    -1,    -1,
     366,   367,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   803,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   814,   391,    -1,   393,    -1,   395,
     396,    -1,    -1,    -1,   400,    -1,    -1,   827,    -1,    -1,
      -1,    -1,    -1,   409,    -1,    -1,    -1,    -1,    -1,   415,
     416,    -1,    -1,   419,    -1,   257,    -1,    -1,    -1,    -1,
     262,   263,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   438,    -1,    -1,    -1,    -1,    -1,    -1,   281,
      -1,     2,   448,     4,    -1,   451,    -1,    -1,    -1,    -1,
     880,   881,    -1,    -1,    15,   461,    -1,   887,    -1,   889,
      -1,    -1,    -1,    -1,    -1,   895,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   315,   480,    -1,    -1,    -1,   320,    -1,
      -1,    -1,    -1,    -1,    -1,   327,   328,    48,    -1,    44,
      -1,    52,   922,    -1,    -1,    -1,    -1,   503,   504,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   514,    -1,
      -1,    -1,    -1,    -1,    -1,    76,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    -1,
      -1,    86,    87,    -1,     2,    -1,     4,    -1,    -1,    -1,
     101,    -1,   103,    -1,    -1,    -1,    -1,   389,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   397,    -1,    -1,    -1,    -1,
      -1,    -1,   117,    -1,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,    -1,    -1,    -1,    -1,    -1,    -1,
      48,    -1,   588,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   434,   599,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   443,    -1,    -1,    71,    72,    73,    74,    75,    76,
      77,    -1,    -1,    80,    81,   457,    -1,   459,    -1,    86,
      87,    -1,    -1,   629,    -1,    -1,   632,    -1,    -1,    -1,
      -1,    -1,   638,   639,    -1,   196,    -1,   198,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   489,    -1,    -1,
      -1,    -1,   119,   120,   121,   122,   123,   124,   125,   126,
     127,   128,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   520,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   692,    -1,    -1,    -1,
     696,   697,    -1,   699,   700,    -1,   257,    -1,    -1,   705,
     706,   262,   263,    -1,    -1,    -1,    -1,    -1,    -1,   715,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     281,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   196,    -1,
     198,    -1,    -1,    -1,    -1,    -1,   742,   743,    -1,   745,
     746,    -1,    -1,   749,    -1,    -1,    -1,    -1,    -1,   591,
     756,    -1,   594,    -1,   315,    -1,    -1,    -1,    -1,   320,
      -1,    -1,    -1,    -1,    -1,    -1,   327,   328,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   792,    -1,    -1,   257,
     796,    -1,    -1,    -1,   262,   263,    -1,   803,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   665,   666,    -1,   832,    -1,   389,    -1,
      -1,    -1,    -1,    -1,   840,    -1,   397,    -1,   680,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   688,   315,    -1,    -1,
      -1,    -1,   320,   695,    -1,    -1,    -1,    -1,    -1,   327,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   713,   714,   434,    -1,    -1,    -1,   719,    -1,    -1,
      -1,    -1,   443,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   736,    -1,   457,    -1,   459,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   750,   751,
     752,    -1,    -1,    -1,    -1,    -1,   922,    -1,    -1,    44,
      -1,   389,    -1,    -1,    -1,    -1,    -1,    -1,   489,   397,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   786,    -1,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,   520,
      -1,    86,    87,    -1,    -1,    -1,   434,    -1,    -1,    -1,
      -1,    -1,   814,    -1,    -1,   443,    -1,    -1,    -1,    -1,
      -1,   823,    -1,    -1,    -1,   827,    -1,    -1,    -1,    51,
      52,    -1,   117,    55,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,    -1,    -1,    -1,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    -1,    -1,    80,    81,
      -1,   489,    84,    85,    86,    87,    -1,    -1,    -1,    -1,
     591,    -1,    -1,   594,    -1,    -1,    98,    -1,   880,   881,
      -1,    -1,    -1,    -1,    -1,   887,    -1,   889,    -1,    -1,
      -1,    -1,    -1,   895,    -1,    -1,    -1,   119,   120,   121,
     122,   123,   124,   125,   126,   127,   128,    -1,   130,   131,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   148,   149,    -1,    -1,
      -1,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,   665,   666,    86,    87,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   680,
      -1,    -1,    -1,   591,    -1,    -1,   594,   688,    -1,    -1,
      -1,    -1,    -1,    -1,   695,    -1,    -1,   117,    -1,   119,
     120,   121,   122,   123,   124,   125,   126,   127,   128,    -1,
      -1,    -1,   713,   714,    -1,    -1,    -1,    -1,   719,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   153,    -1,   736,    -1,    -1,    71,    72,
      73,    74,    75,    76,    77,    -1,    -1,    80,    81,   750,
     751,   752,    -1,    86,    87,    -1,    -1,   665,   666,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   680,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     688,    -1,    -1,    -1,    -1,   786,   119,   120,   121,   122,
     123,   124,   125,   126,   127,   128,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   713,   714,    -1,    -1,    -1,
      -1,   719,    -1,   814,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   823,    -1,    -1,    -1,   827,    -1,   736,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   750,   751,   752,    -1,    -1,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      -1,    -1,    86,    87,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   880,
     881,    -1,    -1,    -1,    -1,    -1,   887,    -1,   889,    -1,
      -1,    -1,    -1,   117,   895,   119,   120,   121,   122,   123,
     124,   125,   126,   127,   128,    -1,   814,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   827,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   880,   881,    -1,    -1,    -1,    -1,    -1,   887,
      -1,   889,    -1,    -1,    -1,     0,     1,   895,     3,     4,
       5,     6,     7,    -1,    -1,    -1,    11,    12,    -1,    -1,
      -1,    16,    -1,    18,    19,    20,    21,    22,    23,    24,
      -1,    -1,    -1,    -1,    -1,    30,    31,    32,    33,    34,
      35,    36,    -1,    -1,    39,    -1,    -1,    -1,    -1,    -1,
      45,    46,    47,    48,    49,    -1,    51,    52,    53,    54,
      55,    56,    -1,    58,    59,    60,    -1,    -1,    63,    -1,
      -1,    66,    67,    -1,    69,    70,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    89,    -1,    -1,    92,    93,    -1,
      95,    96,    -1,    98,    -1,   100,   101,   102,   103,   104,
     105,   106,     0,    -1,   109,    -1,    -1,    -1,    -1,    -1,
       8,     9,    10,    -1,    -1,    13,    14,    15,    -1,    17,
      -1,    -1,    -1,    -1,   129,   130,   131,    -1,    -1,    27,
      28,    29,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    37,
      38,    -1,    40,    41,    42,    43,    44,   152,   153,    -1,
      -1,    -1,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    -1,    -1,    86,    87,    -1,
      -1,    -1,    -1,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    -1,    -1,    86,    87,
      88,    -1,    90,    91,    -1,    -1,    -1,    -1,    -1,    97,
     119,   120,   121,   122,   123,   124,   125,   126,   127,   128,
     108,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   116,   117,
     118,   119,   120,   121,   122,   123,   124,   125,   126,   127,
     128,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   144,   145,   146,   147,
       0,    -1,   150,   151,   152,   153,    -1,    -1,     8,     9,
      10,    -1,    -1,    13,    14,    15,    -1,    17,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    26,    27,    28,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    37,    38,    -1,
      40,    41,    42,    43,    44,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    -1,    -1,    86,    87,    88,    -1,
      -1,    91,    -1,    -1,    -1,    -1,    -1,    97,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   117,    -1,   119,
     120,   121,   122,   123,   124,   125,   126,   127,   128,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   143,   144,   145,   146,   147,     0,   149,
     150,   151,   152,   153,    -1,    -1,     8,     9,    10,    -1,
      -1,    13,    14,    15,    -1,    17,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    27,    28,    29,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    37,    38,    -1,    40,    41,
      42,    43,    44,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    -1,    -1,    86,    87,    88,    -1,    -1,    91,
      -1,    -1,    -1,    -1,    -1,    97,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   108,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   117,   118,   119,   120,   121,
     122,   123,   124,   125,   126,   127,   128,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   144,   145,   146,   147,     0,    -1,   150,   151,
     152,   153,    -1,    -1,     8,     9,    10,    -1,    -1,    13,
      14,    15,    -1,    17,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    26,    27,    28,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    37,    38,    -1,    40,    41,    42,    43,
      44,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      -1,    -1,    86,    87,    88,    -1,    -1,    91,    -1,    -1,
      -1,    -1,    -1,    97,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   117,    -1,   119,   120,   121,   122,   123,
     124,   125,   126,   127,   128,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   143,
     144,   145,   146,   147,     0,   149,   150,   151,   152,   153,
      -1,    -1,     8,     9,    10,    -1,    -1,    13,    14,    15,
      -1,    17,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    27,    28,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    37,    38,    -1,    40,    41,    42,    43,    44,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    -1,    -1,
      86,    87,    88,    -1,    -1,    91,     0,    -1,    -1,    -1,
      -1,    97,    -1,    -1,     8,     9,    10,    -1,    -1,    -1,
      14,    15,    -1,    17,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   117,    26,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   128,    37,    38,    -1,    40,    41,    42,    43,
      44,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   144,   145,
     146,   147,    -1,   149,   150,   151,   152,   153,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      -1,    -1,    86,    87,    88,    -1,    90,    -1,     0,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     8,     9,    10,    -1,
      -1,    -1,    14,    15,    -1,    17,    -1,    -1,    -1,    -1,
      -1,    -1,   116,   117,    -1,   119,   120,   121,   122,   123,
     124,   125,   126,   127,   128,    37,    38,    -1,    40,    41,
      42,    43,    44,    -1,    -1,    -1,    -1,    -1,    -1,   143,
     144,   145,   146,   147,    -1,    -1,   150,    -1,   152,   153,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    -1,    -1,    86,    87,    88,    -1,    90,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   116,   117,    -1,   119,   120,   121,
     122,   123,   124,   125,   126,   127,   128,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   144,   145,   146,   147,    -1,    -1,   150,    -1,
     152,   153,     1,    -1,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    -1,    -1,    15,    16,    -1,    18,
      19,    20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,
      -1,    30,    31,    32,    33,    34,    35,    36,    -1,    -1,
      39,    -1,    -1,    -1,    -1,    -1,    45,    -1,    47,    48,
      49,    -1,    51,    52,    53,    54,    55,    56,    -1,    58,
      59,    60,    -1,    -1,    63,    -1,    -1,    66,    67,    -1,
      69,    70,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      89,    -1,    -1,    92,    93,    -1,    95,    96,    -1,    98,
      -1,   100,   101,   102,   103,   104,   105,   106,    -1,    -1,
     109,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     129,   130,   131,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     1,    -1,     3,     4,
       5,     6,     7,   152,   153,    10,    11,    12,    -1,    14,
      15,    16,    -1,    18,    19,    20,    21,    22,    23,    24,
      -1,    -1,    -1,    -1,    -1,    30,    31,    32,    33,    34,
      35,    36,    -1,    -1,    39,    -1,    -1,    -1,    -1,    -1,
      45,    -1,    47,    48,    49,    -1,    51,    52,    53,    54,
      55,    56,    -1,    58,    59,    60,    -1,    -1,    63,    -1,
      -1,    66,    67,    -1,    69,    70,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    89,    -1,    -1,    92,    93,    -1,
      95,    96,    -1,    98,    -1,   100,   101,   102,   103,   104,
     105,   106,    -1,    -1,   109,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   129,   130,   131,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,     1,    -1,     3,     4,     5,
       6,     7,    -1,    -1,    10,    11,    12,   152,   153,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    -1,
      -1,    -1,    -1,    -1,    30,    31,    32,    33,    34,    35,
      36,    -1,    -1,    39,    -1,    -1,    -1,    -1,    -1,    45,
      -1,    47,    48,    49,    -1,    51,    52,    53,    54,    55,
      56,    -1,    58,    59,    60,    -1,    -1,    63,    -1,    -1,
      66,    67,    -1,    69,    70,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    89,    -1,    -1,    92,    93,    -1,    95,
      96,    -1,    98,    -1,   100,   101,   102,   103,   104,   105,
     106,    -1,    -1,   109,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   129,   130,   131,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,     1,    -1,     3,     4,     5,     6,
       7,    -1,    -1,    10,    11,    12,   152,   153,    15,    16,
      -1,    18,    19,    20,    21,    22,    23,    24,    -1,    -1,
      -1,    -1,    -1,    30,    31,    32,    33,    34,    35,    36,
      -1,    -1,    39,    -1,    -1,    -1,    -1,    -1,    45,    -1,
      47,    48,    49,    -1,    51,    52,    53,    54,    55,    56,
      -1,    58,    59,    60,    -1,    -1,    63,    -1,    -1,    66,
      67,    -1,    69,    70,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    89,    -1,    -1,    92,    93,    -1,    95,    96,
      -1,    98,    -1,   100,   101,   102,   103,   104,   105,   106,
      -1,    -1,   109,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   129,   130,   131,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     1,    -1,     3,     4,     5,     6,     7,    -1,
      -1,    -1,    11,    12,    -1,   152,   153,    16,    -1,    18,
      19,    20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,
      -1,    30,    31,    32,    33,    34,    35,    36,    -1,    -1,
      39,    -1,    -1,    -1,    -1,    -1,    45,    46,    47,    48,
      49,    -1,    51,    52,    53,    54,    55,    56,    -1,    58,
      59,    60,    -1,    -1,    63,    -1,    -1,    66,    67,    -1,
      69,    70,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      89,    -1,    -1,    92,    93,    -1,    95,    96,    -1,    98,
      -1,   100,   101,   102,   103,   104,   105,   106,    -1,    -1,
     109,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     129,   130,   131,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   144,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   152,   153,     1,    -1,     3,     4,     5,
       6,     7,    -1,     9,    10,    11,    12,    -1,    -1,    -1,
      16,    -1,    18,    19,    20,    21,    22,    23,    24,    -1,
      -1,    -1,    -1,    -1,    30,    31,    32,    33,    34,    35,
      36,    -1,    -1,    39,    -1,    -1,    -1,    -1,    -1,    45,
      -1,    47,    48,    49,    -1,    51,    52,    53,    54,    55,
      56,    -1,    58,    59,    60,    -1,    -1,    63,    -1,    -1,
      66,    67,    -1,    69,    70,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    89,    -1,    -1,    92,    93,    -1,    95,
      96,    -1,    98,    -1,   100,   101,   102,   103,   104,   105,
     106,    -1,    -1,   109,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   129,   130,   131,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     1,    -1,     3,     4,     5,     6,     7,
      -1,    -1,    -1,    11,    12,    -1,   152,   153,    16,    -1,
      18,    19,    20,    21,    22,    23,    24,    -1,    -1,    -1,
      -1,    -1,    30,    31,    32,    33,    34,    35,    36,    -1,
      -1,    39,    -1,    -1,    -1,    -1,    -1,    45,    -1,    47,
      48,    49,    -1,    51,    52,    53,    54,    55,    56,    -1,
      58,    59,    60,    -1,    -1,    63,    -1,    -1,    66,    67,
      -1,    69,    70,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    89,    -1,    -1,    92,    93,    -1,    95,    96,    -1,
      98,    -1,   100,   101,   102,   103,   104,   105,   106,    -1,
      -1,   109,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   129,   130,   131,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   150,    -1,   152,   153,     1,    -1,     3,     4,
       5,     6,     7,    -1,    -1,    -1,    11,    12,    -1,    -1,
      -1,    16,    -1,    18,    19,    20,    21,    22,    23,    24,
      -1,    -1,    -1,    -1,    -1,    30,    31,    32,    33,    34,
      35,    36,    -1,    -1,    39,    -1,    -1,    -1,    -1,    -1,
      45,    -1,    47,    48,    49,    -1,    51,    52,    53,    54,
      55,    56,    -1,    58,    59,    60,    -1,    -1,    63,    -1,
      -1,    66,    67,    -1,    69,    70,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    89,    -1,    -1,    92,    93,    -1,
      95,    96,    -1,    98,    -1,   100,   101,   102,   103,   104,
     105,   106,    -1,    -1,   109,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   129,   130,   131,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   150,    -1,   152,   153,     1,
      -1,     3,     4,     5,     6,     7,    -1,    -1,    -1,    11,
      12,    -1,    -1,    -1,    16,    -1,    18,    19,    20,    21,
      22,    23,    24,    -1,    -1,    -1,    -1,    -1,    30,    31,
      32,    33,    34,    35,    36,    -1,    -1,    39,    -1,    -1,
      -1,    -1,    -1,    45,    -1,    47,    48,    49,    -1,    51,
      52,    53,    54,    55,    56,    -1,    58,    59,    60,    -1,
      -1,    63,    -1,    -1,    66,    67,    -1,    69,    70,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    89,    -1,    -1,
      92,    93,    -1,    95,    96,    -1,    98,    -1,   100,   101,
     102,   103,   104,   105,   106,    -1,    -1,   109,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   129,   130,   131,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   144,     1,    -1,     3,     4,     5,     6,     7,
     152,   153,    10,    11,    12,    -1,    -1,    -1,    16,    -1,
      18,    19,    20,    21,    22,    23,    24,    -1,    -1,    -1,
      -1,    -1,    30,    31,    32,    33,    34,    35,    36,    -1,
      -1,    39,    -1,    -1,    -1,    -1,    -1,    45,    -1,    47,
      48,    49,    -1,    51,    52,    53,    54,    55,    56,    -1,
      58,    59,    60,    -1,    -1,    63,    -1,    -1,    66,    67,
      -1,    69,    70,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    89,    -1,    -1,    92,    93,    -1,    95,    96,    -1,
      98,    -1,   100,   101,   102,   103,   104,   105,   106,    -1,
      -1,   109,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   129,   130,   131,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,
      -1,    11,    12,    -1,   152,   153,    16,    -1,    18,    19,
      20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,    -1,
      30,    31,    32,    33,    34,    35,    36,    -1,    -1,    39,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    48,    49,
      -1,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    -1,    -1,    63,    -1,    -1,    66,    67,    -1,    69,
      70,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    89,
      -1,    -1,    92,    93,    -1,    95,    96,    -1,    98,    99,
     100,   101,   102,   103,   104,   105,   106,    -1,    -1,   109,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,
       4,     5,    -1,     7,    -1,    -1,    -1,    11,    12,   129,
     130,   131,    16,    -1,    18,    19,    20,    21,    22,    23,
      24,    -1,    -1,    -1,    -1,    -1,    30,    31,    32,    33,
      34,    35,    36,   153,    -1,    39,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    48,    49,    -1,    51,    52,    53,
      54,    55,    56,    -1,    58,    59,    60,    -1,    -1,    63,
      -1,    -1,    66,    67,    -1,    69,    70,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    89,    -1,    -1,    92,    93,
      -1,    95,    96,    -1,    -1,    -1,   100,   101,   102,   103,
     104,   105,   106,    -1,    -1,   109,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   129,   130,   131,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,
      -1,     7,    -1,    -1,    -1,    11,    12,    -1,   152,   153,
      16,    -1,    18,    19,    20,    21,    22,    23,    24,    -1,
      -1,    -1,    -1,    -1,    30,    31,    32,    33,    34,    35,
      36,    -1,    -1,    39,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    48,    49,    -1,    51,    52,    53,    54,    55,
      56,    -1,    58,    59,    60,    -1,    -1,    63,    -1,    -1,
      66,    67,    -1,    69,    70,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    89,    -1,    -1,    92,    93,    -1,    95,
      96,    -1,    -1,    -1,   100,   101,   102,   103,   104,   105,
     106,    -1,    -1,   109,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     3,     4,     5,     6,     7,    -1,    -1,
      -1,    11,    12,   129,   130,   131,    16,    -1,    18,    19,
      20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,    -1,
      30,    31,    32,    33,    34,    35,    36,   153,    -1,    39,
      -1,    -1,    -1,    -1,    -1,    45,    46,    47,    48,    49,
      -1,    51,    52,    53,    54,    55,    56,    -1,    58,    59,
      60,    -1,    -1,    63,    -1,    -1,    66,    67,    -1,    69,
      70,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    89,
      -1,    -1,    92,    93,    -1,    95,    96,    -1,    98,    -1,
     100,   101,   102,   103,   104,   105,   106,    -1,    -1,   109,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   129,
     130,   131,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,
       5,     6,     7,    -1,    -1,    -1,    11,    12,    -1,    -1,
      -1,    16,   152,    18,    19,    20,    21,    22,    23,    24,
      -1,    -1,    -1,    -1,    -1,    30,    31,    32,    33,    34,
      35,    36,    -1,    -1,    39,    -1,    -1,    -1,    -1,    -1,
      45,    -1,    47,    48,    49,    -1,    51,    52,    53,    54,
      55,    56,    -1,    58,    59,    60,    -1,    -1,    63,    -1,
      -1,    66,    67,    -1,    69,    70,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    89,    -1,    -1,    92,    93,    -1,
      95,    96,    -1,    98,    -1,   100,   101,   102,   103,   104,
     105,   106,    -1,    -1,   109,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   129,   130,   131,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   152,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    -1,    -1,    -1,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    -1,    -1,    -1,    -1,    -1,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    -1,    -1,    80,    81,    -1,    -1,    84,
      85,    86,    87,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    98,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,    -1,   130,   131,    -1,    -1,    -1,
      -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,
      11,    12,    -1,   148,   149,    16,    -1,    18,    19,    20,
      21,    22,    23,    24,    -1,    26,    -1,    -1,    -1,    30,
      31,    32,    33,    34,    35,    36,    -1,    -1,    39,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    48,    49,    -1,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      -1,    -1,    63,    -1,    -1,    66,    67,    -1,    69,    70,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    89,    -1,
      -1,    92,    93,    -1,    95,    96,    -1,    98,    99,   100,
     101,   102,   103,   104,   105,   106,    -1,    -1,   109,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   129,   130,
     131,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,    -1,
       7,    -1,   143,    -1,    11,    12,    -1,    -1,   149,    16,
      -1,    18,    19,    20,    21,    22,    23,    24,    -1,    26,
      -1,    -1,    -1,    30,    31,    32,    33,    34,    35,    36,
      -1,    -1,    39,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    48,    49,    -1,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    -1,    -1,    63,    -1,    -1,    66,
      67,    -1,    69,    70,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    89,    -1,    -1,    92,    93,    -1,    95,    96,
      -1,    98,    99,   100,   101,   102,   103,   104,   105,   106,
      -1,    -1,   109,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   129,   130,   131,    -1,    -1,    -1,    -1,    -1,
       3,     4,     5,    -1,     7,    -1,   143,    -1,    11,    12,
      -1,    -1,   149,    16,    -1,    18,    19,    20,    21,    22,
      23,    24,    -1,    -1,    -1,    -1,    -1,    30,    31,    32,
      33,    34,    35,    36,    -1,    -1,    39,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    48,    49,    -1,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    -1,    -1,
      63,    -1,    -1,    66,    67,    -1,    69,    70,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    89,    90,    -1,    92,
      93,    -1,    95,    96,    -1,    98,    99,   100,   101,   102,
     103,   104,   105,   106,    -1,    -1,   109,    -1,    -1,    -1,
      -1,    -1,    -1,   116,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,     3,     4,     5,    -1,     7,   129,   130,   131,    11,
      12,    -1,    -1,    -1,    16,    -1,    18,    19,    20,    21,
      22,    23,    24,    -1,    -1,    -1,   149,    -1,    30,    31,
      32,    33,    34,    35,    36,    -1,    -1,    39,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    48,    49,    -1,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    -1,
      -1,    63,    -1,    -1,    66,    67,    -1,    69,    70,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    89,    -1,    -1,
      92,    93,    -1,    95,    96,    -1,    98,    99,   100,   101,
     102,   103,   104,   105,   106,    -1,    -1,   109,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     3,     4,     5,    -1,     7,   129,   130,   131,
      11,    12,    -1,    -1,    -1,    16,    -1,    18,    19,    20,
      21,    22,    23,    24,    -1,    -1,    -1,   149,    -1,    30,
      31,    32,    33,    34,    35,    36,    -1,    -1,    39,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    48,    49,    -1,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      -1,    -1,    63,    -1,    -1,    66,    67,    -1,    69,    70,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    89,    -1,
      -1,    92,    93,    -1,    95,    96,    -1,    98,    99,   100,
     101,   102,   103,   104,   105,   106,    -1,    -1,   109,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   129,   130,
     131,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   149,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    -1,    -1,    -1,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    -1,    -1,    -1,    -1,
      -1,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    -1,    -1,    -1,    -1,    -1,    -1,    63,
      -1,    -1,    -1,    -1,    -1,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    -1,    -1,    80,    81,    -1,    -1,
      84,    85,    86,    87,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    98,    -1,    -1,    -1,    -1,    -1,
      -1,   105,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   119,   120,   121,   122,   123,
     124,   125,   126,   127,   128,    -1,   130,   131,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   148,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    -1,
      -1,    -1,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    -1,    -1,    -1,    -1,    -1,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    -1,
      -1,    -1,    -1,    -1,    -1,    63,    -1,    -1,    -1,    -1,
      -1,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      -1,    -1,    80,    81,    -1,    -1,    84,    85,    86,    87,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      98,    -1,    -1,    -1,    -1,    -1,    -1,   105,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   119,   120,   121,   122,   123,   124,   125,   126,   127,
     128,    -1,   130,   131,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     148,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    -1,    -1,    -1,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    -1,    -1,
      -1,    -1,    -1,    45,    46,    47,    48,    49,    50,    51,
      52,    -1,    -1,    55,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    -1,    -1,    80,    81,
      -1,    -1,    84,    85,    86,    87,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    98,    -1,    -1,   101,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   119,   120,   121,
     122,   123,   124,   125,   126,   127,   128,    -1,   130,   131,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   148,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    -1,    -1,    -1,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    -1,    -1,    -1,    -1,    -1,    45,
      46,    47,    48,    49,    50,    51,    52,    -1,    -1,    55,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    -1,    -1,    80,    81,    -1,    -1,    84,    85,
      86,    87,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    98,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   128,    -1,   130,   131,    -1,    -1,     3,     4,
       5,    -1,     7,    -1,    -1,    -1,    11,    12,    -1,    -1,
      -1,    16,   148,    18,    19,    20,    21,    22,    23,    24,
      -1,    -1,    -1,    -1,    -1,    30,    31,    32,    33,    34,
      35,    36,    -1,    -1,    39,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    48,    49,    -1,    51,    52,    53,    54,
      55,    56,    -1,    58,    59,    60,    -1,    -1,    63,    -1,
      -1,    66,    67,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    89,    -1,    -1,    92,    93,    -1,
      95,    96,    -1,    -1,    -1,   100,   101,   102,   103,   104,
     105,   106,    -1,    -1,   109,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,
      -1,     7,    -1,    -1,   129,    11,    12,    -1,    -1,    -1,
      16,    -1,    18,    19,    20,    21,    22,    23,    24,    -1,
      -1,    -1,   147,    -1,    30,    31,    32,    33,    34,    35,
      36,    -1,    -1,    39,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    48,    49,    -1,    51,    52,    53,    54,    55,
      56,    -1,    58,    59,    60,    -1,    -1,    63,    -1,    -1,
      66,    67,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    89,    -1,    -1,    92,    93,    -1,    95,
      96,    -1,    -1,    -1,   100,   101,   102,   103,   104,   105,
     106,    -1,    -1,   109,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,    -1,
       7,    -1,    -1,   129,    11,    12,    -1,    -1,    -1,    16,
      -1,    18,    19,    20,    21,    22,    23,    24,    -1,    -1,
      -1,   147,    -1,    30,    31,    32,    33,    34,    35,    36,
      -1,    -1,    39,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    48,    49,    -1,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    -1,    -1,    63,    -1,    -1,    66,
      67,    -1,    69,    70,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    89,    -1,    -1,    92,    93,    -1,    95,    96,
      -1,    98,    99,   100,   101,   102,   103,   104,   105,   106,
      -1,    -1,   109,   110,    -1,    -1,   113,   114,    -1,    -1,
      -1,    -1,     3,     4,     5,     6,     7,    -1,    -1,    -1,
      11,    12,   129,   130,   131,    16,    -1,    18,    19,    20,
      21,    22,    23,    24,    -1,    -1,    -1,    -1,    -1,    30,
      31,    32,    33,    34,    35,    36,    -1,    -1,    39,    -1,
      -1,    -1,    -1,    -1,    45,    46,    47,    48,    49,    -1,
      51,    52,    53,    54,    55,    56,    -1,    58,    59,    60,
      -1,    -1,    63,    -1,    -1,    66,    67,    -1,    69,    70,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    89,    -1,
      -1,    92,    93,    -1,    95,    96,    -1,    98,    -1,   100,
     101,   102,   103,   104,   105,   106,    -1,    -1,   109,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,
       5,    -1,     7,    -1,    -1,    -1,    11,    12,   129,   130,
     131,    16,    -1,    18,    19,    20,    21,    22,    23,    24,
      -1,    -1,    -1,    -1,    -1,    30,    31,    32,    33,    34,
      35,    36,    -1,    -1,    39,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    48,    49,    -1,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    -1,    -1,    63,    -1,
      -1,    66,    67,    -1,    69,    70,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    89,    -1,    -1,    92,    93,    -1,
      95,    96,    -1,    98,    -1,   100,   101,   102,   103,   104,
     105,   106,    -1,    -1,   109,   110,    -1,    -1,   113,   114,
      -1,    -1,    -1,    -1,     3,     4,     5,     6,     7,    -1,
      -1,    -1,    11,    12,   129,   130,   131,    16,    -1,    18,
      19,    20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,
      -1,    30,    31,    32,    33,    34,    35,    36,    -1,    -1,
      39,    -1,    -1,    -1,    -1,    -1,    45,    -1,    47,    48,
      49,    -1,    51,    52,    53,    54,    55,    56,    -1,    58,
      59,    60,    -1,    -1,    63,    -1,    -1,    66,    67,    -1,
      69,    70,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      89,    -1,    -1,    92,    93,    -1,    95,    96,    -1,    98,
      -1,   100,   101,   102,   103,   104,   105,   106,    -1,    -1,
     109,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
       3,     4,     5,    -1,     7,    -1,    -1,    -1,    11,    12,
     129,   130,   131,    16,    -1,    18,    19,    20,    21,    22,
      23,    24,    -1,    -1,    -1,    -1,    -1,    30,    31,    32,
      33,    34,    35,    36,    -1,    -1,    39,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    48,    49,    -1,    51,    52,
      53,    54,    55,    56,    -1,    58,    59,    60,    -1,    -1,
      63,    -1,    -1,    66,    67,    -1,    69,    70,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    89,    -1,    -1,    92,
      93,    -1,    95,    96,    -1,    98,    -1,   100,   101,   102,
     103,   104,   105,   106,    -1,    -1,   109,   110,    -1,    -1,
     113,   114,    -1,    -1,    -1,    -1,     3,     4,     5,    -1,
       7,    -1,    -1,    -1,    11,    12,   129,   130,   131,    16,
      -1,    18,    19,    20,    21,    22,    23,    24,    -1,    -1,
      -1,    -1,    -1,    30,    31,    32,    33,    34,    35,    36,
      -1,    -1,    39,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    48,    49,    -1,    51,    52,    53,    54,    55,    56,
      -1,    58,    59,    60,    -1,    -1,    63,    -1,    -1,    66,
      67,    -1,    69,    70,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    89,    -1,    -1,    92,    93,    -1,    95,    96,
      -1,    98,    -1,   100,   101,   102,   103,   104,   105,   106,
      -1,    -1,   109,   110,    -1,    -1,   113,   114,    -1,    -1,
      -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,
      11,    12,   129,   130,   131,    16,    -1,    18,    19,    20,
      21,    22,    23,    24,    -1,    -1,    -1,    -1,    -1,    30,
      31,    32,    33,    34,    35,    36,    -1,    -1,    39,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    48,    49,    -1,
      51,    52,    53,    54,    55,    56,    -1,    58,    59,    60,
      -1,    -1,    63,    -1,    -1,    66,    67,    -1,    69,    70,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    89,    -1,
      -1,    92,    93,    -1,    95,    96,    -1,    98,    -1,   100,
     101,   102,   103,   104,   105,   106,    -1,    -1,   109,   110,
      -1,    -1,   113,   114,    -1,    -1,    -1,    -1,     3,     4,
       5,    -1,     7,    -1,    -1,    -1,    11,    12,   129,   130,
     131,    16,    -1,    18,    19,    20,    21,    22,    23,    24,
      -1,    -1,    -1,    -1,    -1,    30,    31,    32,    33,    34,
      35,    36,    -1,    -1,    39,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    48,    49,    -1,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    -1,    -1,    63,    -1,
      -1,    66,    67,    -1,    69,    70,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    89,    -1,    -1,    92,    93,    -1,
      95,    96,    -1,    98,    99,   100,   101,   102,   103,   104,
     105,   106,    -1,    -1,   109,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,     3,     4,     5,    -1,     7,    -1,
      -1,    -1,    11,    12,   129,   130,   131,    16,    -1,    18,
      19,    20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,
      -1,    30,    31,    32,    33,    34,    35,    36,    -1,    -1,
      39,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    48,
      49,    -1,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    -1,    -1,    63,    -1,    -1,    66,    67,    -1,
      69,    70,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      89,    -1,    -1,    92,    93,    -1,    95,    96,    -1,    98,
      -1,   100,   101,   102,   103,   104,   105,   106,    -1,    -1,
     109,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
       3,     4,     5,    -1,     7,    -1,    -1,    -1,    11,    12,
     129,   130,   131,    16,    -1,    18,    19,    20,    21,    22,
      23,    24,    -1,    -1,    -1,    -1,    -1,    30,    31,    32,
      33,    34,    35,    36,    -1,    -1,    39,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    48,    49,    -1,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    -1,    -1,
      63,    -1,    -1,    66,    67,    -1,    69,    70,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    89,    -1,    -1,    92,
      93,    -1,    95,    96,    -1,    -1,    99,   100,   101,   102,
     103,   104,   105,   106,    -1,    -1,   109,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,    -1,
       7,    -1,    -1,    -1,    11,    12,   129,   130,   131,    16,
      -1,    18,    19,    20,    21,    22,    23,    24,    -1,    -1,
      -1,    -1,    -1,    30,    31,    32,    33,    34,    35,    36,
      -1,    -1,    39,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    48,    49,    -1,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    -1,    -1,    63,    -1,    -1,    66,
      67,    -1,    69,    70,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    89,    -1,    -1,    92,    93,    -1,    95,    96,
      -1,    -1,    -1,   100,   101,   102,   103,   104,   105,   106,
      -1,    -1,   109,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,
      11,    12,   129,   130,   131,    16,    -1,    18,    19,    20,
      21,    22,    23,    24,    -1,    -1,    -1,    -1,    -1,    30,
      31,    32,    33,    34,    35,    36,    -1,    -1,    39,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    48,    49,    -1,
      51,    52,    53,    54,    55,    56,    -1,    58,    59,    60,
      -1,    -1,    63,    -1,    -1,    66,    67,    -1,    69,    70,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    89,    -1,
      -1,    92,    93,    -1,    95,    96,    -1,    98,    -1,   100,
     101,   102,   103,   104,   105,   106,    -1,    -1,   109,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,
       5,    -1,     7,    -1,    -1,    -1,    11,    12,   129,   130,
     131,    16,    -1,    18,    19,    20,    21,    22,    23,    24,
      -1,    -1,    -1,    -1,    -1,    30,    31,    32,    33,    34,
      35,    36,    -1,    -1,    39,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    48,    49,    -1,    51,    52,    53,    54,
      55,    56,    -1,    58,    59,    60,    -1,    -1,    63,    -1,
      -1,    66,    67,    -1,    69,    70,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    89,    -1,    -1,    92,    93,    -1,
      95,    96,    -1,    98,    -1,   100,   101,   102,   103,   104,
     105,   106,    -1,    -1,   109,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,     3,     4,     5,    -1,     7,    -1,
      -1,    -1,    11,    12,   129,   130,   131,    16,    -1,    18,
      19,    20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,
      -1,    30,    31,    32,    33,    34,    35,    36,    -1,    -1,
      39,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    48,
      49,    -1,    51,    52,    53,    54,    55,    56,    -1,    58,
      59,    60,    -1,    -1,    63,    -1,    -1,    66,    67,    -1,
      69,    70,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      89,    -1,    -1,    92,    93,    -1,    95,    96,    -1,    98,
      -1,   100,   101,   102,   103,   104,   105,   106,    -1,    -1,
     109,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
       3,     4,     5,    -1,     7,    -1,    -1,    -1,    11,    12,
     129,   130,   131,    16,    -1,    18,    19,    20,    21,    22,
      23,    24,    -1,    -1,    -1,    -1,    -1,    30,    31,    32,
      33,    34,    35,    36,    -1,    -1,    39,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    48,    49,    -1,    51,    52,
      53,    54,    55,    56,    -1,    58,    59,    60,    -1,    -1,
      63,    -1,    -1,    66,    67,    -1,    69,    70,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    89,    -1,    -1,    92,
      93,    -1,    95,    96,    -1,    -1,    -1,   100,   101,   102,
     103,   104,   105,   106,    -1,    -1,   109,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,    -1,
       7,    -1,    -1,    -1,    11,    12,   129,   130,   131,    16,
      -1,    18,    19,    20,    21,    22,    23,    24,    -1,    -1,
      -1,    -1,    -1,    30,    31,    32,    33,    34,    35,    36,
      -1,    -1,    39,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    48,    49,    -1,    51,    52,    53,    54,    55,    56,
      -1,    58,    59,    60,    -1,    -1,    63,    -1,    -1,    66,
      67,    -1,    69,    70,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    89,    -1,    -1,    92,    93,    -1,    95,    96,
      -1,    -1,    -1,   100,   101,   102,   103,   104,   105,   106,
      -1,    -1,   109,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,
      11,    12,   129,   130,   131,    16,    -1,    18,    19,    20,
      21,    22,    23,    24,    -1,    -1,    -1,    -1,    -1,    30,
      31,    32,    33,    34,    35,    36,    -1,    -1,    39,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    48,    49,    -1,
      51,    52,    53,    54,    55,    56,    -1,    58,    59,    60,
      -1,    -1,    63,    -1,    -1,    66,    67,    -1,    69,    70,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    89,    -1,
      -1,    92,    93,    -1,    95,    96,    -1,    -1,    -1,   100,
     101,   102,   103,   104,   105,   106,    -1,    -1,   109,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,
       5,    -1,     7,    -1,    -1,    -1,    11,    12,   129,   130,
     131,    16,    -1,    18,    19,    20,    21,    22,    23,    24,
      -1,    -1,    -1,    -1,    -1,    30,    31,    32,    33,    34,
      35,    36,    -1,    -1,    39,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    48,    49,    -1,    51,    52,    53,    54,
      55,    56,    -1,    58,    59,    60,    -1,    -1,    63,    -1,
      -1,    66,    67,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    89,    -1,    -1,    92,    93,    -1,
      95,    96,    -1,    98,    -1,   100,   101,   102,   103,   104,
     105,   106,    -1,    -1,   109,    -1,    -1,    -1,    -1,    -1,
       3,     4,     5,    -1,     7,    -1,    -1,    -1,    11,    12,
      -1,    -1,    -1,    16,   129,    18,    19,    20,    21,    22,
      23,    24,    -1,    -1,    -1,    -1,    -1,    30,    31,    32,
      33,    34,    35,    36,    -1,    -1,    39,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    48,    49,    -1,    51,    52,
      53,    54,    55,    56,    -1,    58,    59,    60,    -1,    -1,
      63,    -1,    -1,    66,    67,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    89,    -1,    -1,    92,
      93,    -1,    95,    96,    -1,    98,    -1,   100,   101,   102,
     103,   104,   105,   106,    -1,    -1,   109,    -1,    -1,    -1,
      -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,    -1,
      11,    12,    -1,    -1,    -1,    16,   129,    18,    19,    20,
      21,    22,    23,    24,    -1,    -1,    -1,    -1,    -1,    30,
      31,    32,    33,    34,    35,    36,    -1,    -1,    39,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    48,    49,    -1,
      51,    52,    53,    54,    55,    56,    -1,    58,    59,    60,
      -1,    -1,    63,    -1,    -1,    66,    67,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    89,    -1,
      -1,    92,    93,    -1,    95,    96,    -1,    -1,    -1,   100,
     101,   102,   103,   104,   105,   106,    -1,    -1,   109,    -1,
      -1,    -1,    -1,    -1,     3,     4,     5,    -1,     7,    -1,
      -1,    -1,    11,    12,    -1,    -1,    -1,    16,   129,    18,
      19,    20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,
      -1,    30,    31,    32,    33,    34,    35,    36,    -1,    -1,
      39,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    48,
      49,    -1,    51,    52,    53,    54,    55,    56,    -1,    58,
      59,    60,    -1,    -1,    63,    -1,    -1,    66,    67,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      89,    -1,    -1,    92,    93,    -1,    95,    96,    -1,    -1,
      -1,   100,   101,   102,   103,   104,   105,   106,    -1,    -1,
     109,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,    -1,
       7,    -1,    -1,    -1,    11,    12,    -1,    -1,    -1,    16,
     129,    18,    19,    20,    21,    22,    23,    24,    -1,    -1,
      -1,    -1,    -1,    30,    31,    32,    33,    34,    35,    36,
      -1,    -1,    39,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    48,    49,    -1,    51,    52,    53,    54,    55,    56,
      -1,    58,    59,    60,    -1,    -1,    63,    -1,    -1,    66,
      67,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    89,    -1,    -1,    92,    93,    -1,    95,    96,
      -1,    -1,    -1,   100,   101,   102,   103,   104,   105,   106,
      -1,    -1,   109,    -1,    -1,    -1,    -1,    -1,     3,     4,
       5,    -1,     7,    -1,    -1,    -1,    11,    12,    -1,    -1,
      -1,    16,   129,    18,    19,    20,    21,    22,    23,    24,
      -1,    -1,    -1,    -1,    -1,    30,    31,    32,    33,    34,
      35,    36,    -1,    -1,    39,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    48,    49,    -1,    51,    52,    53,    54,
      55,    56,    -1,    58,    59,    60,    -1,    -1,    63,    -1,
      -1,    66,    67,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    89,    -1,    -1,    92,    93,    -1,
      95,    96,    -1,    -1,    -1,   100,   101,   102,   103,   104,
     105,   106,    -1,    -1,   109,    -1,    -1,    51,    52,    -1,
      -1,    55,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   129,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    -1,    -1,    80,    81,    -1,    -1,
      84,    85,    86,    87,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    98,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   119,   120,   121,   122,   123,
     124,   125,   126,   127,   128,    -1,   130,   131,    51,    52,
      -1,    -1,    55,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   148,   149,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    -1,    -1,    80,    81,    -1,
      -1,    84,    85,    86,    87,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    98,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   119,   120,   121,   122,
     123,   124,   125,   126,   127,   128,    -1,   130,   131,    51,
      52,    -1,    -1,    55,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   148,   149,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    -1,    -1,    80,    81,
      -1,    -1,    84,    85,    86,    87,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    98,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   119,   120,   121,
     122,   123,   124,   125,   126,   127,   128,    -1,   130,   131,
      51,    52,    -1,    -1,    55,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   148,   149,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    -1,    -1,    80,
      81,    -1,    -1,    84,    85,    86,    87,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    98,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   119,   120,
     121,   122,   123,   124,   125,   126,   127,   128,    -1,   130,
     131,    51,    52,    -1,    -1,    55,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   148,   149,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    -1,    -1,
      80,    81,    -1,    -1,    84,    85,    86,    87,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    98,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   119,
     120,   121,   122,   123,   124,   125,   126,   127,   128,    -1,
     130,   131,    51,    52,    -1,    -1,    55,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   148,   149,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    -1,
      -1,    80,    81,    -1,    -1,    84,    85,    86,    87,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    98,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     119,   120,   121,   122,   123,   124,   125,   126,   127,   128,
      -1,   130,   131,    51,    52,    -1,    -1,    55,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   148,
     149,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      -1,    -1,    80,    81,    -1,    -1,    84,    85,    86,    87,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      98,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   119,   120,   121,   122,   123,   124,   125,   126,   127,
     128,    -1,   130,   131,    51,    52,    -1,    -1,    55,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     148,   149,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    -1,    -1,    80,    81,    -1,    -1,    84,    85,    86,
      87,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    98,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   119,   120,   121,   122,   123,   124,   125,   126,
     127,   128,    -1,   130,   131,    51,    52,    -1,    -1,    55,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   148,   149,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    -1,    -1,    80,    81,    -1,    -1,    84,    85,
      86,    87,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    98,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   128,    -1,   130,   131,    51,    52,    -1,    -1,
      55,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   148,   149,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    -1,    -1,    80,    81,    -1,    -1,    84,
      85,    86,    87,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    98,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,    -1,   130,   131,    51,    52,    -1,
      -1,    55,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   148,   149,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    -1,    -1,    80,    81,    -1,    -1,
      84,    85,    86,    87,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    98,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   119,   120,   121,   122,   123,
     124,   125,   126,   127,   128,    -1,   130,   131,    51,    52,
      -1,    -1,    55,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   148,   149,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    -1,    -1,    80,    81,    -1,
      -1,    84,    85,    86,    87,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    98,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   119,   120,   121,   122,
     123,   124,   125,   126,   127,   128,    -1,   130,   131,    51,
      52,    -1,    -1,    55,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   148,   149,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    -1,    -1,    80,    81,
      -1,    -1,    84,    85,    86,    87,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    98,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   119,   120,   121,
     122,   123,   124,   125,   126,   127,   128,    -1,   130,   131,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   148
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint16 yystos[] =
{
       0,   155,   156,     0,     1,     3,     4,     5,     6,     7,
      11,    12,    16,    18,    19,    20,    21,    22,    23,    24,
      30,    31,    32,    33,    34,    35,    36,    39,    45,    46,
      47,    48,    49,    51,    52,    53,    54,    55,    56,    58,
      59,    60,    63,    66,    67,    69,    70,    89,    92,    93,
      95,    96,    98,   100,   101,   102,   103,   104,   105,   106,
     109,   129,   130,   131,   157,   158,   159,   164,   166,   167,
     169,   170,   173,   174,   176,   177,   178,   180,   181,   190,
     203,   225,   244,   245,   255,   256,   260,   261,   262,   269,
     270,   271,   273,   274,   275,   276,   277,   278,   302,   315,
     159,   215,   217,   220,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    45,    46,
      47,    48,    49,    50,    51,    52,    55,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    80,    81,    84,    85,
      86,    87,    98,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   128,   130,   131,   148,   149,   184,   188,   189,
     275,   277,   297,    33,    34,    35,    36,    48,    49,    51,
      55,   101,   184,   185,   186,   271,   204,    89,    92,   167,
     168,   181,   225,   275,   276,   278,   168,   152,   153,   168,
     306,   311,   312,   314,   209,   211,    21,    22,    30,    31,
      32,    39,    89,   129,   174,   181,   203,   225,   230,   275,
     278,   302,    57,    98,    99,   130,   173,   190,   191,   196,
     199,   201,   300,   301,   196,   196,   149,   197,   198,   149,
     193,   197,   149,   307,   312,   185,   160,   143,   190,   225,
     190,    55,     1,    92,   162,   163,   164,   175,   176,   315,
     205,   207,   192,   201,   300,   315,   191,   299,   300,   315,
      89,   147,   180,   225,   275,   278,   208,    53,    54,    56,
      63,   105,   184,   272,    62,    64,    65,   111,   112,   257,
     258,    63,   257,    63,   257,    63,   257,    61,   257,    58,
      59,   169,   190,   190,   306,   314,    40,    41,    42,    43,
      44,    37,    38,    28,   242,   116,   147,    92,    98,   177,
     116,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    86,    87,   117,   119,   120,   121,
     122,   123,   124,   125,   126,   127,   128,    88,   145,   146,
      88,   146,   305,    26,   143,   246,    90,    90,   193,   197,
     246,    51,    55,    89,   182,   183,   225,   278,    86,   183,
     298,   222,   305,   105,   147,   161,   162,    55,    13,   226,
     311,   116,    88,   145,   146,    90,    90,   226,   313,   306,
      17,   249,   152,   168,   168,    55,    58,    59,    88,   145,
     146,    25,   191,   191,   191,    91,   147,   200,   315,   147,
     200,   196,   307,   308,   196,   195,   196,   201,   300,   315,
     167,   308,   167,   165,   143,   162,    88,   146,    90,   164,
     175,   150,   306,   314,   308,   167,   308,   151,   147,   310,
     312,   147,   310,   144,   310,    55,   177,   178,   179,   147,
      88,   145,   146,    51,    53,    54,    55,    56,    92,    98,
      99,   123,   126,   149,   240,   282,   283,   284,   285,   286,
     287,   288,   291,   292,   293,   294,   295,    63,   258,   259,
     110,   113,   114,   264,   265,   266,   267,    62,   258,    63,
      63,    63,    61,    71,    71,   159,   168,   168,   168,   168,
     164,   167,   167,   243,    98,   169,   191,   201,   202,   175,
     147,   180,   147,   166,   169,   181,   190,   191,   202,   190,
     190,   190,   190,   190,   190,   190,   190,   190,   190,   190,
     190,   190,   190,   190,   190,   190,   190,   190,   190,   190,
     190,   190,   190,   190,   190,    51,    52,    55,   188,   193,
     303,   304,   195,    51,    52,    55,   188,   193,   303,    51,
      55,   303,   248,   247,   169,   190,   169,   190,    97,   171,
      51,    55,   182,     1,   120,   279,   311,    88,   145,   146,
     167,   221,   167,   149,   281,   282,   223,   187,    10,     8,
     251,   315,   162,    13,   190,    51,    55,   195,    51,    55,
     162,   263,   264,   249,   201,    10,    27,   227,   311,   227,
      51,    55,   303,   195,    51,    55,   303,   213,   191,    98,
     191,   199,   264,   300,   301,   308,   150,   308,   147,   147,
     308,   185,   157,   144,   190,   308,   164,   206,   300,   177,
     179,    51,    55,   195,    51,    55,   116,    51,    92,    98,
     231,   232,   233,   284,   282,    29,   108,   241,   147,   296,
     315,   191,   147,   296,    51,   147,   296,    51,    63,   162,
     268,   265,   110,   267,   190,   190,    79,   121,   235,   236,
     315,   191,   147,   308,   179,   147,   116,    44,   307,    90,
      90,   193,   197,   307,   309,    90,    90,   193,   194,   197,
     315,   194,   197,   235,   235,    44,   172,   311,   280,   216,
      51,    55,   182,   195,   218,   161,   308,   282,   161,   311,
     184,   185,   191,   202,   252,   315,    15,   229,   315,    14,
     228,   229,    90,    90,   309,    90,    90,   229,    10,   147,
     226,   210,   212,   309,   168,   191,    98,   191,   200,   300,
     144,   308,   310,   309,   233,   147,   284,   147,   308,   237,
     307,   162,   162,   285,   291,   293,   295,   286,   288,   293,
     286,   144,   162,   234,   237,   286,   287,   289,   290,   293,
     295,   162,    98,   191,   179,   190,   118,   169,   190,   169,
     190,   171,   151,    90,   169,   190,   169,   190,   171,   246,
     242,   162,   162,   190,   235,   168,   161,   309,   311,    10,
     308,    10,   224,    91,   253,   315,   162,     9,   254,   315,
     168,    10,    90,    10,   191,   162,   162,   162,   227,   191,
     147,   308,   232,   147,    98,   231,   150,   152,    10,   144,
     147,   296,   147,   296,   147,   296,   147,   296,   296,   144,
     237,   121,   147,   296,    89,   225,   147,   296,   147,   296,
      10,   191,   190,   169,   190,    10,   144,   162,   311,    10,
     219,   281,    89,   181,   225,   275,   278,   226,   162,   226,
     229,   249,   250,    10,    10,   214,   147,   232,   147,   284,
      51,   238,   239,   283,   286,   293,   286,   286,   121,   290,
     293,    55,    88,   286,   289,   293,   286,   144,   161,   161,
      55,    88,   145,   146,   162,   162,   162,   232,   147,   147,
     307,   296,   147,   296,   296,   296,   147,   296,   147,   296,
      51,    55,   296,   147,   296,   296,    10,    10,    51,    55,
     195,    51,    55,   251,   228,    10,   232,   239,   286,   286,
     293,   286,   286,   309,   296,   296,   147,   296,   296,   296,
     286,   296
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  However,
   YYFAIL appears to be in use.  Nevertheless, it is formally deprecated
   in Bison 2.4.2's NEWS entry, where a plan to phase it out is
   discussed.  */

#define YYFAIL		goto yyerrlab
#if defined YYFAIL
  /* This is here to suppress warnings from the GCC cpp's
     -Wunused-macros.  Normally we don't worry about that warning, but
     some users do, and we want to make it easy for users to remove
     YYFAIL uses, which will produce warnings from Bison 2.5.  */
#endif

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (p, YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))

/* Error token number */
#define YYTERROR	1
#define YYERRCODE	256


/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */
#ifdef YYLEX_PARAM
# define YYLEX yylex (&yylval, YYLEX_PARAM)
#else
# define YYLEX yylex (&yylval, p)
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
      Type, Value, p); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, parser_state *p)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep, p)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    parser_state *p;
#endif
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  if (!yyvaluep)
    return;
  YYUSE (p);
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, parser_state *p)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep, p)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    parser_state *p;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep, p);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule, parser_state *p)
#else
static void
yy_reduce_print (yyvsp, yyrule, p)
    YYSTYPE *yyvsp;
    int yyrule;
    parser_state *p;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
       yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
           &(yyvsp[(yyi + 1) - (yynrhs)])
                     , p);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule, p); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
  switch (*++yyp)
    {
    case '\'':
    case ',':
      goto do_not_strip_quotes;

    case '\\':
      if (*++yyp != '\\')
        goto do_not_strip_quotes;
      /* Fall through.  */
    default:
      if (yyres)
        yyres[yyn] = *yyp;
      yyn++;
      break;

    case '"':
      if (yyres)
        yyres[yyn] = '\0';
      return yyn;
    }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULL, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULL;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - Assume YYFAIL is not used.  It's too flawed to consider.  See
       <http://lists.gnu.org/archive/html/bison-patches/2009-12/msg00024.html>
       for details.  YYERROR is fine as it does not invoke this
       function.
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULL, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, parser_state *p)
#else
static void
yydestruct (yymsg, yytype, yyvaluep, p)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
    parser_state *p;
#endif
{
  YYUSE (yyvaluep);
  YYUSE (p);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YYUSE (yytype);
}




/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (parser_state *p)
#else
int
yyparse (p)
    parser_state *p;
#endif
#endif
{
/* The lookahead symbol.  */
int yychar;


#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
static YYSTYPE yyval_default;
# define YY_INITIAL_VALUE(Value) = Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval YY_INITIAL_VALUE(yyval_default);

    /* Number of syntax errors so far.  */
    int yynerrs;

    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
  /* Give user a chance to reallocate the stack.  Use copies of
     these so that the &'s don't force the real ones into
     memory.  */
  YYSTYPE *yyvs1 = yyvs;
  yytype_int16 *yyss1 = yyss;

  /* Each stack pointer address is followed by the size of the
     data in use in that stack, in bytes.  This used to be a
     conditional around just the two extra args, but that might
     be undefined if yyoverflow is a macro.  */
  yyoverflow (YY_("memory exhausted"),
        &yyss1, yysize * sizeof (*yyssp),
        &yyvs1, yysize * sizeof (*yyvsp),
        &yystacksize);

  yyss = yyss1;
  yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
  goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
  yystacksize = YYMAXDEPTH;

      {
  yytype_int16 *yyss1 = yyss;
  union yyalloc *yyptr =
    (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
  if (! yyptr)
    goto yyexhaustedlab;
  YYSTACK_RELOCATE (yyss_alloc, yyss);
  YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
  if (yyss1 != yyssa)
    YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
      (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
  YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
/* Line 1787 of yacc.c  */
#line 1155 "parse.y"
    {
                      p->lstate = EXPR_BEG;
                      if (!p->locals) p->locals = cons(0,0);
                    }
    break;

  case 3:
/* Line 1787 of yacc.c  */
#line 1160 "parse.y"
    {
                      p->tree = new_scope(p, (yyvsp[(2) - (2)].nd));
                    }
    break;

  case 4:
/* Line 1787 of yacc.c  */
#line 1166 "parse.y"
    {
                      (yyval.nd) = (yyvsp[(1) - (2)].nd);
                    }
    break;

  case 5:
/* Line 1787 of yacc.c  */
#line 1172 "parse.y"
    {
                      (yyval.nd) = new_begin(p, 0);
                    }
    break;

  case 6:
/* Line 1787 of yacc.c  */
#line 1176 "parse.y"
    {
                      (yyval.nd) = new_begin(p, (yyvsp[(1) - (1)].nd));
                    }
    break;

  case 7:
/* Line 1787 of yacc.c  */
#line 1180 "parse.y"
    {
                      (yyval.nd) = push((yyvsp[(1) - (3)].nd), newline_node((yyvsp[(3) - (3)].nd)));
                    }
    break;

  case 8:
/* Line 1787 of yacc.c  */
#line 1184 "parse.y"
    {
                      (yyval.nd) = new_begin(p, 0);
                    }
    break;

  case 10:
/* Line 1787 of yacc.c  */
#line 1191 "parse.y"
    {
                      (yyval.nd) = local_switch(p);
                    }
    break;

  case 11:
/* Line 1787 of yacc.c  */
#line 1195 "parse.y"
    {
                      yyerror(p, "BEGIN not supported");
                      local_resume(p, (yyvsp[(2) - (5)].nd));
                      (yyval.nd) = 0;
                    }
    break;

  case 12:
/* Line 1787 of yacc.c  */
#line 1206 "parse.y"
    {
                      if ((yyvsp[(2) - (4)].nd)) {
                        (yyval.nd) = new_rescue(p, (yyvsp[(1) - (4)].nd), (yyvsp[(2) - (4)].nd), (yyvsp[(3) - (4)].nd));
                      }
                      else if ((yyvsp[(3) - (4)].nd)) {
                        yywarn(p, "else without rescue is useless");
                        (yyval.nd) = push((yyvsp[(1) - (4)].nd), (yyvsp[(3) - (4)].nd));
                      }
                      else {
                        (yyval.nd) = (yyvsp[(1) - (4)].nd);
                      }
                      if ((yyvsp[(4) - (4)].nd)) {
                        if ((yyval.nd)) {
                          (yyval.nd) = new_ensure(p, (yyval.nd), (yyvsp[(4) - (4)].nd));
                        }
                        else {
                          (yyval.nd) = push((yyvsp[(4) - (4)].nd), new_nil(p));
                        }
                      }
                    }
    break;

  case 13:
/* Line 1787 of yacc.c  */
#line 1229 "parse.y"
    {
                      (yyval.nd) = (yyvsp[(1) - (2)].nd);
                    }
    break;

  case 14:
/* Line 1787 of yacc.c  */
#line 1235 "parse.y"
    {
                      (yyval.nd) = new_begin(p, 0);
                    }
    break;

  case 15:
/* Line 1787 of yacc.c  */
#line 1239 "parse.y"
    {
                      (yyval.nd) = new_begin(p, (yyvsp[(1) - (1)].nd));
                    }
    break;

  case 16:
/* Line 1787 of yacc.c  */
#line 1243 "parse.y"
    {
                        (yyval.nd) = push((yyvsp[(1) - (3)].nd), newline_node((yyvsp[(3) - (3)].nd)));
                    }
    break;

  case 17:
/* Line 1787 of yacc.c  */
#line 1247 "parse.y"
    {
                      (yyval.nd) = new_begin(p, (yyvsp[(2) - (2)].nd));
                    }
    break;

  case 18:
/* Line 1787 of yacc.c  */
#line 1252 "parse.y"
    {p->lstate = EXPR_FNAME;}
    break;

  case 19:
/* Line 1787 of yacc.c  */
#line 1253 "parse.y"
    {
                      (yyval.nd) = new_alias(p, (yyvsp[(2) - (4)].id), (yyvsp[(4) - (4)].id));
                    }
    break;

  case 20:
/* Line 1787 of yacc.c  */
#line 1257 "parse.y"
    {
                      (yyval.nd) = (yyvsp[(2) - (2)].nd);
                    }
    break;

  case 21:
/* Line 1787 of yacc.c  */
#line 1261 "parse.y"
    {
                        (yyval.nd) = new_if(p, cond((yyvsp[(3) - (3)].nd)), (yyvsp[(1) - (3)].nd), 0);
                    }
    break;

  case 22:
/* Line 1787 of yacc.c  */
#line 1265 "parse.y"
    {
                      (yyval.nd) = new_unless(p, cond((yyvsp[(3) - (3)].nd)), (yyvsp[(1) - (3)].nd), 0);
                    }
    break;

  case 23:
/* Line 1787 of yacc.c  */
#line 1269 "parse.y"
    {
                      (yyval.nd) = new_while(p, cond((yyvsp[(3) - (3)].nd)), (yyvsp[(1) - (3)].nd));
                    }
    break;

  case 24:
/* Line 1787 of yacc.c  */
#line 1273 "parse.y"
    {
                      (yyval.nd) = new_until(p, cond((yyvsp[(3) - (3)].nd)), (yyvsp[(1) - (3)].nd));
                    }
    break;

  case 25:
/* Line 1787 of yacc.c  */
#line 1277 "parse.y"
    {
                      (yyval.nd) = new_rescue(p, (yyvsp[(1) - (3)].nd), list1(list3(0, 0, (yyvsp[(3) - (3)].nd))), 0);
                    }
    break;

  case 26:
/* Line 1787 of yacc.c  */
#line 1281 "parse.y"
    {
                      yyerror(p, "END not suported");
                      (yyval.nd) = new_postexe(p, (yyvsp[(3) - (4)].nd));
                    }
    break;

  case 28:
/* Line 1787 of yacc.c  */
#line 1287 "parse.y"
    {
                      (yyval.nd) = new_masgn(p, (yyvsp[(1) - (3)].nd), (yyvsp[(3) - (3)].nd));
                    }
    break;

  case 29:
/* Line 1787 of yacc.c  */
#line 1291 "parse.y"
    {
                      (yyval.nd) = new_op_asgn(p, (yyvsp[(1) - (3)].nd), (yyvsp[(2) - (3)].id), (yyvsp[(3) - (3)].nd));
                    }
    break;

  case 30:
/* Line 1787 of yacc.c  */
#line 1295 "parse.y"
    {
                      (yyval.nd) = new_op_asgn(p, new_call(p, (yyvsp[(1) - (6)].nd), intern("[]",2), (yyvsp[(3) - (6)].nd)), (yyvsp[(5) - (6)].id), (yyvsp[(6) - (6)].nd));
                    }
    break;

  case 31:
/* Line 1787 of yacc.c  */
#line 1299 "parse.y"
    {
                      (yyval.nd) = new_op_asgn(p, new_call(p, (yyvsp[(1) - (5)].nd), (yyvsp[(3) - (5)].id), 0), (yyvsp[(4) - (5)].id), (yyvsp[(5) - (5)].nd));
                    }
    break;

  case 32:
/* Line 1787 of yacc.c  */
#line 1303 "parse.y"
    {
                      (yyval.nd) = new_op_asgn(p, new_call(p, (yyvsp[(1) - (5)].nd), (yyvsp[(3) - (5)].id), 0), (yyvsp[(4) - (5)].id), (yyvsp[(5) - (5)].nd));
                    }
    break;

  case 33:
/* Line 1787 of yacc.c  */
#line 1307 "parse.y"
    {
                      yyerror(p, "constant re-assignment");
                      (yyval.nd) = 0;
                    }
    break;

  case 34:
/* Line 1787 of yacc.c  */
#line 1312 "parse.y"
    {
                      (yyval.nd) = new_op_asgn(p, new_call(p, (yyvsp[(1) - (5)].nd), (yyvsp[(3) - (5)].id), 0), (yyvsp[(4) - (5)].id), (yyvsp[(5) - (5)].nd));
                    }
    break;

  case 35:
/* Line 1787 of yacc.c  */
#line 1316 "parse.y"
    {
                      backref_error(p, (yyvsp[(1) - (3)].nd));
                      (yyval.nd) = new_begin(p, 0);
                    }
    break;

  case 36:
/* Line 1787 of yacc.c  */
#line 1321 "parse.y"
    {
                      (yyval.nd) = new_asgn(p, (yyvsp[(1) - (3)].nd), new_array(p, (yyvsp[(3) - (3)].nd)));
                    }
    break;

  case 37:
/* Line 1787 of yacc.c  */
#line 1325 "parse.y"
    {
                      (yyval.nd) = new_masgn(p, (yyvsp[(1) - (3)].nd), (yyvsp[(3) - (3)].nd));
                    }
    break;

  case 38:
/* Line 1787 of yacc.c  */
#line 1329 "parse.y"
    {
                      (yyval.nd) = new_masgn(p, (yyvsp[(1) - (3)].nd), new_array(p, (yyvsp[(3) - (3)].nd)));
                    }
    break;

  case 40:
/* Line 1787 of yacc.c  */
#line 1336 "parse.y"
    {
                      (yyval.nd) = new_asgn(p, (yyvsp[(1) - (3)].nd), (yyvsp[(3) - (3)].nd));
                    }
    break;

  case 41:
/* Line 1787 of yacc.c  */
#line 1340 "parse.y"
    {
                      (yyval.nd) = new_asgn(p, (yyvsp[(1) - (3)].nd), (yyvsp[(3) - (3)].nd));
                    }
    break;

  case 43:
/* Line 1787 of yacc.c  */
#line 1348 "parse.y"
    {
                      (yyval.nd) = new_and(p, (yyvsp[(1) - (3)].nd), (yyvsp[(3) - (3)].nd));
                    }
    break;

  case 44:
/* Line 1787 of yacc.c  */
#line 1352 "parse.y"
    {
                      (yyval.nd) = new_or(p, (yyvsp[(1) - (3)].nd), (yyvsp[(3) - (3)].nd));
                    }
    break;

  case 45:
/* Line 1787 of yacc.c  */
#line 1356 "parse.y"
    {
                      (yyval.nd) = call_uni_op(p, cond((yyvsp[(3) - (3)].nd)), "!");
                    }
    break;

  case 46:
/* Line 1787 of yacc.c  */
#line 1360 "parse.y"
    {
                      (yyval.nd) = call_uni_op(p, cond((yyvsp[(2) - (2)].nd)), "!");
                    }
    break;

  case 48:
/* Line 1787 of yacc.c  */
#line 1367 "parse.y"
    {
                      if (!(yyvsp[(1) - (1)].nd)) (yyval.nd) = new_nil(p);
                      else (yyval.nd) = (yyvsp[(1) - (1)].nd);
                    }
    break;

  case 53:
/* Line 1787 of yacc.c  */
#line 1382 "parse.y"
    {
                      local_nest(p);
                    }
    break;

  case 54:
/* Line 1787 of yacc.c  */
#line 1388 "parse.y"
    {
                      (yyval.nd) = new_block(p, (yyvsp[(3) - (5)].nd), (yyvsp[(4) - (5)].nd));
                      local_unnest(p);
                    }
    break;

  case 55:
/* Line 1787 of yacc.c  */
#line 1395 "parse.y"
    {
                      (yyval.nd) = new_fcall(p, (yyvsp[(1) - (2)].id), (yyvsp[(2) - (2)].nd));
                    }
    break;

  case 56:
/* Line 1787 of yacc.c  */
#line 1399 "parse.y"
    {
                      args_with_block(p, (yyvsp[(2) - (3)].nd), (yyvsp[(3) - (3)].nd));
                      (yyval.nd) = new_fcall(p, (yyvsp[(1) - (3)].id), (yyvsp[(2) - (3)].nd));
                    }
    break;

  case 57:
/* Line 1787 of yacc.c  */
#line 1404 "parse.y"
    {
                      (yyval.nd) = new_call(p, (yyvsp[(1) - (4)].nd), (yyvsp[(3) - (4)].id), (yyvsp[(4) - (4)].nd));
                    }
    break;

  case 58:
/* Line 1787 of yacc.c  */
#line 1408 "parse.y"
    {
                      args_with_block(p, (yyvsp[(4) - (5)].nd), (yyvsp[(5) - (5)].nd));
                      (yyval.nd) = new_call(p, (yyvsp[(1) - (5)].nd), (yyvsp[(3) - (5)].id), (yyvsp[(4) - (5)].nd));
                   }
    break;

  case 59:
/* Line 1787 of yacc.c  */
#line 1413 "parse.y"
    {
                      (yyval.nd) = new_call(p, (yyvsp[(1) - (4)].nd), (yyvsp[(3) - (4)].id), (yyvsp[(4) - (4)].nd));
                    }
    break;

  case 60:
/* Line 1787 of yacc.c  */
#line 1417 "parse.y"
    {
                      args_with_block(p, (yyvsp[(4) - (5)].nd), (yyvsp[(5) - (5)].nd));
                      (yyval.nd) = new_call(p, (yyvsp[(1) - (5)].nd), (yyvsp[(3) - (5)].id), (yyvsp[(4) - (5)].nd));
                    }
    break;

  case 61:
/* Line 1787 of yacc.c  */
#line 1422 "parse.y"
    {
                      (yyval.nd) = new_super(p, (yyvsp[(2) - (2)].nd));
                    }
    break;

  case 62:
/* Line 1787 of yacc.c  */
#line 1426 "parse.y"
    {
                      (yyval.nd) = new_yield(p, (yyvsp[(2) - (2)].nd));
                    }
    break;

  case 63:
/* Line 1787 of yacc.c  */
#line 1430 "parse.y"
    {
                      (yyval.nd) = new_return(p, ret_args(p, (yyvsp[(2) - (2)].nd)));
                    }
    break;

  case 64:
/* Line 1787 of yacc.c  */
#line 1434 "parse.y"
    {
                      (yyval.nd) = new_break(p, ret_args(p, (yyvsp[(2) - (2)].nd)));
                    }
    break;

  case 65:
/* Line 1787 of yacc.c  */
#line 1438 "parse.y"
    {
                      (yyval.nd) = new_next(p, ret_args(p, (yyvsp[(2) - (2)].nd)));
                    }
    break;

  case 66:
/* Line 1787 of yacc.c  */
#line 1444 "parse.y"
    {
                      (yyval.nd) = (yyvsp[(1) - (1)].nd);
                    }
    break;

  case 67:
/* Line 1787 of yacc.c  */
#line 1448 "parse.y"
    {
                      (yyval.nd) = (yyvsp[(2) - (3)].nd);
                    }
    break;

  case 69:
/* Line 1787 of yacc.c  */
#line 1455 "parse.y"
    {
                      (yyval.nd) = list1((yyvsp[(2) - (3)].nd));
                    }
    break;

  case 70:
/* Line 1787 of yacc.c  */
#line 1461 "parse.y"
    {
                      (yyval.nd) = list1((yyvsp[(1) - (1)].nd));
                    }
    break;

  case 71:
/* Line 1787 of yacc.c  */
#line 1465 "parse.y"
    {
                      (yyval.nd) = list1(push((yyvsp[(1) - (2)].nd),(yyvsp[(2) - (2)].nd)));
                    }
    break;

  case 72:
/* Line 1787 of yacc.c  */
#line 1469 "parse.y"
    {
                      (yyval.nd) = list2((yyvsp[(1) - (3)].nd), (yyvsp[(3) - (3)].nd));
                    }
    break;

  case 73:
/* Line 1787 of yacc.c  */
#line 1473 "parse.y"
    {
                      (yyval.nd) = list3((yyvsp[(1) - (5)].nd), (yyvsp[(3) - (5)].nd), (yyvsp[(5) - (5)].nd));
                    }
    break;

  case 74:
/* Line 1787 of yacc.c  */
#line 1477 "parse.y"
    {
                      (yyval.nd) = list2((yyvsp[(1) - (2)].nd), new_nil(p));
                    }
    break;

  case 75:
/* Line 1787 of yacc.c  */
#line 1481 "parse.y"
    {
                      (yyval.nd) = list3((yyvsp[(1) - (4)].nd), new_nil(p), (yyvsp[(4) - (4)].nd));
                    }
    break;

  case 76:
/* Line 1787 of yacc.c  */
#line 1485 "parse.y"
    {
                      (yyval.nd) = list2(0, (yyvsp[(2) - (2)].nd));
                    }
    break;

  case 77:
/* Line 1787 of yacc.c  */
#line 1489 "parse.y"
    {
                      (yyval.nd) = list3(0, (yyvsp[(2) - (4)].nd), (yyvsp[(4) - (4)].nd));
                    }
    break;

  case 78:
/* Line 1787 of yacc.c  */
#line 1493 "parse.y"
    {
                      (yyval.nd) = list2(0, new_nil(p));
                    }
    break;

  case 79:
/* Line 1787 of yacc.c  */
#line 1497 "parse.y"
    {
                      (yyval.nd) = list3(0, new_nil(p), (yyvsp[(3) - (3)].nd));
                    }
    break;

  case 81:
/* Line 1787 of yacc.c  */
#line 1504 "parse.y"
    {
                      (yyval.nd) = (yyvsp[(2) - (3)].nd);
                    }
    break;

  case 82:
/* Line 1787 of yacc.c  */
#line 1510 "parse.y"
    {
                      (yyval.nd) = list1((yyvsp[(1) - (2)].nd));
                    }
    break;

  case 83:
/* Line 1787 of yacc.c  */
#line 1514 "parse.y"
    {
                      (yyval.nd) = push((yyvsp[(1) - (3)].nd), (yyvsp[(2) - (3)].nd));
                    }
    break;

  case 84:
/* Line 1787 of yacc.c  */
#line 1520 "parse.y"
    {
                      (yyval.nd) = list1((yyvsp[(1) - (1)].nd));
                    }
    break;

  case 85:
/* Line 1787 of yacc.c  */
#line 1524 "parse.y"
    {
                      (yyval.nd) = push((yyvsp[(1) - (2)].nd), (yyvsp[(2) - (2)].nd));
                    }
    break;

  case 86:
/* Line 1787 of yacc.c  */
#line 1530 "parse.y"
    {
                      assignable(p, (yyvsp[(1) - (1)].nd));
                    }
    break;

  case 87:
/* Line 1787 of yacc.c  */
#line 1534 "parse.y"
    {
                      (yyval.nd) = new_call(p, (yyvsp[(1) - (4)].nd), intern("[]",2), (yyvsp[(3) - (4)].nd));
                    }
    break;

  case 88:
/* Line 1787 of yacc.c  */
#line 1538 "parse.y"
    {
                      (yyval.nd) = new_call(p, (yyvsp[(1) - (3)].nd), (yyvsp[(3) - (3)].id), 0);
                    }
    break;

  case 89:
/* Line 1787 of yacc.c  */
#line 1542 "parse.y"
    {
                      (yyval.nd) = new_call(p, (yyvsp[(1) - (3)].nd), (yyvsp[(3) - (3)].id), 0);
                    }
    break;

  case 90:
/* Line 1787 of yacc.c  */
#line 1546 "parse.y"
    {
                      (yyval.nd) = new_call(p, (yyvsp[(1) - (3)].nd), (yyvsp[(3) - (3)].id), 0);
                    }
    break;

  case 91:
/* Line 1787 of yacc.c  */
#line 1550 "parse.y"
    {
                      if (p->in_def || p->in_single)
                        yyerror(p, "dynamic constant assignment");
                      (yyval.nd) = new_colon2(p, (yyvsp[(1) - (3)].nd), (yyvsp[(3) - (3)].id));
                    }
    break;

  case 92:
/* Line 1787 of yacc.c  */
#line 1556 "parse.y"
    {
                      if (p->in_def || p->in_single)
                        yyerror(p, "dynamic constant assignment");
                      (yyval.nd) = new_colon3(p, (yyvsp[(2) - (2)].id));
                    }
    break;

  case 93:
/* Line 1787 of yacc.c  */
#line 1562 "parse.y"
    {
                      backref_error(p, (yyvsp[(1) - (1)].nd));
                      (yyval.nd) = 0;
                    }
    break;

  case 94:
/* Line 1787 of yacc.c  */
#line 1569 "parse.y"
    {
                      assignable(p, (yyvsp[(1) - (1)].nd));
                    }
    break;

  case 95:
/* Line 1787 of yacc.c  */
#line 1573 "parse.y"
    {
                      (yyval.nd) = new_call(p, (yyvsp[(1) - (4)].nd), intern("[]",2), (yyvsp[(3) - (4)].nd));
                    }
    break;

  case 96:
/* Line 1787 of yacc.c  */
#line 1577 "parse.y"
    {
                      (yyval.nd) = new_call(p, (yyvsp[(1) - (3)].nd), (yyvsp[(3) - (3)].id), 0);
                    }
    break;

  case 97:
/* Line 1787 of yacc.c  */
#line 1581 "parse.y"
    {
                      (yyval.nd) = new_call(p, (yyvsp[(1) - (3)].nd), (yyvsp[(3) - (3)].id), 0);
                    }
    break;

  case 98:
/* Line 1787 of yacc.c  */
#line 1585 "parse.y"
    {
                      (yyval.nd) = new_call(p, (yyvsp[(1) - (3)].nd), (yyvsp[(3) - (3)].id), 0);
                    }
    break;

  case 99:
/* Line 1787 of yacc.c  */
#line 1589 "parse.y"
    {
                      if (p->in_def || p->in_single)
                        yyerror(p, "dynamic constant assignment");
                      (yyval.nd) = new_colon2(p, (yyvsp[(1) - (3)].nd), (yyvsp[(3) - (3)].id));
                    }
    break;

  case 100:
/* Line 1787 of yacc.c  */
#line 1595 "parse.y"
    {
                      if (p->in_def || p->in_single)
                        yyerror(p, "dynamic constant assignment");
                      (yyval.nd) = new_colon3(p, (yyvsp[(2) - (2)].id));
                    }
    break;

  case 101:
/* Line 1787 of yacc.c  */
#line 1601 "parse.y"
    {
                      backref_error(p, (yyvsp[(1) - (1)].nd));
                      (yyval.nd) = 0;
                    }
    break;

  case 102:
/* Line 1787 of yacc.c  */
#line 1608 "parse.y"
    {
                      yyerror(p, "class/module name must be CONSTANT");
                    }
    break;

  case 104:
/* Line 1787 of yacc.c  */
#line 1615 "parse.y"
    {
                      (yyval.nd) = cons((node*)1, nsym((yyvsp[(2) - (2)].id)));
                    }
    break;

  case 105:
/* Line 1787 of yacc.c  */
#line 1619 "parse.y"
    {
                      (yyval.nd) = cons((node*)0, nsym((yyvsp[(1) - (1)].id)));
                    }
    break;

  case 106:
/* Line 1787 of yacc.c  */
#line 1623 "parse.y"
    {
                      (yyval.nd) = cons((yyvsp[(1) - (3)].nd), nsym((yyvsp[(3) - (3)].id)));
                    }
    break;

  case 110:
/* Line 1787 of yacc.c  */
#line 1632 "parse.y"
    {
                      p->lstate = EXPR_ENDFN;
                      (yyval.id) = (yyvsp[(1) - (1)].id);
                    }
    break;

  case 111:
/* Line 1787 of yacc.c  */
#line 1637 "parse.y"
    {
                      p->lstate = EXPR_ENDFN;
                      (yyval.id) = (yyvsp[(1) - (1)].id);
                    }
    break;

  case 114:
/* Line 1787 of yacc.c  */
#line 1648 "parse.y"
    {
                      (yyval.nd) = new_undef(p, (yyvsp[(1) - (1)].id));
                    }
    break;

  case 115:
/* Line 1787 of yacc.c  */
#line 1651 "parse.y"
    {p->lstate = EXPR_FNAME;}
    break;

  case 116:
/* Line 1787 of yacc.c  */
#line 1652 "parse.y"
    {
                      (yyval.nd) = push((yyvsp[(1) - (4)].nd), nsym((yyvsp[(4) - (4)].id)));
                    }
    break;

  case 117:
/* Line 1787 of yacc.c  */
#line 1657 "parse.y"
    { (yyval.id) = intern_c('|');   }
    break;

  case 118:
/* Line 1787 of yacc.c  */
#line 1658 "parse.y"
    { (yyval.id) = intern_c('^');   }
    break;

  case 119:
/* Line 1787 of yacc.c  */
#line 1659 "parse.y"
    { (yyval.id) = intern_c('&');   }
    break;

  case 120:
/* Line 1787 of yacc.c  */
#line 1660 "parse.y"
    { (yyval.id) = intern("<=>",3); }
    break;

  case 121:
/* Line 1787 of yacc.c  */
#line 1661 "parse.y"
    { (yyval.id) = intern("==",2);  }
    break;

  case 122:
/* Line 1787 of yacc.c  */
#line 1662 "parse.y"
    { (yyval.id) = intern("===",3); }
    break;

  case 123:
/* Line 1787 of yacc.c  */
#line 1663 "parse.y"
    { (yyval.id) = intern("=~",2);  }
    break;

  case 124:
/* Line 1787 of yacc.c  */
#line 1664 "parse.y"
    { (yyval.id) = intern("!~",2);  }
    break;

  case 125:
/* Line 1787 of yacc.c  */
#line 1665 "parse.y"
    { (yyval.id) = intern_c('>');   }
    break;

  case 126:
/* Line 1787 of yacc.c  */
#line 1666 "parse.y"
    { (yyval.id) = intern(">=",2);  }
    break;

  case 127:
/* Line 1787 of yacc.c  */
#line 1667 "parse.y"
    { (yyval.id) = intern_c('<');   }
    break;

  case 128:
/* Line 1787 of yacc.c  */
#line 1668 "parse.y"
    { (yyval.id) = intern("<=",2);  }
    break;

  case 129:
/* Line 1787 of yacc.c  */
#line 1669 "parse.y"
    { (yyval.id) = intern("!=",2);  }
    break;

  case 130:
/* Line 1787 of yacc.c  */
#line 1670 "parse.y"
    { (yyval.id) = intern("<<",2);  }
    break;

  case 131:
/* Line 1787 of yacc.c  */
#line 1671 "parse.y"
    { (yyval.id) = intern(">>",2);  }
    break;

  case 132:
/* Line 1787 of yacc.c  */
#line 1672 "parse.y"
    { (yyval.id) = intern_c('+');   }
    break;

  case 133:
/* Line 1787 of yacc.c  */
#line 1673 "parse.y"
    { (yyval.id) = intern_c('-');   }
    break;

  case 134:
/* Line 1787 of yacc.c  */
#line 1674 "parse.y"
    { (yyval.id) = intern_c('*');   }
    break;

  case 135:
/* Line 1787 of yacc.c  */
#line 1675 "parse.y"
    { (yyval.id) = intern_c('*');   }
    break;

  case 136:
/* Line 1787 of yacc.c  */
#line 1676 "parse.y"
    { (yyval.id) = intern_c('/');   }
    break;

  case 137:
/* Line 1787 of yacc.c  */
#line 1677 "parse.y"
    { (yyval.id) = intern_c('%');   }
    break;

  case 138:
/* Line 1787 of yacc.c  */
#line 1678 "parse.y"
    { (yyval.id) = intern("**",2);  }
    break;

  case 139:
/* Line 1787 of yacc.c  */
#line 1679 "parse.y"
    { (yyval.id) = intern_c('!');   }
    break;

  case 140:
/* Line 1787 of yacc.c  */
#line 1680 "parse.y"
    { (yyval.id) = intern_c('~');   }
    break;

  case 141:
/* Line 1787 of yacc.c  */
#line 1681 "parse.y"
    { (yyval.id) = intern("+@",2);  }
    break;

  case 142:
/* Line 1787 of yacc.c  */
#line 1682 "parse.y"
    { (yyval.id) = intern("-@",2);  }
    break;

  case 143:
/* Line 1787 of yacc.c  */
#line 1683 "parse.y"
    { (yyval.id) = intern("[]",2);  }
    break;

  case 144:
/* Line 1787 of yacc.c  */
#line 1684 "parse.y"
    { (yyval.id) = intern("[]=",3); }
    break;

  case 145:
/* Line 1787 of yacc.c  */
#line 1685 "parse.y"
    { (yyval.id) = intern_c('`');   }
    break;

  case 186:
/* Line 1787 of yacc.c  */
#line 1703 "parse.y"
    {
                      (yyval.nd) = new_asgn(p, (yyvsp[(1) - (3)].nd), (yyvsp[(3) - (3)].nd));
                    }
    break;

  case 187:
/* Line 1787 of yacc.c  */
#line 1707 "parse.y"
    {
                      (yyval.nd) = new_asgn(p, (yyvsp[(1) - (5)].nd), new_rescue(p, (yyvsp[(3) - (5)].nd), list1(list3(0, 0, (yyvsp[(5) - (5)].nd))), 0));
                    }
    break;

  case 188:
/* Line 1787 of yacc.c  */
#line 1711 "parse.y"
    {
                      (yyval.nd) = new_op_asgn(p, (yyvsp[(1) - (3)].nd), (yyvsp[(2) - (3)].id), (yyvsp[(3) - (3)].nd));
                    }
    break;

  case 189:
/* Line 1787 of yacc.c  */
#line 1715 "parse.y"
    {
                      (yyval.nd) = new_op_asgn(p, (yyvsp[(1) - (5)].nd), (yyvsp[(2) - (5)].id), new_rescue(p, (yyvsp[(3) - (5)].nd), list1(list3(0, 0, (yyvsp[(5) - (5)].nd))), 0));
                    }
    break;

  case 190:
/* Line 1787 of yacc.c  */
#line 1719 "parse.y"
    {
                      (yyval.nd) = new_op_asgn(p, new_call(p, (yyvsp[(1) - (6)].nd), intern("[]",2), (yyvsp[(3) - (6)].nd)), (yyvsp[(5) - (6)].id), (yyvsp[(6) - (6)].nd));
                    }
    break;

  case 191:
/* Line 1787 of yacc.c  */
#line 1723 "parse.y"
    {
                      (yyval.nd) = new_op_asgn(p, new_call(p, (yyvsp[(1) - (5)].nd), (yyvsp[(3) - (5)].id), 0), (yyvsp[(4) - (5)].id), (yyvsp[(5) - (5)].nd));
                    }
    break;

  case 192:
/* Line 1787 of yacc.c  */
#line 1727 "parse.y"
    {
                      (yyval.nd) = new_op_asgn(p, new_call(p, (yyvsp[(1) - (5)].nd), (yyvsp[(3) - (5)].id), 0), (yyvsp[(4) - (5)].id), (yyvsp[(5) - (5)].nd));
                    }
    break;

  case 193:
/* Line 1787 of yacc.c  */
#line 1731 "parse.y"
    {
                      (yyval.nd) = new_op_asgn(p, new_call(p, (yyvsp[(1) - (5)].nd), (yyvsp[(3) - (5)].id), 0), (yyvsp[(4) - (5)].id), (yyvsp[(5) - (5)].nd));
                    }
    break;

  case 194:
/* Line 1787 of yacc.c  */
#line 1735 "parse.y"
    {
                      yyerror(p, "constant re-assignment");
                      (yyval.nd) = new_begin(p, 0);
                    }
    break;

  case 195:
/* Line 1787 of yacc.c  */
#line 1740 "parse.y"
    {
                      yyerror(p, "constant re-assignment");
                      (yyval.nd) = new_begin(p, 0);
                    }
    break;

  case 196:
/* Line 1787 of yacc.c  */
#line 1745 "parse.y"
    {
                      backref_error(p, (yyvsp[(1) - (3)].nd));
                      (yyval.nd) = new_begin(p, 0);
                    }
    break;

  case 197:
/* Line 1787 of yacc.c  */
#line 1750 "parse.y"
    {
                      (yyval.nd) = new_dot2(p, (yyvsp[(1) - (3)].nd), (yyvsp[(3) - (3)].nd));
                    }
    break;

  case 198:
/* Line 1787 of yacc.c  */
#line 1754 "parse.y"
    {
                      (yyval.nd) = new_dot3(p, (yyvsp[(1) - (3)].nd), (yyvsp[(3) - (3)].nd));
                    }
    break;

  case 199:
/* Line 1787 of yacc.c  */
#line 1758 "parse.y"
    {
                      (yyval.nd) = call_bin_op(p, (yyvsp[(1) - (3)].nd), "+", (yyvsp[(3) - (3)].nd));
                    }
    break;

  case 200:
/* Line 1787 of yacc.c  */
#line 1762 "parse.y"
    {
                      (yyval.nd) = call_bin_op(p, (yyvsp[(1) - (3)].nd), "-", (yyvsp[(3) - (3)].nd));
                    }
    break;

  case 201:
/* Line 1787 of yacc.c  */
#line 1766 "parse.y"
    {
                      (yyval.nd) = call_bin_op(p, (yyvsp[(1) - (3)].nd), "*", (yyvsp[(3) - (3)].nd));
                    }
    break;

  case 202:
/* Line 1787 of yacc.c  */
#line 1770 "parse.y"
    {
                      (yyval.nd) = call_bin_op(p, (yyvsp[(1) - (3)].nd), "/", (yyvsp[(3) - (3)].nd));
                    }
    break;

  case 203:
/* Line 1787 of yacc.c  */
#line 1774 "parse.y"
    {
                      (yyval.nd) = call_bin_op(p, (yyvsp[(1) - (3)].nd), "%", (yyvsp[(3) - (3)].nd));
                    }
    break;

  case 204:
/* Line 1787 of yacc.c  */
#line 1778 "parse.y"
    {
                      (yyval.nd) = call_bin_op(p, (yyvsp[(1) - (3)].nd), "**", (yyvsp[(3) - (3)].nd));
                    }
    break;

  case 205:
/* Line 1787 of yacc.c  */
#line 1782 "parse.y"
    {
                      (yyval.nd) = call_uni_op(p, call_bin_op(p, (yyvsp[(2) - (4)].nd), "**", (yyvsp[(4) - (4)].nd)), "-@");
                    }
    break;

  case 206:
/* Line 1787 of yacc.c  */
#line 1786 "parse.y"
    {
                      (yyval.nd) = call_uni_op(p, call_bin_op(p, (yyvsp[(2) - (4)].nd), "**", (yyvsp[(4) - (4)].nd)), "-@");
                    }
    break;

  case 207:
/* Line 1787 of yacc.c  */
#line 1790 "parse.y"
    {
                      (yyval.nd) = call_uni_op(p, (yyvsp[(2) - (2)].nd), "+@");
                    }
    break;

  case 208:
/* Line 1787 of yacc.c  */
#line 1794 "parse.y"
    {
                      (yyval.nd) = call_uni_op(p, (yyvsp[(2) - (2)].nd), "-@");
                    }
    break;

  case 209:
/* Line 1787 of yacc.c  */
#line 1798 "parse.y"
    {
                      (yyval.nd) = call_bin_op(p, (yyvsp[(1) - (3)].nd), "|", (yyvsp[(3) - (3)].nd));
                    }
    break;

  case 210:
/* Line 1787 of yacc.c  */
#line 1802 "parse.y"
    {
                      (yyval.nd) = call_bin_op(p, (yyvsp[(1) - (3)].nd), "^", (yyvsp[(3) - (3)].nd));
                    }
    break;

  case 211:
/* Line 1787 of yacc.c  */
#line 1806 "parse.y"
    {
                      (yyval.nd) = call_bin_op(p, (yyvsp[(1) - (3)].nd), "&", (yyvsp[(3) - (3)].nd));
                    }
    break;

  case 212:
/* Line 1787 of yacc.c  */
#line 1810 "parse.y"
    {
                      (yyval.nd) = call_bin_op(p, (yyvsp[(1) - (3)].nd), "<=>", (yyvsp[(3) - (3)].nd));
                    }
    break;

  case 213:
/* Line 1787 of yacc.c  */
#line 1814 "parse.y"
    {
                      (yyval.nd) = call_bin_op(p, (yyvsp[(1) - (3)].nd), ">", (yyvsp[(3) - (3)].nd));
                    }
    break;

  case 214:
/* Line 1787 of yacc.c  */
#line 1818 "parse.y"
    {
                      (yyval.nd) = call_bin_op(p, (yyvsp[(1) - (3)].nd), ">=", (yyvsp[(3) - (3)].nd));
                    }
    break;

  case 215:
/* Line 1787 of yacc.c  */
#line 1822 "parse.y"
    {
                      (yyval.nd) = call_bin_op(p, (yyvsp[(1) - (3)].nd), "<", (yyvsp[(3) - (3)].nd));
                    }
    break;

  case 216:
/* Line 1787 of yacc.c  */
#line 1826 "parse.y"
    {
                      (yyval.nd) = call_bin_op(p, (yyvsp[(1) - (3)].nd), "<=", (yyvsp[(3) - (3)].nd));
                    }
    break;

  case 217:
/* Line 1787 of yacc.c  */
#line 1830 "parse.y"
    {
                      (yyval.nd) = call_bin_op(p, (yyvsp[(1) - (3)].nd), "==", (yyvsp[(3) - (3)].nd));
                    }
    break;

  case 218:
/* Line 1787 of yacc.c  */
#line 1834 "parse.y"
    {
                      (yyval.nd) = call_bin_op(p, (yyvsp[(1) - (3)].nd), "===", (yyvsp[(3) - (3)].nd));
                    }
    break;

  case 219:
/* Line 1787 of yacc.c  */
#line 1838 "parse.y"
    {
                      (yyval.nd) = call_bin_op(p, (yyvsp[(1) - (3)].nd), "!=", (yyvsp[(3) - (3)].nd));
                    }
    break;

  case 220:
/* Line 1787 of yacc.c  */
#line 1842 "parse.y"
    {
                      (yyval.nd) = call_bin_op(p, (yyvsp[(1) - (3)].nd), "=~", (yyvsp[(3) - (3)].nd));
                    }
    break;

  case 221:
/* Line 1787 of yacc.c  */
#line 1846 "parse.y"
    {
                      (yyval.nd) = call_bin_op(p, (yyvsp[(1) - (3)].nd), "!~", (yyvsp[(3) - (3)].nd));
                    }
    break;

  case 222:
/* Line 1787 of yacc.c  */
#line 1850 "parse.y"
    {
                      (yyval.nd) = call_uni_op(p, cond((yyvsp[(2) - (2)].nd)), "!");
                    }
    break;

  case 223:
/* Line 1787 of yacc.c  */
#line 1854 "parse.y"
    {
                      (yyval.nd) = call_uni_op(p, cond((yyvsp[(2) - (2)].nd)), "~");
                    }
    break;

  case 224:
/* Line 1787 of yacc.c  */
#line 1858 "parse.y"
    {
                      (yyval.nd) = call_bin_op(p, (yyvsp[(1) - (3)].nd), "<<", (yyvsp[(3) - (3)].nd));
                    }
    break;

  case 225:
/* Line 1787 of yacc.c  */
#line 1862 "parse.y"
    {
                      (yyval.nd) = call_bin_op(p, (yyvsp[(1) - (3)].nd), ">>", (yyvsp[(3) - (3)].nd));
                    }
    break;

  case 226:
/* Line 1787 of yacc.c  */
#line 1866 "parse.y"
    {
                      (yyval.nd) = new_and(p, (yyvsp[(1) - (3)].nd), (yyvsp[(3) - (3)].nd));
                    }
    break;

  case 227:
/* Line 1787 of yacc.c  */
#line 1870 "parse.y"
    {
                      (yyval.nd) = new_or(p, (yyvsp[(1) - (3)].nd), (yyvsp[(3) - (3)].nd));
                    }
    break;

  case 228:
/* Line 1787 of yacc.c  */
#line 1874 "parse.y"
    {
                      (yyval.nd) = new_if(p, cond((yyvsp[(1) - (6)].nd)), (yyvsp[(3) - (6)].nd), (yyvsp[(6) - (6)].nd));
                    }
    break;

  case 229:
/* Line 1787 of yacc.c  */
#line 1878 "parse.y"
    {
                      (yyval.nd) = (yyvsp[(1) - (1)].nd);
                    }
    break;

  case 230:
/* Line 1787 of yacc.c  */
#line 1884 "parse.y"
    {
                      (yyval.nd) = (yyvsp[(1) - (1)].nd);
                      if (!(yyval.nd)) (yyval.nd) = new_nil(p);
                    }
    break;

  case 232:
/* Line 1787 of yacc.c  */
#line 1892 "parse.y"
    {
                      (yyval.nd) = (yyvsp[(1) - (2)].nd);
                    }
    break;

  case 233:
/* Line 1787 of yacc.c  */
#line 1896 "parse.y"
    {
                      (yyval.nd) = push((yyvsp[(1) - (4)].nd), new_hash(p, (yyvsp[(3) - (4)].nd)));
                    }
    break;

  case 234:
/* Line 1787 of yacc.c  */
#line 1900 "parse.y"
    {
                      (yyval.nd) = cons(new_hash(p, (yyvsp[(1) - (2)].nd)), 0);
                    }
    break;

  case 235:
/* Line 1787 of yacc.c  */
#line 1906 "parse.y"
    {
                      (yyval.nd) = (yyvsp[(2) - (3)].nd);
                    }
    break;

  case 240:
/* Line 1787 of yacc.c  */
#line 1918 "parse.y"
    {
                      (yyval.nd) = cons((yyvsp[(1) - (2)].nd),0);
                    }
    break;

  case 241:
/* Line 1787 of yacc.c  */
#line 1922 "parse.y"
    {
                      (yyval.nd) = cons(push((yyvsp[(1) - (4)].nd), new_hash(p, (yyvsp[(3) - (4)].nd))), 0);
                    }
    break;

  case 242:
/* Line 1787 of yacc.c  */
#line 1926 "parse.y"
    {
                      (yyval.nd) = cons(list1(new_hash(p, (yyvsp[(1) - (2)].nd))), 0);
                    }
    break;

  case 243:
/* Line 1787 of yacc.c  */
#line 1932 "parse.y"
    {
                      (yyval.nd) = cons(list1((yyvsp[(1) - (1)].nd)), 0);
                    }
    break;

  case 244:
/* Line 1787 of yacc.c  */
#line 1936 "parse.y"
    {
                      (yyval.nd) = cons((yyvsp[(1) - (2)].nd), (yyvsp[(2) - (2)].nd));
                    }
    break;

  case 245:
/* Line 1787 of yacc.c  */
#line 1940 "parse.y"
    {
                      (yyval.nd) = cons(list1(new_hash(p, (yyvsp[(1) - (2)].nd))), (yyvsp[(2) - (2)].nd));
                    }
    break;

  case 246:
/* Line 1787 of yacc.c  */
#line 1944 "parse.y"
    {
                      (yyval.nd) = cons(push((yyvsp[(1) - (4)].nd), new_hash(p, (yyvsp[(3) - (4)].nd))), (yyvsp[(4) - (4)].nd));
                    }
    break;

  case 247:
/* Line 1787 of yacc.c  */
#line 1948 "parse.y"
    {
                      (yyval.nd) = cons(0, (yyvsp[(1) - (1)].nd));
                    }
    break;

  case 248:
/* Line 1787 of yacc.c  */
#line 1953 "parse.y"
    {
                      (yyval.stack) = p->cmdarg_stack;
                      CMDARG_PUSH(1);
                    }
    break;

  case 249:
/* Line 1787 of yacc.c  */
#line 1958 "parse.y"
    {
                      p->cmdarg_stack = (yyvsp[(1) - (2)].stack);
                      (yyval.nd) = (yyvsp[(2) - (2)].nd);
                    }
    break;

  case 250:
/* Line 1787 of yacc.c  */
#line 1965 "parse.y"
    {
                      (yyval.nd) = new_block_arg(p, (yyvsp[(2) - (2)].nd));
                    }
    break;

  case 251:
/* Line 1787 of yacc.c  */
#line 1971 "parse.y"
    {
                      (yyval.nd) = (yyvsp[(2) - (2)].nd);
                    }
    break;

  case 252:
/* Line 1787 of yacc.c  */
#line 1975 "parse.y"
    {
                      (yyval.nd) = 0;
                    }
    break;

  case 253:
/* Line 1787 of yacc.c  */
#line 1981 "parse.y"
    {
                      (yyval.nd) = cons((yyvsp[(1) - (1)].nd), 0);
                    }
    break;

  case 254:
/* Line 1787 of yacc.c  */
#line 1985 "parse.y"
    {
                      (yyval.nd) = cons(new_splat(p, (yyvsp[(2) - (2)].nd)), 0);
                    }
    break;

  case 255:
/* Line 1787 of yacc.c  */
#line 1989 "parse.y"
    {
                      (yyval.nd) = push((yyvsp[(1) - (3)].nd), (yyvsp[(3) - (3)].nd));
                    }
    break;

  case 256:
/* Line 1787 of yacc.c  */
#line 1993 "parse.y"
    {
                      (yyval.nd) = push((yyvsp[(1) - (4)].nd), new_splat(p, (yyvsp[(4) - (4)].nd)));
                    }
    break;

  case 257:
/* Line 1787 of yacc.c  */
#line 1997 "parse.y"
    {
                      (yyval.nd) = push((yyvsp[(1) - (4)].nd), (yyvsp[(4) - (4)].nd));
                    }
    break;

  case 258:
/* Line 1787 of yacc.c  */
#line 2001 "parse.y"
    {
                      (yyval.nd) = push((yyvsp[(1) - (5)].nd), new_splat(p, (yyvsp[(5) - (5)].nd)));
                    }
    break;

  case 259:
/* Line 1787 of yacc.c  */
#line 2007 "parse.y"
    {
                      (yyval.nd) = push((yyvsp[(1) - (3)].nd), (yyvsp[(3) - (3)].nd));
                    }
    break;

  case 260:
/* Line 1787 of yacc.c  */
#line 2011 "parse.y"
    {
                      (yyval.nd) = push((yyvsp[(1) - (4)].nd), new_splat(p, (yyvsp[(4) - (4)].nd)));
                    }
    break;

  case 261:
/* Line 1787 of yacc.c  */
#line 2015 "parse.y"
    {
                      (yyval.nd) = list1(new_splat(p, (yyvsp[(2) - (2)].nd)));
                    }
    break;

  case 269:
/* Line 1787 of yacc.c  */
#line 2028 "parse.y"
    {
                      (yyval.nd) = new_fcall(p, (yyvsp[(1) - (1)].id), 0);
                    }
    break;

  case 270:
/* Line 1787 of yacc.c  */
#line 2032 "parse.y"
    {
                      (yyvsp[(1) - (1)].stack) = p->cmdarg_stack;
                      p->cmdarg_stack = 0;
                    }
    break;

  case 271:
/* Line 1787 of yacc.c  */
#line 2038 "parse.y"
    {
                      p->cmdarg_stack = (yyvsp[(1) - (4)].stack);
                      (yyval.nd) = (yyvsp[(3) - (4)].nd);
                    }
    break;

  case 272:
/* Line 1787 of yacc.c  */
#line 2043 "parse.y"
    {
                      (yyvsp[(1) - (1)].stack) = p->cmdarg_stack;
                      p->cmdarg_stack = 0;
                    }
    break;

  case 273:
/* Line 1787 of yacc.c  */
#line 2047 "parse.y"
    {p->lstate = EXPR_ENDARG;}
    break;

  case 274:
/* Line 1787 of yacc.c  */
#line 2048 "parse.y"
    {
                      p->cmdarg_stack = (yyvsp[(1) - (5)].stack);
                      (yyval.nd) = (yyvsp[(3) - (5)].nd);
                    }
    break;

  case 275:
/* Line 1787 of yacc.c  */
#line 2052 "parse.y"
    {p->lstate = EXPR_ENDARG;}
    break;

  case 276:
/* Line 1787 of yacc.c  */
#line 2053 "parse.y"
    {
                      (yyval.nd) = 0;
                    }
    break;

  case 277:
/* Line 1787 of yacc.c  */
#line 2057 "parse.y"
    {
                      (yyval.nd) = (yyvsp[(2) - (3)].nd);
                    }
    break;

  case 278:
/* Line 1787 of yacc.c  */
#line 2061 "parse.y"
    {
                      (yyval.nd) = new_colon2(p, (yyvsp[(1) - (3)].nd), (yyvsp[(3) - (3)].id));
                    }
    break;

  case 279:
/* Line 1787 of yacc.c  */
#line 2065 "parse.y"
    {
                      (yyval.nd) = new_colon3(p, (yyvsp[(2) - (2)].id));
                    }
    break;

  case 280:
/* Line 1787 of yacc.c  */
#line 2069 "parse.y"
    {
                      (yyval.nd) = new_array(p, (yyvsp[(2) - (3)].nd));
                    }
    break;

  case 281:
/* Line 1787 of yacc.c  */
#line 2073 "parse.y"
    {
                      (yyval.nd) = new_hash(p, (yyvsp[(2) - (3)].nd));
                    }
    break;

  case 282:
/* Line 1787 of yacc.c  */
#line 2077 "parse.y"
    {
                      (yyval.nd) = new_return(p, 0);
                    }
    break;

  case 283:
/* Line 1787 of yacc.c  */
#line 2081 "parse.y"
    {
                      (yyval.nd) = new_yield(p, (yyvsp[(3) - (4)].nd));
                    }
    break;

  case 284:
/* Line 1787 of yacc.c  */
#line 2085 "parse.y"
    {
                      (yyval.nd) = new_yield(p, 0);
                    }
    break;

  case 285:
/* Line 1787 of yacc.c  */
#line 2089 "parse.y"
    {
                      (yyval.nd) = new_yield(p, 0);
                    }
    break;

  case 286:
/* Line 1787 of yacc.c  */
#line 2093 "parse.y"
    {
                      (yyval.nd) = call_uni_op(p, cond((yyvsp[(3) - (4)].nd)), "!");
                    }
    break;

  case 287:
/* Line 1787 of yacc.c  */
#line 2097 "parse.y"
    {
                      (yyval.nd) = call_uni_op(p, new_nil(p), "!");
                    }
    break;

  case 288:
/* Line 1787 of yacc.c  */
#line 2101 "parse.y"
    {
                      (yyval.nd) = new_fcall(p, (yyvsp[(1) - (2)].id), cons(0, (yyvsp[(2) - (2)].nd)));
                    }
    break;

  case 290:
/* Line 1787 of yacc.c  */
#line 2106 "parse.y"
    {
                      call_with_block(p, (yyvsp[(1) - (2)].nd), (yyvsp[(2) - (2)].nd));
                      (yyval.nd) = (yyvsp[(1) - (2)].nd);
                    }
    break;

  case 291:
/* Line 1787 of yacc.c  */
#line 2111 "parse.y"
    {
                      local_nest(p);
                      (yyval.num) = p->lpar_beg;
                      p->lpar_beg = ++p->paren_nest;
                    }
    break;

  case 292:
/* Line 1787 of yacc.c  */
#line 2118 "parse.y"
    {
                      p->lpar_beg = (yyvsp[(2) - (4)].num);
                      (yyval.nd) = new_lambda(p, (yyvsp[(3) - (4)].nd), (yyvsp[(4) - (4)].nd));
                      local_unnest(p);
                    }
    break;

  case 293:
/* Line 1787 of yacc.c  */
#line 2127 "parse.y"
    {
                      (yyval.nd) = new_if(p, cond((yyvsp[(2) - (6)].nd)), (yyvsp[(4) - (6)].nd), (yyvsp[(5) - (6)].nd));
                    }
    break;

  case 294:
/* Line 1787 of yacc.c  */
#line 2134 "parse.y"
    {
                      (yyval.nd) = new_unless(p, cond((yyvsp[(2) - (6)].nd)), (yyvsp[(4) - (6)].nd), (yyvsp[(5) - (6)].nd));
                    }
    break;

  case 295:
/* Line 1787 of yacc.c  */
#line 2137 "parse.y"
    {COND_PUSH(1);}
    break;

  case 296:
/* Line 1787 of yacc.c  */
#line 2137 "parse.y"
    {COND_POP();}
    break;

  case 297:
/* Line 1787 of yacc.c  */
#line 2140 "parse.y"
    {
                      (yyval.nd) = new_while(p, cond((yyvsp[(3) - (7)].nd)), (yyvsp[(6) - (7)].nd));
                    }
    break;

  case 298:
/* Line 1787 of yacc.c  */
#line 2143 "parse.y"
    {COND_PUSH(1);}
    break;

  case 299:
/* Line 1787 of yacc.c  */
#line 2143 "parse.y"
    {COND_POP();}
    break;

  case 300:
/* Line 1787 of yacc.c  */
#line 2146 "parse.y"
    {
                      (yyval.nd) = new_until(p, cond((yyvsp[(3) - (7)].nd)), (yyvsp[(6) - (7)].nd));
                    }
    break;

  case 301:
/* Line 1787 of yacc.c  */
#line 2152 "parse.y"
    {
                      (yyval.nd) = new_case(p, (yyvsp[(2) - (5)].nd), (yyvsp[(4) - (5)].nd));
                    }
    break;

  case 302:
/* Line 1787 of yacc.c  */
#line 2156 "parse.y"
    {
                      (yyval.nd) = new_case(p, 0, (yyvsp[(3) - (4)].nd));
                    }
    break;

  case 303:
/* Line 1787 of yacc.c  */
#line 2160 "parse.y"
    {COND_PUSH(1);}
    break;

  case 304:
/* Line 1787 of yacc.c  */
#line 2162 "parse.y"
    {COND_POP();}
    break;

  case 305:
/* Line 1787 of yacc.c  */
#line 2165 "parse.y"
    {
                      (yyval.nd) = new_for(p, (yyvsp[(2) - (9)].nd), (yyvsp[(5) - (9)].nd), (yyvsp[(8) - (9)].nd));
                    }
    break;

  case 306:
/* Line 1787 of yacc.c  */
#line 2169 "parse.y"
    {
                      (yyval.num) = p->lineno;
                    }
    break;

  case 307:
/* Line 1787 of yacc.c  */
#line 2173 "parse.y"
    {
                      if (p->in_def || p->in_single)
                        yyerror(p, "class definition in method body");
                      (yyval.nd) = local_switch(p);
                    }
    break;

  case 308:
/* Line 1787 of yacc.c  */
#line 2180 "parse.y"
    {
                      (yyval.nd) = new_class(p, (yyvsp[(3) - (7)].nd), (yyvsp[(4) - (7)].nd), (yyvsp[(6) - (7)].nd));
                      SET_LINENO((yyval.nd), (yyvsp[(2) - (7)].num));
                      local_resume(p, (yyvsp[(5) - (7)].nd));
                    }
    break;

  case 309:
/* Line 1787 of yacc.c  */
#line 2186 "parse.y"
    {
                      (yyval.num) = p->lineno;
                    }
    break;

  case 310:
/* Line 1787 of yacc.c  */
#line 2190 "parse.y"
    {
                      (yyval.num) = p->in_def;
                      p->in_def = 0;
                    }
    break;

  case 311:
/* Line 1787 of yacc.c  */
#line 2195 "parse.y"
    {
                      (yyval.nd) = cons(local_switch(p), (node*)(intptr_t)p->in_single);
                      p->in_single = 0;
                    }
    break;

  case 312:
/* Line 1787 of yacc.c  */
#line 2201 "parse.y"
    {
                      (yyval.nd) = new_sclass(p, (yyvsp[(4) - (9)].nd), (yyvsp[(8) - (9)].nd));
                      SET_LINENO((yyval.nd), (yyvsp[(2) - (9)].num));
                      local_resume(p, (yyvsp[(7) - (9)].nd)->car);
                      p->in_def = (yyvsp[(5) - (9)].num);
                      p->in_single = (int)(intptr_t)(yyvsp[(7) - (9)].nd)->cdr;
                    }
    break;

  case 313:
/* Line 1787 of yacc.c  */
#line 2209 "parse.y"
    {
                      (yyval.num) = p->lineno;
                    }
    break;

  case 314:
/* Line 1787 of yacc.c  */
#line 2213 "parse.y"
    {
                      if (p->in_def || p->in_single)
                        yyerror(p, "module definition in method body");
                      (yyval.nd) = local_switch(p);
                    }
    break;

  case 315:
/* Line 1787 of yacc.c  */
#line 2220 "parse.y"
    {
                      (yyval.nd) = new_module(p, (yyvsp[(3) - (6)].nd), (yyvsp[(5) - (6)].nd));
                      SET_LINENO((yyval.nd), (yyvsp[(2) - (6)].num));
                      local_resume(p, (yyvsp[(4) - (6)].nd));
                    }
    break;

  case 316:
/* Line 1787 of yacc.c  */
#line 2226 "parse.y"
    {
                      p->in_def++;
                      (yyval.nd) = local_switch(p);
                      (yyvsp[(1) - (2)].stack) = p->cmdarg_stack;
                      p->cmdarg_stack = 0;
                    }
    break;

  case 317:
/* Line 1787 of yacc.c  */
#line 2235 "parse.y"
    {
                      (yyval.nd) = new_def(p, (yyvsp[(2) - (6)].id), (yyvsp[(4) - (6)].nd), (yyvsp[(5) - (6)].nd));
                      local_resume(p, (yyvsp[(3) - (6)].nd));
                      p->in_def--;
                      p->cmdarg_stack = (yyvsp[(1) - (6)].stack);
                    }
    break;

  case 318:
/* Line 1787 of yacc.c  */
#line 2241 "parse.y"
    {p->lstate = EXPR_FNAME;}
    break;

  case 319:
/* Line 1787 of yacc.c  */
#line 2242 "parse.y"
    {
                      p->in_single++;
                      p->lstate = EXPR_ENDFN; /* force for args */
                      (yyval.nd) = local_switch(p);
                      (yyvsp[(1) - (5)].stack) = p->cmdarg_stack;
                      p->cmdarg_stack = 0;
                    }
    break;

  case 320:
/* Line 1787 of yacc.c  */
#line 2252 "parse.y"
    {
                      (yyval.nd) = new_sdef(p, (yyvsp[(2) - (9)].nd), (yyvsp[(5) - (9)].id), (yyvsp[(7) - (9)].nd), (yyvsp[(8) - (9)].nd));
                      local_resume(p, (yyvsp[(6) - (9)].nd));
                      p->in_single--;
                      p->cmdarg_stack = (yyvsp[(1) - (9)].stack);
                    }
    break;

  case 321:
/* Line 1787 of yacc.c  */
#line 2259 "parse.y"
    {
                      (yyval.nd) = new_break(p, 0);
                    }
    break;

  case 322:
/* Line 1787 of yacc.c  */
#line 2263 "parse.y"
    {
                      (yyval.nd) = new_next(p, 0);
                    }
    break;

  case 323:
/* Line 1787 of yacc.c  */
#line 2267 "parse.y"
    {
                      (yyval.nd) = new_redo(p);
                    }
    break;

  case 324:
/* Line 1787 of yacc.c  */
#line 2271 "parse.y"
    {
                      (yyval.nd) = new_retry(p);
                    }
    break;

  case 325:
/* Line 1787 of yacc.c  */
#line 2277 "parse.y"
    {
                      (yyval.nd) = (yyvsp[(1) - (1)].nd);
                      if (!(yyval.nd)) (yyval.nd) = new_nil(p);
                    }
    break;

  case 332:
/* Line 1787 of yacc.c  */
#line 2296 "parse.y"
    {
                      (yyval.nd) = new_if(p, cond((yyvsp[(2) - (5)].nd)), (yyvsp[(4) - (5)].nd), (yyvsp[(5) - (5)].nd));
                    }
    break;

  case 334:
/* Line 1787 of yacc.c  */
#line 2303 "parse.y"
    {
                      (yyval.nd) = (yyvsp[(2) - (2)].nd);
                    }
    break;

  case 335:
/* Line 1787 of yacc.c  */
#line 2309 "parse.y"
    {
                      (yyval.nd) = list1(list1((yyvsp[(1) - (1)].nd)));
                    }
    break;

  case 337:
/* Line 1787 of yacc.c  */
#line 2316 "parse.y"
    {
                      (yyval.nd) = new_arg(p, (yyvsp[(1) - (1)].id));
                    }
    break;

  case 338:
/* Line 1787 of yacc.c  */
#line 2320 "parse.y"
    {
                      (yyval.nd) = new_masgn(p, (yyvsp[(2) - (3)].nd), 0);
                    }
    break;

  case 339:
/* Line 1787 of yacc.c  */
#line 2326 "parse.y"
    {
                      (yyval.nd) = list1((yyvsp[(1) - (1)].nd));
                    }
    break;

  case 340:
/* Line 1787 of yacc.c  */
#line 2330 "parse.y"
    {
                      (yyval.nd) = push((yyvsp[(1) - (3)].nd), (yyvsp[(3) - (3)].nd));
                    }
    break;

  case 341:
/* Line 1787 of yacc.c  */
#line 2336 "parse.y"
    {
                      (yyval.nd) = list3((yyvsp[(1) - (1)].nd),0,0);
                    }
    break;

  case 342:
/* Line 1787 of yacc.c  */
#line 2340 "parse.y"
    {
                      (yyval.nd) = list3((yyvsp[(1) - (4)].nd), new_arg(p, (yyvsp[(4) - (4)].id)), 0);
                    }
    break;

  case 343:
/* Line 1787 of yacc.c  */
#line 2344 "parse.y"
    {
                      (yyval.nd) = list3((yyvsp[(1) - (6)].nd), new_arg(p, (yyvsp[(4) - (6)].id)), (yyvsp[(6) - (6)].nd));
                    }
    break;

  case 344:
/* Line 1787 of yacc.c  */
#line 2348 "parse.y"
    {
                      (yyval.nd) = list3((yyvsp[(1) - (3)].nd), (node*)-1, 0);
                    }
    break;

  case 345:
/* Line 1787 of yacc.c  */
#line 2352 "parse.y"
    {
                      (yyval.nd) = list3((yyvsp[(1) - (5)].nd), (node*)-1, (yyvsp[(5) - (5)].nd));
                    }
    break;

  case 346:
/* Line 1787 of yacc.c  */
#line 2356 "parse.y"
    {
                      (yyval.nd) = list3(0, new_arg(p, (yyvsp[(2) - (2)].id)), 0);
                    }
    break;

  case 347:
/* Line 1787 of yacc.c  */
#line 2360 "parse.y"
    {
                      (yyval.nd) = list3(0, new_arg(p, (yyvsp[(2) - (4)].id)), (yyvsp[(4) - (4)].nd));
                    }
    break;

  case 348:
/* Line 1787 of yacc.c  */
#line 2364 "parse.y"
    {
                      (yyval.nd) = list3(0, (node*)-1, 0);
                    }
    break;

  case 349:
/* Line 1787 of yacc.c  */
#line 2368 "parse.y"
    {
                      (yyval.nd) = list3(0, (node*)-1, (yyvsp[(3) - (3)].nd));
                    }
    break;

  case 350:
/* Line 1787 of yacc.c  */
#line 2374 "parse.y"
    {
                      (yyval.nd) = new_args(p, (yyvsp[(1) - (6)].nd), (yyvsp[(3) - (6)].nd), (yyvsp[(5) - (6)].id), 0, (yyvsp[(6) - (6)].id));
                    }
    break;

  case 351:
/* Line 1787 of yacc.c  */
#line 2378 "parse.y"
    {
                      (yyval.nd) = new_args(p, (yyvsp[(1) - (8)].nd), (yyvsp[(3) - (8)].nd), (yyvsp[(5) - (8)].id), (yyvsp[(7) - (8)].nd), (yyvsp[(8) - (8)].id));
                    }
    break;

  case 352:
/* Line 1787 of yacc.c  */
#line 2382 "parse.y"
    {
                      (yyval.nd) = new_args(p, (yyvsp[(1) - (4)].nd), (yyvsp[(3) - (4)].nd), 0, 0, (yyvsp[(4) - (4)].id));
                    }
    break;

  case 353:
/* Line 1787 of yacc.c  */
#line 2386 "parse.y"
    {
                      (yyval.nd) = new_args(p, (yyvsp[(1) - (6)].nd), (yyvsp[(3) - (6)].nd), 0, (yyvsp[(5) - (6)].nd), (yyvsp[(6) - (6)].id));
                    }
    break;

  case 354:
/* Line 1787 of yacc.c  */
#line 2390 "parse.y"
    {
                      (yyval.nd) = new_args(p, (yyvsp[(1) - (4)].nd), 0, (yyvsp[(3) - (4)].id), 0, (yyvsp[(4) - (4)].id));
                    }
    break;

  case 355:
/* Line 1787 of yacc.c  */
#line 2394 "parse.y"
    {
                      (yyval.nd) = new_args(p, (yyvsp[(1) - (2)].nd), 0, 1, 0, 0);
                    }
    break;

  case 356:
/* Line 1787 of yacc.c  */
#line 2398 "parse.y"
    {
                      (yyval.nd) = new_args(p, (yyvsp[(1) - (6)].nd), 0, (yyvsp[(3) - (6)].id), (yyvsp[(5) - (6)].nd), (yyvsp[(6) - (6)].id));
                    }
    break;

  case 357:
/* Line 1787 of yacc.c  */
#line 2402 "parse.y"
    {
                      (yyval.nd) = new_args(p, (yyvsp[(1) - (2)].nd), 0, 0, 0, (yyvsp[(2) - (2)].id));
                    }
    break;

  case 358:
/* Line 1787 of yacc.c  */
#line 2406 "parse.y"
    {
                      (yyval.nd) = new_args(p, 0, (yyvsp[(1) - (4)].nd), (yyvsp[(3) - (4)].id), 0, (yyvsp[(4) - (4)].id));
                    }
    break;

  case 359:
/* Line 1787 of yacc.c  */
#line 2410 "parse.y"
    {
                      (yyval.nd) = new_args(p, 0, (yyvsp[(1) - (6)].nd), (yyvsp[(3) - (6)].id), (yyvsp[(5) - (6)].nd), (yyvsp[(6) - (6)].id));
                    }
    break;

  case 360:
/* Line 1787 of yacc.c  */
#line 2414 "parse.y"
    {
                      (yyval.nd) = new_args(p, 0, (yyvsp[(1) - (2)].nd), 0, 0, (yyvsp[(2) - (2)].id));
                    }
    break;

  case 361:
/* Line 1787 of yacc.c  */
#line 2418 "parse.y"
    {
                      (yyval.nd) = new_args(p, 0, (yyvsp[(1) - (4)].nd), 0, (yyvsp[(3) - (4)].nd), (yyvsp[(4) - (4)].id));
                    }
    break;

  case 362:
/* Line 1787 of yacc.c  */
#line 2422 "parse.y"
    {
                      (yyval.nd) = new_args(p, 0, 0, (yyvsp[(1) - (2)].id), 0, (yyvsp[(2) - (2)].id));
                    }
    break;

  case 363:
/* Line 1787 of yacc.c  */
#line 2426 "parse.y"
    {
                      (yyval.nd) = new_args(p, 0, 0, (yyvsp[(1) - (4)].id), (yyvsp[(3) - (4)].nd), (yyvsp[(4) - (4)].id));
                    }
    break;

  case 364:
/* Line 1787 of yacc.c  */
#line 2430 "parse.y"
    {
                      (yyval.nd) = new_args(p, 0, 0, 0, 0, (yyvsp[(1) - (1)].id));
                    }
    break;

  case 366:
/* Line 1787 of yacc.c  */
#line 2437 "parse.y"
    {
                      p->cmd_start = TRUE;
                      (yyval.nd) = (yyvsp[(1) - (1)].nd);
                    }
    break;

  case 367:
/* Line 1787 of yacc.c  */
#line 2444 "parse.y"
    {
                      local_add_f(p, 0);
                      (yyval.nd) = 0;
                    }
    break;

  case 368:
/* Line 1787 of yacc.c  */
#line 2449 "parse.y"
    {
                      local_add_f(p, 0);
                      (yyval.nd) = 0;
                    }
    break;

  case 369:
/* Line 1787 of yacc.c  */
#line 2454 "parse.y"
    {
                      (yyval.nd) = (yyvsp[(2) - (4)].nd);
                    }
    break;

  case 370:
/* Line 1787 of yacc.c  */
#line 2461 "parse.y"
    {
                      (yyval.nd) = 0;
                    }
    break;

  case 371:
/* Line 1787 of yacc.c  */
#line 2465 "parse.y"
    {
                      (yyval.nd) = 0;
                    }
    break;

  case 374:
/* Line 1787 of yacc.c  */
#line 2475 "parse.y"
    {
                      local_add_f(p, (yyvsp[(1) - (1)].id));
                      new_bv(p, (yyvsp[(1) - (1)].id));
                    }
    break;

  case 376:
/* Line 1787 of yacc.c  */
#line 2483 "parse.y"
    {
                      (yyval.nd) = (yyvsp[(2) - (4)].nd);
                    }
    break;

  case 377:
/* Line 1787 of yacc.c  */
#line 2487 "parse.y"
    {
                      (yyval.nd) = (yyvsp[(1) - (1)].nd);
                    }
    break;

  case 378:
/* Line 1787 of yacc.c  */
#line 2493 "parse.y"
    {
                      (yyval.nd) = (yyvsp[(2) - (3)].nd);
                    }
    break;

  case 379:
/* Line 1787 of yacc.c  */
#line 2497 "parse.y"
    {
                      (yyval.nd) = (yyvsp[(2) - (3)].nd);
                    }
    break;

  case 380:
/* Line 1787 of yacc.c  */
#line 2503 "parse.y"
    {
                      local_nest(p);
                    }
    break;

  case 381:
/* Line 1787 of yacc.c  */
#line 2509 "parse.y"
    {
                      (yyval.nd) = new_block(p,(yyvsp[(3) - (5)].nd),(yyvsp[(4) - (5)].nd));
                      local_unnest(p);
                    }
    break;

  case 382:
/* Line 1787 of yacc.c  */
#line 2516 "parse.y"
    {
                      if ((yyvsp[(1) - (2)].nd)->car == (node*)NODE_YIELD) {
                        yyerror(p, "block given to yield");
                      }
                      else {
                        call_with_block(p, (yyvsp[(1) - (2)].nd), (yyvsp[(2) - (2)].nd));
                      }
                      (yyval.nd) = (yyvsp[(1) - (2)].nd);
                    }
    break;

  case 383:
/* Line 1787 of yacc.c  */
#line 2526 "parse.y"
    {
                      (yyval.nd) = new_call(p, (yyvsp[(1) - (4)].nd), (yyvsp[(3) - (4)].id), (yyvsp[(4) - (4)].nd));
                    }
    break;

  case 384:
/* Line 1787 of yacc.c  */
#line 2530 "parse.y"
    {
                      (yyval.nd) = new_call(p, (yyvsp[(1) - (5)].nd), (yyvsp[(3) - (5)].id), (yyvsp[(4) - (5)].nd));
                      call_with_block(p, (yyval.nd), (yyvsp[(5) - (5)].nd));
                    }
    break;

  case 385:
/* Line 1787 of yacc.c  */
#line 2535 "parse.y"
    {
                      (yyval.nd) = new_call(p, (yyvsp[(1) - (5)].nd), (yyvsp[(3) - (5)].id), (yyvsp[(4) - (5)].nd));
                      call_with_block(p, (yyval.nd), (yyvsp[(5) - (5)].nd));
                    }
    break;

  case 386:
/* Line 1787 of yacc.c  */
#line 2542 "parse.y"
    {
                      (yyval.nd) = new_fcall(p, (yyvsp[(1) - (2)].id), (yyvsp[(2) - (2)].nd));
                    }
    break;

  case 387:
/* Line 1787 of yacc.c  */
#line 2546 "parse.y"
    {
                      (yyval.nd) = new_call(p, (yyvsp[(1) - (4)].nd), (yyvsp[(3) - (4)].id), (yyvsp[(4) - (4)].nd));
                    }
    break;

  case 388:
/* Line 1787 of yacc.c  */
#line 2550 "parse.y"
    {
                      (yyval.nd) = new_call(p, (yyvsp[(1) - (4)].nd), (yyvsp[(3) - (4)].id), (yyvsp[(4) - (4)].nd));
                    }
    break;

  case 389:
/* Line 1787 of yacc.c  */
#line 2554 "parse.y"
    {
                      (yyval.nd) = new_call(p, (yyvsp[(1) - (3)].nd), (yyvsp[(3) - (3)].id), 0);
                    }
    break;

  case 390:
/* Line 1787 of yacc.c  */
#line 2558 "parse.y"
    {
                      (yyval.nd) = new_call(p, (yyvsp[(1) - (3)].nd), intern("call",4), (yyvsp[(3) - (3)].nd));
                    }
    break;

  case 391:
/* Line 1787 of yacc.c  */
#line 2562 "parse.y"
    {
                      (yyval.nd) = new_call(p, (yyvsp[(1) - (3)].nd), intern("call",4), (yyvsp[(3) - (3)].nd));
                    }
    break;

  case 392:
/* Line 1787 of yacc.c  */
#line 2566 "parse.y"
    {
                      (yyval.nd) = new_super(p, (yyvsp[(2) - (2)].nd));
                    }
    break;

  case 393:
/* Line 1787 of yacc.c  */
#line 2570 "parse.y"
    {
                      (yyval.nd) = new_zsuper(p);
                    }
    break;

  case 394:
/* Line 1787 of yacc.c  */
#line 2574 "parse.y"
    {
                      (yyval.nd) = new_call(p, (yyvsp[(1) - (4)].nd), intern("[]",2), (yyvsp[(3) - (4)].nd));
                    }
    break;

  case 395:
/* Line 1787 of yacc.c  */
#line 2580 "parse.y"
    {
                      local_nest(p);
                      (yyval.num) = p->lineno;
                    }
    break;

  case 396:
/* Line 1787 of yacc.c  */
#line 2586 "parse.y"
    {
                      (yyval.nd) = new_block(p,(yyvsp[(3) - (5)].nd),(yyvsp[(4) - (5)].nd));
                      SET_LINENO((yyval.nd), (yyvsp[(2) - (5)].num));
                      local_unnest(p);
                    }
    break;

  case 397:
/* Line 1787 of yacc.c  */
#line 2592 "parse.y"
    {
                      local_nest(p);
                      (yyval.num) = p->lineno;
                    }
    break;

  case 398:
/* Line 1787 of yacc.c  */
#line 2598 "parse.y"
    {
                      (yyval.nd) = new_block(p,(yyvsp[(3) - (5)].nd),(yyvsp[(4) - (5)].nd));
                      SET_LINENO((yyval.nd), (yyvsp[(2) - (5)].num));
                      local_unnest(p);
                    }
    break;

  case 399:
/* Line 1787 of yacc.c  */
#line 2608 "parse.y"
    {
                      (yyval.nd) = cons(cons((yyvsp[(2) - (5)].nd), (yyvsp[(4) - (5)].nd)), (yyvsp[(5) - (5)].nd));
                    }
    break;

  case 400:
/* Line 1787 of yacc.c  */
#line 2614 "parse.y"
    {
                      if ((yyvsp[(1) - (1)].nd)) {
                        (yyval.nd) = cons(cons(0, (yyvsp[(1) - (1)].nd)), 0);
                      }
                      else {
                        (yyval.nd) = 0;
                      }
                    }
    break;

  case 402:
/* Line 1787 of yacc.c  */
#line 2628 "parse.y"
    {
                      (yyval.nd) = list1(list3((yyvsp[(2) - (6)].nd), (yyvsp[(3) - (6)].nd), (yyvsp[(5) - (6)].nd)));
                      if ((yyvsp[(6) - (6)].nd)) (yyval.nd) = append((yyval.nd), (yyvsp[(6) - (6)].nd));
                    }
    break;

  case 404:
/* Line 1787 of yacc.c  */
#line 2636 "parse.y"
    {
                        (yyval.nd) = list1((yyvsp[(1) - (1)].nd));
                    }
    break;

  case 407:
/* Line 1787 of yacc.c  */
#line 2644 "parse.y"
    {
                      (yyval.nd) = (yyvsp[(2) - (2)].nd);
                    }
    break;

  case 409:
/* Line 1787 of yacc.c  */
#line 2651 "parse.y"
    {
                      (yyval.nd) = (yyvsp[(2) - (2)].nd);
                    }
    break;

  case 417:
/* Line 1787 of yacc.c  */
#line 2666 "parse.y"
    {
                      (yyval.nd) = (yyvsp[(2) - (2)].nd);
                    }
    break;

  case 418:
/* Line 1787 of yacc.c  */
#line 2670 "parse.y"
    {
                      (yyval.nd) = new_dstr(p, push((yyvsp[(2) - (3)].nd), (yyvsp[(3) - (3)].nd)));
                    }
    break;

  case 420:
/* Line 1787 of yacc.c  */
#line 2677 "parse.y"
    {
                      (yyval.nd) = append((yyvsp[(1) - (2)].nd), (yyvsp[(2) - (2)].nd));
                    }
    break;

  case 421:
/* Line 1787 of yacc.c  */
#line 2683 "parse.y"
    {
                      (yyval.nd) = list1((yyvsp[(1) - (1)].nd));
                    }
    break;

  case 422:
/* Line 1787 of yacc.c  */
#line 2687 "parse.y"
    {
                      (yyval.nd) = p->lex_strterm;
                      p->lex_strterm = NULL;
                    }
    break;

  case 423:
/* Line 1787 of yacc.c  */
#line 2693 "parse.y"
    {
                      p->lex_strterm = (yyvsp[(2) - (4)].nd);
                      (yyval.nd) = list2((yyvsp[(1) - (4)].nd), (yyvsp[(3) - (4)].nd));
                    }
    break;

  case 424:
/* Line 1787 of yacc.c  */
#line 2698 "parse.y"
    {
                      (yyval.nd) = list1(new_literal_delim(p));
                    }
    break;

  case 425:
/* Line 1787 of yacc.c  */
#line 2702 "parse.y"
    {
                      (yyval.nd) = list1(new_literal_delim(p));
                    }
    break;

  case 426:
/* Line 1787 of yacc.c  */
#line 2708 "parse.y"
    {
                        (yyval.nd) = (yyvsp[(2) - (2)].nd);
                    }
    break;

  case 427:
/* Line 1787 of yacc.c  */
#line 2712 "parse.y"
    {
                      (yyval.nd) = new_dxstr(p, push((yyvsp[(2) - (3)].nd), (yyvsp[(3) - (3)].nd)));
                    }
    break;

  case 428:
/* Line 1787 of yacc.c  */
#line 2718 "parse.y"
    {
                        (yyval.nd) = (yyvsp[(2) - (2)].nd);
                    }
    break;

  case 429:
/* Line 1787 of yacc.c  */
#line 2722 "parse.y"
    {
                      (yyval.nd) = new_dregx(p, (yyvsp[(2) - (3)].nd), (yyvsp[(3) - (3)].nd));
                    }
    break;

  case 435:
/* Line 1787 of yacc.c  */
#line 2739 "parse.y"
    {
                      parser_heredoc_info * inf = parsing_heredoc_inf(p);
                      inf->doc = push(inf->doc, new_str(p, "", 0));
                      heredoc_end(p);
                    }
    break;

  case 436:
/* Line 1787 of yacc.c  */
#line 2745 "parse.y"
    {
                      heredoc_end(p);
                    }
    break;

  case 439:
/* Line 1787 of yacc.c  */
#line 2755 "parse.y"
    {
                      parser_heredoc_info * inf = parsing_heredoc_inf(p);
                      inf->doc = push(inf->doc, (yyvsp[(1) - (1)].nd));
                      heredoc_treat_nextline(p);
                    }
    break;

  case 440:
/* Line 1787 of yacc.c  */
#line 2761 "parse.y"
    {
                      (yyval.nd) = p->lex_strterm;
                      p->lex_strterm = NULL;
                    }
    break;

  case 441:
/* Line 1787 of yacc.c  */
#line 2767 "parse.y"
    {
                      parser_heredoc_info * inf = parsing_heredoc_inf(p);
                      p->lex_strterm = (yyvsp[(2) - (4)].nd);
                      inf->doc = push(push(inf->doc, (yyvsp[(1) - (4)].nd)), (yyvsp[(3) - (4)].nd));
                    }
    break;

  case 442:
/* Line 1787 of yacc.c  */
#line 2775 "parse.y"
    {
                      (yyval.nd) = new_words(p, list1((yyvsp[(2) - (2)].nd)));
                    }
    break;

  case 443:
/* Line 1787 of yacc.c  */
#line 2779 "parse.y"
    {
                      (yyval.nd) = new_words(p, push((yyvsp[(2) - (3)].nd), (yyvsp[(3) - (3)].nd)));
                    }
    break;

  case 444:
/* Line 1787 of yacc.c  */
#line 2786 "parse.y"
    {
                      (yyval.nd) = new_sym(p, (yyvsp[(1) - (1)].id));
                    }
    break;

  case 445:
/* Line 1787 of yacc.c  */
#line 2790 "parse.y"
    {
                      p->lstate = EXPR_END;
                      (yyval.nd) = new_dsym(p, push((yyvsp[(3) - (4)].nd), (yyvsp[(4) - (4)].nd)));
                    }
    break;

  case 446:
/* Line 1787 of yacc.c  */
#line 2797 "parse.y"
    {
                      p->lstate = EXPR_END;
                      (yyval.id) = (yyvsp[(2) - (2)].id);
                    }
    break;

  case 451:
/* Line 1787 of yacc.c  */
#line 2808 "parse.y"
    {
                      (yyval.id) = new_strsym(p, (yyvsp[(1) - (1)].nd));
                    }
    break;

  case 452:
/* Line 1787 of yacc.c  */
#line 2812 "parse.y"
    {
                      (yyval.id) = new_strsym(p, (yyvsp[(2) - (2)].nd));
                    }
    break;

  case 453:
/* Line 1787 of yacc.c  */
#line 2818 "parse.y"
    {
                      (yyval.nd) = new_symbols(p, list1((yyvsp[(2) - (2)].nd)));
                    }
    break;

  case 454:
/* Line 1787 of yacc.c  */
#line 2822 "parse.y"
    {
                      (yyval.nd) = new_symbols(p, push((yyvsp[(2) - (3)].nd), (yyvsp[(3) - (3)].nd)));
                    }
    break;

  case 457:
/* Line 1787 of yacc.c  */
#line 2830 "parse.y"
    {
                      (yyval.nd) = negate_lit(p, (yyvsp[(2) - (2)].nd));
                    }
    break;

  case 458:
/* Line 1787 of yacc.c  */
#line 2834 "parse.y"
    {
                      (yyval.nd) = negate_lit(p, (yyvsp[(2) - (2)].nd));
                    }
    break;

  case 459:
/* Line 1787 of yacc.c  */
#line 2840 "parse.y"
    {
                      (yyval.nd) = new_lvar(p, (yyvsp[(1) - (1)].id));
                    }
    break;

  case 460:
/* Line 1787 of yacc.c  */
#line 2844 "parse.y"
    {
                      (yyval.nd) = new_ivar(p, (yyvsp[(1) - (1)].id));
                    }
    break;

  case 461:
/* Line 1787 of yacc.c  */
#line 2848 "parse.y"
    {
                      (yyval.nd) = new_gvar(p, (yyvsp[(1) - (1)].id));
                    }
    break;

  case 462:
/* Line 1787 of yacc.c  */
#line 2852 "parse.y"
    {
                      (yyval.nd) = new_cvar(p, (yyvsp[(1) - (1)].id));
                    }
    break;

  case 463:
/* Line 1787 of yacc.c  */
#line 2856 "parse.y"
    {
                      (yyval.nd) = new_const(p, (yyvsp[(1) - (1)].id));
                    }
    break;

  case 464:
/* Line 1787 of yacc.c  */
#line 2862 "parse.y"
    {
                      assignable(p, (yyvsp[(1) - (1)].nd));
                    }
    break;

  case 465:
/* Line 1787 of yacc.c  */
#line 2868 "parse.y"
    {
                      (yyval.nd) = var_reference(p, (yyvsp[(1) - (1)].nd));
                    }
    break;

  case 466:
/* Line 1787 of yacc.c  */
#line 2872 "parse.y"
    {
                      (yyval.nd) = new_nil(p);
                    }
    break;

  case 467:
/* Line 1787 of yacc.c  */
#line 2876 "parse.y"
    {
                      (yyval.nd) = new_self(p);
                       }
    break;

  case 468:
/* Line 1787 of yacc.c  */
#line 2880 "parse.y"
    {
                      (yyval.nd) = new_true(p);
                       }
    break;

  case 469:
/* Line 1787 of yacc.c  */
#line 2884 "parse.y"
    {
                      (yyval.nd) = new_false(p);
                       }
    break;

  case 470:
/* Line 1787 of yacc.c  */
#line 2888 "parse.y"
    {
                      if (!p->filename) {
                        p->filename = "(null)";
                      }
                      (yyval.nd) = new_str(p, p->filename, strlen(p->filename));
                    }
    break;

  case 471:
/* Line 1787 of yacc.c  */
#line 2895 "parse.y"
    {
                      char buf[16];

                      snprintf(buf, sizeof(buf), "%d", p->lineno);
                      (yyval.nd) = new_int(p, buf, 10);
                    }
    break;

  case 474:
/* Line 1787 of yacc.c  */
#line 2908 "parse.y"
    {
                      (yyval.nd) = 0;
                    }
    break;

  case 475:
/* Line 1787 of yacc.c  */
#line 2912 "parse.y"
    {
                      p->lstate = EXPR_BEG;
                      p->cmd_start = TRUE;
                    }
    break;

  case 476:
/* Line 1787 of yacc.c  */
#line 2917 "parse.y"
    {
                      (yyval.nd) = (yyvsp[(3) - (4)].nd);
                    }
    break;

  case 477:
/* Line 1787 of yacc.c  */
#line 2921 "parse.y"
    {
                      yyerrok;
                      (yyval.nd) = 0;
                    }
    break;

  case 478:
/* Line 1787 of yacc.c  */
#line 2928 "parse.y"
    {
                      (yyval.nd) = (yyvsp[(2) - (3)].nd);
                      p->lstate = EXPR_BEG;
                      p->cmd_start = TRUE;
                    }
    break;

  case 479:
/* Line 1787 of yacc.c  */
#line 2934 "parse.y"
    {
                      (yyval.nd) = (yyvsp[(1) - (2)].nd);
                    }
    break;

  case 480:
/* Line 1787 of yacc.c  */
#line 2940 "parse.y"
    {
                      (yyval.nd) = new_args(p, (yyvsp[(1) - (6)].nd), (yyvsp[(3) - (6)].nd), (yyvsp[(5) - (6)].id), 0, (yyvsp[(6) - (6)].id));
                    }
    break;

  case 481:
/* Line 1787 of yacc.c  */
#line 2944 "parse.y"
    {
                      (yyval.nd) = new_args(p, (yyvsp[(1) - (8)].nd), (yyvsp[(3) - (8)].nd), (yyvsp[(5) - (8)].id), (yyvsp[(7) - (8)].nd), (yyvsp[(8) - (8)].id));
                    }
    break;

  case 482:
/* Line 1787 of yacc.c  */
#line 2948 "parse.y"
    {
                      (yyval.nd) = new_args(p, (yyvsp[(1) - (4)].nd), (yyvsp[(3) - (4)].nd), 0, 0, (yyvsp[(4) - (4)].id));
                    }
    break;

  case 483:
/* Line 1787 of yacc.c  */
#line 2952 "parse.y"
    {
                      (yyval.nd) = new_args(p, (yyvsp[(1) - (6)].nd), (yyvsp[(3) - (6)].nd), 0, (yyvsp[(5) - (6)].nd), (yyvsp[(6) - (6)].id));
                    }
    break;

  case 484:
/* Line 1787 of yacc.c  */
#line 2956 "parse.y"
    {
                      (yyval.nd) = new_args(p, (yyvsp[(1) - (4)].nd), 0, (yyvsp[(3) - (4)].id), 0, (yyvsp[(4) - (4)].id));
                    }
    break;

  case 485:
/* Line 1787 of yacc.c  */
#line 2960 "parse.y"
    {
                      (yyval.nd) = new_args(p, (yyvsp[(1) - (6)].nd), 0, (yyvsp[(3) - (6)].id), (yyvsp[(5) - (6)].nd), (yyvsp[(6) - (6)].id));
                    }
    break;

  case 486:
/* Line 1787 of yacc.c  */
#line 2964 "parse.y"
    {
                      (yyval.nd) = new_args(p, (yyvsp[(1) - (2)].nd), 0, 0, 0, (yyvsp[(2) - (2)].id));
                    }
    break;

  case 487:
/* Line 1787 of yacc.c  */
#line 2968 "parse.y"
    {
                      (yyval.nd) = new_args(p, 0, (yyvsp[(1) - (4)].nd), (yyvsp[(3) - (4)].id), 0, (yyvsp[(4) - (4)].id));
                    }
    break;

  case 488:
/* Line 1787 of yacc.c  */
#line 2972 "parse.y"
    {
                      (yyval.nd) = new_args(p, 0, (yyvsp[(1) - (6)].nd), (yyvsp[(3) - (6)].id), (yyvsp[(5) - (6)].nd), (yyvsp[(6) - (6)].id));
                    }
    break;

  case 489:
/* Line 1787 of yacc.c  */
#line 2976 "parse.y"
    {
                      (yyval.nd) = new_args(p, 0, (yyvsp[(1) - (2)].nd), 0, 0, (yyvsp[(2) - (2)].id));
                    }
    break;

  case 490:
/* Line 1787 of yacc.c  */
#line 2980 "parse.y"
    {
                      (yyval.nd) = new_args(p, 0, (yyvsp[(1) - (4)].nd), 0, (yyvsp[(3) - (4)].nd), (yyvsp[(4) - (4)].id));
                    }
    break;

  case 491:
/* Line 1787 of yacc.c  */
#line 2984 "parse.y"
    {
                      (yyval.nd) = new_args(p, 0, 0, (yyvsp[(1) - (2)].id), 0, (yyvsp[(2) - (2)].id));
                    }
    break;

  case 492:
/* Line 1787 of yacc.c  */
#line 2988 "parse.y"
    {
                      (yyval.nd) = new_args(p, 0, 0, (yyvsp[(1) - (4)].id), (yyvsp[(3) - (4)].nd), (yyvsp[(4) - (4)].id));
                    }
    break;

  case 493:
/* Line 1787 of yacc.c  */
#line 2992 "parse.y"
    {
                      (yyval.nd) = new_args(p, 0, 0, 0, 0, (yyvsp[(1) - (1)].id));
                    }
    break;

  case 494:
/* Line 1787 of yacc.c  */
#line 2996 "parse.y"
    {
                      local_add_f(p, 0);
                      (yyval.nd) = new_args(p, 0, 0, 0, 0, 0);
                    }
    break;

  case 495:
/* Line 1787 of yacc.c  */
#line 3003 "parse.y"
    {
                      yyerror(p, "formal argument cannot be a constant");
                      (yyval.nd) = 0;
                    }
    break;

  case 496:
/* Line 1787 of yacc.c  */
#line 3008 "parse.y"
    {
                      yyerror(p, "formal argument cannot be an instance variable");
                      (yyval.nd) = 0;
                    }
    break;

  case 497:
/* Line 1787 of yacc.c  */
#line 3013 "parse.y"
    {
                      yyerror(p, "formal argument cannot be a global variable");
                      (yyval.nd) = 0;
                    }
    break;

  case 498:
/* Line 1787 of yacc.c  */
#line 3018 "parse.y"
    {
                      yyerror(p, "formal argument cannot be a class variable");
                      (yyval.nd) = 0;
                    }
    break;

  case 499:
/* Line 1787 of yacc.c  */
#line 3025 "parse.y"
    {
                      (yyval.id) = 0;
                    }
    break;

  case 500:
/* Line 1787 of yacc.c  */
#line 3029 "parse.y"
    {
                      local_add_f(p, (yyvsp[(1) - (1)].id));
                      (yyval.id) = (yyvsp[(1) - (1)].id);
                    }
    break;

  case 501:
/* Line 1787 of yacc.c  */
#line 3036 "parse.y"
    {
                      (yyval.nd) = new_arg(p, (yyvsp[(1) - (1)].id));
                    }
    break;

  case 502:
/* Line 1787 of yacc.c  */
#line 3040 "parse.y"
    {
                      (yyval.nd) = new_masgn(p, (yyvsp[(2) - (3)].nd), 0);
                    }
    break;

  case 503:
/* Line 1787 of yacc.c  */
#line 3046 "parse.y"
    {
                      (yyval.nd) = list1((yyvsp[(1) - (1)].nd));
                    }
    break;

  case 504:
/* Line 1787 of yacc.c  */
#line 3050 "parse.y"
    {
                      (yyval.nd) = push((yyvsp[(1) - (3)].nd), (yyvsp[(3) - (3)].nd));
                    }
    break;

  case 505:
/* Line 1787 of yacc.c  */
#line 3056 "parse.y"
    {
                      local_add_f(p, (yyvsp[(1) - (2)].id));
                      (yyval.id) = (yyvsp[(1) - (2)].id);
                    }
    break;

  case 506:
/* Line 1787 of yacc.c  */
#line 3063 "parse.y"
    {
                      (yyval.nd) = cons(nsym((yyvsp[(1) - (2)].id)), (yyvsp[(2) - (2)].nd));
                    }
    break;

  case 507:
/* Line 1787 of yacc.c  */
#line 3069 "parse.y"
    {
                      (yyval.nd) = cons(nsym((yyvsp[(1) - (2)].id)), (yyvsp[(2) - (2)].nd));
                    }
    break;

  case 508:
/* Line 1787 of yacc.c  */
#line 3075 "parse.y"
    {
                      (yyval.nd) = list1((yyvsp[(1) - (1)].nd));
                    }
    break;

  case 509:
/* Line 1787 of yacc.c  */
#line 3079 "parse.y"
    {
                      (yyval.nd) = push((yyvsp[(1) - (3)].nd), (yyvsp[(3) - (3)].nd));
                    }
    break;

  case 510:
/* Line 1787 of yacc.c  */
#line 3085 "parse.y"
    {
                      (yyval.nd) = list1((yyvsp[(1) - (1)].nd));
                    }
    break;

  case 511:
/* Line 1787 of yacc.c  */
#line 3089 "parse.y"
    {
                      (yyval.nd) = push((yyvsp[(1) - (3)].nd), (yyvsp[(3) - (3)].nd));
                    }
    break;

  case 514:
/* Line 1787 of yacc.c  */
#line 3099 "parse.y"
    {
                      local_add_f(p, (yyvsp[(2) - (2)].id));
                      (yyval.id) = (yyvsp[(2) - (2)].id);
                    }
    break;

  case 515:
/* Line 1787 of yacc.c  */
#line 3104 "parse.y"
    {
                      local_add_f(p, 0);
                      (yyval.id) = -1;
                    }
    break;

  case 518:
/* Line 1787 of yacc.c  */
#line 3115 "parse.y"
    {
                      local_add_f(p, (yyvsp[(2) - (2)].id));
                      (yyval.id) = (yyvsp[(2) - (2)].id);
                    }
    break;

  case 519:
/* Line 1787 of yacc.c  */
#line 3122 "parse.y"
    {
                      (yyval.id) = (yyvsp[(2) - (2)].id);
                    }
    break;

  case 520:
/* Line 1787 of yacc.c  */
#line 3126 "parse.y"
    {
                      local_add_f(p, 0);
                      (yyval.id) = 0;
                    }
    break;

  case 521:
/* Line 1787 of yacc.c  */
#line 3133 "parse.y"
    {
                      (yyval.nd) = (yyvsp[(1) - (1)].nd);
                      if (!(yyval.nd)) (yyval.nd) = new_nil(p);
                    }
    break;

  case 522:
/* Line 1787 of yacc.c  */
#line 3137 "parse.y"
    {p->lstate = EXPR_BEG;}
    break;

  case 523:
/* Line 1787 of yacc.c  */
#line 3138 "parse.y"
    {
                      if ((yyvsp[(3) - (4)].nd) == 0) {
                        yyerror(p, "can't define singleton method for ().");
                      }
                      else {
                        switch ((enum node_type)(int)(intptr_t)(yyvsp[(3) - (4)].nd)->car) {
                        case NODE_STR:
                        case NODE_DSTR:
                        case NODE_XSTR:
                        case NODE_DXSTR:
                        case NODE_DREGX:
                        case NODE_MATCH:
                        case NODE_FLOAT:
                        case NODE_ARRAY:
                        case NODE_HEREDOC:
                          yyerror(p, "can't define singleton method for literals");
                        default:
                          break;
                        }
                      }
                      (yyval.nd) = (yyvsp[(3) - (4)].nd);
                    }
    break;

  case 525:
/* Line 1787 of yacc.c  */
#line 3164 "parse.y"
    {
                      (yyval.nd) = (yyvsp[(1) - (2)].nd);
                    }
    break;

  case 526:
/* Line 1787 of yacc.c  */
#line 3170 "parse.y"
    {
                      (yyval.nd) = list1((yyvsp[(1) - (1)].nd));
                    }
    break;

  case 527:
/* Line 1787 of yacc.c  */
#line 3174 "parse.y"
    {
                      (yyval.nd) = push((yyvsp[(1) - (3)].nd), (yyvsp[(3) - (3)].nd));
                    }
    break;

  case 528:
/* Line 1787 of yacc.c  */
#line 3180 "parse.y"
    {
                      (yyval.nd) = cons((yyvsp[(1) - (3)].nd), (yyvsp[(3) - (3)].nd));
                    }
    break;

  case 529:
/* Line 1787 of yacc.c  */
#line 3184 "parse.y"
    {
                      (yyval.nd) = cons(new_sym(p, (yyvsp[(1) - (2)].id)), (yyvsp[(2) - (2)].nd));
                    }
    break;

  case 551:
/* Line 1787 of yacc.c  */
#line 3228 "parse.y"
    {yyerrok;}
    break;

  case 553:
/* Line 1787 of yacc.c  */
#line 3233 "parse.y"
    {
                      p->lineno++;
                      p->column = 0;
                    }
    break;

  case 556:
/* Line 1787 of yacc.c  */
#line 3240 "parse.y"
    {yyerrok;}
    break;

  case 557:
/* Line 1787 of yacc.c  */
#line 3244 "parse.y"
    {
                      (yyval.nd) = 0;
                    }
    break;


/* Line 1787 of yacc.c  */
#line 9049 "y.tab.c"
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (p, YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (p, yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
   error, discard it.  */

      if (yychar <= YYEOF)
  {
    /* Return failure if at end of input.  */
    if (yychar == YYEOF)
      YYABORT;
  }
      else
  {
    yydestruct ("Error: discarding",
          yytoken, &yylval, p);
    yychar = YYEMPTY;
  }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
  {
    yyn += YYTERROR;
    if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
      {
        yyn = yytable[yyn];
        if (0 < yyn)
    break;
      }
  }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
  YYABORT;


      yydestruct ("Error: popping",
      yystos[yystate], yyvsp, p);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (p, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, p);
    }
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
      yystos[*yyssp], yyvsp, p);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}


/* Line 2050 of yacc.c  */
#line 3248 "parse.y"

#define yylval  (*((YYSTYPE*)(p->ylval)))

static void
yyerror(parser_state *p, const char *s)
{
  char* c;
  int n;

  if (! p->capture_errors) {
#ifdef ENABLE_STDIO
    if (p->filename) {
      fprintf(stderr, "%s:%d:%d: %s\n", p->filename, p->lineno, p->column, s);
    }
    else {
      fprintf(stderr, "line %d:%d: %s\n", p->lineno, p->column, s);
    }
#endif
  }
  else if (p->nerr < sizeof(p->error_buffer) / sizeof(p->error_buffer[0])) {
    n = strlen(s);
    c = (char *)parser_palloc(p, n + 1);
    memcpy(c, s, n + 1);
    p->error_buffer[p->nerr].message = c;
    p->error_buffer[p->nerr].lineno = p->lineno;
    p->error_buffer[p->nerr].column = p->column;
  }
  p->nerr++;
}

static void
yyerror_i(parser_state *p, const char *fmt, int i)
{
  char buf[256];

  snprintf(buf, sizeof(buf), fmt, i);
  yyerror(p, buf);
}

static void
yywarn(parser_state *p, const char *s)
{
  char* c;
  int n;

  if (! p->capture_errors) {
#ifdef ENABLE_STDIO
    if (p->filename) {
      fprintf(stderr, "%s:%d:%d: %s\n", p->filename, p->lineno, p->column, s);
    }
    else {
      fprintf(stderr, "line %d:%d: %s\n", p->lineno, p->column, s);
    }
#endif
  }
  else if (p->nwarn < sizeof(p->warn_buffer) / sizeof(p->warn_buffer[0])) {
    n = strlen(s);
    c = (char *)parser_palloc(p, n + 1);
    memcpy(c, s, n + 1);
    p->warn_buffer[p->nwarn].message = c;
    p->warn_buffer[p->nwarn].lineno = p->lineno;
    p->warn_buffer[p->nwarn].column = p->column;
  }
  p->nwarn++;
}

static void
yywarning(parser_state *p, const char *s)
{
  yywarn(p, s);
}

static void
yywarning_s(parser_state *p, const char *fmt, const char *s)
{
  char buf[256];

  snprintf(buf, sizeof(buf), fmt, s);
  yywarning(p, buf);
}

static void
backref_error(parser_state *p, node *n)
{
  int c;

  c = (int)(intptr_t)n->car;

  if (c == NODE_NTH_REF) {
    yyerror_i(p, "can't set variable $%d", (int)(intptr_t)n->cdr);
  }
  else if (c == NODE_BACK_REF) {
    yyerror_i(p, "can't set variable $%c", (int)(intptr_t)n->cdr);
  }
  else {
    mrb_bug(p->mrb, "Internal error in backref_error() : n=>car == %S", mrb_fixnum_value(c));
  }
}

static void pushback(parser_state *p, int c);
static mrb_bool peeks(parser_state *p, const char *s);
static mrb_bool skips(parser_state *p, const char *s);

static inline int
nextc(parser_state *p)
{
  int c;

  if (p->pb) {
    node *tmp;

    c = (int)(intptr_t)p->pb->car;
    tmp = p->pb;
    p->pb = p->pb->cdr;
    cons_free(tmp);
  }
  else {
#ifdef ENABLE_STDIO
    if (p->f) {
      if (feof(p->f)) goto eof;
      c = fgetc(p->f);
      if (c == EOF) goto eof;
    }
    else
#endif
      if (!p->s || p->s >= p->send) {
        goto eof;
      }
      else {
        c = (unsigned char)*p->s++;
      }
  }
  p->column++;
  if (c == '\r') {
    c = nextc(p);
    if (c != '\n') {
      pushback(p, c);
      return '\r';
    }
    return c;
  }
  return c;

  eof:
  if (!p->cxt) return -1;
  else {
    if (p->cxt->partial_hook(p) < 0)
      return -1;
    return -2;
  }
}

static void
pushback(parser_state *p, int c)
{
  if (c < 0) return;
  p->column--;
  p->pb = cons((node*)(intptr_t)c, p->pb);
}

static void
skip(parser_state *p, char term)
{
  int c;

  for (;;) {
    c = nextc(p);
    if (c < 0) break;
    if (c == term) break;
  }
}

static int
peekc_n(parser_state *p, int n)
{
  node *list = 0;
  int c0;

  do {
    c0 = nextc(p);
    if (c0 < 0) return c0;
    list = push(list, (node*)(intptr_t)c0);
  } while(n--);
  if (p->pb) {
    p->pb = append((node*)list, p->pb);
  }
  else {
    p->pb = list;
  }
  return c0;
}

static mrb_bool
peek_n(parser_state *p, int c, int n)
{
  return peekc_n(p, n) == c && c >= 0;
}
#define peek(p,c) peek_n((p), (c), 0)

static mrb_bool
peeks(parser_state *p, const char *s)
{
  int len = strlen(s);

#ifdef ENABLE_STDIO
  if (p->f) {
    int n = 0;
    while (*s) {
      if (!peek_n(p, *s++, n++)) return FALSE;
    }
    return TRUE;
  }
  else
#endif
    if (p->s && p->s + len <= p->send) {
      if (memcmp(p->s, s, len) == 0) return TRUE;
    }
  return FALSE;
}

static mrb_bool
skips(parser_state *p, const char *s)
{
  int c;

  for (;;) {
    /* skip until first char */
    for (;;) {
      c = nextc(p);
      if (c < 0) return c;
      if (c == '\n') {
        p->lineno++;
        p->column = 0;
      }
      if (c == *s) break;
    }
    s++;
    if (peeks(p, s)) {
      int len = strlen(s);

      while (len--) {
        if (nextc(p) == '\n') {
          p->lineno++;
          p->column = 0;
        }
      }
      return TRUE;
    }
    else{
      s--;
    }
  }
  return FALSE;
}


static int
newtok(parser_state *p)
{
  p->bidx = 0;
  return p->column - 1;
}

static void
tokadd(parser_state *p, int32_t c)
{
  char utf8[4];
  unsigned len;

  /* mrb_assert(-0x10FFFF <= c && c <= 0xFF); */
  if (c >= 0) {
    /* Single byte from source or non-Unicode escape */
    utf8[0] = (char)c;
    len = 1;
  }
  else {
    /* Unicode character */
    c = -c;
    if (c < 0x80) {
      utf8[0] = (char)c;
      len = 1;
    }
    else if (c < 0x800) {
      utf8[0] = (char)(0xC0 | (c >> 6));
      utf8[1] = (char)(0x80 | (c & 0x3F));
      len = 2;
    }
    else if (c < 0x10000) {
      utf8[0] = (char)(0xE0 |  (c >> 12)        );
      utf8[1] = (char)(0x80 | ((c >>  6) & 0x3F));
      utf8[2] = (char)(0x80 | ( c        & 0x3F));
      len = 3;
    }
    else {
      utf8[0] = (char)(0xF0 |  (c >> 18)        );
      utf8[1] = (char)(0x80 | ((c >> 12) & 0x3F));
      utf8[2] = (char)(0x80 | ((c >>  6) & 0x3F));
      utf8[3] = (char)(0x80 | ( c        & 0x3F));
      len = 4;
    }
  }
  if (p->bidx+len <= MRB_PARSER_BUF_SIZE) {
    unsigned i;
    for (i = 0; i < len; i++) {
      p->buf[p->bidx++] = utf8[i];
    }
  }
}

static int
toklast(parser_state *p)
{
  return p->buf[p->bidx-1];
}

static void
tokfix(parser_state *p)
{
  if (p->bidx >= MRB_PARSER_BUF_SIZE) {
    yyerror(p, "string too long (truncated)");
  }
  p->buf[p->bidx] = '\0';
}

static const char*
tok(parser_state *p)
{
  return p->buf;
}

static int
toklen(parser_state *p)
{
  return p->bidx;
}

#define IS_ARG() (p->lstate == EXPR_ARG || p->lstate == EXPR_CMDARG)
#define IS_END() (p->lstate == EXPR_END || p->lstate == EXPR_ENDARG || p->lstate == EXPR_ENDFN)
#define IS_BEG() (p->lstate == EXPR_BEG || p->lstate == EXPR_MID || p->lstate == EXPR_VALUE || p->lstate == EXPR_CLASS)
#define IS_SPCARG(c) (IS_ARG() && space_seen && !ISSPACE(c))
#define IS_LABEL_POSSIBLE() ((p->lstate == EXPR_BEG && !cmd_state) || IS_ARG())
#define IS_LABEL_SUFFIX(n) (peek_n(p, ':',(n)) && !peek_n(p, ':', (n)+1))

static int
scan_oct(const int *start, int len, int *retlen)
{
  const int *s = start;
  int retval = 0;

  /* mrb_assert(len <= 3) */
  while (len-- && *s >= '0' && *s <= '7') {
    retval <<= 3;
    retval |= *s++ - '0';
  }
  *retlen = s - start;

  return retval;
}

static int32_t
scan_hex(const int *start, int len, int *retlen)
{
  static const char hexdigit[] = "0123456789abcdef0123456789ABCDEF";
  const int *s = start;
  int32_t retval = 0;
  char *tmp;

  /* mrb_assert(len <= 8) */
  while (len-- && *s && (tmp = (char*)strchr(hexdigit, *s))) {
    retval <<= 4;
    retval |= (tmp - hexdigit) & 15;
    s++;
  }
  *retlen = s - start;

  return retval;
}

/* Return negative to indicate Unicode code point */
static int32_t
read_escape(parser_state *p)
{
  int32_t c;

  switch (c = nextc(p)) {
  case '\\':/* Backslash */
    return c;

  case 'n':/* newline */
    return '\n';

  case 't':/* horizontal tab */
    return '\t';

  case 'r':/* carriage-return */
    return '\r';

  case 'f':/* form-feed */
    return '\f';

  case 'v':/* vertical tab */
    return '\13';

  case 'a':/* alarm(bell) */
    return '\007';

  case 'e':/* escape */
    return 033;

  case '0': case '1': case '2': case '3': /* octal constant */
  case '4': case '5': case '6': case '7':
  {
    int buf[3];
    int i;

    buf[0] = c;
    for (i=1; i<3; i++) {
      buf[i] = nextc(p);
      if (buf[i] < 0) goto eof;
      if (buf[i] < '0' || '7' < buf[i]) {
        pushback(p, buf[i]);
        break;
      }
    }
    c = scan_oct(buf, i, &i);
  }
  return c;

  case 'x':     /* hex constant */
  {
    int buf[2];
    int i;

    for (i=0; i<2; i++) {
      buf[i] = nextc(p);
      if (buf[i] < 0) goto eof;
      if (!ISXDIGIT(buf[i])) {
        pushback(p, buf[i]);
        break;
      }
    }
    c = scan_hex(buf, i, &i);
    if (i == 0) {
      yyerror(p, "Invalid escape character syntax");
      return 0;
    }
  }
  return c;

  case 'u':     /* Unicode */
  {
    int buf[9];
    int i;

    /* Look for opening brace */
    i = 0;
    buf[0] = nextc(p);
    if (buf[0] < 0) goto eof;
    if (buf[0] == '{') {
      /* \u{xxxxxxxx} form */
      for (i=0; i<9; i++) {
        buf[i] = nextc(p);
        if (buf[i] < 0) goto eof;
        if (buf[i] == '}') {
          break;
        }
        else if (!ISXDIGIT(buf[i])) {
          yyerror(p, "Invalid escape character syntax");
          pushback(p, buf[i]);
          return 0;
        }
      }
    }
    else if (ISXDIGIT(buf[0])) {
      /* \uxxxx form */
      for (i=1; i<4; i++) {
        buf[i] = nextc(p);
        if (buf[i] < 0) goto eof;
        if (!ISXDIGIT(buf[i])) {
          pushback(p, buf[i]);
          break;
        }
      }
    }
    else {
      pushback(p, buf[0]);
    }
    c = scan_hex(buf, i, &i);
    if (i == 0) {
      yyerror(p, "Invalid escape character syntax");
      return 0;
    }
    if (c < 0 || c > 0x10FFFF || (c & 0xFFFFF800) == 0xD800) {
      yyerror(p, "Invalid Unicode code point");
      return 0;
    }
  }
  return -c;

  case 'b':/* backspace */
    return '\010';

  case 's':/* space */
    return ' ';

  case 'M':
    if ((c = nextc(p)) != '-') {
      yyerror(p, "Invalid escape character syntax");
      pushback(p, c);
      return '\0';
    }
    if ((c = nextc(p)) == '\\') {
      return read_escape(p) | 0x80;
    }
    else if (c < 0) goto eof;
    else {
      return ((c & 0xff) | 0x80);
    }

  case 'C':
    if ((c = nextc(p)) != '-') {
      yyerror(p, "Invalid escape character syntax");
      pushback(p, c);
      return '\0';
    }
  case 'c':
    if ((c = nextc(p))== '\\') {
      c = read_escape(p);
    }
    else if (c == '?')
      return 0177;
    else if (c < 0) goto eof;
    return c & 0x9f;

    eof:
  case -1:
  case -2:
    yyerror(p, "Invalid escape character syntax");
    return '\0';

  default:
    return c;
  }
}

static int
parse_string(parser_state *p)
{
  int c;
  string_type type = (string_type)(intptr_t)p->lex_strterm->car;
  int nest_level = (intptr_t)p->lex_strterm->cdr->car;
  int beg = (intptr_t)p->lex_strterm->cdr->cdr->car;
  int end = (intptr_t)p->lex_strterm->cdr->cdr->cdr;
  parser_heredoc_info *hinf = (type & STR_FUNC_HEREDOC) ? parsing_heredoc_inf(p) : NULL;

  newtok(p);
  while ((c = nextc(p)) != end || nest_level != 0) {
    if (hinf && (c == '\n' || c < 0)) {
      int line_head;
      tokadd(p, '\n');
      tokfix(p);
      p->lineno++;
      p->column = 0;
      line_head = hinf->line_head;
      hinf->line_head = TRUE;
      if (line_head) {
        /* check whether end of heredoc */
        const char *s = tok(p);
        int len = toklen(p);
        if (hinf->allow_indent) {
          while (ISSPACE(*s) && len > 0) {
            ++s;
            --len;
          }
        }
        if ((len-1 == hinf->term_len) && (strncmp(s, hinf->term, len-1) == 0)) {
          return tHEREDOC_END;
        }
      }
      if (c < 0) {
        char buf[256];
        snprintf(buf, sizeof(buf), "can't find heredoc delimiter \"%s\" anywhere before EOF", hinf->term);
        yyerror(p, buf);
        return 0;
      }
      yylval.nd = new_str(p, tok(p), toklen(p));
      return tHD_STRING_MID;
    }
    if (c < 0) {
      yyerror(p, "unterminated string meets end of file");
      return 0;
    }
    else if (c == beg) {
      nest_level++;
      p->lex_strterm->cdr->car = (node*)(intptr_t)nest_level;
    }
    else if (c == end) {
      nest_level--;
      p->lex_strterm->cdr->car = (node*)(intptr_t)nest_level;
    }
    else if (c == '\\') {
      c = nextc(p);
      if (type & STR_FUNC_EXPAND) {
        if (c == end || c == beg) {
          tokadd(p, c);
        }
        else if (c == '\n') {
          p->lineno++;
          p->column = 0;
          if (type & STR_FUNC_ARRAY) {
            tokadd(p, '\n');
          }
        }
        else if (type & STR_FUNC_REGEXP) {
          tokadd(p, '\\');
          tokadd(p, c);
        }
        else {
          pushback(p, c);
          tokadd(p, read_escape(p));
          if (hinf)
            hinf->line_head = FALSE;
        }
      }
      else {
        if (c != beg && c != end) {
          if (c == '\n') {
            p->lineno++;
            p->column = 0;
          }
          if (!(c == '\\' || ((type & STR_FUNC_ARRAY) && ISSPACE(c)))) {
            tokadd(p, '\\');
          }
        }
        tokadd(p, c);
      }
      continue;
    }
    else if ((c == '#') && (type & STR_FUNC_EXPAND)) {
      c = nextc(p);
      if (c == '{') {
        tokfix(p);
        p->lstate = EXPR_BEG;
        p->cmd_start = TRUE;
        yylval.nd = new_str(p, tok(p), toklen(p));
        if (hinf) {
          hinf->line_head = FALSE;
          return tHD_STRING_PART;
        }
        return tSTRING_PART;
      }
      tokadd(p, '#');
      pushback(p, c);
      continue;
    }
    if ((type & STR_FUNC_ARRAY) && ISSPACE(c)) {
      if (toklen(p) == 0) {
        do {
          if (c == '\n') {
            p->lineno++;
            p->column = 0;
            heredoc_treat_nextline(p);
            if (p->parsing_heredoc != NULL) {
              return tHD_LITERAL_DELIM;
            }
          }
        } while (ISSPACE(c = nextc(p)));
        pushback(p, c);
        return tLITERAL_DELIM;
      }
      else {
        pushback(p, c);
        tokfix(p);
        yylval.nd = new_str(p, tok(p), toklen(p));
        return tSTRING_MID;
      }
    }
    tokadd(p, c);
  }

  tokfix(p);
  p->lstate = EXPR_END;
  end_strterm(p);

  if (type & STR_FUNC_XQUOTE) {
    yylval.nd = new_xstr(p, tok(p), toklen(p));
    return tXSTRING;
  }

  if (type & STR_FUNC_REGEXP) {
    int f = 0;
    int c;
    char *s = strndup(tok(p), toklen(p));
    char flags[3];
    char *flag = flags;
    char *dup;

    newtok(p);
    while (c = nextc(p), c >= 0 && ISALPHA(c)) {
      switch (c) {
      case 'i': f |= 1; break;
      case 'x': f |= 2; break;
      case 'm': f |= 4; break;
      default: tokadd(p, c); break;
      }
    }
    pushback(p, c);
    if (toklen(p)) {
      char msg[128];
      tokfix(p);
      snprintf(msg, sizeof(msg), "unknown regexp option%s - %s",
          toklen(p) > 1 ? "s" : "", tok(p));
      yyerror(p, msg);
    }
    if (f != 0) {
      if (f & 1) *flag++ = 'i';
      if (f & 2) *flag++ = 'x';
      if (f & 4) *flag++ = 'm';
      dup = strndup(flags, (size_t)(flag - flags));
    }
    else {
      dup = NULL;
    }
    yylval.nd = new_regx(p, s, dup);

    return tREGEXP;
  }

  yylval.nd = new_str(p, tok(p), toklen(p));
  return tSTRING;
}


static int
heredoc_identifier(parser_state *p)
{
  int c;
  int type = str_heredoc;
  mrb_bool indent = FALSE;
  mrb_bool quote = FALSE;
  node *newnode;
  parser_heredoc_info *info;

  c = nextc(p);
  if (ISSPACE(c) || c == '=') {
    pushback(p, c);
    return 0;
  }
  if (c == '-') {
    indent = TRUE;
    c = nextc(p);
  }
  if (c == '\'' || c == '"') {
    int term = c;
    if (c == '\'')
      quote = TRUE;
    newtok(p);
    while ((c = nextc(p)) >= 0 && c != term) {
      if (c == '\n') {
        c = -1;
        break;
      }
      tokadd(p, c);
    }
    if (c < 0) {
      yyerror(p, "unterminated here document identifier");
      return 0;
    }
  }
  else {
    if (c < 0) {
      return 0;                 /* missing here document identifier */
    }
    if (! identchar(c)) {
      pushback(p, c);
      if (indent) pushback(p, '-');
      return 0;
    }
    newtok(p);
    do {
      tokadd(p, c);
    } while ((c = nextc(p)) >= 0 && identchar(c));
    pushback(p, c);
  }
  tokfix(p);
  newnode = new_heredoc(p);
  info = (parser_heredoc_info*)newnode->cdr;
  info->term = strndup(tok(p), toklen(p));
  info->term_len = toklen(p);
  if (! quote)
    type |= STR_FUNC_EXPAND;
  info->type = (string_type)type;
  info->allow_indent = indent;
  info->line_head = TRUE;
  info->doc = NULL;
  p->heredocs_from_nextline = push(p->heredocs_from_nextline, newnode);
  p->lstate = EXPR_END;

  yylval.nd = newnode;
  return tHEREDOC_BEG;
}

static int
arg_ambiguous(parser_state *p)
{
  yywarning(p, "ambiguous first argument; put parentheses or even spaces");
  return 1;
}

#include "lex.def"

static int
parser_yylex(parser_state *p)
{
  int32_t c;
  int space_seen = 0;
  int cmd_state;
  enum mrb_lex_state_enum last_state;
  int token_column;

  if (p->lex_strterm) {
    if (is_strterm_type(p, STR_FUNC_HEREDOC)) {
      if (p->parsing_heredoc != NULL)
        return parse_string(p);
    }
    else
      return parse_string(p);
  }
  cmd_state = p->cmd_start;
  p->cmd_start = FALSE;
  retry:
  last_state = p->lstate;
  switch (c = nextc(p)) {
  case '\0':    /* NUL */
  case '\004':  /* ^D */
  case '\032':  /* ^Z */
    return 0;
  case -1:      /* end of script. */
    if (p->heredocs_from_nextline)
      goto maybe_heredoc;
    return 0;

  /* white spaces */
  case ' ': case '\t': case '\f': case '\r':
  case '\13':   /* '\v' */
    space_seen = 1;
    goto retry;

  case '#':     /* it's a comment */
    skip(p, '\n');
    /* fall through */
  case -2:      /* end of partial script. */
  case '\n':
    maybe_heredoc:
    heredoc_treat_nextline(p);
  switch (p->lstate) {
  case EXPR_BEG:
  case EXPR_FNAME:
  case EXPR_DOT:
  case EXPR_CLASS:
  case EXPR_VALUE:
    p->lineno++;
    p->column = 0;
    if (p->parsing_heredoc != NULL) {
      return parse_string(p);
    }
    goto retry;
  default:
    break;
  }
  if (p->parsing_heredoc != NULL) {
    return '\n';
  }
  while ((c = nextc(p))) {
    switch (c) {
    case ' ': case '\t': case '\f': case '\r':
    case '\13': /* '\v' */
      space_seen = 1;
      break;
    case '.':
      if ((c = nextc(p)) != '.') {
        pushback(p, c);
        pushback(p, '.');
        goto retry;
      }
    case -1:                  /* EOF */
    case -2:                  /* end of partial script */
      goto normal_newline;
    default:
      pushback(p, c);
      goto normal_newline;
    }
  }
  normal_newline:
  p->cmd_start = TRUE;
  p->lstate = EXPR_BEG;
  return '\n';

  case '*':
    if ((c = nextc(p)) == '*') {
      if ((c = nextc(p)) == '=') {
        yylval.id = intern("**",2);
        p->lstate = EXPR_BEG;
        return tOP_ASGN;
      }
      pushback(p, c);
      c = tPOW;
    }
    else {
      if (c == '=') {
        yylval.id = intern_c('*');
        p->lstate = EXPR_BEG;
        return tOP_ASGN;
      }
      pushback(p, c);
      if (IS_SPCARG(c)) {
        yywarning(p, "`*' interpreted as argument prefix");
        c = tSTAR;
      }
      else if (IS_BEG()) {
        c = tSTAR;
      }
      else {
        c = '*';
      }
    }
    if (p->lstate == EXPR_FNAME || p->lstate == EXPR_DOT) {
      p->lstate = EXPR_ARG;
    }
    else {
      p->lstate = EXPR_BEG;
    }
    return c;

  case '!':
    c = nextc(p);
    if (p->lstate == EXPR_FNAME || p->lstate == EXPR_DOT) {
      p->lstate = EXPR_ARG;
      if (c == '@') {
        return '!';
      }
    }
    else {
      p->lstate = EXPR_BEG;
    }
    if (c == '=') {
      return tNEQ;
    }
    if (c == '~') {
      return tNMATCH;
    }
    pushback(p, c);
    return '!';

  case '=':
    if (p->column == 1) {
      static const char begin[] = "begin";
      static const char end[] = "\n=end";
      if (peeks(p, begin)) {
        c = peekc_n(p, sizeof(begin)-1);
        if (c < 0 || isspace(c)) {
          do {
            if (!skips(p, end)) {
              yyerror(p, "embedded document meets end of file");
              return 0;
            }
            c = nextc(p);
          } while (!(c < 0 || isspace(c)));
          if (c != '\n') skip(p, '\n');
          p->lineno++;
          p->column = 0;
          goto retry;
        }
      }
    }
    if (p->lstate == EXPR_FNAME || p->lstate == EXPR_DOT) {
      p->lstate = EXPR_ARG;
    }
    else {
      p->lstate = EXPR_BEG;
    }
    if ((c = nextc(p)) == '=') {
      if ((c = nextc(p)) == '=') {
        return tEQQ;
      }
      pushback(p, c);
      return tEQ;
    }
    if (c == '~') {
      return tMATCH;
    }
    else if (c == '>') {
      return tASSOC;
    }
    pushback(p, c);
    return '=';

  case '<':
    last_state = p->lstate;
    c = nextc(p);
    if (c == '<' &&
        p->lstate != EXPR_DOT &&
        p->lstate != EXPR_CLASS &&
        !IS_END() &&
        (!IS_ARG() || space_seen)) {
      int token = heredoc_identifier(p);
      if (token)
        return token;
    }
    if (p->lstate == EXPR_FNAME || p->lstate == EXPR_DOT) {
      p->lstate = EXPR_ARG;
    }
    else {
      p->lstate = EXPR_BEG;
      if (p->lstate == EXPR_CLASS) {
        p->cmd_start = TRUE;
      }
    }
    if (c == '=') {
      if ((c = nextc(p)) == '>') {
        return tCMP;
      }
      pushback(p, c);
      return tLEQ;
    }
    if (c == '<') {
      if ((c = nextc(p)) == '=') {
        yylval.id = intern("<<",2);
        p->lstate = EXPR_BEG;
        return tOP_ASGN;
      }
      pushback(p, c);
      return tLSHFT;
    }
    pushback(p, c);
    return '<';

  case '>':
    if (p->lstate == EXPR_FNAME || p->lstate == EXPR_DOT) {
      p->lstate = EXPR_ARG;
    }
    else {
      p->lstate = EXPR_BEG;
    }
    if ((c = nextc(p)) == '=') {
      return tGEQ;
    }
    if (c == '>') {
      if ((c = nextc(p)) == '=') {
        yylval.id = intern(">>",2);
        p->lstate = EXPR_BEG;
        return tOP_ASGN;
      }
      pushback(p, c);
      return tRSHFT;
    }
    pushback(p, c);
    return '>';

  case '"':
    p->lex_strterm = new_strterm(p, str_dquote, '"', 0);
    return tSTRING_BEG;

  case '\'':
    p->lex_strterm = new_strterm(p, str_squote, '\'', 0);
    return parse_string(p);

  case '`':
    if (p->lstate == EXPR_FNAME) {
      p->lstate = EXPR_ENDFN;
      return '`';
    }
    if (p->lstate == EXPR_DOT) {
      if (cmd_state)
        p->lstate = EXPR_CMDARG;
      else
        p->lstate = EXPR_ARG;
      return '`';
    }
    p->lex_strterm = new_strterm(p, str_xquote, '`', 0);
    return tXSTRING_BEG;

  case '?':
    if (IS_END()) {
      p->lstate = EXPR_VALUE;
      return '?';
    }
    c = nextc(p);
    if (c < 0) {
      yyerror(p, "incomplete character syntax");
      return 0;
    }
    if (isspace(c)) {
      if (!IS_ARG()) {
        int c2;
        switch (c) {
        case ' ':
          c2 = 's';
          break;
        case '\n':
          c2 = 'n';
          break;
        case '\t':
          c2 = 't';
          break;
        case '\v':
          c2 = 'v';
          break;
        case '\r':
          c2 = 'r';
          break;
        case '\f':
          c2 = 'f';
          break;
        default:
          c2 = 0;
          break;
        }
        if (c2) {
          char buf[256];
          snprintf(buf, sizeof(buf), "invalid character syntax; use ?\\%c", c2);
          yyerror(p, buf);
        }
      }
      ternary:
      pushback(p, c);
      p->lstate = EXPR_VALUE;
      return '?';
    }
    token_column = newtok(p);
    /* need support UTF-8 if configured */
    if ((isalnum(c) || c == '_')) {
      int c2 = nextc(p);
      pushback(p, c2);
      if ((isalnum(c2) || c2 == '_')) {
        goto ternary;
      }
    }
    if (c == '\\') {
      c = read_escape(p);
      tokadd(p, c);
    }
    else {
      tokadd(p, c);
    }
    tokfix(p);
    yylval.nd = new_str(p, tok(p), toklen(p));
    p->lstate = EXPR_END;
    return tCHAR;

  case '&':
    if ((c = nextc(p)) == '&') {
      p->lstate = EXPR_BEG;
      if ((c = nextc(p)) == '=') {
        yylval.id = intern("&&",2);
        p->lstate = EXPR_BEG;
        return tOP_ASGN;
      }
      pushback(p, c);
      return tANDOP;
    }
    else if (c == '=') {
      yylval.id = intern_c('&');
      p->lstate = EXPR_BEG;
      return tOP_ASGN;
    }
    pushback(p, c);
    if (IS_SPCARG(c)) {
      yywarning(p, "`&' interpreted as argument prefix");
      c = tAMPER;
    }
    else if (IS_BEG()) {
      c = tAMPER;
    }
    else {
      c = '&';
    }
    if (p->lstate == EXPR_FNAME || p->lstate == EXPR_DOT) {
      p->lstate = EXPR_ARG;
    }
    else {
      p->lstate = EXPR_BEG;
    }
    return c;

  case '|':
    if ((c = nextc(p)) == '|') {
      p->lstate = EXPR_BEG;
      if ((c = nextc(p)) == '=') {
        yylval.id = intern("||",2);
        p->lstate = EXPR_BEG;
        return tOP_ASGN;
      }
      pushback(p, c);
      return tOROP;
    }
    if (c == '=') {
      yylval.id = intern_c('|');
      p->lstate = EXPR_BEG;
      return tOP_ASGN;
    }
    if (p->lstate == EXPR_FNAME || p->lstate == EXPR_DOT) {
      p->lstate = EXPR_ARG;
    }
    else {
      p->lstate = EXPR_BEG;
    }
    pushback(p, c);
    return '|';

  case '+':
    c = nextc(p);
    if (p->lstate == EXPR_FNAME || p->lstate == EXPR_DOT) {
      p->lstate = EXPR_ARG;
      if (c == '@') {
        return tUPLUS;
      }
      pushback(p, c);
      return '+';
    }
    if (c == '=') {
      yylval.id = intern_c('+');
      p->lstate = EXPR_BEG;
      return tOP_ASGN;
    }
    if (IS_BEG() || (IS_SPCARG(c) && arg_ambiguous(p))) {
      p->lstate = EXPR_BEG;
      pushback(p, c);
      if (c >= 0 && ISDIGIT(c)) {
        c = '+';
        goto start_num;
      }
      return tUPLUS;
    }
    p->lstate = EXPR_BEG;
    pushback(p, c);
    return '+';

  case '-':
    c = nextc(p);
    if (p->lstate == EXPR_FNAME || p->lstate == EXPR_DOT) {
      p->lstate = EXPR_ARG;
      if (c == '@') {
        return tUMINUS;
      }
      pushback(p, c);
      return '-';
    }
    if (c == '=') {
      yylval.id = intern_c('-');
      p->lstate = EXPR_BEG;
      return tOP_ASGN;
    }
    if (c == '>') {
      p->lstate = EXPR_ENDFN;
      return tLAMBDA;
    }
    if (IS_BEG() || (IS_SPCARG(c) && arg_ambiguous(p))) {
      p->lstate = EXPR_BEG;
      pushback(p, c);
      if (c >= 0 && ISDIGIT(c)) {
        return tUMINUS_NUM;
      }
      return tUMINUS;
    }
    p->lstate = EXPR_BEG;
    pushback(p, c);
    return '-';

  case '.':
    p->lstate = EXPR_BEG;
    if ((c = nextc(p)) == '.') {
      if ((c = nextc(p)) == '.') {
        return tDOT3;
      }
      pushback(p, c);
      return tDOT2;
    }
    pushback(p, c);
    if (c >= 0 && ISDIGIT(c)) {
      yyerror(p, "no .<digit> floating literal anymore; put 0 before dot");
    }
    p->lstate = EXPR_DOT;
    return '.';

    start_num:
  case '0': case '1': case '2': case '3': case '4':
  case '5': case '6': case '7': case '8': case '9':
  {
    int is_float, seen_point, seen_e, nondigit;

    is_float = seen_point = seen_e = nondigit = 0;
    p->lstate = EXPR_END;
    token_column = newtok(p);
    if (c == '-' || c == '+') {
      tokadd(p, c);
      c = nextc(p);
    }
    if (c == '0') {
#define no_digits() do {yyerror(p,"numeric literal without digits"); return 0;} while (0)
      int start = toklen(p);
      c = nextc(p);
      if (c == 'x' || c == 'X') {
        /* hexadecimal */
        c = nextc(p);
        if (c >= 0 && ISXDIGIT(c)) {
          do {
            if (c == '_') {
              if (nondigit) break;
              nondigit = c;
              continue;
            }
            if (!ISXDIGIT(c)) break;
            nondigit = 0;
            tokadd(p, tolower(c));
          } while ((c = nextc(p)) >= 0);
        }
        pushback(p, c);
        tokfix(p);
        if (toklen(p) == start) {
          no_digits();
        }
        else if (nondigit) goto trailing_uc;
        yylval.nd = new_int(p, tok(p), 16);
        return tINTEGER;
      }
      if (c == 'b' || c == 'B') {
        /* binary */
        c = nextc(p);
        if (c == '0' || c == '1') {
          do {
            if (c == '_') {
              if (nondigit) break;
              nondigit = c;
              continue;
            }
            if (c != '0' && c != '1') break;
            nondigit = 0;
            tokadd(p, c);
          } while ((c = nextc(p)) >= 0);
        }
        pushback(p, c);
        tokfix(p);
        if (toklen(p) == start) {
          no_digits();
        }
        else if (nondigit) goto trailing_uc;
        yylval.nd = new_int(p, tok(p), 2);
        return tINTEGER;
      }
      if (c == 'd' || c == 'D') {
        /* decimal */
        c = nextc(p);
        if (c >= 0 && ISDIGIT(c)) {
          do {
            if (c == '_') {
              if (nondigit) break;
              nondigit = c;
              continue;
            }
            if (!ISDIGIT(c)) break;
            nondigit = 0;
            tokadd(p, c);
          } while ((c = nextc(p)) >= 0);
        }
        pushback(p, c);
        tokfix(p);
        if (toklen(p) == start) {
          no_digits();
        }
        else if (nondigit) goto trailing_uc;
        yylval.nd = new_int(p, tok(p), 10);
        return tINTEGER;
      }
      if (c == '_') {
        /* 0_0 */
        goto octal_number;
      }
      if (c == 'o' || c == 'O') {
        /* prefixed octal */
        c = nextc(p);
        if (c < 0 || c == '_' || !ISDIGIT(c)) {
          no_digits();
        }
      }
      if (c >= '0' && c <= '7') {
        /* octal */
        octal_number:
        do {
          if (c == '_') {
            if (nondigit) break;
            nondigit = c;
            continue;
          }
          if (c < '0' || c > '9') break;
          if (c > '7') goto invalid_octal;
          nondigit = 0;
          tokadd(p, c);
        } while ((c = nextc(p)) >= 0);

        if (toklen(p) > start) {
          pushback(p, c);
          tokfix(p);
          if (nondigit) goto trailing_uc;
          yylval.nd = new_int(p, tok(p), 8);
          return tINTEGER;
        }
        if (nondigit) {
          pushback(p, c);
          goto trailing_uc;
        }
      }
      if (c > '7' && c <= '9') {
        invalid_octal:
        yyerror(p, "Invalid octal digit");
      }
      else if (c == '.' || c == 'e' || c == 'E') {
        tokadd(p, '0');
      }
      else {
        pushback(p, c);
        yylval.nd = new_int(p, "0", 10);
        return tINTEGER;
      }
    }

    for (;;) {
      switch (c) {
      case '0': case '1': case '2': case '3': case '4':
      case '5': case '6': case '7': case '8': case '9':
        nondigit = 0;
        tokadd(p, c);
        break;

      case '.':
        if (nondigit) goto trailing_uc;
        if (seen_point || seen_e) {
          goto decode_num;
        }
        else {
          int c0 = nextc(p);
          if (c0 < 0 || !ISDIGIT(c0)) {
            pushback(p, c0);
            goto decode_num;
          }
          c = c0;
        }
        tokadd(p, '.');
        tokadd(p, c);
        is_float++;
        seen_point++;
        nondigit = 0;
        break;

      case 'e':
      case 'E':
        if (nondigit) {
          pushback(p, c);
          c = nondigit;
          goto decode_num;
        }
        if (seen_e) {
          goto decode_num;
        }
        tokadd(p, c);
        seen_e++;
        is_float++;
        nondigit = c;
        c = nextc(p);
        if (c != '-' && c != '+') continue;
        tokadd(p, c);
        nondigit = c;
        break;

      case '_':       /* `_' in number just ignored */
        if (nondigit) goto decode_num;
        nondigit = c;
        break;

      default:
        goto decode_num;
      }
      c = nextc(p);
    }

    decode_num:
    pushback(p, c);
    if (nondigit) {
      trailing_uc:
      yyerror_i(p, "trailing `%c' in number", nondigit);
    }
    tokfix(p);
    if (is_float) {
      double d;
      char *endp;

      errno = 0;
      d = strtod(tok(p), &endp);
      if (d == 0 && endp == tok(p)) {
        yywarning_s(p, "corrupted float value %s", tok(p));
      }
      else if (errno == ERANGE) {
        yywarning_s(p, "float %s out of range", tok(p));
        errno = 0;
      }
      yylval.nd = new_float(p, tok(p));
      return tFLOAT;
    }
    yylval.nd = new_int(p, tok(p), 10);
    return tINTEGER;
  }

  case ')':
  case ']':
    p->paren_nest--;
  case '}':
    COND_LEXPOP();
    CMDARG_LEXPOP();
    if (c == ')')
      p->lstate = EXPR_ENDFN;
    else
      p->lstate = EXPR_ENDARG;
    return c;

  case ':':
    c = nextc(p);
    if (c == ':') {
      if (IS_BEG() || p->lstate == EXPR_CLASS || IS_SPCARG(-1)) {
        p->lstate = EXPR_BEG;
        return tCOLON3;
      }
      p->lstate = EXPR_DOT;
      return tCOLON2;
    }
    if (IS_END() || ISSPACE(c)) {
      pushback(p, c);
      p->lstate = EXPR_BEG;
      return ':';
    }
    pushback(p, c);
    p->lstate = EXPR_FNAME;
    return tSYMBEG;

  case '/':
    if (IS_BEG()) {
      p->lex_strterm = new_strterm(p, str_regexp, '/', 0);
      return tREGEXP_BEG;
    }
    if ((c = nextc(p)) == '=') {
      yylval.id = intern_c('/');
      p->lstate = EXPR_BEG;
      return tOP_ASGN;
    }
    pushback(p, c);
    if (IS_SPCARG(c)) {
      p->lex_strterm = new_strterm(p, str_regexp, '/', 0);
      return tREGEXP_BEG;
    }
    if (p->lstate == EXPR_FNAME || p->lstate == EXPR_DOT) {
      p->lstate = EXPR_ARG;
    }
    else {
      p->lstate = EXPR_BEG;
    }
    return '/';

  case '^':
    if ((c = nextc(p)) == '=') {
      yylval.id = intern_c('^');
      p->lstate = EXPR_BEG;
      return tOP_ASGN;
    }
    if (p->lstate == EXPR_FNAME || p->lstate == EXPR_DOT) {
      p->lstate = EXPR_ARG;
    }
    else {
      p->lstate = EXPR_BEG;
    }
    pushback(p, c);
    return '^';

  case ';':
    p->lstate = EXPR_BEG;
    return ';';

  case ',':
    p->lstate = EXPR_BEG;
    return ',';

  case '~':
    if (p->lstate == EXPR_FNAME || p->lstate == EXPR_DOT) {
      if ((c = nextc(p)) != '@') {
        pushback(p, c);
      }
      p->lstate = EXPR_ARG;
    }
    else {
      p->lstate = EXPR_BEG;
    }
    return '~';

  case '(':
    if (IS_BEG()) {
      c = tLPAREN;
    }
    else if (IS_SPCARG(-1)) {
      c = tLPAREN_ARG;
    }
    p->paren_nest++;
    COND_PUSH(0);
    CMDARG_PUSH(0);
    p->lstate = EXPR_BEG;
    return c;

  case '[':
    p->paren_nest++;
    if (p->lstate == EXPR_FNAME || p->lstate == EXPR_DOT) {
      p->lstate = EXPR_ARG;
      if ((c = nextc(p)) == ']') {
        if ((c = nextc(p)) == '=') {
          return tASET;
        }
        pushback(p, c);
        return tAREF;
      }
      pushback(p, c);
      return '[';
    }
    else if (IS_BEG()) {
      c = tLBRACK;
    }
    else if (IS_ARG() && space_seen) {
      c = tLBRACK;
    }
    p->lstate = EXPR_BEG;
    COND_PUSH(0);
    CMDARG_PUSH(0);
    return c;

  case '{':
    if (p->lpar_beg && p->lpar_beg == p->paren_nest) {
      p->lstate = EXPR_BEG;
      p->lpar_beg = 0;
      p->paren_nest--;
      COND_PUSH(0);
      CMDARG_PUSH(0);
      return tLAMBEG;
    }
    if (IS_ARG() || p->lstate == EXPR_END || p->lstate == EXPR_ENDFN)
      c = '{';          /* block (primary) */
    else if (p->lstate == EXPR_ENDARG)
      c = tLBRACE_ARG;  /* block (expr) */
    else
      c = tLBRACE;      /* hash */
    COND_PUSH(0);
    CMDARG_PUSH(0);
    p->lstate = EXPR_BEG;
    return c;

  case '\\':
    c = nextc(p);
    if (c == '\n') {
      p->lineno++;
      p->column = 0;
      space_seen = 1;
      goto retry; /* skip \\n */
    }
    pushback(p, c);
    return '\\';

  case '%':
    if (IS_BEG()) {
      int term;
      int paren;

      c = nextc(p);
      quotation:
      if (c < 0 || !ISALNUM(c)) {
        term = c;
        c = 'Q';
      }
      else {
        term = nextc(p);
        if (isalnum(term)) {
          yyerror(p, "unknown type of %string");
          return 0;
        }
      }
      if (c < 0 || term < 0) {
        yyerror(p, "unterminated quoted string meets end of file");
        return 0;
      }
      paren = term;
      if (term == '(') term = ')';
      else if (term == '[') term = ']';
      else if (term == '{') term = '}';
      else if (term == '<') term = '>';
      else paren = 0;

      switch (c) {
      case 'Q':
        p->lex_strterm = new_strterm(p, str_dquote, term, paren);
        return tSTRING_BEG;

      case 'q':
        p->lex_strterm = new_strterm(p, str_squote, term, paren);
        return parse_string(p);

      case 'W':
        p->lex_strterm = new_strterm(p, str_dword, term, paren);
        return tWORDS_BEG;

      case 'w':
        p->lex_strterm = new_strterm(p, str_sword, term, paren);
        return tWORDS_BEG;

      case 'x':
        p->lex_strterm = new_strterm(p, str_xquote, term, paren);
        return tXSTRING_BEG;

      case 'r':
        p->lex_strterm = new_strterm(p, str_regexp, term, paren);
        return tREGEXP_BEG;

      case 's':
        p->lex_strterm = new_strterm(p, str_ssym, term, paren);
        return tSYMBEG;

      case 'I':
        p->lex_strterm = new_strterm(p, str_dsymbols, term, paren);
        return tSYMBOLS_BEG;

      case 'i':
        p->lex_strterm = new_strterm(p, str_ssymbols, term, paren);
        return tSYMBOLS_BEG;

      default:
        yyerror(p, "unknown type of %string");
        return 0;
      }
    }
    if ((c = nextc(p)) == '=') {
      yylval.id = intern_c('%');
      p->lstate = EXPR_BEG;
      return tOP_ASGN;
    }
    if (IS_SPCARG(c)) {
      goto quotation;
    }
    if (p->lstate == EXPR_FNAME || p->lstate == EXPR_DOT) {
      p->lstate = EXPR_ARG;
    }
    else {
      p->lstate = EXPR_BEG;
    }
    pushback(p, c);
    return '%';

  case '$':
    p->lstate = EXPR_END;
    token_column = newtok(p);
    c = nextc(p);
    if (c < 0) {
      yyerror(p, "incomplete global variable syntax");
      return 0;
    }
    switch (c) {
    case '_':     /* $_: last read line string */
      c = nextc(p);
      if (c >= 0 && identchar(c)) { /* if there is more after _ it is a variable */
        tokadd(p, '$');
        tokadd(p, c);
        break;
      }
      pushback(p, c);
      c = '_';
      /* fall through */
    case '~':     /* $~: match-data */
    case '*':     /* $*: argv */
    case '$':     /* $$: pid */
    case '?':     /* $?: last status */
    case '!':     /* $!: error string */
    case '@':     /* $@: error position */
    case '/':     /* $/: input record separator */
    case '\\':    /* $\: output record separator */
    case ';':     /* $;: field separator */
    case ',':     /* $,: output field separator */
    case '.':     /* $.: last read line number */
    case '=':     /* $=: ignorecase */
    case ':':     /* $:: load path */
    case '<':     /* $<: reading filename */
    case '>':     /* $>: default output handle */
    case '\"':    /* $": already loaded files */
      tokadd(p, '$');
      tokadd(p, c);
      tokfix(p);
      yylval.id = intern_cstr(tok(p));
      return tGVAR;

    case '-':
      tokadd(p, '$');
      tokadd(p, c);
      c = nextc(p);
      pushback(p, c);
      gvar:
      tokfix(p);
      yylval.id = intern_cstr(tok(p));
      return tGVAR;

    case '&':     /* $&: last match */
    case '`':     /* $`: string before last match */
    case '\'':    /* $': string after last match */
    case '+':     /* $+: string matches last pattern */
      if (last_state == EXPR_FNAME) {
        tokadd(p, '$');
        tokadd(p, c);
        goto gvar;
      }
      yylval.nd = new_back_ref(p, c);
      return tBACK_REF;

    case '1': case '2': case '3':
    case '4': case '5': case '6':
    case '7': case '8': case '9':
      do {
        tokadd(p, c);
        c = nextc(p);
      } while (c >= 0 && isdigit(c));
      pushback(p, c);
      if (last_state == EXPR_FNAME) goto gvar;
      tokfix(p);
      yylval.nd = new_nth_ref(p, atoi(tok(p)));
      return tNTH_REF;

    default:
      if (!identchar(c)) {
        pushback(p,  c);
        return '$';
      }
    case '0':
      tokadd(p, '$');
    }
    break;

    case '@':
      c = nextc(p);
      token_column = newtok(p);
      tokadd(p, '@');
      if (c == '@') {
        tokadd(p, '@');
        c = nextc(p);
      }
      if (c < 0) {
        if (p->bidx == 1) {
          yyerror(p, "incomplete instance variable syntax");
        }
        else {
          yyerror(p, "incomplete class variable syntax");
        }
        return 0;
      }
      else if (isdigit(c)) {
        if (p->bidx == 1) {
          yyerror_i(p, "`@%c' is not allowed as an instance variable name", c);
        }
        else {
          yyerror_i(p, "`@@%c' is not allowed as a class variable name", c);
        }
        return 0;
      }
      if (!identchar(c)) {
        pushback(p, c);
        return '@';
      }
      break;

    case '_':
      token_column = newtok(p);
      break;

    default:
      if (!identchar(c)) {
        yyerror_i(p,  "Invalid char `\\x%02X' in expression", c);
        goto retry;
      }

      token_column = newtok(p);
      break;
  }

  do {
    tokadd(p, c);
    c = nextc(p);
    if (c < 0) break;
  } while (identchar(c));
  if (token_column == 0 && toklen(p) == 7 && (c < 0 || c == '\n') &&
      strncmp(tok(p), "__END__", toklen(p)) == 0)
    return -1;

  switch (tok(p)[0]) {
  case '@': case '$':
    pushback(p, c);
    break;
  default:
    if ((c == '!' || c == '?') && !peek(p, '=')) {
      tokadd(p, c);
    }
    else {
      pushback(p, c);
    }
  }
  tokfix(p);
  {
    int result = 0;

    last_state = p->lstate;
    switch (tok(p)[0]) {
    case '$':
      p->lstate = EXPR_END;
      result = tGVAR;
      break;
    case '@':
      p->lstate = EXPR_END;
      if (tok(p)[1] == '@')
        result = tCVAR;
      else
        result = tIVAR;
      break;

    default:
      if (toklast(p) == '!' || toklast(p) == '?') {
        result = tFID;
      }
      else {
        if (p->lstate == EXPR_FNAME) {
          if ((c = nextc(p)) == '=' && !peek(p, '~') && !peek(p, '>') &&
              (!peek(p, '=') || (peek_n(p, '>', 1)))) {
            result = tIDENTIFIER;
            tokadd(p, c);
            tokfix(p);
          }
          else {
            pushback(p, c);
          }
        }
        if (result == 0 && isupper((int)(unsigned char)tok(p)[0])) {
          result = tCONSTANT;
        }
        else {
          result = tIDENTIFIER;
        }
      }

      if (IS_LABEL_POSSIBLE()) {
        if (IS_LABEL_SUFFIX(0)) {
          p->lstate = EXPR_BEG;
          nextc(p);
          tokfix(p);
          yylval.id = intern_cstr(tok(p));
          return tLABEL;
        }
      }
      if (p->lstate != EXPR_DOT) {
        const struct kwtable *kw;

        /* See if it is a reserved word.  */
        kw = mrb_reserved_word(tok(p), toklen(p));
        if (kw) {
          enum mrb_lex_state_enum state = p->lstate;
          p->lstate = kw->state;
          if (state == EXPR_FNAME) {
            yylval.id = intern_cstr(kw->name);
            return kw->id[0];
          }
          if (p->lstate == EXPR_BEG) {
            p->cmd_start = TRUE;
          }
          if (kw->id[0] == keyword_do) {
            if (p->lpar_beg && p->lpar_beg == p->paren_nest) {
              p->lpar_beg = 0;
              p->paren_nest--;
              return keyword_do_LAMBDA;
            }
            if (COND_P()) return keyword_do_cond;
            if (CMDARG_P() && state != EXPR_CMDARG)
              return keyword_do_block;
            if (state == EXPR_ENDARG || state == EXPR_BEG)
              return keyword_do_block;
            return keyword_do;
          }
          if (state == EXPR_BEG || state == EXPR_VALUE)
            return kw->id[0];
          else {
            if (kw->id[0] != kw->id[1])
              p->lstate = EXPR_BEG;
            return kw->id[1];
          }
        }
      }

      if (IS_BEG() || p->lstate == EXPR_DOT || IS_ARG()) {
        if (cmd_state) {
          p->lstate = EXPR_CMDARG;
        }
        else {
          p->lstate = EXPR_ARG;
        }
      }
      else if (p->lstate == EXPR_FNAME) {
        p->lstate = EXPR_ENDFN;
      }
      else {
        p->lstate = EXPR_END;
      }
    }
    {
      mrb_sym ident = intern_cstr(tok(p));

      yylval.id = ident;
#if 0
      if (last_state != EXPR_DOT && islower(tok(p)[0]) && lvar_defined(ident)) {
        p->lstate = EXPR_END;
      }
#endif
    }
    return result;
  }
}

static int
yylex(void *lval, parser_state *p)
{
  int t;

  p->ylval = lval;
  t = parser_yylex(p);

  return t;
}

static void
parser_init_cxt(parser_state *p, mrbc_context *cxt)
{
  if (!cxt) return;
  if (cxt->filename) mrb_parser_set_filename(p, cxt->filename);
  if (cxt->lineno) p->lineno = cxt->lineno;
  if (cxt->syms) {
    int i;

    p->locals = cons(0,0);
    for (i=0; i<cxt->slen; i++) {
      local_add_f(p, cxt->syms[i]);
    }
  }
  p->capture_errors = cxt->capture_errors;
  if (cxt->partial_hook) {
    p->cxt = cxt;
  }
}

static void
parser_update_cxt(parser_state *p, mrbc_context *cxt)
{
  node *n, *n0;
  int i = 0;

  if (!cxt) return;
  if ((int)(intptr_t)p->tree->car != NODE_SCOPE) return;
  n0 = n = p->tree->cdr->car;
  while (n) {
    i++;
    n = n->cdr;
  }
  cxt->syms = (mrb_sym *)mrb_realloc(p->mrb, cxt->syms, i*sizeof(mrb_sym));
  cxt->slen = i;
  for (i=0, n=n0; n; i++,n=n->cdr) {
    cxt->syms[i] = sym(n->car);
  }
}

void mrb_codedump_all(mrb_state*, struct RProc*);
void mrb_parser_dump(mrb_state *mrb, node *tree, int offset);

void
mrb_parser_parse(parser_state *p, mrbc_context *c)
{
  struct mrb_jmpbuf buf;
  p->jmp = &buf;

  MRB_TRY(p->jmp) {

    p->cmd_start = TRUE;
    p->in_def = p->in_single = 0;
    p->nerr = p->nwarn = 0;
    p->lex_strterm = NULL;

    parser_init_cxt(p, c);
    yyparse(p);
    if (!p->tree) {
      p->tree = new_nil(p);
    }
    parser_update_cxt(p, c);
    if (c && c->dump_result) {
      mrb_parser_dump(p->mrb, p->tree, 0);
    }

  }
  MRB_CATCH(p->jmp) {
    yyerror(p, "memory allocation error");
    p->nerr++;
    p->tree = 0;
    return;
  }
  MRB_END_EXC(p->jmp);
}

parser_state*
mrb_parser_new(mrb_state *mrb)
{
  mrb_pool *pool;
  parser_state *p;
  static const parser_state parser_state_zero = { 0 };

  pool = mrb_pool_open(mrb);
  if (!pool) return 0;
  p = (parser_state *)mrb_pool_alloc(pool, sizeof(parser_state));
  if (!p) return 0;

  *p = parser_state_zero;
  p->mrb = mrb;
  p->pool = pool;

  p->s = p->send = NULL;
#ifdef ENABLE_STDIO
  p->f = NULL;
#endif

  p->cmd_start = TRUE;
  p->in_def = p->in_single = 0;

  p->capture_errors = FALSE;
  p->lineno = 1;
  p->column = 0;
#if defined(PARSER_TEST) || defined(PARSER_DEBUG)
  yydebug = 1;
#endif

  p->lex_strterm = NULL;
  p->all_heredocs = p->parsing_heredoc = NULL;
  p->lex_strterm_before_heredoc = NULL;

  p->current_filename_index = -1;
  p->filename_table = NULL;
  p->filename_table_length = 0;

  return p;
}

void
mrb_parser_free(parser_state *p) {
  mrb_pool_close(p->pool);
}

mrbc_context*
mrbc_context_new(mrb_state *mrb)
{
  mrbc_context *c;

  c = (mrbc_context *)mrb_calloc(mrb, 1, sizeof(mrbc_context));
  return c;
}

void
mrbc_context_free(mrb_state *mrb, mrbc_context *cxt)
{
  mrb_free(mrb, cxt->syms);
  mrb_free(mrb, cxt);
}

const char*
mrbc_filename(mrb_state *mrb, mrbc_context *c, const char *s)
{
  if (s) {
    int len = strlen(s);
    char *p = (char *)mrb_alloca(mrb, len + 1);

    memcpy(p, s, len + 1);
    c->filename = p;
  }
  return c->filename;
}

void
mrbc_partial_hook(mrb_state *mrb, mrbc_context *c, int (*func)(struct mrb_parser_state*), void *data)
{
  c->partial_hook = func;
  c->partial_data = data;
}

void
mrb_parser_set_filename(struct mrb_parser_state *p, const char *f)
{
  mrb_sym sym;
  size_t i;
  mrb_sym* new_table;

  sym = mrb_intern_cstr(p->mrb, f);
  p->filename = mrb_sym2name_len(p->mrb, sym, NULL);
  p->lineno = (p->filename_table_length > 0)? 0 : 1;

  for (i = 0; i < p->filename_table_length; ++i) {
    if (p->filename_table[i] == sym) {
      p->current_filename_index = i;
      return;
    }
  }

  p->current_filename_index = p->filename_table_length++;

  new_table = (mrb_sym*)parser_palloc(p, sizeof(mrb_sym) * p->filename_table_length);
  if (p->filename_table) {
    memmove(new_table, p->filename_table, sizeof(mrb_sym) * p->filename_table_length);
  }
  p->filename_table = new_table;
  p->filename_table[p->filename_table_length - 1] = sym;
}

char const*
mrb_parser_get_filename(struct mrb_parser_state* p, uint16_t idx) {
  if (idx >= p->filename_table_length) { return NULL; }
  else {
    return mrb_sym2name_len(p->mrb, p->filename_table[idx], NULL);
  }
}

#ifdef ENABLE_STDIO
parser_state*
mrb_parse_file(mrb_state *mrb, FILE *f, mrbc_context *c)
{
  parser_state *p;

  p = mrb_parser_new(mrb);
  if (!p) return 0;
  p->s = p->send = NULL;
  p->f = f;

  mrb_parser_parse(p, c);
  return p;
}
#endif

parser_state*
mrb_parse_nstring(mrb_state *mrb, const char *s, int len, mrbc_context *c)
{
  parser_state *p;

  p = mrb_parser_new(mrb);
  if (!p) return 0;
  p->s = s;
  p->send = s + len;

  mrb_parser_parse(p, c);
  return p;
}

parser_state*
mrb_parse_string(mrb_state *mrb, const char *s, mrbc_context *c)
{
  return mrb_parse_nstring(mrb, s, strlen(s), c);
}

static mrb_value
load_exec(mrb_state *mrb, parser_state *p, mrbc_context *c)
{
  struct RClass *target = mrb->object_class;
  struct RProc *proc;
  mrb_value v;

  if (!p) {
    return mrb_undef_value();
  }
  if (!p->tree || p->nerr) {
    if (p->capture_errors) {
      char buf[256];
      int n;

      n = snprintf(buf, sizeof(buf), "line %d: %s\n",
          p->error_buffer[0].lineno, p->error_buffer[0].message);
      mrb->exc = mrb_obj_ptr(mrb_exc_new(mrb, E_SYNTAX_ERROR, buf, n));
      mrb_parser_free(p);
      return mrb_undef_value();
    }
    else {
      mrb->exc = mrb_obj_ptr(mrb_exc_new_str_lit(mrb, E_SYNTAX_ERROR, "syntax error"));
      mrb_parser_free(p);
      return mrb_undef_value();
    }
  }
  proc = mrb_generate_code(mrb, p);
  mrb_parser_free(p);
  if (proc == NULL) {
    mrb->exc = mrb_obj_ptr(mrb_exc_new_str_lit(mrb, E_SCRIPT_ERROR, "codegen error"));
    return mrb_undef_value();
  }
  if (c) {
    if (c->dump_result) mrb_codedump_all(mrb, proc);
    if (c->no_exec) return mrb_obj_value(proc);
    if (c->target_class) {
      target = c->target_class;
    }
  }
  proc->target_class = target;
  if (mrb->c->ci) {
    mrb->c->ci->target_class = target;
  }
  v = mrb_toplevel_run(mrb, proc);
  if (mrb->exc) return mrb_nil_value();
  return v;
}

#ifdef ENABLE_STDIO
mrb_value
mrb_load_file_cxt(mrb_state *mrb, FILE *f, mrbc_context *c)
{
  return load_exec(mrb, mrb_parse_file(mrb, f, c), c);
}

mrb_value
mrb_load_file(mrb_state *mrb, FILE *f)
{
  return mrb_load_file_cxt(mrb, f, NULL);
}
#endif

mrb_value
mrb_load_nstring_cxt(mrb_state *mrb, const char *s, int len, mrbc_context *c)
{
  return load_exec(mrb, mrb_parse_nstring(mrb, s, len, c), c);
}

mrb_value
mrb_load_nstring(mrb_state *mrb, const char *s, int len)
{
  return mrb_load_nstring_cxt(mrb, s, len, NULL);
}

mrb_value
mrb_load_string_cxt(mrb_state *mrb, const char *s, mrbc_context *c)
{
  return mrb_load_nstring_cxt(mrb, s, strlen(s), c);
}

mrb_value
mrb_load_string(mrb_state *mrb, const char *s)
{
  return mrb_load_string_cxt(mrb, s, NULL);
}

#ifdef ENABLE_STDIO

static void
dump_prefix(int offset)
{
  while (offset--) {
    putc(' ', stdout);
    putc(' ', stdout);
  }
}

static void
dump_recur(mrb_state *mrb, node *tree, int offset)
{
  while (tree) {
    mrb_parser_dump(mrb, tree->car, offset);
    tree = tree->cdr;
  }
}

#endif

void
mrb_parser_dump(mrb_state *mrb, node *tree, int offset)
{
#ifdef ENABLE_STDIO
  int n;

  if (!tree) return;
  again:
  dump_prefix(offset);
  n = (int)(intptr_t)tree->car;
  tree = tree->cdr;
  switch (n) {
  case NODE_BEGIN:
    printf("NODE_BEGIN:\n");
    dump_recur(mrb, tree, offset+1);
    break;

  case NODE_RESCUE:
    printf("NODE_RESCUE:\n");
    if (tree->car) {
      dump_prefix(offset+1);
      printf("body:\n");
      mrb_parser_dump(mrb, tree->car, offset+2);
    }
    tree = tree->cdr;
    if (tree->car) {
      node *n2 = tree->car;

      dump_prefix(offset+1);
      printf("rescue:\n");
      while (n2) {
        node *n3 = n2->car;
        if (n3->car) {
          dump_prefix(offset+2);
          printf("handle classes:\n");
          dump_recur(mrb, n3->car, offset+3);
        }
        if (n3->cdr->car) {
          dump_prefix(offset+2);
          printf("exc_var:\n");
          mrb_parser_dump(mrb, n3->cdr->car, offset+3);
        }
        if (n3->cdr->cdr->car) {
          dump_prefix(offset+2);
          printf("rescue body:\n");
          mrb_parser_dump(mrb, n3->cdr->cdr->car, offset+3);
        }
        n2 = n2->cdr;
      }
    }
    tree = tree->cdr;
    if (tree->car) {
      dump_prefix(offset+1);
      printf("else:\n");
      mrb_parser_dump(mrb, tree->car, offset+2);
    }
    break;

  case NODE_ENSURE:
    printf("NODE_ENSURE:\n");
    dump_prefix(offset+1);
    printf("body:\n");
    mrb_parser_dump(mrb, tree->car, offset+2);
    dump_prefix(offset+1);
    printf("ensure:\n");
    mrb_parser_dump(mrb, tree->cdr->cdr, offset+2);
    break;

  case NODE_LAMBDA:
    printf("NODE_BLOCK:\n");
    goto block;

  case NODE_BLOCK:
    block:
    printf("NODE_BLOCK:\n");
  tree = tree->cdr;
  if (tree->car) {
    node *n = tree->car;

    if (n->car) {
      dump_prefix(offset+1);
      printf("mandatory args:\n");
      dump_recur(mrb, n->car, offset+2);
    }
    n = n->cdr;
    if (n->car) {
      dump_prefix(offset+1);
      printf("optional args:\n");
      {
        node *n2 = n->car;

        while (n2) {
          dump_prefix(offset+2);
          printf("%s=", mrb_sym2name(mrb, sym(n2->car->car)));
          mrb_parser_dump(mrb, n2->car->cdr, 0);
          n2 = n2->cdr;
        }
      }
    }
    n = n->cdr;
    if (n->car) {
      dump_prefix(offset+1);
      printf("rest=*%s\n", mrb_sym2name(mrb, sym(n->car)));
    }
    n = n->cdr;
    if (n->car) {
      dump_prefix(offset+1);
      printf("post mandatory args:\n");
      dump_recur(mrb, n->car, offset+2);
    }
    n = n->cdr;
    if (n) {
      dump_prefix(offset+1);
      printf("blk=&%s\n", mrb_sym2name(mrb, sym(n)));
    }
  }
  dump_prefix(offset+1);
  printf("body:\n");
  mrb_parser_dump(mrb, tree->cdr->car, offset+2);
  break;

  case NODE_IF:
    printf("NODE_IF:\n");
    dump_prefix(offset+1);
    printf("cond:\n");
    mrb_parser_dump(mrb, tree->car, offset+2);
    dump_prefix(offset+1);
    printf("then:\n");
    mrb_parser_dump(mrb, tree->cdr->car, offset+2);
    if (tree->cdr->cdr->car) {
      dump_prefix(offset+1);
      printf("else:\n");
      mrb_parser_dump(mrb, tree->cdr->cdr->car, offset+2);
    }
    break;

  case NODE_AND:
    printf("NODE_AND:\n");
    mrb_parser_dump(mrb, tree->car, offset+1);
    mrb_parser_dump(mrb, tree->cdr, offset+1);
    break;

  case NODE_OR:
    printf("NODE_OR:\n");
    mrb_parser_dump(mrb, tree->car, offset+1);
    mrb_parser_dump(mrb, tree->cdr, offset+1);
    break;

  case NODE_CASE:
    printf("NODE_CASE:\n");
    if (tree->car) {
      mrb_parser_dump(mrb, tree->car, offset+1);
    }
    tree = tree->cdr;
    while (tree) {
      dump_prefix(offset+1);
      printf("case:\n");
      dump_recur(mrb, tree->car->car, offset+2);
      dump_prefix(offset+1);
      printf("body:\n");
      mrb_parser_dump(mrb, tree->car->cdr, offset+2);
      tree = tree->cdr;
    }
    break;

  case NODE_WHILE:
    printf("NODE_WHILE:\n");
    dump_prefix(offset+1);
    printf("cond:\n");
    mrb_parser_dump(mrb, tree->car, offset+2);
    dump_prefix(offset+1);
    printf("body:\n");
    mrb_parser_dump(mrb, tree->cdr, offset+2);
    break;

  case NODE_UNTIL:
    printf("NODE_UNTIL:\n");
    dump_prefix(offset+1);
    printf("cond:\n");
    mrb_parser_dump(mrb, tree->car, offset+2);
    dump_prefix(offset+1);
    printf("body:\n");
    mrb_parser_dump(mrb, tree->cdr, offset+2);
    break;

  case NODE_FOR:
    printf("NODE_FOR:\n");
    dump_prefix(offset+1);
    printf("var:\n");
    {
      node *n2 = tree->car;

      if (n2->car) {
        dump_prefix(offset+2);
        printf("pre:\n");
        dump_recur(mrb, n2->car, offset+3);
      }
      n2 = n2->cdr;
      if (n2) {
        if (n2->car) {
          dump_prefix(offset+2);
          printf("rest:\n");
          mrb_parser_dump(mrb, n2->car, offset+3);
        }
        n2 = n2->cdr;
        if (n2) {
          if (n2->car) {
            dump_prefix(offset+2);
            printf("post:\n");
            dump_recur(mrb, n2->car, offset+3);
          }
        }
      }
    }
    tree = tree->cdr;
    dump_prefix(offset+1);
    printf("in:\n");
    mrb_parser_dump(mrb, tree->car, offset+2);
    tree = tree->cdr;
    dump_prefix(offset+1);
    printf("do:\n");
    mrb_parser_dump(mrb, tree->car, offset+2);
    break;

  case NODE_SCOPE:
    printf("NODE_SCOPE:\n");
    {
      node *n2 = tree->car;

      if (n2  && (n2->car || n2->cdr)) {
        dump_prefix(offset+1);
        printf("local variables:\n");
        dump_prefix(offset+2);
        while (n2) {
          if (n2->car) {
            if (n2 != tree->car) printf(", ");
            printf("%s", mrb_sym2name(mrb, sym(n2->car)));
          }
          n2 = n2->cdr;
        }
        printf("\n");
      }
    }
    tree = tree->cdr;
    offset++;
    goto again;

  case NODE_FCALL:
  case NODE_CALL:
    printf("NODE_CALL:\n");
    mrb_parser_dump(mrb, tree->car, offset+1);
    dump_prefix(offset+1);
    printf("method='%s' (%d)\n",
        mrb_sym2name(mrb, sym(tree->cdr->car)),
        (int)(intptr_t)tree->cdr->car);
    tree = tree->cdr->cdr->car;
    if (tree) {
      dump_prefix(offset+1);
      printf("args:\n");
      dump_recur(mrb, tree->car, offset+2);
      if (tree->cdr) {
        dump_prefix(offset+1);
        printf("block:\n");
        mrb_parser_dump(mrb, tree->cdr, offset+2);
      }
    }
    break;

  case NODE_DOT2:
    printf("NODE_DOT2:\n");
    mrb_parser_dump(mrb, tree->car, offset+1);
    mrb_parser_dump(mrb, tree->cdr, offset+1);
    break;

  case NODE_DOT3:
    printf("NODE_DOT3:\n");
    mrb_parser_dump(mrb, tree->car, offset+1);
    mrb_parser_dump(mrb, tree->cdr, offset+1);
    break;

  case NODE_COLON2:
    printf("NODE_COLON2:\n");
    mrb_parser_dump(mrb, tree->car, offset+1);
    dump_prefix(offset+1);
    printf("::%s\n", mrb_sym2name(mrb, sym(tree->cdr)));
    break;

  case NODE_COLON3:
    printf("NODE_COLON3:\n");
    dump_prefix(offset+1);
    printf("::%s\n", mrb_sym2name(mrb, sym(tree)));
    break;

  case NODE_ARRAY:
    printf("NODE_ARRAY:\n");
    dump_recur(mrb, tree, offset+1);
    break;

  case NODE_HASH:
    printf("NODE_HASH:\n");
    while (tree) {
      dump_prefix(offset+1);
      printf("key:\n");
      mrb_parser_dump(mrb, tree->car->car, offset+2);
      dump_prefix(offset+1);
      printf("value:\n");
      mrb_parser_dump(mrb, tree->car->cdr, offset+2);
      tree = tree->cdr;
    }
    break;

  case NODE_SPLAT:
    printf("NODE_SPLAT:\n");
    mrb_parser_dump(mrb, tree, offset+1);
    break;

  case NODE_ASGN:
    printf("NODE_ASGN:\n");
    dump_prefix(offset+1);
    printf("lhs:\n");
    mrb_parser_dump(mrb, tree->car, offset+2);
    dump_prefix(offset+1);
    printf("rhs:\n");
    mrb_parser_dump(mrb, tree->cdr, offset+2);
    break;

  case NODE_MASGN:
    printf("NODE_MASGN:\n");
    dump_prefix(offset+1);
    printf("mlhs:\n");
    {
      node *n2 = tree->car;

      if (n2->car) {
        dump_prefix(offset+2);
        printf("pre:\n");
        dump_recur(mrb, n2->car, offset+3);
      }
      n2 = n2->cdr;
      if (n2) {
        if (n2->car) {
          dump_prefix(offset+2);
          printf("rest:\n");
          if (n2->car == (node*)-1) {
            dump_prefix(offset+2);
            printf("(empty)\n");
          }
          else {
            mrb_parser_dump(mrb, n2->car, offset+3);
          }
        }
        n2 = n2->cdr;
        if (n2) {
          if (n2->car) {
            dump_prefix(offset+2);
            printf("post:\n");
            dump_recur(mrb, n2->car, offset+3);
          }
        }
      }
    }
    dump_prefix(offset+1);
    printf("rhs:\n");
    mrb_parser_dump(mrb, tree->cdr, offset+2);
    break;

  case NODE_OP_ASGN:
    printf("NODE_OP_ASGN:\n");
    dump_prefix(offset+1);
    printf("lhs:\n");
    mrb_parser_dump(mrb, tree->car, offset+2);
    tree = tree->cdr;
    dump_prefix(offset+1);
    printf("op='%s' (%d)\n", mrb_sym2name(mrb, sym(tree->car)), (int)(intptr_t)tree->car);
    tree = tree->cdr;
    mrb_parser_dump(mrb, tree->car, offset+1);
    break;

  case NODE_SUPER:
    printf("NODE_SUPER:\n");
    if (tree) {
      dump_prefix(offset+1);
      printf("args:\n");
      dump_recur(mrb, tree->car, offset+2);
      if (tree->cdr) {
        dump_prefix(offset+1);
        printf("block:\n");
        mrb_parser_dump(mrb, tree->cdr, offset+2);
      }
    }
    break;

  case NODE_ZSUPER:
    printf("NODE_ZSUPER\n");
    break;

  case NODE_RETURN:
    printf("NODE_RETURN:\n");
    mrb_parser_dump(mrb, tree, offset+1);
    break;

  case NODE_YIELD:
    printf("NODE_YIELD:\n");
    dump_recur(mrb, tree, offset+1);
    break;

  case NODE_BREAK:
    printf("NODE_BREAK:\n");
    mrb_parser_dump(mrb, tree, offset+1);
    break;

  case NODE_NEXT:
    printf("NODE_NEXT:\n");
    mrb_parser_dump(mrb, tree, offset+1);
    break;

  case NODE_REDO:
    printf("NODE_REDO\n");
    break;

  case NODE_RETRY:
    printf("NODE_RETRY\n");
    break;

  case NODE_LVAR:
    printf("NODE_LVAR %s\n", mrb_sym2name(mrb, sym(tree)));
    break;

  case NODE_GVAR:
    printf("NODE_GVAR %s\n", mrb_sym2name(mrb, sym(tree)));
    break;

  case NODE_IVAR:
    printf("NODE_IVAR %s\n", mrb_sym2name(mrb, sym(tree)));
    break;

  case NODE_CVAR:
    printf("NODE_CVAR %s\n", mrb_sym2name(mrb, sym(tree)));
    break;

  case NODE_CONST:
    printf("NODE_CONST %s\n", mrb_sym2name(mrb, sym(tree)));
    break;

  case NODE_MATCH:
    printf("NODE_MATCH:\n");
    dump_prefix(offset + 1);
    printf("lhs:\n");
    mrb_parser_dump(mrb, tree->car, offset + 2);
    dump_prefix(offset + 1);
    printf("rhs:\n");
    mrb_parser_dump(mrb, tree->cdr, offset + 2);
    break;

  case NODE_BACK_REF:
    printf("NODE_BACK_REF: $%c\n", (int)(intptr_t)tree);
    break;

  case NODE_NTH_REF:
    printf("NODE_NTH_REF: $%d\n", (int)(intptr_t)tree);
    break;

  case NODE_ARG:
    printf("NODE_ARG %s\n", mrb_sym2name(mrb, sym(tree)));
    break;

  case NODE_BLOCK_ARG:
    printf("NODE_BLOCK_ARG:\n");
    mrb_parser_dump(mrb, tree, offset+1);
    break;

  case NODE_INT:
    printf("NODE_INT %s base %d\n", (char*)tree->car, (int)(intptr_t)tree->cdr->car);
    break;

  case NODE_FLOAT:
    printf("NODE_FLOAT %s\n", (char*)tree);
    break;

  case NODE_NEGATE:
    printf("NODE_NEGATE\n");
    mrb_parser_dump(mrb, tree, offset+1);
    break;

  case NODE_STR:
    printf("NODE_STR \"%s\" len %d\n", (char*)tree->car, (int)(intptr_t)tree->cdr);
    break;

  case NODE_DSTR:
    printf("NODE_DSTR\n");
    dump_recur(mrb, tree, offset+1);
    break;

  case NODE_XSTR:
    printf("NODE_XSTR \"%s\" len %d\n", (char*)tree->car, (int)(intptr_t)tree->cdr);
    break;

  case NODE_DXSTR:
    printf("NODE_DXSTR\n");
    dump_recur(mrb, tree, offset+1);
    break;

  case NODE_REGX:
    printf("NODE_REGX /%s/%s\n", (char*)tree->car, (char*)tree->cdr);
    break;

  case NODE_DREGX:
    printf("NODE_DREGX\n");
    dump_recur(mrb, tree->car, offset+1);
    dump_prefix(offset);
    printf("tail: %s\n", (char*)tree->cdr->cdr->car);
    dump_prefix(offset);
    printf("opt: %s\n", (char*)tree->cdr->cdr->cdr);
    break;

  case NODE_SYM:
    printf("NODE_SYM :%s\n", mrb_sym2name(mrb, sym(tree)));
    break;

  case NODE_SELF:
    printf("NODE_SELF\n");
    break;

  case NODE_NIL:
    printf("NODE_NIL\n");
    break;

  case NODE_TRUE:
    printf("NODE_TRUE\n");
    break;

  case NODE_FALSE:
    printf("NODE_FALSE\n");
    break;

  case NODE_ALIAS:
    printf("NODE_ALIAS %s %s:\n",
        mrb_sym2name(mrb, sym(tree->car)),
        mrb_sym2name(mrb, sym(tree->cdr)));
    break;

  case NODE_UNDEF:
    printf("NODE_UNDEF");
    {
      node *t = tree;
      while (t) {
        printf(" %s", mrb_sym2name(mrb, sym(t->car)));
        t = t->cdr;
      }
    }
    printf(":\n");
    break;

  case NODE_CLASS:
    printf("NODE_CLASS:\n");
    if (tree->car->car == (node*)0) {
      dump_prefix(offset+1);
      printf(":%s\n", mrb_sym2name(mrb, sym(tree->car->cdr)));
    }
    else if (tree->car->car == (node*)1) {
      dump_prefix(offset+1);
      printf("::%s\n", mrb_sym2name(mrb, sym(tree->car->cdr)));
    }
    else {
      mrb_parser_dump(mrb, tree->car->car, offset+1);
      dump_prefix(offset+1);
      printf("::%s\n", mrb_sym2name(mrb, sym(tree->car->cdr)));
    }
    if (tree->cdr->car) {
      dump_prefix(offset+1);
      printf("super:\n");
      mrb_parser_dump(mrb, tree->cdr->car, offset+2);
    }
    dump_prefix(offset+1);
    printf("body:\n");
    mrb_parser_dump(mrb, tree->cdr->cdr->car->cdr, offset+2);
    break;

  case NODE_MODULE:
    printf("NODE_MODULE:\n");
    if (tree->car->car == (node*)0) {
      dump_prefix(offset+1);
      printf(":%s\n", mrb_sym2name(mrb, sym(tree->car->cdr)));
    }
    else if (tree->car->car == (node*)1) {
      dump_prefix(offset+1);
      printf("::%s\n", mrb_sym2name(mrb, sym(tree->car->cdr)));
    }
    else {
      mrb_parser_dump(mrb, tree->car->car, offset+1);
      dump_prefix(offset+1);
      printf("::%s\n", mrb_sym2name(mrb, sym(tree->car->cdr)));
    }
    dump_prefix(offset+1);
    printf("body:\n");
    mrb_parser_dump(mrb, tree->cdr->car->cdr, offset+2);
    break;

  case NODE_SCLASS:
    printf("NODE_SCLASS:\n");
    mrb_parser_dump(mrb, tree->car, offset+1);
    dump_prefix(offset+1);
    printf("body:\n");
    mrb_parser_dump(mrb, tree->cdr->car->cdr, offset+2);
    break;

  case NODE_DEF:
    printf("NODE_DEF:\n");
    dump_prefix(offset+1);
    printf("%s\n", mrb_sym2name(mrb, sym(tree->car)));
    tree = tree->cdr;
    {
      node *n2 = tree->car;

      if (n2 && (n2->car || n2->cdr)) {
        dump_prefix(offset+1);
        printf("local variables:\n");
        dump_prefix(offset+2);
        while (n2) {
          if (n2->car) {
            if (n2 != tree->car) printf(", ");
            printf("%s", mrb_sym2name(mrb, sym(n2->car)));
          }
          n2 = n2->cdr;
        }
        printf("\n");
      }
    }
    tree = tree->cdr;
    if (tree->car) {
      node *n = tree->car;

      if (n->car) {
        dump_prefix(offset+1);
        printf("mandatory args:\n");
        dump_recur(mrb, n->car, offset+2);
      }
      n = n->cdr;
      if (n->car) {
        dump_prefix(offset+1);
        printf("optional args:\n");
        {
          node *n2 = n->car;

          while (n2) {
            dump_prefix(offset+2);
            printf("%s=", mrb_sym2name(mrb, sym(n2->car->car)));
            mrb_parser_dump(mrb, n2->car->cdr, 0);
            n2 = n2->cdr;
          }
        }
      }
      n = n->cdr;
      if (n->car) {
        dump_prefix(offset+1);
        printf("rest=*%s\n", mrb_sym2name(mrb, sym(n->car)));
      }
      n = n->cdr;
      if (n->car) {
        dump_prefix(offset+1);
        printf("post mandatory args:\n");
        dump_recur(mrb, n->car, offset+2);
      }
      n = n->cdr;
      if (n) {
        dump_prefix(offset+1);
        printf("blk=&%s\n", mrb_sym2name(mrb, sym(n)));
      }
    }
    mrb_parser_dump(mrb, tree->cdr->car, offset+1);
    break;

  case NODE_SDEF:
    printf("NODE_SDEF:\n");
    mrb_parser_dump(mrb, tree->car, offset+1);
    tree = tree->cdr;
    dump_prefix(offset+1);
    printf(":%s\n", mrb_sym2name(mrb, sym(tree->car)));
    tree = tree->cdr->cdr;
    if (tree->car) {
      node *n = tree->car;

      if (n->car) {
        dump_prefix(offset+1);
        printf("mandatory args:\n");
        dump_recur(mrb, n->car, offset+2);
      }
      n = n->cdr;
      if (n->car) {
        dump_prefix(offset+1);
        printf("optional args:\n");
        {
          node *n2 = n->car;

          while (n2) {
            dump_prefix(offset+2);
            printf("%s=", mrb_sym2name(mrb, sym(n2->car->car)));
            mrb_parser_dump(mrb, n2->car->cdr, 0);
            n2 = n2->cdr;
          }
        }
      }
      n = n->cdr;
      if (n->car) {
        dump_prefix(offset+1);
        printf("rest=*%s\n", mrb_sym2name(mrb, sym(n->car)));
      }
      n = n->cdr;
      if (n->car) {
        dump_prefix(offset+1);
        printf("post mandatory args:\n");
        dump_recur(mrb, n->car, offset+2);
      }
      n = n->cdr;
      if (n) {
        dump_prefix(offset+1);
        printf("blk=&%s\n", mrb_sym2name(mrb, sym(n)));
      }
    }
    tree = tree->cdr;
    mrb_parser_dump(mrb, tree->car, offset+1);
    break;

  case NODE_POSTEXE:
    printf("NODE_POSTEXE:\n");
    mrb_parser_dump(mrb, tree, offset+1);
    break;

  case NODE_HEREDOC:
    printf("NODE_HEREDOC:\n");
    mrb_parser_dump(mrb, ((parser_heredoc_info*)tree)->doc, offset+1);
    break;

  default:
    printf("node type: %d (0x%x)\n", (int)n, (int)n);
    break;
  }
#endif
}
