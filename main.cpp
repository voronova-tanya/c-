#include <SFML/Graphics.hpp>
#include <utility>
#include <cmath>
#include <chrono>
#include <thread>
#include <string>
#include <random>
#include <iostream>

template<typename T>
class Grid {
public:
    using size_type = unsigned;

protected:
    T *data;
    size_type x_size;
    size_type y_size;
public:
    Grid(size_type y_size, size_type x_size, T const &t) : x_size(x_size), y_size(y_size) {
        data = (T *) operator new(y_size * x_size * sizeof(T));
        for (unsigned i = 0; i < x_size * y_size; ++i) {
            new(data + i) T(t);
        }
    }

    Grid(size_type y_size, size_type x_size) : Grid(y_size, x_size, T()) {}

    //y_size(y_size), x_size(x_size), ata(new T[x_size * y_size]) {}
    Grid(T const &t) : Grid(1, 1, t) {}


    //конструктор копирования
    Grid(Grid<T> const &src) : Grid(src.x_size, src.y_size) {
        for (unsigned i = 0; i < x_size * y_size; ++i) {
            data[i] = src.data[i];
        }
    }

    //оператор копирующего присваивания
    Grid<T> &operator=(Grid<T> const &src) {
        Grid<T> cmp(src);
        std::swap(this->data, cmp.data);
        std::swap(this->x_size, cmp.x_size);
        std::swap(this->y_size, cmp.y_size);
    }

    //конструктор перемещения
    Grid(Grid<T> &&src) : x_size(src.x_size), y_size(src.y_size), data(src.data) {
        src.data = nullptr;
    }

    //перемещающее присваивание
    Grid<T> &operator=(Grid<T> &&src) {
        Grid<T> cmp(std::move(src));
        std::swap(this->data, cmp.data);
        std::swap(this->x_size, cmp.x_size);
        std::swap(this->y_size, cmp.y_size);
    }

    //деструктор
    ~Grid<T>() {
        for (int i = 0; i < x_size * y_size; i++) {
            (*(data + i)).~T();
        }
        operator delete(data);
        //delete[] data;
    }

    T operator()(size_type y_idx, size_type x_idx) const {
        return data[y_idx * x_size + x_idx];
    }

    T &operator()(size_type y_idx, size_type x_idx) {
        return data[y_idx * x_size + x_idx];
    }

    Grid<T> &operator=(T const &t) {
        for (auto it = data, end = data + x_size * y_size; it != end; ++it)
            *it = t;
        return *this;
    }

    class Arr final {
    private:
        size_type const y_idx;
        Grid const &g;

    public:
        Arr(Grid &g, size_type y_idx) : y_idx(y_idx), g(g) {}

        T &operator[](size_type x_idx) const {
            return g.data[y_idx * g.x_size + x_idx];
        }
    };

    Arr operator[](size_type y_idx) {
        return Arr(*this, y_idx);
    }
};

struct Ball {
    sf::CircleShape shape;

    Ball(sf::Color color, int x, int y, int radius) : shape(radius) {
        shape.setFillColor(color);
        shape.setPosition(x, y);
    }

    Ball(Ball const &src) : Ball(src.shape.getFillColor(), src.shape.getPosition().x, src.shape.getPosition().y,
                                 src.shape.getRadius()) {
    }

    Ball() : Ball(sf::Color::Black, 0, 0, 0) {}

    //оператор копирующего присваивания
    Ball &operator=(Ball const &src) {
        Ball cmp(src);
        std::swap(this->shape, cmp.shape);
        return *this;
    }

    //конструктор перемещения
    Ball(Ball &&src) : shape(src.shape.getRadius()) {
        shape.setFillColor(src.shape.getFillColor());
        shape.setPosition(src.shape.getPosition().x, src.shape.getPosition().y);
    }

    //перемещающее присваивание
    Ball &operator=(Ball &&src) {
        Ball cmp(std::move(src));
        auto color = cmp.shape.getFillColor();
        auto x = cmp.shape.getPosition().x, y = cmp.shape.getPosition().y;
        std::swap(this->shape, cmp.shape);
        shape.setFillColor(color);
        shape.setPosition(x, y);
        return *this;
    }

    ~Ball() = default;

    void draw(sf::RenderWindow *window) const {
        window->draw(shape);
    }
};

struct Point {

    int x, y;

    Point(int x, int y) : x(x), y(y) {}

    Point(sf::Vector2i vector2) : Point(vector2.x, vector2.y) {}

