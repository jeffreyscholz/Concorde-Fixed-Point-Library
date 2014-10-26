/****************************************************************************/
/*                                                                          */
/*  This file is part of CONCORDE                                           */
/*                                                                          */
/*  (c) Copyright 1995--1999 by David Applegate, Robert Bixby,              */
/*  Vasek Chvatal, and William Cook                                         */
/*                                                                          */
/*  Permission is granted for academic research use.  For other uses,       */
/*  contact the authors for licensing options.                              */
/*                                                                          */
/*  Use at your own risk.  We make no guarantees about the                  */
/*  correctness or usefulness of this code.                                 */
/*                                                                          */
/*  ----                                                                    */
/*                                                                          */
/*  (c) Copyright 2014 by Jeffrey Scholz                                    */
/*  This file was modified by Jeffrey Scholz 2014 to support 128 bit        */
/*  fixed point arithmetic with 64 bits on both sides of the decimal        */
/*  point. Permission to use this modification for any purpose is granted   */
/*  provided that this clause is retained in the redistribution, and the    */
/*  copyright and licence of the concorde is respected.                     */
/*                                                                          */
/*  Use at your own risk.  We make no guarantees about the                  */
/*  correctness or usefuleness of this code.                                */
/*                                                                          */
/****************************************************************************/

/****************************************************************************/
/*                                                                          */
/*  BIGGUY OPERATIONS:                                                      */
/*                                                                          */
/*  x += y                  CCbigguy_add (CCbigguy *x, CCbigguy y)          */
/*  x -= y                  CCbigguy_sub (CCbigguy *x, CCbigguy y)          */
/*  x += y*m                CCbigguy_addmult (CCbigguy *x, CCbigguy y,      */
/*                                            int m)                        */
/*  x<y -1, x==y 0, x>y 1   CCbigguy_cmp (CCbigguy x, CCbigguy y)           */
/*  ceil(x)                 CCbigguy_ceil (CCbigguy x)                      */
/*                                                                          */
/*  (double) x              CCbigguy_bigguytod (CCbigguy x)                 */
/*  (bigguy) i              CCbigguy_itobigguy (int i)                      */
/*  (bigguy) d              CCbigguy_dtobigguy (double d)                   */
/*                                                                          */
/*  read(x)                 CCbigguy_sread (CC_SFILE *f, CCbigguy *x)       */
/*  write(x)                CCbigguy_swrite (CC_SFILE *f, CCbigguy x)       */
/*                                                                          */
/*  biggest possible x      const CCbigguy CCbigguy_MAXBIGGUY               */
/*  smallest possible x     const CCbigguy CCbigguy_MINBIGGUY               */
/*  0                       const CCbigguy CCbigguy_ZERO                    */
/*  1                       const CCbigguy CCbigguy_ONE                     */
/*                                                                          */
/*                                                                          */
/*    EXPORTED FUNCTIONS:                                                   */
/*                                                                          */
/*  int CCbigguy_swrite (CC_SFILE *f, CCbigguy x)                           */
/*    NONE                                                                  */
/*                                                                          */
/*  int CCbigguy_sread (CC_SFILE *f, CCbigguy *x)                           */
/*    NONE                                                                  */
/*                                                                          */
/*  void CCbigguy_addmult (CCbigguy *x, CCbigguy y, int m)                  */
/*    If an overflow occurs, an error message is output and the routine     */
/*    aborts.  If CC_BIGGUY_BUILTIN is defined, this is implemented by a    */
/*    macro, and has no overflow checking.                                  */
/*                                                                          */
/*  int CCbigguy_cmp (CCbigguy x, CCbigguy y)                               */
/*    If CC_BIGGUY_BUILTIN is defined, this is implemented by a macro,      */
/*    and has no overflow checking.                                         */
/*                                                                          */
/*  double CCbigguytod (CCbigguy x)                                         */
/*    If CC_BIGGUY_BUILTIN is defined, this is implemented by a macro,      */
/*    and has no overflow checking.                                         */
/*                                                                          */
/*  CCbigguy CCbigguy_itobigguy (int d)                                     */
/*    If CC_BIGGUY_BUILTIN is defined, this is implemented by a macro,      */
/*    and has no overflow checking.                                         */
/*                                                                          */
/*  CCbigguy CCbigguy_dtobigguy (double d)                                  */
/*    If an overflow occurs, an error message is output and the routine     */
/*    aborts.  If CC_BIGGUY_BUILTIN is defined, this is implemented by a    */
/*    macro, and has no overflow checking.                                  */
/*                                                                          */
/*  CCbigguy CCbigguy_ceil (CCbigguy x)                                     */
/*    If an overflow occurs, an error message is output and the routine     */
/*    aborts.  If CC_BIGGUY_BUILTIN is defined, this is implemented by a    */
/*    macro, and has no overflow checking.                                  */
/*                                                                          */
/*  If CC_BIGGUY_BUILTIN is defined, CC_BIGGUY will support up to 64        */
/*  bits (32 bits on both sides of the decimal).                            */
/*                                                                          */
/****************************************************************************/

