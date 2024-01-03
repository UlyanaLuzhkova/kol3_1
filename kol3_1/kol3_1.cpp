// kol3_1.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>

// Базовый класс для операции
class Operation {
public:
    virtual double execute(const std::vector<double>& numbers) const = 0;
    virtual ~Operation() {}
};

// Класс сложения
class Addition : public Operation {
public:
    double execute(const std::vector<double>& numbers) const override {
        double result = 0;
        for (double number : numbers) {
            result += number;
        }
        return result;
    }
};

// Класс умножения
class Multiplication : public Operation {
public:
    double execute(const std::vector<double>& numbers) const override {
        double result = 1;
        for (double number : numbers) {
            result *= number;
        }
        return result;
    }
};

// Класс суммы квадратов
class SumOfSquares : public Operation {
public:
    double execute(const std::vector<double>& numbers) const override {
        double result = 0;
        for (double number : numbers) {
            result += (number * number);
        }
        return result;
    }
};

// Функция для выполнения операции по заданному файлу
void processFile(const std::string& fileName, const Operation& operation, double& result, std::mutex& mtx) {
    std::ifstream file(fileName);
    if (file.is_open()) {
        std::string line;
        std::getline(file, line);

        std::vector<double> numbers;
        double number;
        std::istringstream iss(line);
        while (iss >> number) {
            numbers.push_back(number);
        }

        file.close();

        double operationResult = operation.execute(numbers);

        std::lock_guard<std::mutex> lock(mtx);
        result += operationResult;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "Usage: ./program <directory> <num_threads>" << std::endl;
        return 1;
    }

    std::string directory = argv[1];
    int numThreads = std::stoi(argv[2]);

    std::vector<std::thread> threads;
    double totalResult = 0;
    std::mutex resultMutex;
    const Operation* operations[] = { new Addition(), new Multiplication(), new SumOfSquares() };

    for (int i = 1; i <= numThreads; ++i) {
        std::string fileName = directory + "/in_" + std::to_string(i) + ".txt";
        int operationIndex = i % 3; // Циклическое использование операций
        threads.emplace_back(processFile, fileName, std::ref(*operations[operationIndex]), std::ref(totalResult), std::ref(resultMutex));
    }

    for (std::thread& t : threads) {
        t.join();
    }

    delete operations[0];
    delete operations[1];
    delete operations[2];

    std::ofstream outFile(directory + "/out.txt");
    if (outFile.is_open()) {
        outFile << totalResult;
        outFile.close();
    }

    return 0;
}
