/*
 * Copyright © 2005 Alejandro Ricoveri <alejandroricoveri@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#define WES_ARG_VERBOSE 1 << 0
#define WES_ARG_HELP 	1 << 1

#define WES_MAX_LINE_SIZE	8192
#define WES_MAX_TOKEN_SIZE	128

#define WES_TOKEN_DELIMIT " .,;-\n\t"

typedef struct _line_t
{
    int number;
    int times;
    struct _line_t *next;
} line_t;

typedef struct _token_t
{
    char str[WES_MAX_TOKEN_SIZE];
    line_t  *lines;
    struct _token_t *left;
    struct _token_t *right;
} token_t;

token_t *g_btree =  NULL;
unsigned char g_args = 0;
unsigned int  g_token_count = 0;
char *g_filename;

/*
 * Parse command line arguments
 */
void wes_parsecmdline ( int argc, char ** argv )
{
    int i;

    if (argc == 1) {
        g_args |= WES_ARG_HELP;
        return;
    }

    //while (argc > 1)
    for (i = 1; i < argc; i++)
    {
        if (!strcmp("--help", argv[i]))
        {
            g_args |= WES_ARG_HELP;
        }
        else if (!strcmp("--verbose", argv[i]))
            g_args |= WES_ARG_VERBOSE;
        else
            g_filename = argv[i];
    }
}


void wes_help ()
{
    printf(
		"usage: wes <filename> <args>\n"
		"Arguments can be:\n"
		"--help\t\tPrint this help and exit\n"
		"--verbose\trun with debug info\n"
		);
}

line_t * wes_line_create ( int line )
{
    line_t *new_line = malloc(sizeof(line_t));
    memset((void *)new_line, 0, sizeof(line_t));
    new_line->number = line;
    new_line->times = 1;
    return new_line;
}

void wes_line_delete (line_t *line)
{
    if (line->next)
        wes_line_delete(line->next);
    free(line);
}

void wes_line_insert (token_t *token, int line)
{
    line_t * cur_line;

    for (cur_line = token->lines; cur_line; cur_line = cur_line->next)
    {
        if (cur_line->number == line) {
            cur_line->times++;
            break;
        }

        if (!cur_line->next) {
            cur_line->next = wes_line_create(line);
            break;
        }
    }
}

token_t * wes_token_create ( char *token_str, int line )
{
    token_t *new_token = malloc(sizeof(token_t));
    memset((void *)new_token, 0, sizeof(token_t));

    strcpy(new_token->str, token_str);

    new_token->lines = wes_line_create (line);

    g_token_count++;

    return new_token;
}

void wes_token_delete ( token_t *token )
{
    if (token)
    {
        wes_token_delete(token->left);
        wes_token_delete(token->right);

        wes_line_delete(token->lines);

        free((void *)token);
    }
}

void wes_token_insert ( token_t **token_ptr, char *token_str, int line )
{
    int cmp ;

    if (!(*token_ptr))
        *token_ptr = wes_token_create(token_str, line);
    else if (!(cmp = strcmp(token_str, (*token_ptr)->str)))
        wes_line_insert (*token_ptr, line);
    else if (cmp < 0)
        wes_token_insert(&(*token_ptr)->left, token_str, line);
    else
        wes_token_insert(&(*token_ptr)->right, token_str, line);
}

void wes_log_tree ( token_t *token )
{
    if (token)
    {
        line_t *line;

        wes_log_tree(token->left);
        printf ("* %s: ", token->str);
        for (line = token->lines; line != NULL; line = line->next)
            printf (" %d(%d)", line->number, line->times);
        printf("\n");
        wes_log_tree(token->right);
    }
}

int wes_error ( char *fmt, ... )
{
    va_list ap;

    va_start (ap, fmt);
        fprintf (stderr, "Error: ");
        vfprintf(stderr, fmt, ap);
        fprintf (stderr, "\n");
    va_end(ap);

    return -1;
}

//void wes_log (char *fmt, ...)
//{
//    va_list ap;
//
//    if (g_args & WES_ARG_VERBOSE)
//    {
//        va_start (ap, fmt);
//            vprintf (fmt, ap);
//        va_end(ap);
//        printf("\n");
//    }
//}


int wes_readfile ()
{
    char *in_token = NULL; // This will hold each captured token
    char in_line[WES_MAX_LINE_SIZE]; // This will hold each line in the file

    // Current line number
    unsigned line;

    // The input file itself
    FILE *file;

    // Open the file
    if (!(file = fopen(g_filename, "r")))
		return wes_error("Couldn't open '%s'", g_filename);

    // Proceed to read each line and tokenize it
	for (line = 1; !feof(file); line++)
	{
        // Get the line as a string
		if (fgets(in_line, WES_MAX_LINE_SIZE, file))
		{
            // Capture each token and insert it
            // into the b-tree
            for (
                in_token = strtok (in_line, WES_TOKEN_DELIMIT);
                in_token != NULL;
                in_token = strtok (NULL, WES_TOKEN_DELIMIT))
            {
                // insert a (maybe) new token into the btree of tokens
                wes_token_insert (&g_btree, in_token, line);
            }
		}
    }

    // Print the tree contents
    printf("-- %s tokenizing statistics --\n", g_filename);
    wes_log_tree(g_btree);
    printf("%4d token(s) found\n", g_token_count);
    printf("-- End of execution --\n");

    // Do the house keeping!
    wes_token_delete(g_btree);

    // close the file
    fclose(file);

    // ... and we're done
    return 0;
}

/* Entry point */
int main ( int argc, char *argv[] )
{
    // Parse each argument in the command line
    wes_parsecmdline (argc ,argv);

    // Print help and exit the program
    if (g_args & WES_ARG_HELP)
    {
        // Print help and get out
        wes_help();
        return EXIT_SUCCESS;
    }

    // Else, we read the file then ...
    return wes_readfile();
}
