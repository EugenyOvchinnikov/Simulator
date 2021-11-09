#define _CRT_SECURE_NO_WARNINGS
#include <fstream>
#include <iomanip>
#include <iostream>
#include <set>
#include <stdio.h>

void makeDeal(int& id, char side, int oid1, int oid2, int qty, double price);

struct Order
{
    int m_oid;      // идентификатор заявки
    char m_side;    // покупка (B) / продажа (S)
    int m_qty;      // количество
    double m_price; // цена

    Order(int oid = 0, char side = ' ', int qty = 0, double price = 0)
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

class WorkOrder
{
public:
    int order_complete;
    int now_oid;
    char now_side;
    int now_qty;
    double now_price;

    void makeSellOrder(std::multiset<SellOrder>& sellOrders,
                       std::multiset<SellOrder>::iterator& it_sellOrders,
                       std::multiset<BuyOrder>& buyOrders,
                       std::multiset<BuyOrder>::iterator& it_buyOrders,
                       SellOrder& sellorder,
                       int& id, std::ofstream& outputFile)
    {
        order_complete = 0;

        // проверяем наличие заявки на покупку
        for (it_buyOrders = buyOrders.begin(); it_buyOrders != buyOrders.end();)
        {
            now_price = (*it_buyOrders).m_price;
            if (now_price >= sellorder.m_price) // найдена заявка на покупку
            {
                now_oid = (*it_buyOrders).m_oid;
                now_side = (*it_buyOrders).m_side;
                now_qty = (*it_buyOrders).m_qty;
                if (now_qty > sellorder.m_qty) // количество покупки больше количества продажи
                {
                    // выводим сделку
                    makeDeal(id, 'B', now_oid, sellorder.m_oid, sellorder.m_qty, now_price, outputFile);
                    buyOrders.erase(it_buyOrders); // удаляем заявку покупки
                    // возвращаем заявку покупки с новым количеством
                    buyOrders.emplace(
                        BuyOrder(now_oid, now_side, (now_qty - sellorder.m_qty), now_price));
                    order_complete = 1;
                    break;
                }
                if (now_qty == sellorder.m_qty) // количество покупки равно количеству продажи
                {
                    // выводим сделку
                    makeDeal(id, 'B', now_oid, sellorder.m_oid, sellorder.m_qty, now_price, outputFile);
                    buyOrders.erase(it_buyOrders); // удаляем заявку покупки
                    order_complete = 1;
                    break;
                }
                if (now_qty < sellorder.m_qty) // количество покупки меньше количества продажи
                {
                    sellorder.m_qty = sellorder.m_qty - now_qty;
                    // выводим сделку
                    makeDeal(id, 'B', now_oid, sellorder.m_oid, now_qty, now_price, outputFile);
                    buyOrders.erase(it_buyOrders++); // удаляем заявку покупки
                }
            }
            else
            {
                ++it_buyOrders;
            }
        }
        if (order_complete == 0)
        {
            // заявок на покупку нет или заявка выполнена не полностью, добавляем
            // заявку продажи
            sellOrders.emplace(sellorder);
        }
    }
    void makeBuyOrder(std::multiset<SellOrder>& sellOrders,
                      std::multiset<SellOrder>::iterator& it_sellOrders,
                      std::multiset<BuyOrder>& buyOrders,
                      std::multiset<BuyOrder>::iterator& it_buyOrders,
                      BuyOrder& buyorder,
                      int& id, std::ofstream& outputFile)
    {
        order_complete = 0;

        // проверяем наличие заявки на продажу
        for (it_sellOrders = sellOrders.begin(); it_sellOrders != sellOrders.end();)
        {
            now_price = (*it_sellOrders).m_price;
            if (now_price <= buyorder.m_price) // найдена заявка на продажу
            {
                now_oid = (*it_sellOrders).m_oid;
                now_side = (*it_sellOrders).m_side;
                now_qty = (*it_sellOrders).m_qty;
                if (now_qty > buyorder.m_qty) // количество продажи больше количества покупки
                {
                    // выводим сделку
                    makeDeal(id, 'S', now_oid, buyorder.m_oid, buyorder.m_qty, now_price, outputFile);
                    sellOrders.erase(it_sellOrders); // удаляем заявку продажи
                    // возвращаем заявку продажи с новым количеством
                    sellOrders.emplace(
                        SellOrder(now_oid, now_side, (now_qty - buyorder.m_qty), now_price));
                    order_complete = 1;
                    break;
                }
                if (now_qty == buyorder.m_qty) // количество продажи равно количеству покупки
                {
                    // выводим сделку
                    makeDeal(id, 'S', now_oid, buyorder.m_oid, buyorder.m_qty, now_price, outputFile);
                    sellOrders.erase(it_sellOrders); // удаляем заявку продажи
                    order_complete = 1;
                    break;
                }
                if (now_qty < buyorder.m_qty) // количество продажи меньше количества покупки
                {
                    buyorder.m_qty = buyorder.m_qty - now_qty;
                    // выводим сделку
                    makeDeal(id, 'S', now_oid, buyorder.m_oid, now_qty, now_price, outputFile);
                    sellOrders.erase(it_sellOrders++); // удаляем заявку продажи
                }
            }
            else
            {
                ++it_sellOrders;
            }
        }
        if (order_complete == 0)
        {
            // заявок на продажу нет или заявка выполнена не
            // полностью, добавляем заявку покупки
            buyOrders.emplace(buyorder);
        }
    }

