#ifndef MY_ERRORS_H
#define MY_ERRORS_H

enum Error
{
    internal_error = 1000000,
    passed_null_pointer,
    crc_check_failed,
} error;

#endif