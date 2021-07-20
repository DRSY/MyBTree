#include <iostream>
#include <string>
#include <vector>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#define isminimal(root) (root->curNumNodes==MinNumNodes)
#define ismaximal(root) (root->curNumNodes==MaxNumNodes)


// Node 
typedef struct Node{
    // metadata
    int MaxNumNodes;
    int MinNumNodes;
    int curNumNodes;
    int isLeaf;

    int* keys;
    Node** children;

    Node(int max_num_keys) {
        MaxNumNodes = max_num_keys;
        MinNumNodes = ceil((max_num_keys+1)/2) - 1; 
        curNumNodes = 0;
        isLeaf = true;
        keys = new int[MaxNumNodes];
        children = new Node*[MaxNumNodes+1];
        for (int i=0;i<MaxNumNodes+1;++i)
            children[i] = NULL;
    }

    int index_of_key(int value) {
        int l = 0;
        int r = curNumNodes - 1;
        while(l <= r) {
            int mid = (l+r) >> 1;
            if(keys[mid] == value)
                return mid;
            if(value < keys[mid])
                r = mid - 1;
            else
                l = mid + 1;
        }
        return r + 1; // r+1 is the index where "value" should be inserted, as well as index of the child node for next search
    }

}Node_t, *NodePtr_t;


class BTree {
    int MaxNumNodes;
    int MinNumNodes;
    
    public:
    NodePtr_t root;
    BTree(int max_num_keys) {
        MaxNumNodes = max_num_keys;
        MinNumNodes = ceil((max_num_keys+1)/2) - 1; 
        root = NULL;
    }

    bool search_(NodePtr_t root, int value) {
        if(!root)
            return false;
        int index = root->index_of_key(value);
        if(index < root->curNumNodes && root->keys[index] == value)
            return true;
        else
            return search_(root->children[index], value);
    }

    bool search(int value) {
        return search_(root, value);
    }

    void insert_(NodePtr_t root, int value) {
      // root is now assured to not be a node with curNumNode==MaxNumNodes
        if(root->isLeaf) {
            // insert
            int index = root->index_of_key(value);
            if(index < root->curNumNodes && root->keys[index] == value)
            { printf("%d Already in the B-Tree\n", value); return;}
            for(auto i=root->curNumNodes;i>index;--i)
                root->keys[i] = root->keys[i-1];
            root->keys[index] = value;
            root->curNumNodes++;
        }else {
            // go to next level
            int index = root->index_of_key(value);
            if(index < root->curNumNodes && root->keys[index] == value)
            { printf("%d Already in the B-Tree\n", value); return;}
            NodePtr_t next_node = root->children[index];
            if(next_node->curNumNodes==MaxNumNodes) {
                // split the next node
                int mid_index = (next_node->curNumNodes)/2;
                NodePtr_t new_left = new Node_t(MaxNumNodes);
                new_left->curNumNodes = mid_index;
                NodePtr_t new_right = new Node_t(MaxNumNodes);
                for(auto i=0;i<mid_index;++i) {
                    new_left->keys[i] = next_node->keys[i];
                    new_left->children[i] = next_node->children[i];
                    if(new_left->children[i])
                        new_left->isLeaf = false;
                }
                new_left->children[mid_index] = next_node->children[mid_index];
                new_right->curNumNodes = MaxNumNodes-1-mid_index;
                for(auto i=mid_index+1, j=0;j<new_right->curNumNodes;++j, ++i) {
                    new_right->keys[j] = next_node->keys[i];
                    new_right->children[j] = next_node->children[i];
                    if(new_right->children[i])
                        new_right->isLeaf = false;
                }
                new_right->children[new_right->curNumNodes] = next_node->children[root->curNumNodes];
                // update root to absorb the key lifted from the next node
                for(int i=root->curNumNodes;i>index;--i)
                  root->keys[i] = root->keys[i-1];
                root->keys[index] = next_node->keys[mid_index];
                for(int i=root->curNumNodes+1;i>index;--i)
                  root->children[i] = root->children[i-1];
                root->children[index] = new_left;
                root->children[index+1] = new_right;
                root->curNumNodes++;
                delete next_node;
                if(value > next_node->keys[mid_index])
                  insert_(new_right, value);
                else
                  insert_(new_left, value);
            }else{
              insert_(next_node, value);
            }
        }
    }

