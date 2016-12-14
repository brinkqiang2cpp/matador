//
// Created by sascha on 12.03.16.
//

#ifndef OOS_INSERT_ACTION_HPP
#define OOS_INSERT_ACTION_HPP

#ifdef _MSC_VER
#ifdef oos_EXPORTS
    #define OOS_API __declspec(dllexport)
    #define EXPIMP_TEMPLATE
  #else
    #define OOS_API __declspec(dllimport)
    #define EXPIMP_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
#else
#define OOS_API
#endif

#include <algorithm>

#include "oos/object/action.hpp"

namespace oos {

class object_proxy;
class object_serializer;

/// @cond OOS_DEV

/**
 * @internal
 * @class insert_action
 * @brief Action when inserting an serializable.
 *
 * This action is used when an objected
 * is inserted into the database.
 */
class OOS_API insert_action : public action
{
public:
  typedef std::list<object_proxy*> object_proxy_list_t;
  typedef object_proxy_list_t::iterator iterator;
  typedef object_proxy_list_t::const_iterator const_iterator;

public:
  /**
   * Creates an insert_action.
   *
   * @param type The type of the expected objects
   */
  template < class T >
  explicit insert_action(const std::string &type, T*)
    : type_(type)
  {}

  virtual void accept(action_visitor *av);

  /**
   * Return the object type
   * of the action.
   *
   * @return The object type of the action
   */
  std::string type() const;

  iterator begin();
  const_iterator begin() const;

  iterator end();
  const_iterator end() const;

  bool empty() const;

  iterator find(unsigned long id);
  const_iterator find(unsigned long id) const;

  void push_back(object_proxy *proxy);

  iterator erase(iterator i);

  virtual void backup(byte_buffer &) { }

  virtual void restore(byte_buffer &, object_store *store);

private:
  std::string type_;
  object_proxy_list_t object_proxy_list_;
};

/// @endcond

}

#endif //OOS_INSERT_ACTION_HPP
