// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012, 2013, 2014, 2015, 2016 Francois Bastardie <fba@aqua.dtu.dk>

//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.

//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
// --------------------------------------------------------------------------

#include <iostream>
#include <vector>
#include <time.h>
#include <cmath>
#include <pthread.h>

#include <helpers.h>

#include <algorithm>

using namespace std;

//----------------------------------
//----------------------------------
//----------------------------------
//----------------------------------
//----------------------------------
//----------------------------------
/* A version of Marsaglia-MultiCarry */

static pthread_mutex_t glob_mutex = PTHREAD_MUTEX_INITIALIZER;
static unsigned int I1=1234, I2=5678;

void set_seed(unsigned int i1, unsigned int i2)
{
    pthread_mutex_lock(&glob_mutex);
	I1 = i1; I2 = i2;
    pthread_mutex_unlock(&glob_mutex);
}


void get_seed(unsigned int *i1, unsigned int *i2)
{
    pthread_mutex_lock(&glob_mutex);
	*i1 = I1; *i2 = I2;
    pthread_mutex_unlock(&glob_mutex);
}


double unif_rand(void)
{
    pthread_mutex_lock(&glob_mutex);

	I1= 36969*(I1 & 0177777) + (I1>>16);
	I2= 18000*(I2 & 0177777) + (I2>>16);
    double ret =((I1 << 16)^(I2 & 0177777)) * 2.328306437080797e-10;
    pthread_mutex_unlock(&glob_mutex);
    return ret;
}


void revsort(double *a, int *ib, int n)
{
	/* Sort a[] into descending order by "heapsort";
	 * sort ib[] alongside;
	 * if initially, ib[] = 1...n, it will contain the permutation finally
	 */

	int l, j, ir, i;
	double ra;
	int ii;

	if (n <= 1) return;

	a--; ib--;

	l = (n >> 1) + 1;
	ir = n;

	for (;;)
	{
		if (l > 1)
		{
			l = l - 1;
			ra = a[l];
			ii = ib[l];
		}
		else
		{
			ra = a[ir];
			ii = ib[ir];
			a[ir] = a[1];
			ib[ir] = ib[1];
			if (--ir == 1)
			{
				a[1] = ra;
				ib[1] = ii;
				return;
			}
		}
		i = l;
		j = l << 1;
		while (j <= ir)
		{
			if (j < ir && a[j] > a[j + 1]) ++j;
			if (ra > a[j])
			{
				a[i] = a[j];
				ib[i] = ib[j];
				j += (i = j);
			}
			else
				j = ir + 1;
		}
		a[i] = ra;
		ib[i] = ii;
	}
}


/*
 *  Unequal Probability Sampling.
 *
 *  Modelled after Fortran code provided by:
 *    E. S. Venkatraman <venkat@biosta.mskcc.org>
 *  but with significant modifications in the
 *  "with replacement" case.
 */

/* Unequal probability sampling; with-replacement case */

void ProbSampleReplace(int nval, double *proba, int *perm, int nans, int *ans)
{
	double rU;
	int i, j;
	int nm1 = nval - 1;

	/* record element identities */
	for (i = 0; i < nval; i++)
		perm[i] = i + 1;

	/* sort the probabilities into descending order */
	revsort(proba, perm, nval);

	/* compute cumulative probabilities */
	for (i = 1 ; i < nval; i++)
	{
		proba[i] += proba[i - 1];

	}

	/* compute the sample */
	for (i = 0; i < nans; i++)
	{
		rU = unif_rand();
		//rU = prUs[i];
        dout(cout<< "rU " << rU << "" << endl);
        dout(cout<< "proba[0] " << proba[0] << "" << endl);
		for (j = 0; j < nm1; j++)
		{
			if (rU <= proba[j])
				break;
		}
        dout(cout<< "j " << j << "" << endl);
        dout(cout<< "perm[j] " << perm[j] << "" << endl);
		ans[i] = perm[j];
        dout(cout<< "ans[i] " << ans[i] << "" << endl);
        dout(cout<< "*ans " << *ans << "" << endl);

	}
    dout(cout<< "*ans? " << *ans << "" << endl);

}