    void insert(int value) {
        if(!root) {
            root = new Node_t(MaxNumNodes);
            root->curNumNodes++;
            root->keys[0] = value;
            return;
        }
        else {
            if(root->curNumNodes == MaxNumNodes) {
                // split the root node 
                int mid_index = (root->curNumNodes)/2;
                NodePtr_t new_root = new Node_t(MaxNumNodes);
                NodePtr_t new_left = new Node_t(MaxNumNodes);
                NodePtr_t new_right = new Node_t(MaxNumNodes);
                new_root->isLeaf = false;
                new_root->keys[0] = root->keys[mid_index];
                new_root->curNumNodes = 1;
                new_root->children[0] = new_left;
                new_root->children[1] = new_right;
                new_left->curNumNodes = mid_index;
                for(auto i=0;i<mid_index;++i) {
                    new_left->keys[i] = root->keys[i];
                    new_left->children[i] = root->children[i];
                    if(new_left->children[i])
                        new_left->isLeaf = false;
                }
                new_left->children[mid_index] = root->children[mid_index];
                new_right->curNumNodes = MaxNumNodes-1-mid_index;
                for(auto i=mid_index+1, j=0;j<new_right->curNumNodes;++j, ++i) {
                    new_right->keys[j] = root->keys[i];
                    new_right->children[j] = root->children[i];
                    if(new_right->children[i])
                        new_right->isLeaf = false;
                }
                new_right->children[new_right->curNumNodes] = root->children[root->curNumNodes];
                delete this->root;
                this->root = new_root;
        }
        insert_(this->root, value);
    }
  }

  void rotate_right(NodePtr_t root, int root_index, NodePtr_t leftChild, NodePtr_t rightChild) {
        // root and leftChild is non-minimal, rightChild is minimal
        int lc_value = leftChild->keys[leftChild->curNumNodes-1];
        NodePtr_t lc_ptr = leftChild->children[leftChild->curNumNodes];
        leftChild->children[leftChild->curNumNodes] = NULL;
        leftChild->curNumNodes--;
        int root_value = root->keys[root_index];
        root->keys[root_index] = lc_value;
        for(int i=rightChild->curNumNodes;i>0;--i)
            rightChild->keys[i] = rightChild->keys[i-1];
        rightChild->keys[0] = root_value;
        for(int i=rightChild->curNumNodes+1;i>0;--i)
            rightChild->children[i] = rightChild->children[i-1];
        rightChild->children[0] = lc_ptr;
        rightChild->curNumNodes++;
  }

  void rotate_left(NodePtr_t root, int root_index, NodePtr_t leftChild, NodePtr_t rightChild) {
        // root and leftChild is non-minimal, rightChild is minimal
        int rc_value = rightChild->keys[0];
        NodePtr_t rc_ptr = leftChild->children[0];
        for(int i=0;i<rightChild->curNumNodes-1;++i)
            rightChild->keys[i] = rightChild->keys[i+1];
        for(int i=0;i<rightChild->curNumNodes;++i)
            rightChild->children[i] = rightChild->children[i+1];
        rightChild->curNumNodes--;
        int root_value = root->keys[root_index];
        root->keys[root_index] = rc_value;
        leftChild->keys[leftChild->curNumNodes] = root_value;
        leftChild->children[leftChild->curNumNodes+1] = rc_ptr;
        leftChild->curNumNodes++;
  }

  NodePtr_t merge_two_minimal(NodePtr_t root, int root_index, NodePtr_t leftChild, NodePtr_t rightChild) {
      assert(leftChild->isLeaf == rightChild->isLeaf);
      NodePtr_t new_node = new Node_t(MaxNumNodes);
      for(int i=0;i<leftChild->curNumNodes;++i)
        new_node->keys[i] = leftChild->keys[i];
      new_node->keys[leftChild->curNumNodes] = root->keys[root_index];
      for(int i=0;i<rightChild->curNumNodes;++i)
        new_node->keys[i+leftChild->curNumNodes+1] = rightChild->keys[i];
      for(int i=0;i<leftChild->curNumNodes+1;++i)
        new_node->children[i] = leftChild->children[i];
      for(int i=0;i<rightChild->curNumNodes+1;++i)
        new_node->children[i+leftChild->curNumNodes+1] = rightChild->children[i];
      new_node->curNumNodes = MaxNumNodes;
      new_node->isLeaf = leftChild->isLeaf;
      for(int i=root_index; i<root->curNumNodes-1;++i)
          root->keys[i] = root->keys[i+1];
      for(int i=root_index; i<root->curNumNodes;++i)
          root->children[i] = root->children[i+1];
      root->children[root_index] = new_node;
      root->curNumNodes--;
      return new_node;
  }

