#include <iostream>
#include <cassert>

template <typename T>
class Grid final {
    using value_type = T;
    using size_type = unsigned;
private:
    T* const data;
    size_type y_size, x_size;
public:
    // Конструктор по размеру с заданным значением заполнения
    Grid(size_type y_size, size_type x_size, T const &t):
        y_size(y_size), x_size(x_size),
        data(static_cast<T*>(operator new(x_size * y_size * sizeof(T))))
    {
        for (size_type y = 0; y < y_size; ++y) {
            for (size_type x = 0; x < x_size; ++x) {
                new (data + (y * x_size + x)) T(t);
            }
        }
    }
    // Конструктор по размеру с дефолтным конструктором T
    Grid(size_type y_size, size_type x_size): Grid(y_size, x_size, T()) {  }
    // Конструктор из 1 элемента 1*1
    Grid(T const &t): Grid<T>(1, 1, t) {  }
    // Деструктор
    ~Grid() {
        for (size_type i = 0; i < x_size * y_size; ++i)
            data[i].~T();
        operator delete(data);
    }
    // Конструктор копирования
    Grid(Grid<T> const &src):
        x_size(src.x_size), y_size(src.y_size),
        data(static_cast<T*>(operator new(sizeof(T) * src.x_size * src.y_size)))
    {
        for (size_type y = 0; y < y_size; ++y) {
            for (size_type x = 0; x < x_size; ++x) {
                new (data (y * x_size + x)) T(src(y, x));
            }
        }
    }
    // Опреатор копирующего присваивания
    Grid<T>& operator=(Grid<T> const &src) {
        Grid<T> tmp(src);
        std::swap(this->data, tmp.data);
        std::swap(this->y_size, tmp.y_size);
        std::swap(this->x_size, tmp.x_size);
        return *this;
    }
    // Конструктор перемещения
    Grid(Grid<T> &&src): data(src.data), y_size(src.y_size), x_size(src.x_size) {
        src.data = nullptr;
        src.x_size = 0;
        src.y_size = 0;
    }
    // Оператор перемещающего присваивания
    Grid<T>& operator=(Grid<T> &&src) {
        Grid<T> tmp(std::move(src));
        std::swap(this->data, src.data);
        std::swap(this->y_size, src.y_size);
        std::swap(this->x_size, src.x_size);
        return *this;
    }
    T operator()(size_type y_idx, size_type x_idx) const {
        return data[y_idx * x_size + x_idx];
    }
    T& operator()(size_type y_idx, size_type x_idx) {
        return data[y_idx * x_size + x_idx];
    }
    Grid<T>& operator=(T const &t) {
        for (auto it = data, end = data + x_size * y_size; it != end; ++it) {
            *it = t;
        }
        return *this;
    }
    T* operator[](size_type y) {
        return data + y * x_size;
    }
    size_type get_y_size() const {return y_size;};
    size_type get_x_size() const {return x_size;};
};

int main() {
    Grid<float> g(3, 2, 0.0f);
    assert(3 == g.get_y_size());
    assert(2 == g.get_x_size());

    using gsize_t = unsigned;

    for (gsize_t y_idx = 0; y_idx != g.get_y_size(); ++y_idx)
        for (gsize_t x_idx = 0; x_idx != g.get_x_size(); ++x_idx)
            assert(0.0f == g[y_idx][x_idx]);

    for (gsize_t y_idx = 0; y_idx != g.get_y_size(); ++y_idx)
        for (gsize_t x_idx = 0; x_idx != g.get_x_size(); ++x_idx)
            g[y_idx][x_idx] = 1.0f;


    for (gsize_t y_idx = 0; y_idx != g.get_y_size(); ++y_idx)
        for (gsize_t x_idx = 0; x_idx != g.get_x_size(); ++x_idx)
            assert(1.0f == g[y_idx][x_idx]);

    return 0;
}
