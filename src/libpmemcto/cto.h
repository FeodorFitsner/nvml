/*
 * Copyright 2014-2017, Intel Corporation
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in
 *       the documentation and/or other materials provided with the
 *       distribution.
 *
 *     * Neither the name of the copyright holder nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * cto.h -- internal definitions for libpmem cto module
 */
#ifndef LIBPMEMCTO_CTO_H
#define LIBPMEMCTO_CTO_H 1

#include "os_thread.h"
#include "util.h"
#include "pool_hdr.h"

#define PMEMCTO_LOG_PREFIX "libpmemcto"
#define PMEMCTO_LOG_LEVEL_VAR "PMEMCTO_LOG_LEVEL"
#define PMEMCTO_LOG_FILE_VAR "PMEMCTO_LOG_FILE"

/* attributes of the cto memory pool format for the pool header */
#define CTO_HDR_SIG "PMEMCTO"	/* must be 8 bytes including '\0' */
#define CTO_FORMAT_MAJOR 1
#define CTO_FORMAT_COMPAT 0x0000
#define CTO_FORMAT_INCOMPAT 0x0000
#define CTO_FORMAT_RO_COMPAT 0x0000

/* size of the persistent part of PMEMOBJ pool descriptor (2kB) */
#define CTO_DSC_P_SIZE		2048
/* size of unused part of the persistent part of PMEMOBJ pool descriptor */
#define CTO_DSC_P_UNUSED	(CTO_DSC_P_SIZE - PMEMCTO_MAX_LAYOUT - 36)

/*
 * XXX: CTO
 * We don't care about portable data types, as the pool may only be open
 * on the same platform.
 */
struct pmemcto {
	struct pool_hdr hdr;	/* memory pool header */

	/* persistent part of PMEMCTO pool descriptor (2kB) */
	char layout[PMEMCTO_MAX_LAYOUT];
	void *addr;		/* mapped region */
	size_t size;		/* size of mapped region */
	void *root;		/* root pointer */
	/* XXX - to be replaced with shutdown state in the pool header */
	int consistent;		/* successfully flushed before exit */
	unsigned char unused[CTO_DSC_P_UNUSED]; /* must be zero */
	uint64_t checksum;	/* checksum of above fields */

	/* some run-time state, allocated out of memory pool... */
	struct pool_set *set;	/* pool set info */
	int is_pmem;		/* true if pool is PMEM */
	int rdonly;		/* true if pool is opened read-only */
	int is_dev_dax;		/* true if mapped on device dax */
};

/* data area starts at this alignment after the struct pmemcto above */
#define CTO_FORMAT_DATA_ALIGN ((uintptr_t)4096)

#define CTO_DSC_SIZE (sizeof(struct pmemcto) - sizeof(struct pool_hdr))
#define CTO_DSC_SIZE_ALIGNED\
	roundup(sizeof(struct pmemcto), CTO_FORMAT_DATA_ALIGN)

void cto_init(void);
void cto_fini(void);

#endif /* LIBPMEMCTO_CTO_H */
