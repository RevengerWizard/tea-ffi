/*
** FFI C callback handling
** tea_ccallback.h
*/

#ifndef _TEA_CCALLBACK_H
#define _TEA_CCALLBACK_H

#include <tea.h>
#include "ctype.h"
#include "cdata.h"

bool ccallback_new(tea_State* T, CType* ct, CData* cd, int funcidx);
void ccallback_free(CData* cd);

#endif