vector<int> do_sample( int n, int nval, const std::vector<int> &val, const std::vector<double> &proba)
{
    using Rec = std::tuple<int, double>;
    class RecGreater {
    public:
        bool operator () (const Rec&v1, const Rec&v2) const {
            return std::get<1>(v1) > std::get<1>(v2);
        }
    };

    if (val.size() == 0 || proba.size() == 0 || nval == 0)
        return vector<int>();

    if (nval != (int)val.size() || nval != (int)proba.size())
        throw std::invalid_argument("do_sample requires nval == val.size() == proba.size()");

    double total = 0.0;

    for (auto pr : proba) {
        total += pr;
    }

    vector<Rec> prb;
    for (int i = 0; i < (int)val.size(); ++i) {
        prb.push_back(std::make_pair(val[i], proba[i]/ total));
    }

    vector<int> res;
    res.reserve(n);

	int nans = n;

    double rU;
	int nm1 = nval - 1;

//    for (auto x : prb)
//        cout << "[ " << std::get<0>(x) <<"," << std::get<1>(x) << "] ";
//    cout << endl;

    /* sort the probabilities into descending order */
    std::sort(prb.begin(), prb.end(), RecGreater());

//    for (auto x : prb)
//        cout << "[ " << std::get<0>(x) <<"," << std::get<1>(x) << "] ";
//    cout << endl;

	/* compute cumulative probabilities */
    for (int i = 1 ; i < nval; i++) {
        std::get<1>(prb[i]) += std::get<1>(prb[i - 1]);
    }

	/* compute the sample */
	for (int i = 0; i < nans; i++)
	{
		rU = unif_rand();
		int j;
		for (j = 0; j < nm1; j++)
		{
            if (rU <= std::get<1>(prb[j]))
				break;
		}
//        cout << j << "," << rU << "," << std::get<1>(prb[j]) << "," << std::get<0>(prb[j]) << endl;
        res.push_back(std::get<0>(prb[j]));
	}

//    cout << "----" << endl;
	return res;
}


//----------------------------------
//----------------------------------
//----------------------------------
//----------------------------------
//----------------------------------
//----------------------------------
//----------------------------------
//----------------------------------
//----------------------------------
//----------------------------------
//----------------------------------
//----------------------------------

double exp_rand(void)
{
	/* q[k-1] = sum(log(2)^k / k!)  k=1,..,n, */
	/* The highest n (here 16) is determined by q[n-1] = 1.0 */
	/* within standard precision */
	const static double q[] =
	{
		0.6931471805599453,
		0.9333736875190459,
		0.9888777961838675,
		0.9984959252914960,
		0.9998292811061389,
		0.9999833164100727,
		0.9999985691438767,
		0.9999998906925558,
		0.9999999924734159,
		0.9999999995283275,
		0.9999999999728814,
		0.9999999999985598,
		0.9999999999999289,
		0.9999999999999968,
		0.9999999999999999,
		1.0000000000000000
	};

	double a = 0.;
	double u = unif_rand();		 /* precaution if u = 0 is ever returned */
	while(u <= 0. || u >= 1.) u = unif_rand();
	for (;;)
	{
		u += u;
		if (u > 1.)
			break;
		a += q[0];
	}
	u -= 1.;

	if (u <= q[0])
		return a + u;

	int i = 0;
	double ustar = unif_rand(), umin = ustar;
	do
	{
		ustar = unif_rand();
		if (umin > ustar)
			umin = ustar;
		i++;
	} while (u > q[i]);
	return a + umin * q[0];
}


//----------------------------------
//----------------------------------
//----------------------------------
//----------------------------------
//----------------------------------
//----------------------------------

