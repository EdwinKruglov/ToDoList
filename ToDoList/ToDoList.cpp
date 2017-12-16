#include "ToDoList.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <sstream>
#include <limits>
#include <fstream>
#include <sys/stat.h>

#undef max // to remove max definition in <windows.h> so it doesn't override max() in <limits>

ToDoList::ToDoList(const std::vector<ListItem>& list)
	: toDoList(list)
{
}

void ToDoList::menu()
{
	// A menu for the user.

	enum {ADD = 1, UPDATE, REMOVE, SAVE, OPEN, QUIT};
	int option = 0;

	std::cout << "Welcome to the To Do List!\n" << std::endl;
	std::cin.unsetf(std::ios::skipws);

	do
	{
		showMessages();

		if (!toDoList.empty())
			showItems();

		std::cout << "1. Add item\n";
		std::cout << "2. Update item\n";
		std::cout << "3. Remove item\n";
		std::cout << "4. Save list\n";
		std::cout << "5. Open list\n";
		std::cout << "6. Quit\n";
		std::cout << "\nSelect menu option: ";
		std::cin >> option;
				
		if (std::cin.good())
		{
			std::cin.ignore();

			switch (option)
			{
				case ADD:
					addMenu();
					break;
				case UPDATE:
					if (!checkEmpty())
						updateMenu();
					break;
				case REMOVE:
					if (!checkEmpty())
						removeMenu();
					break;
				case SAVE:
					saveMenu();
					break;
				case OPEN:
					openMenu();
					break;
				case QUIT:
					break;
				default:
					clearConsole();
					std::cout << "Not a menu choice! Try again!\n" << std::endl;
					break;
			}
		}
		else
		{
			clearConsole();
			std::cin.clear();
			std::cout << "Invalid input! Try again!\n" << std::endl;
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		}		
		
	} while (option != QUIT);
}

void ToDoList::addMenu()
{
	// A simple menu that allows the user to add items to the vector.

	// If the vector is empty, the first item they will add will have the num value 1. If the vector already has some items, then the next one's
	// num value should be larger by 1 than the last entered item's num value.

	clearConsole();

	if (!toDoList.empty())
	{
		auto const &i = toDoList.back();
		indexNum = i.num + 1;
	}
	else
	{
		indexNum = 1;
	}

	std::string temp = "";

	std::cout << "Adding items... (type \"x\" to go back)\n\n";
	while (temp != "x")
	{
		std::cout << "> ";
		std::getline(std::cin, temp);
		if (temp != "x")
		{
			item.num = indexNum++;
			item.item = temp;
			addItem(item);
		}
	}

	clearConsole();
}

void ToDoList::updateMenu()
{
	// A user-friendly menu to update vector elements.

	unsigned int indexNum;
	std::string tempItem;

	clearConsole();
	showItems();

	std::cout << "Item # to update (type \"0\" to go back): ";
	
	do
	{
		if (!std::cin.good())
		{
			std::cin.clear();
			std::cout << "Invalid input! Try again!" << std::endl;
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		}

		std::cin >> indexNum;

	} while (!std::cin.good());
	
	if (indexNum != 0)
	{
		std::cin.ignore();
		std::cout << "Update to: ";
		std::getline(std::cin, tempItem);
		updateItem(indexNum, tempItem);
	}
	else
	{
		std::cin.ignore();
	}

	clearConsole();
}

void ToDoList::removeMenu()
{
	// A simple menu that prompts the user to either remove a vector element by entering its position or value.

	enum {NUM = 1, ITEM, ALL, BACK};
	std::string temp = "";
	indexNum = 0;
	int option = 0;
	char confirmation = '\0';

	clearConsole();
	showItems();

	std::cout << "Remove an item by:\n";
	std::cout << "1. Number\n";
	std::cout << "2. Item\n";
	std::cout << "3. Remove all\n";
	std::cout << "4. Go back\n\n";
	std::cout << "> ";

	do
	{
		if (!std::cin.good())
		{
			std::cin.clear();
			std::cout << "Invalid input! Try again!" << std::endl;
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		}

		std::cin >> option;
	} while (!std::cin.good());
	
	std::cin.ignore();

	switch (option)
	{
		case NUM:
			std::cout << "\nItem number: ";
			std::cin >> indexNum;
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			removeItem(indexNum);
			break;
		case ITEM:
			std::cout << "\nItem: ";
			std::getline(std::cin, temp);
			removeItem(temp);
			break;
		case ALL:
			while (confirmation != 'y' && confirmation != 'n')
			{
				std::cout << "Are you sure you want to remove all the items from your list (y/n)? ";
				std::cin >> confirmation;

				if (confirmation == 'y')
					toDoList.clear();

				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			}
			break;
		case BACK:
			break;
		default:
			messages.push("Invalid remove menu choice! Try again!\n");
			break;
	}	

	clearConsole();
}

