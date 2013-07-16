#ifndef HANDLE_H
#define HANDLE_H

#include <iterator>

#define DELEGATE_COMPARAISON_OPERATORS(cl, mem) \
  bool operator<(const cl & o) const            \
  { return mem < o.mem; }                       \
  bool operator>(const cl & o) const            \
  { return mem > o.mem; }                       \
  bool operator<=(const cl & o) const           \
  { return mem <= o.mem; }                      \
  bool operator>=(const cl & o) const           \
  { return mem >= o.mem; }                      \
  bool operator==(const cl & o) const           \
  { return mem == o.mem; }                      \
  bool operator!=(const cl & o) const           \
  { return mem != o.mem; }

// Handle class, emulating pointer functionality
// while hiding ownership (a handle doesn't own the
// pointed member object)
template <typename T>
class Handle
{
  public:
    typedef T Type;

    Handle():
      _t(0) {}

    Handle(Type & t):
      _t(&t) {}

    bool is_null() const
    { return _t == 0; }

    Type * ptr() const
    { return _t; }

    Type & ref() const
    { return *_t; }

    Type & operator*() const
    { return ref(); }

    Type * operator->() const
    { return ptr(); }

    DELEGATE_COMPARAISON_OPERATORS(Handle<T>, _t)

  private:
      Type * _t;
};

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

#undef DELEGATE_COMPARAISON_OPERATORS // FIXME

#endif // HANDLE_H // vim: sw=2 et ts=2 sts=2
