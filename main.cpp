#include <iostream>
#include <cmath>
#include <random>
#include <ctime>
#include <climits>

using namespace std;
//глобальные переменные
double MAX = std::numeric_limits<double>::max();
double MIN = std::numeric_limits<double>::min();
double mean = 0, stddev = 1;
static random_device rd;
static mt19937 gen(rd());
normal_distribution<double> dist(mean, stddev);
uniform_real_distribution<double> dis(1.0, 100.0);
int unar_oper = 6, bin_oper = 6, trin_oper = 1, num_var = 3, counter = -1, max_nodes = 100;
// 1000sin, 1001cos, 1002exp, 1003ln, 1004 1/x, 1005-выраж, 2000log, 2001+, 2002-, 2003*, 2004/, 2005a^x, 3000if..then..else
// 0x,
char type[3]{ 'o', 'v', 'c' };

double generate_normal() {return dist(gen);}

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
    int num_nodes;
    Tree()
    {
        this->root = NULL;
        this->num_nodes = 0;
    }

    ~Tree();

    void ClearTree(Node* node);

    void CloneTree(Node* origin_root);

    int CountVar();

    void UpNumNodes();

    void UpNumNodes(Node* cur_el);

    int CountDepth(Node* cur_el);

    int AddNode(int type, int number, double value, int depth, int* flag, int d);

    void AddNode(int type, int number, double value, int depth, int* flag, int d, Node*& cur_el);

    // читается дерево в глубину по правилу левый-корень-правый
    void PrintTree();

    void PrintTree(Node* cur_el);

    // деление на ноль, основание логарифма должно быть больше 0 и не должно быть равно единицы, аргумент логарифма тоже > 0
    double evaluateExpression(double* x);

    double evaluateExpression(double* x, Node* cur_el);

    string printExpression();

    string printExpression(Node* cur_el);

    double error(int num_obs, double** x, double* y);

    double count_fitness(double error);

    void Growth(int switch_init, int depth);

    void PointMut(Node* cur_el);

    void PartMut(int switch_init, Node* cur_el, int depth, int* flag, bool check_root);

    void StandCross(Node* cur_el, int splitter, Node*& cut_tree, bool cut);
};

void copying(Node* origin_tree, Node*& dupl_tree)
{
    if(origin_tree==NULL){return;}
    if (origin_tree->type == 'o')
    {
        dupl_tree = new Node(origin_tree->type, origin_tree->operation);
    }
    else if (origin_tree->type == 'v')
    {
        dupl_tree = new Node(origin_tree->type, origin_tree->variable);
    }
    else if (origin_tree->type == 'c')
    {
        dupl_tree = new Node(origin_tree->type, origin_tree->constant);
    }
    copying(origin_tree->left, dupl_tree->left);
    copying(origin_tree->right, dupl_tree->right);
    copying(origin_tree->mid, dupl_tree->mid);
}

Tree::~Tree()
{
    ClearTree(root);
}

void Tree::ClearTree(Node* node)
{
    if (node != nullptr)
    {
        ClearTree(node->left);
        ClearTree(node->mid);
        ClearTree(node->right);
        delete node;
        //num_nodes -= 1;
        if (node == root)//не уверена, что это правильно попробую пока так
            root = nullptr;
    }
}

//использовать только при полном копировании!!
void Tree::CloneTree(Node* origin_root)
{
    if(this->root)
    {
        ClearTree(this->root);
    }
    if (origin_root->type == 'o')
    {
        this->root = new Node(origin_root->type, origin_root->operation);
    }
    else if (origin_root->type == 'v')
    {
        this->root = new Node(origin_root->type, origin_root->variable);
    }
    else if (origin_root->type == 'c')
    {
        this->root = new Node(origin_root->type, origin_root->constant);
    }
    copying(origin_root->left, this->root->left);
    copying(origin_root->right, this->root->right);
    copying(origin_root->mid, this->root->mid);
}

