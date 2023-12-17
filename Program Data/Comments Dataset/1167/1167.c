/*
This function is responsible for handling write operations to a memory-mapped register in the SLAVIO_TIMER device.

Here's the detail:
- The function takes opaque, addr, and val as parameters. Opaque is a pointer to the SLAVIO_TIMERState structure, addr is the target physical address, and val is the value to be written to the address.
- It extracts the specific register address from the target address and performs different operations based on the address.
- For the TIMER_LIMIT register, it sets the limit for the timer and may reset the counter depending on whether it's a user timer or not.
- For the TIMER_COUNTER register, it handles setting the user counter LSW and resetting the counter for user timers.
- For the TIMER_COUNTER_NORST register, it sets the limit without resetting the counter.
- For the TIMER_STATUS register, it starts or stops the user counter based on the value written.
- For the TIMER_MODE register, it updates the timer mode and handles timer changes for slave timers.

Need's to notice:
- The function distinguishes between user timers and system timers and performs different actions based on the type of timer.
- It updates the internal state of the SLAVIO_TIMER device based on the write operations, including setting limits, starting/stopping timers, and handling timer mode changes.
- This function assumes that the SLAVIO_TIMERState structure and related data structures are properly initialized and managed outside of this function. Its behavior is tightly coupled with the SLAVIO_TIMER device's functionality and internal state management.
*/
static void slavio_timer_mem_writel(void *opaque, target_phys_addr_t addr,

                                    uint32_t val)

{

    SLAVIO_TIMERState *s = opaque;

    uint32_t saddr;

    int reload = 0;



    DPRINTF("write " TARGET_FMT_plx " %08x\n", addr, val);

    saddr = (addr & TIMER_MAXADDR) >> 2;

    switch (saddr) {

    case TIMER_LIMIT:

        if (slavio_timer_is_user(s)) {

            // set user counter MSW, reset counter

            qemu_irq_lower(s->irq);

            s->limit = TIMER_MAX_COUNT64;

            DPRINTF("processor %d user timer reset\n", s->slave_index);

            ptimer_set_limit(s->timer, LIMIT_TO_PERIODS(s->limit), 1);

        } else {

            // set limit, reset counter

            qemu_irq_lower(s->irq);

            s->limit = val & TIMER_MAX_COUNT32;

            if (!s->limit)

                s->limit = TIMER_MAX_COUNT32;

            ptimer_set_limit(s->timer, s->limit >> 9, 1);

        }

        break;

    case TIMER_COUNTER:

        if (slavio_timer_is_user(s)) {

            // set user counter LSW, reset counter

            qemu_irq_lower(s->irq);

            s->limit = TIMER_MAX_COUNT64;

            DPRINTF("processor %d user timer reset\n", s->slave_index);

            ptimer_set_limit(s->timer, LIMIT_TO_PERIODS(s->limit), 1);

        } else

            DPRINTF("not user timer\n");

        break;

    case TIMER_COUNTER_NORST:

        // set limit without resetting counter

        s->limit = val & TIMER_MAX_COUNT32;

        if (!s->limit)

            s->limit = TIMER_MAX_COUNT32;

        ptimer_set_limit(s->timer, LIMIT_TO_PERIODS(s->limit), reload);

        break;

    case TIMER_STATUS:

        if (slavio_timer_is_user(s)) {

            // start/stop user counter

            if ((val & 1) && !s->running) {

                DPRINTF("processor %d user timer started\n", s->slave_index);

                ptimer_run(s->timer, 0);

                s->running = 1;

            } else if (!(val & 1) && s->running) {

                DPRINTF("processor %d user timer stopped\n", s->slave_index);

                ptimer_stop(s->timer);

                s->running = 0;

            }

        }

        break;

    case TIMER_MODE:

        if (s->master == NULL) {

            unsigned int i;



            for (i = 0; i < s->num_slaves; i++) {

                if (val & (1 << i)) {

                    qemu_irq_lower(s->slave[i]->irq);

                    s->slave[i]->limit = -1ULL;

                }

                if ((val & (1 << i)) != (s->slave_mode & (1 << i))) {

                    ptimer_stop(s->slave[i]->timer);

                    ptimer_set_limit(s->slave[i]->timer,

                                     LIMIT_TO_PERIODS(s->slave[i]->limit), 1);

                    DPRINTF("processor %d timer changed\n",

                            s->slave[i]->slave_index);

                    ptimer_run(s->slave[i]->timer, 0);

                }

            }

            s->slave_mode = val & ((1 << s->num_slaves) - 1);

        } else

            DPRINTF("not system timer\n");

        break;

    default:

        DPRINTF("invalid write address " TARGET_FMT_plx "\n", addr);

        break;

    }

}
