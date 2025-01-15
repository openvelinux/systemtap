/* target operations in the Linux kernel mode
 * Copyright (C) 2005-2019 Red Hat Inc.
 * Copyright (C) 2005-2007 Intel Corporation.
 * Copyright (C) 2007 Quentin Barnes.
 *
 * This file is part of systemtap, and is free software.  You can
 * redistribute it and/or modify it under the terms of the GNU General
 * Public License (GPL); either version 2, or (at your option) any
 * later version.
 */

#ifndef _LINUX_LOC2C_RUNTIME_H_
#define _LINUX_LOC2C_RUNTIME_H_

#ifdef STAPCONF_LINUX_UACCESS_H
#include <linux/uaccess.h>
#else
#include <asm/uaccess.h>
#endif
#include <linux/types.h>
#define intptr_t long
#define uintptr_t unsigned long

#include "../loc2c-runtime.h"


#ifndef STAPCONF_PAGEFAULT_DISABLE  /* before linux commit a866374a */
#define pagefault_disable() preempt_disable()
#define pagefault_enable() preempt_enable_no_resched()
#endif


#define check_fetch_register(regs,regno,maxregno,fn) ({		            \
  if ((regs) == 0 || (regno) < 0 || (regno) > (maxregno)) {		    \
        snprintf(c->error_buffer, sizeof(c->error_buffer),		    \
		 STAP_MSG_LOC2C_04); 					    \
    c->last_error = c->error_buffer;					    \
    goto deref_fault;							    \
  }                  							    \
  fn(regs, regno);	   						    \
})
    
#define check_store_register(regs,regno,maxregno,value,fn) do {		    \
  if ((regs) == 0 || (regno) < 0 || (regno) > (maxregno)) {		    \
        snprintf(c->error_buffer, sizeof(c->error_buffer),		    \
		 STAP_MSG_LOC2C_04); 					    \
    c->last_error = c->error_buffer;					    \
    goto deref_fault;							    \
  }                  							    \
  fn(regs, regno, value);                                                   \
} while(0)


#define k_fetch_register(regno) check_fetch_register(c->kregs,regno,pt_regs_maxno,pt_regs_fetch_register)

#define k_store_register(regno,value) check_store_register(c->kregs,regno,pt_regs_maxno,value,pt_regs_store_register)
    


/* PR 10601: user-space (user_regset) register access.
   Needs arch specific code, only i386 and x86_64 for now.  */
#if ((defined(STAPCONF_REGSET) || defined(STAPCONF_UTRACE_REGSET)) \
     && (defined (__i386__) || defined (__x86_64__)))

#if defined(STAPCONF_REGSET)
#include <linux/regset.h>
#endif

#if defined(STAPCONF_UTRACE_REGSET)
#include <linux/tracehook.h>
/* adapt new names to old decls */
#define user_regset_view utrace_regset_view
#define user_regset utrace_regset
#define task_user_regset_view utrace_native_view

#else // PR13489, inodes-uprobes export kludge
#if !defined(STAPCONF_TASK_USER_REGSET_VIEW_EXPORTED)
// First typedef from the original decl, then #define it as a typecasted call.
// NB: not all archs actually have the function, but the decl is universal in regset.h
typedef typeof(&task_user_regset_view) task_user_regset_view_fn;
/* Special macro to tolerate the kallsyms function pointer being zero. */
#define task_user_regset_view(t) (kallsyms_task_user_regset_view ? \
                                  (* (task_user_regset_view_fn)(kallsyms_task_user_regset_view))((t)) : \
                                  NULL)
#endif
#endif

struct usr_regset_lut {
  char *name;
  unsigned rsn;
  unsigned pos;
};


/* DWARF register number -to- user_regset bank/offset mapping table.
   The register numbers come from the processor-specific ELF documents.
   The user-regset bank/offset values come from kernel $ARCH/include/asm/user*.h
   or $ARCH/kernel/ptrace.c. */
#if defined (__i386__) || defined (__x86_64__)
static const struct usr_regset_lut url_i386[] = {
  { "ax", NT_PRSTATUS, 6*4 },
  { "cx", NT_PRSTATUS, 1*4 },
  { "dx", NT_PRSTATUS, 2*4 },
  { "bx", NT_PRSTATUS, 0*4 },
  { "sp", NT_PRSTATUS, 15*4 },
  { "bp", NT_PRSTATUS, 5*4 },
  { "si", NT_PRSTATUS, 3*4 },
  { "di", NT_PRSTATUS, 4*4 },
  { "ip", NT_PRSTATUS, 12*4 },
};
#endif