int Tree::CountVar()
{
    int current_num_var;
    string expression = printExpression();
    if (expression.find("x0") != std::string::npos)
        current_num_var++;
    if (expression.find("x1") != std::string::npos)
        current_num_var++;
    if (expression.find("x2") != std::string::npos)
        current_num_var++;
    return current_num_var;
}

void Tree::UpNumNodes()
{
    num_nodes = 0;
    UpNumNodes(root);
}

void Tree::UpNumNodes(Node* cur_el)
{
    if(cur_el==NULL){return;}
    num_nodes++;
    UpNumNodes(cur_el->left);
    UpNumNodes(cur_el->right);
    UpNumNodes(cur_el->mid);
}

int Tree::CountDepth(Node* cur_el)
{
    if (cur_el==NULL) return 0;
    int leftDepth = CountDepth(cur_el->left);
    int rightDepth = CountDepth(cur_el->right);
    int middleDepth = CountDepth(cur_el->mid);
    int maxDepth = max(leftDepth, rightDepth);
    return max(maxDepth, middleDepth) + 1;
}

int Tree::AddNode(int type, int number, double value, int depth, int* flag, int d)
{
    AddNode(type, number, value, depth, flag, d, root);
    if (*flag == 0)
        return 0;
    if (*flag == 1)
    {
        return 1;
    }
}

void Tree::AddNode(int type, int number, double value, int depth, int* flag, int d, Node*& cur_el)
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

void Tree::PrintTree()
{
    PrintTree(root);
}

 void Tree::PrintTree(Node* cur_el)
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

// деление на ноль, основание логарифма должно быть больше 0 и не должно быть равно единицы, аргумент логарифма тоже > 0
double Tree::evaluateExpression(double* x)
{
    return evaluateExpression(x, root);
}

double Tree::evaluateExpression(double* x, Node* cur_el)
{
    if (cur_el == nullptr) {return 0.0;}
    if (cur_el->type == 'c') {return cur_el->constant;}
    else if (cur_el->type == 'v')
    {
        return x[cur_el->variable];
    }
    else if (cur_el->operation < 2000)
    {
        double ev_expression = evaluateExpression(x, cur_el->left);
        //if (isnan(ev_expression))
        if (isinf(ev_expression)){ev_expression = MAX;}
        switch(cur_el->operation)
        {
            case 1000:
            {
                return sin(ev_expression);
            }
            case 1001:
            {
                return cos(ev_expression);
            }
            case 1002:
            {
                return exp(ev_expression);
            }
            case 1003:
            {
                if(ev_expression <= 0){ev_expression = MIN;}
                return log(ev_expression);
            }
            case 1004:
            {
                if(ev_expression == 0){ev_expression = MIN;}
                return 1./ev_expression;
            }
            case 1005:
            {
                return -ev_expression;
            }
        }
    }
    else if (cur_el->operation < 3000)
    {
        double left_value = evaluateExpression(x, cur_el->left);
        double right_value = evaluateExpression(x, cur_el->right);
        if (isinf(left_value)){left_value = MAX;}
        if (isinf(right_value)){right_value = MAX;}
        switch (cur_el->operation)
        {
            case 2000:// слева основание
                {
                    if(left_value <= 0){left_value = MIN;}
                    else if(left_value == 1){left_value = 0.9999999999999999;}//вот тут спросить
                    if(right_value <= 0){right_value = MIN;}
                    return log(right_value)/log(left_value);
                }
            case 2001:
                return left_value +  right_value;
            case 2002:
                return left_value - right_value;
            case 2003:
                return left_value * right_value;
            case 2004:
                {
                    if(right_value == 0){right_value = MIN;}
                    return left_value / right_value;
                }
            case 2005:
                {
                    if(left_value < 0){left_value = 0;}
                    if(left_value == 0 && right_value == 0){return 0.0;}//про это сказать
                    return pow(left_value, right_value);
                }
        }
    }
    else if (cur_el->operation >= 3000)
    {
        double left_value = evaluateExpression(x, cur_el->left);
        double mid_value = evaluateExpression(x, cur_el->mid);
        double right_value = evaluateExpression(x, cur_el->right);
        if (isinf(left_value)){left_value = MAX;}
        if (isinf(right_value)){right_value = MAX;}
        if (isinf(mid_value)){mid_value = MAX;}
        switch (cur_el->operation)
        {
            case 3000:
                return left_value +  right_value;//я не знаю что тут возвращать...
        }
    }

    return 0.0;
}

