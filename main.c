#include <stdio.h>
#include <stdlib.h>

/* nfa types */
enum {
        NFA_MATCH, /* match nfa */
        NFA_CHAR,  /* regular character */
        NFA_COUNT, /* type count */
};

/* nfa */
struct nfa {
        struct nfa *n_edges[2]; /* nfa edges */
        int         n_type;     /* type */
        int         n_c;        /* if NFA_CHAR, character */
};

/* next available state */
static int g_next_state = 0;

/**
 * create a new nfa{}:
 *
 * args:
 *  @type: nfa type
 *
 * ret:
 *  @success: pointer to nfa{}
 *  @failure: die
 */
struct nfa *nfa_new(int type);

/**
 * allocate new nfa:
 *
 * args:
 *  @npp: pointer to pointer to nfa{}
 *
 * ret:
 *  @success: *npp set to NULL
 *  @failure: does not
 */
void nfa_free(struct nfa **npp);

/**
 * create a new char nfa{}:
 *
 * args:
 *  @c: character
 *
 * ret:
 *  @success: pointer to nfa{}
 *  @failure: die
 */
struct nfa *nfa_char_new(int c);

/**
 * create a new match nfa{}:
 *
 * args:
 *  none
 *
 * ret:
 *  @success: pointer to nfa{}
 *  @failure: die
 */
struct nfa *nfa_match_new(void);

/**
 * compile regex:
 *
 * args:
 *  @re: regex
 *
 * ret:
 *  @success: pointer to nfa{}
 *  @failure: die
 */
struct nfa *re_comp(const char *re);

/**
 * do compile regex:
 *
 * args:
 *  @spp: pointer to pointer to string
 *
 * ret:
 *  @success: pointer to nfa{}
 *  @failure: die
 */
struct nfa *re_do_comp(const char **spp);

/**
 * compile regex factor:
 *
 * args:
 *  @spp:     pointer to pointer to string
 *  @startpp: pointer to pointer to start nfa{}
 *  @endpp:   pointer to pointer to end nfa{}
 *
 * ret:
 *  @success: nothing
 *  @failure: die
 */
void re_comp_factor(const char **spp, struct nfa **startpp, struct nfa **endpp);

int
main(void)
{
        struct nfa *np = NULL;
        struct nfa *cur = NULL;
        struct nfa *next = NULL;

        np = re_comp("abc");

        for (cur = np; cur; cur = next) {
                next = cur->n_edges[0];

                if (cur->n_type == NFA_MATCH)
                        printf("MATCH\n");
                else
                        printf("%c\n", cur->n_c);

                nfa_free(&cur);
        }
}

struct nfa *
nfa_new(int type)
{
        struct nfa *np = NULL;

        np = calloc(1, sizeof(*np));
        if (np == NULL) {
                perror("nfa_new: calloc");
                exit(1);
        }

        np->n_type = type;
        g_next_state++;
        return np;
}

void
nfa_free(struct nfa **npp)
{
        free(*npp);
        *npp = NULL;
        g_next_state--;
}

struct nfa *
nfa_char_new(int c)
{
        struct nfa *np = NULL;

        np = nfa_new(NFA_CHAR);
        np->n_c = c;

        return np;
}

struct nfa *
nfa_match_new(void)
{
        return nfa_new(NFA_MATCH);
}

struct nfa *
re_comp(const char *re)
{
        return re_do_comp(&re);
}

struct nfa *
re_do_comp(const char **spp)
{
        struct nfa *start2 = NULL;
        struct nfa *end2 = NULL;
        struct nfa *start = NULL;
        struct nfa *end = NULL;
        struct nfa *cur = NULL;

        re_comp_factor(spp, &start, &end);

        cur = start;
        while (**spp) {
                re_comp_factor(spp, &start2, &end2);
                nfa_free(&end);
                cur->n_edges[0] = start2;
                cur = start2;
                end = end2;
        }

        return start;
}

void
re_comp_factor(const char **spp, struct nfa **startpp, struct nfa **endpp)
{
        struct nfa *start = NULL;
        struct nfa *end = NULL;

        start = nfa_char_new(**spp);
        end = nfa_match_new();
        start->n_edges[0] = end;

        *startpp = start;
        *endpp = end;
        (*spp)++;
}
