#define _CRT_SECURE_NO_WARNINGS
#include <fstream>
#include <iomanip>
#include <iostream>
#include <set>
#include <stdio.h>

struct Order
{
    int m_oid;      // идентификатор заявки
    char m_side;    // покупка (B) / продажа (S)
    int m_qty;      // количество
    double m_price; // цена

    explicit Order(int oid, char side, int qty, double price)
        : m_oid(oid)
        , m_side(side)
        , m_qty(qty)
        , m_price(price)
    {
    }
};

//заявки на покупку
struct SellOrder : Order
{
    SellOrder(int oid, char side, int qty, double price) : Order(oid, side, qty, price) {}

    friend bool operator<(const SellOrder& lhs, const SellOrder& rhs)
    {
        // перегрузка оператора < для сортировки по возрастанию цены
        return lhs.m_price < rhs.m_price;
    }
};

//заявки на продажу
struct BuyOrder : Order
{
    BuyOrder(int oid, char side, int qty, double price) : Order(oid, side, qty, price) {}

    friend bool operator<(const BuyOrder& lhs, const BuyOrder& rhs)
    {
        // перегрузка оператора < для сортировки по убыванию цены
        return lhs.m_price > rhs.m_price;
    }
};

class TradeSystem
{
private:
    std::multiset<SellOrder> sellOrders;
    std::multiset<BuyOrder> buyOrders;

public:
    int order_complete;
    int now_oid;
    char now_side;
    int now_qty;
    double now_price;

    void makeSellOrder(SellOrder& sellorder, int& id, std::ofstream& outputFile)
    {
        order_complete = 0;

        // проверяем наличие заявки на покупку
        for (auto it = buyOrders.begin(); it != buyOrders.end();)
        {
            const auto& buyOrder = *it;
            now_price = buyOrder.m_price;
            if (now_price >= sellorder.m_price) // найдена заявка на покупку
            {
                now_oid = buyOrder.m_oid;
                now_side = buyOrder.m_side;
                now_qty = buyOrder.m_qty;
                if (now_qty > sellorder.m_qty) // количество покупки больше количества продажи
                {
                    // выводим сделку
                    makeDeal(id,
                             'B',
                             now_oid,
                             sellorder.m_oid,
                             sellorder.m_qty,
                             now_price,
                             outputFile);
                    buyOrders.erase(it); // удаляем заявку покупки
                    // возвращаем заявку покупки с новым количеством
                    buyOrders.emplace(
                        BuyOrder(now_oid, now_side, (now_qty - sellorder.m_qty), now_price));
                    order_complete = 1;
                    break;
                }
                if (now_qty == sellorder.m_qty) // количество покупки равно количеству продажи
                {
                    // выводим сделку
                    makeDeal(id,
                             'B',
                             now_oid,
                             sellorder.m_oid,
                             sellorder.m_qty,
                             now_price,
                             outputFile);
                    buyOrders.erase(it); // удаляем заявку покупки
                    order_complete = 1;
                    break;
                }
                if (now_qty < sellorder.m_qty) // количество покупки меньше количества продажи
                {
                    sellorder.m_qty = sellorder.m_qty - now_qty;
                    // выводим сделку
                    makeDeal(id, 'B', now_oid, sellorder.m_oid, now_qty, now_price, outputFile);
                    buyOrders.erase(it++); // удаляем заявку покупки
                }
            }
            else
            {
                ++it;
            }
        }
        if (order_complete == 0)
        {
            // заявок на покупку нет или заявка выполнена не полностью, добавляем
            // заявку продажи
            sellOrders.emplace(sellorder);
        }
    }
    void makeBuyOrder(BuyOrder& buyorder, int& id, std::ofstream& outputFile)
    {
        order_complete = 0;

        // проверяем наличие заявки на продажу
        for (auto it = sellOrders.begin(); it != sellOrders.end();)
        {
            const auto& sellOrder = *it;
            now_price = sellOrder.m_price;
            if (now_price <= buyorder.m_price) // найдена заявка на продажу
            {
                now_oid = sellOrder.m_oid;
                now_side = sellOrder.m_side;
                now_qty = sellOrder.m_qty;
                if (now_qty > buyorder.m_qty) // количество продажи больше количества покупки
                {
                    // выводим сделку
                    makeDeal(id,
                             'S',
                             now_oid,
                             buyorder.m_oid,
                             buyorder.m_qty,
                             now_price,
                             outputFile);
                    sellOrders.erase(it); // удаляем заявку продажи
                    // возвращаем заявку продажи с новым количеством
                    sellOrders.emplace(
                        SellOrder(now_oid, now_side, (now_qty - buyorder.m_qty), now_price));
                    order_complete = 1;
                    break;
                }
                if (now_qty == buyorder.m_qty) // количество продажи равно количеству покупки
                {
                    // выводим сделку
                    makeDeal(id,
                             'S',
                             now_oid,
                             buyorder.m_oid,
                             buyorder.m_qty,
                             now_price,
                             outputFile);
                    sellOrders.erase(it); // удаляем заявку продажи
                    order_complete = 1;
                    break;
                }
                if (now_qty < buyorder.m_qty) // количество продажи меньше количества покупки
                {
                    buyorder.m_qty = buyorder.m_qty - now_qty;
                    // выводим сделку
                    makeDeal(id, 'S', now_oid, buyorder.m_oid, now_qty, now_price, outputFile);
                    sellOrders.erase(it++); // удаляем заявку продажи
                }
            }
            else
            {
                ++it;
            }
        }
        if (order_complete == 0)
        {
            // заявок на продажу нет или заявка выполнена не
            // полностью, добавляем заявку покупки
            buyOrders.emplace(buyorder);
        }
    }

