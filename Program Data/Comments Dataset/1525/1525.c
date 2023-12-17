/*
This function is responsible for obtaining the size of a file from an FTP server. It sends a "SIZE" command to the server and retrieves the file size from the server's response.

Here's the detail of the function: The function constructs an FTP command using the file path and sends it to the server. It then waits for the server's response and extracts the file size from the response if the command was successful. If the command fails, it sets the file size to -1 and handles the error by freeing the allocated resources and returning an I/O error.

Need's to notice that the function assumes a specific format of the server's response containing the file size and relies on parsing the response to extract the size. Proper error handling and resource cleanup are essential in case of command failure or unexpected responses from the server. Also, it's important to ensure that the server response and the parsing method are consistent with the FTP protocol specifications.

The code seems to be part of an FTP client implementation and deals with retrieving file size information from the FTP server.

*/static int ftp_file_size(FTPContext* s)

{

    char command[CONTROL_BUFFER_SIZE];

    char *res = NULL;

    const int size_codes[] = {213, 0};



    snprintf(command, sizeof(command), "SIZE %s\r\n", s->path);

    if (ftp_send_command(s, command, size_codes, &res)) {

        s->filesize = strtoll(&res[4], NULL, 10);

    } else {

        s->filesize = -1;

        av_free(res);

        return AVERROR(EIO);

    }



    av_free(res);

    return 0;

}
