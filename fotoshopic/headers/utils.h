#pragma once


#include <functional>
#include <QString>


/*
* @brief Extending the std namespace with a std::hash overload for QSTring.
*/
namespace std {
  template<> struct hash<QString> {
	std::size_t operator()(const QString& s) const {
		return std::hash<std::string>()(s.toStdString());
	}
  };
}


/*
* @brief Alias for an often used type.
*/
template <typename T>
using qstring_map = std::unordered_map<QString, T>;


/*
* @brief Syntax sugar.
*/
template <typename T>
std::pair<QString, T> qstring_pair(std::string name, T &&value) {
	return {QString::fromStdString(std::move(name)), value};
}
