#include <iostream>
#include "category.h"

using namespace std;

int main() {
    Category c1(1, "An uong", CategoryType::EXPENSE);
    Category c2(2, "Luong", CategoryType::INCOME);

    cout << "Danh muc 1:" << endl;
    cout << "ID: " << c1.getId() << endl;
    cout << "Ten: " << c1.getName() << endl;
    cout << "Loai: " << c1.getTypeString() << endl;

    cout << endl;

    cout << "Danh muc 2:" << endl;
    cout << "ID: " << c2.getId() << endl;
    cout << "Ten: " << c2.getName() << endl;
    cout << "Loai: " << c2.getTypeString() << endl;

    return 0;
}