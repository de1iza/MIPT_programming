template <typename T>
class List {
private:
	struct Node {
		T val;
		Node* next;
		Node() : next(nullptr) {}
		Node(const T& val) : val(val), next(nullptr) {}
	};

	Node* head;
	size_t size;

	bool RemoveHead() {
		if (head) {
			Node* new_head = head->next;
			delete head;
			head = new_head;
			--size;
			return true;
		}

		return false;
	}

	Node* Find(const T& val) const {
		Node* cur = head;
		while (cur != nullptr) {
			if (cur->val == val)
				break;
			cur = cur->next;
		}
		return cur;
	}

public:
	List(): head(nullptr), size(0) {}

	List(const T& val): size(1) {
		Node* node = new Node(val);
		head = node;
	}

	~List() {
		while (head) {
			RemoveHead();
		}
	}

	Node* Insert(const T& val) {
		Node* node = new Node(val);

		node->next = head;
		head = node;

		++size;
		return node;
	}

	bool Contains(const T& val) const {
		return Find(val) != nullptr;
	}

	bool Remove(const T& val) {
		Node* cur = head;
		Node* prev = nullptr;

		while (cur != nullptr) {
			if (cur->val == val)
				break;
			prev = cur;
			cur = cur->next;
		}

		if (cur == nullptr)
			return false;

		if (cur == head)
			return RemoveHead();

		prev->next = cur->next;
		--size;
		
		delete cur;

		return true;
	}

	void Show() const {
		Node* cur = head;
		while (cur != nullptr) {
			std::cout << cur->val << " ";
			cur = cur->next;
		}
		std::cout << "\n";
	}

	size_t GetSize() const {
		return size;
	}



};

