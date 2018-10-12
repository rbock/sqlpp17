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

#include <functional>
#include <memory>
#include <optional>
#include <string_view>

#include <sqlpp17/exception.h>
#include <sqlpp17/result_row.h>

#include <sqlpp17/mysql/bind_meta_data.h>

namespace sqlpp::mysql::detail
{
  struct prepared_result_cleanup_t
  {
  public:
    auto operator()(MYSQL_STMT* handle) -> void
    {
      if (handle)
        mysql_stmt_free_result(handle);
    }
  };
  using unique_prepared_result_ptr = std::unique_ptr<MYSQL_STMT, prepared_result_cleanup_t>;

  template <typename BindParameters>
  auto bind(MYSQL_STMT* stmt, BindParameters& bind_parameters) -> void
  {
    if (mysql_stmt_bind_result(stmt, bind_parameters.data()))
    {
      throw sqlpp::exception(std::string("MySQL: mysql_stmt_bind_result: ") +
                             mysql_stmt_error(stmt));
    }
  }
}  // namespace sqlpp::mysql::detail

namespace sqlpp::mysql
{
  template <typename ValueType>
  struct value_type_buffer
  {
    using type = ValueType;
  };

  template <>
  struct value_type_buffer<std::string_view>
  {
    using type = std::string;
  };

  template <typename ValueType>
  struct value_type_buffer<std::optional<ValueType>>
  {
    using type = typename value_type_buffer<ValueType>::type;
  };

  template <typename ColumnSpec>
  using buffer_type_of_t = typename value_type_buffer<value_type_of_t<ColumnSpec>>::type;

  auto refetch_truncated_field(MYSQL_STMT* stmt,
                               [[maybe_unused]] std::string_view& field,
                               std::string& buffer,
                               bind_meta_data_t& meta_data,
                               MYSQL_BIND& param,
                               unsigned index) -> void
  {
    if (meta_data.length > buffer.size())
    {
      buffer.resize(meta_data.length);
      param.buffer = buffer.data();
      param.buffer_length = buffer.size();

      auto err = mysql_stmt_fetch_column(stmt, &param, index, 0);
      if (err)
        throw sqlpp::exception(std::string("MySQL: Fetch column after reallocate failed: ") + "error-code: " +
                               std::to_string(err) + ", stmt-error: " + mysql_stmt_error(stmt) +
                               ", stmt-errno: " + std::to_string(mysql_stmt_errno(stmt)) +
                               ", field index: " + std::to_string(index));
    }
  }

  template<typename Field>
  auto refetch_truncated_field(MYSQL_STMT* stmt,
                               [[maybe_unused]] Field& field,
                               [[maybe_unused]] Field& buffer,
                               bind_meta_data_t& meta_data,
                               MYSQL_BIND& param,
                               unsigned index) -> void
  {
  }

  template<typename Field, typename Buffer>
  auto refetch_truncated_field(MYSQL_STMT* stmt,
                               [[maybe_unused]] std::optional<Field>& field,
                               Buffer& buffer,
                               bind_meta_data_t& meta_data,
                               MYSQL_BIND& param,
                               unsigned index) -> void
  {
    refetch_truncated_field(stmt, buffer, buffer, meta_data, param, index);
  }

  template <typename... ColumnSpecs, unsigned... Is>
  auto refetch_truncated_fields(MYSQL_STMT* stmt,
                                result_row_t<ColumnSpecs...>& row,
                                std::tuple<buffer_type_of_t<ColumnSpecs>...>& buffers,
                                std::array<bind_meta_data_t, sizeof...(ColumnSpecs)>& meta_data,
                                std::array<MYSQL_BIND, sizeof...(ColumnSpecs)>& bind_parameters,
                                std::integer_sequence<unsigned, Is...>) -> void
  {
    (..., refetch_truncated_field(stmt, static_cast<result_column_base<ColumnSpecs>&>(row)(), std::get<Is>(buffers),
                                  meta_data[Is], bind_parameters[Is], Is));
  }

