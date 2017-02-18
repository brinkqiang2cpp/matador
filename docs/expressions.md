---
title: Expressions
---
{% include base_path %}

When working with an ```oos::object_view``` you may want to find
a certain object or you want to apply a function only for
some of the objects in the view.

To achieve this we can use the ```oos::object_view::find_if``` method
or we can use the global ```::for_each_if()``` function. But how can we
define or filter criterion?

First add some elements (persons) to the store and create the view.
As you can see we've extended the person class with an age attribute.

```cpp
// add some persons
ostore.insert(new person("joe", 45))
ostore.insert(new person("walter", 56));
ostore.insert(new person("helen", 37));
ostore.insert(new person("tim", 14));
ostore.insert(new person("linda", 25));
ostore.insert(new person("lea", 30));
ostore.insert(new person("georg", 42));
 
typedef oos::object_view<person> person_view_t;
typedef person_view_t::iterator iterator;

person_view_t pview(ostore);
```

If we want to find the person of age 25 we can achieve this by using
the ```oos::object_view::find_if``` method. But first we have to create a
variable holding the method of our object which should be used for
comparation. Here it is ```person::age```.

```cpp
// create the variable
oos::variable<int> x(oos::make_var(&person::age));

// find the person of age 25
iterator i = pview.find_if(x == 25);
```

The call returns the first occurence of the object matching
the expression or the end iterator.

If you want to apply a function to all objects matching a
certain expression use ```oos::for_each_if()``` and use it in the
following described way.

```cpp
// print all persons with age between 20 and 60 years
void print(const person_ptr &p)
{
  std::cout << p->name() << " is between 40 and 60.\n";
}

// declare a variable/literal for the age
oos::variable<int> x(oos::make_var(&person::age));

// use the for_each_if function
for_each_if(pview.begin(), pview.end(), x > 20 && x < 60, print);
```

All persons whos age lays between 40 and 60 are printed.
