//
// Created by egordm on 9-11-2017.
//

#ifndef MYBOT_UTILS_H
#define MYBOT_UTILS_H


#include "../hlt/types.hpp"
#include <vector>
#include <functional>
#include <algorithm>
#include "../hlt/vector.hpp"
#include "sorting.h"

using namespace std::placeholders;

namespace bot { namespace utils {
	template<class T>
	hlt::nullable<T> closest_object(std::vector<T> objects, hlt::Vector pos) {
		if (objects.size() == 0) return std::make_pair(T(), false);
		return std::make_pair(*std::min_element(objects.begin(), objects.end(),
		                                        std::bind(sorting::sort_by_corrected_distance, pos, _1, _2)), true);
	}

	template<class T>
	void erase(std::vector<T> &v, T item) {
		for (int i = static_cast<int>(v.size() - 1); i >= 0; --i) {
			if (v[i]->entity_id != item->entity_id) continue;
			v.erase(v.begin() + i);
			return;
		}
	}

	template<typename ContainerT, typename PredicateT>
	void erase_if(ContainerT &items, const PredicateT &predicate) {
		for (auto it = items.begin(); it != items.end();) {
			if (predicate(*it)) it = items.erase(it);
			else ++it;
		}
	};

	template<typename T, typename P>
	void erase_if_not(T &items, const P &predicate) {
		for (auto it = items.begin(); it != items.end();) {
			if (!predicate(*it)) it = items.erase(it);
			else ++it;
		}
	};

	template<typename T, typename P>
	unsigned int count(T &items, const P &predicate) {
		unsigned int ret = 0;
		for (auto it = items.begin(); it != items.end();) if (predicate(*it)) ret++;
		return ret;
	};

	template<typename T>
	static void remove_ignored(std::vector<std::shared_ptr<T>> &objects, const std::vector<hlt::EntityIdentifier> &ignore) {
		std::remove_if(objects.begin(), objects.end(), [&ignore](const std::shared_ptr<T> &el) {
			return std::find(ignore.begin(), ignore.end(), el->identify()) != ignore.end();
		});
	}
}}


#endif //MYBOT_UTILS_H