  template <typename... ColumnSpecs>
  auto get_next_result_row(MYSQL_STMT* stmt,
                           result_row_t<ColumnSpecs...>& row,
                           std::tuple<buffer_type_of_t<ColumnSpecs>...>& buffers,
                           std::array<bind_meta_data_t, sizeof...(ColumnSpecs)>& meta_data,
                           std::array<MYSQL_BIND, sizeof...(ColumnSpecs)>& bind_parameters) -> bool
  {
    auto flag = mysql_stmt_fetch(stmt);

    switch (flag)
    {
      case 0:
      case MYSQL_DATA_TRUNCATED:
        refetch_truncated_fields(stmt, row, buffers, meta_data, bind_parameters,
                                 std::make_integer_sequence<unsigned, sizeof...(ColumnSpecs)>{});
        bind(stmt, bind_parameters);
        return true;
      case 1:
        throw sqlpp::exception(std::string("MySQL: Could not fetch next result: ") + mysql_stmt_error(stmt));
      case MYSQL_NO_DATA:
        return false;
      default:
        throw sqlpp::exception("MySQL: Unexpected return value for mysql_stmt_fetch()");
    }
  }
}  // namespace sqlpp::mysql

namespace sqlpp::mysql
{
  inline auto prepare_field_meta_parameter(bind_meta_data_t& meta_data, MYSQL_BIND& bind_parameter) -> void
  {
    bind_parameter.length = &meta_data.length;
    bind_parameter.is_null = &meta_data.is_null;
    bind_parameter.error = &meta_data.error;
  }

  inline auto prepare_field_parameter(int32_t& field,
                                      [[maybe_unused]] int32_t& buffer,
                                      bind_meta_data_t& meta_data,
                                      MYSQL_BIND& bind_parameter) -> void
  {
    bind_parameter.buffer_type = MYSQL_TYPE_LONG;
    bind_parameter.buffer = &field;
    bind_parameter.buffer_length = sizeof(field);
    bind_parameter.is_unsigned = false;
    prepare_field_meta_parameter(meta_data, bind_parameter);
  }

  inline auto prepare_field_parameter(int64_t& field,
                                      [[maybe_unused]] int64_t& buffer,
                                      bind_meta_data_t& meta_data,
                                      MYSQL_BIND& bind_parameter) -> void
  {
    bind_parameter.buffer_type = MYSQL_TYPE_LONGLONG;
    bind_parameter.buffer = &field;
    bind_parameter.buffer_length = sizeof(field);
    bind_parameter.is_unsigned = false;
    prepare_field_meta_parameter(meta_data, bind_parameter);
  }

  inline auto prepare_field_parameter(float& field,
                                      [[maybe_unused]] float& buffer,
                                      bind_meta_data_t& meta_data,
                                      MYSQL_BIND& bind_parameter) -> void
  {
    bind_parameter.buffer_type = MYSQL_TYPE_FLOAT;
    bind_parameter.buffer = &field;
    bind_parameter.buffer_length = sizeof(field);
    bind_parameter.is_unsigned = false;
    prepare_field_meta_parameter(meta_data, bind_parameter);
  }

  inline auto prepare_field_parameter(double& field,
                                      [[maybe_unused]] double& buffer,
                                      bind_meta_data_t& meta_data,
                                      MYSQL_BIND& bind_parameter) -> void
  {
    bind_parameter.buffer_type = MYSQL_TYPE_DOUBLE;
    bind_parameter.buffer = &field;
    bind_parameter.buffer_length = sizeof(field);
    bind_parameter.is_unsigned = false;
    prepare_field_meta_parameter(meta_data, bind_parameter);
  }

  inline auto prepare_field_parameter([[maybe_unused]] std::string_view& field,
                                      std::string& buffer,
                                      bind_meta_data_t& meta_data,
                                      MYSQL_BIND& bind_parameter) -> void
  {
    bind_parameter.buffer_type = MYSQL_TYPE_STRING;
    bind_parameter.buffer = buffer.data();
    bind_parameter.buffer_length = buffer.size();
    bind_parameter.is_unsigned = false;
    prepare_field_meta_parameter(meta_data, bind_parameter);
  }

  inline auto prepare_field_parameter(std::string& field,
                                      [[maybe_unused]] std::string& buffer,
                                      bind_meta_data_t& meta_data,
                                      MYSQL_BIND& bind_parameter) -> void
  {
    bind_parameter.buffer_type = MYSQL_TYPE_STRING;
    bind_parameter.buffer = field.data();
    bind_parameter.buffer_length = field.size();
    bind_parameter.is_unsigned = false;
    prepare_field_meta_parameter(meta_data, bind_parameter);
  }

  template <typename Field, typename Buffer>
  auto prepare_field_parameter(std::optional<Field>& field,
                               Buffer& buffer,
                               bind_meta_data_t& meta_data,
                               MYSQL_BIND& bind_parameter) -> void
  {
    // In case of optional fields, always bind to the buffer (the field could be unavailable if the previous value was NULL)
    prepare_field_parameter(buffer, buffer, meta_data, bind_parameter);
  }

