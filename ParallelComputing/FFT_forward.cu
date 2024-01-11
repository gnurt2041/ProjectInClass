// nvcc -gencode arch=compute_50,code=sm_50 FFT_forward.cu -o FFT_forward
// ./FFT_forward
#include <bits/stdc++.h>
#include <chrono>


using namespace std::chrono;
using namespace std;

typedef complex<double> base;
typedef double2 Complex_my;
#define PI acos(-1.0)

/**
* Parallel Functions for performing various tasks
*/

static __device__ __host__ inline Complex_my Add(Complex_my A, Complex_my B)
{
    Complex_my C;
    C.x = A.x + B.x;
    C.y = A.y + B.y;
    return C;
}

/**
 *  Inverse of Complex_my Number
 */
static __device__ __host__ inline Complex_my Inverse(Complex_my A)
{
    Complex_my C;
    C.x = -A.x;
    C.y = -A.y;
    return C;
}

/**
 *  Multipication of Complex_my Numbers
 */
static __device__ __host__ inline Complex_my Multiply(Complex_my A, Complex_my B)
{
    Complex_my C;
    C.x = A.x * B.x - A.y * B.y;
    C.y = A.y * B.x + A.x * B.y;
    return C;
}

/**
* Reorders array by bit-reversing the indexes.
*/
__global__ void bitrev_reorder(Complex_my *__restrict__ r, Complex_my *__restrict__ d, long s, size_t nthr, long n)
{
    long id = blockIdx.x * nthr + threadIdx.x;
    //r[id].x = -1;
    if (id < n and __brev(id) >> (32 - s) < n)
        r[__brev(id) >> (32 - s)] = d[id];
}

/**
* Inner part of the for loop
*/
__device__ void inplace_fft_inner(Complex_my *__restrict__ A, long i, long j, long len, long n)
{
    if (i + j + len / 2 < n and j < len / 2)
    {
        Complex_my u, v;

        float angle = (2 * M_PI * j) / len;
        v.x = cos(angle);
        v.y = -sin(angle);

        u = A[i + j];
        v = Multiply(A[i + j + len / 2], v);
        // prlongf("i:%d j:%d u_x:%f u_y:%f    v_x:%f v_y:%f\n", i, j, u.x, u.y, v.x, v.y);
        A[i + j] = Add(u, v);
        A[i + j + len / 2] = Add(u, Inverse(v));
    }
}

/**
* FFT if number of threads are sufficient.
*/
__global__ void inplace_fft(Complex_my *__restrict__ A, long i, long len, long n, long threads)
{
    long j = blockIdx.x * threads + threadIdx.x;
    inplace_fft_inner(A, i, j, len, n);
}

/**
* FFt if number of threads are not sufficient.
*/
__global__ void inplace_fft_outer(Complex_my *__restrict__ A, long len, long n, long threads)
{
    long i = (blockIdx.x * threads + threadIdx.x)*len;
    for (long j = 0; j < len / 2; j++)
    {
        inplace_fft_inner(A, i, j, len, n);
    }
}
/**
* parallel FFT transform and inverse transform
* Arguments vector of complex numbers, invert, balance, number of threads
* Perform inplace transform
*/
void fft(vector<base> &a, bool invert, long balance = 10, long threads = 32)
{
    // Creating array from vector
    long n = (long)a.size();
    long data_size = n * sizeof(Complex_my);
    Complex_my *data_array = (Complex_my *)malloc(data_size);
    for (long i = 0; i < n; i++)
    {
        data_array[i].x = a[i].real();
        data_array[i].y = a[i].imag();
    }

    // Copying data to GPU
    Complex_my *A, *dn;
    cudaMalloc((void **)&A, data_size);
    cudaMalloc((void **)&dn, data_size);
    cudaMemcpy(dn, data_array, data_size, cudaMemcpyHostToDevice);
    // Bit reversal reordering
    long s = log2(n);

    bitrev_reorder<<<ceil(float(n) / threads), threads>>>(A, dn, s, threads, n);

    // Synchronize
    cudaDeviceSynchronize();
    // Iterative FFT with loop parallelism balancing
    for (long len = 2; len <= n; len <<= 1)
    {
        if (n / len > balance)
        {

            inplace_fft_outer<<<ceil((float)n / threads / len), threads>>>(A, len, n, threads);
        }
        else
        {
            for (long i = 0; i < n; i += len)
            {
                float repeats = len / 2;
                inplace_fft<<<ceil(repeats / threads), threads>>>(A, i, len, n, threads);
            }
        }
    }

    // Copy data from GPU
    Complex_my *result;
    result = (Complex_my *)malloc(data_size);
    cudaMemcpy(result, A, data_size, cudaMemcpyDeviceToHost);

    // Saving data to vector<complex> in input.
    for (long i = 0; i < n; i++)
    {
        a[i] = base(result[i].x, result[i].y);
    }
    // Free the memory blocks
    free(data_array);
    cudaFree(A);
    cudaFree(dn);
    return;
}


