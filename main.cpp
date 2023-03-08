#include <iostream>
#include <vector>
#include <map>
#include "Table.h"

using namespace std;

int main() {
    Table table("AQxx", "xxx", "JTx", "Kxx");
    cout << table.declarer_num_winners() << endl;
    return 0;
}
