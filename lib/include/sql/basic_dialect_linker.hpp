//
// Created by sascha on 14.08.16.
//

#ifndef OOS_BASIC_DIALECT_LINKER_HPP
#define OOS_BASIC_DIALECT_LINKER_HPP

#ifdef _MSC_VER
#ifdef oos_EXPORTS
    #define OOS_API __declspec(dllexport)
    #define EXPIMP_TEMPLATE
  #else
    #define OOS_API __declspec(dllimport)
    #define EXPIMP_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
  #pragma warning(disable: 4355)
#else
#define OOS_API
#endif

#include "sql/token_visitor.hpp"
#include "sql/token.hpp"
#include "sql/token_list.hpp"

#include <string>
#include <stack>

namespace oos {

class basic_dialect;

namespace detail {

struct build_info;

/// @cond OOS_DEV

class OOS_API basic_dialect_linker : public token_visitor
{
public:
  void link();

  std::string token_string(detail::token::t_token tok) const;

  virtual void visit(const oos::detail::create &) override;
  virtual void visit(const oos::detail::drop &) override;
  virtual void visit(const oos::detail::select &) override;
  virtual void visit(const oos::detail::distinct &) override;
  virtual void visit(const oos::detail::update &) override;
  virtual void visit(const oos::detail::tablename &table) override;
  virtual void visit(const oos::detail::set &) override;
  virtual void visit(const oos::detail::as &) override;
  virtual void visit(const oos::detail::top &) override;
  virtual void visit(const oos::detail::remove &) override;
  virtual void visit(const oos::detail::values &values) override;
  virtual void visit(const oos::detail::basic_value &) override;
  virtual void visit(const oos::detail::order_by &) override;
  virtual void visit(const oos::detail::asc &) override;
  virtual void visit(const oos::detail::desc &) override;
  virtual void visit(const oos::detail::group_by &) override;
  virtual void visit(const oos::detail::insert &) override;
  virtual void visit(const oos::detail::from &) override;
  virtual void visit(const oos::detail::where &) override;
  virtual void visit(const oos::detail::basic_condition &) override;
  virtual void visit(const oos::detail::basic_column_condition &) override;
  virtual void visit(const oos::detail::basic_in_condition &) override;
  virtual void visit(const oos::columns &) override;
  virtual void visit(const oos::column &) override;
  virtual void visit(const oos::detail::typed_column &) override;
  virtual void visit(const oos::detail::typed_identifier_column &) override;
  virtual void visit(const oos::detail::typed_varchar_column &) override;
  virtual void visit(const oos::detail::identifier_varchar_column &) override;
  virtual void visit(const oos::detail::basic_value_column &) override;
  virtual void visit(const oos::detail::begin &) override;
  virtual void visit(const oos::detail::commit &) override;
  virtual void visit(const oos::detail::rollback &) override;
  virtual void visit(oos::detail::query &) override;

protected:
  basic_dialect& dialect() const;
  build_info& top() const;

  void append_to_result(basic_dialect &dialect, const std::string &part);

private:
  friend class oos::basic_dialect;

  void dialect(basic_dialect *d);

  basic_dialect *dialect_;
};

/// @endcond

}

}

#endif //OOS_BASIC_DIALECT_LINKER_HPP