void ToDoList::saveMenu()
{
	std::string filePath;
	struct stat file;

	clearConsole();
	std::cout << "Please save your list as either \".txt\" or \".list\", which can be only opened using this program." << std::endl;
	std::cout << "Enter file path (\"x\" to go back): ";
	std::cin >> filePath;
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

	if (filePath != "x")
	{
		// Check file extension
		std::string fileExt = filePath.substr(filePath.find_last_of(".") + 1);
		if (fileExt == "txt" || fileExt == "list")
		{
			// Check if file exists
			if (stat(filePath.c_str(), &file) == 1)
			{
				char choice = '\0';
				std::cout << "\nFile already exists! Do you want to overwrite (otherwise the file will be appended to) (y/n)? ";
				while (choice != 'y' && choice != 'n')
				{
					std::cin >> choice;
					std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				}
				if (choice == 'y')
					save(filePath, fileExt, true);
				else
					save(filePath, fileExt, false);
			}
			else
			{
				if (fileExt == "txt")
				{
					save(filePath, "txt", false);
				}
				else if (fileExt == "list")
				{
					save(filePath, "list", false);
				}
				else
				{
					messages.push("Invalid file extension when saving! Try again!\n");
				}
			}
		}
		else
		{
			messages.push("Invalid file type!\n");
		}
	}

	clearConsole();
}

void ToDoList::openMenu()
{
	std::string filePath;

	clearConsole();
	std::cout << "You can either open \".txt\" or \".list\" files." << std::endl;
	std::cout << "File path (\"x\" to go back): ";
	std::cin >> filePath;
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

	if (filePath != "x")
	{
		// Check file extension
		std::string fileExt = filePath.substr(filePath.find_last_of(".") + 1);
		if (fileExt == "txt" || fileExt == "list")
		{
			open(filePath, fileExt);
		}
		else
		{
			messages.push("Invalid file type when opening! Try again!\n");
		}
	}

	clearConsole();
}

void ToDoList::addItem(const ListItem& addListItem)
{
	toDoList.push_back(addListItem);
}

void ToDoList::updateItem(unsigned int elementNum, const std::string& newItem)
{
	// Change specified index's "item" value.

	if (checkIndex(elementNum))
	{
		toDoList.at(elementNum - 1).item = newItem;
	}
}

void ToDoList::removeItem(unsigned int elementNum)
{
	// Remove the specified element by providing its position in the vector.

	if (checkIndex(elementNum))
	{
		// it points to the passed in element (-1 because the user wouldn't know that vectors and arrays are 0-based).
		// Use erase() to simply remove that element.
		std::vector<ListItem>::iterator it = toDoList.begin();
		std::advance(it, elementNum - 1);
		toDoList.erase(it);

		// Since it is invalid after being erased, we need to point it to the same position again.
		// This time it will point to the element that followed the removed element, so that makeConsecutive() can decrement its
		// and all the following elements' num values to make the list consecutive.
		it = toDoList.begin() + (elementNum - 1);
		makeConsecutive(it);
	}
}

void ToDoList::removeItem(const std::string& strItem)
{
	// Remove the specified element by finding its value if the vector is not empty.

	// Get the current position of the element that has the string value passed in.
	auto pos = std::find(toDoList.begin(), toDoList.end(), strItem) - toDoList.begin();
	if ((unsigned int)pos >= toDoList.size()) // Cast pos to unsigned int because the size() function returns that; prevents the compiler warning.
	{
		// If the position is outside the vector bounds (no element with the passed in string value was found).
		messages.push("Item \"" + strItem + "\" not found!\n");
	}
	else
	{
		// A lambda function that returns a boolean value if the passed in string is found
		// I used this with remove_if() before deciding to use the find() function above.
		// auto pred = [strItem](const ListItem& j){return j.item == strItem; };

		// I apply the erase-remove idiom here. itRemove returns an iterator pointing to the element to remove after the vector is rearranged
		// and the elements that don't meet the criteria (strItem) are moved to the front. The iterator points to the element past the last
		// which doesn't meet the criteria.
		std::vector<ListItem>::iterator itRemove = std::remove(toDoList.begin(), toDoList.end(), strItem);
		toDoList.erase(itRemove, toDoList.end());

		// index points to the element that previously followed the element that got removed, so that makeConsecutive() can iterate over it
		// and all the following elements to decrement their num value so as to make the list item numbers consecutive.
		std::vector<ListItem>::iterator index = toDoList.begin();
		std::advance(index, pos);
		makeConsecutive(index);
	}
}