#if defined (__x86_64__)
static const struct usr_regset_lut url_x86_64[] = {
  { "rax", NT_PRSTATUS, 10*8 },
  { "rdx", NT_PRSTATUS, 12*8 },
  { "rcx", NT_PRSTATUS, 11*8 },
  { "rbx", NT_PRSTATUS, 5*8 },
  { "rsi", NT_PRSTATUS, 13*8 },
  { "rdi", NT_PRSTATUS, 14*8 },
  { "rbp", NT_PRSTATUS, 4*8 },
  { "rsp", NT_PRSTATUS, 19*8 },
  { "r8", NT_PRSTATUS, 9*8 },
  { "r9", NT_PRSTATUS, 8*8 }, 
  { "r10", NT_PRSTATUS, 7*8 },  
  { "r11", NT_PRSTATUS, 6*8 }, 
  { "r12", NT_PRSTATUS, 3*8 }, 
  { "r13", NT_PRSTATUS, 2*8 }, 
  { "r14", NT_PRSTATUS, 1*8 }, 
  { "r15", NT_PRSTATUS, 0*8 }, 
  { "rip", NT_PRSTATUS, 16*8 },
  /* XXX: SSE registers %xmm0-%xmm7 */ 
  { "xmm0", NT_PRFPREG, 160+0*16}, // dwarf reg# 17 = byte #160 in PRFPREG register dump
  { "xmm1", NT_PRFPREG, 160+1*16}, // see also gdb gdb i387-tdep.c fxsave_offset
  { "xmm2", NT_PRFPREG, 160+2*16}, // see also intel x86-64 architecture software manual, fxsave area
  { "xmm3", NT_PRFPREG, 160+3*16},
  { "xmm4", NT_PRFPREG, 160+4*16},
  { "xmm5", NT_PRFPREG, 160+5*16},
  { "xmm6", NT_PRFPREG, 160+6*16},
  { "xmm7", NT_PRFPREG, 160+7*16},
  /* XXX: SSE2 registers %xmm8-%xmm15 */
  { "xmm8", NT_PRFPREG, 160+8*16},
  { "xmm9", NT_PRFPREG, 160+9*16},
  { "xmm10", NT_PRFPREG, 160+10*16},
  { "xmm11", NT_PRFPREG, 160+11*16},
  { "xmm12", NT_PRFPREG, 160+12*16},
  { "xmm13", NT_PRFPREG, 160+13*16},
  { "xmm14", NT_PRFPREG, 160+14*16},
  { "xmm15", NT_PRFPREG, 160+15*16},
  /* XXX: FP registers %st0-%st7 */
  /* XXX: MMX registers %mm0-%mm7 */
  { "st0", NT_PRFPREG, 32},
  { "st1", NT_PRFPREG, 48},
  { "st2", NT_PRFPREG, 64},
  { "st3", NT_PRFPREG, 80},
  { "st4", NT_PRFPREG, 90},
  { "st5", NT_PRFPREG, 112},
  { "st6", NT_PRFPREG, 128},
  { "st7", NT_PRFPREG, 144}
};
#endif
/* XXX: insert other architectures here. */


static u32 ursl_fetch32 (const struct usr_regset_lut* lut, unsigned lutsize, int e_machine, unsigned regno)
{
  u32 value = ~0;
  const struct user_regset_view *rsv = task_user_regset_view(current); 
  unsigned rsi;
  int rc;
  unsigned rsn;
  unsigned pos;
  unsigned count;

  WARN_ON (!rsv);
  if (!rsv) goto out;
  WARN_ON (regno >= lutsize);
  if (regno >= lutsize) goto out;
  if (rsv->e_machine != e_machine) goto out;

  rsn = lut[regno].rsn;
  pos = lut[regno].pos;
  count = sizeof(value);

  for (rsi=0; rsi<rsv->n; rsi++)
    if (rsv->regsets[rsi].core_note_type == rsn)
      {
        const struct user_regset *rs = & rsv->regsets[rsi];
        rc = (rs->get)(current, rs, pos, count, & value, NULL);
        WARN_ON (rc);
        /* success */
        goto out;
      }
  WARN_ON (1); /* did not find appropriate regset! */
  
 out:
  return value;
}


