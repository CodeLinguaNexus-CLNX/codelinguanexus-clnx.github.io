/*
This function is responsible for setting up the VNC display to listen for incoming connections on specified socket addresses.

Here's the detail:
The function iterates through the given array of socket addresses for listening on standard VNC connections and websockets connections. For each address in the array, it calls the function "vnc_display_listen_addr" to start listening on that address. If an error occurs during the listening process, the function returns -1.

Need's to notice:
1. The function takes in two arrays of socket addresses, `saddr` for standard VNC connections and `wsaddr` for websockets connections.
2. The `nsaddr` and `nwsaddr` parameters specify the sizes of the respective arrays.
3. The function uses the `vnc_display_listen_addr` function to perform the actual listening on each address.
4. The `vd` parameter is a pointer to the VncDisplay structure which holds the necessary information for the listening process.
5. The `errp` parameter is used to return any error that occurs during the listening process.
*/
static int vnc_display_listen(VncDisplay *vd,

                              SocketAddress **saddr,

                              size_t nsaddr,

                              SocketAddress **wsaddr,

                              size_t nwsaddr,

                              Error **errp)

{

    size_t i;



    for (i = 0; i < nsaddr; i++) {

        if (vnc_display_listen_addr(vd, saddr[i],

                                    "vnc-listen",

                                    &vd->lsock,

                                    &vd->lsock_tag,

                                    &vd->nlsock,

                                    errp) < 0) {

            return -1;

        }

    }

    for (i = 0; i < nwsaddr; i++) {

        if (vnc_display_listen_addr(vd, wsaddr[i],

                                    "vnc-ws-listen",

                                    &vd->lwebsock,

                                    &vd->lwebsock_tag,

                                    &vd->nlwebsock,

                                    errp) < 0) {

            return -1;

        }

    }



    return 0;

}
