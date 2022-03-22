#include <iostream>
#include <set>
std::set<int> *GetRandomSet(int n_tracks_in_event, std::set<int> &base)
{
    auto set = new std::set<int>;
    while (set->size() < n_tracks_in_event)
    {
        auto it(base.begin());
        advance(it, rand() % base.size());
        set->insert(*it);
    }
    return set;
}
void test()
{
    std::set<int> base;
    for (int i = 0; i < 100; i++)
    {
        base.insert(i);
    }

    for (int i = 0; i < 1000; i++)
    {
        auto a = GetRandomSet(2, base);
        for (auto num : *a)
        {
            std::cout << num << "\t";
        }

        std::cout << "\n"
                  << a->size() << "\n";
    }
}