void fft_slow(vector<base> & a) // biến đổi de quy fft của vector a
{
    long n = a.size();
    if(n == 1)
    {
        return;
    }
    long i;
    vector<base>  a_even(n/2), a_odd (n/2);

    for (long i = 0; 2 * i < n; i++) {
        a_even[i] = a[2*i];
        a_odd[i] = a[2*i+1];
    }

    fft_slow(a_even);
    fft_slow(a_odd);

    for(i = 0; i < n / 2; i++)
    {
        base t = exp(base(0, -2 * PI * i / n)) * a_odd[i];
		a[i]  = a_even[i] + t;
		a[i + n / 2] = a_even[i] - t;
    }
}

double compare(vector<base>& v1, vector<base>& v2) {

    // Calculate the squared distances between all pairs
    vector<double> squaredDistances;
    for (long i = 0; i < v1.size(); i++) {
        base diff = v1[i] - v2[i];
        // double squaredDistance = diff.real() * diff.real() + diff.imag() * diff.imag();
        double squaredDistance  = round(abs(diff)/abs(v2[i]));
        squaredDistances.push_back(squaredDistance);
    }

    // Find the maximum squared distance
    double maxSquaredDistance = *max_element(squaredDistances.begin(), squaredDistances.end());

    // Calculate and return the maximum distance
    // return sqrt(maxSquaredDistance);
    return maxSquaredDistance;
}

// #define N 100000
// #define BALANCE 2

int main()
{
    long min_power, max_power;
    long BALANCE = 1024;
    string write_to_file;
    string save_csv;
    cout << "Enter min-max power of 2 (min -> max): ";
    cin >> min_power >> max_power;
    cout << "Write to file (y/n): ";
    cin >> write_to_file;

    cout << "Save to csv (y/n): ";
    cin >> save_csv;
    
    if(write_to_file == "y" || write_to_file == "Y"){
        freopen("output.txt", "w", stdout);
    }

    for (long power = min_power; power <= max_power; power+=1)
    {
        long N = pow(2, power);
        ofstream result(to_string(N) + "result.csv");
        result << "Cores,Parallel,Sequential,Speedup,Correct" << endl;
        cout << "Runing with N = " << N << endl;
        vector<long> a(N);
        for(long i = 0; i < N; i++)
        {
            a[i] = rand() % 100;
        }

        // For Sequential
        vector<base> fa_serial(a.begin(), a.end());
        auto begin = chrono::steady_clock::now();
        fft_slow(fa_serial);
        auto end = chrono::steady_clock::now();
        double time_fft_serial = chrono::duration_cast<chrono::microseconds>(end - begin).count();
        cout << "# of Cores    Parallel             Sequential       Speedup    Correct" << endl;
        long max_length = to_string(1024).length();
        for(long threads = 1; threads <= 1024; threads*=2)
        {
            /// For Parallel
            begin = chrono::steady_clock::now();
            vector<base> fa_paralel(a.begin(), a.end());
            fft(fa_paralel, BALANCE, threads);
            end = chrono::steady_clock::now();
            double time_fft_parallel = chrono::duration_cast<chrono::microseconds>(end - begin).count();

            string thread_str = to_string(threads);
            while(thread_str.length() < max_length){
                thread_str = ' ' + thread_str;
            }

            string correct;
            if(round(compare(fa_paralel,fa_serial)) == 0){
                correct = "True";
            }
            else correct = "False";
            cout << "   " <<  thread_str << "      " << scientific << time_fft_parallel << "(µs)" << "   " << scientific << time_fft_serial << "(µs)"
                << "   " << fixed << time_fft_serial/ time_fft_parallel
                << "     " << correct << endl;
            // cout << endl;
            if(save_csv == "y" || save_csv == "Y"){
                result << threads << "," << time_fft_parallel << "," << time_fft_serial << "," << time_fft_serial/ time_fft_parallel << "," << correct << endl;
            }
            if(correct == "False")
            {
                for(long i = 0; i < N; i++){
                    base diff = fa_paralel[i] - fa_serial[i];
                    cout << fa_paralel[i]  << " | " << fa_serial[i] << " -> " << round(abs(diff)/abs(fa_serial[i])) << endl;
                }
            }

        }
    }
    return 0;
}