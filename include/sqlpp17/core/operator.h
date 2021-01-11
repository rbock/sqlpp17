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

#include <sqlpp17/core/bad_expression.h>
#include <sqlpp17/core/wrapped_static_assert.h>

/* Hint
   Most operators are using enable_if instead of wrapped static_assert.
   This is because variants with static_assert would enter the overload
   set and cause havoc.
   */

// logical
#include <sqlpp17/core/operator/logical_and.h>
#include <sqlpp17/core/operator/logical_not.h>
#include <sqlpp17/core/operator/logical_or.h>

// comparison
#include <sqlpp17/core/operator/equal_to.h>
#include <sqlpp17/core/operator/greater.h>
#include <sqlpp17/core/operator/greater_equal.h>
#include <sqlpp17/core/operator/less.h>
#include <sqlpp17/core/operator/less_equal.h>
#include <sqlpp17/core/operator/like.h>
#include <sqlpp17/core/operator/not_equal_to.h>

// arithmetic
#include <sqlpp17/core/operator/divides.h>
#include <sqlpp17/core/operator/minus.h>
#include <sqlpp17/core/operator/modulus.h>
#include <sqlpp17/core/operator/multiplies.h>
#include <sqlpp17/core/operator/negate.h>
#include <sqlpp17/core/operator/plus.h>

// binary
#include <sqlpp17/core/operator/bit_and.h>
#include <sqlpp17/core/operator/bit_not.h>
#include <sqlpp17/core/operator/bit_or.h>
#include <sqlpp17/core/operator/bit_shift_left.h>
#include <sqlpp17/core/operator/bit_shift_right.h>
#include <sqlpp17/core/operator/bit_xor.h>

// assignment
#include <sqlpp17/core/operator/assign.h>

// misc
#include <sqlpp17/core/operator/as.h>

#include <sqlpp17/core/operator/asc.h>
#include <sqlpp17/core/operator/desc.h>

#include <sqlpp17/core/operator/in.h>
#include <sqlpp17/core/operator/not_in.h>

#include <sqlpp17/core/operator/is_not_null.h>
#include <sqlpp17/core/operator/is_null.h>

#include <sqlpp17/core/operator/exists.h>
