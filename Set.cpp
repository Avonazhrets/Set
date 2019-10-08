#include <iostream>
#include <exception>
#include <stdexcept>

template<class ValueType>
class Set {
private:
    class node;

    class e_node {
    public:
        int height;
        node* left = nullptr;
        node* right = nullptr;
        e_node* parent = nullptr;

        e_node() : height(1) {}
    };

    class node : public e_node {
    public:
        ValueType value;

        node(ValueType val) : e_node(), value(val) {}
    };

    e_node* _end = nullptr;
    size_t _size = 0;

    node* get_first_element(node* _node) const {
        while (_node->left != nullptr) {
            _node = _node->left;
        }
        return _node;
    }

    node* get_last_element(node* _node) const {
        while (_node->right != nullptr) {
            _node = _node->right;
        }
        return _node;
    }

    int height(node* _node) const {
        return _node == nullptr ? 0 : _node->height;
    }

    int balance(node* _node) const {
        if (_node == nullptr)
            throw std::runtime_error("Set->balance: null _node");
        return height(_node->right) - height(_node->left);
    }

    void update_height(node* _node) {
        if (_node == nullptr)
            return;
        _node->height = std::max(height(_node->left), height(_node->right)) + 1;
    }

    node* small_R_rotate(node* x) {
        auto y = x->left;
        y->parent = x->parent;

        x->left = y->right;
        if (y->right != nullptr) {
            y->right->parent = x;
        }
        y->right = x;
        x->parent = y;

        update_height(x);
        update_height(y);
        return y;
    }

    node* small_L_rotate(node* x) {
        node* y = x->right;
        y->parent = x->parent;

        x->right = y->left;
        if (y->left != nullptr) {
            y->left->parent = x;
        }

        y->left = x;
        x->parent = y;

        update_height(x);
        update_height(y);
        return y;
    }

    node* balance_node(node* _node) {
        if (balance(_node) == 2) {
            if (balance(_node->right) < 0) {
                _node->right = small_R_rotate(_node->right);
            }
            return small_L_rotate(_node);
        }
        if (balance(_node) == -2) {
            if (balance(_node->left) > 0) {
                _node->left = small_L_rotate(_node->left);
            }
            return small_R_rotate(_node);
        }
        update_height(_node->left);
        update_height(_node->right);
        update_height(_node);
        return _node;
    }

    node* find_min_element(node* _node) const {
        while (_node != nullptr && _node->left != nullptr) {
            _node = _node->left;
        }
        return _node;
    }

    node* erase_min_element(node* _node) {
        if (_node == nullptr) {
            return _node;
        } else if (_node->left == nullptr) {
            return _node->right;
        }
        _node->left = erase_min_element(_node->left);
        if (_node->left != nullptr) {
            _node->left->parent = _node;
        }
        return balance_node(_node);
    }

    node* find_element(node* _node, const ValueType& value) const {
        if (_node == nullptr ||
            (!(value < _node->value) && !(_node->value < value))) {
            return _node;
        } else if (value < _node->value) {
            return find_element(_node->left, value);
        } else {
            return find_element(_node->right, value);
        }
    }

    node* lower_bound_element(node* _node, const ValueType& value) const {
        if (_node == nullptr)
            return nullptr;

        if (_node->value < value) {
            return lower_bound_element(_node->right, value);
        } else {
            auto element_left = lower_bound_element(_node->left, value);
            return element_left == nullptr ? _node : element_left;
        }
    }

    node* insert_value(node* _node, const ValueType& value) {
        if (_node == nullptr) {
            auto new_node = new node(value);
            return new_node;
        }
        if (!(value < _node->value) && !(_node->value < value)) {
            _size--;
            return _node;
        } else if (value < _node->value) {
            _node->left = insert_value(_node->left, value);
            if (_node->left != nullptr) {
                _node->left->parent = _node;
            }
        } else {
            _node->right = insert_value(_node->right, value);
            if (_node->right != nullptr) {
                _node->right->parent = _node;
            }
        }
        return balance_node(_node);
    }

