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

namespace sqlpp
{
  template <typename ConditionlessJoin, typename On>
  class join_t : public join_functions<join_t<ConditionlessJoin, On>>
  {
  public:
    constexpr join_t(ConditionlessJoin conditionless_join, On on) : _conditionless_join(conditionless_join), _on(on)
    {
    }

    ConditionlessJoin _conditionless_join;
    On _on;
  };

  template <typename Context, typename ConditionlessJoin, typename On>
  auto operator<<(Context& context, const join_t<ConditionlessJoin, On>& t)
  {
    return context << t._conditionless_join << t.on;
  }

  template <typename ConditionlessJoin, typename On>
  constexpr auto is_join_v<join_t<ConditionlessJoin, On>> = true;

  template <typename ConditionlessJoin, typename On>
  constexpr auto is_table_v<join_t<ConditionlessJoin, On>> = true;

  template <typename ConditionlessJoin, typename On>
  constexpr auto provided_tables_of_v<join_t<ConditionlessJoin, On>> = provided_tables_of_v<ConditionlessJoin>;
}

