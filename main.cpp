/*
    Гребенков Д.И.
    Группа М8О-207Б-19.
    Вариант задания: 12 (трапеция, список, очередь).

Разработать шаблоны классов согласно варианту задания.  Параметром шаблона должен являться
скалярный тип данных задающий тип данных для оси координат. Классы должны иметь публичные поля.
Фигуры являются фигурами вращения, т.е. равносторонние (кроме трапеции и прямоугольника).
Для хранения координат фигур необходимо использовать шаблон  std::pair.
Создать шаблон динамической коллекцию, согласно варианту задания:
1. Коллекция должна быть реализована с помощью умных указателей (std::shared_ptr, std::weak_ptr).
Опционально использование std::unique_ptr.
2. В качестве параметра шаблона коллекция должна принимать тип данных.
3. Коллекция должна содержать метод доступа:
    o Стек – pop, push, top;
    o Очередь – pop, push, top;
    o Список, динамический массив – доступ к элементу по оператору [].
4. Реализовать аллокатор, который выделяет фиксированный размер памяти (количество блоков памяти – 
является параметром шаблона аллокатора). Внутри аллокатор должен хранить указатель на используемый
блок памяти и динамическую коллекцию указателей на свободные блоки. Динамическая коллекция должна
соответствовать варианту задания (Динамический массив, Список, Стек, Очередь).
5. Коллекция должна использовать аллокатор для выделения и освобождения памяти для своих элементов.
6. Аллокатор должен быть совместим с контейнерами std::map и std::list.
7. Реализовать программу, которая:
    o позволяет вводить с клавиатуры фигуры (с типом int в качестве параметра шаблона фигуры) и
    добавлять в коллекцию использующую аллокатор;
    o позволяет удалять элемент из коллекции по номеру элемента;
    o выводит на экран введенные фигуры с помощью std::for_each.
*/

#include <iostream>
#include <queue>
#include <memory>
#include <cmath>
#include <algorithm>

const int MAX_SIZE = 100;

template <typename T>
class Trapezoid {
    public:
        using type = T;
        std::pair <T, T> left_top, left_bottom, right_bottom;
        Trapezoid () {}
        Trapezoid (T ltx, T lty, T lbx, T lby, T rbx, T rby) {
            left_top.first = ltx;
            left_top.second = lty;
            left_bottom.first = lbx;
            left_bottom.second = lby;
            right_bottom.first = rbx;
            right_bottom.second = rby;
        };
};

template <typename T>
void Vertices (Trapezoid <T> tr) {
    std::cout << "(" << tr.left_top.first << ", " << tr.left_top.second << ")"
    << "(" << tr.left_bottom.first << ", " << tr.left_bottom.second << ")"
    << "(" << tr.right_bottom.first << ", " << tr.right_bottom.second << ")"
    << "(" << tr.right_bottom.first - tr.left_top.first + tr.left_bottom.first << 
    ", " << tr.left_top.second << ")" << std::endl;
}

template <class T, int MAX_SIZE>
class Allocator {
    public:
        size_t sizeOne = sizeof (T);
        std::queue <T*> que;
        T memory [MAX_SIZE];
        using value_type = T;
        using pointer = T *;
        using const_pointer = const T*;
        using size_type = size_t;
        int count = 0;
        void initialize () {
            for (int i = 0; i < MAX_SIZE; i ++) {
                que.push (&memory [i]);
            }
        }
        T* allocate (int s) {
            T* res;
            if ((count + s - 1) < (MAX_SIZE)) {
                res = que.front ();
                for (int i = 0; i < s; i ++) {
                    que.pop ();
                    count ++;
                }
                return (res);
            }
            else {
                throw "ERROR: cannot allocate - no empty space.";
            }
            return (res);
        }
        void deallocate () {
            int size = que.size ();
            for (int i = 0; i < size; i ++) {
                que.pop ();
            }
            count = 0;
            initialize ();
        }
        template <class U>
        struct rebind {
            using other = Allocator <U, MAX_SIZE>;
        };
};

template <class T>
class ListMember {
    public:
        T figure;
        std::shared_ptr <ListMember <T> > next;
};

template <class T>
class Iterator {
    public:
        std::weak_ptr <T> it;
        Iterator (std::shared_ptr <T> al) {
            it = al;
        }
        Iterator * operator++ () {
            if (it.lock ()) {
                it = it.lock ()->next;
            }
            else {
                throw "ERROR: trying to go out of range.";
            }
            return this;
        }
        T operator* () {
            if (!it.lock ()) {
                throw "ERROR: trying to dereference nullptr.";
            }
            return (*(it.lock ().get ()));
        }
        bool operator!= (Iterator <T> sec) {
            bool res;
            if ((it.lock ().get ()) != (sec.it.lock ().get ())) {
                res = true;
            }
            else {
                res = false;
            }
            return (res);
        }
};

