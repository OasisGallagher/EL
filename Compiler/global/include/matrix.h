#pragma once
#include <map>
#include <functional>

#include "debug.h"

template <class key_type>
struct matrix_comparer : public std::binary_function <key_type, key_type, bool> {
	bool operator () (const key_type& lhs, const key_type& rhs) const {
		if (lhs.first == rhs.first) {
			return lhs.second < rhs.second;
		}

		return lhs.first < rhs.first;
	}
};

template <class Kty1, class Kty2, class Ty, class Comp = matrix_comparer<std::pair<Kty1, Kty2>>>
class matrix {
public:
	typedef Kty1 first_key_type;
	typedef Kty2 second_key_type;
	typedef std::pair<first_key_type, second_key_type> key_type;
	typedef Ty value_type;
	typedef Comp comparer_type;

	typedef std::map<key_type, value_type, comparer_type> container_type;
	typedef typename container_type::iterator iterator;
	typedef typename container_type::const_iterator const_iterator;

	typedef std::pair<iterator, bool> ib_pair;

public:
	matrix& operator = (const matrix& other) {
		cont_ = other.cont_;
		return *this;
	}

public:
	value_type& at(const first_key_type& k1, const second_key_type& k2) {
		iterator ite = find(k1, k2);
		Assert(ite != end(), "key does not exist.");
		return ite->second;
	}

	bool get(const first_key_type& k1, const second_key_type& k2, value_type& answer) {
		iterator ite = find(k1, k2);
		if (ite == end()) {
			return false;
		}

		answer = ite->second;
		return true;
	}

	bool get(const first_key_type& k1, const second_key_type& k2, value_type& answer) const {
		const_iterator ite = find(k1, k2);
		if (ite == end()) {
			return false;
		}

		answer = ite->second;
		return true;
	}

	void insert(const_iterator first, const_iterator last) {
		cont_.insert(first, last);
	}

	ib_pair insert(const first_key_type& k1, const second_key_type& k2, const value_type& val) {
		return cont_.insert(std::make_pair(std::make_pair(k1, k2), val));
	}

	iterator find(const first_key_type& k1, const second_key_type& k2) {
		return cont_.find(std::make_pair(k1, k2));
	}

	const_iterator find(const first_key_type& k1, const second_key_type& k2) const {
		return cont_.find(std::make_pair(k1, k2));
	}

	const_iterator begin() const {
		return cont_.begin();
	}

	const_iterator end() const {
		return cont_.end();
	}

	void clear() {
		cont_.clear();
	}

	int size() const {
		return cont_.size();
	}

protected:
	container_type cont_;
};
