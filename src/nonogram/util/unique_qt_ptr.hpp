#pragma once

#include <QtCore/QPointer>

#include <memory>
#include <type_traits>

namespace nonogram
{
  namespace util
  {
    template<typename T>
      class unique_qt_ptr
    {
    public:
      template<typename... Args>
        unique_qt_ptr (Args&&... args)
          : scoped_ (std::make_unique<T> (std::forward<Args> (args)...))
          , qt_owned_ (nullptr)
      {}

      template<typename U>
        unique_qt_ptr (std::unique_ptr<U> scoped)
          : scoped_ (std::move (scoped))
          , qt_owned_ (nullptr)
      {}

      template<typename U>
        unique_qt_ptr (unique_qt_ptr<U>&& other)
          : scoped_ (std::move (other.scoped_))
          , qt_owned_ (other.qt_owned_)
      {
        other.qt_owned_ = nullptr;
      }

      T* release()
      {
        return qt_owned_ = scoped_.release();
      }

      T* get()
      {
        return scoped_ ? scoped_.get() : static_cast<T*> (qt_owned_);
      }
      T& operator*() { return get(); }
      T* operator->() { return get(); }
      T const* get() const
      {
        return scoped_ ? scoped_.get() : static_cast<T const*> (qt_owned_);
      }
      T const& operator*() const { return get(); }
      T const* operator->() const { return get(); }

    private:
      template<typename> friend class unique_qt_ptr;

      std::unique_ptr<T> scoped_;
      //! \note Use QPointer to detect if Qt deleted the object in
      //! between if possible
      typename std::conditional < std::is_base_of<QObject, T>::value
                                , QPointer<T>
                                , T*
                                >::type qt_owned_;
    };
  }
}
