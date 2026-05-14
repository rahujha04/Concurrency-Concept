#include <iostream>

typedef long long ll;


ll binaryExp(ll a, ll n) {
    ll result = 1;

    while (n) {
        if (n&1)
            result *= a;
        a *= a;
        n>>=1;
    }

    return result;
}


int main() {

    std::cout<<binaryExp(2, 10)<<std::endl;

    return 0;
}