#include <iostream>

typedef long long ll;

// exp=d0​+d1​⋅8+d2​⋅82+...
// base^exp = (base ^ d0) . (base ^ 8.d1) . (base ^ 64.d2) . .....
// where 0<=di<=7


ll octalExp(ll base, ll exp) {
    ll result = 1;

    while (exp > 7) {
        int digit = base & 7;
        ll b2 = base * base;
        ll b4 = b2 * b2; 
        switch (digit) {
            case 1: result *= base; break;
            case 2: result *= b2; break;
            case 3: result *= (b2 * base); break;
            case 4: result *= b4; break;
            case 5: result *= (b4 * base); break;
            case 6: result *= (b4 * b2); break;
            case 7: result *= (b4 * b2 * base); break;
            default: break;
        }
        exp >>= 3;
        base = (b4 * b4);
    }

    {
        int digit = exp & 7;
        switch (digit) {
            case 1: result *= base; break;
            case 2: result *= (base * base); break;
            case 3: result *= (base * base * base); break;
            case 4: {
                ll b2 = base * base;
                result *= (b2 * b2);
                break;
            }
            case 5: {
                ll b2 = base * base;
                result *= (b2 * b2 * base);
                break;
            }
            case 6: {
                ll b3 = base * base * base;
                result *= (b3 * b3);
                break;
            }
            case 7: {
                ll b3 = base * base * base;
                result *= (b3 * b3 * base);
                break;
            }
            default: break;
        }
    }

    return result;
}


int main() {

    std::cout<<octalExp(2, 10)<<std::endl;

    return 0;
}