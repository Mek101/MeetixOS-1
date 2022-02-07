/**
 * @brief
 * This file is part of the MeetiX Operating System.
 * Copyright (c) 2017-2021, Marco Cicognani (marco.cicognani@meetixos.org)
 *
 * @developers
 * Marco Cicognani (marco.cicognani@meetixos.org)
 *
 * @license
 * GNU General Public License version 3
 */

#pragma once

#include <initializer_list>
#include <TC/Assertion.hh>
#include <TC/Cxx/Exchange.hh>
#include <TC/Cxx/Move.hh>
#include <TC/DenyCopy.hh>
#include <TC/Functional/Option.hh>
#include <TC/IntTypes.hh>

namespace TC::Collection {
namespace Details {

template<typename Collection, typename T>
class ListIterator {
public:
    explicit ListIterator(Collection& collection);
    ListIterator(Collection& collection, typename Collection::Node* node);
    ListIterator(ListIterator const&) = default;

    ListIterator& operator=(ListIterator const&) = default;

    /**
     * @brief Increment operators
     */
    ListIterator& operator++();
    ListIterator  operator++(int);

    /**
     * @brief ValueReference access operators
     */
    T&       operator*();
    T const& operator*() const;

    /**
     * @brief Pointer access operators
     */
    T*       operator->();
    T const* operator->() const;

    /**
     * @brief Removes the node from the list
     */
    ListIterator erase();

    /**
     * @brief Getters
     */
    [[nodiscard]] bool is_end() const;

    /**
     * @brief Comparison operators
     */
    [[nodiscard]] bool operator==(ListIterator const& rhs) const;
    [[nodiscard]] bool operator!=(ListIterator const& rhs) const;
    [[nodiscard]] bool operator<=>(ListIterator const& rhs) const = default;

private:
    friend Collection;

    void delete_node();

private:
    Collection*                m_collection{ nullptr };
    typename Collection::Node* m_current_node{ nullptr };
};

template<typename T>
struct ListNode {
    explicit ListNode(T&& value);

    T         m_value;
    ListNode* m_next_node{ nullptr };
    ListNode* m_prev_node{ nullptr };
};

} /* namespace Details */

template<typename T>
class List {
    TC_DENY_COPY(List);

public:
    using Iterator      = Details::ListIterator<List, T>;
    using ConstIterator = Details::ListIterator<List const, T const>;
    using Node          = Details::ListNode<T>;

public:
    /**
     * @brief Constructors
     */
    List() = default;
    List(List&& rhs) noexcept;
    List(std::initializer_list<T> initializer_list);

    ~List();

    List& operator=(List&& rhs) noexcept;
    List& operator=(std::initializer_list<T> initializer_list);

    void clear();

    /**
     * @brief Creates a new node and appends it to the last of the list
     */
    void append(T const& value);
    void append(T&& value);

    /**
     * @brief Creates a new node and prepends it to the first of the list
     */
    void prepend(T const& value);
    void prepend(T&& value);

    /**
     * @brief Removes the node referenced by the given iterator
     */
    void erase(Iterator& iterator);

    template<typename Callback>
    void erase_if(Callback callback);

    [[nodiscard]] bool contains(T const& value) const;

    Functional::Option<T&>       find(T const& value);
    Functional::Option<T const&> find(T const& value) const;

    template<typename Callback>
    Functional::Option<T&> find_if(Callback callback);
    template<typename Callback>
    Functional::Option<T const&> find_if(Callback callback) const;

    /**
     * @brief for-each support
     */
    Iterator begin();
    Iterator end();

    ConstIterator begin() const;
    ConstIterator end() const;

    /**
     * @brief Getters
     */
    [[nodiscard]] usize count() const;
    [[nodiscard]] bool  is_empty() const;

    [[nodiscard]] Node*       head();
    [[nodiscard]] Node const* head() const;

    [[nodiscard]] Node*       tail();
    [[nodiscard]] Node const* tail() const;

    [[nodiscard]] T&       first();
    [[nodiscard]] T const& first() const;

