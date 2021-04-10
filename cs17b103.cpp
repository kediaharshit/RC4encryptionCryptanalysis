#include <cstddef>
#include <iostream>
#include <vector>
#include <cmath>
using namespace std;

class RC4
{   
private:
    // taken as int, but actually they are 1 byte size, 0 ot 255
    vector <int> key;       
    vector <int> S;
    vector <int> T;
    int keylen;
    int I, J;       // i,j for generating stream keys

public:
    RC4(){}
    RC4(vector <int> k)
    {
        key = k;
        keylen = key.size();
        S = vector <int>(256,0);
        T = vector <int>(256,0);
        I = J = 0;
    }

    void initial_permutation()
    {
        int i, j;
        for(i=0; i<256; i++)
        {
            S[i] = i;               //identity
            T[i] = key[i%keylen];   // extend key
        }
        j=0;
        for(i=0; i<256; i++)
        {
            j = (j+S[i]+T[i])%256;
            swap(S[i], S[j]);       
        }
    }
    int stream_key()                // returns i'th key at i'th call
    {
        I = (I+1)%256;
        J = (J + S[I])%256;
        swap(S[I], S[J]);
        int t = (S[I] + S[J])%256;
        int C = S[t];
        return C;
    }

    void print_S()
    {
        for(int i=0; i<S.size(); i++)
            cout << S[i] << " ";

        cout << endl;
    }
    void print_Key()
    {
        for(int i=0; i<keylen; i++)
            cout << key[i] << " ";
        cout << endl;
    }
};

class Randomness
{
private:
    RC4 sc1, sc2;
    vector<int> key1, key2;
    int bit_diff;               // no of bits that will be diff in 2 instances

public:
    Randomness() {}
    Randomness(int bit_vary)
    {    
        bit_diff = bit_vary;

        key1 = vector <int>(256, 0);        // key of 256 bytes
        for(int i=0; i<256; i++)    
            key1[i] = rand()%256;           // 1 byte each => 0-255

        key2 = key1;

        for(int i=0; i<bit_diff; i++)
        {
            int idx = rand()%256;             // choose a random index from 0-255
            key2[idx] = (key2[idx] ^ (1<<(i%8))); // change exactly 1 bit in that byte
            // cout << idx << endl;
        }
        sc1 = RC4(key1);
        sc2 = RC4(key2);
        sc1.initial_permutation();
        sc2.initial_permutation();
        // sc1.print_S();
        // sc2.print_S();
    }

    vector <int> frequency(int N)           
    {   
        // N -> number of samples generated
        vector <int> counter(256,0);

        vector <int> xored(N,0);
        for(int i=0; i<N; i++)
            xored[i] = (sc1.stream_key()^sc2.stream_key());

        // for(int i=0; i<N; i++)
        //     cout << xored[i] << " ";
        // cout << endl;
        for(int i=0; i<N-1; i++)
        {
            counter[xored[i]]++;
            for(int j=7; j>=0; j--)
            {
                int t1 = (xored[i] & ((1<<j) -1) );    //(1<<j)-1 :  gives j ones on Least Significant bits
                int t2 = (xored[i+1]&(((1<<(8-j)) -1)<<j));    // ((1<<(8-j)) -1)<<j : gives 8-j ones on MSB of a byte
                int val = (t1<<(8-j)) | (t2>>j);
                counter[val]++;
                // cout << t1 << " " << t2 << " " << val << endl;
            }
        }
        counter[xored[N-1]]++;
        return counter;
    }
};


float standard_deviation(vector <int> A)
{
    int n = A.size();
    double mean = 0;
    for(int i=0; i<n; i++)
        mean += (float)A[i];
    mean = mean/(float)n;
    double sd=0;
    for(int i=0; i<n; i++)
        sd += ((float)A[i]-mean)*((float)A[i]-mean);
    sd = sd/(float)n;
    return (float) sqrt(sd);
}

int main()
{

    srand(time(0));                  // seed the rand function
    vector<int> N = {2,4,8,32,128,1024};
    int C = 256;
    for(int i=1; i<=32; i++)
    {
        for(int j=0; j<N.size(); j++)
        {
            Randomness R(i);
            vector <int> counter = R.frequency(N[j]);
            cout << i  << " " << N[j] << " " << (standard_deviation(counter)*C)/(float)(8*N[j]-7) << endl;  
            //8*N-7 = number of sequences of 8 bits
        }
    }
    return 0;
}