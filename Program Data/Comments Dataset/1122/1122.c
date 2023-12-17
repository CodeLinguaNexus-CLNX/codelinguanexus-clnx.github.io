/*
This function is responsible for outputting a numerical value of type double to the QMP (QEMU Monitor Protocol) as part of the visitor pattern implementation.

Here's the detail:
The function takes a Visitor object, a name for the output, a pointer to the double value, and a pointer to an Error object. It then casts the Visitor object to the QmpOutputVisitor type and uses the qmp_output_add function to add the numerical value to the QMP output by converting it into the appropriate QMP format using the qfloat_from_double function.

Needs to notice:
Before calling this function, it's important to ensure that the Visitor object and the double value are valid and properly initialized. Additionally, any error handling related to the QMP output or the double value conversion should be taken into consideration. Special attention should be given to the format and precision of the output numerical value in the context of the QMP protocol to ensure compatibility with the QEMU Monitor.
*/
static void qmp_output_type_number(Visitor *v, const char *name, double *obj,

                                   Error **errp)

{

    QmpOutputVisitor *qov = to_qov(v);

    qmp_output_add(qov, name, qfloat_from_double(*obj));

}
