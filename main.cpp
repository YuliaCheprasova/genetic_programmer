#include <iostream>
#include <cmath>

using namespace std;
// константы же типа double? их рандомными задавать?
typedef class Node
{
public:
    char type;
    int operation;
    int variable;
    double constant;
    struct Node* left;
    struct Node* mid;
    struct Node* right;

    Node(char type, int number)
    {
        this->type = type;
        if(type == 'o')
        {
            this->operation = number;
            this->variable = NULL;
        }
        if(type == 'v')
        {
            this->variable = number;
            this->operation = NULL;
        }
        this->constant = NULL;
        this->left = NULL;
        this->mid = NULL;
        this->right = NULL;
    }
    Node(char type, double constant)
    {
        this->type = type;
        this->operation = NULL;
        this->variable = NULL;
        this->constant = constant;
        this->left = NULL;
        this->mid = NULL;
        this->right = NULL;
    }
} Node;

class Tree
{
public:
    Node* root;
    Tree()
    {
        this->root = NULL;
    }
    // без прототипа не смогла
    int AddNode(int type, int value, int unar_oper, int bin_oper, int depth, int* flag, int d)
    {
        AddNode(type, value, unar_oper, bin_oper, depth, flag, d, root);
        if (*flag == 0)
            return 0;
        if (*flag == 1)
            return 1;
    }
    int AddNode(int type, double value, int unar_oper, int bin_oper, int depth, int* flag, int d)
    {
        AddNode(type, value, unar_oper, bin_oper, depth, flag, d, root);
        if (*flag == 0)
            return 0;
        if (*flag == 1)
            return 1;
    }
    void AddNode(int type, int value, int unar_oper, int bin_oper, int depth, int* flag, int d, Node*& cur_el)
    {
        // условия вставки в дерево, чтобы в листьях оказались только переменные или константы
        if(cur_el == NULL)
        {
            if(d<depth)
            {
                if (type=='o')
                {
                    cur_el = new Node(type, value);
                    *flag = 1;
                    return;
                }
                else if(type=='v'||type=='c'){return;}
            }
            else if(d==depth)
            {
                if (type=='v'||type=='c')
                {
                    cur_el = new Node(type, value);
                    *flag = 1;
                    return;
                }
                else if(type=='o'){return;}
            }
            else if(d>depth){return;}
        }
        // условия, в зависимости от которых определяется сколько дочерних узлов может быть у узла
        else if(cur_el->operation < unar_oper)
        {
            AddNode(type, value, unar_oper, bin_oper, depth, flag, d+1, cur_el->left);
            if(*flag==1){return;}
        }
        else if(cur_el->operation >= unar_oper && cur_el->operation < unar_oper+bin_oper)
        {
            AddNode(type, value, unar_oper, bin_oper, depth, flag, d+1, cur_el->left);
            if(*flag==1){return;}
            AddNode(type, value, unar_oper, bin_oper, depth, flag, d+1, cur_el->right);
            if(*flag==1){return;}
        }
        else if(cur_el->operation >= unar_oper+bin_oper)
        {
            AddNode(type, value, unar_oper, bin_oper, depth, flag, d+1, cur_el->left);
            if(*flag==1){return;}
            AddNode(type, value, unar_oper, bin_oper, depth, flag, d+1, cur_el->right);
            if(*flag==1){return;}
            AddNode(type, value, unar_oper, bin_oper, depth, flag, d+1, cur_el->mid);
            if(*flag==1){return;}
        }
    }
    void AddNode(int type, double value, int unar_oper, int bin_oper, int depth, int* flag, int d, Node*& cur_el)
    {
        // условия вставки в дерево, чтобы в листьях оказались только переменные или константы
        if(cur_el == NULL)
        {
            if(d<depth)
            {
                if (type=='o')
                {
                    cur_el = new Node(type, value);
                    *flag = 1;
                    return;
                }
                else if(type=='v'||type=='c'){return;}
            }
            else if(d==depth)
            {
                if (type=='v'||type=='c')
                {
                    cur_el = new Node(type, value);
                    *flag = 1;
                    return;
                }
                else if(type=='o'){return;}
            }
            else if(d>depth){return;}
        }
        // условия, в зависимости от которых определяется сколько дочерних узлов может быть у узла
        else if(cur_el->operation < unar_oper)
        {
            AddNode(type, value, unar_oper, bin_oper, depth, flag, d+1, cur_el->left);
            if(*flag==1){return;}
        }
        else if(cur_el->operation >= unar_oper && cur_el->operation < unar_oper+bin_oper)
        {
            AddNode(type, value, unar_oper, bin_oper, depth, flag, d+1, cur_el->left);
            if(*flag==1){return;}
            AddNode(type, value, unar_oper, bin_oper, depth, flag, d+1, cur_el->right);
            if(*flag==1){return;}
        }
        else if(cur_el->operation >= unar_oper+bin_oper)
        {
            AddNode(type, value, unar_oper, bin_oper, depth, flag, d+1, cur_el->left);
            if(*flag==1){return;}
            AddNode(type, value, unar_oper, bin_oper, depth, flag, d+1, cur_el->right);
            if(*flag==1){return;}
            AddNode(type, value, unar_oper, bin_oper, depth, flag, d+1, cur_el->mid);
            if(*flag==1){return;}
        }
    }
    void PrintTree()
    {
        PrintTree(root);
    }
    void PrintTree(Node* cur_el)
    {
        if(cur_el==NULL){return;}
        PrintTree(cur_el->left);

        if(cur_el->type=='o')
            cout << cur_el->operation << endl;
        else if(cur_el->type=='v')
            cout << cur_el->variable << endl;
        else if(cur_el->type=='c')
            cout << cur_el->constant << endl;
        PrintTree(cur_el->mid);
        PrintTree(cur_el->right);
    }
};

