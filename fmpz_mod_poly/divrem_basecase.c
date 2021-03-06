/*
    Copyright (C) 2011, 2010 Sebastian Pancratz
    Copyright (C) 2008, 2009 William Hart

    This file is part of FLINT.

    FLINT is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.  See <http://www.gnu.org/licenses/>.
*/

#include <stdlib.h>
#include "fmpz_vec.h"
#include "fmpz_mod_poly.h"

void _fmpz_mod_poly_divrem_basecase(fmpz *Q, fmpz *R, 
    const fmpz *A, slong lenA, const fmpz *B, slong lenB, 
    const fmpz_t invB, const fmpz_t p)
{
    slong i, iQ, iR;
    fmpz * W;
    TMP_INIT;
	
	TMP_START;
	
    if (R != A)
    {
        W = (fmpz *) TMP_ALLOC(lenA*sizeof(fmpz));
		
		for (i = 0; i < lenA; i++)
		   fmpz_init(W + i);
		   
        _fmpz_vec_set(W, A, lenA);
    } else
       W = R;

    for (iQ = lenA - lenB, iR = lenA - 1; iQ >= 0; iQ--, iR--)
    {
        if (fmpz_is_zero(W + iR))
        {
            fmpz_zero(Q + iQ);
		}
        else
        {
            fmpz_mul(Q + iQ, W + iR, invB);
            fmpz_mod(Q + iQ, Q + iQ, p);

            _fmpz_vec_scalar_submul_fmpz(W + iQ, B, lenB, Q + iQ);
        }
        if (iQ > 0)
            fmpz_mod(W + iR - 1, W + iR - 1, p);
    }

	_fmpz_vec_scalar_mod_fmpz(W, W, lenB - 1, p);
	
    if (R != A)
    {
       for (i = 0; i < lenB - 1; i++)
	      fmpz_swap(R + i, W + i);
	   
	   for (i = 0; i < lenA; i++)
	      fmpz_clear(W + i);
    }
	
	TMP_END;
}

void fmpz_mod_poly_divrem_basecase(fmpz_mod_poly_t Q, fmpz_mod_poly_t R, 
    const fmpz_mod_poly_t A, const fmpz_mod_poly_t B)
{
    const slong lenA = A->length, lenB = B->length, lenQ = lenA - lenB + 1;
    fmpz *q, *r;
    fmpz_t invB;

    if (lenB == 0)
    {
        if (fmpz_is_one(fmpz_mod_poly_modulus(B)))
        {
            fmpz_mod_poly_set(Q, A);
            fmpz_mod_poly_zero(R);
            return;
        } else
        {
            flint_printf("Exception (fmpz_mod_poly_divrem_basecase). Division by zero.\n");
            flint_abort();
        }
    }

    if (lenA < lenB)
    {
        fmpz_mod_poly_set(R, A);
        fmpz_mod_poly_zero(Q);
        return;
    }

    fmpz_init(invB);
    fmpz_invmod(invB, B->coeffs + (lenB - 1), &(B->p));

    if (Q == A || Q == B)
    {
        q = _fmpz_vec_init(lenQ);
    }
    else
    {
        fmpz_mod_poly_fit_length(Q, lenQ);
        q = Q->coeffs;
    }
    if (R == B)
    {
        r = _fmpz_vec_init(lenB - 1);
    }
    else
    {
        fmpz_mod_poly_fit_length(R, lenB - 1);
        r = R->coeffs;
    }

    _fmpz_mod_poly_divrem_basecase(q, r, A->coeffs, lenA,
                                         B->coeffs, lenB, invB, &(B->p));

    if (Q == A || Q == B)
    {
        _fmpz_vec_clear(Q->coeffs, Q->alloc);
        Q->coeffs = q;
        Q->alloc  = lenQ;
        Q->length = lenQ;
    }
    else
    {
        _fmpz_mod_poly_set_length(Q, lenQ);
    }
    if (R == B)
    {
        _fmpz_vec_clear(R->coeffs, R->alloc);
        R->coeffs = r;
        R->alloc  = lenB - 1;
        R->length = lenB - 1;
    }
    else
    {
      _fmpz_mod_poly_set_length(R, lenB - 1);
    }

    _fmpz_mod_poly_normalise(R);

    fmpz_clear(invB);
}