static void ursl_store32 (const struct usr_regset_lut* lut,unsigned lutsize,  int e_machine, unsigned regno, u32 value)
{
  const struct user_regset_view *rsv = task_user_regset_view(current); 
  unsigned rsi;
  int rc;
  unsigned rsn;
  unsigned pos;
  unsigned count;

  WARN_ON (!rsv);
  if (!rsv) goto out;
  WARN_ON (regno >= lutsize);
  if (regno >= lutsize) goto out;
  if (rsv->e_machine != e_machine) goto out;

  rsn = lut[regno].rsn;
  pos = lut[regno].pos;
  count = sizeof(value);

  for (rsi=0; rsi<rsv->n; rsi++)
    if (rsv->regsets[rsi].core_note_type == rsn)
      {
        const struct user_regset *rs = & rsv->regsets[rsi];
        rc = (rs->set)(current, rs, pos, count, & value, NULL);
        WARN_ON (rc);
        /* success */
        goto out;
      }
  WARN_ON (1); /* did not find appropriate regset! */
  
 out:
  return;
}


static u64 ursl_fetch64 (const struct usr_regset_lut* lut, unsigned lutsize, int e_machine, unsigned regno)
{
  u64 value = ~0;
  const struct user_regset_view *rsv = task_user_regset_view(current); 
  unsigned rsi;
  int rc;
  unsigned rsn;
  unsigned pos;
  unsigned count;

  if (!rsv) goto out;
  if (regno >= lutsize) goto out;
  if (rsv->e_machine != e_machine) goto out;

  rsn = lut[regno].rsn;
  pos = lut[regno].pos;
  count = sizeof(value);

  for (rsi=0; rsi<rsv->n; rsi++)
    if (rsv->regsets[rsi].core_note_type == rsn)
      {
        const struct user_regset *rs = & rsv->regsets[rsi];
        rc = (rs->get)(current, rs, pos, count, & value, NULL);
        if (rc)
          goto out;
        /* success */
        return value;
      }
 out:
  printk (KERN_WARNING "process %d mach %d regno %d not available for fetch.\n", current->tgid, e_machine, regno);
  return value;
}


static void ursl_store64 (const struct usr_regset_lut* lut,unsigned lutsize,  int e_machine, unsigned regno, u64 value)
{
  const struct user_regset_view *rsv = task_user_regset_view(current); 
  unsigned rsi;
  int rc;
  unsigned rsn;
  unsigned pos;
  unsigned count;

  WARN_ON (!rsv);
  if (!rsv) goto out;
  WARN_ON (regno >= lutsize);
  if (regno >= lutsize) goto out;
  if (rsv->e_machine != e_machine) goto out;

  rsn = lut[regno].rsn;
  pos = lut[regno].pos;
  count = sizeof(value);

  for (rsi=0; rsi<rsv->n; rsi++)
    if (rsv->regsets[rsi].core_note_type == rsn)
      {
        const struct user_regset *rs = & rsv->regsets[rsi];
        rc = (rs->set)(current, rs, pos, count, & value, NULL);
        if (rc)
          goto out;
        /* success */
        return;
      }
  
 out:
  printk (KERN_WARNING "process %d mach %d regno %d not available for store.\n", current->tgid, e_machine, regno);
  return;
}



#if defined (__i386__)

#define uu_fetch_register(_regs,regno) ursl_fetch32(url_i386, ARRAY_SIZE(url_i386), EM_386, regno)
#define uu_store_register(_regs,regno,value) ursl_store32(url_i386, ARRAY_SIZE(url_i386), EM_386, regno, value)

#define u_fetch_register(regno) check_fetch_register(c->uregs,regno,ARRAY_SIZE(url_i386),uu_fetch_register)
#define u_store_register(regno,value) check_store_register(c->uregs,regno,ARRAY_SIZE(url_i386),value,uu_store_register)

#elif defined (__x86_64__)

#define uu_fetch_register(_regs,regno) (_stp_is_compat_task() ? ursl_fetch32(url_i386, ARRAY_SIZE(url_i386), EM_386, regno) : ursl_fetch64(url_x86_64, ARRAY_SIZE(url_x86_64), EM_X86_64, regno))
#define uu_store_register(_regs,regno,value)  (_stp_is_compat_task() ? ursl_store32(url_i386, ARRAY_SIZE(url_i386), EM_386, regno, value) : ursl_store64(url_x86_64, ARRAY_SIZE(url_x86_64), EM_X86_64, regno, value))

#define u_fetch_register(regno) check_fetch_register(c->uregs,regno,_stp_is_compat_task()?ARRAY_SIZE(url_i386):ARRAY_SIZE(url_x86_64),uu_fetch_register)
#define u_store_register(regno,value) check_store_register(c->uregs,regno,_stp_is_compat_task()?ARRAY_SIZE(url_i386):ARRAY_SIZE(url_x86_64),value,uu_store_register)

