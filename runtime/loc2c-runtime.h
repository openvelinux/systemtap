/* target operations
 * Copyright (C) 2005-2019 Red Hat Inc.
 * Copyright (C) 2005-2007 Intel Corporation.
 * Copyright (C) 2007 Quentin Barnes.
 *
 * This file is part of systemtap, and is free software.  You can
 * redistribute it and/or modify it under the terms of the GNU General
 * Public License (GPL); either version 2, or (at your option) any
 * later version.
 */

#ifndef _LOC2C_RUNTIME_H_
#define _LOC2C_RUNTIME_H_

/* See also the linux/ and dyninst/ runtime specializations. */


/* These three macro definitions are generic, just shorthands
   used by the generated code.  */

#define op_abs(x)	(x < 0 ? -x : x)

#define fetch_bitfield(target, base, higherbits, nbits)			      \
  target = (((base) >> (sizeof (base) * 8 - (higherbits) - (nbits)))	      \
	    & (((__typeof (base)) 1 << (nbits)) - 1))

#define store_bitfield(target, base, higherbits, nbits)			      \
  target = ((target							      \
	     &~ ((((__typeof (target)) 1 << (nbits)) - 1)		      \
		 << (sizeof (target) * 8 - (higherbits) - (nbits))))	      \
	    | ((((__typeof (target)) (base))				      \
		& (((__typeof (target)) 1 << (nbits)) - 1))		      \
	       << (sizeof (target) * 8 - (higherbits) - (nbits))))


/* dwarf_div_op and dwarf_mod_op do division and modulo operations catching any
   divide by zero issues.  When they detect div_by_zero they "fault"
   by jumping to the (slightly misnamed) deref_fault label.  */
#define dwarf_div_op(a,b) ({							\
    if (b == 0) {							\
	snprintf(c->error_buffer, sizeof(c->error_buffer),		\
		 STAP_MSG_LOC2C_03, "DW_OP_div");			\
	c->last_error = c->error_buffer;				\
	goto deref_fault;						\
    }									\
    a / b;								\
})
#define dwarf_mod_op(a,b) ({							\
    if (b == 0) {							\
	snprintf(c->error_buffer, sizeof(c->error_buffer),		\
		 STAP_MSG_LOC2C_03, "DW_OP_mod");			\
	c->last_error = c->error_buffer;				\
	goto deref_fault;						\
    }									\
    a % b;								\
})


/* Given a DWARF register number, fetch its intptr_t (long) value from the
   probe context, or store a new value into the probe context.

   The register number argument is always a canonical decimal number, so it
   can be pasted into an identifier name.  These definitions turn it into a
   per-register macro, defined below for machines with individually-named
   registers.  */
