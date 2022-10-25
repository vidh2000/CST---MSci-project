#include <algorithm>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <numeric>
#include <fstream>
#include <stack>
#include <string>
#include <stdio.h>
#include <vector>
#include <set>

void print_set(std::set<int> set)
{
    for (int const& e : set)
    {
        std::cout << e << ' ';
    }
}
bool set_contains(int element, std::set<int> s)
{
    bool is_in = s.find(element) != s.end();
    return is_in;
}


//for set containing int only here
std::set<int> set_difference(std::set<int> s1, std::set<int> s2)
{
    std::set<int> result;
    std::set_difference(s1.begin(), s1.end(), s2.begin(), s2.end(),
        std::inserter(result, result.end()));
    return result;
}

int main(){
std::set<int> a = {1,2,3,4};
std::set<int> b = {3,4,5};
a = set_difference(a,b);
print_set(a);

}

