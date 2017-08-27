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

#include <memory>
#include <sqlpp17/type_traits.h>
#include <sqlpp17/wrapped_static_assert.h>

namespace sqlpp
{
  namespace tag
  {
    struct table
    {
    };

    template <typename ValueType>
    struct expression
    {
    };
  }

  template <typename Context, typename Tag>
  struct anonymous_t
  {
    template <typename T>
    anonymous_t(T t) : _requires_braces(requires_braces<T>), _impl(std::make_shared<_impl_t<T>>(t))
    {
    }

    anonymous_t(const anonymous_t&) = default;
    anonymous_t(anonymous_t&&) = default;
    anonymous_t& operator=(const anonymous_t&) = default;
    anonymous_t& operator=(anonymous_t&&) = default;
    ~anonymous_t() = default;

    auto serialize(Context& context) const -> Context&
    {
      return _impl->serialize(context);
    }

    bool _requires_braces;

  private:
    struct _impl_base
    {
      virtual auto serialize(Context& context) const -> Context& = 0;
    };

    template <typename T>
    struct _impl_t : public _impl_base
    {
      _impl_t(T t) : _t(t)
      {
      }

      auto serialize(Context& context) const -> Context&
      {
        return context << _t;
      }

      T _t;
    };

    std::shared_ptr<const _impl_base> _impl;
  };

  template <typename Context, typename Tag>
  decltype(auto) operator<<(Context& context, const anonymous_t<Context, Tag>& t)
  {
    if (t._requires_braces)
    {
      context << '(' << t.serialize(context) << ')';
    }
    else
      context << t;

    return context;
  }

  SQLPP_WRAPPED_STATIC_ASSERT(assert_anonymize_arg_has_no_parameters,
                              "anonymized expression must not contain parameters");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_anonymize_arg_has_no_alias, "anonymized expression must not have an alias");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_anonymize_arg_is_table_or_expression,
                              "anonymized expression must be an expression or table");

  template <typename Database, typename T>
  auto anonymize(T t)
  {
    using Context = typename Database::Context;
    if constexpr (!parameters_of<T>.empty())
    {
      return ::sqlpp::bad_statement_t<failed<assert_anonymize_arg_has_no_parameters>>{t};
    }
    else if constexpr (::sqlpp::is_table_v<T>)
    {
      return anonymous_t<Context, tag::table>{t};
    }
    else if constexpr (::sqlpp::is_alias_v<T>)
    {
      return ::sqlpp::bad_statement_t<failed<assert_anonymize_arg_has_no_alias>>{t};
    }
    else if constexpr (::sqlpp::is_expression_v<T>)
    {
      return anonymous_t<Context, tag::expression<decltype(value_type_of_v<T>)>>{t};
    }
    else
    {
      return ::sqlpp::bad_statement_t<failed<assert_anonymize_arg_is_table_or_expression>>{t};
    }
  }

  template <typename Database, typename T>
  auto anonymize(const Database&, T t)
  {
    return anonymize<Database>(t);
  }
}
