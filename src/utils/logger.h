/// @file argparser.h
/// @brief This file contains methods to parse command-line arguments.

#pragma once

#ifndef LISPY_UTILS_LOGGER_H
#define LISPY_UTILS_LOGGER_H

#include <stdio.h>

/// @brief Sets up the log-file in the XDG cache directory.
///
/// This function sets up the log-file in the XDG cache directory if the
/// directory exists.
///
/// \todo Make the function portable.
///
/// @return A pointer to the log file.
FILE* prepare_logfile(void);

#endif
