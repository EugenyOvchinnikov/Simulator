#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <set>
#include <fstream>
#include <iomanip>
#include <stdio.h>

// заявки на покупку
class OrderS
{
public:
    int m_oid;      // идентификатор заявки
    char m_side;    // продажа S
    int m_qty;      // количество
    double m_price; // цена

    OrderS(int oid, char side, int qty, double price) : m_oid(oid), m_side(side), m_qty(qty), m_price(price) {}

    friend bool operator< (const OrderS& o1, const OrderS& o2)
    {
        return  o1.m_price < o2.m_price;    // перегрузка оператора < для сортировки по цене
    }
};

// заявки на продажу
class OrderB
{
public:
    int m_oid;      // идентификатор заявки
    char m_side;    // покупка B
    int m_qty;      // количество
    double m_price; // цена

    OrderB(int oid, char side, int qty, double price) : m_oid(oid), m_side(side), m_qty(qty), m_price(price) {}

    friend bool operator< (const OrderB& o1, const OrderB& o2)
    {
        return  o1.m_price > o2.m_price;    // перегрузка оператора < для сортировки по цене
    }
};

int main()
{
    FILE * pFile;

    pFile = fopen ("input.txt","r");

    // Если мы не можем открыть этот файл для чтения данных,
    if (!pFile)
    {
        // то выводим сообщение об ошибке и выполняем функцию exit()
        std::cerr << "input.txt could not be opened!" << std::endl;
        exit(1);
    }

    std::ofstream outf("output_simulator.txt");


    // Если мы не можем открыть этот файл для записи данных,
    if (!outf)
    {
        // то выводим сообщение об ошибке и выполняем функцию exit()
        std::cerr << "output_simulator.txt could not be opened for writing!" << std::endl;
        exit(1);
    }

    std::multiset <OrderS> orders;
    std::multiset <OrderS>::iterator its;

    std::multiset <OrderB> orderb;
    std::multiset <OrderB>::iterator itb;

    char uid;
    int oid, now_oid;
    char side, now_side;
    int qty, now_qty;
    double price, now_price;

    int id = 0;
    int order_complite;

    std::cout << std::setprecision(2) << std::fixed;

    while (fscanf(pFile, "%c, %d, %c, %d, %lf%с", &uid, &oid, &side, &qty, &price) != EOF)
    {
        // добавление заявки
        if (uid == 'O')
        {
            // заявка на продажу
            if (side == 'S')
            {
                order_complite = 0;

                for (itb = orderb.begin(); itb != orderb.end();)   // проверяем наличие заявки на покупку
                {
                    if ((*itb).m_price >= price)   // найдена заявка на покупку
                    {
                        ++id; // счетчик сделок

                        if ((*itb).m_qty > qty) // количество покупки больше количества продажи
                        {
                            now_oid = (*itb).m_oid;
                            now_side = (*itb).m_side;
                            now_qty= (*itb).m_qty - qty;
                            now_price = (*itb).m_price;

                            outf << "T," << id << ",B," << (*itb).m_oid << "," << oid << "," << qty << "," << (*itb).m_price << "\n"; // выводим сделку
                            orderb.erase(itb); // удаляем заявку покупки
                            orderb.insert(OrderB(now_oid, now_side, now_qty, now_price));   // возвращаем заявку покупки с новым количеством
                            order_complite = 1;
                            break;
                        }
                        if ((*itb).m_qty == qty) // количество покупки равно количеству продажи
                        {
                            outf << "T," << id << ",B," << (*itb).m_oid << "," << oid << "," << qty << "," << (*itb).m_price << "\n"; // выводим сделку
                            orderb.erase(itb); // удаляем заявку покупки
                            order_complite = 1;
                            break;
                        }
                        if ((*itb).m_qty < qty) // количество покупки меньше количества продажи
                        {
                            qty = qty - (*itb).m_qty;
                            outf << "T," << id << ",B," << (*itb).m_oid << "," << oid << "," << (*itb).m_qty << "," << (*itb).m_price << "\n"; // выводим сделку
                            orderb.erase(itb++); // удаляем заявку покупки
                        }
                    }
                    else
                    {
                        ++itb;
                    }
                }
               if (order_complite == 0) orders.insert(OrderS(oid, side, qty, price));   // заявок на покупку нет или заявка выполнена не полностью, добавляем заявку продажи               
            }
            // заявка на покупку
            else if (side == 'B')
            {
                order_complite = 0;

                for (its = orders.begin(); its != orders.end();)   // проверяем наличие заявки на продажу
                {
                    if ((*its).m_price <= price)   // найдена заявка на продажу
                    {
                        ++id; // счетчик сделок

                        if ((*its).m_qty > qty) // количество продажи больше количества покупки
                        {
                            now_oid = (*its).m_oid;
                            now_side = (*its).m_side;
                            now_qty= (*its).m_qty - qty;
                            now_price = (*its).m_price;

                            outf << "T," << id << ",S," << (*its).m_oid << "," << oid << "," << qty << "," << (*its).m_price << "\n"; // выводим сделку
                            orders.erase(its); // удаляем заявку продажи
                            orders.insert(OrderS(now_oid, now_side, now_qty, now_price));   // возвращаем заявку продажи с новым количеством
                            order_complite = 1;
                            break;
                        }
                        if ((*its).m_qty == qty) // количество продажи равно количеству покупки
                        {
                            outf << "T," << id << ",S," << (*its).m_oid << "," << oid << "," << qty << "," << (*its).m_price << "\n"; // выводим сделку
                            orders.erase(its); // удаляем заявку продажи
                            order_complite = 1;
                            break;
                        }
                        if ((*its).m_qty < qty) // количество продажи меньше количества покупки
                        {
                            qty = qty - (*its).m_qty;
                            outf << "T," << id << ",S," << (*its).m_oid << "," << oid << "," << (*its).m_qty << "," << (*its).m_price << "\n"; // выводим сделку
                            orders.erase(its++); // удаляем заявку продажи
                        }
                    }
                    else
                    {
                        ++its;
                    }
                }
               if (order_complite == 0) orderb.insert(OrderB(oid, side, qty, price));   // заявок на продажу нет или заявка выполнена не полностью, добавляем заявку покупки
            }
        }
        else
        {
            // отмена завки
            if (uid == 'C')
            {
                    for (its = orders.begin(); its != orders.end(); its++)
                    {
                        if ((*its).m_oid == oid)
                        {
                            orders.erase(its);
                            outf << "X," << oid << "\n";
                            break;
                        }
                    }
                    for (itb = orderb.begin(); itb != orderb.end(); itb++)
                    {
                        if ((*itb).m_oid == oid)
                        {
                            orderb.erase(itb);
                            outf << "X," << oid << "\n";
                            break;
                        }
                    }
            }
        }      
    }

    fclose (pFile);
    return 0;
}