  int find_predecessor(NodePtr_t root) {
      // root is assured to be a non-minimal node
      if(root->isLeaf) {
          int predecessor_value = root->keys[root->curNumNodes-1];
          root->curNumNodes--;
          return predecessor_value;
      }else {
          NodePtr_t next_node = root->children[root->curNumNodes];
          if(!isminimal(next_node))
              return find_predecessor(next_node);
          // next_node is a minimal node
          NodePtr_t sibling_node = root->children[root->curNumNodes-1];
          if(!isminimal(sibling_node)) {
              // case 1: next_node's left sibiling is non-minimal
              rotate_right(root, root->curNumNodes-1, sibling_node, next_node);
              return find_predecessor(next_node);
          }else {
              // case 2: next_nodes's left sibling is also mininal, merge these two nodes into a non-minimal node
              NodePtr_t new_node = merge_two_minimal(root, root->curNumNodes-1, sibling_node, next_node);
              delete next_node;
              delete sibling_node;
              return find_predecessor(new_node);
          }
      } 
  }

  int find_successor(NodePtr_t root) {
      // root is assured to be a non-minimal node
      if(root->isLeaf) {
          int successor_value = root->keys[0];
          for(int i=0;i<root->curNumNodes-1;++i) 
              root->keys[i] = root->keys[i+1];
          root->curNumNodes--;
          return successor_value;
      }else {
          NodePtr_t next_node = root->children[0];
          if(!isminimal(next_node))
              return find_successor(next_node);
          // next_node is a minimal node
          NodePtr_t sibling_node = root->children[1];
          if(!isminimal(sibling_node)) {
              // case 1: next_node's right sibiling is non-minimal
              rotate_left(root, 0, next_node, sibling_node);
              return find_predecessor(next_node);
          }else {
              // case 2: next_nodes's right sibling is also mininal, merge these two nodes into a non-minimal node
              NodePtr_t new_node = merge_two_minimal(root, 0, next_node, sibling_node);
              delete next_node;
              delete sibling_node;
              return find_predecessor(new_node);
          }
      } 
  }
  
  std::pair<NodePtr_t, int> descend(NodePtr_t root, int value) {
        if(!root)
            return std::pair<NodePtr_t, int>(NULL, -1);
        int index = root->index_of_key(value);
        if(index < root->curNumNodes && root->keys[index] == value)
            return std::pair<NodePtr_t, int>(root, index);
        NodePtr_t next_node = root->children[index];
        if(!next_node)
            return std::pair<NodePtr_t, int>(NULL, -1);
        if(!isminimal(next_node))
            return descend(next_node, value);
        if(index>0 && !isminimal(root->children[index-1])) {
            rotate_right(root, index-1, root->children[index-1], next_node);
            return descend(next_node, value);
        }
        else if(index < root->curNumNodes && !isminimal(root->children[index+1])) {
            rotate_left(root, index, next_node, root->children[index+1]);
            return descend(next_node, value);
        }
        else if(index > 0 && isminimal(root->children[index-1])) {
            NodePtr_t node = merge_two_minimal(root, index-1, root->children[index-1], next_node);
            if(root == this->root && root->curNumNodes==0) { delete this->root; this->root = node;}
            return descend(node, value);
        }
        else {
            NodePtr_t node = merge_two_minimal(root, index, next_node, root->children[index+1]);
            if(root == this->root && root->curNumNodes==0) { delete this->root; this->root = node;}
            return descend(node, value);
        }
  }

  bool remove(int value) {
    std::pair<NodePtr_t, int> tmp = descend(root, value);
    NodePtr_t node = tmp.first;
    int index = tmp.second;
    while(1) {
        if(!node) { return false;}
        if(node->isLeaf) {
            for(int i=index;i<node->curNumNodes-1;++i)
                node->keys[i] = node->keys[i+1];
            node->curNumNodes--;
            return true;
        }else {
            if(!isminimal(node->children[index])) {
                int predesessor_value = find_predecessor(node->children[index]);
                node->keys[index] = predesessor_value;
                return true;
            }else if(!isminimal(node->children[index+1])) {
                int successor_value = find_successor(node->children[index+1]);
                node->keys[index] = successor_value;
                return true;
            }else {
                NodePtr_t merged_node = merge_two_minimal(node, index, node->children[index], node->children[index+1]);
                node = merged_node;
                index = MinNumNodes;
            }
        }
    }
  }

  void inorder_traverse(NodePtr_t node, bool newline) {
    if(!node)
        return;
    for(int i=0;i<node->curNumNodes;++i) {
        inorder_traverse(node->children[i], false);
        std::cout<<node->keys[i]<<" ";
    }
    inorder_traverse(node->children[node->curNumNodes], false);
    if(newline)
        std::cout<<"\n";
  }

  void print() {
      inorder_traverse(root, true);
  }
};


int main() {
    BTree btree(5);
    for(int i=1;i<=12;++i)
        btree.insert(i);
    btree.print();
    btree.remove(6);
    btree.print();
    return 0;
}