    void cancelOrder(int& oid, std::ofstream& outputFile)
    {
        for (auto it = sellOrders.begin(); it != sellOrders.end(); it++)
        {
            if ((*it).m_oid == oid)
            {
                sellOrders.erase(it);
                outputFile << "X," << oid << "\n"; // выводим сделку
                break;
            }
        }
        for (auto it = buyOrders.begin(); it != buyOrders.end(); it++)
        {
            if (it->m_oid == oid)
            {
                buyOrders.erase(it);
                outputFile << "X," << oid << "\n"; // выводим сделку
                break;
            }
        }
    }
    void makeDeal(
        int& id, char side, int oid1, int oid2, int qty, double price, std::ofstream& outputFile)
    {
        outputFile << "T," << ++id << "," << side << "," << oid1 << "," << oid2 << "," << qty << ","
                   << price << "\n"; // выводим сделку
    }
};

int main()
{
    FILE* inputFile;

    inputFile = fopen("input.txt", "r");

    // Если мы не можем открыть этот файл для чтения данных,
    if (!inputFile)
    {
        // то выводим сообщение об ошибке и выполняем функцию exit()
        std::cerr << "input.txt could not be opened!" << std::endl;
        exit(1);
    }

    //    std::ofstream outputFile("output_simulator.txt", std::ios::app);
    std::ofstream outputFile("output_simulator.txt");

    // Если мы не можем открыть этот файл для записи данных,
    if (!outputFile)
    {
        // то выводим сообщение об ошибке и выполняем функцию exit()
        std::cerr << "output_simulator.txt could not be opened for writing!" << std::endl;
        exit(1);
    }

    TradeSystem tradesystem;

    char uid;
    int oid;
    char side;
    int qty;
    double price;
    int id = 0; // счетчик сделок

    std::cout << std::setprecision(2) << std::fixed;

    while (fscanf(inputFile, "%c, %d, %c, %d, %lf%с", &uid, &oid, &side, &qty, &price) != EOF)
    {
        // анализ заявки
        switch (uid)
        {
            case 'O':
                switch (side)
                {
                    // заявка на продажу
                    case 'S':
                    {
                        SellOrder sellorder(oid, side, qty, price);
                        tradesystem.makeSellOrder(sellorder, id, outputFile);
                        break;
                    }
                    // заявка на покупку
                    case 'B':
                        BuyOrder buyorder(oid, side, qty, price);
                        tradesystem.makeBuyOrder(buyorder, id, outputFile);
                        break;
                }
                break;
            // отмена завки
            case 'C':
                tradesystem.cancelOrder(oid, outputFile);
                break;
        }
    }
    fclose(inputFile);
    outputFile.close();
    return 0;
}