void ToDoList::showItems()
{
	// Prints all the ListItem objects in the vector if the vector is not empty.

	if (!toDoList.empty())
	{
		std::cout << "**************TO DO LIST**************\n";
		for (auto const &i : toDoList)
			std::cout << i << std::endl;
		std::cout << "**************************************\n\n";
	}
	else
	{
		messages.push("No items in the list!\n");
	}
}

void ToDoList::clearConsole() const
{
	COORD topLeft = { 0, 0 };
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO screen;
	DWORD written;

	GetConsoleScreenBufferInfo(console, &screen);
	FillConsoleOutputCharacterA(
		console, ' ', screen.dwSize.X * screen.dwSize.Y, topLeft, &written
		);
	FillConsoleOutputAttribute(
		console, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE,
		screen.dwSize.X * screen.dwSize.Y, topLeft, &written
		);
	SetConsoleCursorPosition(console, topLeft);
}

void ToDoList::makeConsecutive(std::vector<ListItem>::iterator & it)
{
	// Decrement the numbers of each list item that comes after the removed one, so that that numbers of the new list are consecutive.

	for (; it != toDoList.end(); it++)
		it->num--;
}

bool ToDoList::checkEmpty()
{
	// Check if the vector is empty and notify the user. Used to validate remove and update functions.

	if (toDoList.empty())
	{
		clearConsole();
		messages.push("Cannot update or remove items while the list is empty!\n");
		return true;
	}
	else
	{
		return false;
	}
}

bool ToDoList::checkIndex(unsigned int elementNum)
{
	// Check if supplied index is not out of vector bounds. Used for validation in remove and update functions.

	if (elementNum > toDoList.size())
	{
		std::ostringstream os;
		os << "Item " << elementNum << " does not exist! Try again!\n";
		messages.push(os.str());
		return false;
	}
	else
	{
		return true;
	}
}

void ToDoList::showMessages()
{
	while (!messages.empty())
	{
		std::cout << messages.front() << std::endl;
		messages.pop();
	}
}

void ToDoList::operator()(const ListItem& item)
{
	// A parentheses overload for a ListItem object.

	toDoList.push_back(item);
}

void ToDoList::save(const std::string& path, const std::string& ext, bool append)
{
	std::ofstream fileOut;

	if (ext == "txt")
	{
		if (append)
			fileOut.open(path, std::ios::app);
		else
			fileOut.open(path, std::ios::trunc);

		if (fileOut)
		{
			for (auto const &item : toDoList)
				fileOut << item.num << ". " << item.item << "\n";

			fileOut.close();
			messages.push("File saved successfully!\n");
		}
		else
		{
			messages.push("Could not save file! Is the path valid? Do you need administrator permissions?\n");
		}
	}
	else if (ext == "list")
	{
		if (append)
			fileOut.open(path, std::ios::app | std::ios::binary);
		else
			fileOut.open(path, std::ios::trunc | std::ios::binary);

		if (fileOut)
		{
			fileOut.write(reinterpret_cast<char*>(this), sizeof(*this));
			fileOut.close();
			messages.push("File saved successfully!\n");
		}
		else
		{
			messages.push("Could not save file! Is the path valid? Do you need administrator permissions?\n");
		}
	}
	else
	{
		messages.push("Invalid file type!\n");
	}
}

void ToDoList::open(const std::string& path, const std::string& ext)
{
	std::ifstream fileOut;
	if (ext == "txt")
	{
		const int MAX_LINE = 255;
		char line[MAX_LINE];
		fileOut.open(path);

		if (fileOut)
		{
			toDoList.clear();
			unsigned int num = 1;
			while (!fileOut.eof())
			{
				fileOut.seekg(3, std::ios::beg);
				fileOut.getline(line, MAX_LINE);
				ListItem item(num, line, "none");
				toDoList.push_back(item);
				num++;
			}

			fileOut.close();
		}
		else
		{
			messages.push("Could not open file! Does it exist? Do you need administrator permissions?\n");
		}


	}
	else if (ext == "list")
	{
		fileOut.open(path, std::ios::binary);
	}
	else
	{
		messages.push("Invalid file type!\n");
	}
}