#include <iostream>
#include <cmath>

using namespace std;
// константы же типа double? их рандомными задавать?
typedef class Node
{
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

    /*Node* FindFree(Node* node)
    {
        if(node == NULL)
        {
            return node;
        }
        FindFree(node.left);
        FindFree(node.right);
        if(*node.left == NULL)
        {
            return *node.left;
        }
        else if(*node.right == NULL)
        {
            return *node.right;
        }
        else if()//тут нужно проверить на глубину
            {FindFree(*node.left);}
    }*/
    int AddElem(Node cur_el, Node new_el)
    {
        Node root = cur_el;
        if(root == NULL)
        {
            root = new_el;
            return 0;
        }
        AddElem(root.left, new_el);
        AddElem(root.right, new_el);
        return 1;
    }


    /*void PrintTree()
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
    }*/
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
    int i, j, num_obs = 400, num_oper = 8, num_var = 1, num_types = 3, depth = 3;
    // 0+, 1-, 2*, 3/, 4sin, 5cos, 6exp, 7log,
    // 0x,
    double* x = new double [num_obs];
    double* y = new double [num_obs];
    string* type = new string[num_types];

    type[0] = 'oper';
    type[1] = 'var';
    type[2] = 'const';
    synthetic_data(x, y, num_obs);

    delete[] x;
    delete[] y;
    delete[] type;
}
