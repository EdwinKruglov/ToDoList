#ifndef TODOLIST_H
#define TODOLIST_H

#include <string>
#include <vector>
#include <queue>

struct ListItem
{
	int num;
	std::string item;
	std::vector<std::string> categories;

	ListItem()
		: num(0), item("unknown")
	{}

	template<typename... Categories>
	ListItem(int number, const std::string& toDo, Categories... cats)
		: num(number), item(toDo)
	{
		for (auto cat : { cats... })
		{
			categories.push_back(cat);
		}
	}

	inline friend std::ostream& operator<<(std::ostream& os, const ListItem& item);
	inline friend bool operator==(const ListItem& item, const std::string& str);
};

// The operator overloads are inline, but I'm not sure why. It seems to prevent linker errors.

std::ostream& operator<<(std::ostream& os, const ListItem& item)
{
	// An ostream overload to print ListItem objects easily.
	os << item.num << ". " << item.item;
	return os;
}

bool operator==(const ListItem& item, const std::string& str)
{
	// This operator overload is needed for std::find() to work when comparing ListItem with strings. Used in the removeItem() function.
	return (item.item == str);
}

class ToDoList
{
	private:
		std::vector<ListItem> toDoList;
		ListItem item;
		unsigned int indexNum;
		std::queue<std::string> messages;
		void addMenu();
		void updateMenu();
		void removeMenu();
		void saveMenu();
		void openMenu();
		void makeConsecutive(std::vector<ListItem>::iterator & it);
	public:
		ToDoList() = default;
		ToDoList(const std::vector<ListItem>& list);
		void menu();
		void addItem(const ListItem& addListItem);
		template<typename... Categories>
		void addItem(unsigned int elementNum, const std::string& item, Categories&&... cats)
		{
			ListItem temp(elementNum, item, std::forward<Categories>(cats)...);
			toDoList.push_back(temp);
		}
		void updateItem(unsigned int elementNum, const std::string& newItem);
		void removeItem(unsigned int elementNum);
		void removeItem(const std::string& strItem);
		void showItems();
		void clearConsole() const;
		bool checkEmpty();
		bool checkIndex(unsigned int elementNum);
		void showMessages();
		void save(const std::string& path, const std::string& ext, bool append);
		void open(const std::string& path, const std::string& ext);
		inline void operator()(const ListItem& item);
		~ToDoList() = default;
};

#endif // !TODOLIST_H