    node* erase_value(node* _node, const ValueType& value) {
        if (_node == nullptr) {
            _size++;
            return nullptr;
        }
        if (value < _node->value) {
            _node->left = erase_value(_node->left, value);
            if (_node->left != nullptr) {
                _node->left->parent = _node;
            }
        } else if (_node->value < value) {
            _node->right = erase_value(_node->right, value);
            if (_node->right != nullptr) {
                _node->right->parent = _node;
            }
        } else {
            auto x = _node->left;
            auto y = _node->right;
            delete _node;
            if (y == nullptr) {
                return x;
            }
            node* min_element = find_min_element(y);
            min_element->right = erase_min_element(y);
            if (min_element->right != nullptr) {
                min_element->right->parent = min_element;
            }
            min_element->left = x;
            if (min_element->left != nullptr) {
                min_element->left->parent = min_element;
            }
            return balance_node(min_element);
            return min_element;
        }
        return balance_node(_node);
        return _node;
    }

public:
    class iterator {
    private:
        e_node* _node = nullptr;

        e_node* get_next_node(e_node* pointer) {
            if (pointer->parent == nullptr) {
                throw std::runtime_error("get_next_node(_end)");
            }

            if (pointer->right != nullptr) {
                pointer = pointer->right;
                while (pointer->left != nullptr) {
                    pointer = pointer->left;
                }
                return pointer;
            }

            while (pointer->parent != nullptr && pointer == pointer->parent->right) {
                pointer = pointer->parent;
            }
            return pointer->parent;
        }

        e_node* get_prev_node(e_node* pointer) {
            if (pointer->parent == nullptr && pointer->left == nullptr) {
                throw std::runtime_error("get_prev_node() empty set");
            }

            if (pointer->left != nullptr) {
                pointer = pointer->left;
                while (pointer->right != nullptr) {
                    pointer = pointer->right;
                }
                return pointer;
            }

            while (pointer->parent != nullptr && pointer == pointer->parent->left) {
                pointer = pointer->parent;
            }
            return pointer->parent;
        }

    public:
        iterator(e_node* pointer) : _node(pointer) {}

        iterator() : _node(nullptr) {}

        iterator operator++() {
            _node = get_next_node(_node);
            return *this;
        }

        const iterator operator++(int) {
            auto old_iterator = *this;
            ++*this;
            return old_iterator;
        }

        const ValueType& operator*() const {
            return (static_cast<node*>(_node))->value;
        }

        iterator operator--() {
            _node = get_prev_node(_node);
            return *this;
        }

        const iterator operator--(int) {
            auto old_iterator = *this;
            --*this;
            return old_iterator;
        }

        bool operator==(iterator other) const {
            return _node == other._node;
        }

        bool operator!=(iterator other) const {
            return _node != other._node;
        }

        const ValueType* operator->() const {
            return &(static_cast<node*>(_node)->value);
        }
    };

    Set() : _end(new e_node()), _size(0) {}

    template<class Iterator>
    Set(Iterator first, Iterator last) : _end(new e_node()), _size(0) {
        while (first != last) {
            insert(*first++);
        }
    }

    Set(const std::initializer_list<ValueType>& arr) {
        *this = Set(arr.begin(), arr.end());
    }

    Set(const Set<ValueType>& other) : _end(new e_node()), _size(0) {
        *this = other;
    }

    Set& operator=(const Set<ValueType>& other) {
        if (end() == other.end()) {
            return *this;
        }
        clear();
        if (_end == nullptr) {
            _end = new e_node();
        }
        for (const auto& element : other) {
            insert(element);
        }
        return *this;
    }

    iterator begin() const {
        if (empty()) {
            return iterator(_end);
        }
        return iterator(get_first_element(_end->left));
    }

    iterator end() const {
        return iterator(_end);
    }

    bool empty() const {
        return _size == 0;
    }

    size_t size() const {
        return _size;
    }

    void clear() {
        while (!empty()) {
            erase(*begin());
        }
    }

    void insert(const ValueType& element) {
        if (empty()) {
            _end->left = new node(element);
            _end->left->parent = _end;
        } else {
            _end->left = insert_value(_end->left, element);
            if (_end->left != nullptr) {
                _end->left->parent = _end;
            }
        }
        _size++;
    }

    void erase(ValueType element) {
        if (empty()) {
            return;
        } else {
            _end->left = erase_value(_end->left, element);
            if (_end->left != nullptr) {
                _end->left->parent = _end;
            }
        }
        _size--;
    }

    iterator find(const ValueType& value) const {
        auto element = find_element(_end->left, value);
        if (element == nullptr) {
            return iterator(_end);
        } else {
            return iterator(element);
        }
    }

    iterator lower_bound(const ValueType& value) const {
        auto element = lower_bound_element(_end->left, value);
        if (element == nullptr) {
            return iterator(_end);
        } else {
            return iterator(element);
        }
    }

    ~Set() {
        clear();
        if (_end != nullptr) {
            delete _end;
        }
    }
};
