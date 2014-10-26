/****************************************************************************/
/*                                                                          */
/*  (c) Copyright 2014 by Jeffrey Scholz                                    */
/*                                                                          */
/*  Permission to use this file for any purpose is granted provided that    */
/*  this clause is retained in the redistribution, and the copyright and    */
/*  licence of concorde is respected.                                       */
/*                                                                          */
/*  Use at your own risk.  We make no guarantees about the                  */
/*  correctness or usefuleness of this code.                                */
/*                                                                          */
/****************************************************************************/

#include "bigguy.h"
#include "machdefs.h"
#include "util.h"

#define SAFE( ret ) if (ret) { \
			printf("memory error line: %d\n", __LINE__); \
			abort();}
#define SAFE_MALLOC( ret ) if (!ret) { \
			printf("heap error line: %d\n", __LINE__); \
			abort();}

int main(void) {

	// write 7 bigguys to a file and create array x[6] of bigguys
	const int no_bigguys = 7;
	CC_SFILE *ccfile = CCutil_sopen ("tempfile", "w");
	CCbigguy *pos_i_bg = malloc(sizeof(CCbigguy) * 6);
	SAFE_MALLOC(pos_i_bg);
	double pos_i_d[no_bigguys] = {(double) 0x7fff0000, (double) 0xffff,
	(double) 0x7f000000, (double) 0xff0000, (double) 0xff, (double) 0xff00,
	(double) -0x77.77p0};

	for (int i = 0; i < no_bigguys; ++i) {
		pos_i_bg[i] = CCbigguy_dtobigguy (pos_i_d[i]);
		SAFE(CCbigguy_swrite (ccfile, pos_i_bg[i]));
	}
	SAFE(CCutil_sclose (ccfile));


	// read the file
	CC_SFILE *nccfile = CCutil_sopen ("tempfile", "r");
	CCbigguy *temp = malloc(sizeof(CCbigguy));
	SAFE_MALLOC(temp);

	for (int i = 0; i < no_bigguys; ++i) {
		SAFE(CCbigguy_sread (nccfile, temp));
		if (0 != CCbigguy_cmp(pos_i_bg[i], *temp)) {
			printf("reading bigguys failed!\n");
		}
		else
			printf("PASS cmp %d/%d\n", i, no_bigguys);
	}
	SAFE(CCutil_sclose (nccfile));

	CCbigguy *expected = malloc(sizeof(CCbigguy));
	CCbigguy *temp1 = malloc(sizeof(CCbigguy));

	// test adding to max value
	*temp = CCbigguy_dtobigguy((double) 0x7fff0000);
	*temp1 = CCbigguy_dtobigguy((double) 0xffff);
	*expected = CCbigguy_dtobigguy((double) 0x7fffffff);
	CCbigguy_add(temp, *temp1);
	if (0 != CCbigguy_cmp(*temp, *expected)) {
		printf("adding ilo to ihi without carry to max value failed\n");
	}
	else
		printf("PASS 0x7fff0000 + 0xffff\n");

	// test only adding ihi
	*temp = CCbigguy_dtobigguy((double) 0x7f000000);
	*temp1 = CCbigguy_dtobigguy((double) 0xff0000);
	*expected = CCbigguy_dtobigguy((double) 0x7fff0000);
	CCbigguy_add(temp, *temp1);
	if (0 != CCbigguy_cmp(*temp, *expected)) {
		printf("adding in ihi failed\n");
	}
	else
		printf("PASS 0x7f000000 + 0xff0000\n");

	// test only adding ilo
	*temp = CCbigguy_dtobigguy((double) 0xff);
	*temp1 = CCbigguy_dtobigguy((double) 0xff00);
	*expected = CCbigguy_dtobigguy((double) 0xffff);
	CCbigguy_add(temp, *temp1);
	if (0 != CCbigguy_cmp(*temp, *expected)) {
		printf("adding in ihi failed\n");
	}
	else
		printf("PASS 0xff + 0xff00\n");


	// adding carry from ilo to ihi
	*temp = CCbigguy_dtobigguy((double) 0xffff);
	*temp1 = CCbigguy_dtobigguy((double) 0xffff);
	*expected = CCbigguy_dtobigguy((double) 0x1fffe);
	CCbigguy_add(temp, *temp1);
	if (0 != CCbigguy_cmp(*temp, *expected)) {
		printf("adding in ihi failed\n");
	}
	else
		printf("PASS 0xffff + 0xffff\n");

	// subtracting carry from ihi to ilo
	*temp = CCbigguy_dtobigguy(65536.0);
	*temp1 = CCbigguy_dtobigguy(1.0);
	*expected = CCbigguy_dtobigguy(65535.0);
	CCbigguy_sub(temp, *temp1);
	if (0 != CCbigguy_cmp(*temp, *expected)) {
		printf("subtracting carry from ihi to ilo failed\n");
	}
	else
		printf("PASS 0x10000 - 1\n");

	// multiplying carry from ilo to ihi
	*temp = CCbigguy_dtobigguy(0.0);
	*temp1 = CCbigguy_dtobigguy((double) 0xffff);
	*expected = CCbigguy_dtobigguy((double) 131070);
	CCbigguy_addmult(temp, *temp1, (int) 0x2);
	if (0 != CCbigguy_cmp(*temp, *expected)) {
		printf("multipying carry from ihi to ilo failed\n");
	}
	else
		printf("PASS 0xffff * 0x2\n");

	// multiplying carry from flo to fhi
	*temp = CCbigguy_dtobigguy(0.0);
	*temp1 = CCbigguy_dtobigguy((double) 0x0.0000ffffp0);
	*expected = CCbigguy_dtobigguy((double) 0x0.0001fffep0);
	CCbigguy_addmult(temp, *temp1, (int) 0x2);
	if (0 != CCbigguy_cmp(*temp, *expected)) {
		printf("multipying carry from flo to fhi failed\n");
	}
	else
		printf("PASS carry from flo to fhi\n");

	// multiplying carry from fhi to ilo
	*temp = CCbigguy_dtobigguy(0.0);
	*temp1 = CCbigguy_dtobigguy((double) 0x0.ffffp0);
	*expected = CCbigguy_dtobigguy((double) 0x1.fffep0);
	CCbigguy_addmult(temp, *temp1, (int) 0x2);
	if (0 != CCbigguy_cmp(*temp, *expected)) {
		printf("multipying carry from fhi to ilo failed\n");
	}
	else
		printf("PASS carry from fhi to ilo \n");

	// negative 0 version 1
	*temp = CCbigguy_dtobigguy(0.0);
	*temp1 = CCbigguy_dtobigguy(-1.0);
	*expected = CCbigguy_dtobigguy(0.0);
	CCbigguy_addmult(temp, *temp1, 0);
	if (0 != CCbigguy_cmp(*temp, *expected)) {
		printf("negative zero failed\n");
	}
	else
		printf("PASS 0 == -0\n");

	// negative 0 version 2
	*temp = CCbigguy_dtobigguy(0.0);
	*temp1 = CCbigguy_dtobigguy(0.0);
	*expected = CCbigguy_dtobigguy(0.0);
	CCbigguy_addmult(temp, *temp1, -1);
	if (0 != CCbigguy_cmp(*temp, *expected)) {
		printf("negative zero failed\n");
	}
	else
		printf("PASS 0 == -0\n");

	// negative no overflow
	*temp = CCbigguy_dtobigguy(0.0);
	*temp1 = CCbigguy_dtobigguy((double) 0xfffff.fffffp0);
	*expected = CCbigguy_dtobigguy((double) -0xfffff.fffffp0);
	CCbigguy_addmult(temp, *temp1, -1);
	if (0 != CCbigguy_cmp(*temp, *expected)) {
		printf("sign change failed\n");
	}
	else
		printf("PASS sign change\n");

	// carry from fhi to ilo
	*temp = CCbigguy_dtobigguy((double) 0x0.ffffp0);
	*temp1 = CCbigguy_dtobigguy((double) 0x0.00ffp0);
	*expected = CCbigguy_dtobigguy((double) 0x1.00fep0);
	CCbigguy_add(temp, *temp1);
	if (0 != CCbigguy_cmp(*temp, *expected)) {
		printf("carry from fhi to ilo failed\n");
	}
	else
		printf("PASS carry from fhi to ilo\n");

	// carry from flo to fhi
	*temp = CCbigguy_dtobigguy((double) 0x0.0000ffffp0);
	*temp1 = CCbigguy_dtobigguy((double) 0x0.00000001p0);
	*expected = CCbigguy_dtobigguy((double) 0x.00010000p0);
	CCbigguy_add(temp, *temp1);
	if (0 != CCbigguy_cmp(*temp, *expected)) {
		printf("carry from flo to fhi failed\n");
	}
	else
		printf("PASS carry from flo to fhi\n");

	// carry from flo to fhi to ilo to ihi
	*temp = CCbigguy_dtobigguy((double) 0xffff.ffffffffp0);
	*temp1 = CCbigguy_dtobigguy((double) 0x0.00000001p0);
	*expected = CCbigguy_dtobigguy((double) 0x10000);
	CCbigguy_add(temp, *temp1);
	if (0 != CCbigguy_cmp(*temp, *expected)) {
		printf("carry from flo to ihi failed\n");
	}
	else
		printf("PASS carry from flo to ihi\n");

	// subtraction accross zero
	*temp = CCbigguy_dtobigguy((double) 0xfffff);
	*temp1 = CCbigguy_dtobigguy((double) 0x1fffff);
	*expected = CCbigguy_dtobigguy((double) -0x100000);
	CCbigguy_sub(temp, *temp1);
	if (0 != CCbigguy_cmp(*temp, *expected)) {
		printf("subtract accross 0 failed\n");
	}
	else
		printf("PASS subtact accross 0\n");

	// add accross zero
	*temp = CCbigguy_dtobigguy((double) -0x100000);
	*temp1 = CCbigguy_dtobigguy((double) 0x1fffff);
	*expected = CCbigguy_dtobigguy((double) 0xfffff);
	CCbigguy_add(temp, *temp1);
	if (0 != CCbigguy_cmp(*temp, *expected)) {
		printf("add accross 0 failed\n");
	}
	else
		printf("PASS add accross 0\n");

	// sign change from max to -max
	*temp = CCbigguy_dtobigguy(0.0);
	*temp1 = CCbigguy_dtobigguy((double) 0x7fffffff);
	*expected = CCbigguy_dtobigguy((double) -0x7fffffff);
	CCbigguy_addmult(temp, *temp1, -1);
	if (0 != CCbigguy_cmp(*temp, *expected)) {
		printf("max to -max (int) failed\n");
	}
	else
		printf("PASS max to -max (int)\n");

	free(temp);
	free(temp1);
	free(expected);
	free(pos_i_bg);
}
