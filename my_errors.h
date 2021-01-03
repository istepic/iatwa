#ifndef MY_ERRORS_H
#define MY_ERRORS_H

enum
{
    internal_error = 1000000,
    passed_null_pointer,
    crc_check_failed,
    reached_default,
} my_error_enum;

#endif