#define pt_regs_fetch_register(pt_regs, regno)		\
  ((intptr_t) pt_dwarf_register_##regno (pt_regs))
#define pt_regs_store_register(pt_regs, regno, value) \
  (pt_dwarf_register_##regno (pt_regs) = (value))

#if defined (STAPCONF_X86_UNIREGS) && defined (__i386__)

#define pt_dwarf_register_0(regs)  regs->ax
#define pt_dwarf_register_1(regs)  regs->cx
#define pt_dwarf_register_2(regs)  regs->dx
#define pt_dwarf_register_3(regs)  regs->bx
#define pt_dwarf_register_4(regs)  ((long) &regs->sp)
#define pt_dwarf_register_5(regs)  regs->bp
#define pt_dwarf_register_6(regs)  regs->si
#define pt_dwarf_register_7(regs)  regs->di
#define pt_regs_maxno 7

#elif defined (STAPCONF_X86_UNIREGS) && defined (__x86_64__)

#define pt_dwarf_register_0(regs)  regs->ax
#define pt_dwarf_register_1(regs)  regs->dx
#define pt_dwarf_register_2(regs)  regs->cx
#define pt_dwarf_register_3(regs)  regs->bx
#define pt_dwarf_register_4(regs)  regs->si
#define pt_dwarf_register_5(regs)  regs->di
#define pt_dwarf_register_6(regs)  regs->bp
#define pt_dwarf_register_7(regs)  regs->sp
#define pt_dwarf_register_8(regs)  regs->r8
#define pt_dwarf_register_9(regs)  regs->r9
#define pt_dwarf_register_10(regs) regs->r10
#define pt_dwarf_register_11(regs) regs->r11
#define pt_dwarf_register_12(regs) regs->r12
#define pt_dwarf_register_13(regs) regs->r13
#define pt_dwarf_register_14(regs) regs->r14
#define pt_dwarf_register_15(regs) regs->r15
#define pt_dwarf_register_17(regs) ({uint64_t v; __asm__ __volatile__("movq %%xmm0, %0" : "=r"(v)); v;})
#define pt_dwarf_register_18(regs) ({uint64_t v; __asm__ __volatile__("movq %%xmm1, %0" : "=r"(v)); v;})
#define pt_dwarf_register_19(regs) ({uint64_t v; __asm__ __volatile__("movq %%xmm2, %0" : "=r"(v)); v;})
#define pt_dwarf_register_20(regs) ({uint64_t v; __asm__ __volatile__("movq %%xmm3, %0" : "=r"(v)); v;})
#define pt_dwarf_register_21(regs) ({uint64_t v; __asm__ __volatile__("movq %%xmm4, %0" : "=r"(v)); v;})
#define pt_dwarf_register_22(regs) ({uint64_t v; __asm__ __volatile__("movq %%xmm5, %0" : "=r"(v)); v;})
#define pt_dwarf_register_23(regs) ({uint64_t v; __asm__ __volatile__("movq %%xmm6, %0" : "=r"(v)); v;})
#define pt_dwarf_register_24(regs) ({uint64_t v; __asm__ __volatile__("movq %%xmm7, %0" : "=r"(v)); v;})
#define pt_dwarf_register_25(regs) ({uint64_t v; __asm__ __volatile__("movq %%xmm8, %0" : "=r"(v)); v;})
#define pt_dwarf_register_26(regs) ({uint64_t v; __asm__ __volatile__("movq %%xmm9, %0" : "=r"(v)); v;})
#define pt_dwarf_register_27(regs) ({uint64_t v; __asm__ __volatile__("movq %%xmm10, %0" : "=r"(v)); v;})
#define pt_dwarf_register_28(regs) ({uint64_t v; __asm__ __volatile__("movq %%xmm11, %0" : "=r"(v)); v;})
#define pt_dwarf_register_29(regs) ({uint64_t v; __asm__ __volatile__("movq %%xmm12, %0" : "=r"(v)); v;})
#define pt_dwarf_register_30(regs) ({uint64_t v; __asm__ __volatile__("movq %%xmm13, %0" : "=r"(v)); v;})
#define pt_dwarf_register_31(regs) ({uint64_t v; __asm__ __volatile__("movq %%xmm14, %0" : "=r"(v)); v;})
#define pt_dwarf_register_32(regs) ({uint64_t v; __asm__ __volatile__("movq %%xmm15, %0" : "=r"(v)); v;})

#define pt_regs_maxno 32
  
#elif defined __i386__

/* The stack pointer is unlike other registers.  When a trap happens in
   kernel mode, it is not saved in the trap frame (struct pt_regs).
   The `esp' (and `xss') fields are valid only for a user-mode trap.
   For a kernel mode trap, the interrupted state's esp is actually an
   address inside where the `struct pt_regs' on the kernel trap stack points. */

#define pt_dwarf_register_0(regs)	regs->eax
#define pt_dwarf_register_1(regs)	regs->ecx
#define pt_dwarf_register_2(regs)	regs->edx
#define pt_dwarf_register_3(regs)	regs->ebx
#define pt_dwarf_register_4(regs)	(user_mode(regs) ? regs->esp : (long)&regs->esp)
#define pt_dwarf_register_5(regs)	regs->ebp
#define pt_dwarf_register_6(regs)	regs->esi
#define pt_dwarf_register_7(regs)	regs->edi
#define pt_regs_maxno 7

#elif defined __ia64__

#undef pt_regs_fetch_register
#undef pt_regs_store_register

#define pt_regs_fetch_register(pt_regs,regno)	\
  ia64_fetch_register(regno, pt_regs, &c->unwaddr)
#define pt_regs_store_register(pt_regs,regno,value) \
  ia64_store_register(regno, pt_regs, value)
#define pt_regs_maxno 128 /* ish */

#elif defined __x86_64__

#define pt_dwarf_register_0(regs)	regs->rax
#define pt_dwarf_register_1(regs)	regs->rdx
#define pt_dwarf_register_2(regs)	regs->rcx
#define pt_dwarf_register_3(regs)	regs->rbx
#define pt_dwarf_register_4(regs)	regs->rsi
#define pt_dwarf_register_5(regs)	regs->rdi
#define pt_dwarf_register_6(regs)	regs->rbp
#define pt_dwarf_register_7(regs)	regs->rsp
#define pt_dwarf_register_8(regs)	regs->r8
#define pt_dwarf_register_9(regs)	regs->r9
#define pt_dwarf_register_10(regs)	regs->r10
#define pt_dwarf_register_11(regs)	regs->r11
#define pt_dwarf_register_12(regs)	regs->r12
#define pt_dwarf_register_13(regs)	regs->r13
#define pt_dwarf_register_14(regs)	regs->r14
#define pt_dwarf_register_15(regs)	regs->r15
#define pt_dwarf_register_17(regs) ({uint64_t v; __asm__ __volatile__("movq %%xmm0, %0" : "=r"(v)); v;})
#define pt_dwarf_register_18(regs) ({uint64_t v; __asm__ __volatile__("movq %%xmm1, %0" : "=r"(v)); v;})
#define pt_dwarf_register_19(regs) ({uint64_t v; __asm__ __volatile__("movq %%xmm2, %0" : "=r"(v)); v;})
#define pt_dwarf_register_20(regs) ({uint64_t v; __asm__ __volatile__("movq %%xmm3, %0" : "=r"(v)); v;})
#define pt_dwarf_register_21(regs) ({uint64_t v; __asm__ __volatile__("movq %%xmm4, %0" : "=r"(v)); v;})
#define pt_dwarf_register_22(regs) ({uint64_t v; __asm__ __volatile__("movq %%xmm5, %0" : "=r"(v)); v;})
#define pt_dwarf_register_23(regs) ({uint64_t v; __asm__ __volatile__("movq %%xmm6, %0" : "=r"(v)); v;})
#define pt_dwarf_register_24(regs) ({uint64_t v; __asm__ __volatile__("movq %%xmm7, %0" : "=r"(v)); v;})
#define pt_dwarf_register_25(regs) ({uint64_t v; __asm__ __volatile__("movq %%xmm8, %0" : "=r"(v)); v;})
#define pt_dwarf_register_26(regs) ({uint64_t v; __asm__ __volatile__("movq %%xmm9, %0" : "=r"(v)); v;})
#define pt_dwarf_register_27(regs) ({uint64_t v; __asm__ __volatile__("movq %%xmm10, %0" : "=r"(v)); v;})
#define pt_dwarf_register_28(regs) ({uint64_t v; __asm__ __volatile__("movq %%xmm11, %0" : "=r"(v)); v;})
#define pt_dwarf_register_29(regs) ({uint64_t v; __asm__ __volatile__("movq %%xmm12, %0" : "=r"(v)); v;})
#define pt_dwarf_register_30(regs) ({uint64_t v; __asm__ __volatile__("movq %%xmm13, %0" : "=r"(v)); v;})
#define pt_dwarf_register_31(regs) ({uint64_t v; __asm__ __volatile__("movq %%xmm14, %0" : "=r"(v)); v;})
#define pt_dwarf_register_32(regs) ({uint64_t v; __asm__ __volatile__("movq %%xmm15, %0" : "=r"(v)); v;})
#define pt_regs_maxno 32
  
#elif defined __powerpc__

#undef pt_regs_fetch_register
#undef pt_regs_store_register
#define pt_regs_fetch_register(pt_regs,regno) \
  ((intptr_t) pt_regs->gpr[regno])
#define pt_regs_store_register(pt_regs,regno,value) \
  (pt_regs->gpr[regno] = (value))
#define pt_regs_maxno 31 /* though PT_REGS_COUNT=44 */
  
#elif defined __mips__

#undef pt_regs_fetch_register
#undef pt_regs_store_register
#define pt_regs_fetch_register(pt_regs,regno) \
  ((intptr_t) pt_regs->regs[regno])
#define pt_regs_store_register(pt_regs,regno,value) \
  (pt_regs->regs[regno] = (value))
#define pt_regs_maxno 31 /* ignore special registers */

#elif defined __riscv

#define pt_dwarf_register_0(regs)	regs->epc
#define pt_dwarf_register_1(regs)	regs->ra
#define pt_dwarf_register_2(regs)	regs->sp
#define pt_dwarf_register_3(regs)	regs->gp
#define pt_dwarf_register_4(regs)	regs->tp
#define pt_dwarf_register_5(regs)	regs->t0
#define pt_dwarf_register_6(regs)	regs->t1
#define pt_dwarf_register_7(regs)	regs->t2
#define pt_dwarf_register_8(regs)	regs->s0
#define pt_dwarf_register_9(regs)	regs->s1
#define pt_dwarf_register_10(regs)	regs->a0
#define pt_dwarf_register_11(regs)	regs->a1
#define pt_dwarf_register_12(regs)	regs->a2
#define pt_dwarf_register_13(regs)	regs->a3
#define pt_dwarf_register_14(regs)	regs->a4
#define pt_dwarf_register_15(regs)	regs->a5
#define pt_dwarf_register_16(regs)	regs->a6
#define pt_dwarf_register_17(regs)	regs->a7
#define pt_dwarf_register_18(regs)	regs->s2
#define pt_dwarf_register_19(regs)	regs->s3
#define pt_dwarf_register_20(regs)	regs->s4
#define pt_dwarf_register_21(regs)	regs->s5
#define pt_dwarf_register_22(regs)	regs->s6
#define pt_dwarf_register_23(regs)	regs->s7
#define pt_dwarf_register_24(regs)	regs->s8
#define pt_dwarf_register_25(regs)	regs->s9
#define pt_dwarf_register_26(regs)	regs->s10
#define pt_dwarf_register_27(regs)	regs->s11
#define pt_dwarf_register_28(regs)	regs->t3
#define pt_dwarf_register_29(regs)	regs->t4
#define pt_dwarf_register_30(regs)	regs->t5
#define pt_dwarf_register_31(regs)	regs->t6
#define pt_regs_maxno 31

#elif defined (__aarch64__)

#define pt_dwarf_register_0(pt_regs)	pt_regs->regs[0]
#define pt_dwarf_register_1(pt_regs)	pt_regs->regs[1]
#define pt_dwarf_register_2(pt_regs)	pt_regs->regs[2]
#define pt_dwarf_register_3(pt_regs)	pt_regs->regs[3]
#define pt_dwarf_register_4(pt_regs)	pt_regs->regs[4]
#define pt_dwarf_register_5(pt_regs)	pt_regs->regs[5]
#define pt_dwarf_register_6(pt_regs)	pt_regs->regs[6]
#define pt_dwarf_register_7(pt_regs)	pt_regs->regs[7]
#define pt_dwarf_register_8(pt_regs)	pt_regs->regs[8]
#define pt_dwarf_register_9(pt_regs)	pt_regs->regs[9]

#define pt_dwarf_register_10(pt_regs)	pt_regs->regs[10]
#define pt_dwarf_register_11(pt_regs)	pt_regs->regs[11]
#define pt_dwarf_register_12(pt_regs)	pt_regs->regs[12]
#define pt_dwarf_register_13(pt_regs)	pt_regs->regs[13]
#define pt_dwarf_register_14(pt_regs)	pt_regs->regs[14]
#define pt_dwarf_register_15(pt_regs)	pt_regs->regs[15]
#define pt_dwarf_register_16(pt_regs)	pt_regs->regs[16]
#define pt_dwarf_register_17(pt_regs)	pt_regs->regs[17]
#define pt_dwarf_register_18(pt_regs)	pt_regs->regs[18]
#define pt_dwarf_register_19(pt_regs)	pt_regs->regs[19]

#define pt_dwarf_register_20(pt_regs)	pt_regs->regs[20]
#define pt_dwarf_register_21(pt_regs)	pt_regs->regs[21]
#define pt_dwarf_register_22(pt_regs)	pt_regs->regs[22]
#define pt_dwarf_register_23(pt_regs)	pt_regs->regs[23]
#define pt_dwarf_register_24(pt_regs)	pt_regs->regs[24]
#define pt_dwarf_register_25(pt_regs)	pt_regs->regs[25]
#define pt_dwarf_register_26(pt_regs)	pt_regs->regs[26]
#define pt_dwarf_register_27(pt_regs)	pt_regs->regs[27]
#define pt_dwarf_register_28(pt_regs)	pt_regs->regs[28]
#define pt_dwarf_register_29(pt_regs)	pt_regs->regs[29]

#define pt_dwarf_register_30(pt_regs)	pt_regs->regs[30]
#define pt_dwarf_register_31(pt_regs)	pt_regs->sp

#define pt_dwarf_register_64(regs) ({uint64_t v; __asm__ __volatile__("mov %0, v0.d[0]" : "=r"(v)); v;})
#define pt_dwarf_register_65(regs) ({uint64_t v; __asm__ __volatile__("mov %0, v1.d[0]" : "=r"(v)); v;})
#define pt_dwarf_register_66(regs) ({uint64_t v; __asm__ __volatile__("mov %0, v2.d[0]" : "=r"(v)); v;})
#define pt_dwarf_register_67(regs) ({uint64_t v; __asm__ __volatile__("mov %0, v3.d[0]" : "=r"(v)); v;})
#define pt_dwarf_register_68(regs) ({uint64_t v; __asm__ __volatile__("mov %0, v4.d[0]" : "=r"(v)); v;})
#define pt_dwarf_register_69(regs) ({uint64_t v; __asm__ __volatile__("mov %0, v5.d[0]" : "=r"(v)); v;})
#define pt_dwarf_register_70(regs) ({uint64_t v; __asm__ __volatile__("mov %0, v6.d[0]" : "=r"(v)); v;})
#define pt_dwarf_register_71(regs) ({uint64_t v; __asm__ __volatile__("mov %0, v7.d[0]" : "=r"(v)); v;})
#define pt_dwarf_register_72(regs) ({uint64_t v; __asm__ __volatile__("mov %0, v8.d[0]" : "=r"(v)); v;})
#define pt_dwarf_register_73(regs) ({uint64_t v; __asm__ __volatile__("mov %0, v9.d[0]" : "=r"(v)); v;})
#define pt_dwarf_register_74(regs) ({uint64_t v; __asm__ __volatile__("mov %0, v10.d[0]" : "=r"(v)); v;})
#define pt_dwarf_register_75(regs) ({uint64_t v; __asm__ __volatile__("mov %0, v11.d[0]" : "=r"(v)); v;})
#define pt_dwarf_register_76(regs) ({uint64_t v; __asm__ __volatile__("mov %0, v12.d[0]" : "=r"(v)); v;})
#define pt_dwarf_register_77(regs) ({uint64_t v; __asm__ __volatile__("mov %0, v13.d[0]" : "=r"(v)); v;})
#define pt_dwarf_register_78(regs) ({uint64_t v; __asm__ __volatile__("mov %0, v14.d[0]" : "=r"(v)); v;})
#define pt_dwarf_register_79(regs) ({uint64_t v; __asm__ __volatile__("mov %0, v15.d[0]" : "=r"(v)); v;})
#define pt_dwarf_register_80(regs) ({uint64_t v; __asm__ __volatile__("mov %0, v16.d[0]" : "=r"(v)); v;})
#define pt_dwarf_register_81(regs) ({uint64_t v; __asm__ __volatile__("mov %0, v17.d[0]" : "=r"(v)); v;})
#define pt_dwarf_register_82(regs) ({uint64_t v; __asm__ __volatile__("mov %0, v18.d[0]" : "=r"(v)); v;})
#define pt_dwarf_register_83(regs) ({uint64_t v; __asm__ __volatile__("mov %0, v19.d[0]" : "=r"(v)); v;})
#define pt_dwarf_register_84(regs) ({uint64_t v; __asm__ __volatile__("mov %0, v20.d[0]" : "=r"(v)); v;})
#define pt_dwarf_register_85(regs) ({uint64_t v; __asm__ __volatile__("mov %0, v21.d[0]" : "=r"(v)); v;})
#define pt_dwarf_register_86(regs) ({uint64_t v; __asm__ __volatile__("mov %0, v22.d[0]" : "=r"(v)); v;})
#define pt_dwarf_register_87(regs) ({uint64_t v; __asm__ __volatile__("mov %0, v23.d[0]" : "=r"(v)); v;})
#define pt_dwarf_register_88(regs) ({uint64_t v; __asm__ __volatile__("mov %0, v24.d[0]" : "=r"(v)); v;})
#define pt_dwarf_register_89(regs) ({uint64_t v; __asm__ __volatile__("mov %0, v25.d[0]" : "=r"(v)); v;})
#define pt_dwarf_register_90(regs) ({uint64_t v; __asm__ __volatile__("mov %0, v26.d[0]" : "=r"(v)); v;})
#define pt_dwarf_register_91(regs) ({uint64_t v; __asm__ __volatile__("mov %0, v27.d[0]" : "=r"(v)); v;})
#define pt_dwarf_register_92(regs) ({uint64_t v; __asm__ __volatile__("mov %0, v28.d[0]" : "=r"(v)); v;})
#define pt_dwarf_register_93(regs) ({uint64_t v; __asm__ __volatile__("mov %0, v29.d[0]" : "=r"(v)); v;})
#define pt_dwarf_register_94(regs) ({uint64_t v; __asm__ __volatile__("mov %0, v30.d[0]" : "=r"(v)); v;})
#define pt_dwarf_register_95(regs) ({uint64_t v; __asm__ __volatile__("mov %0, v31.d[0]" : "=r"(v)); v;})
#define pt_regs_maxno 95

#elif defined (__arm__)

#undef pt_regs_fetch_register
#undef pt_regs_store_register
#define pt_regs_fetch_register(pt_regs,regno) \
  ((long) pt_regs->uregs[regno])
#define pt_regs_store_register(pt_regs,regno,value) \
  (pt_regs->uregs[regno] = (value))
#define pt_regs_maxno 17
  
#elif defined (__s390__) || defined (__s390x__)

#define _fetch_float_register(regno) \
  ({unsigned long v;  __asm__ __volatile__("lgdr %[_DEST],%%f%n[_REGNO]" : [_DEST] "=r" (v): [_REGNO] "n" (-(regno-16))) ; v;})

#undef pt_regs_fetch_register
#undef pt_regs_store_register
#define pt_regs_fetch_register(pt_regs,regno) \
  (regno < 16 ? ((intptr_t) pt_regs->gprs[regno]) : (_fetch_float_register(regno)))
#define pt_regs_store_register(pt_regs,regno,value) \
  (pt_regs->gprs[regno] = (value))
#define pt_regs_maxno 32 /* NUM_GPRS */
  
#endif


#if STP_SKIP_BADVARS
#define DEREF_FAULT(addr) ({0; })
#define STORE_DEREF_FAULT(addr) ({0; })
#define CATCH_DEREF_FAULT() ({0; })
#else
#define DEREF_FAULT(addr) ({						    \
    snprintf(c->error_buffer, sizeof(c->error_buffer),			    \
	     STAP_MSG_LOC2C_01, (long)(intptr_t)(addr));		    \
    c->last_error = c->error_buffer;					    \
    goto deref_fault;							    \
    })

#define STORE_DEREF_FAULT(addr) ({					    \
    snprintf(c->error_buffer, sizeof(c->error_buffer),			    \
	     STAP_MSG_LOC2C_02, (long)(intptr_t)(addr));		    \
    c->last_error = c->error_buffer;					    \
    goto deref_fault;							    \
    })

#define CATCH_DEREF_FAULT()	({0; }) /* always emitted by translator.cxx for functions & probes */
#endif

#endif /* _LOC2C_RUNTIME_H_ */
