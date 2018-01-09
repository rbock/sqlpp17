#pragma once

/*
Copyright (c) 2017 - 2018, Roland Bock
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this
   list of conditions and the following disclaimer in the documentation and/or
   other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <sqlpp17/bad_expression.h>
#include <sqlpp17/wrapped_static_assert.h>

// logical
#include <sqlpp17/operator/logical_and.h>
#include <sqlpp17/operator/logical_not.h>
#include <sqlpp17/operator/logical_or.h>

// comparison
#include <sqlpp17/operator/equal_to.h>
#include <sqlpp17/operator/greater.h>
#include <sqlpp17/operator/greater_equal.h>
#include <sqlpp17/operator/less.h>
#include <sqlpp17/operator/less_equal.h>
#include <sqlpp17/operator/like.h>
#include <sqlpp17/operator/not_equal_to.h>

// arithmetic
#include <sqlpp17/operator/divides.h>
#include <sqlpp17/operator/minus.h>
#include <sqlpp17/operator/modulus.h>
#include <sqlpp17/operator/multiplies.h>
#include <sqlpp17/operator/negate.h>
#include <sqlpp17/operator/plus.h>

// binary
#include <sqlpp17/operator/bit_and.h>
#include <sqlpp17/operator/bit_or.h>
#include <sqlpp17/operator/bit_xor.h>

// assignment
#include <sqlpp17/operator/assign.h>

// misc
#include <sqlpp17/operator/as.h>

#include <sqlpp17/operator/asc.h>
#include <sqlpp17/operator/desc.h>

#include <sqlpp17/operator/in.h>
#include <sqlpp17/operator/not_in.h>

#include <sqlpp17/operator/is_not_null.h>
#include <sqlpp17/operator/is_null.h>

// quasi functions
#include <sqlpp17/operator/exists.h>
