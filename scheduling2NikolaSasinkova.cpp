#include <iostream>
#include <algorithm>
using namespace std;

const int n = 100;

typedef int index;
typedef index set_pointer;

struct nodetype {
    int parent;
    int depth;
    int smallest; // najmensi prvok
};

typedef nodetype universe[n];

universe U;

// inicializacia novej mnoziny, ktora obsahuje iba i
void makeset(index i) {
    U[i].parent = i;
    U[i].depth = 0;
    U[i].smallest = i; // zatial je v mnozine iba i, takze i je najmensie
}

// zistenie korenu mnoziny, v ktorej sa nachadza x
index find_set(index x) {
    if (U[x].parent == x) {
        return x; // x je koren
    }
    return find_set(U[x].parent); // rekurzivne hladanie korenu
}

// mergnutie dvoch mnozin s p a q
void merge(set_pointer p, set_pointer q) {
    if (U[p].depth == U[q].depth) {
        // ak maju rovnaku depth - p = koren + zvacsenie jeho hlbky
        U[q].parent = p;
        U[p].depth++;
        // aktualizacia najmensieho prvku v mergnutej mnozine
        if (U[q].smallest < U[p].smallest)
            U[p].smallest = U[q].smallest;
    }
    else if (U[p].depth < U[q].depth) {
        // strom s mensou hlbkou p sa pripoji pod strom s vacsou hlbkou q, aby sa nezvacsovala vyska vysledneho stromu
        U[p].parent = q;
        // koren = q
        if (U[p].smallest < U[q].smallest)
            U[q].smallest = U[p].smallest;
    }
    else {
        // pripojenie q pod p
        U[q].parent = p;
        // koren = p - smallest: min z hodnot smallest v oboch stromoch
        if (U[q].smallest < U[p].smallest)
            U[p].smallest = U[q].smallest;
    }
}

// vratenie najmensieho prvku = najmensieho volneho slotu
int small(set_pointer p) {
    return U[p].smallest;
}

struct Job {
    int id;
    int deadline;
    int profit;
};

Job jobsArr[n]; 
int resultArr[n];

void scheduleJobs(Job jobs[], int n, int maxDeadline) {

    // vstupne joby - kvoli zoradeniu
    Job jobsCopy[100];
    copy(jobs, jobs + n, jobsCopy);

    // zoradenie jobov podla profitov zostupne
    sort(jobsCopy, jobsCopy + n, [](const Job& a, const Job& b) {
        return a.profit > b.profit;
        });

    // inicializacia mnozin pre casove sloty, kazdy slot i = vlastna mnozina
    for (int i = 0; i <= maxDeadline; i++) {
        makeset(i);
        resultArr[i] = -1; // v slote nie je nic
    }

    // prechadzanie jobov v poradi podla profitov od najvacsieho
    for (int i = 0; i < n; i++) {
        int d = jobsCopy[i].deadline; // deadline akt. jobu

        // najdenie dostupneho slotu najblizsie k deadlinu
        int available = small(find_set(d));

        // ak uz nie je ziadny volny slot, job preskocim
        if (available == 0) {
            continue;
        }

        // naplanovanie jobu do slotu available
        resultArr[available] = jobsCopy[i].id;

        // slot available uz nie je volny
        merge(find_set(available), find_set(available - 1));
    }
}

int main() {

    int n = 8;

    jobsArr[0] = { 0, 0, 0 };
    jobsArr[1] = { 1, 2, 40 };
    jobsArr[2] = { 2, 4, 15 };
    jobsArr[3] = { 3, 3, 60 };
    jobsArr[4] = { 4, 2, 20 };
    jobsArr[5] = { 5, 3, 10 };
    jobsArr[6] = { 6, 1, 45 };
    jobsArr[7] = { 7, 1, 55 };

    // zistenie maximalneho deadlinu - aby som vedela, kolko slotov potrebujem
    int d = 0;
    for (int i = 0; i < n; i++) {
        d = max(d, jobsArr[i].deadline);
    }

    const int maxDeadline = d;

    scheduleJobs(jobsArr, n, d);

    // vypocitanie vysledneho zisku
    int totalProfit = 0;
    cout << "Scheduled Jobs: ";
    for (int i = 1; i <= d; i++) { // casove sloty
        if (resultArr[i] != -1) {
            cout << resultArr[i] << " ";
            totalProfit += jobsArr[resultArr[i]].profit;
        }
    }
    cout << endl;
    cout << "Maximum total profit = " << totalProfit << endl;
    return 0;
}