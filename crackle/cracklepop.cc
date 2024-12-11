#include <iostream>

using namespace std;

int main() {
  for (int i = 1; i<= 100; ++i) {
    if (i % 15 == 0) {
      cout << "CracklePop" << endl;
    } else if (i % 5 == 0) {
      cout << "Pop" << endl;
    } else if (i % 3 == 0) {
      cout << "Crackle" << endl;
    } else {
      cout << i << endl;
    }
  }
}
