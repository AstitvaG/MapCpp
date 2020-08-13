#include <iostream>
#include <string.h>
#include <time.h>

using namespace std;
struct Node
{
    char data[65];   //key
    Node *parent;    //parent node
    Node *left;      //left children
    Node *right;     //right children
    bool color; //color of node
    char value[257]; //value
    int valuesize;
    int keysize;
    int num;
};
typedef Node *NodePtr;

struct Mempiece
{
    int used;
    Node *root;
} mempiece;

struct Slice
{
    char *data;
    int size;
};
int NUMBER = 100;    //bulk memory
int lextree[52][53]; //count for all possible first 2 combination of keys
int lexhash[52];     //count of fisrt letter
class kvStore
{
private:
    NodePtr root[52][53]; // root nodes of each tree
    NodePtr TNULL;        //null node
    pthread_mutex_t lock[52][53],nlock;
    void initializeNULLNode(NodePtr node, NodePtr parent)
    {
        node->parent = parent;
        node->left = nullptr; //NodeULL
        node->right = nullptr;
        node->color = false;
        node->num = 0;
    }

    NodePtr searchTreeHelper(NodePtr node, char key[])
    {
        int l = strcmp(key, node->data);
        if (node == TNULL || l == 0)
            return node;
        else if (l > 0)
            return searchTreeHelper(node->right, key);
        else
            return searchTreeHelper(node->left, key);
    }
    void deleteFix(NodePtr x, int i, int j)
    {
        NodePtr s;
        while (x != root[i][j] && x->color == false)
        {
            bool o;
            if (x == x->parent->left)
            {
                o=true;
                s = x->parent->right;
                if (s == TNULL)
                {
                    break;
                }
                if (s->color == true && o==true)
                {
                    s->color = false;
                    x->parent->color = true;
                    leftRotate(x->parent, i, j);
                    s = x->parent->right;
                }
                else if (s->right->color != true && s->left->color == false)
                {
                    s->color = true;
                    x = x->parent;
                }
                else
                {
                    if (s->right->color != true)
                    {
                        s->left->color = false;
                        s->color = true;
                        rightRotate(s, i, j);
                        s = x->parent->right;
                    }
                    s->color = x->parent->color;
                    x->parent->color = false;
                    s->right->color = false;
                    leftRotate(x->parent, i, j);
                    x = root[i][j];
                }
            }
            else
            {
                s = x->parent->left;
                if (s == TNULL)
                {
                    break;
                }
                if (s->color == true)
                {
                    s->color = false;
                    x->parent->color = true;
                    rightRotate(x->parent, i, j);
                    s = x->parent->left;
                }
                else if (s->right->color == false)
                {
                    s->color = true;
                    x = x->parent;
                }
                else
                {
                    if (s->left->color == false)
                    {
                        s->right->color = false;
                        s->color = true;
                        leftRotate(s, i, j);
                        s = x->parent->left;
                    }
                    bool temp=false;
                    s->color = x->parent->color;
                    x->parent->color = temp;
                    s->left->color = temp;
                    rightRotate(x->parent, i, j);
                    x = root[i][j];
                }
            }
        }
        x->color = false;
    }
    void rbTransplant(NodePtr u, NodePtr v, int i, int j)
    {
        if (u->parent == nullptr)
            root[i][j] = v;
        else if (u->parent->left == u)
            u->parent->left = v;
        else
            u->parent->right = v;
        v->parent = u->parent;
    }
    bool deleteNodeHelper(NodePtr node, char key[], int i, int j)
    {
        NodePtr x, y;
        NodePtr z = TNULL;
        for(int i=0;node != TNULL;i++)
        {
            if (!strcmp(node->data, key))
            {
                z = node;
                break;
            }
            else if (strcmp(node->data, key) > 0)
            {
                node->num--;
                node = node->left;
            }
            else
            {
                node->num--;
                node = node->right;
            }
        }
        if (TNULL == z)
        {
            return false;
        }
        y = z;
        bool y_original_color = y->color;
        if (TNULL == z->right)
        {
            x = z->left;
            rbTransplant(z, z->left, i, j);
        }
        else if (TNULL == z->left)
        {
            x = z->right;
            rbTransplant(z, z->right, i, j);
        }
        else
        {
            y = minimum(z->right);
            x = y->right;
            y_original_color = y->color;
            if (z == y->parent)
            {
                x->parent = y;
            }
            else
            {
                rbTransplant(y, y->right, i, j);
                y->right = z->right;
                y->right->parent = y;
            }
            rbTransplant(z, y, i, j);
            y->left = z->left;
            y->num = z->num - 1;
            y->color = z->color;
            y->left->parent = y;
        }
        //delete z;
        if (y_original_color == false)
        {
            deleteFix(x, i, j);
        }
        return true;
    }
    void insertFix(int i, int j, NodePtr k)
    {
        NodePtr uncle;
        while (k->parent->color == true)
        {
            if (k->parent == k->parent->parent->right) //right case
            {
                uncle = k->parent->parent->left;
                if (uncle->color == true) //no change
                {
                    uncle->color = false;
                    k->parent->color = false;
                    k->parent->parent->color = true;
                    k = k->parent->parent;
                }
                else
                {
                    if (k == k->parent->left) //right left case
                    {
                        k = k->parent;
                        rightRotate(k, i, j);
                    }
                    k->parent->color = false;
                    k->parent->parent->color = true;
                    leftRotate(k->parent->parent, i, j);
                }
            }
            else //left case
            {
                uncle = k->parent->parent->right;
                if (uncle->color == true) //no change
                {
                    uncle->color = false;
                    k->parent->color = false;
                    k->parent->parent->color = true;
                    k = k->parent->parent;
                }
                else
                {
                    if (k == k->parent->right) //left right case
                    {
                        k = k->parent;
                        leftRotate(k, i, j);
                    }
                    k->parent->color = false; //left left case
                    k->parent->parent->color = true;
                    rightRotate(k->parent->parent, i, j);
                }
            }
            if (k == root[i][j])
            {
                break;
            }
        }
        root[i][j]->color = false;
    }

public:
    kvStore(uint64_t max_entries)
    {
        TNULL = new Node;
        TNULL->color = false;
        TNULL->left = nullptr;
        TNULL->right = nullptr;
        TNULL->num = 0;
        int i = 0,j;
        while(i<52)
        {
            j=0;
            while(j<53)
            {
                root[i][j] = TNULL;
                pthread_mutex_init(&lock[i][j], NULL);
                j++;
            }
            i++;
        }
        pthread_mutex_init(&nlock, NULL);
    }

