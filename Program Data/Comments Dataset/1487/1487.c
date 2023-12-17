/*
This main function is responsible for running a set of unit tests and returning an appropriate exit status based on the test results.

Here's the detail:

- It initializes a Suite pointer and a SRunner pointer for organizing and executing the unit tests.
- The qfloat_suite() function is called to obtain the test suite, which presumably contains a set of unit tests related to floating-point operations.
- The srunner_run_all() function is used to execute all the tests within the test suite with normal test mode (CK_NORMAL).
- The number of failed tests is obtained using srunner_ntests_failed() to determine the overall test result.
- The SRunner object is freed before the function concludes, releasing allocated memory.

Needs to notice:

- This main function is specifically designed to work with the Check unit testing framework, as indicated by the function calls such as srunner_create(), srunner_run_all(), and srunner_ntests_failed().
- The return value is based on the number of failed tests, where EXIT_SUCCESS is returned if no tests fail, and EXIT_FAILURE is returned if there are any failed tests.
- It's crucial to ensure that the qfloat_suite() function has been correctly implemented to populate the test suite with relevant unit tests, and that the Check framework is properly configured for accurate test execution.
*/
int main(void)

{

    int nf;

    Suite *s;

    SRunner *sr;



    s = qfloat_suite();

    sr = srunner_create(s);



    srunner_run_all(sr, CK_NORMAL);

    nf = srunner_ntests_failed(sr);

    srunner_free(sr);



    return (nf == 0) ? EXIT_SUCCESS : EXIT_FAILURE;

}
