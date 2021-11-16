#pragma once
#include<queue>
#include<vector>


using namespace std;

//������������ڵ�
template<class W>

struct HuffmanTreeNode {
	HuffmanTreeNode<W>* left;
	HuffmanTreeNode<W>* right;
	HuffmanTreeNode<W>* parent;
	W weight;//�ڵ��Ȩֵ

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
	//array[],Ȩֵ�ļ��ϣ�sizeԪ�ظ���
	void CreateHuffmanTree(const W array[], size_t size,const W& invalid) {
		//���ȼ�����Ĭ���ǰ�less��ʽ�Ƚϣ������������Ǵ�ѣ���������ҪС��

		std::priority_queue<Node*, vector<Node*>, my_Compare<W>> q;
		//��ʹ��������Ȩֵ����ֻ�и��ڵ�Ķ�����ɭ��
		for (size_t i = 0; i < size; i++) {
			if (array[i] != invalid) {
				q.push(new Node(array[i]));
			}
		}
		//ѭ��ֱ��ɭ����ʣ���һ������
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