    [[nodiscard]] T&       last();
    [[nodiscard]] T const& last() const;

private:
    Node* m_head_node{ nullptr };
    Node* m_tail_node{ nullptr };
    usize m_values_count{ 0 };
};

template<typename T>
List<T>::List(List&& rhs) noexcept
    : m_head_node{ Cxx::exchange(rhs.m_head_node, nullptr) }
    , m_tail_node{ Cxx::exchange(rhs.m_tail_node, nullptr) }
    , m_values_count{ Cxx::exchange(rhs.m_values_count, 0) } {
}

template<typename T>
List<T>::List(std::initializer_list<T> initializer_list) {
    for ( auto const& value : initializer_list )
        append(value);
}

template<typename T>
List<T>::~List() {
    clear();
}

template<typename T>
List<T>& List<T>::operator=(List&& rhs) noexcept {
    if ( this == &rhs )
        return *this;

    clear();
    m_head_node    = Cxx::exchange(rhs.m_head_node, nullptr);
    m_tail_node    = Cxx::exchange(rhs.m_tail_node, nullptr);
    m_values_count = Cxx::exchange(rhs.m_values_count, 0);
    return *this;
}

template<typename T>
List<T>& List<T>::operator=(std::initializer_list<T> initializer_list) {
    clear();
    for ( auto const& value : initializer_list )
        append(value);
    return *this;
}

template<typename T>
void List<T>::clear() {
    for ( auto* node = m_head_node; node != nullptr; ) {
        auto* next_node = node->m_next_node;
        delete node;
        node = next_node;
    }
    m_head_node    = nullptr;
    m_tail_node    = nullptr;
    m_values_count = 0;
}

template<typename T>
void List<T>::append(T const& value) {
    append(T{ value });
}

template<typename T>
void List<T>::append(T&& value) {
    auto* new_node = new Node{ Cxx::move(value) };

    if ( m_tail_node == nullptr )
        m_head_node = new_node;
    else {
        m_tail_node->m_next_node = new_node;
        new_node->m_prev_node    = m_tail_node;
    }

    m_tail_node = new_node;
    ++m_values_count;
}

template<typename T>
void List<T>::prepend(T const& value) {
    prepend(T{ value });
}

template<typename T>
void List<T>::prepend(T&& value) {
    auto* new_node = new Node{ Cxx::move(value) };

    if ( m_head_node == nullptr )
        m_tail_node = new_node;
    else {
        m_head_node->m_prev_node = new_node;
        new_node->m_next_node    = m_head_node;
    }

    m_head_node = new_node;
    ++m_values_count;
}

template<typename T>
void List<T>::erase(Iterator& iterator) {
    VERIFY(!iterator.is_end());

    auto* node_to_erase = iterator.m_current_node;
    if ( m_head_node == node_to_erase )
        m_head_node = node_to_erase->m_next_node;
    if ( m_tail_node == node_to_erase )
        m_tail_node = node_to_erase->m_prev_node;
    if ( node_to_erase->m_prev_node != nullptr )
        node_to_erase->m_prev_node->m_next_node = node_to_erase->m_next_node;
    if ( node_to_erase->m_next_node != nullptr )
        node_to_erase->m_next_node->m_prev_node = node_to_erase->m_prev_node;

    iterator.delete_node();
    --m_values_count;
}

template<typename T>
template<typename Callback>
void List<T>::erase_if(Callback callback) {
    auto it = begin();
    while ( it != end() ) {
        if ( callback(*it) )
            it = it.erase();
        else
            ++it;
    }
}

template<typename T>
bool List<T>::contains(T const& value) const {
    return find(value).is_present();
}

template<typename T>
Functional::Option<T&> List<T>::find(T const& value) {
    return find_if([&value](auto const& v) { return value == v; });
}

template<typename T>
Functional::Option<const T&> List<T>::find(T const& value) const {
    return find_if([&value](auto const& v) { return value == v; });
}

template<typename T>
template<typename Callback>
Functional::Option<T&> List<T>::find_if(Callback callback) {
    for ( auto& node : *this ) {
        if ( callback(node) )
            return node;
    }
    return {};
}

template<typename T>
template<typename Callback>
Functional::Option<T const&> List<T>::find_if(Callback callback) const {
    for ( auto const& node : *this ) {
        if ( callback(node) )
            return node;
    }
    return {};
}

template<typename T>
typename List<T>::Iterator List<T>::begin() {
    return Iterator{ *this, m_head_node };
}

template<typename T>
typename List<T>::Iterator List<T>::end() {
    return Iterator{ *this };
}

template<typename T>
typename List<T>::ConstIterator List<T>::begin() const {
    return ConstIterator{ *this, m_head_node };
}

template<typename T>
typename List<T>::ConstIterator List<T>::end() const {
    return ConstIterator{ *this };
}

template<typename T>
usize List<T>::count() const {
    return m_values_count;
}

template<typename T>
bool List<T>::is_empty() const {
    return count() == 0;
}

template<typename T>
typename List<T>::Node* List<T>::head() {
    return m_head_node;
}

template<typename T>
typename List<T>::Node const* List<T>::head() const {
    return m_head_node;
}

template<typename T>
typename List<T>::Node* List<T>::tail() {
    return m_tail_node;
}

template<typename T>
typename List<T>::Node const* List<T>::tail() const {
    return m_tail_node;
}
template<typename T>
T& List<T>::first() {
    VERIFY(m_head_node != nullptr);
    return head()->m_value;
}

template<typename T>
T const& List<T>::first() const {
    VERIFY(m_head_node != nullptr);
    return head()->m_value;
}

template<typename T>
T& List<T>::last() {
    VERIFY(m_tail_node != nullptr);
    return tail()->m_value;
}

template<typename T>
T const& List<T>::last() const {
    VERIFY(m_tail_node != nullptr);
    return tail()->m_value;
}

namespace Details {

template<typename Collection, typename T>
ListIterator<Collection, T>::ListIterator(Collection& collection)
    : m_collection{ &collection } {
}

template<typename Collection, typename T>
ListIterator<Collection, T>::ListIterator(Collection& collection, typename Collection::Node* node)
    : m_collection{ &collection }
    , m_current_node{ node } {
}

template<typename Collection, typename T>
ListIterator<Collection, T>& ListIterator<Collection, T>::operator++() {
    if ( m_current_node != nullptr )
        m_current_node = m_current_node->m_next_node;
    return *this;
}

template<typename Collection, typename T>
ListIterator<Collection, T> ListIterator<Collection, T>::operator++(int) {
    ListIterator it{ *this };

    operator++();
    return it;
}

template<typename Collection, typename T>
T& ListIterator<Collection, T>::operator*() {
    VERIFY(m_current_node != nullptr);
    return m_current_node->m_value;
}

template<typename Collection, typename T>
T const& ListIterator<Collection, T>::operator*() const {
    VERIFY(m_current_node != nullptr);
    return m_current_node->m_value;
}

template<typename Collection, typename T>
T* ListIterator<Collection, T>::operator->() {
    return &operator*();
}

template<typename Collection, typename T>
T const* ListIterator<Collection, T>::operator->() const {
    return &operator*();
}

template<typename Collection, typename T>
ListIterator<Collection, T> ListIterator<Collection, T>::erase() {
    ListIterator it{ *this };
    ++it;

    m_collection->erase(*this);
    return it;
}

template<typename Collection, typename T>
bool ListIterator<Collection, T>::is_end() const {
    return m_current_node == nullptr;
}

template<typename Collection, typename T>
bool ListIterator<Collection, T>::operator==(const ListIterator& rhs) const {
    return m_current_node == rhs.m_current_node;
}

template<typename Collection, typename T>
bool ListIterator<Collection, T>::operator!=(const ListIterator& rhs) const {
    return m_current_node != rhs.m_current_node;
}

template<typename Collection, typename T>
void ListIterator<Collection, T>::delete_node() {
    delete m_current_node;
    m_current_node = nullptr;
}

template<typename T>
ListNode<T>::ListNode(T&& value)
    : m_value{ Cxx::move(value) } {
}

} /* namespace Details */
} /* namespace TC::Collection */