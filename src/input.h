#pragma once

#include "klangc.h"
#include <assert.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

// *******************************
// Input.
// *******************************
// -------------------------------
// Structures.
// -------------------------------
/**
 * Input.
 */
typedef struct klangc_input klangc_input_t;

// -------------------------------
// Constructors.
// -------------------------------
/**
 * Make a new input.
 * @param fp File pointer.
 * @param name Name.
 * @return New input.
 */
klangc_input_t *klangc_input_new(FILE *fp, const char *name);

// -------------------------------
// Destructors.
// -------------------------------
/**
 * Free an input.
 * @param input Input.
 */
void klangc_input_free(klangc_input_t *input);

// -------------------------------
// Accessors.
// -------------------------------
/**
 * Get the name of an input.
 * @param input Input.
 * @return Name.
 */
const char *klangc_input_get_name(klangc_input_t *input);

/**
 * Get the position of an input.
 * @param input Input.
 * @return File pointer.
 */
klangc_ipos_t klangc_input_save(klangc_input_t *input);

/**
 * Restore the position of an input.
 * @param input Input.
 * @param ipos Position.
 */
void klangc_input_restore(klangc_input_t *input, klangc_ipos_t ipos);

/**
 * Get the next char of an input.
 * @param input Input.
 * @return Next char.
 */
int klangc_getc(klangc_input_t *input);

/**
 * Test if the input is spaces.
 * @param c Char.
 * @param in_comment In comment.
 * @return True if the input is spaces.
 */
int klangc_isspace(int c, int *in_comment);

/**
 * Get the next char of an input after skip spaces.
 * @param input Input.
 * @return Next char.
 */
int klangc_getc_skipspaces(klangc_input_t *input);

/**
 * Get the position of an input after skip spaces.
 * @param input Input.
 * @return Position.
 */
klangc_ipos_t klangc_skipspaces(klangc_input_t *input);

// -------------------------------
// Printers.
// -------------------------------
/**
 * Print an input position.
 * @param output Output.
 * @param ipos Position.
 */
int klangc_ipos_print(klangc_output_t *output, klangc_ipos_t ipos);