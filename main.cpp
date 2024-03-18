#include <iostream>
#include <cmath>
#include <random>

using namespace std;
// константы же типа double? их рандомными задавать?
typedef class Node
{
public:
    char type;
    int operation;
    int variable;
    double constant;
    Node* left;
    Node* mid;
    Node* right;

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
    int AddNode(int type, int number, double value, int depth, int* flag, int d)
    {
        AddNode(type, number, value, depth, flag, d, root);
        if (*flag == 0)
            return 0;
        if (*flag == 1)
            return 1;
    }

    void AddNode(int type, int number, double value, int depth, int* flag, int d, Node*& cur_el)
    {
        // условия вставки в дерево, чтобы в листьях оказались только переменные или константы
        if(cur_el == NULL)
        {
            if(d<depth)
            {
                if (type=='o'||type=='v')
                {
                    cur_el = new Node(type, number);
                    *flag = 1;
                    return;
                }
                else if(type=='c')
                {
                    cur_el = new Node(type, value);
                    *flag = 1;
                    return;
                }
            }
            else if(d==depth)
            {
                if (type=='v')
                {
                    cur_el = new Node(type, number);
                    *flag = 1;
                    return;
                }
                else if(type=='c')
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
        else if(cur_el->type=='o')
        {
            if(cur_el->operation < 2000)
            {
                AddNode(type, number, value, depth, flag, d+1, cur_el->left);
                if(*flag==1){return;}
            }
            else if(cur_el->operation >= 2000 && cur_el->operation < 3000)
            {
                AddNode(type, number, value, depth, flag, d+1, cur_el->left);
                if(*flag==1){return;}
                AddNode(type, number, value, depth, flag, d+1, cur_el->right);
                if(*flag==1){return;}
            }
            else if(cur_el->operation >= 3000)
            {
                AddNode(type, number, value, depth, flag, d+1, cur_el->left);
                if(*flag==1){return;}
                AddNode(type, number, value, depth, flag, d+1, cur_el->right);
                if(*flag==1){return;}
                AddNode(type, number, value, depth, flag, d+1, cur_el->mid);
                if(*flag==1){return;}
            }
        }
        else if(cur_el->type=='v'||cur_el->type=='c'){return;}
    }

    // читается дерево в глубину по правилу левый-корень-правый
    void PrintTree()
    {
        PrintTree(root);
    }
    void PrintTree(Node* cur_el)
    {
        if(cur_el==NULL){return;}
        PrintTree(cur_el->left);

        if(cur_el->type=='o')
            cout << cur_el->type<< " " << cur_el->operation << endl;
        else if(cur_el->type=='v')
            cout << cur_el->type<< " " << cur_el->variable << endl;
        else if(cur_el->type=='c')
            cout << cur_el->type<< " " << cur_el->constant << endl;
        PrintTree(cur_el->mid);
        PrintTree(cur_el->right);
    }

    ~Tree()
    {
        ClearTree(root);
    }

    void ClearTree(Node* node) {
        if (node != nullptr) {
            ClearTree(node->left);
            ClearTree(node->mid);
            ClearTree(node->right);
            delete node;
        }
    }
};

double generate_normal() {
    double mean = 0, stddev = 1;
    static random_device rd;
    static mt19937 gen(rd());
    normal_distribution<double> dist(mean, stddev);
    return dist(gen);
}

void synthetic_data(double* x, double* y, int num_obs)
{
    int i;
    for(i = 0; i < num_obs; i++)
    {
        x[i] = 0.01*i;
        y[i] = 10*x[i] + sin(x[i]);
    }
}

// заполняется дерево с верхушки, потом влево, потом вправо, потом в середину
void init_population(int switch_init, int n, int unar_oper, int bin_oper, int trin_oper, Tree* tree, char* type, int depth, int num_var)
{
    int success, i, random_opervar, random_type, random_oper, flag = 0;
    double random_const;
    if(switch_init == 0)
    {
        for(i = 0; i < n; i++)
        {
            success = 1;
            while (success == 1)
            {
                flag = 0;
                random_oper = rand() % 2;//if..then..else пока не использую;
                if(random_oper == 0)
                {
                    random_opervar = (rand() % unar_oper)+1000;
                }
                else if(random_oper == 1)
                {
                    random_opervar = (rand() % bin_oper)+2000;
                }
                success = tree[i].AddNode(type[0], random_opervar, 0, depth-1, &flag, 0);
            }
            success = 1;
            while (success == 1)
            {
                flag = 0;
                random_type = 1+rand() % 2;
                if(random_type == 1)
                {
                    random_opervar = rand() % num_var;
                    success = tree[i].AddNode(type[random_type], random_opervar, 0, depth-1, &flag, 0);
                }
                else if(random_type == 2)
                {
                    random_const = generate_normal();
                    success = tree[i].AddNode(type[random_type], 0, random_const, depth-1, &flag, 0);
                }
            }
        }
    }
    if(switch_init == 1)// возможно нужно сделать не равные шансы выпадания элементов из функционально и терминального множеств
        //мне кажется у функционального должно быть больше шансов
    {
        for(i = 0; i < n; i++)
        {
            success = 1;
            while (success == 1)
            {
                flag = 0;
                random_type = rand() % 3;
                if(random_type == 0)
                {
                    random_oper = rand() % 2;//if..then..else пока не использую;
                    if(random_oper == 0)
                    {
                        random_opervar = (rand() % unar_oper)+1000;
                    }
                    else if(random_oper == 1)
                    {
                        random_opervar = (rand() % bin_oper)+2000;
                    }
                    success = tree[i].AddNode(type[0], random_opervar, 0, depth-1, &flag, 0);
                }
                else if(random_type == 1)
                {
                    random_opervar = rand() % num_var;
                    success = tree[i].AddNode(type[random_type], random_opervar, 0, depth-1, &flag, 0);
                }
                else if(random_type == 2)
                {
                    random_const = generate_normal();
                    success = tree[i].AddNode(type[random_type], 0, random_const, depth-1, &flag, 0);
                }
            }
            success = 1;
            while (success == 1)
            {
                flag = 0;
                random_type = 1+rand() % 2;
                if(random_type == 1)
                {
                    random_opervar = rand() % num_var;
                    success = tree[i].AddNode(type[random_type], random_opervar, 0, depth-1, &flag, 0);
                }
                else if(random_type == 2)
                {
                    random_const = generate_normal();
                    success = tree[i].AddNode(type[random_type], 0, random_const, depth-1, &flag, 0);
                }
            }
        }
    }
}

/*void count_f()
{
    double f_temp;
    if(cur_el==NULL){return;}
    PrintTree(cur_el->left);

    if(cur_el->type=='o')

    else if(cur_el->type=='v')
        cout << cur_el->type<< " " << cur_el->variable << endl;
    else if(cur_el->type=='c')
        cout << cur_el->type<< " " << cur_el->constant << endl;
    PrintTree(cur_el->mid);
    PrintTree(cur_el->right);
}*/

int main()
{
    int i, j, num_obs = 400, unar_oper = 3, bin_oper = 5, trin_oper = 1, num_var = 1, num_types = 3, depth = 3, flag=0;
    // 1000sin, 1001cos, 1002exp, 2000log, 2001+, 2002-, 2003*, 2004/, 3000if..then..else
    // 0x,
    // flag нужен для того, чтобы понимать добавился ли элемент в дерево, d контролирует глубину строящегося дерева
    int n = 10, num_generals=10;
    //n - количество индивидов в поколении, num_generals - количество поколений
    int switch_init = 1;
    //0 - полный метод, 1 - метод выращивания
    double* x = new double [num_obs];
    double* y = new double [num_obs];
    char* type = new char[num_types];

    type[0] = 'o';
    type[1] = 'v';
    type[2] = 'c';
    synthetic_data(x, y, num_obs);

    Tree* tree = new Tree[n];

    init_population(switch_init, n, unar_oper, bin_oper, trin_oper, tree, type, depth, num_var);
    for(i = 0; i < n; i++)
    {
        cout << "Tree " << i <<endl;
        tree[i].PrintTree();
    }


    delete[] x;
    delete[] y;
    delete[] type;
}
// нужно спросить как преобразовать дерево в математическое выражение, про прототипы спросить, может есть способ убрать повторяющийся код
// выяснить что меняется если убрать те условия, константы нулями стали
