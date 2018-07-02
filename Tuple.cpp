#include <iostream>
#include <tuple>
#include <iostream>
#include <vector>
#include <utility>
#include <cassert>
#include <functional>
#include <algorithm>


template <class... Args>
class Tuple {
};


template <typename T, class... Args>
class Tuple<T, Args...> : public Tuple<Args...> {
private:
    T value_;
public:

    static constexpr size_t size = sizeof...(Args) + 1;

    Tuple<Args...>* base;

    Tuple() : value_(), base(new Tuple<Args...>()) {}

    explicit Tuple(const T& value, const Args&... args) : value_(value) {
        base = new Tuple<Args...>(args...);
    }

    template<class... UTypes>
    explicit Tuple(T&& value, UTypes&&... args) : value_(std::forward<T>(value)) {
        base = new Tuple<Args...>(std::forward<UTypes>(args)...);
    }

    Tuple(const Tuple<T, Args...>& other) : value_(other.value_) {
        base = other.base;
    }

    Tuple(Tuple< T, Args... >&& other) : value_(std::move(other.value_)) {
        base = std::move(other.base);
        other.base = nullptr;
        other.value_ = T();
    }

    Tuple& operator=(const Tuple<T, Args...>& other) {
        if (this == &other) {
            return *this;
        }
        value_ = other.value_;
        base = other.base;
        return *this;
    }

    Tuple& operator=(Tuple<T, Args...>&& other) {
        value_ = std::move(other.value_);
        base = std::move(other.base);
        other.base = nullptr;
        other.value_ = T();
        return *this;
    }

    T& get_value() const {
        return const_cast<T&>(value_);
    }

    void swap(Tuple<T, Args...>& other) {
        Tuple<T, Args...> temp = other;
        other = std::move(*this);
        *this = std::move(temp);
    }

    ~Tuple<T, Args...>() = default;
};


template <>
class Tuple<> {
public:
    static constexpr size_t size = 0;
    int value_ = 0;
    int* base = nullptr;

    Tuple() {}

    Tuple(const Tuple<>& other) {}

    int& get_value() const {
        return const_cast<int&>(value_);
    }
};

template<class... UTypes>
constexpr auto makeTuple(UTypes&&... args) -> Tuple<std::decay<UTypes>...> {
return Tuple<std::decay<UTypes>...>(std::forward<UTypes>(args)...);
};

template<size_t i, typename... Args, class=std::enable_if_t<i != 0>>
constexpr decltype(auto) get(Tuple<Args...>& tuple) {
    return get<i - 1>(*tuple.base);
}

template<size_t i, typename... Args, class=std::enable_if_t<i != 0>>
constexpr decltype(auto) get(const Tuple<Args...>& tuple) {
    return get<i - 1>(*tuple.base);
}

template<size_t i, typename... Args, class=std::enable_if_t<i != 0>>
constexpr decltype(auto) get(Tuple<Args...>&& tuple) {
    return get<i - 1>(std::move(*tuple.base));
}

template<size_t i, typename... Args>
constexpr decltype(auto) get(Tuple<Args...>& tuple, typename std::enable_if<i == 0>::type* = 0) {
    return tuple.get_value();
}

template<size_t i, typename... Args>
constexpr decltype(auto) get(const Tuple<Args...>& tuple, typename std::enable_if<i == 0>::type* = 0) {
    return tuple.get_value();
}

template<size_t i, typename... Args>
constexpr decltype(auto) get(Tuple<Args...>&& tuple,  typename std::enable_if<i == 0>::type* = 0) {
    return std::move(tuple.get_value());
}
/*--------------------------------------------*/
template<typename T, typename U, typename... Args, class=std::enable_if_t<!std::is_same<U, std::decay_t<T>>::value>>
constexpr decltype(auto) get(Tuple<U, Args...>& tuple)  {
    return get<T>(*tuple.base);
}

template<typename T, typename U, typename... Args, class=std::enable_if_t<!std::is_same<U, std::decay_t<T>>::value>>
constexpr decltype(auto) get(const Tuple<U, Args...>& tuple) {
    return get<T>(*tuple.base);
}

template<typename T, typename U, typename... Args, class=std::enable_if_t<!std::is_same<U, std::decay_t<T>>::value>>
constexpr decltype(auto) get(Tuple<U, Args...>&& tuple) {
    return get<T>(*tuple.base);
}

template<typename T, typename U, typename...Args>
constexpr decltype(auto) get(Tuple<U, Args...>& tuple, typename std::enable_if<(std::is_same<U, std::decay_t<T>>::value)>::type* = 0) {
    return tuple.get_value();
}

template<typename T, typename U, typename...Args>
constexpr decltype(auto) get(const Tuple<U, Args...>& tuple, typename std::enable_if<(std::is_same<U, std::decay_t<T>>::value)>::type* = 0) {
    return tuple.get_value();
}

template<typename T, typename U, typename...Args>
constexpr decltype(auto) get(Tuple<U, Args...>&& tuple, typename std::enable_if<(std::is_same<U, std::decay_t<T>>::value)>::type* = 0) {
    return std::move(tuple.get_value());
}

