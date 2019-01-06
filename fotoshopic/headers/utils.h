#ifndef UTILS_H
#define UTILS_H

#include <functional>

#include <QString>

namespace std {
  template<> struct hash<QString> {
	std::size_t operator()(const QString& s) const {
		return std::hash<std::string>()(s.toStdString());
	}
  };
}

#endif // UTILS_H
