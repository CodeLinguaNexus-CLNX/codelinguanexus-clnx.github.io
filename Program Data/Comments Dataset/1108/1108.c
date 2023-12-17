/*
This function is responsible for testing the behavior of the qemu_strtoull function when encountering an overflow condition.

Here's the detail:
The function begins by initializing a string, str, with a value that represents an extremely large number. It also declares a character variable, f, and assigns it the value 'X'. Additionally, it initializes a pointer variable, endptr, to point to the address of f. A uint64_t variable, res, is initialized with the value 999. An int variable, err, is also declared.

The qemu_strtoull function is then called, passing the string, endptr, a base of 0, and the address of res as arguments. This function is expected to convert the string to an unsigned long long integer, updating the value of res with the result and the endptr with the address of the first invalid character encountered during the conversion.

The g_assert_cmpint macro is used to verify the expected behavior of qemu_strtoull. It checks that the value of err is equal to -ERANGE, indicating an overflow error. It also checks that the value of res is equal to ULLONG_MAX, which represents the maximum value that can be stored in an unsigned long long integer. Finally, it verifies that endptr is pointing to the end of the string, as determined by comparing its value to str + strlen(str).

Need's to notice:
This test case aims to verify the correct handling of overflow conditions by the qemu_strtoull function. Overflow occurs when the input string represents a number that is too large to be stored in a uint64_t variable. The expected behavior in this case is for qemu_strtoull to return -ERANGE and to set res to the maximum possible value. The endptr should also point to the end of the input string.
*/
static void test_qemu_strtoull_overflow(void)

{

    const char *str = "99999999999999999999999999999999999999999999";

    char f = 'X';

    const char *endptr = &f;

    uint64_t res = 999;

    int err;



    err = qemu_strtoull(str, &endptr, 0, &res);



    g_assert_cmpint(err, ==, -ERANGE);

    g_assert_cmpint(res, ==, ULLONG_MAX);

    g_assert(endptr == str + strlen(str));

}
