/*
 * utils.cpp
 *
 *  Created on: 28 мая 2021 г.
 *      Author: alex9932
 */

#include "utils.h"

bool rg_streql(rg_string left, rg_string right) {
	size_t lenpre = strlen(left);
	size_t lenstr = strlen(right);
	return ( (lenstr == lenpre) && (SDL_memcmp(left, right, lenpre) == 0) ) ? true : false;
}
