/*
This function is responsible for starting incoming migration over TCP by setting up a listening socket and handling incoming migration requests.

Here's the detail of the function:
The function begins by creating a socket and setting it up for listening on the specified host and port. If successful, it sets the file descriptor handler for the socket to tcp_accept_incoming_migration, which handles incoming migration requests when a connection is established.

Needs to notice:
- It's important to ensure that the host_port parameter is correctly formatted and that the socket is set up to listen on the intended interface and port.
- Any potential errors during the socket setup and file descriptor handler setting should be properly handled to prevent unexpected behavior during migration.
- It's crucial to have a clear understanding of the migration process and how this function fits into the overall migration workflow to use it effectively.
*/
void tcp_start_incoming_migration(const char *host_port, Error **errp)

{

    int s;



    s = inet_listen(host_port, NULL, 256, SOCK_STREAM, 0, errp);

    if (s < 0) {

        return;

    }



    qemu_set_fd_handler2(s, NULL, tcp_accept_incoming_migration, NULL,

                         (void *)(intptr_t)s);

}
