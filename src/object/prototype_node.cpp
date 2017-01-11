/*
 * This file is part of OpenObjectStore OOS.
 *
 * OpenObjectStore OOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenObjectStore OOS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenObjectStore OOS. If not, see <http://www.gnu.org/licenses/>.
 */

#include "oos/object/prototype_node.hpp"
#include "oos/object/object_exception.hpp"
#include "oos/object/object_proxy.hpp"

#include <algorithm>

using namespace std;

namespace oos {

prototype_node::prototype_node()
  : type_index_(typeid(void))
{}

prototype_node::~prototype_node()
{}

void prototype_node::initialize(object_store *tree, const char *type, bool abstract)
{
  tree_ = tree;
  first.reset(new prototype_node);
  last.reset(new prototype_node);
  type_.assign(type);
  abstract_ = abstract;
  first->next = last.get();
  last->prev = first.get();
}

bool
prototype_node::empty(bool self) const
{
  return op_first->next() == (self ? op_marker : op_last);
}

unsigned long
prototype_node::size() const
{
  return count;
}

const char *prototype_node::type() const
{
  return type_.c_str();
}

const char *prototype_node::type_id() const
{
  return type_index_.name();
}

void prototype_node::append(prototype_node *sibling)
{
  sibling->parent = parent;

  sibling->prev = this;
  sibling->next = next;
  next = sibling;
  sibling->next->prev = sibling;
  sibling->depth = depth;

  if (!parent) {
    sibling->op_first = new object_proxy();
//    sibling->op_first = new object_proxy(nullptr);
    sibling->op_last = sibling->op_marker = new object_proxy();
//    sibling->op_last = sibling->op_marker = new object_proxy(nullptr);
    sibling->op_first->next_ = sibling->op_last;
    sibling->op_last->prev_ = sibling->op_first;
  } else {
    throw object_exception("");
    // 1. first
//    if (op_first->next() == op_last) {
//      // node hasn't any serializable (proxy)
//      sibling->op_first = op_first;
//    } else {
//      // node has some objects (proxy)
//      sibling->op_first = op_last->prev_;
//    }
//    // 2. marker
//    sibling->op_marker = op_last;
//    // 3. last
//    sibling->op_last = op_last;
  }
}

void prototype_node::insert(prototype_node *child)
{
  child->parent = this;
  child->prev = last->prev;
  child->next = last.get();
  last->prev->next = child;
  last->prev = child;
  // set depth
  child->depth = depth + 1;
  // set object proxy pointer
  // 1. first
  if (op_first->next() == op_last) {
    // node hasn't any serializable (proxy)
    child->op_first = op_first;
  } else {
    // node has some objects (proxy)
    child->op_first = op_last->prev_;
  }
  // 2. marker
  child->op_marker = op_last;
  // 3. last
  child->op_last = op_last;
}

void prototype_node::insert(object_proxy *proxy)
{
  // check count of serializable in subtree
  if (count >= 2) {

     // there are more than two objects (normal case)
     // insert before last last

    proxy->link(op_marker->prev_);
  } else if (count == 1) {

     // there is one serializable in subtree
     // insert as first; adjust "left" marker

    proxy->link(op_marker->prev_);
    adjust_left_marker(this, proxy->next_, proxy);
  } else { // node->count == 0

     // there is no serializable in subtree
     //insert as last; adjust "right" marker

    proxy->link(op_marker);
    adjust_left_marker(this, proxy->next_, proxy);
    adjust_right_marker(this, proxy->prev_, proxy);
  }
  // set prototype node
  proxy->node_ = this;
  // adjust size
  ++count;
  // find and insert primary key
  std::shared_ptr<basic_identifier> pk(proxy->primary_key_);
  if (pk) {
    id_map_.insert(std::make_pair(pk, proxy));
  }
}

void prototype_node::remove(object_proxy *proxy)
{
  if (proxy == op_first->next()) {
    // adjust left marker
    adjust_left_marker(this, op_first->next_, op_first->next_->next_);
  }
  if (proxy == op_marker->prev()) {
    // adjust right marker
    adjust_right_marker(this, proxy, op_marker->prev_->prev_);
  }
  // unlink object_proxy
  if (proxy->prev()) {
    proxy->prev_->next_ = proxy->next_;
  }
  if (proxy->next()) {
    proxy->next_->prev_ = proxy->prev_;
  }
  proxy->prev_ = nullptr;
  proxy->next_ = nullptr;

  if (has_primary_key()) {
    if (id_map_.erase(proxy->primary_key_) == 0) {
      // couldn't find and erase primary key
    }
  }

  // adjust serializable count for node
  --count;
}

void prototype_node::clear(bool recursive)
{
  if (!empty(true)) {
    adjust_left_marker(this, op_first->next_, op_marker);
    adjust_right_marker(this, op_marker->prev_, op_first);

    while (op_first->next() != op_marker) {
      object_proxy *op = op_first->next_;
      // remove serializable proxy from list
      op->unlink();
      // delete serializable proxy and serializable
      delete op;
    }
    id_map_.clear();
    count = 0;
  }

  if (recursive) {
    prototype_node *current = first->next;

    while (current != last.get()) {
      current->clear(recursive);
      current = current->next;
    }
  }
}

void prototype_node::unlink()
{
  // unlink node
  prev->next = next;
  next->prev = prev;
  next = 0;
  prev = 0;
}

prototype_node* prototype_node::next_node() const
{
  // if we have a child, child is the next iterator to return
  // (if we don't do iterate over the siblings)
  if (first && first->next != last.get())
    return first->next;
  else {
    // if there is no child, we check for sibling
    // if there is a sibling, this is our next iterator to return
    // if not, we go back to the parent
    const prototype_node *node = this;
    while (node->parent && node->next == node->parent->last.get()) {
      node = node->parent;
    }
    return node->next;
  }
}

prototype_node* prototype_node::next_node(const prototype_node *root) const
{
  // if we have a child, child is the next iterator to return
  // (if we don't do iterate over the siblings)
  if (first && first->next != last.get())
    return first->next;
  else {
    // if there is no child, we check for sibling
    // if there is a sibling, this is our next iterator to return
    // if not, we go back to the parent
    const prototype_node *node = this;
    while (node->parent && node->next == node->parent->last.get() && node->parent != root) {
      node = node->parent;
    }
    return node->next;
  }
}

prototype_node* prototype_node::previous_node() const
{
  // if node has a previous sibling, we set it
  // as our next iterator. then we check if there
  // are last childs. if so, we set the last last
  // child as our iterator
  if (prev && prev->prev) {
    const prototype_node *node = prev;
    while (node->last && node->first->next != node->last.get()) {
      node = node->last->prev;
    }
    return const_cast<prototype_node*>(node);
    // if there is no previous sibling, our next iterator
    // is the parent of the node
  } else {
    return parent;
  }
}

prototype_node* prototype_node::previous_node(const prototype_node *root) const
{
  // if node has a previous sibling, we set it
  // as our next iterator. then we check if there
  // are last childs. if so, we set the last last
  // child as our iterator
  if (prev && prev->prev) {
    const prototype_node *node = prev;
    while (node->last && node->first->next != node->last.get() && node->parent != root) {
      node = node->last->prev;
    }
    return const_cast<prototype_node*>(node);
    // if there is no previous sibling, our next iterator
    // is the parent of the node
  } else {
    return parent;
  }
}

object_store *prototype_node::tree() const
{
  return tree_;
}

bool prototype_node::is_child_of(const prototype_node *parent) const
{
  const prototype_node *node = this;
  while (parent->depth < node->depth) {
    node = node->parent;
  }
  return node == parent;
}


bool prototype_node::has_children() const
{
  return first->next != last.get();
}

bool prototype_node::has_primary_key() const
{
  return id_.get() != nullptr;
}

basic_identifier *prototype_node::id() const
{
  return id_.get();
}

//size_t prototype_node::relation_count() const
//{
//  return relations.size();
//}
//
//bool prototype_node::has_relation(const std::string &relation_name) const
//{
//  return relations.find(relation_name) != relations.end();
//}

//size_t prototype_node::foreign_key_count() const
//{
//  return foreign_keys.size();
//}
//
//bool prototype_node::has_foreign_key(const std::string &foreign_key_name) const
//{
//  return foreign_keys.find(foreign_key_name) != foreign_keys.end();
//}

bool prototype_node::is_abstract() const
{
  return abstract_;
}

std::type_index prototype_node::type_index() const
{
  return type_index_;
}

object_proxy *prototype_node::find_proxy(const std::shared_ptr<basic_identifier> &pk)
{
  detail::t_identifier_map::iterator i = std::find_if(id_map_.begin(), id_map_.end(), [pk](const detail::t_identifier_map::value_type &x) {
    return *pk == *(x.first);
  });
//  t_primary_key_map::iterator i = primary_key_map.find(pk);
  return (i != id_map_.end() ? i->second : nullptr);
}

void prototype_node::register_belongs_to(const std::type_index &tindex, const prototype_node::relation_info &relation_info)
{
  belongs_to_map_.insert(std::make_pair(tindex, relation_info));
}

void prototype_node::register_has_many(const std::string &field, prototype_node *node)
{
  has_many_map_.insert(std::make_pair(field, node));
}

/*
 * adjust the marker of all predeccessor nodes
 * self and last marker
 */
void prototype_node::adjust_left_marker(prototype_node *root, object_proxy *old_proxy, object_proxy *new_proxy)
{
  // store start node
  prototype_node *node = root->previous_node();
  // get previous node
  while (node) {
    if (node->op_marker == old_proxy) {
      node->op_marker = new_proxy;
    }
    if (node->depth >= root->depth && node->op_last == old_proxy) {
      node->op_last = new_proxy;
    }
    node = node->previous_node();
  }
}

void prototype_node::adjust_right_marker(prototype_node *root, object_proxy* old_proxy, object_proxy *new_proxy)
{
  // store start node
  prototype_node *node = root->next_node();
  while (node) {
    if (node->op_first == old_proxy) {
      node->op_first = new_proxy;
    }
    node = node->next_node();
  }
}

std::ostream& operator <<(std::ostream &os, const prototype_node &pn)
{
  if (pn.parent) {
    os << "\t" << pn.parent->type_ << " -> " << pn.type_ << "\n";
  }
  os << "\t" << pn.type_ << " [label=\"{" << pn.type_ << " (" << &pn << ")";
  os << "|{op_first|" << pn.op_first << "}";
  os << "|{op_marker|" << pn.op_marker << "}";
  os << "|{op_last|" << pn.op_last << "}";
  os << "|{parent|" << pn.parent << "}";
  os << "|{prev|" << pn.prev << "}";
  os << "|{next|" << pn.next << "}";
  os << "|{first|" << pn.first.get() << "}";
  os << "|{last|" << pn.last.get() << "}";
  // determine size
  int i = 0;
  const object_proxy *iop = pn.op_first;
  while (iop && iop->next() != pn.op_marker) {
    ++i;
    iop = iop->next();
  }
  os << "|{size|" << i << "}";

  os << "|{relations}";
  // list relations
//  prototype_node::field_prototype_map_t::const_iterator first = pn.relations.begin();
//  prototype_node::field_prototype_map_t::const_iterator last = pn.relations.end();
//  while (first != last) {
//    os << "|{parent node type|" << first->first << "|node|" << first->second.first->type() << "|foreign field|" << first->second.second << "}";
//    ++first;
//  }
  os << "}\"]\n";
  return os;
}

void prototype_node::notify_delete_relation(object_proxy *owner, object_proxy *proxy)
{
  std::cout << "(" << type_ << ") deleting one relation [" << proxy->node()->type() << "] for owner [" << owner->node()->type() << "]\n";
}

void prototype_node::notify_insert_relation(object_proxy *owner, object_proxy *proxy)
{
  std::cout << "(" << type_ << ") inserting one relation [" << proxy->node()->type() << "] for owner [" << owner->node()->type() << "]\n";
}

}