void synthetic_data(double* x, double* y, int num_obs)
{
    int i;
    for(i = 0; i < num_obs; i++)
    {
        x[i] = 0.01*i;
        y[i] = 10*x[i] + sin(x[i]);
    }
}

int main()
{
    int i, j, num_obs = 400, unar_oper = 3, bin_oper = 5, trin_oper = 1, num_var = 1, num_types = 3, depth = 4, flag=0, success;
    // 0sin, 1cos, 2exp, 3log, 4+, 5-, 6*, 7/, 8if..then..else
    // 0x,
    // flag нужен для того, чтобы понимать добавился ли элемент в дерево, d контролирует глубину строящегося дерева
    int n = 10, num_generals=10, random_opervar, random_type;
    double random_const;
    //n - количество индивидов в поколении, num_generals - количество поколений
    int switch_init = 0;
    //0 - полный метод
    double* x = new double [num_obs];
    double* y = new double [num_obs];
    char* type = new char[num_types];

    type[0] = 'o';
    type[1] = 'v';
    type[2] = 'c';
    synthetic_data(x, y, num_obs);

    Tree* tree = new Tree[n];
    // заполняется дерево с верхушки, потом влево, потом вправо, потом в середину
    if(switch_init == 0)
    {
        for(i = 0; i < n; i++)
        {
            success = 1;
            while (success == 1)
            {
                flag = 0;
                random_opervar = rand() % (unar_oper+bin_oper);
                success = tree[i].AddNode(type[0], random_oper, unar_oper, bin_oper, depth-1, &flag, 0);
            }
            success = 1;
            while (success == 1)
            {
                flag = 0;
                random_type = 1+rand() % 2;
                if(random_type == 1)
                {
                    random_opervar = rand() % num_var;
                    success = tree[i].AddNode(type[random_type], random_opervar, unar_oper, bin_oper, depth-1, &flag, 0);
                }
                if(random_type == 2)
                {
                    random_const = rand() % num_var;//тут узнать и поменять
                    success = tree[i].AddNode(type[random_type], random_const, unar_oper, bin_oper, depth-1, &flag, 0);
                }
            }
        }
    }
    cout << "tree" << endl;
    // читается дерево в глубину по правилу левый-корень-правый
    tree.PrintTree();
    delete[] tree;
    delete[] x;
    delete[] y;
    delete[] type;
}