double fmax2(double x, double y)
{
	#ifdef IEEE_754
	if (ISNAN(x) || ISNAN(y))
		return x + y;
	#endif
	return (x < y) ? y : x;
}


double fmin2(double x, double y)
{
	#ifdef IEEE_754
	if (ISNAN(x) || ISNAN(y))
		return x + y;
	#endif
	return (x < y) ? x : y;
}


// disable warnings
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"

//----------------------------------
//----------------------------------
//----------------------------------
//----------------------------------
//----------------------------------
//----------------------------------
#define repeat for(;;)
double norm_rand(void)
{

	const static double a[32] =
	{
		0.0000000, 0.03917609, 0.07841241, 0.1177699,
		0.1573107, 0.19709910, 0.23720210, 0.2776904,
		0.3186394, 0.36012990, 0.40225010, 0.4450965,
		0.4887764, 0.53340970, 0.57913220, 0.6260990,
		0.6744898, 0.72451440, 0.77642180, 0.8305109,
		0.8871466, 0.94678180, 1.00999000, 1.0775160,
		1.1503490, 1.22985900, 1.31801100, 1.4177970,
		1.5341210, 1.67594000, 1.86273200, 2.1538750
	};

	const static double d[31] =
	{
		0.0000000, 0.0000000, 0.0000000, 0.0000000,
		0.0000000, 0.2636843, 0.2425085, 0.2255674,
		0.2116342, 0.1999243, 0.1899108, 0.1812252,
		0.1736014, 0.1668419, 0.1607967, 0.1553497,
		0.1504094, 0.1459026, 0.1417700, 0.1379632,
		0.1344418, 0.1311722, 0.1281260, 0.1252791,
		0.1226109, 0.1201036, 0.1177417, 0.1155119,
		0.1134023, 0.1114027, 0.1095039
	};

	const static double t[31] =
	{
		7.673828e-4, 0.002306870, 0.003860618, 0.005438454,
		0.007050699, 0.008708396, 0.010423570, 0.012209530,
		0.014081250, 0.016055790, 0.018152900, 0.020395730,
		0.022811770, 0.025434070, 0.028302960, 0.031468220,
		0.034992330, 0.038954830, 0.043458780, 0.048640350,
		0.054683340, 0.061842220, 0.070479830, 0.081131950,
		0.094624440, 0.112300100, 0.136498000, 0.171688600,
		0.227624100, 0.330498000, 0.584703100
	};

	const static double h[31] =
	{
		0.03920617, 0.03932705, 0.03950999, 0.03975703,
		0.04007093, 0.04045533, 0.04091481, 0.04145507,
		0.04208311, 0.04280748, 0.04363863, 0.04458932,
		0.04567523, 0.04691571, 0.04833487, 0.04996298,
		0.05183859, 0.05401138, 0.05654656, 0.05953130,
		0.06308489, 0.06737503, 0.07264544, 0.07926471,
		0.08781922, 0.09930398, 0.11555990, 0.14043440,
		0.18361420, 0.27900160, 0.70104740
	};

	/*----------- Constants and definitions for  Kinderman - Ramage --- */
	/*
	 *  REFERENCE
	 *
	 *    Kinderman A. J. and Ramage J. G. (1976).
	 *    Computer generation of normal random variables.
	 *    JASA 71, 893-896.
	 */

	#define C1      0.398942280401433
	#define C2      0.180025191068563
	#define g(x)        (C1*exp(-x*x/2.0)-C2*(A-x))

	const static double A =  2.216035867166471;

	double s, u1, w, y, u2, u3, aa, tt, theta, R;
	int i;

	//KINDERMAN_RAMAGE: /* see Reference above */
	/* corrected version from Josef Leydold
	 * */

	u1 = unif_rand();
	if(u1 < 0.884070402298758)
	{
		u2 = unif_rand();
		return A*(1.131131635444180*u1+u2-1);
	}

	if(u1 >= 0.973310954173898)	 /* tail: */
	{
		repeat
		{
			u2 = unif_rand();
			u3 = unif_rand();
			tt = (A*A-2*log(u3));
			if( u2*u2<(A*A)/tt )
				return (u1 < 0.986655477086949) ? sqrt(tt) : -sqrt(tt);
		}
	}

	if(u1 >= 0.958720824790463)	 /* region3: */
	{
		repeat
		{
			u2 = unif_rand();
			u3 = unif_rand();
			tt = A - 0.630834801921960* fmin2(u2,u3);
			if(fmax2(u2,u3) <= 0.755591531667601)
				return (u2<u3) ? tt : -tt;
			if(0.034240503750111*fabs(u2-u3) <= g(tt))
				return (u2<u3) ? tt : -tt;
		}
	}

	if(u1 >= 0.911312780288703)	 /* region2: */
	{
		repeat
		{
			u2 = unif_rand();
			u3 = unif_rand();
			tt = 0.479727404222441+1.105473661022070*fmin2(u2,u3);
			if( fmax2(u2,u3)<=0.872834976671790 )
				return (u2<u3) ? tt : -tt;
			if( 0.049264496373128*fabs(u2-u3)<=g(tt) )
				return (u2<u3) ? tt : -tt;
		}
	}

	/* ELSE	 region1: */
	repeat
	{
		u2 = unif_rand();
		u3 = unif_rand();
		tt = 0.479727404222441-0.595507138015940*fmin2(u2,u3);
		if (tt < 0.) continue;
		if(fmax2(u2,u3) <= 0.805577924423817)
			return (u2<u3) ? tt : -tt;
		if(0.053377549506886*fabs(u2-u3) <= g(tt))
			return (u2<u3) ? tt : -tt;
	}

}

