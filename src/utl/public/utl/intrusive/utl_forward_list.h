// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/exception.h"
#include "utl/functional/utl_equal_to.h"
#include "utl/functional/utl_forward.h"
#include "utl/functional/utl_less.h"
#include "utl/intrusive/utl_node_ptr.h"
#include "utl/intrusive/utl_nodes.h"
#include "utl/type_traits/utl_declval.h"
#include "utl/utility/utl_exchange.h"
#include "utl/utility/utl_forward.h"

#ifdef UTL_CXX20
UTL_NAMESPACE_BEGIN
namespace intrusive {
template <typename NodeType, typename Policy>
class forward_list;

namespace details {
class forward_list {
    template <typename NodeType, typename Policy>
    friend class UTL_SCOPE intrusive::forward_list;

    template <typename NodeType>
    class base {
    private:
        using reference = node_type&;
        using const_reference = node_type const&;
        using pointer = node_type*;
        using const_pointer = node_type const*;

    public:
        base(base const&) = delete;
        base& operator=(base const&) = delete;
        // delete because we cannot clear the pointer from here
        base& operator=(base&&) noexcept = delete;

        UTL_ATTRIBUTE(NODISCARD)
        constexpr bool empty() const { return head_ == nullptr; }
        UTL_ATTRIBUTE(NODISCARD) UTL_CONSTEXPR_CXX14 reference front() noexcept { return *head_; }
        UTL_ATTRIBUTE(NODISCARD)
        constexpr const_reference front() const noexcept { return *head_; }

        constexpr base() noexcept = default;
        UTL_CONSTEXPR_CXX20 ~base() noexcept = default;
        constexpr base(pointer head) noexcept : head_(head) {}
        UTL_CONSTEXPR_CXX14 base(base&& other) noexcept : head_(other.exchange_head(nullptr)) {}

        constexpr pointer get_head() const noexcept { return head_; }

        UTL_CONSTEXPR_CXX14
        pointer exchange_head(pointer node) noexcept { return exchange(head_, node); }

        UTL_CONSTEXPR_CXX14
        pointer set_head(pointer node) noexcept { return head_ = node; }

        UTL_CONSTEXPR_CXX14
        reference push_front(pointer node) noexcept UTL_ATTRIBUTE(LIFETIMEBOUND) {
            UTL_ASSERT(node && unlinked(*node));
            set_next(*node, head_);
            head_ = node;
            return *node;
        }

        UTL_CONSTEXPR_CXX14
        pointer pop_front() noexcept {
            node_type* output = head_;
            if (output != nullptr) {
                head_ = get_next(*output);
                set_next(*output, nullptr);
            }

            return output;
        }

        UTL_CONSTEXPR_CXX14
        void swap(base& other) noexcept { head_ = exchange(other.head_, head_); }

        static pointer reverse(pointer head) noexcept {
            if (head == nullptr) {
                return head;
            }

            pointer tail = get_next(*head);
            if (tail == nullptr) {
                return head;
            }

            for (pointer next = get_next(*tail); next != nullptr; next = get_next(*tail)) {
                pointer keep = get_next(*head);
                set_next(*head, next);
                set_next(*tail, get_next(*next));
                set_next(*get_next(*head), keep);
            }
        }

    private:
        pointer head_ = nullptr;
    };

    template <typename NodeType>
    class const_iterator;
    template <typename NodeType>
    class iterator;

    template <typename NodeType, typename It, bool Const>
    class iterator_base {
        using dereferenced_type = conditional_t<Const, NodeType const, NodeType>;
        using iterator_type = It;

    public:
        using value_type = NodeType;
        using reference = dereferenced_type&;
        using pointer = dereferenced_type*;
        using difference_type = decltype((char*)0 - (char*)0);

        UTL_ATTRIBUTE(NODISCARD)
        constexpr reference operator*() const noexcept {
            UTL_ASSERT_CXX14(list_ == nullptr);
            return *node_;
        }
        UTL_ATTRIBUTE(NODISCARD)
        constexpr pointer operator->() const noexcept {
            UTL_ASSERT_CXX14(list_ == nullptr);
            return *node_;
        }

