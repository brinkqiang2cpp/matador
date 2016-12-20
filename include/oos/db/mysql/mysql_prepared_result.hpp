#ifndef MYSQL_PREPARED_RESULT_HPP
#define MYSQL_PREPARED_RESULT_HPP

#include "oos/sql/result_impl.hpp"

#include "oos/db/mysql/mysql_result_info.hpp"

#ifdef _MSC_VER
//#include <winsock2.h>
#include <mysql.h>
#else
#include <mysql/mysql.h>
#endif

#include <vector>
#include <unordered_map>
#include <string>
#include <memory>

namespace oos {

class object_base_ptr;
class varchar_base;
class basic_identifier;
class time;
class date;

namespace mysql {

class mysql_prepared_result : public detail::result_impl
{
private:
  mysql_prepared_result(const mysql_prepared_result &) = delete;
  mysql_prepared_result &operator=(const mysql_prepared_result &) = delete;

public:
  typedef detail::result_impl::size_type size_type;
  typedef std::unordered_map<std::string, std::shared_ptr<basic_identifier> > t_pk_map;

public:
  mysql_prepared_result(MYSQL_STMT *s, int rs);
  ~mysql_prepared_result();

  virtual const char* column(size_type c) const override;
  virtual bool fetch() override;

  virtual size_type affected_rows() const override;
  virtual size_type result_rows() const override;
  virtual size_type fields() const override;

  virtual int transform_index(int index) const override;

  virtual void serialize(const char *id, char &x) override;
  virtual void serialize(const char *id, short &x) override;
  virtual void serialize(const char *id, int &x) override;
  virtual void serialize(const char *id, long &x) override;
  virtual void serialize(const char *id, unsigned char &x) override;
  virtual void serialize(const char *id, unsigned short &x) override;
  virtual void serialize(const char *id, unsigned int &x) override;
  virtual void serialize(const char *id, unsigned long &x) override;
  virtual void serialize(const char *id, bool &x) override;
  virtual void serialize(const char *id, float &x) override;
  virtual void serialize(const char *id, double &x) override;
  virtual void serialize(const char *id, char *x, size_t s) override;
  virtual void serialize(const char *id, oos::date &x) override;
  virtual void serialize(const char *id, oos::time &x) override;
  virtual void serialize(const char *id, std::string &x) override;
  virtual void serialize(const char *id, varchar_base &x) override;
  virtual void serialize(const char *id, basic_identifier &x) override;
  virtual void serialize(const char *id, identifiable_holder &x, cascade_type) override;

protected:
  virtual bool needs_bind() override;
  virtual bool finalize_bind() override;

  virtual bool prepare_fetch() override;
  virtual bool finalize_fetch() override;

private:

  template < class T >
  void prepare_bind_column(int index, enum_field_types type, T &value)
  {
    bind_[index].buffer_type = type;
    bind_[index].buffer= (char *)&value;
    bind_[index].buffer_length = sizeof(T);
    bind_[index].is_null = &info_[index].is_null;
    bind_[index].length = &info_[index].length;
    bind_[index].error = &info_[index].error;
  }

  void prepare_bind_column(int index, enum_field_types type, oos::date &value);
  void prepare_bind_column(int index, enum_field_types type, oos::time &value);
  void prepare_bind_column(int index, enum_field_types type, std::string &value);
  void prepare_bind_column(int index, enum_field_types type, char *x, size_t s);
  void prepare_bind_column(int index, enum_field_types type, varchar_base &value);

private:
  int column_index_ = 0;
  size_type affected_rows_;
  size_type rows;
  size_type fields_;
  MYSQL_STMT *stmt;
  int result_size;
  MYSQL_BIND *bind_;
  mysql_result_info *info_;

  t_pk_map pk_map_;

  bool prepare_binding_ = true;

  typedef std::unordered_map<std::string, std::shared_ptr<basic_identifier> > t_foreign_key_map;
  t_foreign_key_map foreign_keys_;
};

}

}

#endif /* MYSQL_PREPARED_RESULT_HPP */
