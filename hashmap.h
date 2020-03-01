#pragma once
#include <initializer_list>
#include <list>
#include <vector>
#include <iterator>
#include <stdexcept>
#include <utility>

template<class KeyType, class ValueType, class Hash = std::hash<KeyType> >
class HashMap {
 public:
    using iterator = typename std::list<
        std::pair<const KeyType, ValueType> >::iterator;
    using const_iterator = typename std::list<
        std::pair<const KeyType, ValueType> >::const_iterator;
    using elem_pair = std::pair<const KeyType, ValueType>;
    using ConstKeyValuePair = std::pair<const KeyType, ValueType>;
    using KeyValuePair = std::pair<KeyType, ValueType>;

    iterator begin() {
        return elements_.begin();
    }

    iterator end() {
        return elements_.end();
    }

    const_iterator begin() const {
        return elements_.cbegin();
    }

    const_iterator end() const {
        return elements_.cend();
    }

    HashMap(Hash new_hash = Hash());

    template <typename iter_temp>
    HashMap(iter_temp start, iter_temp finish, Hash new_hash = Hash());

    HashMap(std::initializer_list<KeyValuePair>
             to_hashmap, Hash new_hash = Hash());

    void insert(KeyValuePair new_elem);

    Hash hash_function() const {
        return hasher_;
    }

    iterator find(KeyType key);

    const_iterator find(KeyType key) const;

    int size() const {
        return num_elements_;
    }

    bool empty() const {
        return (num_elements_ == 0);
    }

    void erase(KeyType key);

    ValueType& operator[](const KeyType key);

    const ValueType& at(const KeyType key) const;

    void clear();

    HashMap& operator=(const HashMap& another_hashmap);

 private:
    std::list<ConstKeyValuePair> elements_;

    std::vector< std::list< typename std::list<
     ConstKeyValuePair>::iterator > > hash_table_;

    int modulo_ = 8;

    int num_elements_ = 0;

    Hash hasher_;

    void rebuild(int new_modulo);

    void DoubleSize();

    void HalveSize();

    typename std::list<ConstKeyValuePair>::iterator
        find_key(size_t bucket, KeyType key);

    typename std::list<ConstKeyValuePair>::const_iterator
        find_key_const(size_t bucket, KeyType key) const;
};

template<class KeyType, class ValueType, class Hash>
HashMap<KeyType, ValueType, Hash>::HashMap(Hash new_hash) : hasher_(new_hash) {
    hash_table_.resize(modulo_);
}

template<class KeyType, class ValueType, class Hash>
template <typename iter_temp>
HashMap<KeyType, ValueType, Hash>::HashMap(
    iter_temp start, iter_temp finish, Hash new_hash)
     : hasher_(new_hash) {
    hash_table_.resize(modulo_);
    for (auto it = start; it != finish; it++) {
        insert(*it);
    }
}

template<class KeyType, class ValueType, class Hash>
HashMap<KeyType, ValueType, Hash>::HashMap(
        std::initializer_list<std::pair<KeyType, ValueType> >
             to_hashmap, Hash new_hash) : hasher_(new_hash) {
    hash_table_.resize(modulo_);
    for (auto it : to_hashmap) {
        insert(it);
    }
}

template<class KeyType, class ValueType, class Hash>
void HashMap<KeyType, ValueType, Hash>::insert(
    std::pair<KeyType, ValueType> new_elem) {
        size_t bucket = hasher_(new_elem.first) % modulo_;
        typename std::list< std::pair<const KeyType, ValueType> >::iterator
            check = find_key(bucket, new_elem.first);
        if (check != elements_.end())
            return;
        num_elements_++;
        elements_.push_back(new_elem);
        hash_table_[bucket].push_back(std::prev(elements_.end()));
        if (modulo_ == num_elements_) {
            DoubleSize();
        }
    }

template<class KeyType, class ValueType, class Hash>
typename std::list< std::pair<const KeyType, ValueType> >::iterator
HashMap<KeyType, ValueType, Hash>::find(KeyType key) {
    size_t bucket = hasher_(key) % modulo_;
    typename std::list< std::pair<const KeyType, ValueType> >::iterator
        check = find_key(bucket, key);
    if (check != elements_.end())
        return check;
    return elements_.end();
}