        constexpr iterator_base() noexcept : node_(nullptr), list_(nullptr) {}
        UTL_CONSTEXPR_CXX20 ~iterator_base() noexcept = default;

        constexpr iterator_base(value_type* node) noexcept : node_(node), list_(nullptr) {}
        constexpr iterator_base(base<NodeType>* b) noexcept : node_(nullptr), list_(b) {}

        UTL_ATTRIBUTE(NODISCARD) constexpr pointer node() const noexcept { return node_; }
        UTL_ATTRIBUTE(NODISCARD) constexpr pointer next() const noexcept {
            return list_ ? list_.get_head() : get_next(*node_);
        }
        UTL_CONSTEXPR_CXX14 void set_next(pointer ptr) noexcept {
            if (list_) {
                list_.set_head(ptr);
            } else {
                set_next(node_, ptr);
            }
        }

        UTL_CONSTEXPR_CXX14 It transfer_after(iterator_base before, iterator_base end) noexcept {
            auto keep = before.next();
            if (end.node() != nullptr) {
                before.set_next(end.next());
                end.set_next(next());
            } else {
                before.set_next(nullptr);
            }
            set_next(keep);
            return It(end.node());
        }

    private:
        value_type* node_;
        base<NodeType>* list_;

        friend UTL_CONSTEXPR_CXX14 iterator_type& operator++(iterator_type& it) noexcept {
            if (it.list_ != nullptr) {
                it.node_ = exchange(it.list_, nullptr)->get_head();
            } else {
                UTL_ASSERT(it.node_ != nullptr);
                it.node_ = get_next(*it.node_);
            }
            return it;
        }

        friend UTL_CONSTEXPR_CXX14 iterator_type operator++(iterator_type& it, int) noexcept {
            auto tmp = it;
            ++it;
            return tmp;
        }

        friend constexpr bool operator==(iterator_type const& l, iterator_type const& r) noexcept {
            return l.list_ == r.list_ && l.node_ == r.node_;
        }

        friend constexpr bool operator!=(iterator_type const& l, iterator_type const& r) noexcept {
            return !(l == r);
        }
    };

    template <typename NodeType>
    class iterator : private iterator_base<NodeType, iterator<NodeType>, false> {
        using base_type = iterator_base<NodeType>;

    public:
        constexpr iterator() noexcept = default;
        using base_type::operator->();
        using base_type::operator*();

    private:
        using base_type::base_type;

        friend UTL_SCOPE forward_list<NodeType>;

        friend constexpr bool operator==(iterator_type const& l, const_iterator const& r) noexcept;

        friend constexpr bool operator!=(iterator_type const& l, const_iterator const& r) noexcept;

        friend constexpr bool operator==(const_iterator const& l, iterator_type const& r) noexcept;

        friend constexpr bool operator!=(const_iterator const& l, iterator_type const& r) noexcept;
    };

    template <typename NodeType>
    class const_iterator : private iterator_base<NodeType, const_iterator<NodeType>, true> {
    public:
        using iterator_type = iterator<TBase>;

        constexpr const_iterator() noexcept = default;
        using base_type::operator->();
        using base_type::operator*();

    private:
        using base_type::base_type;

        friend UTL_SCOPE forward_list<NodeType>;

        iterator<NodeType> unconst() const noexcept { return iterator<NodeType>(*this); }

        friend constexpr bool operator==(iterator_type const& l, const_iterator const& r) noexcept {
            return l == r.unconst();
        }

        friend constexpr bool operator!=(iterator_type const& l, const_iterator const& r) noexcept {
            return !(l == r);
        }

        friend constexpr bool operator==(const_iterator const& l, iterator_type const& r) noexcept {
            return r == l;
        }

        friend constexpr bool operator!=(const_iterator const& l, iterator_type const& r) noexcept {
            return !(l == r);
        }
    };

    template <typename NodeType, typename Policy>
    class impl : protected base<NodeType> {
    public:
        using node_type = NodeType;
        using node_policy = Policy;
        using policy_traits = node_policy_traits<node_policy>;
        using fancy_pointer = typename policy_traits::fancy_pointer;
        using node_deleter = typename policy_traits::deleter_type;
        using reference = typename policy_traits::reference;
        using const_reference = typename policy_traits::const_reference;
        using pointer = typename policy_traits::pointer;
        using const_pointer = typename policy_traits::const_pointer;
        using size_type = size_t;

