//
// Created by sascha on 6/9/16.
//

#include <iostream>
#include "sql/sql.hpp"
#include "sql/basic_dialect_compiler.hpp"
#include "sql/basic_dialect.hpp"

namespace oos {

namespace detail {

void basic_dialect_compiler::compile(basic_dialect &dialect)
{
  on_compile_start();

  auto last = dialect.top().tokens_.end();
  token_list_t::iterator next;

  while (dialect.top().current != last) {
    (next = dialect.top().current)++;
    (*dialect.top().current)->accept(*this);
      dialect.top().current = next;
  }

  on_compile_finish();
}

void basic_dialect_compiler::visit(const oos::detail::create &) { }

void basic_dialect_compiler::visit(const oos::detail::drop &) { }

void basic_dialect_compiler::visit(const oos::detail::select &) { }

void basic_dialect_compiler::visit(const oos::detail::distinct &) { }

void basic_dialect_compiler::visit(const oos::detail::update &) { }

void basic_dialect_compiler::visit(const oos::detail::tablename &) { }

void basic_dialect_compiler::visit(const oos::detail::set &) { }

void basic_dialect_compiler::visit(const oos::columns &) { }

void basic_dialect_compiler::visit(const oos::column &) { }

void basic_dialect_compiler::visit(const oos::detail::typed_column &) { }

void basic_dialect_compiler::visit(const oos::detail::typed_identifier_column &) { }

void basic_dialect_compiler::visit(const oos::detail::typed_varchar_column &) { }

void basic_dialect_compiler::visit(const oos::detail::identifier_varchar_column &) { }

void basic_dialect_compiler::visit(const oos::detail::basic_value_column &) { }

void basic_dialect_compiler::visit(const oos::detail::from &) { }

void basic_dialect_compiler::visit(const oos::detail::where &) { }

void basic_dialect_compiler::visit(const oos::detail::basic_condition &) { }

void basic_dialect_compiler::visit(const oos::detail::basic_column_condition &) { }

void basic_dialect_compiler::visit(const oos::detail::basic_in_condition &) { }

void basic_dialect_compiler::visit(const oos::detail::order_by &) { }

void basic_dialect_compiler::visit(const oos::detail::asc &) { }

void basic_dialect_compiler::visit(const oos::detail::desc &) { }

void basic_dialect_compiler::visit(const oos::detail::group_by &) { }

void basic_dialect_compiler::visit(const oos::detail::insert &) { }

void basic_dialect_compiler::visit(const oos::detail::values &) { }

void basic_dialect_compiler::visit(const oos::detail::basic_value &) { }

void basic_dialect_compiler::visit(const oos::detail::remove &) { }

void basic_dialect_compiler::visit(const oos::detail::top &) { }

void basic_dialect_compiler::visit(const oos::detail::as &) { }

void basic_dialect_compiler::visit(const oos::detail::begin &) { }

void basic_dialect_compiler::visit(const oos::detail::commit &) { }

void basic_dialect_compiler::visit(const oos::detail::rollback &) { }

void basic_dialect_compiler::visit(oos::detail::query &) {}

void basic_dialect_compiler::on_compile_start() { }

void basic_dialect_compiler::on_compile_finish() { }

basic_dialect &basic_dialect_compiler::dialect() const
{
  return *dialect_;
}

build_info &basic_dialect_compiler::top() const
{
  return dialect_->top();
}

void basic_dialect_compiler::dialect(basic_dialect *d)
{
  dialect_ = d;
}

}

}