string Tree::printExpression()
{
    return printExpression(root);
}

string Tree::printExpression(Node* cur_el)
{
    string check;
    if (cur_el == nullptr) {return "";}
    if (cur_el->type == 'c')
    {
        if(cur_el->constant<0)
        {
            return "(" + to_string(cur_el->constant) + ")";
        }
        else{return to_string(cur_el->constant);}
    }
    else if (cur_el->type == 'v')
    {
        return "x" + to_string(cur_el->variable);
    }
    else if (cur_el->operation < 2000)
    {
        check = printExpression(cur_el->left);
        switch(cur_el->operation)
        {
            case 1000:
            {
                if (check.find("(")==0)
                    return "sin" + check;
                else
                {
                    return "sin(" + check + ")";
                }
            }
            case 1001:
            {
                if (check.find("(")==0)
                    return "cos" + check;
                else
                {
                    return "cos(" + check + ")";
                }
            }
            case 1002://если основание чуть меньше одного, то минус бесконечность
            {
                if (check.find("(")==0)
                    return "exp" + check;
                else
                {
                    return "exp(" + check + ")";
                }
            }
            case 1003:
            {
                return "ln(" + check + ")";
            }
            case 1004:
            {
                if (check.find("(")==0)
                    return "1/" + check;
                else return "1/(" + check + ")";
            }
            case 1005:// вот тут надо прям подумать
            {
                return "(-" + check + ")";
            }
        }
    }
     else if (cur_el->operation >=2000 && cur_el->operation < 3000)
    {
        string left_value = printExpression(cur_el->left);
        string right_value = printExpression(cur_el->right);
        switch (cur_el->operation)
        {
            case 2000:
                return "log(" + left_value + ", " + right_value + ")";
            case 2001:
                return "(" + left_value + "+" + right_value + ")";
            case 2002:
                return "(" + left_value + "-" + right_value + ")";
            case 2003:
                return "(" + left_value + "*" + right_value + ")";
            case 2004:
                return "(" + left_value + "/" + right_value + ")";
            case 2005:
                return "(" + left_value + ")^" + "(" + right_value + ")";
        }
    }
    else if (cur_el->operation >= 3000)
    {
        string left_value = printExpression(cur_el->left);
        string mid_value = printExpression(cur_el->mid);
        string right_value = printExpression(cur_el->right);
        switch (cur_el->operation)
        {
            case 3000:
                return "if" + left_value + "then" + mid_value + "else" + right_value;
        }
    }

    return "";
}

double Tree::error(int num_obs, double** x, double* y)
{
    int obs;
    double res=0, check;
    for(obs = 0; obs < num_obs; obs++)
    {
        check = evaluateExpression(x[obs]);
        //if (isnan(check)||check == inf)
            //check = 0;
        res+=(y[obs]-check)*(y[obs]-check);
    }
    return res;
}

double Tree::count_fitness(double error)
{
    UpNumNodes();
    int current_num_var = CountVar();
    double fitness = 1./(1+error)-double(num_nodes)/double(max_nodes)+0.01*double(current_num_var)/double(num_var);
    if(isinf(fitness)){fitness = MAX;}
    return fitness;
}

