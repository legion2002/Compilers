#include <iostream>
#include <memory>
#include <unistd.h>

// compile: g++ -std=c++14 -O3 unique.cpp -o unique

static int numinserts = 1000000;

struct node
{
    int val;
    std::shared_ptr<struct node> next;
};

static unsigned long long rdtsc()
{
    unsigned hi, lo;
    asm volatile("rdtsc" : "=a"(lo), "=d"(hi));
    return (((unsigned long long)hi) << 32) | lo;
}

int delete_node(std::shared_ptr<struct node>& root, int idx) {
    if (root == nullptr || idx < 0) return 0;
    if (idx == 0) {
        root = root->next;
        return 1;
    }
    std::shared_ptr<struct node> curr = root;
    for (int i = 0; i < idx - 1 && curr->next; i++) {
        curr = curr->next;
    }
    if (curr->next == nullptr) return 0;
    curr->next = curr->next->next;
    return 1;
}

int iterate_list(const std::shared_ptr<struct node>& root) {
    if (root == nullptr) return 0;
    int ret = 0;
    std::shared_ptr<struct node> curr = root;
    while (curr) {
        ret++;
        curr = curr->next;
    }
    return ret;
}

int insert_nodes(std::shared_ptr<struct node> &root)
{
    std::shared_ptr<struct node> elem(new struct node);
    if (elem == nullptr)
        return 0;
    elem->val = 0;
    elem->next = root;
    root = elem;
    return 1;
}

int main(int argc, const char *argv[])
{
    std::shared_ptr<struct node> head = nullptr;
    int numelems;
    unsigned long long time;
    if (argc == 2)
    {
        numinserts = atoi(argv[1]);
    }
    printf("numinserts : %d\n", numinserts);
    if (numinserts < 0)
        return 0;

    time = rdtsc();
    for (int i = 0; i < numinserts; i++)
    {
        if (!insert_nodes(head))
        {
            std::cout << "failed to insert node : " << i << std::endl;
            return 0;
        }
    }
    printf("time taken during insert: %lld ms\n", (rdtsc() - time) / 2530000);

    numelems = iterate_list(head);
    if (numelems != numinserts)
    {
        printf("test failed! %d %d\n", numelems, numinserts);
    }
    time = rdtsc();
    for (int i = 0; i < numinserts / 20; i++)
    {
        int idx = rand() % ((numinserts / 20) - i);
        if (!delete_node(head, idx))
        {
            std::cout << "failed to delete node : " << idx << std::endl;
            return 0;
        }
    }
    printf("time taken during delete: %lld ms\n", (rdtsc() - time) / 2530000);

    numelems = iterate_list(head);
    if (numelems != numinserts - (numinserts / 20))
    {
        printf("test failed! %d %d\n", numelems, numinserts);
    }

    time = rdtsc();
    head = nullptr;
    printf("time taken during cleanup: %lld ms\n", (rdtsc() - time) / 2530000);

    return 0;
}

// Source: 
// * used LLMs for help with common C++ syntax and functions