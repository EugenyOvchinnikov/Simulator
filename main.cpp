#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <set>
#include <fstream>
#include <iomanip>
#include <stdio.h>

struct Order
{
    int m_oid;      // идентификатор заявки
    char m_side;    // покупка (B) / продажа (S)
    int m_qty;      // количество
    double m_price; // цена

    Order(int oid, char side, int qty, double price) : m_oid(oid), m_side(side), m_qty(qty), m_price(price) {}
};

//заявки на покупку
struct SellOrder : Order
{
    SellOrder(int oid, char side, int qty, double price) : Order(oid, side, qty, price) {}

    friend bool operator< (const SellOrder& lhs, const SellOrder& rhs)
    {
        return  lhs.m_price < rhs.m_price;    // перегрузка оператора < для сортировки по цене
    }
};

//заявки на продажу
struct BuyOrder : Order
{
    BuyOrder(int oid, char side, int qty, double price) : Order(oid, side, qty, price) {}

    friend bool operator< (const BuyOrder& lhs, const BuyOrder& rhs)
    {
        return  lhs.m_price > rhs.m_price;    // перегрузка оператора < для сортировки по цене
    }
};

int main()
{
    FILE * inputFile;

    inputFile = fopen ("input.txt","r");

    // Если мы не можем открыть этот файл для чтения данных,
    if (!inputFile)
    {
        // то выводим сообщение об ошибке и выполняем функцию exit()
        std::cerr << "input.txt could not be opened!" << std::endl;
        exit(1);
    }

    std::ofstream outputFile("output_simulator.txt");


    // Если мы не можем открыть этот файл для записи данных,
    if (!outputFile)
    {
        // то выводим сообщение об ошибке и выполняем функцию exit()
        std::cerr << "output_simulator.txt could not be opened for writing!" << std::endl;
        exit(1);
    }

    std::multiset <SellOrder> sellOrders;
    std::multiset <SellOrder>::iterator it_sellOrders;

    std::multiset <BuyOrder> buyOrders;
    std::multiset <BuyOrder>::iterator it_buyOrders;

    char uid;
    int oid, now_oid;
    char side, now_side;
    int qty, now_qty;
    double price, now_price;

    int id = 0;
    int order_complete;

    std::cout << std::setprecision(2) << std::fixed;

    while (fscanf(inputFile, "%c, %d, %c, %d, %lf%с", &uid, &oid, &side, &qty, &price) != EOF)
    {
        // добавление заявки
        if (uid == 'O')
        {
            // заявка на продажу
            if (side == 'S')
            {
                order_complete = 0;

                for (it_buyOrders = buyOrders.begin(); it_buyOrders != buyOrders.end();)   // проверяем наличие заявки на покупку
                {
                    if ((*it_buyOrders).m_price >= price)   // найдена заявка на покупку
                    {
                        ++id; // счетчик сделок

                        if ((*it_buyOrders).m_qty > qty) // количество покупки больше количества продажи
                        {
                            now_oid = (*it_buyOrders).m_oid;
                            now_side = (*it_buyOrders).m_side;
                            now_qty= (*it_buyOrders).m_qty - qty;
                            now_price = (*it_buyOrders).m_price;

                            outputFile << "T," << id << ",B," << (*it_buyOrders).m_oid << "," << oid << "," << qty << "," << (*it_buyOrders).m_price << "\n"; // выводим сделку
                            buyOrders.erase(it_buyOrders); // удаляем заявку покупки
                            buyOrders.emplace(BuyOrder(now_oid, now_side, now_qty, now_price));   // возвращаем заявку покупки с новым количеством
                            order_complete = 1;
                            break;
                        }
                        if ((*it_buyOrders).m_qty == qty) // количество покупки равно количеству продажи
                        {
                            outputFile << "T," << id << ",B," << (*it_buyOrders).m_oid << "," << oid << "," << qty << "," << (*it_buyOrders).m_price << "\n"; // выводим сделку
                            buyOrders.erase(it_buyOrders); // удаляем заявку покупки
                            order_complete = 1;
                            break;
                        }
                        if ((*it_buyOrders).m_qty < qty) // количество покупки меньше количества продажи
                        {
                            qty = qty - (*it_buyOrders).m_qty;
                            outputFile << "T," << id << ",B," << (*it_buyOrders).m_oid << "," << oid << "," << (*it_buyOrders).m_qty << "," << (*it_buyOrders).m_price << "\n"; // выводим сделку
                            buyOrders.erase(it_buyOrders++); // удаляем заявку покупки
                        }
                    }
                    else
                    {
                        ++it_buyOrders;
                    }
                }
               if (order_complete == 0) sellOrders.insert(SellOrder(oid, side, qty, price));   // заявок на покупку нет или заявка выполнена не полностью, добавляем заявку продажи               
            }
            // заявка на покупку
            else if (side == 'B')
            {
                order_complete = 0;

                for (it_sellOrders = sellOrders.begin(); it_sellOrders != sellOrders.end();)   // проверяем наличие заявки на продажу
                {
                    if ((*it_sellOrders).m_price <= price)   // найдена заявка на продажу
                    {
                        ++id; // счетчик сделок

                        if ((*it_sellOrders).m_qty > qty) // количество продажи больше количества покупки
                        {
                            now_oid = (*it_sellOrders).m_oid;
                            now_side = (*it_sellOrders).m_side;
                            now_qty= (*it_sellOrders).m_qty - qty;
                            now_price = (*it_sellOrders).m_price;

                            outputFile << "T," << id << ",S," << (*it_sellOrders).m_oid << "," << oid << "," << qty << "," << (*it_sellOrders).m_price << "\n"; // выводим сделку
                            sellOrders.erase(it_sellOrders); // удаляем заявку продажи
                            sellOrders.emplace(SellOrder(now_oid, now_side, now_qty, now_price));   // возвращаем заявку продажи с новым количеством
                            order_complete = 1;
                            break;
                        }
                        if ((*it_sellOrders).m_qty == qty) // количество продажи равно количеству покупки
                        {
                            outputFile << "T," << id << ",S," << (*it_sellOrders).m_oid << "," << oid << "," << qty << "," << (*it_sellOrders).m_price << "\n"; // выводим сделку
                            sellOrders.erase(it_sellOrders); // удаляем заявку продажи
                            order_complete = 1;
                            break;
                        }
                        if ((*it_sellOrders).m_qty < qty) // количество продажи меньше количества покупки
                        {
                            qty = qty - (*it_sellOrders).m_qty;
                            outputFile << "T," << id << ",S," << (*it_sellOrders).m_oid << "," << oid << "," << (*it_sellOrders).m_qty << "," << (*it_sellOrders).m_price << "\n"; // выводим сделку
                            sellOrders.erase(it_sellOrders++); // удаляем заявку продажи
                        }
                    }
                    else
                    {
                        ++it_sellOrders;
                    }
                }
               if (order_complete == 0) buyOrders.emplace(BuyOrder(oid, side, qty, price));   // заявок на продажу нет или заявка выполнена не полностью, добавляем заявку покупки
            }
        }
        else
        {
            // отмена завки
            if (uid == 'C')
            {
                    for (it_sellOrders = sellOrders.begin(); it_sellOrders != sellOrders.end(); it_sellOrders++)
                    {
                        if ((*it_sellOrders).m_oid == oid)
                        {
                            sellOrders.erase(it_sellOrders);
                            outputFile << "X," << oid << "\n";
                            break;
                        }
                    }
                    for (it_buyOrders = buyOrders.begin(); it_buyOrders != buyOrders.end(); it_buyOrders++)
                    {
                        if ((*it_buyOrders).m_oid == oid)
                        {
                            buyOrders.erase(it_buyOrders);
                            outputFile << "X," << oid << "\n";
                            break;
                        }
                    }
            }
        }      
    }

    fclose (inputFile);
    return 0;
}