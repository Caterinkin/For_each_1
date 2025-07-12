#include <iostream>
#include <vector>
#include <algorithm>
#include <future>    // ��� ����������� �����
#include <iterator>  // ��� std::distance
#include <clocale>   // ��� ��������� �������� �����

// ������������ ������ for_each � ����������� �����������
template<typename Iterator, typename Func>
void parallel_for_each(Iterator first, Iterator last, Func&& func, size_t min_block_size = 1000)
{
    // ��������� ������ ��������������� ���������
    const size_t size = std::distance(first, last);

    // ���� ������ ������ ������������ �����, ������������ ���������������
    if (size < min_block_size)
    {
        std::for_each(first, last, std::forward<Func>(func));
        return;
    }

    // ������� �������� ���������
    Iterator middle = first;
    std::advance(middle, size / 2);

    // ���������� ��������� parallel_for_each ��� ������ �������� ����������
    auto future = std::async(std::launch::async,
        [first, middle, &func, min_block_size]()
        {
            parallel_for_each(first, middle, std::forward<Func>(func), min_block_size);
        });

    // ������������ ������ �������� � ������� ������
    parallel_for_each(middle, last, std::forward<Func>(func), min_block_size);

    // ���������� ���������� ����������� ������
    future.get();
}

int main()
{
    // ������������� ������ ��� ��������� �������� �����
    setlocale(LC_ALL, "Russian");

    std::vector<int> numbers = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

    std::cout << "�������� ������:\n";
    for (const auto& num : numbers)
    {
        std::cout << num << " ";
    }
    std::cout << "\n\n";

    // ������� ��� ��������� ��������� (�������� � �������)
    auto square = [](int& n) { n *= n; };

    // ��������� parallel_for_each
    parallel_for_each(numbers.begin(), numbers.end(), square);

    std::cout << "������ ����� ���������� � �������:\n";
    for (const auto& num : numbers)
    {
        std::cout << num << " ";
    }
    std::cout << "\n";

    return 0;
}