#include <iostream>
#include <vector>
#include <algorithm>
#include <future>    // для асинхронных задач
#include <iterator>  // для std::distance
#include <clocale>   // для поддержки русского языка

// Параллельная версия for_each с рекурсивным разделением
template<typename Iterator, typename Func>
void parallel_for_each(Iterator first, Iterator last, Func&& func, size_t min_block_size = 1000)
{
    // Вычисляем размер обрабатываемого диапазона
    const size_t size = std::distance(first, last);

    // Если размер меньше минимального блока, обрабатываем последовательно
    if (size < min_block_size)
    {
        std::for_each(first, last, std::forward<Func>(func));
        return;
    }

    // Находим середину диапазона
    Iterator middle = first;
    std::advance(middle, size / 2);

    // Рекурсивно запускаем parallel_for_each для каждой половины асинхронно
    auto future = std::async(std::launch::async,
        [first, middle, &func, min_block_size]()
        {
            parallel_for_each(first, middle, std::forward<Func>(func), min_block_size);
        });

    // Обрабатываем вторую половину в текущем потоке
    parallel_for_each(middle, last, std::forward<Func>(func), min_block_size);

    // Дожидаемся завершения асинхронной задачи
    future.get();
}

int main()
{
    // Устанавливаем локаль для поддержки русского языка
    setlocale(LC_ALL, "Russian");

    std::vector<int> numbers = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

    std::cout << "Исходный массив:\n";
    for (const auto& num : numbers)
    {
        std::cout << num << " ";
    }
    std::cout << "\n\n";

    // Функция для обработки элементов (возводит в квадрат)
    auto square = [](int& n) { n *= n; };

    // Применяем parallel_for_each
    parallel_for_each(numbers.begin(), numbers.end(), square);

    std::cout << "Массив после возведения в квадрат:\n";
    for (const auto& num : numbers)
    {
        std::cout << num << " ";
    }
    std::cout << "\n";

    return 0;
}