    private:
        using base_type = base<NodeType>;
        class invalid_t {
            invalid_t() = default;
            ~invalid_t() = default;
        };
        template <typename Tag>
        class tag_t {};
        using copy_ctor_t = conditional_t<policy_traits::is_node_clonable::value, impl, invalid_t>;
        using copy_assign_t =
            conditional_t<policy_traits::is_node_clonable::value, impl, invalid_t>;

        /* Deliberately undefined, because it can't be used */
        impl(tag_t<invalid_t>, invalid_t const&) noexcept;
        UTL_CONSTEXPR_CXX14 impl& assign(tag_t<invalid_t>, invalid_t const&) noexcept {
            return *this;
        }

        static pointer clone_all(policy_type& policy, pointer src) {
            if (src == = nullptr) {
                return src;
            }

            UTL_TRY {
                auto head = policy_traits::clone(policy, src);
                UTL_ASSERT(head != nullptr);
                for (pointer prev_dst = head, src_node = get_next(src); src_node != nullptr;
                     src_node = get_next(*src_node), prev_dst = get_next(*prev_dst)) {
                    set_next(*prev_dst, policy_traits::clone(policy_, *src_node));
                }
                return head;
            }
            UTL_CATCH(...) {
                head = base_type::reverse(head);
                while (head != nullptr) {
                    pointer tmp = head;
                    head = get_next(*head);
                    policy_traits::release(policy_, tmp);
                }

                UTL_RETHROW;
            }
        }

        template <typename T = impl, enable_if<is_same<T, impl>::value, bool> = true>
        impl(tag_t<T>, T const& other,
            policy_type const& p = policy_traits::select_on_container_copy_construction(
                other.policy_))
            : base_type()
            , policy_(p) {
            head_ = clone_all(policy_, other.head_);
        }

        template <typename T = impl, enable_if<is_same<T, impl>::value, bool> = true>
        impl& assign(tag_t<T>, T const& other) {
            if (&other != this) {
                clear();
                policy_traits::assign(policy_, other.policy_);
                head_ = clone_all(policy_, other.head_);
            }

            return *this;
        }

    public:
        constexpr impl() noexcept = default;
        UTL_CONSTEXPR_CXX20 ~impl() noexcept { clear(); }
        constexpr impl(policy_type const& p) noexcept : base_type(), policy_(p) {}
        // TODO: ignore "-Winvalid-constexpr" for the following lines
        constexpr impl(copy_ctor_t const& other, policy_type const& p)
            : impl(tag_t<copy_ctor_t>{}, other, p) {}
        constexpr impl(copy_ctor_t const& other) : impl(tag_t<copy_ctor_t>{}, other) {}

        constexpr impl(impl&& other) noexcept = default;
        constexpr impl(impl&& other, policy_type const& p) noexcept(
            policy_traits::is_always_equal::value)
            : policy_(p) {
            if (other.empty()) {
                return;
            }

            if (!policy_traits::equals(policy_, other.get_policy())) {
                head_ = clone_all(policy_, other.head_);
                other.clear();
            } else {
                head_ = other.exchange_head(nullptr);
            }
        }

        UTL_CONSTEXPR_CXX14 impl& operator=(copy_assign_t const& other) noexcept {
            static constexpr tag_t<copy_assign_t> tag = {};
            return assign(tag, other);
        }

        UTL_CONSTEXPR_CXX14 impl& operator=(move_assign_t&& other) noexcept {
            if (this != addressof(other)) {
                clear();
                other.swap(static_cast<base_type&>(*this));
                policy_traits::assign(policy_, move(other.policy_));
            }

            return *this;
        }

        UTL_CONSTEXPR_CXX14 void clear() noexcept {
            node_type* current_node = this->exchange_head(nullptr);
            while (current_node != nullptr) {
                node_type* next_node = get_next(*current_node);
                set_next(*current_node, nullptr);
                policy_traits::release(policy_, current_node);
                current_node = next_node;
            }
        }

