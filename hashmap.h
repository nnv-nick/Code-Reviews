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
    using KeyValuePair = std::pair<const KeyType, ValueType>;
    using iterator = typename std::list<KeyValuePair>::iterator;
    using const_iterator = typename std::list<KeyValuePair>::const_iterator;

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

    iterator find(const KeyType key);

    const_iterator find(const KeyType key) const;

    int size() const {
        return num_elements_;
    }

    bool empty() const {
        return (num_elements_ == 0);
    }

    void erase(const KeyType key);

    ValueType& operator[](const KeyType key);

    const ValueType& at(const KeyType key) const;

    void clear();

    HashMap& operator=(const HashMap& another_hashmap);

 private:
    const int initialModulo_ = 8;

    std::list<KeyValuePair> elements_;

    std::vector< std::list<iterator> > hash_table_;

    int modulo_ = initialModulo_;

    int num_elements_ = 0;

    Hash hasher_;

    void Rebuild(int new_modulo);

    void DoubleSize();

    void HalveSize();

    iterator FindKey(const KeyType key);

    const_iterator FindKeyConst(const KeyType key) const;
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
        std::initializer_list<KeyValuePair> to_hashmap,
            Hash new_hash) : hasher_(new_hash) {
    hash_table_.resize(modulo_);
    for (auto it : to_hashmap) {
        insert(it);
    }
}

template<class KeyType, class ValueType, class Hash>
void HashMap<KeyType, ValueType, Hash>::insert(
        KeyValuePair new_elem) {
    iterator check = FindKey(new_elem.first);
    if (check != elements_.end()) {
        return;
    }
    num_elements_++;
    size_t bucket = hasher_(new_elem.first) % modulo_;
    elements_.push_back(new_elem);
    hash_table_[bucket].push_back(std::prev(elements_.end()));
    if (modulo_ == num_elements_) {
        DoubleSize();
    }
}

template<class KeyType, class ValueType, class Hash>
auto HashMap<KeyType, ValueType, Hash>::find(const KeyType key) -> iterator {
    iterator check = FindKey(key);
    if (check != elements_.end()) {
        return check;
    }
    return elements_.end();
}

template<class KeyType, class ValueType, class Hash>
auto HashMap<KeyType, ValueType, Hash>::find(const KeyType key)
        const -> const_iterator {
    const_iterator check = FindKeyConst(key);
    if (check != elements_.end()) {
        return check;
    }
    return elements_.cend();
}

template<class KeyType, class ValueType, class Hash>
void HashMap<KeyType, ValueType, Hash>::erase(const KeyType key) {
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
    modulo_ = initialModulo_;
    hash_table_.resize(modulo_);
}

template<class KeyType, class ValueType, class Hash>
ValueType& HashMap<KeyType, ValueType, Hash>::operator[](const KeyType key) {
    iterator check = FindKey(key);
    if (check != elements_.end()) {
        return (check->second);
    }
    ValueType v = {};
    insert(KeyValuePair(key, v));
    return find(key)->second;
}

template<class KeyType, class ValueType, class Hash>
const ValueType& HashMap<KeyType, ValueType, Hash>::at(const KeyType key)
        const {
    const_iterator check = FindKeyConst(key);
    if (check != elements_.end()) {
        return (check->second);
    }
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
void HashMap<KeyType, ValueType, Hash>::Rebuild(int new_modulo) {
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
    Rebuild(modulo_ * 2);
}

template<class KeyType, class ValueType, class Hash>
void HashMap<KeyType, ValueType, Hash>::HalveSize() {
    Rebuild(modulo_ / 2);
}

template<class KeyType, class ValueType, class Hash>
auto HashMap<KeyType, ValueType, Hash>::FindKey(
        const KeyType key) -> iterator {
    size_t bucket = hasher_(key) % modulo_;
    for (auto it : hash_table_[bucket]) {
        if (it->first == key) {
            return it;
        }
    }
    return elements_.end();
}

template<class KeyType, class ValueType, class Hash>
auto HashMap<KeyType, ValueType, Hash>::FindKeyConst(
        const KeyType key) const -> const_iterator {
    size_t bucket = hasher_(key) % modulo_;
    for (auto it : hash_table_[bucket]) {
        if (it->first == key) {
            return it;
        }
    }
    return elements_.end();
}
