#ifndef HANDLE_H
#define HANDLE_H

// Handle class, emulating pointer functionality
// while hiding ownership (a handle doesn't own the
// pointed member object)
//
// Note: the addressing operator isn't overloaded
// to leave functionalities needing the actual
// object's address available (ex: Qt signals/slots)
template <typename T>
class Handle
{
  public:
    typedef T Type;

    Handle();
    Handle(Type &);

    // Check if the handle is a "null" handle,
    // that is pointing to no underlying value
    bool is_null() const;

    // Accessors (pointer, reference)
    Type * ptr() const;
    Type & ref() const;

    // Operators all delegated to members
    Type & operator*() const;
    Type * operator->() const;
    bool operator<(const Handle<T> &) const;
    bool operator>(const Handle<T> &) const;
    bool operator<=(const Handle<T> &) const;
    bool operator>=(const Handle<T> &) const;
    bool operator==(const Handle<T> &) const;
    bool operator!=(const Handle<T> &) const;

  private:
      Type * _t;
};

#include <iterator>

// Handle iterator, an iterator wrapping any other InputIterator
// but yielding a Handle for the actual underlying value.
//
// Also follows the STL's InputIterator concept.
template <typename Iterator>
class Handle_iterator:
  public std::iterator<std::input_iterator_tag,
  Handle<typename Iterator::value_type> >
{
  typedef Iterator It;
  typedef Handle_iterator<It> Self;

  public:
    typedef Handle<typename Iterator::value_type const> Handled;

    Handle_iterator(const It & it):
      _it(it) {}

    Self & operator++()
    { ++_it; return *this; }

    Self operator++(int)
    { Self tmp(*this);
      ++(*this); return tmp; }

    bool operator==(const Self & sit) const
    { return _it == sit._it; }

    bool operator!=(const Self & sit) const
    { return !(*this == sit); }

    Handled operator*()
    { return Handled(*_it); }

  private:
    It _it;
};

#endif // HANDLE_H // vim: sw=2 et ts=2 sts=2