#include "machdefs.h"
#include "util.h"
#include "bigguy.h"

#ifdef  CC_BIGGUY_BUILTIN

int CCbigguy_swrite (CC_SFILE *f, CCbigguy x)
{
    if (CCutil_swrite_ushort (f, (unsigned short) ((x>>48)&0xffff))) return -1;
    if (CCutil_swrite_ushort (f, (unsigned short) ((x>>32)&0xffff))) return -1;
    if (CCutil_swrite_ushort (f, (unsigned short) ((x>>16)&0xffff))) return -1;
    if (CCutil_swrite_ushort (f, (unsigned short) (x&0xffff))) return -1;

    return 0;
}

int CCbigguy_sread (CC_SFILE *f, CCbigguy *x)
{
    unsigned short y;

    if (CCutil_sread_ushort (f, &y)) return -1;
    *x = ((CCbigguy) y) << 48;
    if (CCutil_sread_ushort (f, &y)) return -1;
    *x |= ((CCbigguy) y) << 32;
    if (CCutil_sread_ushort (f, &y)) return -1;
    *x |= ((CCbigguy) y) << 16;
    if (CCutil_sread_ushort (f, &y)) return -1;
    *x |= ((CCbigguy) y);
    return 0;
}

#else  /* CC_BIGGUY_BUILTIN */

const CCbigguy CCbigguy_MINBIGGUY = {0x80000000,0x00000000,0x00000000,0x00000001};
const CCbigguy CCbigguy_MAXBIGGUY = {0x7fffffff,0xffffffff,0xffffffff,0xffffffff};
const CCbigguy CCbigguy_ZERO = {0,0,0,0};
const CCbigguy CCbigguy_ONE = {0,1,0,0};

static void
    bigguy_neg (CCbigguy *x);

static void bigguy_neg (CCbigguy *x)
{
    x->ihi = ((unsigned int) BIT_32_UMAX) - (unsigned int) x->ihi;
    x->ilo = ((unsigned int) BIT_32_UMAX) - (unsigned int) x->ilo;
    x->fhi = ((unsigned int) BIT_32_UMAX) - (unsigned int) x->fhi;
    x->flo = ((unsigned int) BIT_32_UMAX) - (unsigned int) x->flo;

    if ((unsigned int) x->flo < BIT_32_UMAX) {
        x->flo = (unsigned int) x->flo + 1;
    } else {
        x->flo = 0;
        if ((unsigned int) x->fhi < BIT_32_UMAX) {
            x->fhi = (unsigned int) x->fhi + 1;
        } else {
            x->fhi = 0;
            if ((unsigned int) x->ilo < BIT_32_UMAX) {
                x->ilo = (unsigned int) x->ilo + 1;
            } else {
                x->ilo = 0;
                if ((unsigned int) x->ihi == BIT_32_SMAX) {
                    fprintf (stderr, "OVERFLOW in bigguy_neg\n");
                    fprintf (stderr, "BIGGUY errors are fatal\n");
                    abort ();
                } else if ((unsigned int) x->ihi < BIT_32_UMAX) {
                    x->ihi = (unsigned int) x->ihi + 1;
                } else {
                    x->ihi = 0;
                }
            }
        }
    }
}

int CCbigguy_swrite (CC_SFILE *f, CCbigguy x)
{
    if (CCutil_swrite_uint (f, x.ihi)) return -1;
    if (CCutil_swrite_uint (f, x.ilo)) return -1;
    if (CCutil_swrite_uint (f, x.fhi)) return -1;
    if (CCutil_swrite_uint (f, x.flo)) return -1;

    return 0;
}

int CCbigguy_sread (CC_SFILE *f, CCbigguy *x)
{
    if (CCutil_sread_uint (f, &(x->ihi))) return -1;
    if (CCutil_sread_uint (f, &(x->ilo))) return -1;
    if (CCutil_sread_uint (f, &(x->fhi))) return -1;
    if (CCutil_sread_uint (f, &(x->flo))) return -1;
    return 0;
}

