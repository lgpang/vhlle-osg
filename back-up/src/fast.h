#ifndef FAST_H
#define FAST_H

/*
 * See http://martin.ankerl.com/2007/10/04/optimized-pow-approximation-for-java-and-c-c/
 *
 * All of these rely on being on a little endian machine, such as an Intel box.
 *
 * These can be _quite_ inaccurate. ~20% in many cases, but being much faster (~7x) may
 * permit more loop iterations of tuning algorithms that only need approximate powers.
 *
 * This version of Ankerl's algorithm has been extended to provide optionally conservative (lower) bounds
 * and also to generate a full linear interpolation across the entire significand rather than 'stair-step'
 * at the expense of performing a 64 bit operation rather than a 32 bit one. This is cheap these days.
 *
 * 'exp' is further improved by using a suggestion by Nic Schraudolph:
 *
 * "You can get a much better approximation (piecewise rational instead of linear) at
 * the cost of a single floating-point division by using better_exp(x) = exp(x/2)/exp(-x/2),
 * where exp() is my published approximation but you don't need the additive constant anymore,
 * you can use c=0. On machines with hardware division this is very attractive." -- Nic Schraudolph
 *
 * --Edward Kmett
 *
 * TODO: Incorporate the techniques from https://code.google.com/p/fastapprox/ to enable us
 * to calculate more interesting approximate functions. They might need to be generalized to work on
 * Double values where appropriate I suppose.
 *
 * Magic numbers:
 * float /int      : round(1<<23/log(2)) = 12102203,          127<<23 = 1065353216
 * double/int      : round(1<<20/log(2)) = 1512775,          1023<<20 = 1072693248
 * double/long long: round(1<<52/log(2)) = 6497320848556798, 1023<<52 = 4607182418800017408
 *
 * The fudge factors such that exp y <= exp_fast y:
 * >>> ceiling (2^23 * (1 - (log (log 2) + 1)/log 2))
 * 722019
 * >>> ceiling (2^20 * (1 - (log (log 2) + 1)/log 2))
 * 90253
 * >>> ceiling (2^52 * (1 - (log (log 2) + 1)/log 2))
 * 387630818974388
 *
 * The fudge factor such that exp_fast y <= exp y is uniformly -1
 *
 * TODO: perform exponential doubling for pow based on better_exp_fast instead for better accuracy.
 */

/* Schraudolph's published algorithm extended into the least significant bits to avoid the stair step.
 double long long approximation: round 1<<52/log(2) 6497320848556798,
  mask = 0x3ff0000000000000LL = 4607182418800017408LL
 double approximation: round(1<<20/log(2)) = 1512775, 1023<<20 = 1072693248
*/

/* 4607182418800017408 - 387630818974388 = 4606794787981043020

Exponent mask adapted to full 64 bit precision:
>>> 1023 * 2^52
4607182418800017408

The fudge factor for conservative lower bound adapted to full 64 bit precision:
>>> round (2^52 * (1 - (log (log 2) + 1)/log 2))
387630818974388

As a lower bound this is suitable for use when generating Mass and Precision estimates.
*/
double exp_fast_lb(double a);

/* 4607182418800017408 + 1 */
double exp_fast_ub(double a);

double exp_fast(double a);

double better_exp_fast(double a);

/* Schraudolph's published algorithm */
double exp_fast_schraudolph(double a) ;

/* 1065353216 + 1 */
float expf_fast_ub(float a) ;

/* Schraudolph's published algorithm with John's constants */
/* 1065353216 - 486411 = 1064866805 */
float expf_fast(float a) ;

//  1056478197 
double better_expf_fast(float a) ;

/* 1065353216 - 722019 */
float expf_fast_lb(float a);

/* Ankerl's inversion of Schraudolph's published algorithm, converted to explicit multiplication */
double log_fast_ankerl(double a) ;

double log_fast_ub(double a) ;

/* Ankerl's inversion of Schraudolph's published algorithm with my constants */
double log_fast(double a) ;

double log_fast_lb(double a) ;


/* 1065353216 - 722019 */
float logf_fast_ub(float a) ;
/* Ankerl's adaptation of Schraudolph's published algorithm with John's constants */
/* 1065353216 - 486411 = 1064866805 */
float logf_fast(float a) ;

/* 1065353216 + 1 */
float logf_fast_lb(float a) ;

/* Ankerl's version of Schraudolph's approximation. */
double pow_fast_ankerl(double a, double b);

/*
 These constants are based loosely on the following comment off of Ankerl's blog:

 "I have used the same trick for float, not double, with some slight modification to the constants to suite IEEE754 float format. The first constant for float is 1<<23/log(2) and the second is 127<<23 (for double they are 1<<20/log(2) and 1023<<20)." -- John
*/

/* 1065353216 + 1      = 1065353217 ub */
/* 1065353216 - 486411 = 1064866805 min RMSE */
/* 1065353216 - 722019 = 1064631197 lb */
float powf_fast(float a, float b) ;

float powf_fast_lb(float a, float b) ;

float powf_fast_ub(float a, float b) ;

/*
  Now that 64 bit arithmetic is cheap we can (try to) improve on Ankerl's algorithm.

 double long long approximation: round 1<<52/log(2) 6497320848556798,
  mask = 0x3ff0000000000000LL = 4607182418800017408LL

>>> round (2**52 * log (3 / (8 * log 2) + 1/2) / log 2 - 1/2)
261140389990638
>>> 0x3ff0000000000000 - round (2**52 * log (3 / (8 * log 2) + 1/2) / log 2 - 1/2)
4606921278410026770

*/

double pow_fast_ub(double a, double b);

double pow_fast(double a, double b) ;

double pow_fast_lb(double a, double b);

/* should be much more precise with large b, still ~3.3x faster. */
double pow_fast_precise_ankerl(double a, double b) ;

/* should be much more precise with large b, still ~3.3x faster. */
double pow_fast_precise(double a, double b) ;

double better_pow_fast_precise(double a, double b) ;


/* should be much more precise with large b */
float powf_fast_precise(float a, float b) ;

/* should be much more precise with large b */
float better_powf_fast_precise(float a, float b);

#endif

