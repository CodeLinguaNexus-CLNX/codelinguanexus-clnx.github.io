/*
This function is responsible for handling interrupts and exceptions in the x86 architecture. It retrieves information from the Interrupt Descriptor Table (IDT) and performs various checks and operations based on the type of interrupt being handled.

Here's the detail: The function first checks whether the interrupt is a hardware interrupt, and if it's not, it determines whether an error code needs to be handled. It then retrieves the interrupt gate descriptor from the IDT and performs several checks, including gate type, privilege level, and validity. Depending on the results of these checks, it sets up a new stack and pushes relevant information onto the stack. It also clears certain flags and updates the instruction pointer (EIP).

Need's to notice: It's important to understand the specific checks and operations performed in this function in order to ensure proper handling of interrupts and exceptions. Additionally, considering the complex nature of interrupt handling and the x86 architecture, any modifications or optimizations to this function should be made with great care and thorough testing.
*/
static void do_interrupt64(CPUX86State *env, int intno, int is_int,

                           int error_code, target_ulong next_eip, int is_hw)

{

    SegmentCache *dt;

    target_ulong ptr;

    int type, dpl, selector, cpl, ist;

    int has_error_code, new_stack;

    uint32_t e1, e2, e3, ss;

    target_ulong old_eip, esp, offset;



    has_error_code = 0;

    if (!is_int && !is_hw) {

        has_error_code = exception_has_error_code(intno);

    }

    if (is_int) {

        old_eip = next_eip;

    } else {

        old_eip = env->eip;

    }



    dt = &env->idt;

    if (intno * 16 + 15 > dt->limit) {

        raise_exception_err(env, EXCP0D_GPF, intno * 16 + 2);

    }

    ptr = dt->base + intno * 16;

    e1 = cpu_ldl_kernel(env, ptr);

    e2 = cpu_ldl_kernel(env, ptr + 4);

    e3 = cpu_ldl_kernel(env, ptr + 8);

    /* check gate type */

    type = (e2 >> DESC_TYPE_SHIFT) & 0x1f;

    switch (type) {

    case 14: /* 386 interrupt gate */

    case 15: /* 386 trap gate */

        break;

    default:

        raise_exception_err(env, EXCP0D_GPF, intno * 16 + 2);

        break;

    }

    dpl = (e2 >> DESC_DPL_SHIFT) & 3;

    cpl = env->hflags & HF_CPL_MASK;

    /* check privilege if software int */

    if (is_int && dpl < cpl) {

        raise_exception_err(env, EXCP0D_GPF, intno * 16 + 2);

    }

    /* check valid bit */

    if (!(e2 & DESC_P_MASK)) {

        raise_exception_err(env, EXCP0B_NOSEG, intno * 16 + 2);

    }

    selector = e1 >> 16;

    offset = ((target_ulong)e3 << 32) | (e2 & 0xffff0000) | (e1 & 0x0000ffff);

    ist = e2 & 7;

    if ((selector & 0xfffc) == 0) {

        raise_exception_err(env, EXCP0D_GPF, 0);

    }



    if (load_segment(env, &e1, &e2, selector) != 0) {

        raise_exception_err(env, EXCP0D_GPF, selector & 0xfffc);

    }

    if (!(e2 & DESC_S_MASK) || !(e2 & (DESC_CS_MASK))) {

        raise_exception_err(env, EXCP0D_GPF, selector & 0xfffc);

    }

    dpl = (e2 >> DESC_DPL_SHIFT) & 3;

    if (dpl > cpl) {

        raise_exception_err(env, EXCP0D_GPF, selector & 0xfffc);

    }

    if (!(e2 & DESC_P_MASK)) {

        raise_exception_err(env, EXCP0B_NOSEG, selector & 0xfffc);

    }

    if (!(e2 & DESC_L_MASK) || (e2 & DESC_B_MASK)) {

        raise_exception_err(env, EXCP0D_GPF, selector & 0xfffc);

    }

    if ((!(e2 & DESC_C_MASK) && dpl < cpl) || ist != 0) {

        /* to inner privilege */

        new_stack = 1;

        esp = get_rsp_from_tss(env, ist != 0 ? ist + 3 : dpl);

        ss = 0;

    } else if ((e2 & DESC_C_MASK) || dpl == cpl) {

        /* to same privilege */

        if (env->eflags & VM_MASK) {

            raise_exception_err(env, EXCP0D_GPF, selector & 0xfffc);

        }

        new_stack = 0;

        esp = env->regs[R_ESP];

        dpl = cpl;

    } else {

        raise_exception_err(env, EXCP0D_GPF, selector & 0xfffc);

        new_stack = 0; /* avoid warning */

        esp = 0; /* avoid warning */

    }

    esp &= ~0xfLL; /* align stack */



    PUSHQ(esp, env->segs[R_SS].selector);

    PUSHQ(esp, env->regs[R_ESP]);

    PUSHQ(esp, cpu_compute_eflags(env));

    PUSHQ(esp, env->segs[R_CS].selector);

    PUSHQ(esp, old_eip);

    if (has_error_code) {

        PUSHQ(esp, error_code);

    }



    /* interrupt gate clear IF mask */

    if ((type & 1) == 0) {

        env->eflags &= ~IF_MASK;

    }

    env->eflags &= ~(TF_MASK | VM_MASK | RF_MASK | NT_MASK);



    if (new_stack) {

        ss = 0 | dpl;

        cpu_x86_load_seg_cache(env, R_SS, ss, 0, 0, 0);

    }

    env->regs[R_ESP] = esp;



    selector = (selector & ~3) | dpl;

    cpu_x86_load_seg_cache(env, R_CS, selector,

                   get_seg_base(e1, e2),

                   get_seg_limit(e1, e2),

                   e2);

    env->eip = offset;

}
