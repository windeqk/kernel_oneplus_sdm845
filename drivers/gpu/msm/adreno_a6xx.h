/* Copyright (c) 2018,2020, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef _ADRENO_A6XX_H_
#define _ADRENO_A6XX_H_

#include "a6xx_reg.h"

#define CP_CLUSTER_FE		0x0
#define CP_CLUSTER_SP_VS	0x1
#define CP_CLUSTER_PC_VS	0x2
#define CP_CLUSTER_GRAS		0x3
#define CP_CLUSTER_SP_PS	0x4
#define CP_CLUSTER_PS		0x5

/**
 * struct a6xx_cp_preemption_record - CP context record for
 * preemption.
 * @magic: (00) Value at this offset must be equal to
 * A6XX_CP_CTXRECORD_MAGIC_REF.
 * @info: (04) Type of record. Written non-zero (usually) by CP.
 * we must set to zero for all ringbuffers.
 * @errno: (08) Error code. Initialize this to A6XX_CP_CTXRECORD_ERROR_NONE.
 * CP will update to another value if a preemption error occurs.
 * @data: (12) DATA field in YIELD and SET_MARKER packets.
 * Written by CP when switching out. Not used on switch-in. Initialized to 0.
 * @cntl: (16) RB_CNTL, saved and restored by CP. We must initialize this.
 * @rptr: (20) RB_RPTR, saved and restored by CP. We must initialize this.
 * @wptr: (24) RB_WPTR, saved and restored by CP. We must initialize this.
 * @_pad28: (28) Reserved/padding.
 * @rptr_addr: (32) RB_RPTR_ADDR_LO|HI saved and restored. We must initialize.
 * rbase: (40) RB_BASE_LO|HI saved and restored.
 * counter: (48) Pointer to preemption counter.
 */
struct a6xx_cp_preemption_record {
	uint32_t  magic;
	uint32_t  info;
	uint32_t  errno;
	uint32_t  data;
	uint32_t  cntl;
	uint32_t  rptr;
	uint32_t  wptr;
	uint32_t  _pad28;
	uint64_t  rptr_addr;
	uint64_t  rbase;
	uint64_t  counter;
};

/**
 * struct a6xx_cp_smmu_info - CP preemption SMMU info.
 * @magic: (00) The value at this offset must be equal to
 * A6XX_CP_SMMU_INFO_MAGIC_REF.
 * @_pad4: (04) Reserved/padding
 * @ttbr0: (08) Base address of the page table for the
 * incoming context.
 * @context_idr: (16) Context Identification Register value.
 */
struct a6xx_cp_smmu_info {
	uint32_t  magic;
	uint32_t  _pad4;
	uint64_t  ttbr0;
	uint32_t  asid;
	uint32_t  context_idr;
};

#define A6XX_CP_SMMU_INFO_MAGIC_REF     0x241350D5UL

/**
 * struct cpu_gpu_spinlock - CP spinlock structure for power up list
 * @flag_ucode: flag value set by CP
 * @flag_kmd: flag value set by KMD
 * @turn: turn variable set by both CP and KMD
 * @list_length: this tells CP the last dword in the list:
 * 16 + (4 * (List_Length - 1))
 * @list_offset: this tells CP the start of preemption only list:
 * 16 + (4 * List_Offset)
 */
struct cpu_gpu_lock {
	uint32_t flag_ucode;
	uint32_t flag_kmd;
	uint32_t turn;
	uint16_t list_length;
	uint16_t list_offset;
};

#define A6XX_CP_CTXRECORD_MAGIC_REF     0xAE399D6EUL
/* Size of the preemption counter block (in bytes) */
#define A6XX_CP_CTXRECORD_PREEMPTION_COUNTER_SIZE   (16 * 4)
/* Size of the user context record block (in bytes) */
#define A6XX_CP_CTXRECORD_USER_RESTORE_SIZE (192 * 1024)
/* Size of the performance counter save/restore block (in bytes) */
#define A6XX_CP_PERFCOUNTER_SAVE_RESTORE_SIZE   (4 * 1024)

#define A6XX_CP_RB_CNTL_DEFAULT ((1 << 27) | ((ilog2(4) << 8) & 0x1F00) | \
		(ilog2(KGSL_RB_DWORDS >> 1) & 0x3F))

/* Preemption functions */
void a6xx_preemption_trigger(struct adreno_device *adreno_dev);
void a6xx_preemption_schedule(struct adreno_device *adreno_dev);
void a6xx_preemption_start(struct adreno_device *adreno_dev);
int a6xx_preemption_init(struct adreno_device *adreno_dev);

unsigned int a6xx_preemption_post_ibsubmit(struct adreno_device *adreno_dev,
		unsigned int *cmds);
unsigned int a6xx_preemption_pre_ibsubmit(struct adreno_device *adreno_dev,
		struct adreno_ringbuffer *rb,
		unsigned int *cmds, struct kgsl_context *context);

unsigned int a6xx_set_marker(unsigned int *cmds,
		enum adreno_cp_marker_type type);

void a6xx_preemption_callback(struct adreno_device *adreno_dev, int bit);

int a6xx_preemption_context_init(struct kgsl_context *context);

void a6xx_preemption_context_destroy(struct kgsl_context *context);

void a6xx_snapshot(struct adreno_device *adreno_dev,
		struct kgsl_snapshot *snapshot);
void a6xx_crashdump_init(struct adreno_device *adreno_dev);
#endif
