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
#include <sqlpp17/interpret.h>

namespace sqlpp
{
  template <typename Context>
  struct interpretable_t
  {
    template <typename T>
    interpretable_t(T t) : _requires_braces(requires_braces<T>), _impl(std::make_shared<_impl_t<T>>(t))
    {
    }

    interpretable_t(const interpretable_t&) = default;
    interpretable_t(interpretable_t&&) = default;
    interpretable_t& operator=(const interpretable_t&) = default;
    interpretable_t& operator=(interpretable_t&&) = default;
    ~interpretable_t() = default;

    auto interpret(Context& context) const -> Context&
    {
      return _impl->interpret(context);
    }

    bool _requires_braces;

  private:
    struct _impl_base
    {
      virtual auto interpret(Context& context) const -> Context& = 0;
    };

    template <typename T>
    struct _impl_t : public _impl_base
    {
      static_assert(parameters_of<T>.size() == 0, "parameters not supported in dynamic statement parts");
      _impl_t(T t) : _t(t)
      {
      }

      auto interpret(Context& context) const -> Context&
      {
        ::sqlpp::interpret(_t, context);
        return context;
      }

      T _t;
    };

    std::shared_ptr<const _impl_base> _impl;
  };

  template <typename Context, typename Database>
  struct interpreter_t<Context, interpretable_t<Database>>
  {
    using T = interpretable_t<Database>;

    static Context& _(const T& t, Context& context)
    {
      if (t._requires_braces)
      {
        context << '(';
        t.interpret(context);
        context << ')';
      }
      else
        t.interpret(context);

      return context;
    }
  };
}

