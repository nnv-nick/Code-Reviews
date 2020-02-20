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
    iterator begin() {
        return elems_.begin();
    }
    iterator end() {
        return elems_.end();
    }
    const_iterator begin() const {
        return elems_.begin();
    }
    const_iterator end() const {
        return elems_.end();
    }
    HashMap(Hash new_hash = Hash()) : hasher_(new_hash) {
        modulo_ = 8;
        amount_of_elems_ = 0;
        iters_.resize(modulo_);
    }
    template <typename iter_temp>
    HashMap(iter_temp start, iter_temp finish, Hash new_hash = Hash())
     : hasher_(new_hash) {
        modulo_ = 8;
        amount_of_elems_ = 0;
        iters_.resize(modulo_);
        for (auto it = start; it != finish; it ++) {
            insert(*it);
        }
    }
    HashMap(std::initializer_list<std::pair<KeyType, ValueType> >
             to_hashmap, Hash new_hash = Hash()) : hasher_(new_hash) {
        modulo_ = 8;
        amount_of_elems_ = 0;
        iters_.resize(modulo_);
        for (auto it : to_hashmap) {
            insert(it);
        }
    }
    void insert(std::pair<KeyType, ValueType> new_elem);
    Hash hash_function() const {
        return hasher_;
    }
    iterator find(KeyType key_to_find);
    const_iterator find(KeyType key_to_find) const;
    int size() const {
        return amount_of_elems_;
    }
    bool empty() const {
        return (amount_of_elems_ == 0);
    }
    void erase(KeyType key_to_erase);
    ValueType& operator[](const KeyType key);
    const ValueType& at(const KeyType key) const;
    void clear();
    HashMap& operator=(const HashMap& another_hashmap);

 private:
    std::list< std::pair<const KeyType, ValueType> > elems_;
    std::vector< std::list< typename std::list<
     std::pair<const KeyType, ValueType> >::iterator > > iters_;
    int modulo_, amount_of_elems_;
    Hash hasher_;
    void rebuild(bool is_size_increasing);
};

template<class KeyType, class ValueType, class Hash>
void HashMap<KeyType, ValueType, Hash>::insert
    (std::pair<KeyType, ValueType> new_elem) {
        size_t p = hasher_(new_elem.first);
        p %= modulo_;
        for (auto iterat : iters_[p]) {
            if (iterat->first == new_elem.first) {
                return;
            }
        }
        amount_of_elems_++;
        elems_.push_back(new_elem);
        auto it = elems_.end();
        it--;
        iters_[p].push_back(it);
        if (modulo_ == amount_of_elems_) {
            rebuild(true);
        }
    }

template<class KeyType, class ValueType, class Hash>
typename std::list< std::pair<const KeyType, ValueType> >::iterator
HashMap<KeyType, ValueType, Hash>::find(KeyType key_to_find) {
    size_t p = hasher_(key_to_find);
    p %= modulo_;
    for (auto it = iters_[p].begin(); it != iters_[p].end(); it ++) {
        if ((*it)->first == key_to_find) {
            return (*it);
        }
    }
    return elems_.end();
}

template<class KeyType, class ValueType, class Hash>
typename std::list<std::pair<const KeyType, ValueType> >::const_iterator
HashMap<KeyType, ValueType, Hash>::find(KeyType key_to_find) const {
    size_t p = hasher_(key_to_find);
    p %= modulo_;
    for (auto it = iters_[p].begin(); it != iters_[p].end(); it ++) {
        const_iterator toret = (*it);
        if ((*it)->first == key_to_find) {
            return toret;
        }
    }
    return elems_.end();
}

template<class KeyType, class ValueType, class Hash>
void HashMap<KeyType, ValueType, Hash>::erase(KeyType key_to_erase) {
    size_t p = hasher_(key_to_erase);
    p %= modulo_;
    for (auto it = iters_[p].begin(); it != iters_[p].end(); it ++) {
        if ((*it)->first == key_to_erase) {
            elems_.erase((*it));
            iters_[p].erase(it);
            amount_of_elems_--;
            if (amount_of_elems_ < modulo_ / 2) {
                rebuild(false);
            }
            return;
        }
    }
}

template<class KeyType, class ValueType, class Hash>
void HashMap<KeyType, ValueType, Hash>::clear() {
    iters_.clear();
    elems_.clear();
    amount_of_elems_ = 0;
    modulo_ = 8;
    iters_.resize(modulo_);
}

template<class KeyType, class ValueType, class Hash>
ValueType& HashMap<KeyType, ValueType, Hash>::operator[](const KeyType key) {
    size_t p = hasher_(key);
    p %= modulo_;
    for (auto it = iters_[p].begin(); it != iters_[p].end(); it ++) {
        if ((*it)->first == key) {
            return ((*it)->second);
        }
    }
    ValueType v = {};
    insert(std::pair<KeyType, ValueType>(key, v));
    return ((find(key))->second);
}

template<class KeyType, class ValueType, class Hash>
const ValueType& HashMap<KeyType, ValueType, Hash>::at(const KeyType key)
    const {
    size_t p = hasher_(key);
    p %= modulo_;
    for (auto it = iters_[p].begin(); it != iters_[p].end(); it ++) {
        if ((*it)->first == key) {
            return ((*it)->second);
        }
    }
    throw std::out_of_range("");
}

template<class KeyType, class ValueType, class Hash>
HashMap<KeyType, ValueType, Hash>& HashMap<KeyType, ValueType, Hash>::operator=
    (const HashMap<KeyType, ValueType, Hash>& another_hashmap) {
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
void HashMap<KeyType, ValueType, Hash>::rebuild(bool is_size_increasing) {
    if (is_size_increasing) {
        modulo_ *= 2;
    } else {
        modulo_ /= 2;
    }
    iters_.clear();
    iters_.resize(modulo_);
    for (auto it = elems_.begin(); it != elems_.end(); it ++) {
        size_t p = hasher_(it->first);
        p %= modulo_;
        iters_[p].push_back(it);
    }
}