#endif

#else /* ! STAPCONF_REGSET */

/* Downgrade to pt_dwarf_register access. */

#define u_store_register(regno, value) \
  check_store_register(c->uregs,regno,pt_regs_maxno,value,pt_regs_store_register)

/* If we're in a 32/31-bit task in a 64-bit kernel, we need to emulate
 * 32-bitness by masking the output of pt_regs_fetch_register() */
#ifndef CONFIG_COMPAT
#define u_fetch_register(regno) \
  check_fetch_register(c->uregs,regno,pt_regs_maxno,pt_regs_fetch_register)

#else
#define u_fetch_register(regno) \
  check_fetch_register(c->uregs,regno,pt_regs_maxno,pt_regs_fetch_register) & (_stp_is_compat_task() ? 0xffffffff : ~(int64_t)0)
#endif

#endif /* STAPCONF_REGSET */


/* The deref and store_deref macros are called to safely access
   addresses in the probe context.  These macros are used only for
   kernel addresses.  The macros must handle bogus addresses here
   gracefully (as from corrupted data structures, stale pointers,
   etc), by doing a "goto deref_fault".

   Prior to kernel 5.10, on most machines, the asm/uaccess.h macros
   __get_user and __put_user macros do exactly the low-level work
   we need to access memory with fault handling,
   and are not actually specific to user-address access at all.

   After kernel 5.10 on arches removing set_fs(), kernel
   addresses should be read/written with get_kernel_nofault and
   copy_to_kernel_nofault while user addresses are still read/written
   with __get_user and __put_user. So we have wrapper macros
   __stp_{get,put}_either which do the right thing on all kernel
   versions. */

/*
 * On most platforms, __get_user_inatomic() and __put_user_inatomic()
 * are defined, which are the same as __get_user() and __put_user(),
 * but without a call to might_sleep(). Since we're disabling page
 * faults when we read, we want to use the 'inatomic' variants when
 * available.
 */
#ifdef __get_user_inatomic
#define __stp_get_user __get_user_inatomic
#else
#define __stp_get_user __get_user
#endif
#ifdef __put_user_inatomic
#define __stp_put_user __put_user_inatomic
#else
#define __stp_put_user __put_user
#endif

/*
 * Some arches (like aarch64) don't have __get_user_bad() or
 * __put_user_bad(), so use BUILD_BUG() instead.
 */
#ifdef BUILD_BUG
#define __stp_get_user_bad BUILD_BUG
#define __stp_put_user_bad BUILD_BUG
#else
#define __stp_get_user_bad __get_user_bad
#define __stp_put_user_bad __put_user_bad
#endif

/*
 * __stp_{get,put}_either take an stp_mm_segment_t parameter
 * and use that to decide the correct address space
 * on post-5.10 non-set_fs() kernels.
 */
#ifdef STAPCONF_SET_FS

#define __stp_get_either(v, addr, seg) __stp_get_user((v), (addr))
#define __stp_put_either(v, addr, seg) __stp_put_user((v), (addr))

#else

/* PR26811: Distinguish user- and kernel-space get and put operations.
 *
 * XXX There is slight redundancy between the size adjustments we
 * do and the size adjustments done by {get,copy_to}_kernel_nofault. */

/* XXX copy_to_kernel_nofault is what we need, but it's not exported.
 * First typedef from the original decl, then #define it as a typecasted call.
 */
typedef typeof(&copy_to_kernel_nofault) copy_to_kernel_nofault_fn;
#define copy_to_kernel_nofault(dst, src, size)                      \
  (kallsyms_copy_to_kernel_nofault ?                                \
   (* (copy_to_kernel_nofault_fn)(kallsyms_copy_to_kernel_nofault)) \
   ((dst),(src),(size)) :                                           \
   -EFAULT)

#define __stp_get_either(v, addr, seg)          \
  (MM_SEG_IS_KERNEL((seg)) ?                    \
   get_kernel_nofault((v), (addr)) :            \
   __stp_get_user((v), (addr)))
#define __stp_put_either(v, addr, seg)                                  \
  (MM_SEG_IS_KERNEL((seg)) ?                                            \
   ({typeof(v) _v = (v); long rc = copy_to_kernel_nofault((void *)(addr), (void *)&_v, sizeof(v)); rc;}) : \
   __stp_put_user((v), (addr)))

#endif

