#ifndef _randomNumbers_H_
#define _randomNumbers_H_

/* A C-program for MT19937: Real number version                */
/*   genrand() generates one pseudorandom real number (double) */
/* which is uniformly distributed on [0,1]-interval, for each  */
/* call. sgenrand(seed) set initial values to the working area */
/* of 624 words. Before genrand(), sgenrand(seed) must be      */
/* called once. (seed is any 32-bit integer except for 0).     */
/* Integer generator is obtained by modifying two lines.       */
/*   Coded by Takuji Nishimura, considering the suggestions by */
/* Topher Cooper and Marc Rieffel in July-Aug. 1997.           */

/* This library is free software; you can redistribute it and/or   */
/* modify it under the terms of the GNU Library General Public     */
/* License as published by the Free Software Foundation; either    */
/* version 2 of the License, or (at your option) any later         */
/* version.                                                        */
/* This library is distributed in the hope that it will be useful, */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of  */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.            */
/* See the GNU Library General Public License for more details.    */
/* You should have received a copy of the GNU Library General      */
/* Public License along with this library; if not, write to the    */
/* Free Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA   */ 
/* 02111-1307  USA                                                 */

/* Copyright (C) 1997 Makoto Matsumoto and Takuji Nishimura.       */
/* Any feedback is very welcome. For any question, comments,       */
/* see http://www.math.keio.ac.jp/matumoto/emt.html or email       */
/* matumoto@math.keio.ac.jp                                        */


/* 2005, re-editted by B. Nicoletti to be C++ and to carry the seed with it */


#ifdef _WINDOWS
#define uint32_t unsigned int
#else
#include <stdint.h> // for uint32_t
#endif


//  Mersenne Twister random number generator
class RandomGenerator {
private :    
    enum {N = 624,
          M = 397};
     uint32_t mt[N]; /* the array for the state vector  */
     int mti; /* mti==N+1 means mt[N] is not initialized */

public :
    /* ctor */
    RandomGenerator(uint32_t seed = 0);

    /* reseed it */
    void reseed(uint32_t seed = 0);

    /* get a random value from it */
    double random(void);
};

#endif
