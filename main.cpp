#define _USE_MATH_DEFINES
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <vector>
#include <iostream>
#include <cstdlib>

// Функция находит точку пересечения двух отрезков
std::vector<float>get_intersection(std::vector<float> vertex1, std::vector<float> vertex2, std::vector<float> vertex3, std::vector<float> vertex4) {
    float x = 0; float y = 0;
    std::vector<float> intersection;
    if (vertex2[0] - vertex1[0] == 0) {
        x = vertex1[0];
        float m2 = ((vertex4[1] - vertex3[1]) / (vertex4[0] - vertex3[0]));
        float b2 = vertex3[1] - m2 * vertex3[0];
        y = m2 * x + b2;
    }

    else if (vertex4[0] - vertex3[0] == 0) {
        x = vertex3[0];
        float m1 = ((vertex2[1] - vertex1[1]) / (vertex2[0] - vertex1[0]));
        float b1 = vertex1[1] - m1 * vertex1[0];
        y = m1 * x + b1;
    }
    else {
        float m1 = (vertex2[1] - vertex1[1]) / (vertex2[0] - vertex1[0]);
        float b1 = vertex1[1] - m1 * vertex1[0];
        float m2 = (vertex4[1] - vertex3[1]) / (vertex4[0] - vertex3[0]);
        float b2 = vertex3[1] - m2 * vertex3[0];
        x = (b2 - b1) / (m1 - m2);
        y = m1 * x + b1;
    }

    intersection.push_back(x);
    intersection.push_back(y);

    return intersection;
}

// Функция проверяет находится ли точка внутри или вне фигуры
bool point_is_inside(std::vector < float>vertex1, std::vector<float> vertex2, std::vector<float>vertex3) {
    if (((vertex3[1] - vertex1[1]) * (vertex2[0] - vertex1[0]) - ((vertex3[0] - vertex1[0]) * (vertex2[1] - vertex1[1]))) < 0) return false;
    else return true;
}

// Функция находит все точки пересечения фигур
void get_coordinates(std::vector<std::vector<float>>coordinates1, std::vector<std::vector<float>>coordinates2, std::vector<std::vector<float>>& output) {
    std::vector<float> vertex1; std::vector<float>vertex2; std::vector<float>vertex3; std::vector<float> vertex4; std::vector<std::vector<float>>next;
    output = coordinates1; std::vector<std::vector<float>>new_coordinates;
    for (int i = 0; i < coordinates2.size(); i++) {
        if (output.size() == 0) { output = {}; }
        if (output.size() > 0) {
            new_coordinates = output;
            output = {};
            if (i == 0) { vertex1 = coordinates2[coordinates2.size() - 1]; }
            else { vertex1 = coordinates2[i - 1]; }
            vertex2 = coordinates2[i];

            for (int j = 0; j < new_coordinates.size(); j++) {

                if (j == 0) { vertex3 = new_coordinates[new_coordinates.size() - 1]; }
                else { vertex3 = new_coordinates[j - 1]; }
                vertex4 = new_coordinates[j];

                if (point_is_inside(vertex1, vertex2, vertex4)) {
                    if (not point_is_inside(vertex1, vertex2, vertex3)) {
                        std::vector<float>a = get_intersection(vertex3, vertex4, vertex1, vertex2);
                        output.push_back(a);
                    }
                    output.push_back(vertex4);
                }
                else if (point_is_inside(vertex1, vertex2, vertex3)) {
                    output.push_back(get_intersection(vertex3, vertex4, vertex1, vertex2));

                }
            }
        }


    }
}

// Функция нахождения площади
double count_square(std::vector < std::vector<float>>output) {
    if (output.size() == 0) { return 0; }
    else {
        output.push_back(output[0]);
        double xSum = 0; double ySum = 0;
        for (int i = 0; i < output.size() - 1; i++) {
            xSum += output[i][0] * output[i + 1][1];
            ySum += output[i][1] * output[i + 1][0];
        }
        return 0.5 * abs(xSum - ySum);
    }
}