void Tree::Growth(int switch_init, int depth)
{
    int success, random_opervar, random_type, random_oper, flag = 0;
    double random_const;
    if(switch_init == 0)
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
            success = AddNode(type[0], random_opervar, 0, depth-1, &flag, 0);
        }
        success = 1;
        while (success == 1)
        {
            flag = 0;
            random_type = 1+rand() % 2;
            if(random_type == 1)
            {
                random_opervar = rand() % num_var;
                success = AddNode(type[random_type], random_opervar, 0, depth-1, &flag, 0);
            }
            else if(random_type == 2)
            {
                random_const = (double) rand() / (double)(RAND_MAX);
                if(random_const < 0.05)
                {
                    if(rand()%2==0){random_const = M_PI;}
                    else {random_const = M_E;}
                }
                else{random_const = generate_normal();}
                success = AddNode(type[random_type], 0, random_const, depth-1, &flag, 0);
            }
        }
    }
    else if(switch_init == 1)
    {
        success = 1;
        while (success == 1)
        {
            flag = 0;
            random_const = (double) rand() / (double)(RAND_MAX);
            if(random_const > 0.1)
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
                success = AddNode(type[0], random_opervar, 0, depth-1, &flag, 0);
            }
            else
            {
                random_type = 1+rand() % 2;
                if(random_type == 1)
                {
                    random_opervar = rand() % num_var;
                    success = AddNode(type[random_type], random_opervar, 0, depth-1, &flag, 0);
                }
                else if(random_type == 2)
                {
                    random_const = (double) rand() / (double)(RAND_MAX);
                    if(random_const < 0.05)
                    {
                        if(rand()%2==0){random_const = M_PI;}
                        else {random_const = M_E;}
                    }
                    else{random_const = generate_normal();}
                    success = AddNode(type[random_type], 0, random_const, depth-1, &flag, 0);
                }
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
                success = AddNode(type[random_type], random_opervar, 0, depth-1, &flag, 0);
            }
            else if(random_type == 2)
            {
                random_const = (double) rand() / (double)(RAND_MAX);
                if(random_const < 0.05)
                {
                    if(rand()%2==0){random_const = M_PI;}
                    else {random_const = M_E;}
                }
                else{random_const = generate_normal();}
                success = AddNode(type[random_type], 0, random_const, depth-1, &flag, 0);
            }
        }
    }
}

void Tree::PointMut(Node* cur_el)
{
    double p = 1, mut, before, random_const;
    int random_opervar;
    if(cur_el==NULL){return;}
    mut = ((double) rand() / (double)(RAND_MAX));
    if (mut <= p)
    {
        if(cur_el->type=='o')
        {
            before = cur_el->operation;
            do
            {
                if(before < 2000)
                {
                    random_opervar = (rand() % unar_oper)+1000;
                }
                else if(before >= 2000 && before < 3000)
                {
                    random_opervar = (rand() % bin_oper)+2000;
                }
                else if(before >= 3000)
                {
                    random_opervar = (rand() % trin_oper)+3000;
                }
            }while (before == random_opervar);
            cur_el->operation = random_opervar;
        }
        else if(cur_el->type=='v')
        {
            before = cur_el->variable;
            if (num_var > 1)
            {
               do
                {
                    random_opervar = rand() % num_var;
                }while (before == random_opervar);
                cur_el->variable = random_opervar;
            }
        }
        else if(cur_el->type=='c')
        {
            before = cur_el->constant;
            do
            {
                random_const = generate_normal();
            }while (before == random_const);
            cur_el->constant = random_const;
        }
        //cout << endl;
        //cout << "Mutation "<< before << endl;
        //cout << endl;
    }
    PointMut(cur_el->left);
    PointMut(cur_el->mid);
    PointMut(cur_el->right);

}

void Tree::PartMut(int switch_init, Node* cur_el, int depth, int* flag, bool check_root = false)
{
    double p = 0.1, mut;
    if(cur_el==NULL){return;}
    mut = ((double) rand() / (double)(RAND_MAX));
    if (mut <= p)//mut <= p
    {
        //cout << "До мутации" << endl;
        //PrintTree();
        ClearTree(cur_el);
        if(check_root)
        {
            Growth(switch_init, depth);
            //cout << "После мутации" << endl;
            //PrintTree();
            return;
        }
        *flag = 1;
        return;
    }
    PartMut(switch_init, cur_el->left, depth, flag);
    if(*flag == 1)
    {
        cur_el->left = NULL;
        Growth(switch_init, depth);
        //cout << "После мутации" << endl;
        //PrintTree();
        *flag = 0;
    }
    PartMut(switch_init, cur_el->right, depth, flag);
    if(*flag == 1)
    {
        cur_el->right = NULL;
        Growth(switch_init, depth);
        //cout << "После мутации" << endl;
        //PrintTree();
        *flag = 0;
    }
    PartMut(switch_init, cur_el->mid, depth, flag);
    if(*flag == 1)
    {
        cur_el->mid = NULL;
        Growth(switch_init, depth);
        //cout << "После мутации" << endl;
        //PrintTree();
        *flag = 0;
    }
}

