#include <iostream>
#include <string>
#include <vector>

/**
 * Класс Rplsmt предназначен для хранения параметров текста, вставляемого в файл.
 */
class Rplsmt {

public:
	Rplsmt ();
	Rplsmt (long int pos, std::string insText);

	/**
	 * Поле pos хранит позицию в файле, в которую должен быть вставлен текст.
	 */
	long int pos;

	/**
	 * Поле insText предназначено для хранения текста для вставки.
	 */
	std::string insText;

	/**
	 * Оператор rplsmtGreater используется для ранжирования объектов по возрастанию.
	 */
	static struct {
	  bool operator()(const Rplsmt firstElem, const Rplsmt secondElem)
	  {
		if (firstElem.pos < secondElem.pos)
		{
			return true;
		}
		else if (firstElem.pos > secondElem.pos)
		{
			return false;
		}
		else
		{
			if ((firstElem.insText == "B") && ((secondElem.insText == "E") || (secondElem.insText == "R")))
			{
				return true;
			}
			else if (firstElem.insText == secondElem.insText)
			{
				return true;
			}
			else if (((firstElem.insText == "E") || (firstElem.insText == "R")) && (secondElem.insText == "B"))
			{
				return false;
			}
			else
			{
				std::cout << "Error while compare replacements\n\r";
				exit (-1);
			}
		}
	  }
	} rplsmtSmallerOrEqual;

	friend std::ostream& operator << (std::ostream& out, Rplsmt& R)
	{
		return out << std::to_string(R.pos) << " " << R.insText;
	}
};
