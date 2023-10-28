#include <iostream>
#include <memory>

template <typename T, unsigned n>
class Grid final {
    using value_type = T;
    using size_type = unsigned;
private:
    size_type size;
    Grid<T, n-1>* data;
    // Конструктор по размеру с дефолтным конструктором T
    // size_type* shape - массив длины n с соответствующими размерностями
    Grid(size_type* shape):
        data(static_cast<Grid<T, n-1>*>(operator new(sizeof(Grid<T, n-1>) * size))),
        size(shape[0])
    {
        for (size_type i = 0; i < size; ++i) {
            data[i] = Grid<T, n-1>(shape + 1);
        }
    }
    // Конструктор по размеру с заданным значением заполнения
    Grid(size_type* shape, T const &t):
        data(static_cast<Grid<T, n-1>*>(operator new(sizeof(Grid<T, n-1>) * size))),
        size(shape[0])
    {
        for (size_type i = 0; i < size; ++i) {
            data[i] = Grid<T, n-1>(shape + 1, t);
        }
    }
    // Конструктор из 1 элемента 1*1
    Grid(T const &t):
    data(static_cast<Grid<T, n-1>*>(operator new(sizeof(Grid<T, n-1>)))), size(1)
    {
        data[0] = Grid<T, n-1>(t);
    }
    // Деструктор
    ~Grid() {
        if (data == nullptr)
            return;
        for (int i = 0; i < size; ++i) {
            data[i].~Grid<T, n-1>();
        }
        delete[] data;
    }
    // Конструктор копирования
    Grid(Grid<T, n> const &src): size(src.size),
        data(static_cast<Grid<T, n-1>*>(operator new(sizeof(Grid<T, n-1>) * size)))
    {
        for (int i = 0; i < size; ++i) {
            data[i] = src.data[i];
        }
    }
    // Оператор копирующего присваивания
    Grid<T, 1>& operator=(Grid<T, n> const &src) {
        Grid<T, n> tmp(src);
        std::swap(this->data, tmp.data);
        std::swap(this->size, tmp.size);
        return *this;
    }
    // Конструктор перемещения
    Grid(Grid<T, n> &&src): data(src.data), size(src.size) {
        src.data = nullptr;
    }
    // Оператор перемещающего присваивания
    Grid<T, n>& operator=(Grid<T, n> &&src) {
        Grid<T, n> tmp(std::move(src));
        std::swap(this->data, tmp.data);
        std::swap(this->size, tmp.size);
        return *this;
    }
    // T operator()(size_type idx) const {
    //     return data[idx];
    // }

    // T& operator()(size_type idx) {
    //     return data[idx];
    // }
    Grid<T, n>& operator=(T const &t) {
        for (auto it = data, end = data + size; it != end; ++it) {
            *it = t;
        }
        return *this;
    }
    T* operator[](size_type idx) {
        return data[idx];
    }
    // size_type get_size() const {return size;};
};
template <typename T>
class Grid<T, 1> final {
    using value_type = T;
    using size_type = unsigned;
private:
    size_type size;
    T* data;
    // Конструктор с дефолтным конструктором T
    // size_type* shape - массив длины n с соответствующими размерностями
    Grid(size_type* shape):
        data(new T[shape[0]]),
        size(shape[0])
    {
        for (size_type i = 0; i < size; ++i) {
            data[i] = T();
        }
    }
    // Конструктор по размеру с заданным значением заполнения
    Grid(size_type* shape, T const &t):
        data(static_cast<T*>(operator new(sizeof(T) * size))),
        size(shape[0])
    {
        for (size_type i = 0; i < size; ++i) {
            data[i] = t;
        }
    }
    // Конструктор из 1 элемента 1*1
    Grid(T const &t):
    data(static_cast<T*>(operator new(sizeof(T)))), size(1)
    {
        data[0] = t;
    }
    // Деструктор
    ~Grid() {
        delete[] data;
    }
    // Конструктор копирования
    Grid(Grid<T, 1> const &src): size(src.size),
        data(static_cast<T*>(operator new(sizeof(T) * src.size)))
    {
        for (int i = 0; i < size; ++i) {
            data[i] = src.data[i];
        }
    }
    // Оператор копирующего присваивания
    Grid<T, 1>& operator=(Grid<T, 1> const &src) {
        Grid<T, 1> tmp(src);
        std::swap(this->data, tmp.data);
        std::swap(this->size, tmp.size);
        return *this;
    }
    // Конструктор перемещения
    Grid(Grid<T, 1> &&src): data(src.data), size(src.size) {
        src.data = nullptr;
    }
    // Оператор перемещающего присваивания
    Grid<T, 1>& operator=(Grid<T, 1> &&src) {
        Grid<T, 1> tmp(std::move(src));
        std::swap(this->data, tmp.data);
        std::swap(this->size, tmp.size);
        return *this;
    }
    T operator()(size_type idx) const {
        return data[idx];
    }
    T& operator()(size_type idx) {
        return data[idx];
    }
    Grid<T, 1>& operator=(T const &t) {
        for (auto it = data, end = data + size; it != end; ++it) {
            *it = t;
        }
        return *this;
    }
    T* operator[](size_type idx) {
        return data[idx];
    }
    size_type get_size() const {return size;};
};

int main() {

    return 0;
}
