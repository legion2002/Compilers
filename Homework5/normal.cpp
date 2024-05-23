#include <iostream>
#include <unistd.h>

// compile: g++ -std=c++14 -O3 normal.cpp -o normal

static int numinserts = 1000000;

struct node
{
    int val;
    struct node *next;
};

static unsigned long long rdtsc()
{
    unsigned hi, lo;
    asm volatile("rdtsc" : "=a"(lo), "=d"(hi));
    return (((unsigned long long)hi) << 32) | lo;
}

int delete_node(struct node **root, int idx)
{
    if (*root == NULL || idx < 0)
        return 0;

    if (idx == 0)
    {
        struct node *tmp = *root;
        *root = (*root)->next;
        delete tmp;
        return 1;
    }

    struct node *rootp = *root;

    for (int i = 0; i < idx - 1 && rootp->next; i++)
    {
        rootp = rootp->next;
    }
    if (rootp->next == NULL)
        return 0;
    struct node *temp = rootp->next;
    rootp->next = rootp->next->next;
    delete temp;
    return 1;
}

int iterate_list(struct node *root)
{
    if (root == NULL)
        return 0;

    struct node *rootp = root;
    int ret = 0;

    while (rootp)
    {
        ret++;
        rootp = rootp->next;
    }
    return ret;
}

int insert_nodes(struct node **root)
{
    struct node *elem;
    elem = new struct node;
    if (elem == NULL)
        return 0;
    elem->val = 0;
    elem->next = *root;
    *root = elem;
    return 1;
}

int main(int argc, const char *argv[])
{
    struct node *head = NULL;
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
        if (!insert_nodes(&head))
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
        if (!delete_node(&head, idx))
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
    struct node *rootp = head;
    while (rootp)
    {
        struct node *tmp = rootp;
        rootp = rootp->next;
        delete tmp;
    }
    head = NULL;
    printf("time taken during cleanup: %lld ms\n", (rdtsc() - time) / 2530000);

    return 0;
}

// Source:
// * used LLMs for help with common C++ syntax and functions