        UTL_CONSTEXPR_CXX14 void swap() noexcept {
            set_head(other.exchange_head(get_head()));
            policy_traits::swap(policy_, other.policy_);
        }

        policy_type policy_;
    };
} // namespace forward_list
} // namespace details

template <unidirectional NodeType, release_invocable<NodeType> Policy>
class forward_list<NodeType, Policy> : private details::forward_list::impl<NodeType, Policy> {
    using base_type = details::forward_list::base<NodeType>;
    using impl_type = details::forward_list::impl<NodeType, Policy>;

public:
    using impl_type::const_pointer;
    using impl_type::const_reference;
    using impl_type::deleter_type;
    using impl_type::fancy_pointer;
    using impl_type::node_policy;
    using impl_type::node_type;
    using impl_type::pointer;
    using impl_type::policy_traits;
    using impl_type::reference;
    using impl_type::size_type;

    using const_iterator = details::forward_list::const_iterator<NodeType>;
    using iterator = details::forward_list::iterator<NodeType>;

    constexpr forward_list() noexcept(
        is_nothrow_default_constructible<node_policy>::value) = default;
    constexpr explicit forward_list(node_policy const& policy) noexcept : impl_type(policy) {}

    constexpr explicit forward_list(fancy_pointer head) noexcept(
        noexcept(impl_type(head.release(), head.get_policy())))
        : impl_type(head.release(), head.get_policy()) {}

    constexpr explicit forward_list(pointer head,
        node_policy const& policy = node_policy()) noexcept(noexcept(impl_type(head, policy)))
        : impl_type(head, policy) {}

    constexpr forward_list(forward_list&&) noexcept = default;
    UTL_CONSTEXPR_CXX14 forward_list& operator=(forward_list&&) noexcept = default;
    forward_list(forward_list const&) = default;
    forward_list& operator=(forward_list const&) = default;

    template <typename T = forward_list, enable_if_t<is_same<T, forward_list>::value, bool> = true>
    constexpr forward_list(T&& other, node_policy const& policy) noexcept(
        policy_traits::is_always_equal::value)
        : impl_type(move(other), policy) {}

    template <typename T = forward_list,
        enable_if_t<is_same<T, forward_list>::value && is_copy_constructible<forward_list>::value,
            bool> = true>
    forward_list(T const& other, node_policy const& policy) : impl_type(other, policy) {}

    ~forward_list() = default;

    /**
     * Pops a non-copyable unique pointer off the head of the list
     *
     * @return A non-copyable smart pointer to the node
     */
    UTL_CONSTEXPR_CXX14 fancy_pointer pop_front() noexcept {
        return fancy_pointer(base_type::pop_front(), node_deleter(policy_));
    }

    template <typename... Args>
    requires acquire_invocable<node_policy, node_type, Args...>
    UTL_CONSTEXPR_CXX14 reference emplace_front(Args&&... args) noexcept(
        noexcept(node_policy::acquire(declval<Args>()...))) UTL_ATTRIBUTE(LIFETIMEBOUND) {
        return base_type::push_front(policy_traits::acquire(this->policy_, forward<Args>(args)...));
    }

    UTL_CONSTEXPR_CXX14 reference push_front(fancy_pointer node) noexcept
        UTL_ATTRIBUTE(LIFETIMEBOUND) {
        return emplace_front(node.release());
    }

    template <typename... Args>
    requires acquire_invocable<node_policy, node_type, Args...>
    iterator emplace_after(const_iterator pos, Args&&... args) noexcept(noexcept(
        policy_traits::acquire(this->policy_, declval<Args>()...))) UTL_ATTRIBUTE(LIFETIMEBOUND) {
        pointer node = policy_traits::acquire(this->policy_, forward<Args>(args)...);
        UTL_ASSERT(node != nullptr);
        UTL_ASSERT(pos != end());

        set_next(*node, pos.next());
        pos.set_next(node);
        return iterator(node);
    }