template<class KeyType, class ValueType, class Hash>
typename std::list<std::pair<const KeyType, ValueType> >::const_iterator
HashMap<KeyType, ValueType, Hash>::find(KeyType key) const {
    size_t bucket = hasher_(key) % modulo_;
    typename std::list< std::pair<const KeyType, ValueType> >::const_iterator
        check = find_key_const(bucket, key);
    if (check != elements_.end())
        return check;
    return elements_.cend();
}

template<class KeyType, class ValueType, class Hash>
void HashMap<KeyType, ValueType, Hash>::erase(KeyType key) {
    size_t bucket = hasher_(key) % modulo_;
    for (auto it = hash_table_[bucket].begin();
        it != hash_table_[bucket].end(); it++) {
            if ((*it)->first == key) {
                elements_.erase((*it));
                hash_table_[bucket].erase(it);
                num_elements_--;
                if (num_elements_ < modulo_ / 2) {
                    HalveSize();
                }
                return;
            }
        }
}

template<class KeyType, class ValueType, class Hash>
void HashMap<KeyType, ValueType, Hash>::clear() {
    hash_table_.clear();
    elements_.clear();
    num_elements_ = 0;
    modulo_ = 8;
    hash_table_.resize(modulo_);
}

template<class KeyType, class ValueType, class Hash>
ValueType& HashMap<KeyType, ValueType, Hash>::operator[](const KeyType key) {
    size_t bucket = hasher_(key) % modulo_;
    typename std::list< std::pair<const KeyType, ValueType> >::iterator
        check = find_key(bucket, key);
    if (check != elements_.end())
        return (check->second);
    ValueType v = {};
    insert(std::pair<KeyType, ValueType>(key, v));
    return ((find(key))->second);
}

template<class KeyType, class ValueType, class Hash>
const ValueType& HashMap<KeyType, ValueType, Hash>::at(const KeyType key)
    const {
    size_t bucket = hasher_(key) % modulo_;
    typename std::list< std::pair<const KeyType, ValueType> >::const_iterator
        check = find_key_const(bucket, key);
    if (check != elements_.end())
        return (check->second);
    throw std::out_of_range("");
}

template<class KeyType, class ValueType, class Hash>
HashMap<KeyType, ValueType, Hash>& HashMap<KeyType, ValueType, Hash>::operator=(
    const HashMap<KeyType, ValueType, Hash>& another_hashmap) {
    if (this == &another_hashmap) {
        return *this;
    }
    clear();
    for (auto it : another_hashmap) {
        insert(it);
    }
    return *this;
}

template<class KeyType, class ValueType, class Hash>
void HashMap<KeyType, ValueType, Hash>::rebuild(int new_modulo) {
    modulo_ = new_modulo;
    hash_table_.clear();
    hash_table_.resize(modulo_);
    for (auto it = elements_.begin(); it != elements_.end(); it++) {
        size_t bucket = hasher_(it->first) % modulo_;
        hash_table_[bucket].push_back(it);
    }
}

template<class KeyType, class ValueType, class Hash>
void HashMap<KeyType, ValueType, Hash>::DoubleSize() {
    rebuild(modulo_ * 2);
}

template<class KeyType, class ValueType, class Hash>
void HashMap<KeyType, ValueType, Hash>::HalveSize() {
    rebuild(modulo_ / 2);
}

template<class KeyType, class ValueType, class Hash>
typename std::list< std::pair<const KeyType, ValueType> >::iterator
HashMap<KeyType, ValueType, Hash>::find_key(size_t bucket, KeyType key) {
    for (auto it : hash_table_[bucket])
        if (it->first == key)
            return it;
    return elements_.end();
}

template<class KeyType, class ValueType, class Hash>
typename std::list< std::pair<const KeyType, ValueType> >::const_iterator
HashMap<KeyType, ValueType, Hash>::find_key_const(
    size_t bucket, KeyType key) const {
    for (auto it : hash_table_[bucket])
        if (it->first == key)
            return it;
    return elements_.end();
}
