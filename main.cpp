#define _USE_MATH_DEFINES
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <vector>
#include <iostream>
#include <cstdlib>


bool collinear(sf::Vector2f  mainPoint, sf::Vector2f b, sf::Vector2f c) {
    return (b.x - mainPoint.x) * (c.y - mainPoint.y) - (b.y - mainPoint.y) * (c.x - mainPoint.x) == 0;
}


std::vector<sf::Vector2f> grahamScan(std::vector<sf::Vector2f>& points) {
    std::vector<sf::Vector2f> hull;

    // Находим самую нижнюю и левую точку в наборе
    sf::Vector2f startPoint = points[0];
    for (const auto& point : points) {
        if (point.x < startPoint.x || (point.x == startPoint.x && point.y < startPoint.y)) {
            startPoint = point;
        }
    }

    // Сортируем точки по полярному углу относительно startPoint
    std::sort(points.begin(), points.end(), [startPoint](const sf::Vector2f& a, const sf::Vector2f& b) {
        // Вычисляем углы в радианах относительно startPoint
        float angleA = atan2(a.y - startPoint.y, a.x - startPoint.x);
        float angleB = atan2(b.y - startPoint.y, b.x - startPoint.x);
        return angleA < angleB;
    });

    // Добавляем первые две точки в выпуклую оболочку
    hull.push_back(points[0]);
    hull.push_back(points[1]);

    // Строим остальную часть выпуклой оболочки
    for (size_t i = 2; i < points.size(); ++i) {
        while (hull.size() >= 2) {
            sf::Vector2f new_vector = points[i] - hull.back();
            sf::Vector2f last_vector = hull.back() - hull[hull.size() - 2];
            if ((new_vector.x * last_vector.y - new_vector.y * last_vector.x) > 0)
                hull.pop_back();
            else {
                if (collinear(hull.back(), hull[hull.size() - 2], points[i])) {
                    hull.pop_back(); // Удаляем последнюю точку, так как она коллинеарна с предпоследней и новой точками
                }
                break;
            }
        }
        hull.push_back(points[i]);
    }
    return hull;
}



// Функция находит точку пересечения двух прямых
sf::Vector2f calculate_intersection_lines(sf::Vector2f l1_vertex1, sf::Vector2f l1_vertex2, sf::Vector2f l2_vertex1, sf::Vector2f l2_vertex2) {
    float x = 0; float y = 0;
    sf::Vector2f intersection;
    if (l1_vertex2.x - l1_vertex1.x == 0) {
        x = l1_vertex1.x;
        float k2 = ((l2_vertex2.y - l2_vertex1.y) / (l2_vertex2.x - l2_vertex1.x));
        float b2 = l2_vertex1.y - k2 * l2_vertex1.x;
        y = k2 * x + b2;
    }

    else if (l2_vertex2.x - l2_vertex1.x == 0) {
        x = l2_vertex1.x;
        float k1 = ((l1_vertex2.y - l1_vertex1.y) / (l1_vertex2.x - l1_vertex1.x));
        float b1 = l1_vertex1.y - k1 * l1_vertex1.x;
        y = k1 * x + b1;
    }
    else {
        float k1 = (l1_vertex2.y - l1_vertex1.y) / (l1_vertex2.x - l1_vertex1.x);
        float b1 = l1_vertex1.y - k1 * l1_vertex1.x;
        float k2 = (l2_vertex2.y - l2_vertex1.y) / (l2_vertex2.x - l2_vertex1.x);
        float b2 = l2_vertex1.y - k2 * l2_vertex1.x;
        x = (b2 - b1) / (k1 - k2);
        y = k1 * x + b1;
    }

    intersection.x = x;
    intersection.y = y;

    return intersection;
}

// Функция проверяет находится ли точка внутри или вне фигуры
bool is_point_internal(sf::Vector2f l_vertex1, sf::Vector2f l_vertex2, sf::Vector2f point) {
    if (((point.y - l_vertex1.y) * (l_vertex2.x - l_vertex1.x) - ((point.x - l_vertex1.x) * (l_vertex2.y - l_vertex1.y))) <= 0) return false;
    else return true;
}

// Функция находит все точки пересечения фигур
void find_coordinates(std::vector <sf::Vector2f>coordinates_polygon1, std::vector<sf::Vector2f>coordinates_polygon2, std::vector<sf::Vector2f>& output) {
    sf::Vector2f p1_vertex1; sf::Vector2f p1_vertex2; sf::Vector2f p2_vertex1; sf::Vector2f p2_vertex2;
    output = coordinates_polygon1;
    std::vector<sf::Vector2f>new_coordinates;

    for (int i = 0; i < coordinates_polygon2.size(); i++) {
        if (output.size() == 0) { output = {}; }
        if (output.size() > 0) {
            new_coordinates = output;
            output = {};
            if (i == 0) { p2_vertex1 = coordinates_polygon2[coordinates_polygon2.size() - 1]; }
            else { p2_vertex1 = coordinates_polygon2[i - 1]; }
            p2_vertex2 = coordinates_polygon2[i];

            for (int j = 0; j < new_coordinates.size(); j++) {

                if (j == 0) { p1_vertex1 = new_coordinates[new_coordinates.size() - 1]; }
                else { p1_vertex1 = new_coordinates[j - 1]; }
                p1_vertex2 = new_coordinates[j];

                if (is_point_internal(p2_vertex1, p2_vertex2, p1_vertex2)) {
                    if (not is_point_internal(p2_vertex1, p2_vertex2, p1_vertex1)) {
                        output.push_back(calculate_intersection_lines(p1_vertex1, p1_vertex2, p2_vertex1, p2_vertex2));
                    }
                    output.push_back(p1_vertex2);
                }
                else if (is_point_internal(p2_vertex1, p2_vertex2, p1_vertex1)) {
                    output.push_back(calculate_intersection_lines(p1_vertex1, p1_vertex2, p2_vertex1, p2_vertex2));

                }
            }
        }
    }
}