void Tree::StandCross(Node* cur_el, int splitter, Node*& cut_tree, bool cut)
{
    if(cur_el==NULL){return;}
    counter++;
    if (counter == splitter)
    {
        if(cut){copying(cur_el, cut_tree);}
        else
        {
             ClearTree(cur_el->left);
             cur_el->left = NULL;
             //cout << "here" << endl;
             ClearTree(cur_el->right);
             cur_el->right = NULL;
             ClearTree(cur_el->mid);
             cur_el->mid = NULL;
             //PrintTree();
             cur_el->type = cut_tree->type;
             cur_el->operation = cut_tree->operation;
             cur_el->variable = cut_tree->variable;
             cur_el->constant = cut_tree->constant;
             copying(cut_tree->left, cur_el->left);
             copying(cut_tree->right, cur_el->right);
             copying(cut_tree->mid, cur_el->mid);
        }
    }
     else if(counter > splitter)
        return;
    StandCross(cur_el->left, splitter, cut_tree, cut);
    StandCross(cur_el->right, splitter, cut_tree, cut);
    StandCross(cur_el->mid, splitter, cut_tree, cut);
}

int part(double* p, int* in, int start, int _end)
{
    double pivot = p[_end];
    int pIndex = start;
    for (int i = start; i < _end; i++)
    {
        if (p[i] <= pivot)
        {
            swap(p[i], p[pIndex]);
            swap(in[i], in[pIndex]);
            pIndex++;
        }
    }
    swap (p[pIndex], p[_end]);
    swap (in[pIndex], in[_end]);
    return pIndex;
}

void quicksort(double* p, int* in, int start, int _end)
{
    if (start >= _end) {
        return;
    }
    int pivot = part(p, in, start, _end);
    quicksort(p, in, start, pivot - 1);
    quicksort(p, in, pivot + 1, _end);
}

int part(double* p, double* in, int start, int _end)
{
    double pivot = p[_end];
    int pIndex = start;
    for (int i = start; i < _end; i++)
    {
        if (p[i] <= pivot)
        {
            swap(p[i], p[pIndex]);
            swap(in[i], in[pIndex]);
            pIndex++;
        }
    }
    swap (p[pIndex], p[_end]);
    swap (in[pIndex], in[_end]);
    return pIndex;
}

void quicksort(double* p, double* in, int start, int _end)
{
    if (start >= _end) {
        return;
    }
    int pivot = part(p, in, start, _end);
    quicksort(p, in, start, pivot - 1);
    quicksort(p, in, pivot + 1, _end);
}

int part(int* p, double* in, int start, int _end)
{
    int pivot = p[_end];
    int pIndex = start;
    for (int i = start; i < _end; i++)
    {
        if (p[i] <= pivot)
        {
            swap(p[i], p[pIndex]);
            swap(in[i], in[pIndex]);
            pIndex++;
        }
    }
    swap (p[pIndex], p[_end]);
    swap (in[pIndex], in[_end]);
    return pIndex;
}

void quicksort(int* p, double* in, int start, int _end)
{
    if (start >= _end) {
        return;
    }
    int pivot = part(p, in, start, _end);
    quicksort(p, in, start, pivot - 1);
    quicksort(p, in, pivot + 1, _end);
}


void synthetic_data(double** x, double* y, int num_obs)
{
    int i;
    for(i = 0; i < num_obs; i++)
    {
        x[i][0] = dis(gen);
        x[i][1] = dis(gen);
        x[i][2] = dis(gen);
        y[i] = x[i][0]*x[i][0]+0*x[i][1];
    }
}