    void cancelOrder(std::multiset<SellOrder>& sellOrders,
                     std::multiset<SellOrder>::iterator& it_sellOrders,
                     std::multiset<BuyOrder>& buyOrders,
                     std::multiset<BuyOrder>::iterator& it_buyOrders,
                     int oid,
                     std::ofstream& outputFile)
    {
        for (it_sellOrders = sellOrders.begin(); it_sellOrders != sellOrders.end(); it_sellOrders++)
        {
            if ((*it_sellOrders).m_oid == oid)
            {
                sellOrders.erase(it_sellOrders);
                outputFile << "X," << oid << "\n"; // выводим сделку
                break;
            }
        }
        for (it_buyOrders = buyOrders.begin(); it_buyOrders != buyOrders.end(); it_buyOrders++)
        {
            if ((*it_buyOrders).m_oid == oid)
            {
                buyOrders.erase(it_buyOrders);
                outputFile << "X," << oid << "\n"; // выводим сделку
                break;
            }
        }
    }
    void makeDeal(int& id, char side, int oid1, int oid2, int qty, double price, std::ofstream& outputFile)
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

    std::multiset<SellOrder> sellOrders;
    std::multiset<SellOrder>::iterator it_sellOrders;

    std::multiset<BuyOrder> buyOrders;
    std::multiset<BuyOrder>::iterator it_buyOrders;

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
                        (new WorkOrder)
                            ->makeSellOrder(sellOrders,
                                            it_sellOrders,
                                            buyOrders,
                                            it_buyOrders,
                                            sellorder,
                                            id, outputFile);
                        break;
                    }
                    // заявка на покупку
                    case 'B':
                        BuyOrder buyorder(oid, side, qty, price);
                        (new WorkOrder)
                            ->makeBuyOrder(sellOrders,
                                           it_sellOrders,
                                           buyOrders,
                                           it_buyOrders,
                                           buyorder,
                                           id, outputFile);
                        break;
                }
                break;
            // отмена завки
            case 'C':
                (new WorkOrder)
                    ->cancelOrder(sellOrders,
                                  it_sellOrders,
                                  buyOrders,
                                  it_buyOrders,
                                  oid,
                                  outputFile);
                break;
        }
    }
    fclose(inputFile);
    outputFile.close();
    return 0;
}