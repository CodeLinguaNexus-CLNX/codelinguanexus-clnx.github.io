/*
This function is responsible for starting an outgoing migration process from the source host to the destination host. It sets up the necessary file descriptor and handles for executing the migration command and managing the migration state.

Here's the detail:
The function begins by allocating memory for the FdMigrationState structure and then attempts to execute the migration command using popen. It sets the file descriptor for the popen'd handle, configures it for non-blocking mode, and initializes the migration state and parameters.

Needs to notice:
1. The command provided should be a valid migration command understood by the target system.
2. The bandwidth_limit parameter determines the maximum bandwidth to be used during the migration process.
3. The async parameter indicates whether the migration should be asynchronous, with a value of 1 representing asynchronous migration.
4. Proper error handling should be in place to handle failures during the migration initialization process.
*/
MigrationState *exec_start_outgoing_migration(const char *command,

                                             int64_t bandwidth_limit,

                                             int async)

{

    FdMigrationState *s;

    FILE *f;



    s = qemu_mallocz(sizeof(*s));

    if (s == NULL) {

        dprintf("Unable to allocate FdMigrationState\n");

        goto err;

    }



    f = popen(command, "w");

    if (f == NULL) {

        dprintf("Unable to popen exec target\n");

        goto err_after_alloc;

    }



    s->fd = fileno(f);

    if (s->fd == -1) {

        dprintf("Unable to retrieve file descriptor for popen'd handle\n");

        goto err_after_open;

    }



    if (fcntl(s->fd, F_SETFD, O_NONBLOCK) == -1) {

        dprintf("Unable to set nonblocking mode on file descriptor\n");

        goto err_after_open;

    }



    s->opaque = qemu_popen(f, "w");




    s->get_error = file_errno;

    s->write = file_write;

    s->mig_state.cancel = migrate_fd_cancel;

    s->mig_state.get_status = migrate_fd_get_status;

    s->mig_state.release = migrate_fd_release;



    s->state = MIG_STATE_ACTIVE;

    s->detach = !async;

    s->bandwidth_limit = bandwidth_limit;



    if (s->detach == 1) {

        dprintf("detaching from monitor\n");

        monitor_suspend();

        s->detach = 2;

    }



    migrate_fd_connect(s);

    return &s->mig_state;



err_after_open:

    pclose(f);

err_after_alloc:

    qemu_free(s);

err:

    return NULL;

}