// заполняется дерево с верхушки, потом влево, потом вправо, потом в середину
void init_population(int switch_init, int n, Tree* tree, int depth)
{
    int i;
    for(i = 0; i < n; i++)
    {
         tree[i].Growth(switch_init, depth);
    }
}

string delete_brackets(string s)
{
    if (s.find("(")==0)
    {
        s.erase(0, 1);
        s.erase(s.size()-1,1);
    }
    return s;
}

void selection (string sel_switch, double *fitness, int n, Tree* parents, Tree* tree)
{
    double *temp_fitness = new double[n];
    double sumfitness = 0;
    int *marks = new int[n];
    int *index = new int[n];
    int fight, i, j, candidate1, candidate2, winner, cont = 0, k, sumrang = 0;
    double *rang = new double[n];
    double *p = new double[n];
    double temp = 0, randp;
    if (sel_switch == "prop")
    {
        for (i = 0; i < n; i++)
        {
            sumfitness += fitness[i];
        }
        for (i = 0; i < n; i++)
        {
            p[i] = temp + (double)fitness[i]/((double)sumfitness);
            //cout << p[i] << endl;
            temp += (double)fitness[i]/((double)sumfitness);
        }
        for (fight = 0; fight < n*2; fight++)
        {
            randp = ((double) rand() / (RAND_MAX));
            if (randp >= 0&&randp < p[0])
            {
                parents[fight].CloneTree(tree[0].root);
                continue;
            }
            for (j = 0; j < n-1; j++)
            {
                if (randp < p[j+1]&&randp >=p[j])
                {
                    parents[fight].CloneTree(tree[j+1].root);
                    break;
                }
            }
        }
    }
    if (sel_switch == "rang")
    {
        for (i = 0; i < n; i++)
        {
            temp_fitness[i] = fitness[i];
            index[i] = i;//индекс начинается с нуля
            rang[i] = i + 1;//ранг нулевым быть не должен
            marks[i] = 0;
        }
        quicksort(temp_fitness, index, 0, n - 1);
        for (i = 0; i < n-1; i++)
        {
            if (marks[i] == 2)
                continue;
            sumrang = i+1;
            for (j = i + 1; j < n; j++)
            {
                if (temp_fitness[i] == temp_fitness[j])
                {
                    cont +=1;
                    marks[i] = 1;
                    marks[j] = 1;
                    sumrang += j+1;
                }
            }
            if (cont > 0)
            {
                cont += 1;
                for (k = 0; k < n; k++)
                {
                    if (marks[k] == 1)
                    {
                        rang[k] = (double)sumrang/((double) cont);
                        marks[k] = 2;
                    }
                }
            }
            cont = 0;
            sumrang = 0;
        }
        quicksort(index, rang, 0, n-1);
        for (i = 0; i < n; i++)
        {
            sumfitness += rang[i];
        }
        for (i = 0; i < n; i++)
        {
            p[i] = temp + (double)rang[i]/((double)sumfitness);
            //cout << p[i] << endl;
            temp += (double)rang[i]/((double)sumfitness);
        }

        for (fight = 0; fight < n*2; fight++)
        {
            randp = ((double) rand() / (RAND_MAX));
            if (0 <= randp && randp < p[0])
            {
                parents[fight].CloneTree(tree[0].root);
                continue;
            }
            for (j = 0; j < n-1; j++)
            {
                if (p[j] <= randp && randp < p[j+1])
                {
                    parents[fight].CloneTree(tree[j+1].root);
                    break;
                }
            }
        }
    }
    if (sel_switch == "tour")
    {
         for (int fight = 0; fight < n*2; fight++)
        {
            candidate1 = rand() % n;
            candidate2 = rand() % n;
            if (fitness[candidate1] > fitness[candidate2])
            {
                parents[fight].CloneTree(tree[candidate1].root);
            }
            if (fitness[candidate1] < fitness[candidate2])
            {
                parents[fight].CloneTree(tree[candidate2].root);
            }
            if (fitness[candidate1] == fitness[candidate2])
            {
                winner = rand() % 2;
                if (winner == 0)
                    parents[fight].CloneTree(tree[candidate1].root);
                if (winner == 1)
                    parents[fight].CloneTree(tree[candidate2].root);
            }
        }
    }
    delete[] marks;
    delete[] temp_fitness;
    delete[] index;
    delete[] rang;
    delete[] p;
}

