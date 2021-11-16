#pragma once
#include<queue>
#include<vector>


using namespace std;

//构造哈夫曼树节点
template<class W>

struct HuffmanTreeNode {
	HuffmanTreeNode<W>* left;
	HuffmanTreeNode<W>* right;
	HuffmanTreeNode<W>* parent;
	W weight;//节点的权值

	HuffmanTreeNode(const W& w = W())
		:left(nullptr)
		,right(nullptr)
		,parent(nullptr)
		, weight(w)
	{}
};

template<class W>
struct my_Compare {
	typedef HuffmanTreeNode<W> Node;
	bool operator()(const Node* left, const Node* right) {
		return left->weight > right->weight;
	}
};
template<class W>
class HuffmanTree {
	typedef HuffmanTreeNode<W> Node;
public:
	HuffmanTree()
		:root(nullptr)
	{}
	~HuffmanTree() {
		Destroy(root);
	}
	//array[],权值的集合，size元素个数
	void CreateHuffmanTree(const W array[], size_t size,const W& invalid) {
		//优先级队列默认是按less方式比较，创建出来的是大堆，而我们需要小堆

		std::priority_queue<Node*, vector<Node*>, my_Compare<W>> q;
		//先使用所给的权值创建只有根节点的二叉树森林
		for (size_t i = 0; i < size; i++) {
			if (array[i] != invalid) {
				q.push(new Node(array[i]));
			}
		}
		//循环直到森林中剩最后一棵树，
		while (q.size() > 1) {
			Node* left = q.top();
			q.pop();
			Node* right = q.top();
			q.pop();

			Node* parent = new Node(left->weight + right->weight);
			parent->left = left;
			parent->right = right;
			left->parent = parent;
			right->parent = parent;

			q.push(parent);
		}
		root = q.top();
	}
	Node* GetRoot() {
		return root;
	}
	void Destroy(Node*& proot) {
		if (proot) {
			Destroy(proot->left);
			Destroy(proot->right);
			delete proot;
			proot = nullptr;
		}
	}

private:
	Node* root;
};

