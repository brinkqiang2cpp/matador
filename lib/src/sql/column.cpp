//
// Created by sascha on 3/2/16.
//

#include "sql/column.hpp"
#include "sql/token_visitor.hpp"

namespace oos {

column::column(const std::string &col)
  : token(COLUMN)
  , name(col)
{}

void column::accept(token_visitor &visitor)
{
  return visitor.visit(*this);
}

columns::columns(const std::initializer_list<std::string> &colnames, t_brackets with_brackets)
  : token(COLUMNS)
  , with_brackets_(with_brackets)
{
  for (auto &&colname : colnames) {
    push_back(std::make_shared<column>(colname));
  }
}

columns::columns(std::initializer_list<column> cols, t_brackets with_brackets)
  : token(COLUMNS)
  , with_brackets_(with_brackets)
{
  for (auto &&col : cols) {
    push_back(std::make_shared<column>(col));
  }
}

columns::columns(std::initializer_list<std::shared_ptr<column>> cols, t_brackets with_brackets)
  : token(COLUMNS)
  , with_brackets_(with_brackets)
{
  columns_.assign(cols);
}

columns::columns(t_brackets with_brackets)
  : token(COLUMNS)
  , with_brackets_(with_brackets)
{}

namespace detail {

typed_column::typed_column(const std::string &col, data_type t)
  : typed_column(col, t, 0, false)
{}

typed_column::typed_column(const std::string &col, data_type t, std::size_t idx, bool host)
  : column(col)
  , type(t), index(idx), is_host(host)
{}

void typed_column::accept(token_visitor &visitor)
{
  return visitor.visit(*this);
}

}

void columns::push_back(const std::shared_ptr<column> &col)
{
  columns_.push_back(col);
}

columns &columns::with_brackets()
{
  with_brackets_ = WITH_BRACKETS;
  return *this;
}

columns &columns::without_brackets()
{
  with_brackets_ = WITHOUT_BRACKETS;
  return *this;
}

void columns::accept(token_visitor &visitor)
{
  return visitor.visit(*this);
}

columns columns::all()
{
  return all_;
}

column columns::count_all()
{
  return count_all_;
}

columns columns::all_ = columns({std::make_shared<detail::unquoted_column>("*")}, WITHOUT_BRACKETS);
detail::unquoted_column columns::count_all_ = detail::unquoted_column("COUNT(*)");

}
