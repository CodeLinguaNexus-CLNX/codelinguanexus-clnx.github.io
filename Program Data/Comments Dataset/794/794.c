/*
This function is responsible for handling and processing mouse events within the context of the SDL (Simple DirectMedia Layer) input system. It translates the mouse motion, position, and button state into the corresponding input events for the virtualized system.

Here's the detail of the function:
- It begins by defining a static array bmap to map the virtual input buttons to the SDL button identifiers, such as SDL_BUTTON_LEFT, SDL_BUTTON_MIDDLE, and SDL_BUTTON_RIGHT.
- The function uses a static variable prev_state to keep track of the previous state of the mouse buttons.
- Upon receiving a mouse event, the function compares the previous state with the current state. If a change is detected, it calls qemu_input_update_buttons to update the button states in the virtual input system by passing the previous and current button state along with the button mapping array.
- The function then processes the mouse motion and position. If absolute mouse positioning is enabled (as determined by qemu_input_is_absolute), it queues absolute axis events using the current mouse coordinates and the screen dimensions.
- If absolute mouse positioning is not enabled, it calculates the relative motion (dx, dy) and updates the guest cursor position accordingly. It then queues relative axis events using the calculated motion values.
- Finally, the function calls qemu_input_event_sync to synchronize and flush the queued input events, ensuring they are processed by the virtualized system.

Needs to notice:
- Understanding the interaction between the SDL input system, virtual input subsystem, and guest OS input handling is essential for maintaining and modifying this function.
- Proper synchronization and event processing mechanisms should be in place to ensure smooth and accurate translation of the physical mouse events to virtual input events.
- Care should be taken to handle absolute and relative mouse positioning correctly based on the virtualized system's capabilities and guest OS requirements.
*/
static void sdl_send_mouse_event(int dx, int dy, int x, int y, int state)

{

    static uint32_t bmap[INPUT_BUTTON__MAX] = {

        [INPUT_BUTTON_LEFT]       = SDL_BUTTON(SDL_BUTTON_LEFT),

        [INPUT_BUTTON_MIDDLE]     = SDL_BUTTON(SDL_BUTTON_MIDDLE),

        [INPUT_BUTTON_RIGHT]      = SDL_BUTTON(SDL_BUTTON_RIGHT),

        [INPUT_BUTTON_WHEEL_UP]   = SDL_BUTTON(SDL_BUTTON_WHEELUP),

        [INPUT_BUTTON_WHEEL_DOWN] = SDL_BUTTON(SDL_BUTTON_WHEELDOWN),

    };

    static uint32_t prev_state;



    if (prev_state != state) {

        qemu_input_update_buttons(dcl->con, bmap, prev_state, state);

        prev_state = state;

    }



    if (qemu_input_is_absolute()) {

        qemu_input_queue_abs(dcl->con, INPUT_AXIS_X, x,

                             real_screen->w);

        qemu_input_queue_abs(dcl->con, INPUT_AXIS_Y, y,

                             real_screen->h);

    } else {

        if (guest_cursor) {

            x -= guest_x;

            y -= guest_y;

            guest_x += x;

            guest_y += y;

            dx = x;

            dy = y;

        }

        qemu_input_queue_rel(dcl->con, INPUT_AXIS_X, dx);

        qemu_input_queue_rel(dcl->con, INPUT_AXIS_Y, dy);

    }

    qemu_input_event_sync();

}