/* 
 * __stp_deref_nocheck(): reads a simple type from a
 * location with no address sanity checking.
 *
 * value: read the simple type into this variable
 * size: number of bytes to read
 * addr: address to read from
 * seg: memory segment to use, either kernel (STP_KERNEL_DS) or user
 * (STP_USER_DS)
 *
 * Note that the caller *must* check the address for validity and do
 * any other checks necessary. This macro is designed to be used as
 * the base for the other macros more suitable for the rest of the
 * code to use. Note that the caller is also responsible for ensuring
 * that the kernel doesn't pagefault while reading.
 */

static inline int __stp_deref_nocheck_(u64 *pv, size_t size,
                                       void *addr, stp_mm_segment_t seg)
{
  u64 v = 0;
  int r = -EFAULT;

  switch (size)
    {
    case 1: { u8 b; r = __stp_get_either(b, (u8 *)addr, seg); v = b; } break;
    case 2: { u16 w; r = __stp_get_either(w, (u16 *)addr, seg); v = w; } break;
    case 4: { u32 l; r = __stp_get_either(l, (u32 *)addr, seg); v = l; } break;
#if defined(__i386__) || defined(__arm__)
    /* x86 and arm can't do 8-byte get_user, so we have to split it */
    case 8: { union { u32 l[2]; u64 ll; } val;
	      r = __stp_get_either(val.l[0], &((u32 *)addr)[0], seg);
	      if (! r)
		  r = __stp_get_either(val.l[1], &((u32 *)addr)[1], seg);
	      if (! r)
		  v = val.ll;
	    } break;
#else
    case 8: { r = __stp_get_either(v, (u64 *)addr, seg); } break;
#endif
    }

  *pv = v;
  return r;
}

#define __stp_deref_nocheck(value, size, addr, seg)                     \
  ({									\
    u64 _v = 0; int _e = -EFAULT;					\
    switch (size)							\
      {									\
      case 1: case 2: case 4: case 8:					\
	_e = __stp_deref_nocheck_                                       \
          (&_v, (size), (void *)(uintptr_t)(addr), (seg));              \
	(value) = _v;							\
	break;								\
      default:								\
	__stp_get_user_bad();						\
      }									\
    _e;									\
  })


/* 
 * _stp_lookup_bad_addr(): safely verify an address
 *
 * type: memory access type (either VERIFY_READ or VERIFY_WRITE)
 * size: number of bytes to verify
 * addr: address to verify
 * seg: memory segment to use, either kernel (STP_KERNEL_DS) or user
 * (STP_USER_DS)
 * 
 * The macro returns 0 if the address is valid, non-zero otherwise.
 * Note that the kernel will not pagefault when trying to verify the
 * memory. Also note that no DEREF_FAULT will occur when verifying the
 * memory.
 */

static inline int _stp_lookup_bad_addr_(int type, size_t size,
                                        uintptr_t addr, stp_mm_segment_t seg)
{
  int bad;
#ifdef STAPCONF_SET_FS
  mm_segment_t oldfs = get_fs();

  set_fs(seg);
#endif
  pagefault_disable();
  bad = lookup_bad_addr(type, addr, size, seg);
  pagefault_enable();
#ifdef STAPCONF_SET_FS
  set_fs(oldfs);
#endif

  return bad;
}

#define _stp_lookup_bad_addr(type, size, addr, seg) \
  _stp_lookup_bad_addr_((type), (size), (uintptr_t)(addr), (seg))


/* 
 * _stp_deref_nofault(): safely read a simple type from memory without
 * a DEREF_FAULT on error
 *
 * value: read the simple type into this variable
 * size: number of bytes to read
 * addr: address to read from
 * seg: memory segment to use, either kernel (STP_KERNEL_DS) or user
 * (STP_USER_DS)
 * 
 * If this macro gets an error while trying to read memory, nonzero is
 * returned. On success, 0 is return. Note that the kernel will not
 * pagefault when trying to read the memory.
 */

static inline int _stp_deref_nofault_(u64 *pv, size_t size, void *addr,
				      stp_mm_segment_t seg)
{
  int r = -EFAULT;
#ifdef STAPCONF_SET_FS
  mm_segment_t oldfs = get_fs();

  set_fs(seg);
#endif
  pagefault_disable();
  if (lookup_bad_addr(VERIFY_READ, (uintptr_t)addr, size, seg))
    r = -EFAULT;
  else
    r = __stp_deref_nocheck_(pv, size, addr, seg);
  pagefault_enable();
#ifdef STAPCONF_SET_FS
  set_fs(oldfs);
#endif

  return r;
}