// Функция нахождения площади
double count_square(std::vector < sf::Vector2f >output) {
    if (output.size() == 0) { return 0; }
    else {
        output.push_back(output[0]);
        double x_Sum = 0; double y_Sum = 0;
        for (int i = 0; i < output.size() - 1; i++) {
            x_Sum += output[i].x * output[i + 1].y;
            y_Sum += output[i].y * output[i + 1].x;
        }
        return 0.5 * abs(x_Sum - y_Sum);
    }
}

int main()
{
    int Xlim = 1200;
    int Ylim = 800;

    // Установка текущего времени в качестве зерна для генератора случайных чисел
    srand(static_cast<unsigned>(time(0)));

    // Создание окна приложения
    sf::RenderWindow window(sf::VideoMode(Xlim, Ylim), "Triangle Area Calculator");

    // Начальное множетсво вершин
    std::vector<sf::Vector2f> vertices;

    // Случайный выпуклый многоугольнк
    std::vector<sf::Vector2f> convexHull;

    // Хранение таких многоугольников
    std::vector<sf::ConvexShape> polygons;

    // Хранение точек пересечения
    std::vector<std::vector <sf::Vector2f>> intersection_coordinates;
    std::vector<sf::Vector2f> output;

    // Площадь
    double square;

    sf::Clock clock;  // Часть для плавной отрисовки 

    // Поиск шрифта
    sf::Font font;
    if (!font.loadFromFile("EdgeDisplay-Regular.otf"))
    {
        return EXIT_FAILURE;
    }

    // Создание текста для вывода площади
    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(18);
    text.setFillColor(sf::Color::Red);
    text.setPosition(10, 10);

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
                // Очистка вершин многоугольника
                vertices.clear();

                // Получение координат клика мыши
                sf::Vector2f mousePosition = window.mapPixelToCoords(sf::Mouse::getPosition(window));

                // Добавление клика мыши в качестве одной из вершин многоугольника
                vertices.push_back(mousePosition);

                // Диапазон для x-координат точек
                int lower_boundX = std::max(1, static_cast<int>(mousePosition.x) - (Xlim / 4) + 1);
                int upper_boundX = std::min(Xlim - 1, static_cast<int>(mousePosition.x) + (Xlim / 4) - 1);

                // Диапазон для y-координат точек
                int lower_boundY = mousePosition.y + 1;
                int upper_boundY = std::min(Ylim - 1, static_cast<int>(mousePosition.y) + (Ylim / 2) - 1);

                // Генерация количества вершин и просто случайных вершин
                int count = rand() % (static_cast<int>(std::min(Xlim - mousePosition.x - 1, mousePosition.y - 1))) + 2;
                for (int i = 0; i < std::min(9, count); ++i) {
                    int randomX = lower_boundX + rand() % (upper_boundX - lower_boundX + 1);
                    int randomY = lower_boundY + rand() % (upper_boundY - lower_boundY + 1);
                    if (randomX == static_cast<int>(mousePosition.x))
                        randomX += 5;
                    sf::Vector2f randomPoint(randomX, randomY);
                    vertices.push_back(randomPoint);
                }

                // Вычисление выпуклой оболочки
                convexHull = grahamScan(vertices);

                //Устанавливаем точки в многоугольник
                sf::ConvexShape convexShape;
                convexShape.setPointCount(convexHull.size());
                for (size_t i = 0; i < convexHull.size(); ++i) {
                    convexShape.setPoint(i, convexHull[i]);
                }

                //Отрисовка цвета и границы
                convexShape.setFillColor(sf::Color(std::rand() % 256, std::rand() % 256, std::rand() % 256));
                convexShape.setOutlineThickness(2);
                convexShape.setOutlineColor(sf::Color::Black);

                // Добавление многоугольника в вектор
                polygons.push_back(convexShape);

                // Нахождение пересечения многоугольников и его площади
                if (polygons.size() == 1) {
                    intersection_coordinates.push_back(convexHull);
                }
                else if (polygons.size() > 1) {
                    find_coordinates(intersection_coordinates.back(), convexHull, output);
                    square = count_square(output);

                    // Установка вершин пересечения для отрисовки
                    polygon.setPointCount(output.size());
                    for (int i = 0; i < output.size(); i++)
                    {
                        polygon.setPoint(i, output[i]);
                    }

                    // Устанавливаем цвет заливки пересечения
                    polygon.setFillColor(sf::Color::Blue);
                    text.setString("Square : " + std::to_string(static_cast<int>(square)));
                    intersection_coordinates.push_back(output);
                    output.clear();
                }
            }
        }
        // Очистка окна
        window.clear(sf::Color::White);

        // Отрисовка всех многоугольники из вектора
        for (const auto& polygon : polygons) {
            window.draw(polygon);
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