void mutation(string mut_switch, int switch_init, int n, Tree *children)
{
    int i, flag = 0, depth;
    if(mut_switch == "point")
    {
        for (i = 0; i< n; i++)
        {
            children[i].PointMut(children[i].root);
        }
    }
    if(mut_switch == "part")
    {
        for (i = 0; i< n; i++)
        {
            depth = children[i].CountDepth(children[i].root);
            children[i].PartMut(switch_init, children[i].root, depth, &flag, true);
        }
    }
}


// вырезается со второго дерева, вставляется в первое
void crossover(int n, Tree *parents, Tree *children)
{
    int i, k, splitter1, splitter2;
    Tree cut_tree;
    // поменять на n*2
    for (i = 0, k = 0; k < n*2; i++, k += 2)
    {
        //cout << "Выражение " << parents[k].printExpression() << endl;
        //cout << "Выражение " << parents[k+1].printExpression() << endl;
        parents[k].UpNumNodes();
        parents[k+1].UpNumNodes();
        if (parents[k].num_nodes == 1)
            splitter1 = 0;
        else splitter1 = rand() % (parents[k].num_nodes-1)+1;
        if(parents[k+1].num_nodes == 1)
            splitter2 = 0;
        else splitter2 = rand() % (parents[k+1].num_nodes-1)+1;
        parents[k+1].StandCross(parents[k+1].root, splitter2, cut_tree.root, true);
        counter = -1;
        //cout << "Вырезанная часть со второго дерева " << splitter2 << "\t" << cut_tree.printExpression() << endl;
        children[i].CloneTree(parents[k].root);
        //children[i].PrintTree();
        //cout << splitter1 << " " << splitter2 << " " << k << endl;
        children[i].StandCross(children[i].root, splitter1, cut_tree.root, false);
        //cout << "Новое дерево " << splitter1 << "\t" << children[i].printExpression() << endl;
        //children[i].PrintTree();
        counter = -1;
        cut_tree.ClearTree(cut_tree.root);
        cut_tree.root = NULL;
    }
}


