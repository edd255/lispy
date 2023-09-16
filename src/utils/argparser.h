/// @file argparser.h
/// @brief This file contains methods to parse command-line arguments.

#pragma once

#ifndef LISPY_UTILS_ARGPARSE_H
#define LISPY_UTILS_ARGPARSE_H

/// @brief Sets up the command-line argument parser.
///
/// This function sets up the command-line argument parser. In case, that wrong
/// arguments are given, the usage is repeated.
/// @param argc The number of arguments
/// @param argv The array containing the arguments
void parse_args(int argc, const char** argv);

/// Getter for filename arg
char* get_filename(void);

/// Getter for no_stdlib arg
int get_nostdlib(void);

/// Getter for print_logs arg
int get_print_logs(void);

#endif
