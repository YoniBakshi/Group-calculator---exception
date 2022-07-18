#include "SetCalculator.h"

#include "Union.h"
#include "Intersection.h"
#include "Difference.h"
#include "Product.h"
#include "Comp.h"
#include "Identity.h"

#include <format>
#include <istream>
#include <ostream>
#include <fstream>
#include <sstream>
#include <algorithm>

namespace rng = std::ranges;

SetCalculator::SetCalculator(std::istream& istr, std::ostream& ostr)
	: m_istr(istr), m_ostr(ostr),
	m_actions(createActions()), m_operations(createOperations()),
	m_currLimitList(3), m_requestedLimit(3),
	m_isLimitValid(false)
{
}

void SetCalculator::run()
{
	resizeMaxList();

	do
	{
		printOperations();

		try
		{
			runAction(readAction());
		}
		catch (std::exception& e)
		{
			handleException(e);
		}
	}

	while (m_running);
}

void SetCalculator::eval()
{
	if (auto index = readOperationIndex(); index)
	{
		const auto& operation = m_operations[*index];
		auto inputs = std::vector<Set>();

		for (auto i = 0; i < operation->inputCount(); ++i)
			inputs.push_back(Set(m_istr));

		operation->print(m_ostr, inputs);
		m_ostr << " = " << operation->compute(inputs) << '\n';
	}
}

void SetCalculator::del()
{
	if (auto i = readOperationIndex(); i > 2)
		m_operations.erase(m_operations.begin() + *i);
	else
		throw std::ios_base::failure("Cannot be deleted from the list! ");
}

void SetCalculator::read()
{
	auto filePath = std::string();
	std::getline(m_istr >> std::ws, filePath);
	auto file = std::ifstream(filePath); 	
	auto readAnything = false;
	std::streambuf* cinbuf = m_istr.rdbuf();				//Save old buf

	while (std::getline(file, m_inpStr))
	{
		readAnything = true;
		try
		{
			m_input.str(m_inpStr);
			m_istr.rdbuf(m_input.rdbuf());                 //Redirect std::cin to txt file!
			m_ostr << std::format("Read the current command: {}\n\n", m_inpStr);
			runAction(readAction());
		}
		catch (std::exception& e)
		{
			m_istr.clear();
			m_istr.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			m_ostr << "ERROR : " << e.what() << std::endl;
			m_istr.rdbuf(cinbuf);

			if (!askToContinue())
				file.seekg(std::ios::cur, std::ios::end);
		}

		if (file.eof() || !m_running)
			break;
	}

	m_ostr << "Exit file - " << filePath << "\n\n";
	file.close();
	m_istr.rdbuf(cinbuf);

	if (!readAnything)
		throw std::ios_base::failure("File not exists");
}

bool SetCalculator::askToContinue()
{
	m_ostr << "Do you wish to continue? ( y / n )\n";

	m_input.clear();
	m_input.str("");
	std::getline(m_istr, m_inpStr);
	m_input.str(m_inpStr);

	std::string toContinue;
	if (!(m_input >> toContinue) || (toContinue != "y" && toContinue != "n") ||
		!(m_input.eof() || (m_input >> std::ws).eof()))
		throw(std::invalid_argument("Answer invalid - expected one character ( y / n )\n"));

	return toContinue == "y";
}

void SetCalculator::help()
{
	m_ostr << "The available commands are:\n";

	for (const auto& action : m_actions)
		m_ostr << "* " << action.command << action.description << '\n';

	m_ostr << '\n';
}

void SetCalculator::exit()
{
	m_ostr << "Goodbye! :)\n";
	m_running = false;
}

void SetCalculator::printOperations() const
{
	m_ostr << "\nMax amount of option allowed : " + std::to_string(m_currLimitList) + "\nList of available set operations:\n";
	
	for (decltype(m_operations.size()) i = 0; i < m_operations.size(); ++i)
	{
		m_ostr << i << ".\t";
		auto gen = NameGenerator();
		m_operations[i]->print(m_ostr, gen);
		m_ostr << '\n';
	}

	m_ostr << "\nEnter command('help' for the list of available commands) : ";
}

void SetCalculator::resizeMaxList()
{
	m_isLimitValid = false;

	while (!m_isLimitValid)
	{
		try
		{
			readLimit();
		}
		catch (std::exception& e)
		{
			handleException(e);
		}
	}
}

void SetCalculator::readLimit()
{
	m_ostr << "\nPlease enter limit of options for operation's list (3 - 100): ";

	if (!(m_istr >> m_requestedLimit))
		throw(std::ios_base::failure("Bad input. Quantity can be only an integer. "));

	if (!(m_requestedLimit >= 3) || !(m_requestedLimit <= 100))
		throw std::out_of_range("Input is out of range - range can be only between 3-100. ");

	isResizeValid();

	m_currLimitList = m_requestedLimit;
	m_isLimitValid = true;
}