#define _stp_deref_nofault(value, size, addr, seg)			\
  ({									\
    u64 _v = 0; int _e = -EFAULT;					\
    switch (size)							\
      {									\
      case 1: case 2: case 4: case 8:					\
	_e = _stp_deref_nofault_					\
		(&_v, (size), (void *)(uintptr_t)(addr), (seg));	\
	break;								\
      default:								\
	__stp_get_user_bad();						\
      }									\
    (value) = _v;							\
    _e;									\
  })


/* 
 * _stp_deref(): safely read a simple type from memory
 *
 * size: number of bytes to read
 * addr: address to read from
 * seg: memory segment to use, either kernel (STP_KERNEL_DS) or user
 * (STP_USER_DS)
 * 
 * The macro returns the value read. If this macro gets an error while
 * trying to read memory, a DEREF_FAULT will occur. Note that the
 * kernel will not pagefault when trying to read the memory.
 */

#define _stp_deref(size, addr, seg)					\
  ({									\
    u64 _v = 0; int _e = -EFAULT;					\
    switch (size)							\
      {									\
      case 1: case 2: case 4: case 8:					\
	_e = _stp_deref_nofault_					\
		(&_v, (size), (void *)(uintptr_t)(addr), (seg));	\
	break;								\
      default:								\
	__stp_get_user_bad();						\
      }									\
    if (_e)								\
      DEREF_FAULT(addr);						\
    _v;									\
  })


/* 
 * __stp_store_deref_nocheck(): writes a simple type to a location
 * with no address sanity checking.
 *
 * size: number of bytes to write
 * addr: address to write to
 * value: read the simple type from this variable
 * seg: memory segment to use, either kernel (STP_KERNEL_DS) or user
 * (STP_USER_DS)
 *
 * Note that the caller *must* check the address for validity and do
 * any other checks necessary. This macro is designed to be used as
 * the base for the other macros more suitable for the rest of the
 * code to use. Note that the caller is also responsible for ensuring
 * that the kernel doesn't pagefault while writing.
 */

static inline int __stp_store_deref_nocheck_(size_t size, void *addr,
                                             u64 v, stp_mm_segment_t seg)
{
  int r;
  switch (size)
    {
    case 1: r = __stp_put_either((u8)v, (u8 *)addr, seg); break;
    case 2: r = __stp_put_either((u16)v, (u16 *)addr, seg); break;
    case 4: r = __stp_put_either((u32)v, (u32 *)addr, seg); break;
#if defined(__i386__) || defined(__arm__)
    /* x86 and arm can't do 8-byte put_user, so we have to split it */
    default: { union { u32 l[2]; u64 ll; } val;
	       val.ll = v;
	       r = __stp_put_either(val.l[0], &((u32 *)addr)[0], seg);
	       if (! r)
                 r = __stp_put_either(val.l[1], &((u32 *)addr)[1], seg);
	     } break;
#else
    default: r = __stp_put_either(v, (u64 *)addr, seg); break;
#endif
    }
  return r;
}

#define __stp_store_deref_nocheck(size, addr, value, seg)               \
  ({									\
    int _e = -EFAULT;							\
    switch (size)							\
      {									\
      case 1: case 2: case 4: case 8:					\
	_e = __stp_store_deref_nocheck_					\
          ((size), (void *)(uintptr_t)(addr), (value), (seg));		\
	break;								\
      default:								\
        __stp_put_user_bad();						\
      }									\
    _e;									\
  })


/* 
 * _stp_store_deref(): safely write a simple type to memory
 *
 * size: number of bytes to write
 * addr: address to write to
 * value: read the simple type from this variable
 * seg: memory segment to use, either kernel (STP_KERNEL_DS) or user
 * (STP_USER_DS)
 * 
 * The macro has no return value. If this macro gets an error while
 * trying to write, a STORE_DEREF_FAULT will occur. Note that the
 * kernel will not pagefault when trying to write the memory.
 */

static inline int _stp_store_deref_(size_t size, void *addr, u64 v,
				    stp_mm_segment_t seg)
{
  int r;
#ifdef STAPCONF_SET_FS
  mm_segment_t oldfs = get_fs();

  set_fs(seg);
#endif
  pagefault_disable();
  if (lookup_bad_addr(VERIFY_READ, (uintptr_t)addr, size, seg))
    r = -EFAULT;
  else
    r = __stp_store_deref_nocheck_(size, addr, v, seg);
  pagefault_enable();
#ifdef STAPCONF_SET_FS
  set_fs(oldfs);
#endif

  return r;
}

