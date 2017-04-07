#pragma once

/*
Copyright (c) 2016, Roland Bock
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

#include <sqlpp17/clause_fwd.h>
#include <sqlpp17/result_row.h>
#include <sqlpp17/type_traits.h>
#include <sqlpp17/wrapped_static_assert.h>

namespace sqlpp
{
  template <typename FieldSpec>
  struct cte_column_spec_t
  {
    using _alias_t = typename FieldSpec::_alias_t;

    using value_type = value_type_of_t<FieldSpec>;
    static constexpr auto tags = sqlpp::tag::must_not_insert | sqlpp::tag::must_not_update |
                                 (can_be_null_v<FieldSpec> ? sqlpp::tag::can_be_null : sqlpp::tag::none);
  };

  template <typename Cte, typename ResultRow>
  struct cte_columns_t
  {
    static_assert(wrong<cte_columns_t>, "Invalid arguments for cte_columns_t");
  };

  template <typename Cte, typename... FieldSpecs>
  struct cte_columns_t<Cte, result_row_t<FieldSpecs...>> : column_t<Cte, cte_column_spec_t<FieldSpecs>>...
  {
  };

  template <typename AliasProvider, typename Statement>
  struct cte_t : cte_columns_t<cte_t<AliasProvider, Statement>, typename Statement::result_row_t>
  {
    Statement _statement;
  };

  template <typename Context, typename AliasProvider, typename Statement>
  decltype(auto) operator<<(Context& context, const cte_t<AliasProvider, Statement>& t)
  {
#warning : the With clause must not use this one, it must serialize the statement, too.
    return context << name_of_v<AliasProvider>;
  }
}
