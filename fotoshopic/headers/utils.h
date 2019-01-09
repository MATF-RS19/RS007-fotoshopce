#pragma once

#include <functional>

#include <QString>

namespace std {
  template<> struct hash<QString> {
	std::size_t operator()(const QString& s) const {
		return std::hash<std::string>()(s.toStdString());
	}
  };
}

template <typename T>
using qstring_map = std::unordered_map<QString, T>;