    iterator insert_after(const_iterator pos, fancy_pointer node) noexcept(noexcept(
        policy_traits::acquire(this->policy_, node.release()))) UTL_ATTRIBUTE(LIFETIMEBOUND) {
        UTL_ASSERT(node != nullptr);
        UTL_ASSERT(pos != end());

        set_next(*node, pos.next());
        pos.set_next(node.release());
        return iterator(get_next(to));
    }

    iterator erase_after(const_iterator pos) noexcept UTL_ATTRIBUTE(LIFETIMEBOUND) {
        UTL_ASSERT(pos != end());

        auto to_erase = pos.next();
        auto out_node = get_next(*to_erase);
        pos.set_next(out_node);
        policy_traits::release(this->policy_, to_erase);

        return iterator(out_node);
    }

    iterator erase_after(const_iterator pos, const_iterator last_it) noexcept
        UTL_ATTRIBUTE(LIFETIMEBOUND) {
        UTL_ASSERT(pos != end());

        pointer const last = last_it.unconst().node();
        pointer curr = pos.next();
        while (curr != last) {
            pointer tmp = curr;
            curr = get_next(*curr);
            policy_traits::release(this->policy_, tmp);
        }

        pos.set_next(last);
        return iterator(last);
    }

    void swap(forward_list& other) noexcept { impl_type::swap(other); }

    friend void swap(forward_list& left, forward_list& right) noexcept { left.swap(right); }

    using base_type::before_begin;
    using base_type::begin;
    using base_type::cbegin;
    using base_type::cend;
    using base_type::empty;
    using base_type::end;
    using base_type::front;
    using impl_type::clear;

    UTL_ATTRIBUTE(NODISCARD)
    UTL_CONSTEXPR_CXX14 policy_type& get_policy() noexcept { return this->policy_; }

    UTL_ATTRIBUTE(NODISCARD)
    constexpr policy_type const& get_policy() const noexcept { return this->policy_; }

    UTL_ATTRIBUTE(NODISCARD)
    UTL_CONSTEXPR_CXX14 iterator before_begin() noexcept { return iterator(this); }
    UTL_ATTRIBUTE(NODISCARD)
    constexpr const_iterator before_cbegin() const noexcept { return const_iterator(this); }
    UTL_ATTRIBUTE(NODISCARD)
    constexpr const_iterator before_begin() const noexcept { return before_cbegin(); }

    UTL_ATTRIBUTE(NODISCARD)
    UTL_CONSTEXPR_CXX14 iterator begin() noexcept { return iterator(head_); }
    UTL_ATTRIBUTE(NODISCARD)
    constexpr const_iterator cbegin() const noexcept { return const_iterator(head_); }
    UTL_ATTRIBUTE(NODISCARD)
    constexpr const_iterator begin() const noexcept { return cbegin(); }

    UTL_ATTRIBUTE(NODISCARD)
    UTL_CONSTEXPR_CXX14 iterator end() noexcept { return iterator(); }
    UTL_ATTRIBUTE(NODISCARD)
    constexpr const_iterator cend() const noexcept { return const_iterator(); }
    UTL_ATTRIBUTE(NODISCARD)
    constexpr const_iterator end() const noexcept { return cend(); }

    // TODO suppress instantiation
    void merge(forward_list& other) { merge(other, UTL_SCOPE less<T>{}); }

    void merge(forward_list&& other) { merge(other); }

    template <typename Comparer>
    void merge(forward_list& other, Comparer comp) {
        if (this == addressof(other)) {
            return;
        }

        UTL_ASSERT(policy_traits::equals(other.policy_, this->policy_));

        iterator node = before_begin();
        while (node.next() && other.begin().node()) {
            if (comp(*other.begin().node(), *node.next())) {
                node.transfer_after(other.before_begin(), other.begin());
            }

            ++node;
        }

        if (!other.empty()) {
            node.set_next(other.exchange_head(nullptr));
        }
    }

    template <typename Comparer>
    void merge(forward_list&& other, Comparer comp) {
        merge(other, move(comp));
    }

    void splice_after(const_iterator pos, forward_list& other) { splice_after(pos, move(other)); }

    void splice_after(const_iterator pos, forward_list& other, const_iterator it) {
        splice_after(pos, move(other), it);
    }