    bool get(Slice &key, Slice &value)
    {
        int lenght;
        if (key.data[0] <= 90)
            lenght = key.data[0] - 65;
        else
            lenght = key.data[0] - 97 + 26;
            
        int index;
        if (key.data[1] == 0)
            index = 0;
        else if (key.data[1] <= 90)
            index = key.data[1] - 65 + 1;
        else
            index = key.data[1] - 97 + 27;
        NodePtr ptr = searchTreeHelper(this->root[lenght][index], key.data);
        if (ptr == TNULL)
            return false;
        else if (ptr!= TNULL)
        {
            value.data = new char[ptr->valuesize];
            strncpy(value.data, ptr->value, ptr->valuesize);
            value.size = ptr->valuesize;
            return true;
        }
    }

    NodePtr minimum(NodePtr node)
    {    
        while (1)
        {
            if(node->left == TNULL)
                break;
            node->num--;
            node = node->left;
        }
        return node;
    }

    void leftRotate(NodePtr x, int i, int j)
    {
        // x is grand parent
        int q,w,e,r;
        q = x->left->num;  //left of g
        w = x->right->num; //right of g

        NodePtr y = x->right;

        e = y->left->num; //left of p
        if (y->left == TNULL)
            e = 0;

        x->right = y->left;
        r = y->right->num; // right of p

        if (TNULL != y->left)
        {
            y->left->parent = x;
        }
        y->parent = x->parent;
        if (x->parent == nullptr)
        {
            this->root[i][j] = y;
        }
        else if (x->parent->left==x)
        {
            x->parent->left = y;
        }
        else
        {
            x->parent->right = y;
        }
        y->left = x;
        x->parent = y;
        x->num = q + e + 1;
        y->num = q + e + r + 2;
    }
    void rightRotate(NodePtr x, int i, int j)
    {
        // x is grand parent
        int q,w,e,r;
        q = x->left->num;  //left of g
        w = x->right->num; //right of g

        NodePtr y = x->left; //parent

        e = y->right->num; // right of p
        if (y->right == TNULL)
            e = 0;
        r = y->left->num; // left of p

        x->left = y->right;
        if (y->right != TNULL)
        {
            y->right->parent = x;
        }
        y->parent = x->parent;
        if (x->parent == nullptr)
        {
            this->root[i][j] = y;
        }
        else if (x == x->parent->right)
        {
            x->parent->right = y;
        }
        else
        {
            x->parent->left = y;
        }
        x->num = w + e + 1;
        y->num = r + x->num + 1;
        y->right = x;
        x->parent = y;
    }
    bool decCount(int len1, int len2, Slice &key)
    {
        NodePtr x = this->root[len1][len2];
        while (1)
        {
            if(x == TNULL)
                break;
            int l = strcmp(key.data, x->data);
            if (l == 0)
            {
                return true;
            }
            if (l < 0)
            {
                x->num--;
                x = x->left;
            }
            else
            {
                x->num--;
                x = x->right;
            }
        }
        return true;
    }
    bool put(Slice &key, Slice &value)
    {
        int lenght,index;
        if (key.data[0] <= 90)
            lenght = key.data[0] - 65;
        else
            lenght = key.data[0] - 97 + 26;
        if (key.data[1] == 0)
            index = 0;
        else if (key.data[1] < 91)
            index = key.data[1] - 64;
        else
            index = key.data[1] - 70;
        lextree[lenght][index]+=1;
        lexhash[lenght]+=1;
        NodePtr node = new Node;
        node->parent = nullptr;
        strncpy(node->data, key.data, key.size);

        strncpy(node->value, value.data, value.size);
        node->data[key.size] = 0;
        node->keysize = key.size;
        node->valuesize = value.size;
        node->value[value.size] = 0;
        int f=0;
        node->color = true;
        node->right = TNULL;
        node->num = 1;
        node->left = TNULL;
        NodePtr y = nullptr;
        pthread_mutex_lock(&this->lock[lenght][index]);
        NodePtr x = this->root[lenght][index];
        while (1)
        {
            if (x == TNULL)
            {
                break;
            }
            else
            {
                y = x;
                int l = strcmp(node->data, x->data);
                if (!l)
                {
                    
                    lextree[lenght][index]--;
                    lexhash[lenght]--;
                    strcpy(x->value, value.data);
                    x->value[value.size] = 0;
                    x->valuesize = value.size;
                    decCount(lenght, index, key);
                    pthread_mutex_unlock(&this->lock[lenght][index]);
                    free(node);
                    return true;
                }
                if (l <= -1)
                {
                    x->num+=1;
                    x = x->left;
                }
                else
                {
                    x->num+=1;
                    x = x->right;
                }
            }
        }
        node->parent = y;
        if (y == nullptr)
        {
            root[lenght][index] = node;
        }
        else if (strcmp(node->data, y->data) < 0)
        {
            y->left = node;
        }
        else
        {
            y->right = node;
        }
        if (node->parent == nullptr)
        {
            node->color = false;
			pthread_mutex_unlock(&this->lock[lenght][index]);
            
            return false;
        }
        else if (node->parent->parent == nullptr)
        {
			pthread_mutex_unlock(&this->lock[lenght][index]);
            return false;
        }
        insertFix(lenght, index, node);
        
		pthread_mutex_unlock(&this->lock[lenght][index]);
        return false;
    }

