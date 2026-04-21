#include <iostream>
#include <algorithm>
using namespace std;

// postupnost indexov jobov
struct sequence_of_integer {
    int data[100]; // pole na ulozenie indexov jobov
    int size; // velkost postupnosti

    sequence_of_integer() : size(0) {}

    // pridanie prvku na koniec postupnosti
    void push_back(int value) {
        data[size++] = value;
    }

    // pristup k prvku cez operator []
    int& operator[](int index) {
        return data[index];
    }

    const int& operator[](int index) const {
        return data[index];
    }

    int getSize() const {
        return size;
    }

    // priradenie jednej postupnosti do druhej
    sequence_of_integer& operator=(const sequence_of_integer& other) {
        size = other.size;
        for (int i = 0; i < size; ++i) {
            data[i] = other.data[i];
        }
        return *this;
    }

    // vlozenie jobu tak, aby boli joby zoradene podla deadlinov
    void insertInOrder(int job, const int deadline[]) {
        int insertPos = size;

        // najdenie pozicie, kam vlozit job podla deadlinu
        for (int p = 0; p < size; ++p) {
            if (deadline[job] < deadline[data[p]]) {
                insertPos = p;
                break;
            }
        }

        // posunutie prvkov doprava a vlozenie noveho jobu
        size++;
        for (int pos = size - 1; pos > insertPos; --pos) {
            data[pos] = data[pos - 1];
        }
        data[insertPos] = job;
    }
};

// kontrola, ci je postupnost J realizovatelna vzhladom na deadline
bool isFeasible(const sequence_of_integer& J, const int deadline[]) {
    int size = J.getSize();
    for (int pos = 0; pos < size; ++pos) {
        int jobIndex = J[pos];  
        int finishingTime = pos + 1;  
        if (finishingTime > deadline[jobIndex]) {
            return false;
        }
    }
    return true;
}

void schedule(int n, const int deadline[], sequence_of_integer& J) {
    // prvy job - ten, kt. ma najvacsi profit
    J.push_back(1);
    sequence_of_integer K;

    // kazdy dalsi job sa skusi pridat do schedulu
    for (int i = 2; i <= n; i++) {
        K = J; // aktualny schedule

        // vlozenie noveho jobu tak, aby bolo poradie podla deadlinov
        K.insertInOrder(i, deadline);

        // ak je novy schedule realizovatelny, prijmem ho
        if (isFeasible(K, deadline)) {
            J = K;
        }
    }
}

int main() {
    const int n = 7;

    int deadline[n + 1] = { 0, 2, 4, 3, 2, 3, 1, 1 };
    int profit[n + 1] = { 0, 40, 15, 60, 20, 10, 45, 55 };

    int jobOrder[n + 1];

    // vytvorenie poradia indexov jobov od 1 do n
    for (int i = 1; i <= n; ++i) {
        jobOrder[i] = i;
    }

    // zoradenie indexov jobov podla profitu zostupne
    sort(jobOrder + 1, jobOrder + n + 1, [&](int a, int b) {
        return profit[a] > profit[b];
        });

    int sortedDeadline[n + 1];
    int sortedProfit[n + 1];
    int originalJobMap[n + 1];  

    for (int i = 1; i <= n; ++i) {
        int origJob = jobOrder[i]; // povodny index jobu
        sortedDeadline[i] = deadline[origJob];
        sortedProfit[i] = profit[origJob];
        originalJobMap[i] = origJob; // pozicia i v zoradenom poli zodpoveda jobu origJob
    }

    sequence_of_integer J; // vysledne joby v zoradenom poradi
    schedule(n, sortedDeadline, J);

    int totalProfit = 0;
    int size = J.getSize();
    cout << "Scheduled Jobs: ";
    for (int i = 0; i < size; ++i) {
        int sortedPos = J[i]; // pozicia v zoradenom zozname - podla profitu
        int originalJob = originalJobMap[sortedPos]; // povodny index jobu
        cout << originalJob << " ";
        totalProfit += profit[originalJob]; // pocitanie zisku
    }
    cout << "\nMaximum total profit = " << totalProfit << endl;

    return 0;
}