#pragma GCC diagnostic pop

//#include "nmath.h"
//a=>shape
double rgamma(double a, double scale)
{
	/* Constants : */
	const static double sqrt32 = 5.656854;
								 /* exp(-1) = 1/e */
	const static double exp_m1 = 0.36787944117144232159;

	/* Coefficients q[k] - for q0 = sum(q[k]*a^(-k))
	 * Coefficients a[k] - for q = q0+(t*t/2)*sum(a[k]*v^k)
	 * Coefficients e[k] - for exp(q)-1 = sum(e[k]*q^k)
	 */
	const static double q1 = 0.04166669;
	const static double q2 = 0.02083148;
	const static double q3 = 0.00801191;
	const static double q4 = 0.00144121;
	const static double q5 = -7.388e-5;
	const static double q6 = 2.4511e-4;
	const static double q7 = 2.424e-4;

	const static double a1 = 0.3333333;
	const static double a2 = -0.250003;
	const static double a3 = 0.2000062;
	const static double a4 = -0.1662921;
	const static double a5 = 0.1423657;
	const static double a6 = -0.1367177;
	const static double a7 = 0.1233795;

	/* State variables [FIXME for threading!] :*/
	static double aa = 0.;
	static double aaa = 0.;
	static double s, s2, d;		 /* no. 1 (step 1) */
	static double q0, b, si, c;	 /* no. 2 (step 4) */

	double e, p, q, r, t, u, v, w, x, ret_val;

	//if (!R_FINITE(a) || !R_FINITE(scale) || a < 0.0 || scale <= 0.0) {
	//if(scale == 0.) return 0.;
	//ML_ERR_return_NAN;
	//}

	if (a < 1.)					 /* GS algorithm for parameters a < 1 */
	{
		if(a == 0)
			return 0.;
		e = 1.0 + exp_m1 * a;
		repeat
		{
			p = e * unif_rand();
			if (p >= 1.0)
			{
				x = -log((e - p) / a);
				if (exp_rand() >= (1.0 - a) * log(x))
					break;
			}
			else
			{
				x = exp(log(p) / a);
				if (exp_rand() >= x)
					break;
			}
		}
		return scale * x;
	}

	/* --- a >= 1 : GD algorithm --- */

	/* Step 1: Recalculations of s2, s, d if a has changed */
	if (a != aa)
	{
		aa = a;
		s2 = a - 0.5;
		s = sqrt(s2);
		d = sqrt32 - s * 12.0;
	}
	/* Step 2: t = standard normal deviate,
			   x = (s,1/2) -normal deviate. */

	/* immediate acceptance (i) */
	t = norm_rand();
	x = s + 0.5 * t;
	ret_val = x * x;
	if (t >= 0.0)
		return scale * ret_val;

	/* Step 3: u = 0,1 - uniform sample. squeeze acceptance (s) */
	u = unif_rand();
	if (d * u <= t * t * t)
		return scale * ret_val;

	/* Step 4: recalculations of q0, b, si, c if necessary */

	if (a != aaa)
	{
		aaa = a;
		r = 1.0 / a;
		q0 = ((((((q7 * r + q6) * r + q5) * r + q4) * r + q3) * r
			+ q2) * r + q1) * r;

		/* Approximation depending on size of parameter a */
		/* The constants in the expressions for b, si and c */
		/* were established by numerical experiments */

		if (a <= 3.686)
		{
			b = 0.463 + s + 0.178 * s2;
			si = 1.235;
			c = 0.195 / s - 0.079 + 0.16 * s;
		}
		else if (a <= 13.022)
		{
			b = 1.654 + 0.0076 * s2;
			si = 1.68 / s + 0.275;
			c = 0.062 / s + 0.024;
		}
		else
		{
			b = 1.77;
			si = 0.75;
			c = 0.1515 / s;
		}
	}
	/* Step 5: no quotient test if x not positive */

	if (x > 0.0)
	{
		/* Step 6: calculation of v and quotient q */
		v = t / (s + s);
		if (fabs(v) <= 0.25)
			q = q0 + 0.5 * t * t * ((((((a7 * v + a6) * v + a5) * v + a4) * v
				+ a3) * v + a2) * v + a1) * v;
		else
			q = q0 - s * t + 0.25 * t * t + (s2 + s2) * log(1.0 + v);

		/* Step 7: quotient acceptance (q) */
		if (log(1.0 - u) <= q)
			return scale * ret_val;
	}

	repeat
	{
		/* Step 8: e = standard exponential deviate
		 *	u =  0,1 -uniform deviate
		 *	t = (b,si)-double exponential (laplace) sample */
		e = exp_rand();
		u = unif_rand();
		u = u + u - 1.0;
		if (u < 0.0)
			t = b - si * e;
		else
			t = b + si * e;
		/* Step	 9:  rejection if t < tau(1) = -0.71874483771719 */
		if (t >= -0.71874483771719)
		{
			/* Step 10:	 calculation of v and quotient q */
			v = t / (s + s);
			if (fabs(v) <= 0.25)
				q = q0 + 0.5 * t * t *
					((((((a7 * v + a6) * v + a5) * v + a4) * v + a3) * v
					+ a2) * v + a1) * v;
			else
				q = q0 - s * t + 0.25 * t * t + (s2 + s2) * log(1.0 + v);
			/* Step 11:	 hat acceptance (h) */
			/* (if q not positive go to step 8) */
			if (q > 0.0)
			{
				w = expm1(q);
				/*  ^^^^^ original code had approximation with rel.err < 2e-7 */
				/* if t is rejected sample again at step 8 */
				if (c * fabs(u) <= w * exp(e - 0.5 * t * t))
					break;
			}
		}
	}							 /* repeat .. until  `t' is accepted */
	x = s + 0.5 * t;
	return scale * x * x;
}


double rnorm(double mu, double sigma)
{
	return mu + sigma * norm_rand();
}


double rlnorm(double meanlog, double sdlog)
{

	return exp(rnorm(meanlog, sdlog));
}