template <class T, class Allocator>
class List {
    public:
        std::shared_ptr <T> first;
        Allocator al;
        int count = 0;
        List () {
            al.initialize ();
        }
        struct deleter {
            void operator() (void * ptr) {}
        };
        deleter del;
        Iterator <T> begin () {
            return (Iterator <T> (first));
        }
        Iterator <T> end () {
            std::shared_ptr <T> temp = first;
            while (temp) {
                temp = temp->next;
            }
            return (Iterator <T> (temp));
        }
        void insert () {
            int idx;
            int ltx, lty, lbx, lby, rbx, rby;
            T* ptr = al.allocate (1);
            std::cout << "Enter index: ";
            std::cin >> idx;
            if ((idx > count) || (idx < 0)) {
                throw "ERROR: incorrect index.";
            }
            std::cout << "Enter coordinates of left-top, left-bottom and right-bottom vertices: " << 
            std::endl;
            std::cin >> ltx >> lty >> lbx >> lby >> rbx >> rby;
            std::shared_ptr <T> elem (new (ptr) T, del);
            elem->figure.left_top.first = ltx;
            elem->figure.left_top.second = lty;
            elem->figure.left_bottom.first = lbx;
            elem->figure.left_bottom.second = lby;
            elem->figure.right_bottom.first = rbx;
            elem->figure.right_bottom.second = rby;
            if (idx == 0) {
                if (first) {
                    elem->next = first;
                    first = elem;
                }
                else {
                    first = elem;
                }
                count ++;
            }
            else {
                std::shared_ptr <T> temp = first;
                std::shared_ptr <T> prev;
                for (int i = 0; i < idx; i ++) {
                    prev = temp;
                    temp = temp->next;
                }
                prev->next = elem;
                elem->next = temp;
                count ++;
            }
        }
        void erase () {
            int idx;
            std::cout << "Enter index: ";
            std::cin >> idx;
            if ((idx >= count) || (idx < 0)) {
                throw "ERROR: incorrect index.";
            }
            if (idx == 0) {
                if (first) {
                    first = first->next;
                    count --;
                }
                else {
                    throw "ERROR: list is empty.";
                }
            }
            else {
                std::shared_ptr <T> temp = first;
                std::shared_ptr <T> prev;
                for (int i = 0; i < idx; i ++) {
                    prev = temp;
                    temp = temp->next;
                }
                prev->next = temp->next;
                count --;
            }
        }
        T operator[] (int idx) {
            if ((idx >= count) || (idx < 0)) {
                throw "ERROR: incorrect index.";
            }
            std::shared_ptr <T> temp = first;
            for (int i = 0; i < idx; i ++) {
                temp = temp->next;
            }
            return (*(temp.get ()));
        }
};

void Print (ListMember <Trapezoid <int> > lm) {
    Vertices (lm.figure);
}

void ShowMenu () {
    std::cout << " 0. Show menu." << std::endl;
    std::cout << " 1. Insert figure." << std::endl;
    std::cout << " 2. Delete figure." << std::endl;
    std::cout << " 3. Print element." << std::endl;
    std::cout << " 4. Print list." << std::endl;
    std::cout << " 5. Quit program." << std::endl;
}

int main () {
    List <ListMember <Trapezoid <int> >, Allocator <ListMember <Trapezoid <int> >, MAX_SIZE > > list;
    int option = 7;
    ShowMenu ();
    while (option != 5) {
        std::cout << "Enter option: ";
        std::cin >> option;
        switch (option) {
            case 0: {
                ShowMenu ();
                break;
            }
            case 1: {
                try {
                    list.insert ();
                }
                catch (const char* exc) {
                    std::cerr << exc << std::endl;
                }
                break;
            }
            case 2: {
                try {
                    list.erase ();
                }
                catch (const char* exc) {
                    std::cerr << exc << std::endl;
                }
                break;
            }
            case 3: {
                try {
                    int idx;
                    std::cout << "Enter index: ";
                    std::cin >> idx;
                    Vertices (list [idx].figure);
                }
                catch (const char* exc) {
                    std::cerr << exc << std::endl;
                }
                break;
            }
            case 4: {
                try {
                    if (list.first) {
                        if (list.begin () != list.end ()) {
                            std::for_each (list.begin (), list.end (), Print);
                        }
                        else {
                            Vertices (list [0].figure);
                        }
                    }
                    else {
                        std::cout << "List is empty." << std::endl;
                    }
                }
                catch (const char* exc) {
                    std::cerr << exc << std::endl;
                }
                break;
            }
            case 5: {
                break;
            }
            default: {
                std::cout << "ERROR: incorrect option." << std::endl;
                break;
            }
        }
    }
    return 0;
}