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
        if(type == 'oper')
        {
            this->operation = number;
            this->variable = NULL;
        }
        if(type == 'var')
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
    Node* root;
    Tree()
    {
        this->root = NULL;
    }

    /*void AddElem(Node* parent, Node* cur_el, Node* new_el, int unar_oper, int bin_oper)
    {
        if(cur_el == NULL)
        {
            cur_el = new_el;
            return;
        }
        else if(*parent < unar_oper)
        {
            AddElem(cur_el, cur_el->left, new_el, unar_oper, bin_oper);
        }
        else if(*parent >= unar_oper && *parent < unar_oper+bin_oper)
        {
            AddElem(cur_el, cur_el->left, new_el, unar_oper, bin_oper);
            AddElem(cur_el, cur_el->right, new_el, unar_oper, bin_oper);
        }
        else if(*parent >= unar_oper+bin_oper)
        {
            AddElem(cur_el, cur_el->left, new_el, unar_oper, bin_oper);
            AddElem(cur_el, cur_el->right, new_el, unar_oper, bin_oper);
            AddElem(cur_el, cur_el->mid, new_el, unar_oper, bin_oper);
        }
    }*/
    void AddNode(Node* new_el, int unar_oper, int bin_oper)
    {
        AddNode(new_el, unar_oper, bin_oper, root);
    }

    void AddNode(Node* new_el, int unar_oper, int bin_oper, Node* cur_el)
    {
        if(cur_el == NULL)
        {
            cur_el = new_el;
            return;
        }
        else if(cur_el->operation < unar_oper)
        {
            AddNode(new_el, unar_oper, bin_oper, cur_el->left);
        }
        else if(cur_el->operation >= unar_oper && cur_el->operation < unar_oper+bin_oper)
        {
            AddNode(new_el, unar_oper, bin_oper, cur_el->left);
            AddNode(new_el, unar_oper, bin_oper, cur_el->right);
        }
        else if(cur_el->operation >= unar_oper+bin_oper)
        {
            AddNode(new_el, unar_oper, bin_oper, cur_el->left);
            AddNode(new_el, unar_oper, bin_oper, cur_el->right);
            AddNode(new_el, unar_oper, bin_oper, cur_el->mid);
        }
    }

    void PrintTree()
    {
        if (this->p_left != NULL)
        {
            this->p_left->PrintTree();
        }

        if (this->p_mid != NULL)
        {
            this->p_mid->PrintTree();
        }

        cout << this->n << " ";

        if (this->p_right != NULL)
        {
            this->p_right->PrintTree();
        }
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
    int i, j, num_obs = 400, unar_oper = 3, bin_oper = 5, trin_oper = 1, num_var = 1, num_types = 3, depth = 3;
    // 0sin, 1cos, 2exp, 3log, 4+, 5-, 6*, 7/, 8if..then..else
    // 0x,
    double* x = new double [num_obs];
    double* y = new double [num_obs];
    string* type = new string[num_types];

    type[0] = 'oper';
    type[1] = 'var';
    type[2] = 'const';
    synthetic_data(x, y, num_obs);
    Tree* tree = new Tree();
    Node* node = new Node('oper', 2);
    tree.AddNode(node, unar_oper, bin_oper);
    delete[] x;
    delete[] y;
    delete[] type;
}