#define _stp_store_deref(size, addr, value, seg)			\
  ({									\
    int _e = -EFAULT;							\
    switch (size)							\
      {									\
      case 1: case 2: case 4: case 8:					\
	_e = _stp_store_deref_						\
		((size), (void *)(uintptr_t)(addr), (value), (seg));	\
	break;								\
      default:								\
        __stp_put_user_bad();						\
      }									\
    if (_e)								\
      STORE_DEREF_FAULT(addr);						\
    (void)0;								\
  })


/* Map kderef/uderef to the generic segment-aware deref macros. */ 

#ifndef kderef
#define kderef(s,a) _stp_deref(s,a,STP_KERNEL_DS)
#endif

#ifndef store_kderef
#define store_kderef(s,a,v) _stp_store_deref(s,a,v,STP_KERNEL_DS)
#endif

#ifndef uderef
#define uderef(s,a) _stp_deref(s,a,STP_USER_DS)
#endif

#ifndef store_uderef
#define store_uderef(s,a,v) _stp_store_deref(s,a,v,STP_USER_DS)
#endif

#ifndef CONFIG_64BIT

/* The kderef/uderef macros (which is what Xderef gets set to), alway
 * returns a 64-bit value. This causes a problem on a 32-bit system
 * when we want to cast the 64-bit value to a 32-bit pointer - gcc
 * gives a "cast to pointer from integer of different size" error. So,
 * we'll cast it to a u32 before doing the final cast to the actual
 * type. */

#define __Xread(ptr, Xderef)						\
  ((sizeof(*(ptr)) == 8)						\
   ? *(typeof(ptr))&(u64) { Xderef(sizeof(*(ptr)), (ptr)) }		\
   : *(typeof(ptr))&(u32) { (u32) Xderef(sizeof(*(ptr)), (ptr)) } )

/* For __Xwrite, we need to handle the case where 'value' is a pointer
 * and avoid the "cast from pointer to integer of different size" gcc
 * errors. */

#define __Xwrite(ptr, value, store_Xderef)				  \
  ({									  \
    if (sizeof(*(ptr)) == 8) {						  \
      union { typeof(*(ptr)) v; u64 l; } _kw;				  \
      _kw.v = (typeof(*(ptr)))(value);					  \
      store_Xderef(8, (ptr), _kw.l);					  \
    } else								  \
      store_Xderef(sizeof(*(ptr)), (ptr), (long)(typeof(*(ptr)))(value)); \
  })

#else

#define __Xread(ptr, Xderef) \
  ( (typeof(*(ptr))) Xderef(sizeof(*(ptr)), (ptr)) )
#define __Xwrite(ptr, value, store_Xderef) \
  ( store_Xderef(sizeof(*(ptr)), (ptr), (long)(typeof(*(ptr)))(value)) )

#endif

#define kread(ptr) __Xread((ptr), kderef)
#define uread(ptr) __Xread((ptr), uderef)

#define kwrite(ptr, value) __Xwrite((ptr), (value), store_kderef)
#define uwrite(ptr, value) __Xwrite((ptr), (value), store_uderef)


/* Dereference a kernel buffer ADDR of size MAXBYTES. Put the bytes in
 * address DST (which can be NULL).
 *
 * This function is useful for reading memory when the size isn't a
 * size that kderef() handles.  This function is very similar to
 * kderef_string(), but kderef_buffer() doesn't quit when finding a
 * '\0' byte or append a '\0' byte.
 */

static inline char *kderef_buffer_(char *dst, void *addr, size_t len)
{
  int err = 0;
  size_t i;
#ifdef STAPCONF_SET_FS
  mm_segment_t oldfs = get_fs();

  set_fs(KERNEL_DS);
#endif
  pagefault_disable();
  if (lookup_bad_addr(VERIFY_READ, (uintptr_t)addr, len, STP_KERNEL_DS))
    err = 1;
  else
    for (i = 0; i < len; ++i)
      {
	u8 v;
	err = __stp_get_either(v, (u8 *)addr + i, STP_KERNEL_DS);
	if (err)
	  break;
	if (dst)
	  *dst++ = v;
      }
  pagefault_enable();
#ifdef STAPCONF_SET_FS
  set_fs(oldfs);
#endif

  return err ? (char *)-1 : dst;
}

#define kderef_buffer(dst, addr, maxbytes)				\
  ({									\
    char *_r = kderef_buffer_((dst), (void *)(uintptr_t)(addr), (maxbytes)); \
    if (_r == (char *)-1)						\
      DEREF_FAULT(addr);						\
    _r;									\
  })

