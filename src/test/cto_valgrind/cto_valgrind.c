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
 * cto_valgrind.c -- unit test for Valgrind instrumentation in libpmemcto
 *
 * usage: cto_valgrind filename <test-number>
 *
 * test-number can be a number from 0 to 5
 */

#include "unittest.h"

int
main(int argc, char *argv[])
{
	START(argc, argv, "cto_valgrind");

	if (argc != 3)
		UT_FATAL("usage: %s filename <test-number>", argv[0]);

	PMEMctopool *pcp = pmemcto_create(argv[1], "test",
			PMEMCTO_MIN_POOL, 0600);
	UT_ASSERTne(pcp, NULL);

	int test_case = atoi(argv[2]);
	int *ptr;

	switch (test_case) {
		case 0: {
			UT_OUT("remove all allocations and delete pool");
			ptr = pmemcto_malloc(pcp, sizeof(int));
			UT_ASSERTne(ptr, NULL);

			pmemcto_free(pcp, ptr);
			pmemcto_close(pcp);
			break;
		}
		case 1: {
			UT_OUT("only remove allocations");
			ptr = pmemcto_malloc(pcp, sizeof(int));
			UT_ASSERTne(ptr, NULL);

			pmemcto_free(pcp, ptr);
			break;
		}
		case 2: {
			UT_OUT("only delete pool");
			ptr = pmemcto_malloc(pcp, sizeof(int));
			UT_ASSERTne(ptr, NULL);

			pmemcto_close(pcp);

			/* prevent reporting leaked memory as still reachable */
			ptr = NULL;
			break;
		}
		case 3: {
			UT_OUT("memory leaks");
			ptr = pmemcto_malloc(pcp, sizeof(int));
			UT_ASSERTne(ptr, NULL);

			/* prevent reporting leaked memory as still reachable */
			ptr = NULL;
			break;
		}
		case 4: {
			UT_OUT("heap block overrun");
			ptr = pmemcto_malloc(pcp, 12 * sizeof(int));
			UT_ASSERTne(ptr, NULL);

			/* heap block overrun */
			ptr[12] = 7;

			pmemcto_free(pcp, ptr);
			pmemcto_close(pcp);
			break;
		}
		default: {
			UT_FATAL("unknown test-number");
		}
	}

	DONE(NULL);
}
