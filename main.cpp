#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <algorithm> // Для std::max_element

void drawHistogram(const std::vector<double>& lengths, const cv::Scalar& histColor) {
    // Размеры изображения гистограммы
    int histWidth = 512; // Ширина изображения
    int histHeight = 400; // Высота изображения
    int binWidth = cvRound((double) histWidth / lengths.size()); // Ширина столбца

    // Создание изображения гистограммы
    cv::Mat histImage(histHeight, histWidth, CV_8UC3, cv::Scalar(255, 255, 255));

    // Нахождение максимального значения для нормализации
    double maxVal = *std::max_element(lengths.begin(), lengths.end());

    // Нормализация значений и отрисовка каждого столбца гистограммы
    for (size_t i = 0; i < lengths.size(); i++) {
        rectangle(histImage,
                  cv::Point(i * binWidth, histHeight),
                  cv::Point((i + 1) * binWidth, histHeight - cvRound(lengths[i] * histHeight / maxVal)),
                  histColor, cv::FILLED);
    }

    // Отображение гистограммы
    cv::imshow("Гистограмма", histImage);
    cv::waitKey(0);
}

int main() {
    // Чтение изображения гистограммы
    cv::Mat image = cv::imread("../src/real_2.jpg", cv::IMREAD_COLOR);
    if (image.empty()) {
        std::cout << "Не удалось загрузить изображение" << std::endl;
        return -1;
    }

    // Преобразование из BGR в HSV
    cv::Mat hsvImage;
    cv::cvtColor(image, hsvImage, cv::COLOR_BGR2HSV);

    // Определение диапазона HSV для синего цвета гистограммы
    // Эти значения могут потребовать настройки
    cv::Scalar lowerBlue(100, 150, 0); // Нижний порог синего цвета в HSV
    cv::Scalar upperBlue(140, 255, 255); // Верхний порог синего цвета в HSV

    // Пороговая обработка для выделения синего цвета
    cv::Mat blueMask;
    cv::inRange(hsvImage, lowerBlue, upperBlue, blueMask);

    // Нарисовать только горизонтальные линии контуров на исходном изображении
    // Нахождение контуров на бинарном изображении
 // Нахождение контуров на бинарном изображении
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(blueMask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    // Вектор для хранения длин горизонтальных линий
    std::vector<double> horizontalLineLengths;

    // Обход всех контуров
    for (size_t i = 0; i < contours.size(); i++) {
        // Упрощение контура до многоугольника
        std::vector<cv::Point> polygon;
        cv::approxPolyDP(contours[i], polygon, 0.1, true);
        std::cout << "Идет обход полигона " << i << "\n"; 
        // Обход всех сторон многоугольника
        for (size_t j = 0; j < polygon.size(); j++) {
            // Получение следующей точки (циклически)
            cv::Point p1 = polygon[j];
            cv::Point p2 = polygon[(j + 1) % polygon.size()];        
            std::cout << "{" << p1.x  << "," << p1.y << "} {" << p2.x  << "," << p2.y  << "}\n";            
            // Проверка, является ли линия горизонтальной
            if (p1.y != 401 and std::abs(p1.y - p2.y)<=1) {
            //if (true) {
                double length = 401-p1.y;
                horizontalLineLengths.push_back(length);
                // Рисование горизонтальной линии
                cv::line(image, p1, p2, cv::Scalar(0, 255, 0), 2);
            }
        }
    }

    // Отображение изображения с контурами
    cv::imshow("Изображение с выделенными горизонтальными линиями", image);
    cv::waitKey(0);

        // Вывод значений в консоль
    std::cout << "[";
    for (size_t i = 0; i < horizontalLineLengths.size(); i++) {
        std::cout << horizontalLineLengths[i];
        if (i < horizontalLineLengths.size() - 1) std::cout << ", ";
    }
    std::cout << "]" << std::endl;

    // Отрисовка гистограммы с длинами горизонтальных линий
    //drawHistogram(horizontalLineLengths, cv::Scalar(185, 121, 0)); // Используем BGR формат цвета

    return 0;
}