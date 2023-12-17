/*
This function is responsible for creating a list of command information by querying the QMP (QEMU Machine Protocol) commands supported by the system.

Here's the detail:
- The function initializes a CommandInfoList pointer 'list' to NULL.
- It then iterates through each of the QMP commands using the qmp_for_each_command utility, which populates the 'list' with information about each command by invoking the query_commands_cb callback for each command.
- The query_commands_cb callback function fills the 'list' with relevant command information during each iteration.
- Finally, the function returns the populated 'list' containing information about the available QMP commands.

Usage needs to notice:
- Developers using this function should be familiar with the QMP and its commands in the context of QEMU.
- The 'list' returned needs to be properly handled and freed to avoid memory leaks.
- Proper error handling and reporting need to be considered in case of any failures during the command information query process.

If you have further questions, feel free to ask.
*/
CommandInfoList *qmp_query_commands(Error **errp)

{

    CommandInfoList *list = NULL;



    qmp_for_each_command(&qmp_commands, query_commands_cb, &list);



    return list;

}