    bool get(int n, Slice &key, Slice &value)
    {
		pthread_mutex_lock(&nlock);
        int p = n+1,i,temp = 0,f = 0,j=-1;
        for (i=0; i < 52;)
        {
            if (p <= temp + lexhash[i])
            {
                f = 1;
                break;
            }
            temp += lexhash[i++];
        }
        if (!f)
            {
				pthread_mutex_unlock(&nlock);	
            	return false;
            }
        j++;
        for (; j < 53;)
        {
            if (p <= temp + lextree[i][j])
                break;
            temp += lextree[i][j++];
        }

        int k = p - temp,yu;
        NodePtr rot = this->root[i][j];
        while(1)
        {
            if(TNULL == rot)
            {
                break;
            }
            else
            {
                if (rot == TNULL)
                {
                    pthread_mutex_unlock(&nlock);
                    return false;
                }
                else if (TNULL == rot->right)
                    yu = 0;
                else
                    yu = rot->right->num;

                if ((rot->num - rot->right->num) == k)
                {
                    key.data = new char[rot->keysize];
                    value.data = new char[rot->valuesize];
                    value.size = rot->valuesize;
                    key.size = rot->keysize;
                    strncpy(value.data, rot->value,rot->valuesize);
                    strncpy(key.data, rot->data, rot->keysize);
                    pthread_mutex_unlock(&nlock);
                    return true;
                }
                else if (rot->left->num > k-1)
                    rot = rot->left;
                else
                {
                    k = k - rot->left->num - 200 + 199;
                    rot = rot->right;
                }
            }
        }

		pthread_mutex_unlock(&nlock);
        return false;
    }