double CCbigguy_bigguytod (CCbigguy x)
{
    int sgn = 1;

    if ((unsigned int) x.ihi >= 2147483648) {
        bigguy_neg (&x);
        sgn = -1;
    }

    return sgn * (((double) x.ihi) * (BIT_32_UMAXF+1) +
                  ((double) x.ilo) +
                  ((double) x.fhi) / (BIT_32_UMAXF+1) +
                  ((double) x.flo) / ((BIT_32_UMAXF+1)*(BIT_32_UMAXF+1)));
}

// note that this function will report an overflow in int,
// not in bigguy.
CCbigguy CCbigguy_itobigguy (int d)
{
    CCbigguy x;
    int sgn;

    if (d < 0) {
        d = -d;
        sgn = -1;
    } else {
        sgn = 1;
    }

    if (d < 0 || (d >> 16) >= 32768) {
        fprintf (stderr, "OVERFLOW in CCbigguy_itobigguy\n");
        fprintf (stderr, "BIGGUY errors are fatal\n");
        fprintf (stderr, "An overflowed integer was passed in\n");
        abort ();
    }
    x.ihi = ((unsigned int) (0x00000000));
    x.ilo = ((unsigned int) (d & 0xffffffff));
    x.fhi = 0;
    x.flo = 0;

    if (sgn == -1) {
        bigguy_neg (&x);
    }
    return x;
}

CCbigguy CCbigguy_dtobigguy (double d)
{
    CCbigguy x;
    int sgn;

    if (d < 0.0) {
        d = -d;
        sgn = -1;
    } else {
        sgn = 1;
    }

    if (d / (BIT_32_UMAXF+1) >= (BIT_32_SMAXF+1)) {
        fprintf (stderr, "OVERFLOW in CCbigguy_dtobigguy (%.6f)\n", d * sgn);
        fprintf (stderr, "BIGGUY errors are fatal\n");
        abort ();
    }

    x.ihi = ((unsigned int) (d / (BIT_32_UMAXF+1)));
    d -= ((double) x.ihi) * (BIT_32_UMAXF+1);
    x.ilo = ((unsigned int) d);
    d -= ((double) x.ilo);
    x.fhi = ((unsigned int) (d * (BIT_32_UMAXF+1)));
    d -= ((double) x.fhi) / (BIT_32_UMAXF+1);
    x.flo = ((unsigned int) (d * (BIT_32_UMAXF+1) * (BIT_32_UMAXF+1)));

    if (sgn == -1) {
        bigguy_neg (&x);
    }

    return x;
}

CCbigguy CCbigguy_ceil (CCbigguy x)
{
    if ((unsigned int) x.fhi || (unsigned int) x.flo) {
        x.fhi = 0;
        x.flo = 0;
        x.ilo = (unsigned int) x.ilo + 1;
        if ((unsigned int) x.ilo == 0) {
            if ((unsigned int) x.ihi == BIT_32_SMAX) {
                fprintf (stderr, "OVERFLOW in CCbigguy_ceil\n");
                fprintf (stderr, "BIGGUY errors are fatal\n");
                abort ();
            }
            x.ihi = (unsigned int) x.ihi + 1;
        }
    }

    return x;
}

int CCbigguy_cmp (CCbigguy x, CCbigguy y)
{
    if ((unsigned int) x.ihi >= 2147483648 && (unsigned int) y.ihi < 2147483648) return -1;
    else if ((unsigned int) x.ihi < 2147483648 && (unsigned int) y.ihi >= 2147483648) return 1;
    else if ((unsigned int) x.ihi < (unsigned int) y.ihi) return -1;
    else if ((unsigned int) x.ihi > (unsigned int) y.ihi) return 1;
    else if ((unsigned int) x.ilo < (unsigned int) y.ilo) return -1;
    else if ((unsigned int) x.ilo > (unsigned int) y.ilo) return 1;
    else if ((unsigned int) x.fhi < (unsigned int) y.fhi) return -1;
    else if ((unsigned int) x.fhi > (unsigned int) y.fhi) return 1;
    else if ((unsigned int) x.flo < (unsigned int) y.flo) return -1;
    else if ((unsigned int) x.flo > (unsigned int) y.flo) return 1;
    else return 0;
}