  template <typename... ColumnSpecs,
            unsigned... Is>
  auto prepare_field_parameters(result_row_t<ColumnSpecs...>& row,
                                std::tuple<buffer_type_of_t<ColumnSpecs>...>& buffers,
                                std::array<bind_meta_data_t, sizeof...(ColumnSpecs)>& meta_data,
                                std::array<MYSQL_BIND, sizeof...(ColumnSpecs)>& bind_parameters,
                                std::integer_sequence<unsigned, Is...>) -> void
  {
    (..., prepare_field_parameter(static_cast<result_column_base<ColumnSpecs>&>(row)(), std::get<Is>(buffers),
                                  meta_data[Is], bind_parameters[Is]));
  }

  template <typename Field, typename Buffer>
  auto assign_field(Field& field, const Buffer& buffer, const bind_meta_data_t& meta_data) -> void
  {
  }

  inline auto assign_field(std::string_view& field,
                           const std::string& buffer,
                           [[maybe_unused]] const bind_meta_data_t meta_data) -> void
  {
    field = std::string_view{buffer.data(), meta_data.length};
  }

  template <typename Field, typename Buffer>
  auto assign_field(std::optional<Field>& field, const Buffer& buffer, const bind_meta_data_t& meta_data)
      -> void
  {
    if (meta_data.is_null)
    {
      field.reset();
    }
    else
    {
      field = buffer;
    }
  }

  template <typename... ColumnSpecs,
            unsigned... Is>
  auto assign_fields(result_row_t<ColumnSpecs...>& row,
                     const std::tuple<buffer_type_of_t<ColumnSpecs>...>& buffers,
                     const std::array<bind_meta_data_t, sizeof...(ColumnSpecs)>& meta_data,
                     std::integer_sequence<unsigned, Is...>) -> void
  {
    (..., assign_field(static_cast<result_column_base<ColumnSpecs>&>(row)(), std::get<Is>(buffers), meta_data[Is]));
  }

  template <typename ResultRow>
  class prepared_statement_result_t
  {
    static_assert(wrong<ResultRow>, "ResultRow must be a result_row_t<...>");
  };

  template <typename... ColumnSpecs>
  class prepared_statement_result_t<result_row_t<ColumnSpecs...>>
  {
    detail::unique_prepared_result_ptr _handle;
    bool _unbound = true;
    std::tuple<buffer_type_of_t<ColumnSpecs>...> _bind_buffers; // For receiving optional values
    std::array<bind_meta_data_t, sizeof...(ColumnSpecs)> _bind_meta_data;  // For receiving is_null / length
    std::array<MYSQL_BIND, sizeof...(ColumnSpecs)> _bind_parameters;

    result_row_t<ColumnSpecs...> _row;

  public:
    using row_type = decltype(_row);

    prepared_statement_result_t() = default;
    prepared_statement_result_t(detail::unique_prepared_result_ptr&& handle, size_t number_of_columns)
        : _handle(std::move(handle))
    {
    }
    prepared_statement_result_t(const prepared_statement_result_t&) = delete;
    prepared_statement_result_t(prepared_statement_result_t&& rhs) = default;
    prepared_statement_result_t& operator=(const prepared_statement_result_t&) = delete;
    prepared_statement_result_t& operator=(prepared_statement_result_t&& rhs) = default;
    ~prepared_statement_result_t() = default;

    auto get_next_row() -> void
    {
      if (_unbound)
      {
        prepare_field_parameters(_row, _bind_buffers, _bind_meta_data, _bind_parameters,
                                 std::make_integer_sequence<unsigned, sizeof...(ColumnSpecs)>{});
        bind(_handle.get(), _bind_parameters);
        _unbound = false;
      }

      if (get_next_result_row(_handle.get(), _row, _bind_buffers, _bind_meta_data, _bind_parameters))
      {
        // assign bound fields, where necessary (e.g. optional columns, string_views)
        assign_fields(_row, _bind_buffers, _bind_meta_data, std::make_integer_sequence<unsigned, sizeof...(ColumnSpecs)>{});
      }
      else
      {
        reset();
      }
    }

    [[nodiscard]] auto& row() const
    {
      return _row;
    }

    [[nodiscard]] operator bool() const
    {
      return !!_handle;
    }

    [[nodiscard]] auto* get() const
    {
      return _handle.get();
    }

    auto reset() -> void
    {
      operator=(prepared_statement_result_t{});
    }
  };

}  // namespace sqlpp::mysql


