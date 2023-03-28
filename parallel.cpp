#include <iostream>
#include <omp.h>
#include <cmath>
#include <iomanip>
#include <fstream>
omp_lock_t lock;
using namespace std;
ofstream file;


class Integral {
    public:
        virtual float f(float x) = 0;
        float a, b;
};


class FirstIntegral: public Integral {
    public:
        float f(float x) override {
            return 1 / (0.35 * pow(cos(0.6 * x), 2) + pow(sin(0.6 * x), 2));
        }
};


class SecondIntegral: public Integral {
    public:
        float f(float x) override {
            return (x * sqrtf(0.7 * pow(x, 2) + 1.2 * x + 2.1)) / pow(2.1 + x, 2);
        }
};


float getIntegralVal (Integral& integral, float h, int threadsNum) {
    int n = (integral.b - integral.a) / h;
    float sum = 0;

    omp_set_num_threads(threadsNum);
    double t1, t2;
    t1 = omp_get_wtime();
    omp_init_lock(&lock);
    #pragma omp parallel shared(n)
    {
        float x;
        #pragma omp master
        cout << "Lab 3, Завантаження та синхронізація в OpenMP, КН-314, Ратінський Олег Юрійович, Варіант 19, \n"
                "Розробити програму паралельного розрахунку означеного інтегралу за методом лівих прямокутників" << endl;

        #pragma omp for schedule(dynamic, 2)
        for (int i = 0; i <= n - 1; i++) {
            x = integral.a + i * h;

            #pragma omp atomic
            sum += integral.f(x);

            omp_set_lock(&lock);
            file << "The beginning of the closed section (thread " << omp_get_thread_num() << ")...\n" ;
            file << "The end of the closed section (thread " << omp_get_thread_num() << ")...\n" ;
            omp_unset_lock(&lock);
        }
    }
    omp_destroy_lock(&lock);
    t2 = omp_get_wtime();
    cout << endl << "Time (seconds): " << setprecision(50) << t2 - t1 << endl;

    float result = h * sum;
    return result;
}


int main() {
    float step, result = 0;
    int threadsNum = 0;

    cout << "step: ";
    cin >> step;
    cout << "threads num: ";
    cin >> threadsNum;

    FirstIntegral fi;
    fi.a = 0.1;
    fi.b = 1.1;

    SecondIntegral si;
    si.a = 0.3;
    si.b = 1.3;

    cout << endl;
    file.open ("../firstIntegral.txt");
    result = getIntegralVal(fi, step, threadsNum);
    cout << "first integral result: " << result << endl;
    file << "first integral result: " << result << endl;
    cout << endl;
    file.close();

    file.open ("../secondIntegral.txt");
    result = getIntegralVal(si, step, threadsNum);
    cout << "second integral result: " << result << endl;
    file << "second integral result: " << result << endl;
    cout << endl;
    file.close();

    return 0;
}
