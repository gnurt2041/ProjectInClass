// g++ ./fftomp.cpp -o ./fftomp -fopenmp
// ./fftomp

#include <complex>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <chrono>
#include <algorithm>
#include <string>

#include "omp.h"

using namespace std;

typedef complex<double> cplx;
typedef vector<cplx> v_clpx;
const double PI = acos(-1);

void fft_slow(long long n, v_clpx& a) // biến đổi de quy fft của vector a
{
    if(n == 1)
    {
        return;
    }
    long long i, j, k;
    v_clpx a_even(n/2), a_odd (n/2);

    for (long long i = 0; 2 * i < n; i++) {
        a_even[i] = a[2*i];
        a_odd[i] = a[2*i+1];
    }

    fft_slow(a_even.size(), a_even);
    fft_slow(a_odd.size(), a_odd);

    for(i = 0; i < n / 2; i++)
    {
        cplx t = exp(cplx(0, -2 * PI * i / n)) * a_odd[i];
		a[i]  = a_even[i] + t;
		a[i + n / 2] = a_even[i] - t;
    }
}

long long bit_revert(long long nbit, long long mask)
{
    long long i, j;
    for(i = 0, j = nbit - 1; i <= j; ++i, --j)
    {
        if( (mask >> i & 1) != (mask >> j & 1) )
        {
            mask ^= 1<<i;
            mask ^= 1<<j;
        }
    }
    return mask;
}

// Biến đổi FFT của mảng a
void fft_improved(long long n, v_clpx& a)
{
    if(n == 1)
        return;
    long long i, j, tmp;
    cplx t1;

    long long nbit = 0;
    while ((1 << nbit) < n)
        ++nbit;

    for(i = 0; i < n; i++)
    {
        j= bit_revert(nbit,i);
        if(i < j) { 
            swap(a[i], a[j]); 
        }
    }
    cplx* W = new cplx[n]();
    v_clpx anext(n); 

    for(i = 0; i < n; i++){
        double angle = 2*PI*i/n;
        angle -= floor(angle / (2*PI)) * (2*PI);  // Reduce angle modulo 2*PI
        W[i] = cplx(cos(angle),-sin(angle));
    }
    for(long long step = 1; step < n; step <<= 1) // step = step * 2
    {
        tmp = step << 1;
        long long start_even = 0;
        for (start_even=0; start_even < n; start_even += tmp)
        {
            for(i = 0; i < step; i++)
            {
                t1 = W[(n*i)/tmp] * a[start_even + step + i];
                anext[start_even + i]        = a[start_even + i] + t1;
                anext[start_even + i + step] = a[start_even + i] - t1;
            }
        }
        for(i = 0; i < n; i++)
            a[i] = anext[i];
    }
    delete[] W;
    anext.clear();
}

// Biến đổi // FFT của mảng a
void fft_parallel(long long n, v_clpx &a)
{
    if (n == 1)
        return;
    long long i,j,k,tmp;
    cplx t1;

    long long nbit = 0;
    while ((1 << nbit) < n)
        ++nbit;
    #pragma omp parallel for shared(nbit,a) private(j)
    for (i = 0; i < n; ++i)
    {
        j = bit_revert(nbit, i);
        #pragma omp critical
        if (i < j)
        {
            swap(a[i], a[j]);
        }
    }
    cplx *W = new cplx[n];
    v_clpx anext(n); 
    
    #pragma omp parallel for shared(W,n)
    for(i = 0; i < n; ++i){
        double angle = 2*PI*i/n;
        angle -= floor(angle / (2*PI)) * (2*PI);  // Reduce angle modulo 2*PI
        W[i] = cplx(cos(angle),-sin(angle));
    }

    for (long long step = 1; step < n; step <<=1) // step = step * 2
    {
        tmp = step << 1;
        long long start_even = 0;
        #pragma omp parallel private(start_even,i,t1,k) 
        {
            #pragma omp for nowait schedule(static)
            for (start_even=0; start_even < n; start_even += tmp)
            {
                for (i = 0; i < step; ++i)
                {
                    k = (n * i) / tmp;
                    t1 = W[k] * a[start_even + step + i];
                    anext[start_even + i]        = a[start_even + i] + t1;
                    anext[start_even + i + step] = a[start_even + i] - t1;
                }
            }
        }
        
        #pragma omp parallel
        {
            #pragma omp for nowait
            for (i = 0; i < n; ++i)
                a[i] = anext[i];
        }
       
    }

    delete[] W;
    anext.clear();
}

double compare(v_clpx& v1, v_clpx& v2) {

    // Calculate the squared distances between all pairs
    vector<double> squaredDistances;
    for (long long i = 0; i < v1.size(); i++) {
        cplx diff = v1[i] - v2[i];
        double squaredDistance = diff.real() * diff.real() + diff.imag() * diff.imag();
        squaredDistances.push_back(squaredDistance);
    }

    // Find the maximum squared distance
    double maxSquaredDistance = *max_element(squaredDistances.begin(), squaredDistances.end());

    // Calculate and return the maximum distance
    return sqrt(maxSquaredDistance);
}


int main()
{
    long long n;
    cout<<"N: ";
    cin >> n;
    cout << "Number of inputs (2^N): " << pow(2,n) << endl;
    int range_threads;
    cout<<"Range of threads: ";
    cin >> range_threads;
    static v_clpx a(pow(2,n)),b(pow(2,n)),c(pow(2,n)),distances(pow(2,n)),temp(pow(2,n));
    for(long long i=0;i<pow(2,n);i++)
    {
        // cin>>a[i];
        // a[i] = i;
        a[i] = rand()%100;
        b[i] = a[i];
        c[i] = a[i];
        temp[i] = a[i];

    }
    auto begin = chrono::steady_clock::now();
    fft_slow(a.size(),a);
    auto end = chrono::steady_clock::now();

    cout << "(Recursive) Elapsed time in microseconds: "
        << chrono::duration_cast<chrono::microseconds>(end - begin).count()
        << " µs" << endl;
    double time_fft = chrono::duration_cast<chrono::microseconds>(end - begin).count();

    begin = chrono::steady_clock::now();
    fft_improved(b.size(),b);
    end = chrono::steady_clock::now();

	cout << "(No recursive) Elapsed time in microseconds: "
        << chrono::duration_cast<chrono::microseconds>(end - begin).count()
        << " µs" << endl;
    double time_fft_improved = chrono::duration_cast<chrono::microseconds>(end - begin).count();
    ofstream result ("result.txt");
    cout << "Thread             Time             Speedup(vs non-recursive)    Speedup(vs recursive)    Correct" << endl;
    for(int threads = 1; threads <= range_threads; threads++){
        
        omp_set_num_threads(threads);
        begin = chrono::steady_clock::now();
        fft_parallel(c.size(),c);
        end = chrono::steady_clock::now();
        double time_fft_parallel = chrono::duration_cast<chrono::microseconds>(end - begin).count();

        string correct;
        if(b == c &&  round(compare(a,c)) == 0){
            correct = "True";
        }
        else correct = "False";
        cout << "   " << threads << "         " <<  scientific << time_fft_parallel << "(µs)" << fixed << "              " 
            << time_fft_improved/time_fft_parallel << "                   " << time_fft/time_fft_parallel << "              " 
            << correct << endl;
        c = temp;
    }
    a.clear();
    b.clear();
    c.clear();
    distances.clear();
    temp.clear();
    return 0;
}