int main()
{
    srand(time(NULL));
    setlocale(0, "");
    int i, j, k, in, obs, num_obs = 10, depth = 3;
    // flag нужен для того, чтобы понимать добавился ли элемент в дерево, d контролирует глубину строящегося дерева
    int n = 100, num_generals=100, general;
    //n - количество индивидов в поколении, num_generals - количество поколений
    int switch_init = 0;
    //0 - полный метод, 1 - метод выращивания
    string sel_switch = "rang";// prop, rang, tour
    string mut_switch = "part";// point, part
    //string cross_switch = "stand";// stand, one


    double** x = new double* [num_obs];//строчки - наблюдения, столбцы - признаки для каждого наблюдения
    for(i = 0; i < num_obs; i++)
    {
        x[i] = new double[num_var];
    }
    double* y = new double [num_obs];
    double* fitness = new double[n*2];
    double* fitness_temp = new double[n];
    Tree* tree = new Tree[n];
    Tree* tree_temp = new Tree[n];
    Tree* parents = new Tree[n*2];
    Tree* children = new Tree[n];//зачем n*2??
    int *index = new int[n*2];

    synthetic_data(x, y, num_obs);

    init_population(switch_init, n, tree, depth);
    for(i = 0; i < n; i++)
    {
        fitness[i] = tree[i].count_fitness(tree[i].error(num_obs, x, y));
    }

    //цикл поколений
    for(general = 0; general < num_generals; general++)
    {
        cout << "General " << general << endl;

        tree[0].UpNumNodes();
        cout << "Лучший индивид " << tree[0].printExpression() << " " << tree[0].CountDepth(tree[0].root) << endl;
        cout << "Значение функции пригодности " << fitness[0] << " vars " << tree[0].CountVar() << endl;

        /*for(i = 0; i < n; i++)
        {
            cout << "Tree " << i <<endl;
            //tree[i].PrintTree();
            tree[i].UpNumNodes();
            cout << "Выражение " << tree[i].printExpression() << " " << tree[i].CountDepth(tree[i].root) << endl;
            //cout << "Значение выражения " << tree[i].evaluateExpression(x[0]) << endl;
            cout << "Значение функции пригодности " << fitness[i] << endl;
            cout << tree[i].root << endl;
        }*/
        selection(sel_switch, fitness, n, parents, tree);
        /*for(i = 0; i < n*2; i++)
        {
            cout << "Parent " << i <<endl;
            parents[i].UpNumNodes();
            cout << "Выражение " << parents[i].printExpression() << " " << parents[i].num_nodes << endl;
            cout << parents[i].root << endl;
        }*/
        crossover(n, parents, children);
        /*if (general == 94)
        {
            for(i = 0; i < n; i++)
            {
                cout << "Child before mutation " << i <<endl;
                //children[i].PrintTree();
                cout << "Выражение " << children[i].printExpression() << endl;
            }
        }
        for(i = 0; i < n; i++)
        {
            cout << "Child before mutation " << i <<endl;
            //children[i].PrintTree();
            cout << "Выражение " << children[i].printExpression() << endl;
            cout << children[i].root << endl;
        }
        if (general == 94)
        {
            mutation1(mut_switch, switch_init, n, children);
        }
        */
        mutation(mut_switch, switch_init, n, children);
        /*for(i = 0; i < n; i++)
        {
            cout << "Child after mutation " << i <<endl;
            //children[i].PrintTree();
            cout << "Выражение " << children[i].printExpression() << endl;
            cout << children[i].root << endl;
        }*/
        for(i = 0; i < n; i++)
        {
            fitness[i+n] = children[i].count_fitness(children[i].error(num_obs, x, y));
        }
        for (i = 0; i < n*2; i++)
            index[i] = i;
        quicksort(fitness, index, 0, n*2 - 1);
        //отбор индивидов
        for (i = 0, k = n*2-1; i < n; i++,k--)
        {
            in = index[k];
            if (in < n)
            {
                tree_temp[i].CloneTree(tree[in].root);
                fitness_temp[i] = fitness[k];
            }
            if (in >= n)
            {
                tree_temp[i].CloneTree(children[in-n].root);
                fitness_temp[i] = fitness[k];
            }
        }
        for (i = 0; i < n; i++)
        {
            tree[i].CloneTree(tree_temp[i].root);
            fitness[i] = fitness_temp[i];
        }
    }
    /*cout << "children" << endl;
    for (i = 0; i < n; i++)
    {
        cout << "Выражение " << children[i].printExpression() << endl;
        cout << children[i].root << endl;
    }*/

    delete[] children; //рано это еще смотреть так как пока не понятно где они там обновляются и так далее
    delete[] parents;//разобраться с памятью, возможно, это одни и те же деревья, поэтому вылазит ошибка
    delete[] fitness;
    delete[] fitness_temp;
    delete[] tree_temp;
    delete[] tree;
    delete[] index;
    for(i = 0; i < num_obs; i++)
    {
        delete[] x[i];
    }
    delete[] x;
    delete[] y;
}

// штраф = коэффициент*количество узлов в дереве, пока что вообще не надо добавлять это потом
// что насчет минус бесконечности
// нужно выбирать лучших индивидов на основе рангов, ранги рассчитываются так: по трем критериям - ошибка, количество переменных,
// количество узлов, каждому индивиду поставить три ранга в соответствии с этими критериями, затем эти ранги сложить и определить лучшего (максимум)
// делать ограничения на максимальное количество узлов в дереве при селекции и частичной мутации, если дерево выходит за рамки, то пробовать снова, пока не получится 100 раз