    Point &operator=(sf::Vector2<int> vec) {
        x = vec.x;
        y = vec.y;
        return *this;
    }
};

struct Field : public Grid<Ball> {
    int size, score, size_cell, steps;
    sf::RenderWindow window;
    sf::Text text;
    sf::Font font;
    std::vector<sf::Color> colors;

    Field(int size, std::vector<sf::Color> colors) : Grid<Ball>(size, size, Ball(sf::Color::Magenta, 0, 0, 20)),
                                                size(x_size), size_cell(50), score(0), text(), steps(0),
                                                window(sf::VideoMode(800, 600), ""), colors(colors) {

        for (unsigned i = 0; i < size * size; ++i) {
            data[i] = Ball(colors[i], size_cell * (i / size), size_cell * (i % size), 20);

        }


        font.loadFromFile(".\\arial.ttf");

        text.setFont(font);
        text.setPosition(window.getSize().x * 0.8, 0);
        text.setString("Score: \n" + std::to_string(score) + "\n Steps: \n" + std::to_string(steps));
        text.setFillColor(sf::Color::White);
        text.setOutlineColor(sf::Color::Red);
    }

    Field() : Field(0, std::vector<sf::Color>()) {}

//    конструктор копирования
    Field(Field const &src) : Field(src.size, src.colors) {
        for (unsigned i = 0; i < x_size * y_size; ++i) {
            data[i] = src.data[i];
        }
    }

    //конструктор перемещения
    Field(Field &&src)  : Grid<Ball>(src.size, src.size, Ball(sf::Color::Magenta, 0, 0, 20)),
                         size(src.size), size_cell(src.size_cell), score(src.score), text(), steps(src.steps),
                         window(sf::VideoMode(600, 400), ""), colors(src.colors) {
        for (unsigned i = 0; i < size * size; ++i) {
            data[i] = Ball(colors[i], size_cell * (i / size), size_cell * (i % size), 20);
        }
        font.loadFromFile("../arial.ttf");
        text.setFont(src.font);
        text.setPosition(src.text.getPosition());
        text.setString(src.text.getString());
        text.setFillColor(src.text.getFillColor());
        src.data = nullptr;
    }

    //оператор копирующего присваивания
    Field &operator=(Field const &src) = delete;

    Field &operator=(Field &&src) = delete;

    ~Field() = default;

    void draw() {
        for (unsigned i = 0; i < size * size; ++i) {
            data[i].draw(&window);
        }
        window.draw(text);
    }

    std::vector<int> check() {
        std::vector<int> poses_v, poses_h;
        for (int i = 0; i < size; i++) {
            int start = 0, max = 1, current = 1;
            if (poses_v.empty()) {
                for (int j = 1; j < size; j++) {
                    auto color1 = data[i * size + j - 1].shape.getFillColor();
                    auto color2 = data[i * size + j].shape.getFillColor();
                    if (color1 == color2) {
                        current++;
                    } else {
                        if (current > max) {
                            max = current;
                            start = j - current;
                        }
                        current = 1;
                    }
                }
                if (current > max) {
                    max = current;
                    start = size - current;
                }
                if (max >= 3) {
                    for (int j = 0; j < max; j++) {
                        poses_v.push_back(i * size + start + j);
                    }
                }
            }
        }

        for (int j = 0; j < size; j++) {
            int start = 0, max = 1, current = 1;
            if (poses_h.empty()) {
                for (int i = 1; i < size; i++) {
                    auto color1 = data[(i - 1) * size + j].shape.getFillColor();
                    auto color2 = data[i * size + j].shape.getFillColor();
                    if (color1 == color2) {
                        current++;
                    } else {
                        if (current > max) {
                            max = current;
                            start = i - current;
                        }
                        current = 1;
                    }
                }
                if (current > max) {
                    max = current;
                    start = size - current;
                }
                if (max >= 3) {
                    for (int i = 0; i < max; i++) {
                        poses_h.push_back(i * size + start * size + j);
                    }
                }
            }
        }
        if (poses_h.size()>poses_v.size())
            return poses_h;
        return poses_v;
    }