int main()
{
    int Xlim = 1200;
    int Ylim = 800;

    // Создание окна приложения
    sf::RenderWindow window(sf::VideoMode(Xlim, Ylim), "Triangle Area Calculator");

    // Создание контейнера для хранения треугольников
    std::vector<sf::ConvexShape> triangles;

    // Хранение точек пересечения
    std::vector<std::vector<std::vector<float>>> vertices;
    std::vector<std::vector<std::vector<float>>> intersection_coordinates;
    std::vector<std::vector<float>> output;

    // Площадь
    double square;

    sf::Clock clock;  // Часть для плавной отрисовки 

    // Поиск шрифта
    sf::Font font;
    if (!font.loadFromFile("EdgeDisplay-Regular.otf"))
    {
        return EXIT_FAILURE;
    }

    // Создание текста для вывода позиции фигур
    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(18);
    text.setFillColor(sf::Color::Red);
    text.setPosition(10, 10);

    // Создаем вектор SFML
    std::vector<sf::Vector2f> points;

    // Создаем многоугольник
    sf::ConvexShape polygon;


    // Основной цикл приложения
    while (window.isOpen()) {
        // Плавная отрисовка
        float time = clock.getElapsedTime().asMicroseconds();  // Прошедшее время в микросекундах
        clock.restart();  // Перезагрузка времени окна
        time = time / 800;  // Скорость

        // Обработка событий
        sf::Event event;

        while (window.pollEvent(event))  // Работа внутри окна, пока оно открыто
        {
            // Закртытие окна обычным методом или через клавишу Esc 
            if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape))
            {
                window.close();
            }
            // Действия если произошло нажатие левой кнопки мыши
            else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
            {
                // Получение координат клика мыши
                sf::Vector2f mousePosition = window.mapPixelToCoords(sf::Mouse::getPosition(window));

                // Создание треугольника
                sf::ConvexShape triangle;
                triangle.setPointCount(3);

                float mx = mousePosition.x;
                float my = mousePosition.y;


                //вычисляем координаты вершин, расчеты в тетради
                float minAx = std::max(2.0f, 2 * mx - Xlim);
                float maxAx = mx;
                double ax = rand() % static_cast<int>(maxAx - minAx + 1) + minAx;

                float minBx = mx;
                float maxBx = std::min(2 * mx, static_cast<float>(Xlim));
                double bx = rand() % static_cast<int>(maxBx - minBx + 1) + minBx;

                double cx = 3 * mx - ax - bx;

                float minAy = std::max(2.0f, 2 * my - Ylim);
                float maxAy = my;
                double ay = rand() % static_cast<int>(maxAy - minAy + 1) + minAy;

                float minBy = my;
                float maxBy = std::min(2 * my, static_cast<float>(Ylim));
                double by = rand() % static_cast<int>(maxBy - minBy + 1) + minBy;

                double cy = 3 * my - ay - by;


                // Сортируем вершины по часовой стрелке от крайней левой
                float x1 = 0, x2 = 0, y1 = 0, y2 = 0, x3 = 0, y3 = 0;

                x1 = std::min(ax, cx);
                y1 = (x1 == ax) ? ay : cy;

                x2 = (by >= ((ax <= cx) ? cy : ay)) ? ((ax <= cx) ? cx : ay) : bx;
                y2 = (x2 == bx) ? by : ((ax <= cx) ? cy : ay);

                x3 = (by >= cy) ? bx : cx;
                y3 = (x3 == bx) ? by : cy;

                // Сохранение вершин для поиска пересечения
                std::vector<std::vector<float>> tmp = { {x1, y1}, {x2, y2}, {x3, y3} };
                vertices.push_back(tmp);

                // Устанавливаем координаты вершин в треугольнике
                triangle.setPoint(0, sf::Vector2f(ax, ay));
                triangle.setPoint(1, sf::Vector2f(bx, by));
                triangle.setPoint(2, sf::Vector2f(cx, cy));

                // Закраска треугольника рандомным цветом
                triangle.setFillColor(sf::Color(std::rand() % 256, std::rand() % 256, std::rand() % 256));

                // Установка цвета границы (черный)
                triangle.setOutlineColor(sf::Color::Black);
                triangle.setOutlineThickness(2.f);

                // Добавление треугольника в вектор
                triangles.push_back(triangle);

                // Сохранение информации в текст
                //text.setString("A:  " + std::to_string(static_cast<int>(x1)) + "   " + std::to_string(static_cast<int>(y1)) + " | " + "B:  " + std::to_string(static_cast<int>(x2)) + "   " + std::to_string(static_cast<int>(y2)) + " | " + "C:  " + std::to_string(static_cast<int>(x3)) + "   " + std::to_string(static_cast<int>(y3)) + "  MOUSE  " + std::to_string(static_cast<int>(mx)) + "   " + std::to_string(static_cast<int>(my) ));

                // Нахождение пересечения треугольников и его площади
                if (triangles.size() == 1) {
                    intersection_coordinates.push_back(vertices.back());
                }
                else if (triangles.size() > 1) {
                    get_coordinates(intersection_coordinates.back(), vertices.back(), output);
                    square = count_square(output);
                    points = {};

                    // Преобразуем каждую точку и добавляем ее в массив SFML
                    for (const auto& coordinate : output)
                    {
                        sf::Vector2f sfCoordinate(coordinate[0], coordinate[1]);
                        points.push_back(sfCoordinate);
                    }

                    // Сохранение вершин пересечения 
                    polygon.setPointCount(points.size());
                    for (int i = 0; i < points.size(); i++)
                    {
                        polygon.setPoint(i, points[i]);
                    }

                    // Устанавливаем цвет заливки пересечения
                    polygon.setFillColor(sf::Color::Blue);

                    text.setString("Square : " + std::to_string(static_cast<int>(square)));

                    intersection_coordinates.push_back(output);
                    output = {};
                }
            }
        }

        // Очистка окна
        window.clear(sf::Color::White);

        // Рисование всех треугольников из вектора
        for (const auto& triangle : triangles) {
            window.draw(triangle);
        }

        // Отрисовка многоугольника
        window.draw(polygon);

        // Вывод текста
        window.draw(text);

        // Обновление содержимого окна
        window.display();

    }

    return 0;
}