/* 
 * _stp_deref_string_nofault(): safely read a string from memory
 * without a DEREF_FAULT on error
 *
 * dst: read the string into this address
 * addr: address to read from
 * len: maximum number of bytes to store in dst, including the trailing NUL
 * seg: memory segment to use, either kernel (STP_KERNEL_DS) or user
 * (STP_USER_DS)
 * 
 * If this function gets an error while trying to read memory, -EFAULT
 * is returned. On success, the number of bytes copied is returned
 * (not including the trailing NULL). Note that the kernel will not
 * pagefault when trying to read the string.
 */

static inline long _stp_deref_string_nofault(char *dst, const char *addr,
					     size_t len, stp_mm_segment_t seg)
{
  int err = 0;
  size_t i = 0;
#ifdef STAPCONF_SET_FS
  mm_segment_t oldfs = get_fs();

  set_fs(seg);
#endif
  pagefault_disable();
  if (lookup_bad_addr(VERIFY_READ, (uintptr_t)addr, len, seg))
    err = 1;
  else
    {
      /* Reduce len by 1 to leave room for '\0' terminator. */
      for (i = 0; i + 1 < len; ++i)
	{
	  u8 v;
	  err = __stp_get_either(v, (u8 *)addr + i, seg);
	  if (err || v == '\0')
	    break;
	  if (dst)
	    *dst++ = v;
	}
      if (!err && dst)
	*dst = '\0';
    }
  pagefault_enable();
#ifdef STAPCONF_SET_FS
  set_fs(oldfs);
#endif

  return err ? -EFAULT : i;
}

#define kderef_string(dst, addr, maxbytes)				\
  ({									\
    long _r = _stp_deref_string_nofault((dst), (void *)(uintptr_t)(addr), (maxbytes), STP_KERNEL_DS); \
    if (_r < 0)								\
      DEREF_FAULT(addr);						\
    _r;									\
  })

/* 
 * _stp_store_deref_string(): safely write a string to memory
 *
 * src: source string
 * addr: address to write to
 * maxbytes: maximum number of bytes to write
 * seg: memory segment to use, either kernel (STP_KERNEL_DS) or user
 * (STP_USER_DS)
 * 
 * The macro has no return value. If this macro gets an error while
 * trying to write, a STORE_DEREF_FAULT will occur. Note that the
 * kernel will not pagefault when trying to write the memory.
 */

static inline int _stp_store_deref_string_(char *src, void *addr, size_t len,
					   stp_mm_segment_t seg)
{
  int err = 0;
  size_t i;
#ifdef STAPCONF_SET_FS
  mm_segment_t oldfs = get_fs();

  set_fs(seg);
#endif
  pagefault_disable();
  if (lookup_bad_addr(VERIFY_WRITE, (uintptr_t)addr, len, seg))
    err = 1;
  else if (len > 0)
    {
      for (i = 0; i < len - 1; ++i)
	{
	  err = __stp_put_either(*src++, (u8 *)addr + i, seg);
	  if (err)
	    goto out;
	}
      err = __stp_put_either('\0', (u8 *)addr + i, seg);
    }
 out:
  pagefault_enable();
#ifdef STAPCONF_SET_FS
  set_fs(oldfs);
#endif

  return err;
}

#define _stp_store_deref_string(src, addr, maxbytes, seg)		\
  ({									\
    if (_stp_store_deref_string_					\
	((src), (void *)(uintptr_t)(addr), (maxbytes), (seg)))		\
      STORE_DEREF_FAULT(addr);						\
    (void)0;								\
  })


/* 
 * store_kderef_string(): safely write a string to kernel memory
 *
 * src: source string
 * addr: address to write to
 * maxbytes: maximum number of bytes to write
 * 
 * The macro has no return value. If this macro gets an error while
 * trying to write, a STORE_DEREF_FAULT will occur. Note that the
 * kernel will not pagefault when trying to write the memory.
 */

#define store_kderef_string(src, addr, maxbytes)                              \
  _stp_store_deref_string((src), (addr), (maxbytes), STP_KERNEL_DS)


/* 
 * store_uderef_string(): safely write a string to user memory
 *
 * src: source string
 * addr: address to write to
 * maxbytes: maximum number of bytes to write
 * 
 * The macro has no return value. If this macro gets an error while
 * trying to write, a STORE_DEREF_FAULT will occur. Note that the
 * kernel will not pagefault when trying to write the memory.
 */

#define store_uderef_string(src, addr, maxbytes)                              \
  _stp_store_deref_string((src), (addr), (maxbytes), STP_USER_DS)

#endif /* _LINUX_LOC2C_RUNTIME_H_ */