    void remove(std::vector<int> poses) {
        score += poses.size();
        text.setString("Score: \n" + std::to_string(score) + "\n Steps: \n" + std::to_string(steps));
        if (abs(poses[0] - poses[1]) > 1) {
            for (int pose: poses) {
                int k = pose / size;
                for (int j = pose % size; j > 0; j--) {
                    data[k * size + j].shape.setFillColor(data[k * size + j - 1].shape.getFillColor());
                    data[k * size + j].shape.setRadius(data[k * size + j - 1].shape.getRadius());
                }
                int random = std::rand() % (size * size);
                data[k * size].shape.setFillColor(data[random].shape.getFillColor());
                data[k * size].shape.setRadius(data[random].shape.getRadius());
            }
        } else {
            int m = *std::max_element(poses.begin(), poses.end());
            int k = m / size;
            for (int j = m; j >= k * size + poses.size(); j--) {
                data[j].shape.setFillColor(data[j - poses.size()].shape.getFillColor());
                data[j].shape.setRadius(data[j - poses.size()].shape.getRadius());
            }
            for (int j = k * size; j < k * size + poses.size(); j++) {
                int random = std::rand() % (size * size);
                data[j].shape.setFillColor(data[random].shape.getFillColor());
                data[j].shape.setRadius(data[random].shape.getRadius());
            }
        }
    }

    int count(Point point, int ban_number = -1) {
        int x = point.x;
        int y = point.y;
        int number = -1;
        for (unsigned i = 0; i < size * size; ++i) {
            if (i != ban_number) {
                int r = data[i].shape.getRadius();
                if (pow((data[i].shape.getPosition().x + r - x), 2) + pow((data[i].shape.getPosition().y + r - y), 2)
                    <= pow(r, 2)) {
                    number = i;
                }
            }
        }
        return number;
    }

    void move(int number, Point start_data, Point start_mouse) {
        if (number >= 0) {
            data[number].shape.setPosition(sf::Mouse::getPosition(window).x + start_data.x - start_mouse.x,
                                           sf::Mouse::getPosition(window).y + start_data.y - start_mouse.y);
        }
    }

    int mouse_released(int moving_number, Point start_data) {
        int number = count(sf::Mouse::getPosition(window), moving_number);
        if (moving_number >= 0) {
            data[moving_number].shape.setPosition(start_data.x, start_data.y);
            bool near1 = ((moving_number % size == number % size) && (abs(moving_number / size - number / size) == 1));
            bool near2 = ((moving_number / size == number / size) && (abs(moving_number % size - number % size) == 1));
            bool near = near1 xor near2;
            if (number >= 0 && number != moving_number && near) {

                auto color = data[number].shape.getFillColor();
                data[number].shape.setFillColor(data[moving_number].shape.getFillColor());
                data[moving_number].shape.setFillColor(color);

                auto radius = data[number].shape.getRadius();
                data[number].shape.setRadius(data[moving_number].shape.getRadius());
                data[moving_number].shape.setRadius(radius);

                this->steps++;
                text.setString("Score: \n" + std::to_string(score) + "\n Steps: \n" + std::to_string(steps));
            }
            moving_number = -1;
        }
        return moving_number;
    }

    void full_draw(int moving_number) {
        window.clear();
        draw();
        if (moving_number >= 0) {
            data[moving_number].draw(&window);
        }
        window.display();
    }

    void run() {
        int moving_number = -1;
        Point start_data(0, 0), start_mouse(0, 0);
        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();

                if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                    if (event.type == sf::Event::MouseButtonPressed) {
                        start_mouse = sf::Mouse::getPosition(window);
                        moving_number = count(start_mouse);
                        if (moving_number >= 0)
                            start_data = Point(data[moving_number].shape.getPosition().x,
                                               data[moving_number].shape.getPosition().y);
                    }
                    move(moving_number, start_data, start_mouse);
                }

                if (event.type == sf::Event::MouseButtonReleased) {
                    moving_number = mouse_released(moving_number, start_data);
                }

            }
            std::vector<int> f = check();
            if (!f.empty()) {
                full_draw(moving_number);
                remove(f);
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            }
            full_draw(moving_number);
        }
    }
};


int main() {
    std::vector<sf::Color> color;
    color.push_back(sf::Color::Magenta);
    color.push_back(sf::Color::Blue);
    color.push_back(sf::Color::Green);
    color.push_back(sf::Color::Cyan);
    color.push_back(sf::Color::White);

    std::vector<sf::Color> colors;
    int n = 9;
    std::srand(time(NULL));
    for (int i = 0; i < n * n; i++) {
        int k = std::rand() % color.size();
        colors.push_back(color[k]);
    }

    Field field(n, colors);
    field.run();

    return 0;
}
