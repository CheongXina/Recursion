
#include <iostream>
#include <vector>
#include <stdexcept>





struct My2Vec {
	My2Vec(int x, int y) { 
		_x = x; 
		_y = y; 
	}
	int _x = 0;
	int _y = 0;
	int x() { return _x; }
	int y() { return _y; }

	My2Vec operator+(const My2Vec& other) {
		return My2Vec(this->x() + other._x, this->y() + other._y);
	}

	bool isNeg() {
		if (_x < 0 || _y < 0) return true;
		return false;
	}
};

struct Node {
	const char* id;
	Node* parent;
	std::vector<Node*> children;
	std::vector<My2Vec> steps_taken;
	Node() {
		parent = nullptr;
	}
	void Add_child(Node* child) {
		children.push_back(child);
	}

	void Add_step(My2Vec step) {
		steps_taken.push_back(step);
	}
};

enum class eOBJ : int {
	Node = 0,
	My2Vec = 1
};

void* Find_Winning_Recursive(std::vector<void*>& void_vecRef, eOBJ otype, Node* up_node) {

	if (up_node->id == "G") {
		std::cout << "G was found!" << std::endl;
		switch (otype) {
		case eOBJ::Node:
			return &up_node;
			break;
		case eOBJ::My2Vec:
			return &up_node->steps_taken[0];
			break;
		}
	}
	for (auto childNode : up_node->children) {
		auto v = Find_Winning_Recursive(void_vecRef, otype, childNode);
		if (v != nullptr) {
			void_vecRef.push_back(v);
			switch (otype) {
			case eOBJ::Node:
				return &up_node;
				break;
			case eOBJ::My2Vec:
				return &up_node->steps_taken[0];
				break;
			}
		}
	}
	return nullptr;
}

struct Maze {
	Maze(int size) {
		_grid.resize(size);
		_size = size;
		std::vector<char> row(size, '.');
		for (int i{ 0 }; i < size; i++) {
			_grid[i] = row;
		}
	}

	Maze(int size, My2Vec vec) {
		_grid.resize(size);
		_size = size;
		std::vector<char> row(size, '.');
		for (int i{ 0 }; i < size; i++) {
			_grid[i] = row;
		}
		_grid[vec.x()][vec.y()] = 'G';
	}

	char get_rc(My2Vec vec) {
		return char{ _grid[vec._x][vec._y] };
	}

	void set_rc(My2Vec vec, char val) {
		_grid[vec._x][vec._y] = val;
	}

	int size() { return _size; }

	bool outRange(My2Vec& vec) {
		if (vec.isNeg() || vec._x >= _size || vec._y >= _size) 
		{ 
			return true; 
		}
		return false;
	}
	bool isBlocked(char blockchar, My2Vec& vec) {
		if (outRange(vec) || get_rc(vec) == blockchar) 
		{ 
			return true; 
		}
		return false;
	}

	Maze operator+(const Maze& other) {

		auto newMaze = Maze(_size);
		auto ngrid = new std::vector<std::vector<char>>(_grid);
		newMaze._grid = *ngrid;
		delete ngrid;

		if (other._size != newMaze._size) { throw std::out_of_range("Grid of != sizes cannot be added."); }
		for (int row{ 0 }; row < newMaze._size; row++) {
			for (int col{ 0 }; col < newMaze._size; col++) {
				if (other._grid[row][col] != '.' && newMaze._grid[row][col] != 'G') {
					newMaze._grid[row][col] = other._grid[row][col];
				}
			}
		}
		return newMaze;
	}

	std::string str() {
		std::string strng{};
		for (int row{ 0 }; row < _size; row++) {
			strng += "\n";
			for (int col{ 0 }; col < _size; col++) {
				strng += _grid[row][col];
			}
		}
		return strng;
	}

	std::vector<std::vector<char>> _grid;
	int _size = 0;

};

struct Generic {
	Generic() 
	{ _type = "none"; _data = nullptr; }
	Generic(const char* name, void* data) 
	{ _type = name; _data = data; }
	const char* _type;
	void* _data;
};



std::vector<My2Vec> PossibleDirecs(My2Vec& start, Maze* maze, Maze* steps) {
	My2Vec Ds[4] = { start + My2Vec{0,1},start + My2Vec{-1,0} ,start + My2Vec{0,-1} ,start + My2Vec{1,0} };
	std::vector<My2Vec> ret_Ds{};
	for (auto D : Ds) {
		auto StartwD = start + D;
		if (steps->isBlocked('o', D) || maze->isBlocked('x', D)) { continue; }
		ret_Ds.push_back(D);	
	}
	if (ret_Ds.size() == 0) {
		//std::cout << "no direcs returned (dead end)" << std::endl;
	}
	return ret_Ds;
}

Generic recur(My2Vec start, Maze* maze, Maze* steps, int* counter_stackframes, Node* parent_node) {

	if (counter_stackframes) { 
		*counter_stackframes += 1; 
		if (*counter_stackframes >= 1000) 
		{ 
			throw std::out_of_range("You've hit 1000 stackframes/recursive calls. Exit."); 
		} 
	}

	auto curNode = new Node();
	curNode->Add_step(start);
	parent_node->Add_child(curNode);
	curNode->parent = parent_node;
	
	
	if (maze->get_rc(start) == 'G' || maze->get_rc(start) == 'g') { 
		steps->set_rc(start, 'g'); 
		curNode->id = "G";
		return Generic("MazeC",steps); 
	}
	steps->set_rc(start, 'o');

	auto possibleDirecs = PossibleDirecs(start, maze, steps);
	if (possibleDirecs.size() == 0) { 
		std::cout << "deadend on recur(current) sf#"<< *counter_stackframes <<".no possible direcs" << std::endl;
		return Generic("nullptr", nullptr);  //On a dead end should cut off ancestry.
	}
	
	for (auto d : possibleDirecs) {
		auto res = recur(d, maze, steps, counter_stackframes, curNode);
		if (res._type == "MazeC") { return res; }
		else {
			//One that returns a dead end has his ancestry cut off.
			std::cout << "deadend on recur(child) sf#" << *counter_stackframes << ". no possible direcs" << std::endl;
		}
	}

	return Generic("nullptr",nullptr);

};



int main(int argc, const char* argv[]) {

	auto maze = new Maze(11, My2Vec(5, 5));
	auto steps = new Maze(11);
	auto barriers = new Maze(11);
	
	maze->set_rc({ 1,1 }, 'x');
	maze->set_rc({ 2,1 }, 'x');
	maze->set_rc({ 3,1 }, 'x');
	maze->set_rc({ 4,1 }, 'x');
	maze->set_rc({ 5,1 }, 'x');

	maze->set_rc({ 1,2 }, 'x');
	maze->set_rc({ 1,3 }, 'x');
	maze->set_rc({ 1,4 }, 'x');
	maze->set_rc({ 1,5 }, 'x');
	maze->set_rc({ 0,5 }, 'x');

	std::cout << maze->str() << std::endl;

	int* recurN = new int(0);
	auto mainNode = new Node();
	mainNode->id = "MAIN";
	auto res = recur(My2Vec(0,0), maze, steps, recurN, mainNode);
	std::cout << "stackframes created " << *recurN << std::endl;
	if (res._type == "MazeC") {
		std::cout << "\n" + static_cast<Maze*>(res._data)->str() << std::endl;
	}
	std::vector<void*> generic_container;
	Find_Winning_Recursive(generic_container, eOBJ::Node, mainNode);

	delete recurN;
	delete maze;
	delete steps;
	recurN = nullptr;
	maze = nullptr;
	steps = nullptr;

	return 0;


	


};
