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
#define WES_ARG_VERSION 1 << 2

#define WES_MAX_LINE_SIZE	8192
#define WES_MAX_TOKEN_SIZE	128

#define WES_TOKEN_DELIMIT " .,;-\n\t"

#define WES_NAME "wes"
#define WES_VER_MAJOR 0
#define WES_VER_MINOR 1

/*
 * Type definitions
 */

/*
 * Each token holds a line list
 * which hold a line number
 * and the number of ocurrences
 * for the token on that line
 */
typedef struct _line_t
{
    int number; // line number
    int times; // Number of ocurrences
    struct _line_t *next;
} line_t;

/*
 * Each new found token is hold by
 * instances of token_t
 */
typedef struct _token_t
{
    char str[WES_MAX_TOKEN_SIZE]; // string
    line_t  *lines; // a list of the lines where this token was found
    struct _token_t *left; // left wing children
    struct _token_t *right; // right wing children
} token_t;

// Token b-tree root
token_t *g_btree =  NULL;

// A word holding argument flags
unsigned char g_args = 0;

// This holds the total number of tokens
// found on file
unsigned int  g_token_count = 0;

// A string holding the file name to be tokenized
char *g_filename;

/*
 * Parse command line arguments
 */
void
wes_parsecmdline ( int argc, char ** argv )
{
    int i; // counter

    if ( argc == 1 ) {
        // Activate help flag
        g_args |= WES_ARG_HELP;
        return;
    }

    // Process every argument in the command line
    for ( i = 1; i < argc; i++ )
    {
        // Help
        if ( !strcmp("--help", argv[i]) )
            g_args |= WES_ARG_HELP;

        // Verbose mode
        else if ( !strcmp("--verbose", argv[i]) )
            g_args |= WES_ARG_VERBOSE;

        // Print version
        else if ( !strcmp("--version", argv[i]) )
            g_args |= WES_ARG_VERSION;

        // Everything else will be considered as the file name
        else g_filename = argv[i];
    }
}

/*
 * Print help information
 */
void
wes_help ()
{
    printf(
        "usage: wes <filename> <args>\n"
        "Arguments can be:\n"
        "--help\t\tPrint this help and exit\n"
        "--verbose\trun with debug info\n"
        );
}

/*
 * Print version and license info
 */
void
wes_version ()
{
    printf (
        "%s %d.%d\n"
        "Copyright (C) 2005 Alejandro Ricoveri\n"
        "This is free software; see the source for "
        "copying conditions.  There is NO\n"
        "warranty; not even for MERCHANTABILITY or "
        "FITNESS FOR A PARTICULAR PURPOSE.\n"
        ,WES_NAME, WES_VER_MAJOR, WES_VER_MINOR
        );
}

/*
 * Create a new line
 */
line_t *
wes_line_create ( int number )
{
    // Allocate some mem for the new line
    line_t *new_line = malloc(sizeof(line_t));

    // Initial data might have ramdon bytes on it
    // so, we set it up right, fill it with zeros
    memset((void *)new_line, 0, sizeof(line_t));

    // Assign a number for this line
    new_line->number = number;

    // Initial ocurrence for this line is 1
    new_line->times = 1;

    // ... and we're done
    return new_line;
}

/*
 * Delete a list of lines
 * A recursive algorithm
 */
void
wes_line_delete (line_t *line)
{
    // First, we delete the next line
    // before deleting this one
    if (line->next)
        wes_line_delete(line->next);

    // And that's it, we proceed to free that mem
    free(line);
}

/*
 * Append a line to a token line list
 */
void
wes_line_insert ( line_t **line_ptr, int number )
{
    if (*line_ptr)
    {
        if ((*line_ptr)->number == number)
            // At this point, the token has been found
            // more than once on this line number
            (*line_ptr)->times++;

        // There is no other ocurrence for the token
        // on this line number, so we try to seek for
        // it on the next one
        else wes_line_insert(&((*line_ptr))->next, number);
    }

    // At this point, the token has been found
    // for the first time on this line number
    else *line_ptr = wes_line_create(number);
}

/*
 * Create a new token
 */
token_t *
wes_token_create ( char *token_str, int number )
{
    // Allocate some mem for the new token
    token_t *new_token = malloc(sizeof(token_t));

    // And set it up first
    memset((void *)new_token, 0, sizeof(token_t));

    // Copy the string to the new token
    strcpy(new_token->str, token_str);

    // There's automatically a first ocurrence
    // of this token on this line number
    new_token->lines = wes_line_create (number);

    // Another token to the global count!
    g_token_count++;

    // ... and we're done
    return new_token;
}

/*
 * Delete a token (and all of his children)
 * A recursive algorithm
 */
void
wes_token_delete ( token_t *token )
{
    if (token)
    {
        // First things first,
        // Delete left wing children
        wes_token_delete(token->left);

        // Delete right wing children
        wes_token_delete(token->right);

        // Deletes the line list for this token
        wes_line_delete(token->lines);

        // ... And then, we can finally free this mem
        free((void *)token);
    }
}

/*
 * Append a token to the token b-tree
 */
void
wes_token_insert ( token_t **token_ptr, char *token_str, int line )
{
    if (*token_ptr)
    {
        int cmp ; // It hold string delta

        if (!(cmp = strcmp(token_str, (*token_ptr)->str)))
            // Token already exists on the b-tree, so
            // we proceed to append a new line ocurrence
            // for this token
            wes_line_insert (&((*token_ptr))->lines, line);

        // Token may be not found here
        // Try to insert into whether its right or left wing
        else if (cmp < 0)
            wes_token_insert(&(*token_ptr)->left, token_str, line);
        else
            wes_token_insert(&(*token_ptr)->right, token_str, line);
    }

    // At this point, token doesn't exists
    // So, we create it ...
    else *token_ptr = wes_token_create(token_str, line);
}

/*
 * Print the b-tree contents
 * A recursive algorithm
 */
void
wes_log_tree ( token_t *token )
{
    if (token)
    {
        line_t *line;

        // First things first,

        // We print left children contents
        wes_log_tree(token->left);

        // Print this token's info
        printf ("* %s: ", token->str);
        for (line = token->lines; line != NULL; line = line->next)
            printf (" %d(%d)", line->number, line->times);
        printf("\n");

        // Print right wing children info
        wes_log_tree(token->right);
    }
}

/*
 * Print error
 */
int
wes_error ( char *fmt, ... )
{
    va_list ap;

    va_start (ap, fmt);
        fprintf (stderr, "Error: ");
        vfprintf(stderr, fmt, ap);
        fprintf (stderr, "\n");
    va_end(ap);

    return -1;
}

/*
 * Read the file contents
 * An iterative algorithm
 */
int
wes_readfile ()
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
int
main ( int argc, char *argv[] )
{
    // Parse each argument in the command line
    wes_parsecmdline (argc ,argv);

    if (g_args & WES_ARG_HELP)
    {
        // Print help and exit
        wes_help();
        return EXIT_SUCCESS;
    }

    if (g_args & WES_ARG_VERSION)
    {
        // Print version and exit
        wes_version();
        return EXIT_SUCCESS;
    }

    // Else, we read the file then ...
    return wes_readfile();
}
