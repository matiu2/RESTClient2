#pragma once

#include <memory>
#include <utility>

template <typename T> class pimpl {
private:
  std::unique_ptr<T> m;

public:
  pimpl();
  template <typename... Args> pimpl(Args &&...);
  ~pimpl();
  T *operator->();
  T &operator*();
};

template <typename T> pimpl<T>::pimpl() : m{new T{}} {}

template <typename T>
template <typename... Args>
pimpl<T>::pimpl(Args &&... args) : m{new T{std::forward<Args>(args)...}} {}

template <typename T> pimpl<T>::~pimpl() {}

template <typename T> T *pimpl<T>::operator->() { return m.get(); }

template <typename T> T &pimpl<T>::operator*() { return *m.get(); }