    void splice_after(
        const_iterator pos, forward_list& other, const_iterator first, const_iterator last) {
        splice_after(pos, move(other), first, last);
    }

    void splice_after(const_iterator pos, forward_list&& other) {
        splice_after(pos, other, other.before_begin(), other.end());
    }

    void splice_after(const_iterator pos, forward_list&& other, const_iterator it) noexcept {
        UTL_ASSERT(pos != this->end());
        iterator jt = it.unconst();
        ++jt;
        if (pos == it || pos == jt) {
            return;
        }

        pos.transfer_after(it, jt);
    }

    void splice_after(const_iterator pos, forward_list&& other, const_iterator before,
        const_iterator last) noexcept {
        splice_after_impl(pos, move(other), before, last);
    }

    template <typename UnaryPredicate>
    size_type remove_if(UnaryPredicate pred) {
        size_type removed = 0;
        forward_list to_destroy(get_policy());

        const_iterator prev_it = before_cbegin();
        for (auto node = prev_it.next(); node != nullptr; node = prev_it.next()) {
            if (pred(*node)) {
                to_destroy.splice_after(to_destroy.before_cbegin(), *this, prev_it);
                ++removed;
            } else {
                ++prev_it;
            }
        }

        return removed;
    }

    void reverse() noexcept { base_type::set_head(base_type::reverse(base_type::get_head())); }

    size_type unique() { return unique(UTL_SCOPE equal_to<NodeType>{}); }

    template <typename BinaryPredicate>
    size_type unique(BinaryPredicate pred) {
        iterator first = begin();
        iterator last = end();
        if (first == last) {
            return 0;
        }

        size_type removed = 0;
        forward_list to_destroy(get_policy());
        iterator next = first;
        while (++next != last) {
            if (pred(*first, *next)) {
                to_destroy.splice_after(to_destroy.cbefore_begin(), *this, first);
                ++removed;
            } else {
                first = next;
            }
            next = first;
        }

        return removed;
    }

    void sort() { return sort(UTL_SCOPE less<NodeType>{}); }

    template <typename BinaryPredicate>
    void sort(BinaryPredicate pred) {
        pointer list = base_type::get_head();
        if (list == nullptr) {
            return;
        }

        size_type insize = 1;
        while (1) {
            pointer p = exchange(list, nullptr);
            pointer tail = nullptr;
            size_type merges = 0;
            while (p != nullptr) {
                ++merges;
                pointer q = p;
                size_type psize = 0;
                for (size_type i = 0; i < insize; ++i) {
                    ++psize;
                    q = get_next(*q);
                    if (q == nullptr) {
                        break;
                    }
                }

                size_type qsize = insize;
                while (psize > 0 || (qsize > 0 && q != nullptr)) {
                    pointer e;
                    if (psize == 0) {
                        e = q;
                        q = get_next(*q);
                        --qsize;
                    } else if (qsize == 0 || q == nullptr) {
                        e = p;
                        p = get_next(*p);
                        --psize;
                    } else if (!pred(*q, *p)) {
                        e = p;
                        p = get_next(*p);
                        --psize;
                    } else {
                        e = q;
                        q = get_next(*q);
                        --qsize;
                    }

                    if (tail != nullptr) {
                        set_next(*tail, e);
                    } else {
                        list = e;
                    }
                    tail = e;
                }
                p = q;
            }
            set_next(*tail, nullptr);

            if (merges <= 1) {
                base_type::set_head(list);
                return;
            }

            insize *= 2;
        }
    }

private:
    iterator splice_after_impl(const_iterator pos, forward_list&& other, const_iterator before,
        const_iterator last) noexcept {
        if (other.empty() || before == last) {
            return pos.unconst();
        }
        UTL_ASSERT(pos != this->end());
        UTL_ASSERT(before != other.end());

        auto tmp = pos;
        auto b = before;
        auto end = b;
        while (end.node() && end.next() != last.node()) {
            ++end;
        }

        if (b != end) {
            return tmp.transfer_after(b, end).unconst();
        }

        return iterator(tmp.node());
    }
};
} // namespace intrusive
UTL_NAMESPACE_END
#endif