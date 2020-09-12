#pragma once

#include <type_traits>
#include <iterator>
#include <cassert>
#include <memory>

namespace intrusive
{
    struct default_tag;

    template <typename Tag = default_tag>
    struct list_element
    {
    private:
        template <typename FT, typename FTag>
        friend struct list;
        list_element *next = nullptr;
        list_element *prev = nullptr;
    public:
        void unlink()
        {
            if (next != nullptr)
                next->prev = prev;
            if (prev != nullptr)
                prev->next = next;
            prev = next = nullptr;
        }
    };

    template <typename T, typename Tag = default_tag>
    class list
    {
    private:
        list_element<Tag> *root = new list_element<Tag>;

        template<typename IT>
        class iterator_impl
        {
        public:
            using iterator_category = std::bidirectional_iterator_tag;
            using value_type = IT;
            using difference_type = std::ptrdiff_t;
            using pointer = value_type*;
            using reference	= value_type&;
        private:
            friend list;
            list_element<Tag>* me;
            explicit iterator_impl(list_element<Tag>* to) noexcept
                : me(to)
            {}
        public:
            iterator_impl(void) noexcept
                : me(nullptr)
            {}
            pointer operator->(void) const noexcept
            {
                assert(me != nullptr);
                return static_cast<IT*>(me);
            }
            operator reference(void) const
            {
                return static_cast<IT&>(*me);
            }

            reference operator*(void) const noexcept
            {
                assert(me != nullptr);
                return static_cast<IT&>(*me);
            }

            iterator_impl& operator++(void) noexcept
            {
                assert(me != nullptr);
                me = me->next;
                return *this;
            }
            iterator_impl operator++(int) noexcept
            {
                assert(me != nullptr);
                auto copy = me;
                me = me->next;
                return iterator_impl(copy);
            }
            iterator_impl& operator--(void) noexcept
            {
                assert(me != nullptr);
                me = me->prev;
                return *this;
            }
            iterator_impl operator--(int) noexcept
            {
                assert(me != nullptr);
                auto copy = me;
                me = me->prev;
                return iterator_impl(copy);
            }

            template<typename T1>
            bool operator==(const iterator_impl<T1> &r) noexcept
            {
                return me == r.me;
            }
            template<typename T1>
            bool operator!=(const iterator_impl<T1> &r) noexcept
            {
                return !operator==(r);
            }

            operator iterator_impl<const value_type>(void) const noexcept
            {
                return iterator_impl<const value_type>(me);
            }
        };
        static list_element<Tag> & cast_el(T &r) noexcept
        {
            return static_cast<list_element<Tag>&>(r);
        }
    public:
        using iterator = iterator_impl<T>;
        using const_iterator = iterator_impl<const T>;

        static_assert(std::is_convertible_v<T&, list_element<Tag>&>,
                      "value type is not convertible to list_element");

        list() noexcept
        {
            clear();
        }
        list(list const&) = delete;
        list(list&& r) noexcept
        {
            operator=(std::move(r));
        }
        ~list()
        {
            if (!empty())
                root->next->prev = root->prev->next = nullptr;
            delete root;
        }

        list& operator=(list const&) = delete;
        list& operator=(list&& r) noexcept
        {
            if (r.empty())
            {
                clear();
                return *this;
            }
            r.root->next->prev = root;
            root->next = r.root->next;
            r.root->prev->next = root;
            root->prev = r.root->prev;
            r.clear();
            return *this;
        }

        void clear() noexcept
        {
            root->next = root->prev = root;
        }

        void push_back(T& u) noexcept
        {
            auto &v = static_cast<list_element<Tag>&>(u);
            root->prev->next = &v;
            v.prev = root->prev;
            v.next = root;
            root->prev = &v;
        }
        void pop_back() noexcept
        {
            assert(!empty());
            root->prev->unlink();
        }
        T& back() noexcept
        {
            assert(!empty());
            return static_cast<T&>(*root->prev);
        }
        T const& back() const noexcept
        {
            assert(!empty());
            return static_cast<const T&>(*root->prev);
        }

        void push_front(T& u) noexcept
        {
            auto &v = static_cast<list_element<Tag>&>(u);
            root->next->prev = &v;
            v.next = root->next;
            v.prev = root;
            root->next = &v;
        }
        void pop_front() noexcept
        {
            assert(!empty());
            root->next->unlink();
        }
        T& front() noexcept
        {
            assert(!empty());
            return static_cast<T&>(*root->next);
        }
        T const& front() const noexcept
        {
            assert(!empty());
            return static_cast<T&>(*root->next);
        }

        bool empty() const noexcept
        {
            return root->next == root;
        }

        iterator begin() noexcept
        {
            return iterator(root->next);
        }
        const_iterator begin() const noexcept
        {
            return const_iterator(root->next);
        }

        iterator end() noexcept
        {
            return iterator(root);
        }
        const_iterator end() const noexcept
        {
            return const_iterator(root);
        }

        iterator insert(const_iterator pos, T& u) noexcept
        {
            auto &v = static_cast<list_element<Tag>&>(u);
            pos.me->prev->next = &v;
            v.prev = pos.me->prev;
            v.next = pos.me;
            pos.me->prev = &v;
            return iterator(&v);
        }
        iterator erase(const_iterator pos) noexcept
        {
            assert(pos.me != root);
            iterator ret(pos.me->next);
            pos.me->unlink();
            return ret;
        }
        void splice(const_iterator pos, list&, const_iterator first, const_iterator last) noexcept
        {
            if (pos == first || first == last)
                return;
            auto *true_last = last.me->prev;
            first.me->prev->next = true_last->next;
            true_last->next->prev = first.me->prev;

            pos.me->prev->next = first.me;
            first.me->prev = pos.me->prev;

            pos.me->prev = true_last;
            true_last->next = pos.me;
        }
    };
}
