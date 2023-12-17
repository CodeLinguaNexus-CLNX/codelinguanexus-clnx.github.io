/*
This function is responsible for updating the link status of a VhostUserState structure and invoking relevant callbacks if necessary.

Here's the detail:
1. The function 'net_vhost_link_down' takes a VhostUserState pointer 's' and a boolean 'link_down' as arguments. It updates the link status of 's' with the value of 'link_down'.

2. If 's' has a non-null 'peer', it also updates the 'link_down' status of the peer VhostUserState structure.

3. If the 'link_status_changed' callback is set in the 'info' field of 's->nc', the function invokes this callback, passing a reference to 's->nc'.

4. Similarly, if the 'peer' has a non-null 'info' field and its 'link_status_changed' callback is set, the function invokes this callback with 's->nc.peer' as the argument.

Need's to notice:
It's important to ensure that the pointers being dereferenced such as 's', 's->nc', 's->nc.peer' are properly validated for null before dereferencing to avoid potential segmentation faults or undefined behavior. Additionally, it's necessary to verify that the callbacks being invoked are intended to be called at this specific point and that the relevant callback functions have been properly initialized.
*/
static void net_vhost_link_down(VhostUserState* s, bool link_down)

{

    s->nc.link_down = link_down;



    if (s->nc.peer) {

        s->nc.peer->link_down = link_down;

    }



    if (s->nc.info->link_status_changed) {

        s->nc.info->link_status_changed(&s->nc);

    }



    if (s->nc.peer && s->nc.peer->info->link_status_changed) {

        s->nc.peer->info->link_status_changed(s->nc.peer);

    }

}
