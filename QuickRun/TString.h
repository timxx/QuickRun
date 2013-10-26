//========================================================================================================

#ifndef TSTRING_H
#define TSTRING_H

#pragma once

#include "StringEx.h"

typedef StringEx TString;

wchar_t *A2U(const char *a);
char *U2A(const wchar_t *u);

#endif	//TSTRING_H