bool SetCalculator::addOperationValid(Action action) const
{
	switch (action)
	{
	case Action::Union:
	case Action::Intersection:
	case Action::Difference:
	case Action::Product:
	case Action::Comp:
		return (m_operations.size() < m_currLimitList);
	}

	return true;
}

void SetCalculator::handleException(std::exception& e)
{
	m_ostr << "ERROR : " << e.what() << std::endl;
	m_istr.clear();
	m_istr.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void SetCalculator::resize()
{
	try
	{
		resizeMaxList();
	}
	catch (std::exception& e)
	{
		handleException(e);
		resizeMaxList();
	}
}

std::optional<int> SetCalculator::readOperationIndex() const
{
	auto i = 0;

	if (!(m_istr >> i))
		throw(std::ios_base::failure("Bad input of operation index! "));

	if (i >= m_operations.size() || i < 0)
		throw(std::out_of_range("Operation #" + std::to_string(i) + " doesn't exist.\n"));   //return {};

	return i;
}

SetCalculator::Action SetCalculator::readAction() const
{
	auto action = std::string();
	m_istr >> action;
	const auto i = std::ranges::find(m_actions, action, &ActionDetails::command);
	if (i != m_actions.end())
		return i->action;

	return Action::Invalid;					//Invalid_argument
}

void SetCalculator::runAction(Action action)
{
	if (!addOperationValid(action))
		throw(std::length_error("Cannot add new option - Not enough space available in list."));

	switch (action)
	{
	default:
		throw(std::ios_base::failure("Unknown enum entry used!\n"));
		break;

	case Action::Invalid:
		throw (std::invalid_argument("The entered command doesn't exist - incomplete.\n"));
		break;

	case Action::Eval:         eval();                     break;
	case Action::Union:        binaryFunc<Union>();        break;
	case Action::Intersection: binaryFunc<Intersection>(); break;
	case Action::Difference:   binaryFunc<Difference>();   break;
	case Action::Product:      binaryFunc<Product>();      break;
	case Action::Comp:         binaryFunc<Comp>();         break;
	case Action::Resize:       resize();				   break;
	case Action::Read:         read();                     break;
	case Action::Del:          del();                      break;
	case Action::Help:         help();                     break;
	case Action::Exit:         exit();                     break;
	}
}

void SetCalculator::isResizeValid()
{
	if (m_operations.size() > m_requestedLimit)
	{
		m_ostr << "Requested size is lower than current existing operations in list. " \
			"\nAre you sure you want to change the limit? ( y / n ) \n";

		if (m_istr >> m_inpStr)
		{
			if (m_inpStr == "n")
				m_requestedLimit = m_currLimitList;
			else if (m_inpStr == "y")
				m_operations.erase(m_operations.begin() + m_requestedLimit, m_operations.end());
			else
				throw(std::invalid_argument("Answer y = yes or n = no ."));
		}
	}
}

SetCalculator::ActionMap SetCalculator::createActions()
{
	return ActionMap
	{
		{
			"eval",
			"(uate) num ... - compute the result of function #num on the "
			"following set(s); each set is prefixed with the count of numbers to"
			" read",
			Action::Eval
		},
		{
			"uni",
			"(on) num1 num2 - Creates an operation that is the union of "
			"operation #num1 and operation #num2",
			Action::Union
		},
		{
			"inter",
			"(section) num1 num2 - Creates an operation that is the "
			"intersection of operation #num1 and operation #num2",
			Action::Intersection
		},
		{
			"diff",
			"(erence) num1 num2 - Creates an operation that is the "
			"difference of operation #num1 and operation #num2",
			Action::Difference
		},
		{
			"prod",
			"(uct) num1 num2 - Creates an operation that returns the product of"
			" the items from the results of operation #num1 and operation #num2",
			Action::Product
		},
		{
			"comp",
			"(osite) num1 num2 - creates an operation that is the composition "
			"of operation #num1 and operation #num2",
			Action::Comp
		},
		{
			"resize",
			"Choose to enter a new limit size for list of operations.",
			Action::Resize
		},
		{
			"read",
			"- Read commands from text file. \n(for operation 'eval' write the groups in the same row).",
			Action::Read
		},
		{
			"del",
			"(ete) num - delete operation #num from the operation list",
			Action::Del
		},
		{
			"help",
			" - print this command list",
			Action::Help
		},
		{
			"exit",
			" - exit the program",
			Action::Exit
		}
	};
}

SetCalculator::OperationList SetCalculator::createOperations()
{
	return OperationList
	{
		std::make_shared<Union>(std::make_shared<Identity>(), std::make_shared<Identity>()),
		std::make_shared<Intersection>(std::make_shared<Identity>(), std::make_shared<Identity>()),
		std::make_shared<Difference>(std::make_shared<Identity>(), std::make_shared<Identity>())
	};
}