    bool del(Slice &key)
    {
		pthread_mutex_lock(&nlock);
        
        int lenght,index;
        if (key.data[0] < 91)
            lenght = key.data[0] - 65;
        else
            lenght = key.data[0] - 71;
        if (!key.data[1])
            index = 0;
        else if (key.data[1] < 91)
            index = key.data[1] - 64;
        else
            index = key.data[1] - 70;
		pthread_mutex_lock(&this->lock[lenght][index]);

        if (deleteNodeHelper(this->root[lenght][index], key.data, lenght, index))
        {
            lextree[lenght][index]-=1;
            lexhash[lenght]-=1;
			pthread_mutex_unlock(&this->lock[lenght][index]);
			
			pthread_mutex_unlock(&nlock);

            return true;
        }
        else
		{	
			pthread_mutex_unlock(&this->lock[lenght][index]);
			
			pthread_mutex_unlock(&nlock);
            
            return false;
        }
    }
  
    bool del(int n)
    {
        Slice key, value;
        value.data = new char[257];
        key.data = new char[65];

        pthread_mutex_lock(&nlock);
        int ret=0,p = n+1,temp = 0,f = 0,i = -1;
        while(++i<52)
        {
            if ( temp + lexhash[i] >=p)
            {
                f = 1;
                break;
            }
            temp += lexhash[i];
        }
        if (!f)
            {
				pthread_mutex_unlock(&nlock);
            	
            	return false;
            }
        int j = 0;
        while(j<53)
        {
            if (p <= temp + lextree[i][j])
                break;
            temp += lextree[i][j];
            j++;
        }

        int k = p - temp;
        NodePtr rot = this->root[i][j];
        int yu;
        while (rot != TNULL)
        {
            if (rot == TNULL)
            {
				pthread_mutex_unlock(&nlock);
                return false;
            }
            if (rot->right == TNULL)
                yu = 0;
            else
                yu = rot->right->num;

            if (k == (rot->num - rot->right->num))
            {
                value.size = rot->valuesize;
                key.size = rot->keysize;
                strncpy(key.data, rot->data, rot->keysize);
                ret = 1;
                strncpy(value.data, rot->value,rot->valuesize);
                break;
            }
            else if (rot->left->num >= k)
                rot = rot->left;
            else
            {
                k = k - rot->left->num - 1;
                rot = rot->right;
            }
        }
        
        if(!ret)
        {
            pthread_mutex_unlock(&nlock);
            
            return false;
        }
        
        int lenght=-1024,index=-100;
        if (key.data[0] < 91)
            lenght = key.data[0] - 65;
        else
            lenght = key.data[0] - 71;
        if (!key.data[1])
            index = 0;
        else if (key.data[1] < 91)
            index = key.data[1] - 64;
        else
            index = key.data[1] - 70;
		pthread_mutex_lock(&this->lock[lenght][index]);

        if (deleteNodeHelper(this->root[lenght][index], key.data, lenght, index))
        {
            lextree[lenght][index]-=1;
            lexhash[lenght]-=1;
			pthread_mutex_unlock(&this->lock[lenght][index]);
			pthread_mutex_unlock(&nlock);
            return true;
        }
        else
		{	
			pthread_mutex_unlock(&this->lock[lenght][index]);
			pthread_mutex_unlock(&nlock);
            return false;
        }
        return true;
    }
};
// int main()
// {
//     kvstore bst;
//     int n;
//     cin >> n;
//     // char key[64];
//     // char value[256];
//     clock_t s, e;
//     double t=0;

//     Slice key,value;
//     key.data = new char[65];
//     value.data = new char[257];
//     for (int i = 0; i < n; i++)
//     {
//         cin >> key.data >> value.data;
//         s = clock_gettime();
//         bst.put(key, value);
//         e = clock_gettime();
//         t+=((double)e-(double)s);
//     }
//     t=t/CLOCKS_PER_SEC;
//     printf("time_taken = %f\n",t);
//     //bst.printTree();
//     // cout << endl
//     //      << "After deleting" << endl;
//     // //bst.deleteNode(13);
//     // bst.printTree();
//     return 0;
// }