template< class... TTypes, class... UTypes>
bool operator==(const Tuple<TTypes...>& lhs, const Tuple<UTypes...>& rhs) {
    if (lhs.size != rhs.size) {
        return false;
    } else if (lhs.size == 0) {
        return true;
    } else {
        return lhs.get_value() == rhs.get_value() && *lhs.base == *rhs.base;
    }
};

template< class... TTypes, class... UTypes >
bool operator!=(const Tuple<TTypes...>& lhs, const Tuple<UTypes...>& rhs) {
    return !(lhs == rhs);
};

template< class... TTypes, class... UTypes >
bool operator<(const Tuple<TTypes...>& lhs, const Tuple<UTypes...>& rhs) {
    if (lhs.size == 0) {
        return ((rhs.size == 0) ? false : true);
    } else if (rhs.size == 0) {
        return false;
    } else if (lhs.get_value() == rhs.get_value()) {
        return *lhs.base < *rhs.base;
    } else {
        return lhs.value_ < rhs.value_;
    }
};

template< class... TTypes, class... UTypes >
bool operator<=(const Tuple<TTypes...>& lhs, const Tuple<UTypes...>& rhs) {
    return !(lhs > rhs);
};

template< class... TTypes, class... UTypes >
bool operator>(const Tuple<TTypes...>& lhs, const Tuple<UTypes...>& rhs) {
    if (rhs.size == 0) {
        return lhs.size == 0 ? false : true;
    } else if (lhs.size == 0) {
        return false;
    } else if (lhs.get_value() == rhs.get_value()) {
        return *lhs.base > *rhs.base;
    } else {
        return lhs.get_value()> rhs.get_value();
    }
};

template< class... TTypes, class... UTypes >
bool operator>=(const Tuple<TTypes...>& lhs, const Tuple<UTypes...>& rhs) {
    return !(rhs < lhs);
};

void test_tuple() {


    {
        Tuple<int, std::string, long> tuple;
    }

    {
        int test_int = 5;
        std::string test_string = "Hello";
        Tuple<int, std::string> tuple(test_int, test_string);
    }

    {
        int test_int = 5;
        std::string test_string = "Hello";
        std::vector<int> test_vector(10, 5);
        Tuple<int, std::string, std::vector<int>> tuple(test_int, test_string, std::move(test_vector));
    }

    {
        std::string test_string = "Hello";
        std::vector<int> test_vector(10, 5);
        Tuple<std::string, std::vector<int>> tuple(std::move(test_string), std::move(test_vector));
    }

    {
        Tuple<int> first(5);
        auto second = first;
        first.swap(second);

        auto third = std::move(first);

        assert(third == second);
    }

    {
        Tuple<int> first(12);
        auto second = first;
        assert(get<0>(first) == get<0>(second));

        second = Tuple<int>(14);
        assert(get<0>(second) == 14);

        first.swap(second);
        assert(get<0>(second) == 12);
        assert(get<0>(first) == 14);

        int&& test_int = get<0>(std::move(first));
        assert(test_int == 14);
    }

    {
        Tuple<int, long> tuple(12, 16);
        assert(get<int>(tuple) == 12);
        assert(get<long>(tuple) == 16);

        int test_int_lv = get<int>(tuple);
        int&& test_int_rv = get<int>(std::move(tuple));
        assert(test_int_lv == test_int_rv);
    }

    {
        Tuple<int, std::string, std::vector<int>> tuple = makeTuple(5, std::string("test"), std::vector<int>(2, 5));
        get<2>(tuple)[1] = 2;
        assert(get<2>(tuple)[1] == 2);
    }

    {
        int test_int = 1;
        auto tuple = makeTuple(test_int, std::ref(test_int));
        test_int = 2;
        assert(get<0>(tuple) == 1);
        assert(get<1>(tuple) == 2);
    }

    {
        std::vector<std::tuple<int, std::string, float>> v;
        v.emplace_back(2, "baz", -0.1);
        v.emplace_back(2, "bar", 3.14);
        v.emplace_back(1, "foo", 100.1);
        std::sort(v.begin(), v.end());

        assert(get<0>(v[0]) == 1);
        assert(get<0>(v[1]) == 2);
        assert(get<0>(v[2]) == 2);

        assert(get<1>(v[0]) == std::string("foo"));
        assert(get<1>(v[1]) == std::string("bar"));
        assert(get<1>(v[2]) == std::string("baz"));
    }

    {
        auto tuple = makeTuple(1, "hello");
        auto test_tuple = tupleCat(tuple, tuple, makeTuple(5, 10));

        assert(get<0>(test_tuple) == get<2>(test_tuple));
        assert(get<4>(test_tuple) == 5);
    }


    for (int i = 0; i < 10000; ++i) {
        Tuple<int, std::vector<int>> tuple(4, std::vector<int>(10000, 5));
        assert(get<int>(tuple) == 4);
    }

}

int main() {
    test_tuple();
    return 0;
}