void CCbigguy_addmult (CCbigguy *x, CCbigguy y, int m)
{
    long int carry = 0;
    int sgn;
    int oldsgn;
    long int mlo;
    long int mhi;

    if (m == -m && m != 0) {
        fprintf (stderr, "OVERFLOW in CCbigguy_addmult (1)\n");
        fprintf (stderr, "BIGGUY errors are fatal\n");
        abort ();
    }

    if ((unsigned int) y.ihi >= 2147483648) {
        bigguy_neg (&y);
        m = -m;
    }

    mhi = m / (BIT_32_UMAX+1);
    mlo = m - mhi*(BIT_32_UMAX+1);
    if (mlo < -2147483648) {
        mlo += (BIT_32_UMAX+1);
        mhi--;
    }
    if (mlo > BIT_32_SMAX) {
        mlo -= (BIT_32_UMAX+1);
        mhi++;
    }
    if (mlo < -2147483648 || mlo > BIT_32_SMAX) {
        fprintf (stderr, "OVERFLOW in CCbigguy_addmult (2)\n");
        fprintf (stderr, "BIGGUY errors are fatal\n");
        abort ();
    }
    if (mhi < -2147483648 || mhi > BIT_32_SMAX) {
        fprintf (stderr, "OVERFLOW in CCbigguy_addmult (3)\n");
        fprintf (stderr, "BIGGUY errors are fatal\n");
        abort ();
    }

    if ((unsigned int) x->ihi >= 2147483648) {
        oldsgn = -1;
    } else {
        oldsgn = 1;
    }

    carry = (unsigned long int) x->flo + mlo * (unsigned long int) y.flo;
    x->flo = carry & ((unsigned long int) 0xffffffff);
    carry -= (unsigned long int) x->flo;
    carry /= (BIT_32_UMAX+1);
    carry = carry + (unsigned long int) x->fhi + mlo * (unsigned long int) y.fhi;
    x->fhi = carry & ((unsigned long int) 0xffffffff);
    carry -= (unsigned long int) x->fhi;
    carry /= (BIT_32_UMAX+1);
    carry = carry + (unsigned long int) x->ilo + mlo * (unsigned long int) y.ilo;
    x->ilo = carry & ((unsigned long int) 0xffffffff);
    carry -= (unsigned long int) x->ilo;
    carry /= (BIT_32_UMAX+1);
    carry = carry + (unsigned long int) x->ihi + mlo * (unsigned long int) y.ihi;
    x->ihi = carry & ((unsigned long int) 0xffffffff);
    carry -= (unsigned long int) x->ihi;
    carry /= (BIT_32_UMAX+1);

    if ((unsigned long int) x->ihi >= 2147483648) {
        sgn = -1;
    } else {
        sgn = 1;
    }
    if (carry < -1 || carry > 1 ||
        (carry == -1 && !(oldsgn == 1 && sgn == -1)) ||
        (carry == 0 && oldsgn != sgn) ||
        (carry == 1 && !(oldsgn == -1 && sgn == 1))) {
        fprintf (stderr, "OVERFLOW in CCbigguy_addmult (4)\n");
        fprintf (stderr, "BIGGUY errors are fatal\n");
        abort ();
    }

    oldsgn = sgn;

    carry = (unsigned long int) x->fhi + mhi * (unsigned long int) y.flo;
    x->fhi = carry & ((unsigned long int) 0xffffffff);
    carry -= (unsigned long int) x->fhi;
    carry /= (BIT_32_UMAX+1);
    carry = carry + (unsigned long int) x->ilo + mhi * (unsigned long int) y.fhi;
    x->ilo = carry & ((unsigned long int) 0xffffffff);
    carry -= (unsigned long int) x->ilo;
    carry /= (BIT_32_UMAX+1);
    carry = carry + (unsigned long int) x->ihi + mhi * (unsigned long int) y.ilo;
    x->ihi = carry & ((unsigned long int) 0xffffffff);
    carry -= (unsigned long int) x->ihi;
    carry /= (BIT_32_UMAX+1);
    carry = carry + mhi * (unsigned long int) y.ihi;

    if ((unsigned long int) x->ihi >= 2147483648) {
        sgn = -1;
    } else {
        sgn = 1;
    }
    if (carry < -1 || carry > 1 ||
        (carry == -1 && !(oldsgn == 1 && sgn == -1)) ||
        (carry == 0 && oldsgn != sgn) ||
        (carry == 1 && !(oldsgn == -1 && sgn == 1))) {
        fprintf (stderr, "OVERFLOW in CCbigguy_addmult (4)\n");
        fprintf (stderr, "BIGGUY errors are fatal\n");
        abort ();
    }
}

#endif /* CC_BIGGUY_BUILTIN */
